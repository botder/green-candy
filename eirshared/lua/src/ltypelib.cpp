// Lua typesystem utilities library for debugging.
#include "luacore.h"

#include "lobject.h"
#include "lstate.h"
#include "lapi.h"
#include "lauxlib.h"

#include "lualib.h"

static int typelib_iscollectable( lua_State *L )
{
    ConstValueAddress firstArgValue = index2constadr( L, 1 );

    lua_pushboolean( L, iscollectable( firstArgValue ) );
    return 1;
}

template <typename callbackType>
inline void IterateTypeInfoInheritance( LuaTypeSystem::typeInfoBase *subclassType, callbackType& cb )
{
    LuaTypeSystem::typeInfoBase *inheritsFrom = subclassType->inheritsFrom;

    if ( inheritsFrom )
    {
        IterateTypeInfoInheritance( inheritsFrom, cb );
    }

    // Execute the callback.
    cb.OnEntry( subclassType );
}

static inline void ConstructTypeHierarchy( LuaTypeSystem::typeInfoBase *subclassType, std::string& typeHier )
{
    // Give the concatenation iterator required meta-data.
    struct concatenation_iterator
    {
        std::string& typeHier;
        LuaTypeSystem::typeInfoBase *subclassType;

        inline concatenation_iterator( std::string& typeHier ) : typeHier( typeHier )
        {
            return;
        }

        inline void OnEntry( LuaTypeSystem::typeInfoBase *iter )
        {
            std::string concat_item = iter->name;

            // If its not the ending type, append the separator.
            if ( iter != this->subclassType )
            {
                concat_item += "::";
            }

            this->typeHier += concat_item;
        }
    };
    concatenation_iterator concat_iter( typeHier );

    concat_iter.subclassType = subclassType;

    // Iterate over the types.
    IterateTypeInfoInheritance( subclassType, concat_iter );
}

static int typelib_hierarchy( lua_State *L )
{
    // Check whether we have a Lua type that is collectable.
    ConstValueAddress firstArgValue = index2constadr( L, 1 );

    if ( !iscollectable( firstArgValue ) )
        throw lua_exception( L, LUA_ERRRUN, "expected gcobj as first argument" );

    GCObject *obj = gcvalue( firstArgValue );

    // Construct a type hierarchy.
    std::string typeHier;
    {
        LuaRTTI *rtObj = LuaTypeSystem::GetTypeStructFromObject( obj );

        if ( rtObj )
        {
            LuaTypeSystem::typeInfoBase *subclassType = LuaTypeSystem::GetTypeInfoFromTypeStruct( rtObj );

            ConstructTypeHierarchy( subclassType, typeHier );
        }
    }
    lua_pushlstring( L, typeHier.c_str(), typeHier.size() );
    return 1;
}

static int typelib_rttype( lua_State *L )
{
    ConstValueAddress firstObjValue = index2constadr( L, 1 );

    int valType = ttype( firstObjValue );

    if ( valType == LUA_TNIL )
    {
        lua_pushcstring( L, "nil" );
    }
    else if ( valType == LUA_TNUMBER )
    {
        lua_pushcstring( L, "number" );
    }
    else if ( valType == LUA_TBOOLEAN )
    {
        lua_pushcstring( L, "boolean" );
    }
    else if ( valType == LUA_TLIGHTUSERDATA )
    {
        lua_pushcstring( L, "pointer" );
    }
    else if ( iscollectable( firstObjValue ) )
    {
        GCObject *gcobj = gcvalue( firstObjValue );

        struct rttype_iterator
        {
            lua_config *config;
            const char *typeName;

            inline void OnEntry( LuaTypeSystem::typeInfoBase *iter )
            {
                if ( this->typeName != nullptr )
                    return;

                if ( iter == this->config->gcobjTypeInfo ||
                     iter == this->config->grayobjTypeInfo )
                {
                    return;
                }

                this->typeName = iter->name;
            }
        };

        rttype_iterator iter;
        iter.config = G(L)->config;
        iter.typeName = nullptr;

        {
            LuaRTTI *rtObj = LuaTypeSystem::GetTypeStructFromObject( gcobj );

            if ( rtObj )
            {
                LuaTypeSystem::typeInfoBase *subclassType = LuaTypeSystem::GetTypeInfoFromTypeStruct( rtObj );

                // Iterate over the types.
                IterateTypeInfoInheritance( subclassType, iter );
            }
        }

        if ( const char *typeName = iter.typeName )
        {
            lua_pushstring( L, typeName );
        }
        else
        {
            lua_pushboolean( L, false );
        }
    }
    else
    {
        lua_pushboolean( L, false );
    }

    return 1;
}

