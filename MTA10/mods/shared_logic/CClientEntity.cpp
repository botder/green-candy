/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientEntity.cpp
*  PURPOSE:     Base entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Chris McArthur <>
*               Jax <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using std::list;

extern CClientGame* g_pClientGame;

static int entity_setPosition( lua_State *L )
{
    CVector pos;
   
    CScriptArgReader argStream( L );
    argStream.ReadVector( pos );

    if ( argStream.HasErrors() )
        throw lua_exception( L, LUA_ERRRUN, SString( "Bad argument @ 'setPosition' [%s]", *argStream.GetErrorMessage() ) );

    ((CClientEntity*)lua_getmethodtrans( L ))->SetPosition( pos );
    return 0;
}

static int entity_getPosition( lua_State *L )
{
    CVector pos;
    ((CClientEntity*)lua_getmethodtrans( L ))->GetPosition( pos );

    lua_pushnumber( L, pos[0] );
    lua_pushnumber( L, pos[1] );
    lua_pushnumber( L, pos[2] );
    return 3;
}

static int entity_setMatrix( lua_State *L )
{
    RwMatrix *mat = lua_readclass <RwMatrix> ( L, 1, LUACLASS_MATRIX );

    if ( !mat )
        throw lua_exception( L, LUA_ERRRUN, "wrong type at 'setMatrix'; expected matrix" );

    ((CClientEntity*)lua_getmethodtrans( L ))->SetMatrix( *mat );
    return 0;
}

static int entity_getMatrix( lua_State *L )
{
    RwMatrix mat;
    
    if ( !((CClientEntity*)lua_getmethodtrans( L ))->GetMatrix( mat ) )
    {
        lua_pushboolean( L, false );
        return 1;
    }

    lua_creatematrix( L, mat );
    return 1;
}

static int entity_isValidChild( lua_State *L )
{
    lua_pushboolean( L, lua_type( L, 1 ) == LUA_TCLASS && lua_refclass( L, 1 )->IsTransmit( LUACLASS_ELEMENT ) );
    return 1;
}

int CClientEntity::entitychildAPI_notifyDestroy( lua_State *L )
{
    // Remove the child from the parent's children list
    CClientEntity *child = (CClientEntity*)lua_touserdata( L, lua_upvalueindex( 1 ) );
    CClientEntity *parent = child->m_pParent;

    parent->m_Children.remove( child );
    child->m_pParent = NULL;

    // Moving out of FromRoot?
    if ( IsFromRoot( parent ) )
        RemoveEntityFromRoot( child->m_uiTypeHash, child );

    return 0;
}

const static luaL_Reg entitychildAPI_interface[] =
{
    { "notifyDestroy", CClientEntity::entitychildAPI_notifyDestroy },
    { NULL, NULL }
};

static int entitychildAPI_constructor( lua_State *L )
{
    lua_pushvalue( L, lua_upvalueindex( 1 ) );
    lua_refclass( L, LUA_ENVIRONINDEX )->RegisterInterface( L, entitychildAPI_interface, 1 );
    return 0;
}

int CClientEntity::entity_setChild( lua_State *L )
{
    ILuaClass *childC = lua_refclass( L, 1 );
    CClientEntity *child;
    bool isEntity = childC->GetTransmit( LUACLASS_ENTITY, (void*&)child );

    CClientEntity *entity = (CClientEntity*)lua_touserdata( L, lua_upvalueindex( 1 ) );
    lua_getfield( L, LUA_ENVIRONINDEX, "super" );
    lua_pushvalue( L, 1 );
    lua_call( L, 1, 1 );

    if ( isEntity )
    {
        // Set the new parent
        child->m_pParent = entity;

        // Extend the childAPI
        lua_pushlightuserdata( L, child );
        lua_pushcclosure( L, entitychildAPI_constructor, 1 );
        luaJ_extend( L, 2, 0 );

        // Add us to the new parent's children list
        entity->m_Children.push_back( child );

        // Moving into FromRoot?
        if ( IsFromRoot( entity ) )
            AddEntityFromRoot( child->m_uiTypeHash, child );
    }

    return 1;
}

static const luaL_Reg entity_interface_trans[] =
{
    { "setPosition", entity_setPosition },
    { "getPosition", entity_getPosition },
    { "setMatrix", entity_setMatrix },
    { "getMatrix", entity_getMatrix },
    { "isValidChild", entity_isValidChild },
    { NULL, NULL }
};

