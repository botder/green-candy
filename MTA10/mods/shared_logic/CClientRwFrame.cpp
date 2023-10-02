/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientRwFrame.cpp
*  PURPOSE:     RenderWare frame management
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

static LUA_DECLARE( setName )
{
    const char *name;

    LUA_ARGS_BEGIN;
    argStream.ReadString( name );
    LUA_ARGS_END;

    ((CClientRwFrame*)lua_getmethodtrans( L ))->SetName( name );
    LUA_SUCCESS;
}

static LUA_DECLARE( getName )
{
    lua_pushstring( L, ((CClientRwFrame*)lua_getmethodtrans( L ))->GetName() );
    return 1;
}

static LUA_DECLARE( getHash )
{
    lua_pushnumber( L, ((CClientRwFrame*)lua_getmethodtrans( L ))->GetHash() );
    return 1;
}

static LUA_DECLARE( getLTM )
{
    lua_creatematrix( L, ((CClientRwFrame*)lua_getmethodtrans( L ))->GetObject().GetLTM() );
    return 1;
}

static LUA_DECLARE( setModelling )
{
    RwMatrix *mat;

    LUA_ARGS_BEGIN;
    argStream.ReadClass( mat, LUACLASS_MATRIX );
    LUA_ARGS_END;

    ((CClientRwFrame*)lua_getmethodtrans( L ))->GetObject().SetModelling( *mat );
    LUA_SUCCESS;
}

static LUA_DECLARE( getModelling )
{
    lua_creatematrix( L, ((CClientRwFrame*)lua_getmethodtrans( L ))->GetObject().GetModelling() );
    return 1;
}

static LUA_DECLARE( setPosition )
{
    CVector pos;

    LUA_ARGS_BEGIN;
    argStream.ReadVector( pos );
    LUA_ARGS_END;

    ((CClientRwFrame*)lua_getmethodtrans( L ))->GetObject().SetPosition( pos );
    LUA_SUCCESS;
}

static LUA_DECLARE( getPosition )
{
    const CVector& pos = ((CClientRwFrame*)lua_getmethodtrans( L ))->GetObject().GetPosition();

    lua_pushnumber( L, pos[0] );
    lua_pushnumber( L, pos[1] );
    lua_pushnumber( L, pos[2] );
    return 3;
}

static LUA_DECLARE( getObjects )
{
    CClientRwFrame::objects_t& list = ((CClientRwFrame*)lua_getmethodtrans( L ))->m_objects;

    lua_settop( L, 0 );
    lua_createtable( L, list.size(), 0 );

    unsigned int n = 1;

    for ( CClientRwFrame::objects_t::iterator iter = list.begin(); iter != list.end(); iter++, n++ )
    {
        (*iter)->PushStack( L );
        lua_rawseti( L, 1, n );
    }

    return 1;
}

static inline CClientRwFrame* RwFrameFindChildHierarchy( CClientRwFrame *frame, const std::string& name )
{
    if ( name == frame->GetName() )
        return frame;

    // Check the children
    for ( CClientRwFrame::children_t::iterator iter = frame->m_children.begin(); iter != frame->m_children.end(); iter++ )
        if ( CClientRwFrame *rslt = RwFrameFindChildHierarchy( *iter, name ) )
            return rslt;

    return NULL;
}

static LUA_DECLARE( findFrame )
{
    std::string name;
    bool checkHierarchy;

    LUA_ARGS_BEGIN;
    argStream.ReadString( name );
    argStream.ReadBool( checkHierarchy, true );
    LUA_ARGS_END;

    CClientRwFrame *frame = ((CClientRwFrame*)lua_getmethodtrans( L ));

    if ( checkHierarchy )
    {
        for ( CClientRwFrame::children_t::iterator iter = frame->m_children.begin(); iter != frame->m_children.end(); iter++ )
        {
            if ( CClientRwFrame *rslt = RwFrameFindChildHierarchy( *iter, name ) )
            {
                rslt->PushStack( L );
                return 1;
            }
        }
    }
    else
    {
        for ( CClientRwFrame::children_t::iterator iter = frame->m_children.begin(); iter != frame->m_children.end(); iter++ )
        {
            if ( (*iter)->GetName() != name )
                continue;

            (*iter)->PushStack( L );
            return 1;
        }
    }

    return 0;
}

static LUA_DECLARE( getLinkedFrames )
{
    CClientRwFrame::children_t& list = ((CClientRwFrame*)lua_getmethodtrans( L ))->m_children;

    lua_settop( L, 0 );
    lua_createtable( L, list.size(), 0 );

    unsigned int n = 1;

    for ( CClientRwFrame::children_t::iterator iter = list.begin(); iter != list.end(); iter++, n++ )
    {
        (*iter)->PushStack( L );
        lua_rawseti( L, 1, n );
    }

    return 1;
}

