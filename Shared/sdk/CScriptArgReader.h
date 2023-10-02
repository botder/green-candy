/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        CScriptArgReader.h
*  PURPOSE:     Unified Lua script argumentation
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

// If any new class is added, please update this!
static inline const char* lua_gettransmittype( int transmit )
{
#ifdef MTA_CLIENT
    switch( transmit )
    {
    case LUACLASS_CAMERA:                       return "camera";
    case LUACLASS_CIVILIAN:                     return "civilian";
    case LUACLASS_COLSHAPE:                     return "colshape";
    case LUACLASS_DFF:                          return "clump";
    case LUACLASS_DUMMY:                        return "dummy";
    case LUACLASS_ENTITY:                       return "entity";
    case LUACLASS_SYSENTITY:                    return "sysentity";
    case LUACLASS_TEXTURE:                      return "game-texture";
    case LUACLASS_GUIELEMENT:                   return "gui-element";
    case LUACLASS_MARKER:                       return "marker";
    case LUACLASS_OBJECT:                       return "object";
    case LUACLASS_PATHNODE:                     return "pathnode";
    case LUACLASS_PED:                          return "ped";
    case LUACLASS_PICKUP:                       return "pickup";
    case LUACLASS_PLAYER:                       return "player";
    case LUACLASS_PROJECTILE:                   return "projectile";
    case LUACLASS_RADARAREA:                    return "radararea";
    case LUACLASS_RADARMARKER:                  return "blip";
    case LUACLASS_RENDERELEMENT:                return "render-element";
    case LUACLASS_SOUND:                        return "sound";
    case LUACLASS_STREAMELEMENT:                return "stream-element";
    case LUACLASS_TEAM:                         return "team";
    case LUACLASS_CORETEXTURE:                  return "texture";
    case LUACLASS_TXD:                          return "txd";
    case LUACLASS_VEHICLE:                      return "vehicle";
    case LUACLASS_WATER:                        return "water";
    case LUACLASS_SHADER:                       return "shader";
    case LUACLASS_GUIBUTTON:                    return "gui-button";
    case LUACLASS_GUICHECKBOX:                  return "gui-checkbox";
    case LUACLASS_GUICOMBOBOX:                  return "gui-combobox";
    case LUACLASS_GUIEDIT:                      return "gui-edit";
    case LUACLASS_GUIGRIDLIST:                  return "gui-gridlist";
    case LUACLASS_GUILABEL:                     return "gui-label";
    case LUACLASS_GUIMEMO:                      return "gui-memo";
    case LUACLASS_GUIPROGRESSBAR:               return "gui-progressbar";
    case LUACLASS_GUIRADIOBUTTON:               return "gui-radiobutton";
    case LUACLASS_GUISCROLLBAR:                 return "gui-scrollbar";
    case LUACLASS_GUISCROLLPANE:                return "gui-scrollpane";
    case LUACLASS_GUISTATICIMAGE:               return "gui-staticimage";
    case LUACLASS_GUITAB:                       return "gui-tab";
    case LUACLASS_GUITABPANEL:                  return "gui-tabpanel";
    case LUACLASS_GUIWINDOW:                    return "gui-window";
    }
#endif

    // Try to obtain a shared name
    return lua_getclassdesc( transmit );
}

template <class type>
inline static type* lua_readclass( lua_State *L, int idx, int transmit )
{
    if ( lua_type( L, idx ) != LUA_TCLASS )
        return NULL;

    ILuaClass& j = *lua_refclass( L, idx );

    if ( j.IsDestroyed() )
        return NULL;

    type *ptr;

    if ( !j.GetTransmit( transmit, (void*&)ptr ) )
        return NULL;

    return ptr;
}