static LUA_DECLARE( destroy )
{
    CClientEntity *entity = (CClientEntity*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    // Before we do anything, fire the on-destroy event
    // This is placed here as calling from destructor is invalid (due to C++ class integrity)
    entity->CallEvent( "onClientElementDestroy", L, 0 );

    return 0;
}

static const luaL_Reg entity_interface[] =
{
    { "setChild", CClientEntity::entity_setChild },
    LUA_METHOD( destroy ),
    { NULL, NULL }
};

static int customdata_index( lua_State *L )
{
    lua_rawget( L, lua_upvalueindex( 1 ) );
    return 1;
}

static int customdata_newindex( lua_State *L )
{
    if ( lua_type( L, 2 ) != LUA_TSTRING )
        throw lua_exception( L, LUA_ERRRUN, SString( "found %s at customdata key, string required", luaL_typename( L, 2 ) ) );

    if ( lua_objlen( L, 2 ) > MAX_CUSTOMDATA_NAME_LENGTH )
        throw lua_exception( L, LUA_ERRRUN, "customdata name too long" );

    switch( lua_type( L, 3 ) )
    {
    case LUA_TCLASS:
        if ( lua_refclass( L, 3 )->IsTransmit( LUACLASS_ENTITY ) )
            break;
    default:
        lua_pushtype( L, 3 );
        throw lua_exception( L, LUA_ERRRUN, ( std::string( "attempt to assign invalid type to customdata: " ) + lua_getstring( L, 4 ) ).c_str() );
    case LUA_TNUMBER:
    case LUA_TSTRING:
    case LUA_TTABLE:
    case LUA_TNIL:
    case LUA_TBOOLEAN:
        break;  // allowed types
    }

    lua_pushvalue( L, 2 );
    lua_rawget( L, lua_upvalueindex( 1 ) );

    // If the data is equal, do not sync redundant changes
    if ( lua_equal( L, 3, 4 ) )
        return 0;

    lua_pushvalue( L, 2 );
    lua_pushvalue( L, 3 );
    lua_rawset( L, lua_upvalueindex( 1 ) );

    // Trigger the onClientElementDataChange event on us
    lua_pushvalue( L, 2 );
    lua_pushvalue( L, 4 );
    lua_pushvalue( L, 3 );
    ((CClientEntity*)lua_touserdata( L, lua_upvalueindex( 2 ) ) )->CallEvent( "onClientElementDataChange", L, 3 );
    return 0;
}

static const luaL_Reg customdata_interface[] =
{
    { "__index", customdata_index },
    { "__newindex", customdata_newindex },
    { NULL, NULL }
};

static int customdata_constructor( lua_State *L )
{
    lua_newtable( L );
    lua_pushvalue( L, lua_upvalueindex( 1 ) );
    lua_refclass( L, LUA_ENVIRONINDEX )->RegisterInterface( L, customdata_interface, 2 );

    lua_pushlstring( L, "customdata", 10 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}

static int entity_constructor( lua_State *L )
{
    lua_settop( L, 1 );

    CClientEntity *entity = (CClientEntity*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    ILuaClass& j = *lua_refclass( L, 1 );
    j.SetTransmit( LUACLASS_ENTITY, entity );

    lua_pushvalue( L, lua_upvalueindex( 1 ) );
    j.RegisterInterface( L, entity_interface, 1 );

    j.RegisterInterfaceTrans( L, entity_interface_trans, 0, LUACLASS_ENTITY );

    // Allocate customdata class
    lua_pushvalue( L, lua_upvalueindex( 1 ) );
    lua_pushcclosure( L, customdata_constructor, 1 );
    lua_newclassex( L, LCLASS_API_LIGHT );
    lua_setfield( L, LUA_ENVIRONINDEX, "data" );

    lua_pushlstring( L, "entity", 6 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}

static int sysentity_index( lua_State *L )
{
    if ( lua_type( L, 2 ) == LUA_TSTRING )
    {
        std::string key = lua_getstring( L, 2 );

        if ( key == "destroy" )
            return 0;

        if ( key == "setParent" )
            return 0;
    }

    lua_pushmethodsuper( L );
    lua_insert( L, 1 );
    lua_call( L, 2, 1 );
    return 1;
}

static const luaL_Reg sysentity_interface[] =
{
    { "__index", sysentity_index },
    { NULL, NULL }
};

static int sysentity_constructor( lua_State *L )
{
    CClientEntity *entity = (CClientEntity*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    ILuaClass& j = *lua_refclass( L, 1 );
    j.SetTransmit( LUACLASS_SYSENTITY, entity );

    j.RegisterInterfaceTrans( L, sysentity_interface, 0, LUACLASS_SYSENTITY );

    lua_pushlstring( L, "sysentity", 9 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}

CClientEntity::CClientEntity( ElementID ID, bool system, lua_State *L ) : LuaElement( L ), m_FromRootNode( this )
{
    // Extend our entity instance
    PushStack( L );
    lua_pushlightuserdata( L, this );
    lua_pushcclosure( L, entity_constructor, 1 );
    luaJ_extend( L, -2, 0 );

    if ( system )
    {
        lua_pushlightuserdata( L, this );
        lua_pushcclosure( L, sysentity_constructor, 1 );
        luaJ_extend( L, -2, 0 );
    }
    lua_pop( L, 1 );

    m_bSystemEntity = system;

    // Init
    m_pManager = NULL;
    m_pParent = NULL;
    m_usDimension = 0;
    m_ucSyncTimeContext = 0;
    m_ucInterior = 0;
    m_bDoubleSided = false;
    m_bDoubleSidedInit = false;

    // Need to generate a clientside ID?
    if ( ID == INVALID_ELEMENT_ID )
    {
        ID = CElementIDs::PopClientID ();
    }

    // Set our index in the element array
    m_ID = ID;
    CElementIDs::SetElement ( ID, this );

    m_pEventManager = new CMapEventManager( this );

    m_pAttachedToEntity = NULL;

    strncpy ( m_szTypeName, "unknown", MAX_TYPENAME_LENGTH );
    m_szTypeName [MAX_TYPENAME_LENGTH] = 0;
    m_uiTypeHash = HashString ( m_szTypeName );

    m_szName [0] = 0;

    m_pElementGroup = NULL;
    m_pModelInfo = NULL;
}

CClientEntity::~CClientEntity()
{
    // Remove collision properties
    while ( !m_notCollidableWith.empty() )
        SetCollidableWith( m_notCollidableWith.front(), true );

    // Reset our index in the element array
    if ( m_ID != INVALID_ELEMENT_ID )
    {
        CElementIDs::SetElement ( m_ID, NULL );

        // Got a clientside ID? Push it back on our stack so we don't run out of client id's
        if ( IsLocalEntity () )
        {
            CElementIDs::PushClientID ( m_ID );
        }
    }

    if ( m_pAttachedToEntity )
    {
        m_pAttachedToEntity->RemoveAttachedEntity ( this );
    }

    for ( attachments_t::iterator iter = m_AttachedEntities.begin() ; iter != m_AttachedEntities.end(); ++iter )
    {
        CClientEntity* pAttachedEntity = *iter;
        if ( pAttachedEntity )
        {
            pAttachedEntity->m_pAttachedToEntity = NULL;
        }
    }

    RemoveAllCollisions ( true );

    if ( m_pEventManager )
    {
        delete m_pEventManager;
        m_pEventManager = NULL;
    }

    if ( m_pElementGroup )
    {
        m_pElementGroup->Remove ( this );
    }

    list < CClientPed* > ::iterator iterUsers = m_OriginSourceUsers.begin ();
    for ( ; iterUsers != m_OriginSourceUsers.end () ; iterUsers++ )
    {
        CClientPed* pModel = *iterUsers;
        if ( pModel->m_interp.pTargetOriginSource == this )
        {
            pModel->m_interp.pTargetOriginSource = NULL;
            pModel->m_interp.bHadOriginSource = true;
        }
    }
    m_OriginSourceUsers.clear ();

    // Unlink our contacts
    list < CClientPed * > ::iterator iterContacts = m_Contacts.begin ();
    for ( ; iterContacts != m_Contacts.end () ; iterContacts++ )
    {
        CClientPed * pModel = *iterContacts;
        if ( pModel->GetCurrentContactEntity () == this )
        {
            pModel->SetCurrentContactEntity ( NULL );
        }
    }
    m_Contacts.clear ();

    // Remove from spatial database
    if ( !g_pClientGame->IsBeingDeleted () )
        GetClientSpatialDatabase ()->RemoveEntity ( this );

    // Ensure intrusive list nodes have been isolated
    assert ( m_FromRootNode.m_pOuterItem == this && !m_FromRootNode.m_pPrev && !m_FromRootNode.m_pNext );

    if ( !g_pClientGame->IsBeingDeleted () )
        CClientEntityRefManager::OnEntityDelete ( this );
}

void CClientEntity::SetTypeName ( const char* szName )
{
    CClientEntity::RemoveEntityFromRoot ( m_uiTypeHash, this );
    strncpy ( m_szTypeName, szName, MAX_TYPENAME_LENGTH );
    m_uiTypeHash = HashString ( szName );

    if ( IsFromRoot ( m_pParent ) )
        CClientEntity::AddEntityFromRoot ( m_uiTypeHash, this );
}

bool CClientEntity::CanUpdateSync ( unsigned char ucRemote )
{
    // We can update this element's sync only if the sync time
    // matches or either of them are 0 (ignore).
    return ( m_ucSyncTimeContext == ucRemote || ucRemote == 0 || m_ucSyncTimeContext == 0 );
}

bool CClientEntity::SetParent ( CClientEntity* pParent )
{
    if ( !pParent )
        return false;

    // Lua is top priority, so update it first
    PushMethod( m_lua, "setParent" );
    pParent->PushStack( m_lua );
    lua_call( m_lua, 1, 1 );

    bool rslt = lua_toboolean( m_lua, -1 );
    lua_pop( m_lua, 1 );

    return rslt;
}

void CClientEntity::SetID ( ElementID ID )
{
    // Eventually reset what we have at the old ID
    if ( m_ID != INVALID_ELEMENT_ID )
    {
        CElementIDs::SetElement ( m_ID, NULL );

        // Are we a clientside element? Push the unique ID
        if ( IsLocalEntity () )
        {
            CElementIDs::PushClientID ( m_ID );
        }
    }

    // Set our new ID and index in the array
    m_ID = ID;

    if ( ID != INVALID_ELEMENT_ID )
    {
        CElementIDs::SetElement ( ID, this );
    }
}

void CClientEntity::PushCustomData( lua_State *L, const char *key, bool inherit )
{
    // Try our data first
    PushStack( L );
    lua_getfield( L, -1, "data" );
    
    if ( lua_isnil( L, -1 ) && inherit && m_pParent )
    {
        lua_pop( L, 1 );

        // Might try parent's data too
        m_pParent->PushCustomData( L, key, true );
    }

    lua_insert( L, -3 );
    lua_pop( L, 2 );
}

bool CClientEntity::GetCustomDataString ( const char* szName, SString& strOut, bool bInheritData )
{
    bool success;
    PushCustomData( m_lua, szName, bInheritData );

    if ( success = ( lua_isstring( m_lua, -1 ) == 1 ) )
        strOut = lua_tostring( m_lua, -1 );

    lua_pop( m_lua, 1 );
    return success;
}

bool CClientEntity::GetCustomDataInt ( const char* szName, int& iOut, bool bInheritData )
{
    bool success;
    PushCustomData( m_lua, szName, bInheritData );

    if ( success = ( lua_isnumber( m_lua, -1 ) == 1 ) )
        iOut = (int)lua_tonumber( m_lua, -1 );

    lua_pop( m_lua, 1 );
    return success;
}

bool CClientEntity::GetCustomDataFloat ( const char* szName, float& fOut, bool bInheritData )
{
    bool success;
    PushCustomData( m_lua, szName, bInheritData );

    if ( success = ( lua_isnumber( m_lua, -1 ) == 1 ) )
        fOut = (float)lua_tonumber( m_lua, -1 );

    lua_pop( m_lua, 1 );
    return success;
}

bool CClientEntity::GetCustomDataBool ( const char* szName, bool& bOut, bool bInheritData )
{
    PushCustomData( m_lua, szName, bInheritData );

    switch( lua_type( m_lua, -1 ) )
    {
    case LUA_TSTRING:
        bOut = lua_getstring( m_lua, -1 ) == "true";
        break;
    case LUA_TNUMBER:
        bOut = lua_tonumber( m_lua, -1 ) != 0;
        break;
    case LUA_TBOOLEAN:
        bOut = lua_toboolean( m_lua, -1 ) == 1;
        break;
    default:
        lua_pop( m_lua, 1 );
        return false;
    }

    lua_pop( m_lua, 1 );
    return true;
}

bool CClientEntity::DeleteCustomData ( const char* szName, bool bRecursive )
{
    PushStack( m_lua );
    lua_getfield( m_lua, -1, "data" );
    lua_getfield( m_lua, -1, szName );
   
    bool success = lua_isnil( m_lua, -1 ) == 0;

    if ( success )
    {
        lua_pushnil( m_lua );
        lua_setfield( m_lua, -3, szName );
    }

    lua_pop( m_lua, 3 );

    if ( bRecursive && m_pParent )
        if ( m_pParent->DeleteCustomData( szName, true ) )
            success = true;

    return success;
}

bool CClientEntity::GetMatrix ( RwMatrix& matrix ) const
{
    const CEntity* pEntity = GetGameEntity ();
    if ( pEntity )
    {
        pEntity->GetMatrix( matrix );
        return true;
    }

    return false;
}

bool CClientEntity::SetMatrix ( const RwMatrix& matrix )
{
    CEntity * pEntity = GetGameEntity ();
    if ( pEntity )
    {
        pEntity->SetMatrix ( matrix );
        return true;
    }

    return false;
}

void CClientEntity::GetPositionRelative ( CClientEntity * pOrigin, CVector& vecPosition ) const
{
    CVector vecOrigin;
    pOrigin->GetPosition ( vecOrigin );
    GetPosition ( vecPosition );
    vecPosition -= vecOrigin;
}

void CClientEntity::SetPositionRelative ( CClientEntity * pOrigin, const CVector& vecPosition )
{
    CVector vecOrigin;
    pOrigin->GetPosition ( vecOrigin );
    SetPosition ( vecOrigin + vecPosition );
}

bool CClientEntity::IsOutOfBounds ( void )
{
    CVector vecPosition;
    GetPosition ( vecPosition );

    return ( vecPosition.fX < -3000.0f || vecPosition.fX > 3000.0f ||
             vecPosition.fY < -3000.0f || vecPosition.fY > 3000.0f ||
             vecPosition.fZ < -3000.0f || vecPosition.fZ > 3000.0f );
}

void CClientEntity::AttachTo ( CClientEntity* pEntity )
{
    if ( m_pAttachedToEntity )
        m_pAttachedToEntity->RemoveAttachedEntity ( this );

    m_pAttachedToEntity = pEntity;

    if ( m_pAttachedToEntity )
        m_pAttachedToEntity->AddAttachedEntity ( this );

    InternalAttachTo ( pEntity );
}

void CClientEntity::InternalAttachTo ( CClientEntity* pEntity )
{
    CPhysical * pThis = dynamic_cast < CPhysical * > ( GetGameEntity () );
    if ( pThis )
    {
        if ( pEntity )
        {
            switch ( pEntity->GetType () )
            {
                case CCLIENTVEHICLE:
                {
                    CVehicle * pGameVehicle = static_cast < CClientVehicle* > ( pEntity )->GetGameVehicle ();
                    if ( pGameVehicle )
                    {
                        pThis->AttachTo( *pGameVehicle, m_vecAttachedPosition, m_vecAttachedRotation );
                    }
                    break;
                }
                case CCLIENTPED:
                case CCLIENTPLAYER:
                {
                    CPlayerPed * pGamePed = static_cast < CClientPed* > ( pEntity )->GetGamePlayer ();
                    if ( pGamePed )
                    {
                        pThis->AttachTo( *pGamePed, m_vecAttachedPosition, m_vecAttachedRotation );                
                    }
                    break;
                }
                case CCLIENTOBJECT:
                {
                    CObject * pGameObject = static_cast < CClientObject* > ( pEntity )->GetGameObject ();
                    if ( pGameObject )
                    {
                        pThis->AttachTo( *pGameObject, m_vecAttachedPosition, m_vecAttachedRotation );
                    }
                    break;
                }
                case CCLIENTPICKUP:
                {
                    CObject * pGameObject = static_cast < CClientPickup * > ( pEntity )->GetGameObject ();
                    if ( pGameObject )
                    {
                        pThis->AttachTo( *pGameObject, m_vecAttachedPosition, m_vecAttachedRotation );
                    }
                    break;
                }
            }
        }
        else
        {
            pThis->DetachFrom( 0, 0, 0, 0 );
        }
    }
}

void CClientEntity::GetAttachedOffsets ( CVector & vecPosition, CVector & vecRotation )
{
    vecPosition = m_vecAttachedPosition;
    vecRotation = m_vecAttachedRotation;
}

void CClientEntity::SetAttachedOffsets ( CVector & vecPosition, CVector & vecRotation )
{
    CPhysical * pThis = dynamic_cast < CPhysical * > ( GetGameEntity () );
    if ( pThis )
    {
        pThis->SetAttachedOffsets ( vecPosition, vecRotation );
    }
    m_vecAttachedPosition = vecPosition;
    m_vecAttachedRotation = vecRotation;
}

bool CClientEntity::AddEvent( CLuaMain *main, const char *name, const LuaFunctionRef& ref, bool propagated )
{
    return m_pEventManager->Add( main, name, ref, propagated );
}

// The_GTA: use childCall sparringly, because in Eir it's use does not make sense.
bool CClientEntity::CallEvent( const char *name, lua_State *callee, unsigned int argCount, bool childCall )
{
    lua_checkstack( m_lua, 1 );

    // Reference ourselves so we cannot be deleted during eventcalls
    lua_class_reference ref;
    Reference( ref );

    CEvents *pEvents = g_pClientGame->GetEvents();

    // Make sure our event-manager knows we're about to call an event
    pEvents->PrePulse();

    // Call the event on our parents/us
    CallParentEvent( callee, argCount, name, this );

    if ( childCall )
        CallEventNoParent( callee, argCount, name, this );

    // Remove the arguments from the stack
    lua_pop( callee, (int)argCount );

    // Tell the event manager that we're done calling the event
    pEvents->PostPulse();

    // Return whether it got cancelled or not
    return !pEvents->WasCancelled();
}

// This _thing_ has to be abolished
void CClientEntity::CallEventNoParent( lua_State *callee, unsigned int argCount, const char *name, CClientEntity *source )
{
    // We do not have to reference ourselves, because we are referenced through CallEvent already

    // Call it on us if this isn't the same class it was raised on
    if ( source != this )
        m_pEventManager->Call( callee, argCount, name, source );

    if ( m_Children.empty() )
        return;

    // Call it on all our children
    luaRefs refs;
    CChildListType::const_iterator iter = m_Children.begin();

    for ( ; iter != m_Children.end(); iter++ )
    {
        (*iter)->Reference( refs );
        (*iter)->CallEventNoParent( callee, argCount, name, source );
    }
}

void CClientEntity::CallParentEvent( lua_State *callee, unsigned int argCount, const char *name, CClientEntity *source )
{
    // Lua takes care of logical referencing here:
    // Referenced children prevent parent destruction!

    // Call the event on us
    m_pEventManager->Call( callee, argCount, name, source );

    // Call parent's handler
    if ( m_pParent )
        m_pParent->CallParentEvent( callee, argCount, name, source );
}

bool CClientEntity::DeleteEvent( CLuaMain *main, const char *name, const LuaFunctionRef *ref )
{
    return m_pEventManager->Delete( main, name, ref );
}

void CClientEntity::DeleteEvents( CLuaMain *main, bool recursive )
{
    // Delete it from our events
    m_pEventManager->Delete( main );

    // Delete it from all our children's events
    if ( recursive )
    {
        CChildListType::const_iterator iter = m_Children.begin();

        for ( ; iter != m_Children.end(); iter++ )
            (*iter)->DeleteEvents( main, true );
    }
}

void CClientEntity::DeleteAllEvents()
{
    m_pEventManager->DeleteAll();
}

void CClientEntity::CleanUpForVM( CLuaMain *main, bool recursive )
{
    // Delete all attached invalid events
    DeleteEvents( main, false );

    // If recursive, do it on our children too
    if ( recursive )
    {
        CChildListType::const_iterator iter = m_Children.begin();

        for ( ; iter != m_Children.end(); iter++ )
            (*iter)->CleanUpForVM( main, true );
    }
}

CClientEntity* CClientEntity::FindChild ( const char* szName, unsigned int uiIndex, bool bRecursive )
{
    // Is it our name?
    unsigned int uiCurrentIndex = 0;
    if ( strcmp ( szName, m_szName ) == 0 )
    {
        if ( uiIndex == 0 )
        {
            return this;
        }
        else
        {
            ++uiCurrentIndex;
        }
    }

    // Find it among our children
    return FindChildIndex ( szName, uiIndex, uiCurrentIndex, bRecursive );
}

CClientEntity* CClientEntity::FindChildIndex ( const char* szName, unsigned int uiIndex, unsigned int& uiCurrentIndex, bool bRecursive )
{
    // Look among our children
    CChildListType ::const_iterator iter = m_Children.begin ();
    for ( ; iter != m_Children.end (); iter++ )
    {
        CClientEntity* pChild = *iter;
        // Name matches?
        if ( strcmp ( pChild->GetName (), szName ) == 0 )
        {
            // Does the index match? If it doesn't, increment it and keep searching
            if ( uiIndex == uiCurrentIndex )
            {
                return pChild;
            }
            else
            {
                ++uiCurrentIndex;
            }
        }

        // Tell this child to search too if recursive
        if ( bRecursive )
        {
            CClientEntity* pEntity = pChild->FindChildIndex ( szName, uiIndex, uiCurrentIndex, true );
            if ( pEntity )
            {
                return pEntity;
            }
        }
    }

    // Doesn't exist within us
    return NULL;
}

CClientEntity* CClientEntity::FindChildByType ( const char* szType, unsigned int uiIndex, bool bRecursive )
{
    // Is it our type?
    unsigned int uiCurrentIndex = 0;
    if ( strcmp ( szType, GetTypeName () ) == 0 )
    {
        if ( uiIndex == 0 )
        {
            return this;
        }
        else
        {
            ++uiCurrentIndex;
        }
    }

    // Find it among our children
    unsigned int uiNameHash = HashString ( szType );
    return FindChildByTypeIndex ( uiNameHash, uiIndex, uiCurrentIndex, bRecursive );
}

CClientEntity* CClientEntity::FindChildByTypeIndex ( unsigned int uiTypeHash, unsigned int uiIndex, unsigned int& uiCurrentIndex, bool bRecursive )
{
    // Look among our children
    CChildListType ::const_iterator iter = m_Children.begin ();
    for ( ; iter != m_Children.end (); iter++ )
    {
        // Name matches?
        if ( (*iter)->GetTypeHash() == uiTypeHash )
        {
            // Does the index match? If it doesn't, increment it and keep searching
            if ( uiIndex == uiCurrentIndex )
            {
                return *iter;
            }
            else
            {
                ++uiCurrentIndex;
            }
        }

        // Tell this child to search too if recursive
        if ( bRecursive )
        {
            CClientEntity* pEntity = (*iter)->FindChildByTypeIndex ( uiTypeHash, uiIndex, uiCurrentIndex, true );
            if ( pEntity )
            {
                return pEntity;
            }
        }
    }

    // Doesn't exist within us
    return NULL;
}

void CClientEntity::FindAllChildrenByType ( const char* szType, lua_State* luaVM, bool bStreamedIn )
{
    // Add all children of the given type to the table
    unsigned int uiIndex = 0;
    unsigned int uiTypeHash = HashString ( szType );

    if ( this == g_pClientGame->GetRootEntity () )
    {
        GetEntitiesFromRoot ( uiTypeHash, luaVM, bStreamedIn );
    }
    else
    {
        FindAllChildrenByTypeIndex ( uiTypeHash, luaVM, uiIndex, bStreamedIn );
    }
}

void CClientEntity::FindAllChildrenByTypeIndex ( unsigned int uiTypeHash, lua_State* luaVM, unsigned int& uiIndex, bool bStreamedIn )
{
    // Our type matches?
    if ( m_uiTypeHash == uiTypeHash )
    {
        // Only streamed in elements?
        if ( !bStreamedIn || !IsStreamingCompatibleClass() || 
             reinterpret_cast < CClientStreamElement* > ( this )->IsStreamedIn() )
        {
            // Add it to the table
            lua_pushnumber ( luaVM, ++uiIndex );
            PushStack( luaVM );
            lua_settable ( luaVM, -3 );
        }
    }

    // Call us on the children
    CChildListType ::const_iterator iter = m_Children.begin ();
    for ( ; iter != m_Children.end (); iter++ )
    {
        (*iter)->FindAllChildrenByTypeIndex ( uiTypeHash, luaVM, uiIndex, bStreamedIn );
    }
}

void CClientEntity::GetChildren ( lua_State* luaVM )
{
    // Add all our children to the table on top of the given lua main's stack
    unsigned int uiIndex = 0;
    CChildListType ::const_iterator iter = m_Children.begin ();
    for ( ; iter != m_Children.end (); iter++ )
    {
        // Add it to the table
        lua_pushnumber ( luaVM, ++uiIndex );
        (*iter)->PushStack( luaVM );
        lua_settable ( luaVM, -3 );
    }
}

bool CClientEntity::CollisionExists ( CClientColShape* pShape )
{
    list < CClientColShape* > ::iterator iter = m_Collisions.begin ();
    for ( ; iter != m_Collisions.end () ; iter++ )
    {
        if ( *iter == pShape )
        {
            return true;
        }
    }
    return false;
}

void CClientEntity::RemoveAllCollisions ( bool bNotify )
{
    if ( !m_Collisions.empty () )
    {
        if ( bNotify )
        {
            list < CClientColShape* > ::iterator iter = m_Collisions.begin ();
            for ( ; iter != m_Collisions.end () ; iter++ )
            {
                (*iter)->RemoveCollider ( this );
            }
        }

        m_Collisions.clear ();
    }
}

bool CClientEntity::IsEntityAttached ( CClientEntity* pEntity )
{
    list < CClientEntity* > ::iterator iter = m_AttachedEntities.begin ();
    for ( ; iter != m_AttachedEntities.end (); iter++ )
    {
        if ( *iter == pEntity )
            return true;
    }

    return false;
}

void CClientEntity::ReattachEntities ( void )
{
    // Jax: this should be called on streamIn/creation

    if ( m_pAttachedToEntity )
    {
        InternalAttachTo ( m_pAttachedToEntity );
    }

    // Reattach any entities attached to us
    list < CClientEntity* > ::iterator iter = m_AttachedEntities.begin ();
    for ( ; iter != m_AttachedEntities.end (); iter++ )
    {
        (*iter)->InternalAttachTo ( this );
    }  
}

bool CClientEntity::IsAttachable()
{
    switch( GetType() )
    {
    case CCLIENTPED:
    case CCLIENTPLAYER:
    case CCLIENTRADARMARKER:
    case CCLIENTVEHICLE:
    case CCLIENTOBJECT:
    case CCLIENTMARKER:
    case CCLIENTPICKUP:
    case CCLIENTSOUND:
    case CCLIENTCOLSHAPE:
        return true;
    }

    return false;
}

bool CClientEntity::IsAttachToable()
{
    switch( GetType() )
    {
    case CCLIENTPED:
    case CCLIENTPLAYER:
    case CCLIENTRADARMARKER:
    case CCLIENTVEHICLE:
    case CCLIENTOBJECT:
    case CCLIENTMARKER:
    case CCLIENTPICKUP:
    case CCLIENTSOUND:
    case CCLIENTCOLSHAPE:
        return true;
    }
    return false;
}

void CClientEntity::DoAttaching()
{
    if ( m_pAttachedToEntity )
    {
        RwMatrix matrix, returnMatrix;
        m_pAttachedToEntity->GetMatrix( matrix );

        AttachedMatrix( matrix, returnMatrix, m_vecAttachedPosition, m_vecAttachedRotation );

        SetMatrix ( returnMatrix );
    }
}

unsigned int CClientEntity::GetTypeID ( const char* szTypeName )
{
    if ( strcmp ( szTypeName, "dummy" ) == 0 )              return CCLIENTDUMMY;
    else if ( strcmp ( szTypeName, "ped" ) == 0 )           return CCLIENTPED;
    else if ( strcmp ( szTypeName, "player" ) == 0 )        return CCLIENTPLAYER;
    else if ( strcmp ( szTypeName, "projectile" ) == 0 )    return CCLIENTPROJECTILE;
    else if ( strcmp ( szTypeName, "vehicle" ) == 0 )       return CCLIENTVEHICLE;
    else if ( strcmp ( szTypeName, "object" ) == 0 )        return CCLIENTOBJECT;
    else if ( strcmp ( szTypeName, "marker" ) == 0 )        return CCLIENTMARKER;
    else if ( strcmp ( szTypeName, "blip" ) == 0 )          return CCLIENTRADARMARKER;
    else if ( strcmp ( szTypeName, "pickup" ) == 0 )        return CCLIENTPICKUP;
    else if ( strcmp ( szTypeName, "radararea" ) == 0 )     return CCLIENTRADARAREA;
    else if ( strcmp ( szTypeName, "sound" ) == 0 )         return CCLIENTSOUND;
    else                                                    return CCLIENTUNKNOWN;
}

bool CClientEntity::IsStatic ( void )
{
    CEntity * pEntity = GetGameEntity ();
    if ( pEntity )
    {
        return (pEntity->IsStatic () == TRUE);
    }
    return false;
}

void CClientEntity::SetStatic ( bool bStatic )
{
    CEntity * pEntity = GetGameEntity ();
    if ( pEntity )
    {
        pEntity->SetStatic ( bStatic );
    }
}

bool CClientEntity::IsDoubleSided ( void )
{
    CEntity* pEntity = GetGameEntity ();
    if ( pEntity )
    {
        m_bDoubleSidedInit = true;
        m_bDoubleSided = !pEntity->IsBackfaceCulled ();
    }
    return m_bDoubleSided;
}

void CClientEntity::SetDoubleSided ( bool bDoubleSided )
{
    CEntity* pEntity = GetGameEntity ();
    if ( pEntity )
    {
        pEntity->SetBackfaceCulled ( !bDoubleSided );
    }
    m_bDoubleSidedInit = true;
    m_bDoubleSided = bDoubleSided;
}

unsigned char CClientEntity::GetInterior ( void )
{
    CEntity * pEntity = GetGameEntity ();
    if ( pEntity )
    {
        return pEntity->GetAreaCode ();
    }
    return m_ucInterior;
}

void CClientEntity::SetInterior ( unsigned char ucInterior )
{
    CEntity * pEntity = GetGameEntity ();
    if ( pEntity )
    {
        pEntity->SetAreaCode ( ucInterior );
    }
    m_ucInterior = ucInterior;
}

bool CClientEntity::IsOnScreen ( void )
{
    CEntity * pEntity = GetGameEntity ();
    if ( pEntity )
    {
        return pEntity->IsOnScreen ();
    }
    return false;
}

#include <unordered_map>

// Entities from root optimization for getElementsByType
typedef CIntrusiveListExt < CClientEntity, &CClientEntity::m_FromRootNode > CFromRootListType;
typedef std::unordered_map < unsigned int, CFromRootListType > t_mapEntitiesFromRoot;
static t_mapEntitiesFromRoot    ms_mapEntitiesFromRoot;
static bool                     ms_bEntitiesFromRootInitialized = false;

void CClientEntity::StartupEntitiesFromRoot ()
{
    if ( !ms_bEntitiesFromRootInitialized )
    {
        ms_bEntitiesFromRootInitialized = true;
    }
}

// Returns true if top parent is root
bool CClientEntity::IsFromRoot ( CClientEntity* pEntity )
{
    if ( !pEntity )
        return false;
    if ( pEntity == g_pClientGame->GetRootEntity() )
        return true;
    return CClientEntity::IsFromRoot ( pEntity->GetParent () );
}

void CClientEntity::AddEntityFromRoot ( unsigned int uiTypeHash, CClientEntity* pEntity, bool bDebugCheck )
{
    // Insert into list
    CFromRootListType& listEntities = ms_mapEntitiesFromRoot [ uiTypeHash ];
    listEntities.remove ( pEntity );
    listEntities.push_front ( pEntity );

    // Apply to child elements as well
    CChildListType ::const_iterator iter = pEntity->IterBegin ();
    for ( ; iter != pEntity->IterEnd (); iter++ )
        CClientEntity::AddEntityFromRoot ( (*iter)->GetTypeHash (), *iter, false );

#if CHECK_ENTITIES_FROM_ROOT
    if ( bDebugCheck )
        _CheckEntitiesFromRoot ( uiTypeHash );
#endif
}

void CClientEntity::RemoveEntityFromRoot ( unsigned int uiTypeHash, CClientEntity* pEntity )
{
    // Remove from list
    t_mapEntitiesFromRoot::iterator find = ms_mapEntitiesFromRoot.find ( uiTypeHash );
    if ( find != ms_mapEntitiesFromRoot.end () )
    {
        CFromRootListType& listEntities = find->second;
        listEntities.remove ( pEntity );
        if ( listEntities.size () == 0 )
            ms_mapEntitiesFromRoot.erase ( find );
    }

    // Apply to child elements as well
    CChildListType ::const_iterator iter = pEntity->IterBegin ();
    for ( ; iter != pEntity->IterEnd (); iter++ )
        CClientEntity::RemoveEntityFromRoot ( (*iter)->GetTypeHash (), *iter );
}

void CClientEntity::GetEntitiesFromRoot ( unsigned int uiTypeHash, lua_State* luaVM, bool bStreamedIn )
{
#if CHECK_ENTITIES_FROM_ROOT
    _CheckEntitiesFromRoot ( uiTypeHash );
#endif

    t_mapEntitiesFromRoot::iterator find = ms_mapEntitiesFromRoot.find ( uiTypeHash );
    if ( find != ms_mapEntitiesFromRoot.end () )
    {
        CFromRootListType& listEntities = find->second;
        CClientEntity* pEntity;
        unsigned int uiIndex = 0;

        for ( CFromRootListType::reverse_iterator i = listEntities.rbegin ();
              i != listEntities.rend ();
              ++i )
        {
            pEntity = *i;

            // Only streamed in elements?
            if ( !bStreamedIn || !pEntity->IsStreamingCompatibleClass() || 
                 reinterpret_cast < CClientStreamElement* > ( pEntity )->IsStreamedIn() )
            {
                // Add it to the table
                lua_pushnumber ( luaVM, ++uiIndex );
                pEntity->PushStack( luaVM );
                lua_settable ( luaVM, -3 );
            }
        }
    }    
}


#if CHECK_ENTITIES_FROM_ROOT

//
// Check that GetEntitiesFromRoot produces the same results as FindAllChildrenByTypeIndex on the root element
//
void CClientEntity::_CheckEntitiesFromRoot ( unsigned int uiTypeHash )
{
    std::map < CClientEntity*, int > mapResults1;
    g_pClientGame->GetRootEntity()->_FindAllChildrenByTypeIndex ( uiTypeHash, mapResults1 );

    std::map < CClientEntity*, int > mapResults2;
    _GetEntitiesFromRoot ( uiTypeHash, mapResults2 );

    std::map < CClientEntity*, int > :: const_iterator iter1 = mapResults1.begin ();
    std::map < CClientEntity*, int > :: const_iterator iter2 = mapResults2.begin ();

    for ( ; iter1 != mapResults1.end (); ++iter1 )
    {
        CClientEntity* pElement1 = iter1->first;

        if ( mapResults2.find ( pElement1 ) == mapResults2.end () )
        {
            OutputDebugString ( SString ( "Client: 0x%08x  %s is missing from GetEntitiesFromRoot list\n", pElement1, pElement1->GetTypeName () ) );
        }
    }

    for ( ; iter2 != mapResults2.end (); ++iter2 )
    {
        CClientEntity* pElement2 = iter2->first;

        if ( mapResults1.find ( pElement2 ) == mapResults1.end () )
        {
            OutputDebugString ( SString ( "Client: 0x%08x  %s is missing from FindAllChildrenByTypeIndex list\n", pElement2, pElement2->GetTypeName () ) );
        }
    }

    assert ( mapResults1 == mapResults2 );
}

void CClientEntity::_FindAllChildrenByTypeIndex ( unsigned int uiTypeHash, std::map < CClientEntity*, int >& mapResults )
{
    // Our type matches?
    if ( uiTypeHash == m_uiTypeHash )
    {
        // Add it to the table
        assert ( mapResults.find ( this ) == mapResults.end () );
        mapResults [ this ] = 1;

        assert ( this );
        ElementID ID = this->GetID ();
        assert ( ID != INVALID_ELEMENT_ID );
        assert ( this == CElementIDs::GetElement ( ID ) );
        if ( this->IsBeingDeleted () )
            OutputDebugString ( SString ( "Client: 0x%08x  %s is flagged as IsBeingDeleted() but is still in FindAllChildrenByTypeIndex\n", this, this->GetTypeName () ) );
    }

    // Call us on the children
    CChildListType ::const_iterator iter = m_Children.begin ();
    for ( ; iter != m_Children.end (); iter++ )
    {
        (*iter)->_FindAllChildrenByTypeIndex ( uiTypeHash, mapResults );
    }
}

void CClientEntity::_GetEntitiesFromRoot ( unsigned int uiTypeHash, std::map < CClientEntity*, int >& mapResults )
{
    t_mapEntitiesFromRoot::iterator find = ms_mapEntitiesFromRoot.find ( uiTypeHash );
    if ( find != ms_mapEntitiesFromRoot.end () )
    {
        CFromRootListType& listEntities = find->second;
        CClientEntity* pEntity;
        unsigned int uiIndex = 0;

        for ( CFromRootListType::const_reverse_iterator i = listEntities.rbegin ();
              i != listEntities.rend ();
              ++i )
        {
            pEntity = *i;

            assert ( pEntity );
            ElementID ID = pEntity->GetID ();
            assert ( ID != INVALID_ELEMENT_ID );
            assert ( pEntity == CElementIDs::GetElement ( ID ) );
            if ( pEntity->IsBeingDeleted () )
                OutputDebugString ( SString ( "Client: 0x%08x  %s is flagged as IsBeingDeleted() but is still in GetEntitiesFromRoot\n", pEntity, pEntity->GetTypeName () ) );

            assert ( mapResults.find ( pEntity ) == mapResults.end () );
            mapResults [ pEntity ] = 1;
        }
    }    
}

#endif

bool CClientEntity::IsCollidableWith( CClientEntity *entity ) const
{
    return std::find( m_notCollidableWith.begin(), m_notCollidableWith.end(), entity ) == m_notCollidableWith.end();
}

void CClientEntity::SetCollidableWith( CClientEntity *entity, bool enable )
{
    CEntity *g_entity = GetGameEntity();
    CEntity *collision = entity->GetGameEntity();

    if ( g_entity && collision )
    {
        g_entity->SetCollidableWith( collision, enable );
    }

    // Attempt to find our entity in the list.
    collisionEntities_t::iterator iter = std::find( m_notCollidableWith.begin(), m_notCollidableWith.end(), entity );

    // If it is in the list, then collisions should be disabled for that entity.
    // We assume that every entity that is not in the list has a green light for collision.
    if ( !enable && iter == m_notCollidableWith.end() )
    {
        // Set it for the entities
        m_notCollidableWith.push_back( entity );
        entity->m_notCollidableWith.push_back( this );
    }
    else if ( enable && iter != m_notCollidableWith.end() )
    {
        m_notCollidableWith.erase( iter );
        entity->m_notCollidableWith.erase( std::find( entity->m_notCollidableWith.begin(), entity->m_notCollidableWith.end(), this ) );
    }
}

CSphere CClientEntity::GetWorldBoundingSphere()
{
    // Default to a point around the entity's position
    CVector vecPosition;
    GetPosition ( vecPosition );
    return CSphere ( vecPosition, 0.f );
}

void CClientEntity::UpdateSpatialData ( void )
{
    GetClientSpatialDatabase ()->UpdateEntity ( this );
}

// Return the distance to the other entity.
// A negative value indicates overlapping bounding spheres
float CClientEntity::GetDistanceBetweenBoundingSpheres ( CClientEntity* pOther )
{
    CSphere sphere = GetWorldBoundingSphere ();
    CSphere otherSphere = pOther->GetWorldBoundingSphere ();
    return ( sphere.vecPosition - otherSphere.vecPosition ).Length () - sphere.fRadius - otherSphere.fRadius;
}