static LUA_DECLARE( isValidChild )
{
    if ( lua_type( L, 1 ) != LUA_TCLASS )
        return 0;

    ILuaClass& j = *lua_refclass( L, 1 );

    if ( j.IsTransmit( LUACLASS_RWFRAME ) )
    {
        lua_pushboolean( L, true );
        return 1;
    }

    CClientRwObject *obj;

    lua_pushboolean( L, j.GetTransmit( LUACLASS_RWOBJECT, (void*&)obj ) && obj->GetObject().IsFrameExtension() );
    return 1;
}

LUA_DECLARE( CClientRwFrame::unlinkParentFrame )
{
    CClientRwFrame::children_t& list = ((CClientRwFrame*)lua_touserdata( L, lua_upvalueindex( 1 ) ))->m_children;
    CClientRwFrame *child = ((CClientRwFrame*)lua_touserdata( L, lua_upvalueindex( 2 ) ));

    // Remove the frame connection
    child->m_parent = NULL;

    // Untie internally and in our API
    child->m_frame.Unlink();
    list.erase( std::find( list.begin(), list.end(), child ) );
    return 0;
}

LUA_DECLARE( CClientRwFrame::unlinkParentObject )
{
    CClientRwFrame::objects_t& list = ((CClientRwFrame*)lua_touserdata( L, lua_upvalueindex( 1 ) ))->m_objects;
    CClientRwObject *obj = (CClientRwObject*)lua_touserdata( L, lua_upvalueindex( 2 ) );

    // Remove the frame connection
    obj->m_parent = NULL;

    // Untie stuff
    obj->GetObject().SetFrame( NULL );
    list.erase( std::find( list.begin(), list.end(), obj ) );
    return 0;
}

