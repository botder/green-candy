/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaArguments.cpp
*  PURPOSE:     Lua argument manager class
*  DEVELOPERS:  Ed Lyons <>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Chris McArthur <>
*               Cecill Etheredge <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

#ifndef WIN32
#include <clocale>
#endif

extern CGame* g_pGame;

#ifndef VERIFY_ELEMENT
#define VERIFY_ELEMENT(element) (g_pGame->GetMapManager()->GetRootElement ()->IsMyChild(element,true)&&!element->IsBeingDeleted())
#endif

CLuaArguments::CLuaArguments( NetBitStreamInterface& stream )
{
    ReadFromBitStream( stream );
}

void CLuaArguments::ReadArgument( lua_State *L, int idx, luaArgRep_t *cached )
{
    m_args.push_back( new CLuaArgument( L, idx, cached ) );
}

LuaArgument* CLuaArguments::PushNil()
{
    CLuaArgument *arg = new CLuaArgument;
    m_args.push_back( arg );
    return arg;
}

LuaArgument* CLuaArguments::PushBoolean( bool b )
{
    CLuaArgument *arg = new CLuaArgument( b );
    m_args.push_back( arg );
    return arg;
}

LuaArgument* CLuaArguments::PushNumber( double d )
{
    CLuaArgument *arg = new CLuaArgument( d );
    m_args.push_back( arg );
    return arg;
}

LuaArgument* CLuaArguments::PushString( const std::string& str )
{
    CLuaArgument *arg = new CLuaArgument( str );
    m_args.push_back( arg );
    return arg;
}

LuaArgument* CLuaArguments::PushUserData( void *ud )
{
    CLuaArgument *arg = new CLuaArgument( ud );
    m_args.push_back( arg );
    return arg;
}

LuaArgument* CLuaArguments::PushArgument( const LuaArgument& other )
{
    CLuaArgument *arg = (CLuaArgument*)other.Clone();
    m_args.push_back( arg );
    return arg;
}

#if 0
LuaArgument* CLuaArguments::PushTable( const LuaArguments& table )
{
    CLuaArgument *arg = new CLuaArgument();
    arg->Read( &table );
    m_args.push_back( arg );
    return arg;
}
#endif

CLuaArgument* CLuaArguments::PushElement( CClientEntity* element )
{
    CLuaArgument *arg = new CLuaArgument( element );
    m_args.push_back( arg );
    return arg;
}

CLuaArgument* CLuaArguments::PushElement( CElement *element )
{
    CLuaArgument *arg = new CLuaArgument( element );
    m_args.push_back( arg );
    return arg;
}

CLuaArgument* CLuaArguments::PushACL( CAccessControlList *acl )
{
    CLuaArgument *arg = new CLuaArgument;
    arg->ReadUserData( acl );
    m_args.push_back( arg );
    return arg;
}

CLuaArgument* CLuaArguments::PushACLGroup( CAccessControlListGroup *aclGroup )
{
    CLuaArgument *arg = new CLuaArgument;
    arg->ReadUserData( aclGroup );
    m_args.push_back( arg );
    return arg;
}

CLuaArgument* CLuaArguments::PushAccount( CAccount *acc )
{
    CLuaArgument *arg = new CLuaArgument;
    arg->ReadUserData( acc );
    m_args.push_back( arg );
    return arg;
}

CLuaArgument* CLuaArguments::PushResource( CResource *res )
{
    CLuaArgument *arg = new CLuaArgument;
    arg->ReadUserData( res );
    m_args.push_back( arg );
    return arg;
}

CLuaArgument* CLuaArguments::PushTextDisplay( CTextDisplay *disp )
{
    CLuaArgument *arg = new CLuaArgument;
    arg->ReadUserData( disp );
    m_args.push_back( arg );
    return arg;
}

CLuaArgument* CLuaArguments::PushTextItem( CTextItem *item )
{
    CLuaArgument *arg = new CLuaArgument;
    arg->ReadUserData( item );
    m_args.push_back( arg );
    return arg;
}