/////////////////////////////////////////////////////////////////////////
//
// CScriptArgReader
//
//
// Attempt to simplify the reading of arguments from a script call
//
//////////////////////////////////////////////////////////////////////
class CScriptArgReader
{
public:
    CScriptArgReader ( lua_State* luaVM )
    {
        m_luaVM = luaVM;
        m_iIndex = 1;
        m_iErrorIndex = 0;
        m_bError = false;
        m_bIgnoreMismatchMatch = false;
        m_pPendingFunctionOutValue = NULL;
        m_pPendingFunctionIndex = -1;
    }

    CScriptArgReader ( void )
    {
        assert ( !IsReadFunctionPending () );
    }

    void SetIndex( int index )
    {
        m_iIndex = index;
    }

    int GetIndex( void ) const
    {
        return m_iIndex;
    }

    //
    // Read next number
    //
    template < class T >
    bool ReadNumber ( T& outValue )
    {
        if ( lua_isnumber( m_luaVM, m_iIndex ) )
        {
            outValue = static_cast < T > ( lua_tonumber ( m_luaVM, m_iIndex++ ) );
            return true;
        }

        outValue = 0;
        SetTypeError ( "number" );
        m_iIndex++;
        return false;
    }

    //
    // Read next number, using default if needed
    //
    template < class T, class U >
    bool ReadNumber ( T& outValue, const U& defaultValue )
    {
        int iArgument = lua_type ( m_luaVM, m_iIndex );
        if ( iArgument == LUA_TNUMBER || iArgument == LUA_TSTRING )
        {
            outValue = static_cast < T > ( lua_tonumber ( m_luaVM, m_iIndex++ ) );
            return true;
        }
        else
        if ( iArgument == LUA_TNONE || iArgument == LUA_TNIL || m_bIgnoreMismatchMatch )
        {
            outValue = static_cast < T > ( defaultValue );
            return false;
        }

        outValue = 0;
        SetTypeError ( "number" );
        m_iIndex++;
        return false;
    }



    //
    // Read next bool
    //
    bool ReadBool ( bool& bOutValue )
    {
        int iArgument = lua_type ( m_luaVM, m_iIndex );
        if ( iArgument == LUA_TBOOLEAN )
        {
            bOutValue = lua_toboolean ( m_luaVM, m_iIndex++ ) ? true : false;
            return true;
        }

        bOutValue = false;
        SetTypeError ( "bool" );
        m_iIndex++;
        return false;
    }

    //
    // Read next bool, using default if needed
    //
    bool ReadBool ( bool& bOutValue, const bool bDefaultValue )
    {
        int iArgument = lua_type ( m_luaVM, m_iIndex );
        if ( iArgument == LUA_TBOOLEAN )
        {
            bOutValue = lua_toboolean ( m_luaVM, m_iIndex++ ) ? true : false;
            return true;
        }
        else
        if ( iArgument == LUA_TNONE || iArgument == LUA_TNIL || m_bIgnoreMismatchMatch )
        {
            bOutValue = bDefaultValue;
            return false;
        }

        bOutValue = false;
        SetTypeError ( "bool" );
        m_iIndex++;
        return false;
    }


    //
    // Read next string, using default if needed
    //
    template <class stringType>
    bool ReadString( stringType& outValue, const char* defaultValue = NULL )
    {
        if ( lua_isstring( m_luaVM, m_iIndex ) )
        {
            outValue = lua_tostring ( m_luaVM, m_iIndex++ );
            return true;
        }
        else if ( lua_isnil( m_luaVM, m_iIndex ) || m_bIgnoreMismatchMatch )
        {
            if ( defaultValue )
            {
                outValue = defaultValue;
                return false;
            }
        }

        outValue = "";
        SetTypeError ( "string" );
        m_iIndex++;
        return false;
    }


    //
    // Read next string as an enum
    //
    template < class T >
    bool ReadEnumString ( T& outValue )
    {
        int iArgument = lua_type ( m_luaVM, m_iIndex );
        if ( iArgument == LUA_TSTRING )
        {
            SString strValue = lua_tostring ( m_luaVM, m_iIndex );
            if ( StringToEnum ( strValue, outValue ) )
            {
                m_iIndex++;
                return true;
            }
        }

        outValue = (T)0;
        SetTypeError ( GetEnumTypeName ( outValue ) );
        m_iIndex++;
        return false;
    }