LUA_DECLARE( CClientRwFrame::setChild )
{
    ILuaClass& j = *lua_refclass( L, 1 );
    CClientRwFrame *frame = (CClientRwFrame*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    // Make sure we end up with child(1), childAPI(2) stack values!
    lua_settop( L, 1 );

    // Grab the childAPI; this will check the first argument for class validity
    lua_getfield( L, LUA_ENVIRONINDEX, "super" );
    lua_pushvalue( L, 1 );
    lua_call( L, 1, 1 );

    // Get the childAPI's environment
    lua_refclass( L, 2 )->PushEnvironment( L );

    union
    {
        CClientRwFrame *child;
        CClientRwObject *obj;
    };

    if ( j.GetTransmit( LUACLASS_RWFRAME, (void*&)child ) )
    {
        // frame holds child (CClientRwFrame -> CClientRwFrame)
        frame->m_children.insert( frame->m_children.begin(), child );
        frame->m_frame.Link( &child->m_frame );

        lua_pushlightuserdata( L, frame );
        lua_pushlightuserdata( L, child );
        lua_pushcclosure( L, unlinkParentFrame, 2 );
    }
    else if ( j.GetTransmit( LUACLASS_RWOBJECT, (void*&)obj ) )
    {
        // frame holds obj (CClientRwFrame -> CClientRwObjectEx)
        frame->m_objects.insert( frame->m_objects.begin(), obj );
        obj->GetObject().SetFrame( &frame->m_frame );
        
        lua_pushlightuserdata( L, frame );
        lua_pushlightuserdata( L, obj );
        lua_pushcclosure( L, unlinkParentObject, 2 );
    }
    else
        assert( 0 );

    // A disconnector has to be provided above
    lua_setfield( L, 3, "notifyDestroy" );
    lua_settop( L, 2 );

    // <3 unions are lovely!
    obj->m_parent = frame;
    obj->SetOwner( frame->GetOwner() );
    return 1;
}

static const luaL_Reg rwframe_interface[] =
{
    { "setChild", CClientRwFrame::setChild },
    { NULL, NULL }
};

static const luaL_Reg rwframe_interface_trans[] =
{
    LUA_METHOD( getName ),
    LUA_METHOD( setName ),
    LUA_METHOD( getHash ),
    LUA_METHOD( getLTM ),
    LUA_METHOD( setModelling ),
    LUA_METHOD( getModelling ),
    LUA_METHOD( setPosition ),
    LUA_METHOD( getPosition ),
    LUA_METHOD( getObjects ),
    LUA_METHOD( findFrame ),
    LUA_METHOD( getLinkedFrames ),
    LUA_METHOD( isValidChild ),
    { NULL, NULL }
};

static LUA_DECLARE( luaconstructor_rwframe )
{
    CClientRwFrame *frame = (CClientRwFrame*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    ILuaClass& j = *lua_refclass( L, 1 );
    j.SetTransmit( LUACLASS_RWFRAME, frame );

    lua_pushvalue( L, lua_upvalueindex( 1 ) );
    j.RegisterInterface( L, rwframe_interface, 1 );
    j.RegisterInterfaceTrans( L, rwframe_interface_trans, 0, LUACLASS_RWFRAME );

    lua_pushlstring( L, "rwframe", 7 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}

static inline void RwFrameAssignObject( CRwObject *obj, CClientRwFrame *parent )
{
    union
    {
        CClientRwObject *rwobj;
        CClientAtomic *atom;
        CClientLight *light;
        CClientRwCamera *cam;
    };

    switch( obj->GetType() )
    {
    case RW_ATOMIC:
        atom = new CClientAtomic( g_L, NULL, *dynamic_cast <CRpAtomic*> ( obj ) );
        break;
    case RW_LIGHT:
        light = new CClientLight( g_L, NULL, *dynamic_cast <CRpLight*> ( obj ) );
        break;
    case RW_CAMERA:
        cam = new CClientRwCamera( g_L, *dynamic_cast <CRwCamera*> ( obj ) );
        break;
    }

    rwobj->SetOwner( parent->GetOwner() );
    rwobj->SetParent( parent );
    rwobj->SetRoot( parent->GetRoot() );
    rwobj->DisableKeepAlive();
}

static inline void RwFrameAssignChild( CRwFrame *child, CClientRwFrame *parent )
{
    lua_State *L = g_L;
    CClientRwFrame *frame = new CClientRwFrame( L, *child, parent->GetOwner() );

    // Establish the connection; this should do all the magic
    frame->SetParent( parent );
}

CClientRwFrame::CClientRwFrame( lua_State *L, CRwFrame& frame, CResource *owner ) : CClientRwObject( L, frame ), m_frame( frame )
{
    // Lua instancing
    PushStack( L );
    lua_pushlightuserdata( L, this );
    lua_pushcclosure( L, luaconstructor_rwframe, 1 );
    luaJ_extend( L, -2, 0 );
    lua_pop( L, 1 );

    // The frame needs the owner at creation to give it to all children
    SetOwner( owner );

    // Obtain all children...
    {
        CRwFrame::childList_t& list = frame.GetChildren();

        for ( CRwFrame::childList_t::iterator iter = list.begin(); iter != list.end(); iter++ )
            RwFrameAssignChild( *iter, this );
    }

    // ... and objects
    {
        CRwFrame::objectList_t& list = frame.GetObjects();

        for ( CRwFrame::objectList_t::iterator iter = list.begin(); iter != list.end(); iter++ )
            RwFrameAssignObject( *iter, this );
    }
}

CClientRwFrame::~CClientRwFrame()
{
}

void CClientRwFrame::MarkGC( lua_State *L )
{
    // Due to the complexity of this routine, we should reference the frame
    // This is not to prevent Garbage Collection, but destruction by scripts or the system.
    IncrementMethodStack();

    // We need new lists and reference both lists
    // Referencing has to be an atomic operation: do not yield inbetween!
    objects_t _objects;
    children_t _children;

    for ( objects_t::const_iterator iter = m_objects.begin(); iter != m_objects.end(); iter++ )
    {
        (*iter)->IncrementMethodStack();
        _objects.push_back( *iter );
    }

    for ( children_t::const_iterator iter = m_children.begin(); iter != m_children.end(); iter++ )
    {
        (*iter)->IncrementMethodStack();
        _children.push_back( *iter );
    }

    // Mark all objects...
    for ( objects_t::const_iterator iter = _objects.begin(); iter != _objects.end(); iter++ )
    {
        // Objects which are in frames have to be kept alive no matter what the object says
        CClientRwObject *obj = *iter;

        obj->m_class->Propagate( L );
        lua_gcpaycost( L, sizeof( *obj ) );
    }

    // ...and children
    for ( children_t::const_iterator iter = _children.begin(); iter != _children.end(); iter++ )
        (*iter)->MarkGC( L );

    // We can dereference everything
    for ( children_t::const_iterator iter = _children.begin(); iter != _children.end(); iter++ )
        (*iter)->DecrementMethodStack();

    for ( objects_t::const_iterator iter = _objects.begin(); iter != _objects.end(); iter++ )
        (*iter)->DecrementMethodStack();

    m_class->Propagate( L );
    lua_gcpaycost( L, sizeof( *this ) );

    // Dereference us again (and possibly destroy)
    DecrementMethodStack();
}