/*****************************************************************************
*
*  PROJECT:     Lua Interpreter
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        luafile.cpp
*  PURPOSE:     Test environment for the filesystem
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  For documentation visit http://wiki.mtasa.com/wiki/MTA:Eir/FileSystem/
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include "luafile.Utils.hxx"

using namespace std;

static int luafile_onIndex( lua_State *lua )
{
    lua_pushvalue( lua, 2 );
    lua_gettable( lua, lua_upvalueindex( 1 ) );

    if ( lua_type( lua, 3 ) == LUA_TBOOLEAN && lua_toboolean( lua, 3 ) == false )
        return 0;

    lua_pop( lua, 1 );
    lua_gettable( lua, lua_upvalueindex( 2 ) );
    return 1;
}

static int luafile_onNewindex( lua_State *lua )
{
    // We do not allow any modification from outside
    return 0;
}

static int luafile_read( lua_State *L )
{
    luaL_checktype( L, 1, LUA_TNUMBER );

    CFile *file = (CFile*)lua_getmethodtrans( L );

    long byteCount = (long)lua_tonumber( L, 1 );

    LUA_CHECK( byteCount >= 0 );

    size_t bytesRead = (size_t)byteCount;

    if ( bytesRead == 0 )
    {
        lua_pushlstring( L, "", 0 );
        return 1;
    }

    std::vector <char> buf( bytesRead );

#ifdef FU_CLASS
    bytesRead = file->Read( &buf[0], 1, bytesRead );

    if ( bytesRead == 0 )
    {
        lua_pushlstring( L, "", 0 );
        return 1;
    }
#else
    bytesRead = file->Read( &buf[0], 1, bytesRead );
#endif

    lua_pushlstring( L, &buf[0], bytesRead );
    return 1;
}

inline bool IsUnsigned( unsigned char )
{
    return true;
}
inline bool IsUnsigned( char )
{
    return false;
}
inline bool IsUnsigned( unsigned short )
{
    return true;
}
inline bool IsUnsigned( short )
{
    return false;
}
inline bool IsUnsigned( unsigned int )
{
    return true;
}
inline bool IsUnsigned( int )
{
    return false;
}

struct _CheckDefaultValidity
{
    template <typename numberType>
    AINLINE bool IsNumberValid( lua_Number number )
    {
        return true;
    }
};

template <typename numberType>
AINLINE int _fileReadNumber( lua_State *L )
{
    numberType out_num;

    LUA_CHECK(
        ((CFile*)lua_getmethodtrans( L ))->ReadStruct( out_num )
    );

    lua_pushnumber( L, (lua_Number)out_num );
    return 1;
}

// We use the computer systems analogy char<->byte.
static int luafile_readByte( lua_State *L )
{
    return _fileReadNumber <fsChar_t> ( L );
}
static int luafile_readUByte( lua_State *L )
{
    return _fileReadNumber <fsUChar_t> ( L );
}
static int luafile_readShort( lua_State *L )
{
    return _fileReadNumber <fsShort_t> ( L );
}
static int luafile_readUShort( lua_State *L )
{
    return _fileReadNumber <fsUShort_t> ( L );
}
static int luafile_readInt( lua_State *L )
{
    return _fileReadNumber <fsInt_t> ( L );
}
static int luafile_readUInt( lua_State *L )
{
    return _fileReadNumber <fsUInt_t> ( L );
}
static int luafile_readWideInt( lua_State *L )
{
    return _fileReadNumber <fsWideInt_t> ( L );
}
static int luafile_readUWideInt( lua_State *L )
{
    return _fileReadNumber <fsUWideInt_t> ( L );
}

static int luafile_readFloat( lua_State *L )
{
    return _fileReadNumber <fsFloat_t> ( L );
}
static int luafile_readDouble( lua_State *L )
{
    return _fileReadNumber <fsDouble_t> ( L );
}

static int luafile_readBoolean( lua_State *L )
{
    bool out_b;

    bool successful =
        ((CFile*)lua_getmethodtrans( L ))->ReadBool( out_b );

    if ( !successful )
        lua_pushnil( L );
    else
        lua_pushboolean( L, out_b );

    return 1;
}

template <typename numberType, typename validityChecker>
AINLINE int _writeFileNumber( lua_State *L, const char *methodName )
{
    luaL_checktype( L, 1, LUA_TNUMBER );

    lua_Number number = lua_tonumber( L, 1 );

    // Check validity of number.
    {
        validityChecker checker;

        if ( !checker.template IsNumberValid <numberType> ( number ) )
        {
            // todo: print a warning.
        }
    }

    numberType realNum = (numberType)number;

    lua_pushnumber( L, ((CFile*)lua_getmethodtrans( L ))->WriteStruct( realNum ) );
    return 1;
}

static int luafile_write( lua_State *L )
{
    luaL_checktype( L, 1, LUA_TSTRING );

    size_t len;
    const char *string = lua_tolstring( L, 1, &len );

    lua_pushwideinteger( L, ((CFile*)lua_getmethodtrans( L ))->Write( string, 1, len ) );
    return 1;
}

static int luafile_writeByte( lua_State *L )
{
    return _writeFileNumber <fsChar_t, _CheckDefaultValidity> ( L, "writeByte" );
}
static int luafile_writeUByte( lua_State *L )
{
    return _writeFileNumber <fsUChar_t, _CheckDefaultValidity> ( L, "writeUByte" );
}
static int luafile_writeShort( lua_State *L )
{
    return _writeFileNumber <fsShort_t, _CheckDefaultValidity> ( L, "writeShort" );
}
static int luafile_writeUShort( lua_State *L )
{
    return _writeFileNumber <fsUShort_t, _CheckDefaultValidity> ( L, "writeUShort" );
}
static int luafile_writeInt( lua_State *L )
{
    return _writeFileNumber <fsInt_t, _CheckDefaultValidity> ( L, "writeInt" );
}
static int luafile_writeUInt( lua_State *L )
{
    return _writeFileNumber <fsUInt_t, _CheckDefaultValidity> ( L, "writeUInt" );
}
static int luafile_writeWideInt( lua_State *L )
{
    return _writeFileNumber <fsWideInt_t, _CheckDefaultValidity> ( L, "writeWideInt" );
}
static int luafile_writeUWideInt( lua_State *L )
{
    return _writeFileNumber <fsUWideInt_t, _CheckDefaultValidity> ( L, "writeUWideInt" );
}

static int luafile_writeFloat( lua_State *L )
{
    return _writeFileNumber <fsFloat_t, _CheckDefaultValidity> ( L, "writeFloat" );
}
static int luafile_writeDouble( lua_State *L )
{
    return _writeFileNumber <fsDouble_t, _CheckDefaultValidity> ( L, "writeDouble" );
}

static int luafile_writeBoolean( lua_State *L )
{
    luaL_checktype( L, 1, LUA_TBOOLEAN );
    lua_pushnumber( L,
        ((CFile*)lua_getmethodtrans( L ))->WriteBool(
            ( lua_toboolean( L, 1 ) != 0 )
        )
    );
    return 1;
}

static int luafile_size( lua_State *L )
{
    lua_pushnumber( L, ((CFile*)lua_getmethodtrans( L ))->GetSize() );
    return 1;
}

static int luafile_stat( lua_State *L )
{
    struct stat stats;

    if ( !((CFile*)lua_getmethodtrans( L ))->Stat( &stats ) )
        return 0;

    luafile_pushStats( L, stats );
    return 1;
}

static int luafile_tell( lua_State *L )
{
    // (Attempt to) Get a large FileSystem number that stands for the current seek.
    fsOffsetNumber_t filePosition = ((CFile*)lua_getmethodtrans( L ))->TellNative();

    lua_Number num = (lua_Number)filePosition;

    lua_pushnumber( L, num );
    return 1;
}

static int luafile_seek( lua_State *L )
{
    luaL_checktype( L, 1, LUA_TNUMBER );

    int seekType;

    switch( lua_type( L, 2 ) )
    {
    case LUA_TNUMBER:
        if ( (seekType = (int)lua_tonumber( L, 2 )) < 0 || seekType > SEEK_END )
            goto defMethod;

        break;
    case LUA_TSTRING:
    {
        const char *type = lua_tostring( L, 2 );

        if ( strcmp( type, "cur" ) == 0 )
        {
            seekType = SEEK_CUR;
            break;
        }
        else if ( strcmp( type, "set" ) == 0 )
        {
            seekType = SEEK_SET;
            break;
        }
        else if ( strcmp( type, "end" ) == 0 )
        {
            seekType = SEEK_END;
            break;
        }
    }
    default:
defMethod:
        lua_pushstring( L, "unknown seekmode" );
        lua_error( L );
        return -1;
    }

    // Convert lua_Number into a large FileSystem number.
    lua_Number num = lua_tonumber( L, 1 );

    fsOffsetNumber_t seekOffset = (fsOffsetNumber_t)num;

    lua_pushnumber( L, ((CFile*)lua_getmethodtrans( L ))->SeekNative( seekOffset, seekType ) );
    return 1;
}

static int luafile_eof( lua_State *L )
{
    lua_pushboolean( L, ((CFile*)lua_getmethodtrans( L ))->IsEOF() );
    return 1;
}

static int luafile_flush( lua_State *L )
{
    ((CFile*)lua_getmethodtrans( L ))->Flush();
    lua_pushboolean( L, true );
    return 1;
}

static int luafile_seekEnd( lua_State *L )
{
    ((CFile*)lua_getmethodtrans( L ))->SetSeekEnd();
    lua_pushboolean( L, true );
    return 1;
}

static int luafile_isWritable( lua_State *L )
{
    lua_pushboolean( L, ((CFile*)lua_getmethodtrans( L ))->IsWriteable() );
    return 1;
}

static int luafile_isReadable( lua_State *L )
{
    lua_pushboolean( L, ((CFile*)lua_getmethodtrans( L ))->IsReadable() );
    return 1;
}

static int luafile_destroy( lua_State *lua )
{
    delete (CFile*)lua_touserdata( lua, lua_upvalueindex( 1 ) );

    return 0;
}

static const luaL_Reg fileInterface_sys[] =
{
#ifndef FU_CLASS
    { "__newindex", luafile_onNewindex },
#endif
    { "destroy", luafile_destroy },
#ifndef FU_CLASS
    { NULL, NULL }
};

static const luaL_Reg fileInterface[] =
{
#endif
    { "read", luafile_read },
    { "readByte", luafile_readByte },
    { "readUByte", luafile_readUByte },
    { "readShort", luafile_readShort },
    { "readUShort", luafile_readUShort },
    { "readInt", luafile_readInt },
    { "readUInt", luafile_readUInt },
    { "readWideInt", luafile_readWideInt },
    { "readUWideInt", luafile_readUWideInt },
    { "readFloat", luafile_readFloat },
    { "readDouble", luafile_readDouble },
    { "readBoolean", luafile_readBoolean },
    { "write", luafile_write },
    { "writeByte", luafile_writeByte },
    { "writeUByte", luafile_writeUByte },
    { "writeShort", luafile_writeShort },
    { "writeUShort", luafile_writeUShort },
    { "writeInt", luafile_writeInt },
    { "writeUInt", luafile_writeUInt },
    { "writeWideInt", luafile_writeWideInt },
    { "wrideUWideInt", luafile_writeUWideInt },
    { "writeFloat", luafile_writeFloat },
    { "writeDouble", luafile_writeDouble },
    { "writeBoolean", luafile_writeBoolean },
    { "size", luafile_size },
    { "stat", luafile_stat },
    { "tell", luafile_tell },
    { "seek", luafile_seek },
    { "eof", luafile_eof },
    { "flush", luafile_flush },
    { "seekEnd", luafile_seekEnd },
    { "isWritable", luafile_isWritable },
    { "isReadable", luafile_isReadable },
    { NULL, NULL }
};

int luaconstructor_file( lua_State *lua )
{
#ifndef FU_CLASS
    CFile *file = (CFile*)lua_touserdata( lua, lua_upvalueindex( 1 ) );

    // Register as file
    ILuaClass *j = lua_refclass( lua, 1 );
    j->SetTransmit( LUACLASS_FILE, file );

    lua_basicextend( lua );

    // Create the illegal access table
    lua_newtable( lua );
    lua_pushboolean( lua, false );
    lua_setfield( lua, 2, "__index" );
    lua_pushboolean( lua, false );
    lua_setfield( lua, 2, "__newindex" );

    // We need the class outer environment
    j->PushOuterEnvironment( lua );

    lua_pushcclosure( lua, luafile_onIndex, 2 );
    lua_setfield( lua, LUA_ENVIRONINDEX, "__index" );

    lua_pushvalue( lua, lua_upvalueindex( 1 ) );
    lua_setfield( lua, LUA_ENVIRONINDEX, "ioptr" );

    j->RegisterInterfaceTrans( lua, fileInterface, 0, LUACLASS_FILE );
#endif

    lua_pushvalue( lua, LUA_ENVIRONINDEX );
    lua_pushvalue( lua, lua_upvalueindex( 1 ) );
    luaL_openlib( lua, NULL, fileInterface_sys, 1 );

    lua_pushlstring( lua, "file", 4 );
    lua_setfield( lua, LUA_ENVIRONINDEX, "__type" );
    return 0;
}

void luafile_open( lua_State *lua )
{
}