    //
    // Read next string as an enum, using default if needed
    //
    template < class T >
    bool ReadEnumString ( T& outValue, const T& defaultValue )
    {
        int iArgument = lua_type ( m_luaVM, m_iIndex );
        if ( iArgument == LUA_TSTRING )
        {
            SString strValue = lua_tostring ( m_luaVM, m_iIndex );
            if ( StringToEnum ( strValue, outValue ) )
            {
                m_iIndex++;
                return true;
            }
        }
        else
        if ( iArgument == LUA_TNONE || iArgument == LUA_TNIL || m_bIgnoreMismatchMatch )
        {
            outValue = defaultValue;
            return false;
        }

        outValue = (T)0;
        SetTypeError ( GetEnumTypeName ( outValue ) );
        m_iIndex++;
        return false;
    }

    // Read MTA:Lua type
    template <class T>
    bool ReadClass( T*& outValue, unsigned int transmit )
    {
        if ( lua_type( m_luaVM, m_iIndex ) != LUA_TCLASS )
            goto fail;

        {
            ILuaClass& j = *lua_refclass( m_luaVM, m_iIndex );

            if ( j.IsDestroyed() || !j.GetTransmit( transmit, (void*&)outValue ) )
                goto fail;

            m_iIndex++;
        }
        return true;

fail:
        outValue = NULL;
        SetTypeError( lua_gettransmittype( transmit ) );
        m_iIndex++;
        return false;
    }

    // Read MTA:Lua type while accepting default
    template <class T>
    void ReadClass( T*& outValue, unsigned int transmit, T *def )
    {
        if ( lua_type( m_luaVM, m_iIndex ) != LUA_TCLASS )
            goto pass;

        {
            ILuaClass& j = *lua_refclass( m_luaVM, m_iIndex );

            if ( j.IsDestroyed() || !j.GetTransmit( transmit, (void*&)outValue ) )
                goto pass;

            m_iIndex++;
            return;
        }
pass:
        outValue = def;
        m_iIndex++;
    }

    // Read MTA:Lua type while accepting default
    template <class T>
    void ReadClass( T*& outValue, unsigned int transmit, int )
    {
        if ( lua_type( m_luaVM, m_iIndex ) != LUA_TCLASS )
            goto pass;

        {
            ILuaClass& j = *lua_refclass( m_luaVM, m_iIndex );

            if ( j.IsDestroyed() || !j.GetTransmit( transmit, (void*&)outValue ) )
                goto pass;

            m_iIndex++;
            return;
        }
pass:
        outValue = NULL;
        m_iIndex++;
    }

    // Vector unit for easy dispatch and performance
    inline bool ReadVector( CVector& outValue )
    {
        return ReadNumber( outValue.fX ) && ReadNumber( outValue.fY ) && ReadNumber( outValue.fZ );
    }

    inline bool ReadVector( CVector& outValue, const CVector& defValue )
    {
        return ReadNumber( outValue.fX, defValue.fX ) && ReadNumber( outValue.fY, defValue.fY ) && ReadNumber( outValue.fZ, defValue.fZ );
    }

    // Reads a color unit by unsigned char
    bool ReadColor( unsigned char& outValue )
    {
        double color;

        if ( !ReadNumber( color ) )
            return false;

        if ( color < 0 || color > 255 )
            return false;

        outValue = (unsigned char)color;
        return true;
    }

