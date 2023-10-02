/*****************************************************************************
*
*  PROJECT:     Eir FileSystem for MTA:BLUE
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        luafile.cpp
*  PURPOSE:     Lua implementation of the FileSystem CFile class
*
*  For documentation visit http://wiki.mtasa.com/wiki/MTA:Eir/FileSystem/
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include "luafile.Utils.hxx"

#include <cstring>

static int luafile_read( lua_State *L )
{
    CFile *file = fsLuaGetFile( L, 1 );
    luaL_checktype( L, 2, LUA_TNUMBER );

    long byteCount = (long)lua_tonumber( L, 2 );

    LUA_CHECK( byteCount >= 0 );

    size_t bytesRead = (size_t)byteCount;

    if ( bytesRead == 0 )
    {
        lua_pushlstring( L, "", 0 );
        return 1;
    }

    eir::Vector <char, FileSysCommonAllocator> buf;

    try
    {
        buf.Resize( bytesRead );
    }
    catch( eir::out_of_memory_exception& )
    {
        lua_pushboolean( L, false );
        return 1;
    }

    LUAFILE_GUARDFSCALL_BEGIN
        bytesRead = file->Read( &buf[0], bytesRead );
    LUAFILE_GUARDFSCALL_END

    if ( bytesRead == 0 )
    {
        lua_pushlstring( L, "", 0 );
        return 1;
    }

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
    CFile *file = fsLuaGetFile( L, 1 );

    numberType out_num;

    bool success;

    LUAFILE_GUARDFSCALL_BEGIN
        success = file->ReadStruct( out_num );
    LUAFILE_GUARDFSCALL_END

    LUA_CHECK( success );

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
    CFile *file = fsLuaGetFile( L, 1 );

    bool out_b;

    bool successful;

    LUAFILE_GUARDFSCALL_BEGIN
        successful = file->ReadBool( out_b );
    LUAFILE_GUARDFSCALL_END

    if ( !successful )
        lua_pushnil( L );
    else
        lua_pushboolean( L, out_b );

    return 1;
}

template <typename numberType, typename validityChecker>
AINLINE int _writeFileNumber( lua_State *L, const char *methodName )
{
    CFile *file = fsLuaGetFile( L, 1 );
    luaL_checktype( L, 2, LUA_TNUMBER );

    lua_Number number = lua_tonumber( L, 2 );

    // Check validity of number.
    {
        validityChecker checker;

        if ( !checker.template IsNumberValid <numberType> ( number ) )
        {
            // todo: print a warning.
        }
    }

    numberType realNum = (numberType)number;

    size_t numWrite;

    LUAFILE_GUARDFSCALL_BEGIN
        numWrite = file->WriteStruct( realNum );
    LUAFILE_GUARDFSCALL_END

    lua_pushnumber( L, numWrite );
    return 1;
}

static int luafile_write( lua_State *L )
{
    CFile *file = fsLuaGetFile( L, 1 );
    luaL_checktype( L, 2, LUA_TSTRING );

    size_t len;
    const char *string = lua_tolstring( L, 2, &len );

    size_t numWrite;

    LUAFILE_GUARDFSCALL_BEGIN
        numWrite = file->Write( string, len );
    LUAFILE_GUARDFSCALL_END

    lua_pushnumber( L, numWrite );
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
    CFile *file = fsLuaGetFile( L, 1 );
    luaL_checktype( L, 2, LUA_TBOOLEAN );

    size_t numWrite;

    LUAFILE_GUARDFSCALL_BEGIN
        numWrite = file->WriteBool(
            ( lua_toboolean( L, 2 ) != 0 )
        );
    LUAFILE_GUARDFSCALL_END

    lua_pushnumber( L, numWrite );
    return 1;
}

static int luafile_size( lua_State *L )
{
    CFile *file = fsLuaGetFile( L, 1 );

    size_t fileSize;

    LUAFILE_GUARDFSCALL_BEGIN
        fileSize = file->GetSize();
    LUAFILE_GUARDFSCALL_END

    lua_pushnumber( L, fileSize );
    return 1;
}

static int luafile_stat( lua_State *L )
{
    CFile *file = fsLuaGetFile( L, 1 );

    filesysStats stats;

    LUAFILE_GUARDFSCALL_BEGIN
        if ( !file->QueryStats( stats ) )
            return 0;

        fsOffsetNumber_t fileSize = file->GetSizeNative();

        luafile_pushStats( L, fileSize, stats );
        return 1;
    LUAFILE_GUARDFSCALL_END
}

static int luafile_tell( lua_State *L )
{
    CFile *file = fsLuaGetFile( L, 1 );

    // (Attempt to) Get a large FileSystem number that stands for the current seek.
    fsOffsetNumber_t filePosition;

    LUAFILE_GUARDFSCALL_BEGIN
        filePosition = file->TellNative();
    LUAFILE_GUARDFSCALL_END

    lua_Number num = (lua_Number)filePosition;

    lua_pushnumber( L, num );
    return 1;
}

static int luafile_seek( lua_State *L )
{
    CFile *file = fsLuaGetFile( L, 1 );
    luaL_checktype( L, 2, LUA_TNUMBER );

    int seekType;

    switch( lua_type( L, 3 ) )
    {
    case LUA_TNUMBER:
        if ( (seekType = (int)lua_tonumber( L, 3 )) < 0 || seekType > SEEK_END )
            goto defMethod;

        break;
    case LUA_TSTRING:
    {
        const char *type = lua_tostring( L, 3 );

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
    lua_Number num = lua_tonumber( L, 2 );

    fsOffsetNumber_t seekOffset = (fsOffsetNumber_t)num;

    int seekReturn;

    LUAFILE_GUARDFSCALL_BEGIN
        seekReturn = file->SeekNative( seekOffset, seekType );
    LUAFILE_GUARDFSCALL_END

    lua_pushnumber( L, seekReturn );
    return 1;
}

static int luafile_eof( lua_State *L )
{
    CFile *file = fsLuaGetFile( L, 1 );

    bool is_eof;

    LUAFILE_GUARDFSCALL_BEGIN
        is_eof = file->IsEOF();
    LUAFILE_GUARDFSCALL_END

    lua_pushboolean( L, is_eof );
    return 1;
}

static int luafile_flush( lua_State *L )
{
    CFile *file = fsLuaGetFile( L, 1 );

    LUAFILE_GUARDFSCALL_BEGIN
        file->Flush();
    LUAFILE_GUARDFSCALL_END

    lua_pushboolean( L, true );
    return 1;
}

static int luafile_seekEnd( lua_State *L )
{
    CFile *file = fsLuaGetFile( L, 1 );

    LUAFILE_GUARDFSCALL_BEGIN
        file->SetSeekEnd();
    LUAFILE_GUARDFSCALL_END

    lua_pushboolean( L, true );
    return 1;
}

static int luafile_isWritable( lua_State *L )
{
    CFile *file = fsLuaGetFile( L, 1 );
    lua_pushboolean( L, file->IsWriteable() );
    return 1;
}

static int luafile_isReadable( lua_State *L )
{
    CFile *file = fsLuaGetFile( L, 1 );
    lua_pushboolean( L, file->IsReadable() );
    return 1;
}

static const luaL_Reg file_methods[] =
{
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
    { nullptr, nullptr }
};

void luafile_makemeta( lua_State *L, int ridx )
{
    lua_newtable( L );
    lua_pushvalue( L, ridx - 1 );
    luaL_openlib( L, nullptr, file_methods, 1 );
}