static int typelib_regtypes( lua_State *L )
{
    lua_newtable( L );

    int n = 1;

    LuaTypeSystem& typeSys = G(L)->config->typeSys;

    // Iterate through all the types in the type system and
    // add the hierarchy qualifier for each that is an end type.
    for ( LuaTypeSystem::type_iterator iter = typeSys.GetTypeIterator(); !iter.IsEnd(); iter.Increment() )
    {
        LuaTypeSystem::typeInfoBase *typeInfo = iter.Resolve();

        if ( typeInfo->IsEndType() )
        {
            std::string typeHier;

            ConstructTypeHierarchy( typeInfo, typeHier );

            lua_pushlstring( L, typeHier.c_str(), typeHier.size() );
            lua_rawseti( L, -2, n++ );
        }
    }

    return 1;
}

static int typelib_create( lua_State *L )
{
    const char *typeName = lua_tostring( L, 1 );

    if ( !typeName )
    {
        throw lua_exception( L, LUA_ERRRUN, "no typename provided for type creation", 1 );
    }

    global_State *g = G(L);

    LuaTypeSystem& typeSys = g->config->typeSys;

    LuaTypeSystem::typeInfoBase *regTypeInfo = typeSys.ResolveTypeInfo( typeName, nullptr );

    bool hasPushedObject = false;

    if ( regTypeInfo )
    {
        // Prohibit construction of exclusive types.
        if ( typeSys.IsTypeInfoExclusive( regTypeInfo ) == false )
        {
            // It must inherit from GCObject. Otherwise we cannot pass it to the runtime.
            if ( typeSys.IsTypeInheritingFrom( g->config->gcobjTypeInfo, regTypeInfo ) )
            {
                // Create it and push it onto the stack.
                GCObject *luaObject = lua_new <GCObject> ( L, regTypeInfo );

                if ( luaObject )
                {
                    pushgcvalue( L, luaObject );

                    // Since the object is now on the stack, we can dereference it.
                    luaObject->DereferenceGC( L );

                    hasPushedObject = true;
                }
            }
        }
    }

    if ( !hasPushedObject )
    {
        pushbvalue( L, false );
    }

    return 1;
}

static int typelib_clone( lua_State *L )
{
    // Clone any given GC object.
    ConstValueAddress givenObj = index2constadr( L, 1 );

    if ( !iscollectable( givenObj ) )
    {
        throw lua_exception( L, LUA_ERRRUN, "first argument to cloning prototype is not an object", 1 );
    }

    global_State *g = G(L);

    LuaTypeSystem& typeSys = g->config->typeSys;

    GCObject *gcObj = gcvalue( givenObj );

    GCObject *newObj = nullptr;
    {
        const LuaRTTI *rtSrcObj = LuaTypeSystem::GetTypeStructFromObject( gcObj );

        LuaRTTI *rtClonedObj = typeSys.Clone( g, rtSrcObj );

        if ( rtClonedObj )
        {
            newObj = (GCObject*)LuaTypeSystem::GetObjectFromTypeStruct( rtClonedObj );
        }
    }

    if ( newObj )
    {
        pushgcvalue( L, newObj );

        // Since the object is now on the stack, we can dereference it.
        newObj->DereferenceGC( L );
    }
    else
    {
        pushbvalue( L, false );
    }

    return 1;
}