    // Reads a color unit by unsigned char with default value
    bool ReadColor( unsigned char& outValue, double defValue )
    {
        double color;
        ReadNumber( color, defValue );

        if ( color < 0 || color > 255 )
            return false;

        outValue = (unsigned char)color;
        return true;
    }

#if 0
    //
    // Read next userdata, using default if needed
    //
    template < class T >
    bool ReadUserData ( T*& outValue, T* defaultValue, bool bArgCanBeNil = false, bool bDefaultCanBeNil = false )
    {
        int iArgument = lua_type ( m_luaVM, m_iIndex );

        if ( iArgument == LUA_TLIGHTUSERDATA )
        {
            outValue = (T*)UserDataCast < T > ( (T*)0, lua_touserdata ( m_luaVM, m_iIndex++ ), m_luaVM );
            if ( outValue || bArgCanBeNil )
                return true;

            outValue = NULL;
            SetTypeError ( GetClassTypeName ( (T*)0 ), m_iIndex - 1 );
            return false;
        }
        else
        if ( iArgument == LUA_TNONE || m_bIgnoreMismatchMatch || ( iArgument == LUA_TNIL && bArgCanBeNil ) )
        {
            if ( defaultValue != (T*)-1 )
            {
                outValue = defaultValue;
                if ( outValue || bDefaultCanBeNil )
                    return false;
            }
        }

        outValue = NULL;
        SetTypeError ( GetClassTypeName ( (T*)0 ) );
        m_iIndex++;
        return false;
    }


    //
    // Read next userdata, using NULL default or no default
    //
    template < class T >
    bool ReadUserData ( T*& outValue, int defaultValue = -1 )
    {
        return ReadUserData ( outValue, (T*)defaultValue, defaultValue == 0, true );
    }


    //
    // Read next wrapped userdata
    //
    template < class T, class U >
    bool ReadUserData ( U*& outValue )
    {
        if ( ReadUserData ( outValue ) )
        {
            SString strErrorExpectedType;
            if ( CheckWrappedUserDataType < T > ( outValue, strErrorExpectedType ) )
                return true;
            SetTypeError ( strErrorExpectedType, m_iIndex - 1 );
        }
        return false;
    }
#endif //0

    //
    // Read a function, but don't do it yet due to Lua stack issues
    //
    bool ReadFunction ( LuaFunctionRef& outValue, int defaultValue = -2 )
    {
        int iArgument = lua_type ( m_luaVM, m_iIndex );
        if ( iArgument == LUA_TFUNCTION )
        {
            m_pPendingFunctionOutValue = &outValue;
            m_pPendingFunctionIndex = m_iIndex++;
            return true;
        }
        else if ( defaultValue == LUA_REFNIL )
        {
            outValue = LuaFunctionRef();
            return true;
        }

        SetTypeError ( "function", m_iIndex );
        m_iIndex++;
        return false;
    }

    //
    // Call after other arguments have been read
    //
    bool ReadFunctionComplete()
    {
        if ( !m_pPendingFunctionOutValue )
            return true;

        *m_pPendingFunctionOutValue = lua_readuserdata_assert <CLuaMain, LUA_STORAGEINDEX, 2> ( m_luaVM )->CreateReference( m_luaVM, m_pPendingFunctionIndex );
        if ( VERIFY_FUNCTION( *m_pPendingFunctionOutValue ) )
        {
            m_pPendingFunctionIndex = -1;
            return true;
        }

        SetTypeError ( "function", m_pPendingFunctionIndex );
        m_pPendingFunctionIndex = -1;
        return false;
    }

    // Debug check
    bool IsReadFunctionPending ( void ) const
    {
        return m_pPendingFunctionOutValue && m_pPendingFunctionIndex != -1;
    }