CLuaArgument* CLuaArguments::PushTimer( CLuaTimer *timer )
{
    CLuaArgument *arg = new CLuaArgument;
    arg->ReadUserData( timer );
    m_args.push_back( arg );
    return arg;
}

bool CLuaArguments::ReadFromBitStream( NetBitStreamInterface& bitStream )
{
    unsigned short usNumArgs;

    if ( !bitStream.ReadCompressed( usNumArgs ) )
        return true;

    while ( usNumArgs-- )
        m_Arguments.push_back( new CLuaArgument( bitStream ) );

    return true;
}

bool CLuaArguments::WriteToBitStream( NetBitStreamInterface& bitStream ) const
{
    std::vector <LuaArgument*>::const_iterator iter = m_args.begin();

    bitStream.WriteCompressed( (unsigned short)m_args.size() ) );

    for ( ; iter != m_args.end(); iter++ )
    {
        if ( !(*iter)->WriteToBitStream( bitStream ) )
            return false;
    }

    return true;
}

bool CLuaArguments::WriteToJSONString( std::string& json, bool serialize )
{
    json_object *arr = WriteToJSONArray( serialize );

    if ( !arr )
        return false;

    json = json_object_get_string( arr );
    json_object_put( arr ); // dereference - causes a crash, is actually commented out in the example too
    return true;
}

#ifndef VERIFY_ELEMENT
#define VERIFY_ELEMENT(element) (g_pGame->GetMapManager()->GetRootElement ()->IsMyChild(element,true)&&!element->IsBeingDeleted())
#endif

#ifndef VERIFY_RESOURCE
#define VERIFY_RESOURCE(resource) (g_pGame->GetResourceManager()->Exists(resource))
#endif

inline static json_object* json_CreateObjectFromArgument( LuaArgument& arg, bool serialize )
{
    switch( arg->GetType() )
    {
    case LUA_TNIL:
        return json_object_new_int( 0 );
    case LUA_TBOOLEAN:
        return json_object_new_boolean( arg.GetBoolean() );
    case LUA_TTABLE:
#ifdef _TODO
        if ( pKnownTables && pKnownTables->find ( m_pTableData ) != pKnownTables->end () )
        {
            char szTableID[10];
            snprintf ( szTableID, sizeof(szTableID), "^T^%lu", pKnownTables->find ( m_pTableData )->second );
            return json_object_new_string ( szTableID );
        }
        else
        {
            return m_pTableData->WriteTableToJSONObject( bSerialize, pKnownTables );
        }
#endif //_TODO
        break;
    case LUA_TNUMBER:
        int iNum = (int)arg.GetNumber();

        if ( iNum == (float)arg.GetNumber() )
            return json_object_new_int( iNum );

        return json_object_new_double( arg.GetNumber() );
    case LUA_TSTRING:
        if ( arg.GetString().size() > 65535 )
        {
            g_pGame->GetScriptDebugging()->LogError( "Couldn't convert argument list to JSON. Invalid string specified, limit is 65535 characters." );
            return NULL;
        }

        return json_object_new_string_len( arg.GetString().c_str(), arg.GetString().size() );
    case LUA_TLIGHTUSERDATA:
        CElement *pElement = arg.GetElement();
        CResource *pResource = (CResource*)arg.GetLightUserData();
        
        // Elements are dynamic, so storing them is potentially unsafe
        if ( pElement && serialize )
        {
            char szElementID[10];
            snprintf( szElementID, 9, "^E^%d", pElement->GetID().Value() );
            return json_object_new_string( szElementID );
        }
        else if ( VERIFY_RESOURCE( pResource ) )
        {
            char szElementID[ MAX_RESOURCE_NAME_LENGTH + 4 ];
            snprintf( szElementID, MAX_RESOURCE_NAME_LENGTH + 3, "^R^%s", pResource->GetName().c_str() );
            return json_object_new_string( szElementID );
        }

        g_pGame->GetScriptDebugging()->LogError( "Couldn't convert argument list to JSON, only valid elements can be sent." );
        return NULL;
    }

    g_pGame->GetScriptDebugging()->LogError( "Couldn't convert argument list to JSON, unsupported data type. Use Table, Nil, String, Number, Boolean, Resource or Element." );
    return NULL;
}