static int typelib_destroy( lua_State *L )
{
    // TODO.
    pushbvalue( L, false );
    return 1;
}

static int typelib_isexclusive( lua_State *L )
{
    const char *typeName = lua_tostring( L, 1 );

    if ( !typeName )
    {
        throw lua_exception( L, LUA_ERRRUN, "no typename provided", 1 );
    }

    global_State *g = G(L);

    LuaTypeSystem& typeSys = g->config->typeSys;

    LuaTypeSystem::typeInfoBase *regTypeInfo = typeSys.ResolveTypeInfo( typeName, nullptr );

    bool hasPushedValue = false;

    if ( regTypeInfo )
    {
        bool isExclusive = typeSys.IsTypeInfoExclusive( regTypeInfo );

        pushbvalue( L, isExclusive );

        hasPushedValue = true;
    }

    if ( !hasPushedValue )
    {
        pushbvalue( L, false );
    }

    return 1;
}

static int typelib_isabstract( lua_State *L )
{
    const char *typeName = lua_tostring( L, 1 );

    if ( !typeName )
    {
        throw lua_exception( L, LUA_ERRRUN, "no typename provided", 1 );
    }

    global_State *g = G(L);

    LuaTypeSystem& typeSys = g->config->typeSys;

    LuaTypeSystem::typeInfoBase *regTypeInfo = typeSys.ResolveTypeInfo( typeName, nullptr );

    bool hasPushedValue = false;

    if ( regTypeInfo )
    {
        bool isAbstract = typeSys.IsTypeInfoAbstract( regTypeInfo );

        pushbvalue( L, isAbstract );

        hasPushedValue = true;
    }

    if ( !hasPushedValue )
    {
        pushbvalue( L, false );
    }

    return 1;
}

static const luaL_Reg _typelibFuncs[] =
{
    { "iscollectable", typelib_iscollectable },
    { "hierarchy", typelib_hierarchy },
    { "rttype", typelib_rttype },
    { "regtypes", typelib_regtypes },
    { "create", typelib_create },
    { "clone", typelib_clone },
    { "destroy", typelib_destroy },
    { "isexclusive", typelib_isexclusive },
    { "isabstract", typelib_isabstract },
    { nullptr, nullptr }
};

LUALIB_API int luaopen_typelib( lua_State *L )
{
    luaL_register( L, LUA_TYPELIBNAME, _typelibFuncs );
    return 0;
}

void* luaTL_checktype( lua_State *L, int stackidx, LuaTypeSystem::typeInfoBase *typeInfo )
{
    global_State *g = G(L);

    lua_config *cfg = g->config;

    ConstValueAddress o = index2constadr( L, stackidx );

    if ( iscollectable( o ) == false )
    {
        luaO_pushfstring( L, "argument %d is not an object (%s expected)", stackidx, typeInfo->name );
        lua_error( L );
    }
    else
    {
        GCObject *gcObj = gcvalue(o);

        // Check the native type info.
        {
            LuaRTTI *rtObj = LuaTypeSystem::GetTypeStructFromObject( gcObj );

            LuaTypeSystem::typeInfoBase *objTypeInfo = LuaTypeSystem::GetTypeInfoFromTypeStruct( rtObj );

            // If the object type info inherits from the given type info, the cast is valid.
            if ( cfg->typeSys.IsTypeInheritingFrom( typeInfo, objTypeInfo ) )
            {
                // Alright, the object is valid.
                return gcObj;
            }
        }
    }

    // We have not found a valid object link.
    luaO_pushfstring( L, "argument %d does not inherit from %s", stackidx, typeInfo->name );
    lua_error( L );

    // Return NULL just to make the compiler happy.
    return nullptr;
}