    //
    // Peek at next type
    //
    bool NextIs ( int iArgument, int iOffset = 0 ) const  { return iArgument == lua_type ( m_luaVM, m_iIndex + iOffset ); }
    bool NextIsNone         ( int iOffset = 0 ) const  { return NextIs ( LUA_TNONE, iOffset ); }
    bool NextIsNil          ( int iOffset = 0 ) const  { return NextIs ( LUA_TNIL, iOffset ); }
    bool NextIsBool         ( int iOffset = 0 ) const  { return NextIs ( LUA_TBOOLEAN, iOffset ); }
    bool NextIsUserData     ( int iOffset = 0 ) const  { return NextIs ( LUA_TLIGHTUSERDATA, iOffset ); }
    bool NextIsClass        ( int iOffset = 0 ) const  { return NextIs ( LUA_TCLASS, iOffset ); }
    bool NextIsNumber       ( int iOffset = 0 ) const  { return NextIs ( LUA_TNUMBER, iOffset ); }
    bool NextIsString       ( int iOffset = 0 ) const  { return NextIs ( LUA_TSTRING, iOffset ); }
    bool NextIsTable        ( int iOffset = 0 ) const  { return NextIs ( LUA_TTABLE, iOffset ); }
    bool NextIsFunction     ( int iOffset = 0 ) const  { return NextIs ( LUA_TFUNCTION, iOffset ); }
    bool NextCouldBeNumber  ( int iOffset = 0 ) const  { return NextIsNumber ( iOffset ) || NextIsString ( iOffset ); }
    bool NextCouldBeString  ( int iOffset = 0 ) const  { return NextIsNumber ( iOffset ) || NextIsString ( iOffset ); }

    template < class T >
    bool NextIsEnumString ( T&, int iOffset = 0 )
    {
        int iArgument = lua_type ( m_luaVM, m_iIndex + iOffset );
        if ( iArgument == LUA_TSTRING )
        {
            T eDummyResult;
            SString strValue = lua_tostring ( m_luaVM, m_iIndex + iOffset );
            return StringToEnum ( strValue, eDummyResult );
        }
        return false;
    }

    //
    // SetTypeError
    //
    void SetTypeError ( const SString& strExpectedType, int iIndex = -1 )
    {
        if ( iIndex == - 1 )
            iIndex = m_iIndex;
        if ( !m_bError || iIndex <= m_iErrorIndex )
        {
            m_bError = true;
            m_iErrorIndex = iIndex;
            m_strErrorExpectedType = strExpectedType;
        }
    }

    //
    // HasErrors - Optional check if there are any unread arguments
    //
    bool HasErrors ( bool bCheckUnusedArgs = false ) const
    {
        assert ( !IsReadFunctionPending () );
        if ( bCheckUnusedArgs && lua_type ( m_luaVM, m_iIndex ) != LUA_TNONE )
            return true;
        return m_bError;
    }

    //
    // GetErrorMessage
    //
    SString GetErrorMessage() const
    {
        if ( !m_bError )
            return "No error";

        int             iGotArgumentType    = lua_type ( m_luaVM, m_iErrorIndex );
        SString         strGotArgumentType  = lua_typename( m_luaVM, iGotArgumentType );
        SString         strGotArgumentValue = lua_tostring ( m_luaVM, m_iErrorIndex );

        // Compose error message
        SString strMessage ( "Expected %s at argument %d", *m_strErrorExpectedType, m_iErrorIndex );

        if ( iGotArgumentType == LUA_TCLASS )
        {
            lua_pushtype( m_luaVM, m_iErrorIndex );
            strGotArgumentType = lua_getstring( m_luaVM, -1 );
            lua_pop( m_luaVM, 1 );
        }

        if ( !strGotArgumentType.empty () )
        {
            strMessage += SString ( ", got %s", *strGotArgumentType );

            // Append value if available
            if ( !strGotArgumentValue.empty () )
                strMessage += SString ( " '%s'", *strGotArgumentValue );
        }

        return strMessage;
    }

    //
    // Strict off means mismatches are ignored if they have a default value
    //
    void SetStrict ( bool bStrictMode )
    {
        m_bIgnoreMismatchMatch = !bStrictMode;
    }


    bool                    m_bIgnoreMismatchMatch;
    bool                    m_bError;
    int                     m_iErrorIndex;
    SString                 m_strErrorExpectedType;
    int                     m_iIndex;
    lua_State*              m_luaVM;
    LuaFunctionRef*         m_pPendingFunctionOutValue;
    int                     m_pPendingFunctionIndex;
};