json_object* CLuaArguments::WriteToJSONArray( bool serialize )
{
    //TODO: add table referencing support
    json_object *arr = json_object_new_array();
    std::vector <LuaArgument*>::const_iterator iter = m_args.begin();

    for ( ; iter != m_args.end() ; iter++ )
    {
        json_object *obj = json_CreateObjectFromArgument( **iter, serialize );

        if ( !obj )
            continue;

        json_object_array_add( arr, obj );
    }
    return arr;
}

static inline bool json_WriteArgumentToString( const LuaArgument& arg, std::string& buf )
{
    switch( arg.GetType() )
    {
    case LUA_TNONE:
    case LUA_TNIL:
        buf = "0";
        return true;

    case LUA_TBOOLEAN:
        buf = arg.GetString();
        return true;

    case LUA_TTABLE:
        g_pGame->GetScriptDebugging()->LogError( "Cannot convert table to string (do not use tables as keys in tables if you want to send them over http/JSON)." );
        return false;

    case LUA_TNUMBER:
        float fNum = (float)GetNumber();
        int iNum = (int)GetNumber();
        std::stringstream stream;

        if ( iNum == fNum )
            stream << iNum;
        else
            stream << fNum;

        buf = stream.str();
        return true;

    case LUA_TSTRING:
        size_t len = arg.GetString().size();

        if ( len > 65535 )
        {
            g_pGame->GetScriptDebugging()->LogError( "String is too long. Limit is 65535 characters." );
            return false;
        }

        buf = arg.GetString();
        return true;

    case LUA_TLIGHTUSERDATA:
        CLuaArgument& argex = (CLuaArgument&)arg;
        CElement *elem = argex.GetElement();
        CResource *res = (CResource*)GetLightUserData();

        if ( elem )
        {
            buf = SString( "#E#%d", (int)elem->GetID().Value() );
            return true;
        }
        else if ( VERIFY_RESOURCE( res ) )
        {
            buf = SString( "#R#%s", res->GetName().c_str() );
            return true;
        }

        g_pGame->GetScriptDebugging()->LogError( "Couldn't convert element to string, only valid elements can be sent." );
        return false;
    }

    g_pGame->GetScriptDebugging()->LogError( "Couldn't convert argument to string, unsupported data type. Use String, Number, Boolean or Element." );
    return false;
}

json_object* CLuaArguments::WriteTableToJSONObject( bool serialize )
{
    std::vector <LuaArgument*>::iterator iter = m_args.begin();

    if ( IsIndexedArray() )
    {
        json_object *arr = json_object_new_array();

        for ( ; iter != m_args.end(); iter++ ) 
        {
            iter++; // skip the key values

            json_object *obj = json_CreateObjectFromArgument( **iter, serialize );

            if ( !obj )
                break;

            json_object_array_add( arr, obj );
        }

        return arr;
    }

    json_object *dict = json_object_new_object();

    for ( ; iter != m_args.end(); iter++ )
    {
        std::string buf;

        if ( !json_WriteArgumentToString( **iter, buf ) ) // index
            break;

        iter++;

        json_object *obj = json_CreateObjectFromArgument( **iter, serialize );

        if ( !obj2 )
            break;

        json_object_object_add( dict, buf.c_str(), obj );
    }

    return dict;
}

inline static CLuaArgument* json_CreateArgumentFromObject( json_object *obj )
{
    if ( is_error( obj ) )
        return NULL;

    if ( !obj )
        return new CLuaArgument; 

    switch( json_object_get_type( obj ) )
    {
    case json_type_null:
        return new CLuaArgument;
    case json_type_boolean:
        return new CLuaArgument( json_object_get_boolean( obj ) == TRUE );
    case json_type_double:
        return new CLuaArgument( json_object_get_double( obj ) );
    case json_type_int:
        return new CLuaArgument( (double)json_object_get_int( object ) );
    case json_type_object:
#ifdef _TODO
        m_table = new CLuaArguments();
        m_pTableData->ReadFromJSONObject ( object, pKnownTables );
        m_bWeakTableRef = false;
        m_iType = LUA_TTABLE;
        break;
#endif //_TODO
        return NULL;
    case json_type_array:
#ifdef _TODO
        m_pTableData = new CLuaArguments();
        m_pTableData->ReadFromJSONArray ( object, pKnownTables );
        m_bWeakTableRef = false;
        m_iType = LUA_TTABLE;
        break;
#endif //_TODO
        return NULL;
    case json_type_string:
        char *string = json_object_get_string( obj );

        if ( strlen( string ) < 4 || string[0] != '^' || string[2] != '^' )
        {
            Read( std::string( szString ) );
            return true;
        }

        switch( string[1] )
        {
        case 'E': // element
            int id = atoi( string + 3 );
            CElement *element;

            if ( id == 0 || !( element = CElementIDs::GetElement( id ) ) )
                return new CLuaArgument;

            return new CLuaArgument( element );
        case 'R': // resource
            CResource *resource = g_pGame->GetResourceManager()->GetResource( string + 3 );

            if ( !resource )
            {
                g_pGame->GetScriptDebugging()->LogError( "Invalid resource specified in JSON string '%s'.", string );
                return new CLuaArgument;
            }
            return CLuaArgument( (void *)resource );
        case 'T':   // Table reference
#ifdef _TODO
            unsigned long ulTableID = static_cast < unsigned long > ( atol ( szString + 3 ) );

            if ( pKnownTables && ulTableID >= 0 && ulTableID < pKnownTables->size () )
            {
                m_pTableData = pKnownTables->at ( ulTableID );
                m_bWeakTableRef = true;
                m_iType = LUA_TTABLE;
            }
            else
            {
                g_pGame->GetScriptDebugging()->LogError ( NULL, SString ( "Invalid table reference specified in JSON string '%s'.", szString ) );
                m_iType = LUA_TNIL;
            }
            break;
#endif //_TODO
            return new CLuaArgument;
        }
        return new CLuaArgument;
    }
    return new CLuaArgument;
}

bool CLuaArguments::ReadFromJSONString( const char *json )
{
    json_object *object = json_tokener_parse( json );

    if ( is_error( object ) )
        return false;

    if ( json_object_get_type( object ) != json_type_array )
    {
        json_object_put( object );
        return false;
    }

    int size = json_object_array_length( object );
    
    for ( int i=0; i < size; i++ ) 
    {
        CLuaArgument *arg = json_CreateArgumentFromObject( json_object_array_get_idx( object, i ) );

        m_args.push_back( new CLuaArgument( i ) );

        if ( !arg )
        {
            json_object_put( object );
            return false;
        }

        m_args.push_back( arg ); // then the value
    }
    json_object_put( object ); // dereference
    return true;
}

bool CLuaArguments::ReadFromJSONObject( json_object *object )
{
    if ( is_error( object ) )
        return false;

    if ( json_object_get_type ( object ) != json_type_object )
        return false;

    json_object_object_foreach( object, key, val ) 
    {
        CLuaArgument *arg;

        arg = json_CreateArgumentFromObject( val );

        if ( !arg )
            return false;

        m_args.push_back( new CLuaArgument( std::string( key ) ) ); // push the key first
        m_args.push_back( arg ); // now the value
    }

    return true;
}

bool CLuaArguments::ReadFromJSONArray( json_object *object )
{
    if ( is_error( object ) )
        return false;

    if ( json_object_get_type( object ) != json_type_array )
        return false;

    int size = json_object_array_length( object );

    for ( int i=0; i < size; i++ ) 
    {
        CLuaArgument *arg;

        if ( !( arg = json_CreateArgumentFromObject( json_object_array_get_idx( object, i ) ) ) )
            return false;

        m_args.push_back( new CLuaArgument( (double)i + 1 ) ); // push the key
        m_args.push_back( arg ); // then the value
    }

    return true;
}
