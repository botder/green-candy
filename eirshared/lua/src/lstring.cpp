/*
** $Id: lstring.c,v 2.8.1.1 2007/12/27 13:02:25 roberto Exp $
** String table (keeps all strings handled by Lua)
** See Copyright Notice in lua.h
*/


#include "luacore.h"

#include "lmem.h"
#include "lobject.h"
#include "lstate.h"
#include "lstring.h"
#include "ldebug.h"
#include "lvm.h"

#include "lpluginutil.hxx"


struct stringConstructionParams
{
    size_t length;
    unsigned int hash;
    lua_State *allocThread;
};

// String type information.
struct stringTypeInfo_t
{
    struct stringTypeMetaInfo : public LuaTypeSystem::structTypeMetaInfo
    {
        size_t GetTypeSize( global_State *g, void *construction_params ) const override
        {
            stringConstructionParams *params = (stringConstructionParams*)construction_params;

            size_t length = 0;

            if ( params )
            {
                length = params->length;
            }

            return _sizestring( sizeof( TString ), length );
        }

        size_t GetTypeSizeByObject( global_State *g, const void *langObj ) const override
        {
            TString *strObj = (TString*)langObj;

            return _sizestring( sizeof( TString ), strObj->len );
        }
    };

    struct udataTypeMetaInfo : public LuaTypeSystem::structTypeMetaInfo
    {
        size_t GetTypeSize( global_State *g, void *construction_params ) const override
        {
            stringConstructionParams *params = (stringConstructionParams*)construction_params;

            size_t length = 0;

            if ( params )
            {
                length = params->length;
            }

            return _sizeudata( sizeof( Udata ), length );
        }

        size_t GetTypeSizeByObject( global_State *g, const void *langObj ) const
        {
            Udata *u = (Udata*)langObj;

            return _sizeudata( sizeof( Udata ), u->len );
        }
    };

    stringTypeMetaInfo _stringMetaInfo;
    udataTypeMetaInfo _udataMetaInfo;

    inline void Initialize( lua_config *cfg )
    {
        LuaTypeSystem& typeSys = cfg->typeSys;

        // Register the string type.
        stringTypeInfo = typeSys.RegisterDynamicStructType <TString> ( "string", &_stringMetaInfo, false, cfg->gcobjTypeInfo );
        udataTypeInfo = typeSys.RegisterDynamicStructType <Udata> ( "udata", &_udataMetaInfo, false, cfg->gcobjTypeInfo );

        // Set up properties.
        typeSys.SetTypeInfoExclusive( stringTypeInfo, true );
    }

    inline void Shutdown( lua_config *cfg )
    {
        LuaTypeSystem& typeSys = cfg->typeSys;

        // Delete all types.
        typeSys.DeleteType( stringTypeInfo );
    }

    LuaTypeSystem::typeInfoBase *stringTypeInfo;
    LuaTypeSystem::typeInfoBase *udataTypeInfo;
};

PluginDependantStructRegister <stringTypeInfo_t, namespaceFactory_t> stringTypeInfo;

// Global state string environment.
struct globalStateStringEnv
{
    Mbuffer buff;  /* temporary buffer for string concatentation */
};

typedef PluginConnectingBridge
    <globalStateStringEnv,
        globalStateStructFactoryMeta <globalStateStringEnv, globalStateFactory_t, lua_config>,
    namespaceFactory_t>
        stringEnvConnectingBridge_t;

stringEnvConnectingBridge_t stringEnvConnectingBridge;

globalStateStringEnv* GetGlobalStringEnv( global_State *g )
{
    return stringEnvConnectingBridge.GetPluginStruct( g->config, g );
}

void luaS_resize (lua_State *L, int newsize)
{
    gcObjList_t *newhash;
    stringtable *tb;

    if (luaC_getstate( L ) == GCSsweepstring)
    {
        /* cannot resize during GC traverse, so lock until we have left it */
        luaC_finish( L );
    }

    newhash = luaM_newvector <gcObjList_t> (L, newsize);
    tb = &G(L)->strt;

    /* rehash */
    for (int i = 0; i < tb->size; i++)
    {
        gcObjList_t::iterator p = tb->hash[i].GetIterator();

        while ( !p.IsEnd() )
        {  /* for each node in the list */
            GCObject *curr = (GCObject*)p.Resolve();

            p.Increment();

            unsigned int h = gco2ts(curr)->hash;
            int h1 = lmod(h, newsize);  /* new position */

            lua_assert( cast_int( h % newsize ) == lmod( h, newsize ) );

            newhash[h1].Insert( curr );     /* chain it */
        }

        // Make sure we clear the list before deleting it.
        tb->hash[i].Clear();
    }

    luaM_freearray(L, tb->hash, tb->size);
    tb->size = newsize;
    tb->hash = newhash;
}

TString::TString( global_State *g, void *construction_params ) : GCObject( g )
{
    stringConstructionParams *params = (stringConstructionParams*)construction_params;

    size_t strLength = 0;
    unsigned int strHash = 0;

    lua_State *L = g->mainthread;

    if ( params )
    {
        strLength = params->length;
        strHash = params->hash;
        L = params->allocThread;
    }

    this->reserved = 0;
    this->hash = strHash;
    this->len = strLength;

    luaC_register( g, this, LUA_TSTRING );

    unsigned int h = strHash;

    // Store this string in the global string table.
    stringtable *tb = &g->strt;
    h = lmod(h, tb->size);
    tb->hash[h].Insert( this );  /* chain new entry */
    tb->nuse++;

    try
    {
        if (tb->nuse > cast(lu_int32, tb->size) && tb->size <= MAX_INT/2)
        {
            luaS_resize(L, tb->size*2);  /* too crowded */
        }
    }
    catch( ... )
    {
        // We are not in the string table anymore.
        tb->hash[h].Remove( this );

        tb->nuse--;

        throw;
    }

    // Strings are always on the GC.
    this->gcflags.isGCActive = true;
}

TString::TString( const TString& right ) : GCObject( right )
{
    // We do not support cloning strings, as they are globally unique.
    throw lua_exception( right.gstate->mainthread, LUA_ERRRUN, "attempt to clone a string", 1 );
}

TString::~TString( void )
{
    global_State *g = this->gstate;

    stringtable *tb = &g->strt;

    if ( this->gcflags.isGCActive == true )
    {
        // Remove ourselves from the hash list.
        unsigned int h = this->hash;

        h = lmod(h, tb->size);

        tb->hash[h].Remove( this );

        this->gcflags.isGCActive = false;
    }

    // Decrease the global string count.
    tb->nuse--;
}

static TString* newlstr (lua_State *L, const char *str, size_t l, unsigned int h)
{
    global_State *g = G(L);

    // Attempt to get the string type information.
    stringTypeInfo_t *typeInfo = stringTypeInfo.GetPluginStruct( g->config );

    // If we have no information about the string type, fail.
    if ( !typeInfo )
        return nullptr;

    if (l+1 > (MAX_SIZET - sizeof(TString))/sizeof(char))
    {
        luaM_toobig(L);
    }

    stringConstructionParams params;
    params.length = l;
    params.hash = h;
    params.allocThread = L;

    TString *ts = lua_new <TString> ( L, typeInfo->stringTypeInfo, &params );

    if ( ts )
    {
        // Store the string that we want.
        memcpy( ts+1, str, l*sizeof(char) );

        ((char *)(ts+1))[l] = '\0';  /* ending 0 */
    }
    return ts;
}


TString *luaS_newlstr (lua_State *L, const char *str, size_t l)
{
    unsigned int h = cast(unsigned int, l);  /* seed */
    size_t step = (l>>5)+1;  /* if string is too long, don't hash all its chars */

    for (size_t l1=l; l1>=step; l1-=step)  /* compute hash */
    {
        h = h ^ ((h<<5)+(h>>2)+cast(unsigned char, str[l1-1]));
    }

    gcObjList_t& strHashList = G(L)->strt.hash[lmod(h, G(L)->strt.size)];

    for ( gcObjList_t::iterator iter = strHashList.GetIterator(); !iter.IsEnd(); iter.Increment() )
    {
        GCObject *o = (GCObject*)iter.Resolve();

        TString *ts = rawgco2ts(o);

        if (ts->len == l && (memcmp(str, getstr(ts), l) == 0))
        {
            /* string may be dead */
            if (isdead(G(L), o))
            {
                // TODO: this does not belong here.
                changewhite(o);
            }

            // When returning an already existing string, we increase its reference count.
            // This is for one to complement the behaviour of returning a new string.
            // Another reason is purely logical, as the GC ref count is a usage count.
            ts->ReferenceGC( L );

            return ts;
        }
    }

    return newlstr(L, str, l, h);  /* not found */
}

int luaS_concat (lua_State *L, int topOffset, int total)
{
    // Only concat strings if we can get the global string environment.
    globalStateStringEnv *stringEnv = GetGlobalStringEnv( G(L) );

    if ( !stringEnv )
    {
        luaG_runerror( L, "no global string environment" );
    }

    int n = 0;  // the amount of string items that have been concatenated to one.

    if ( total > 0 )
    {
        /* at least one string value; get as many as possible */
        size_t tl = 0;

        /* collect total length */
        for ( n = 0; n < total; n++ )
        {
            ValueAddress stringVal = index2adr( L, topOffset - n );

            int canBeStringed = tostring(L, stringVal);

            if ( canBeStringed == 0 )
                break;

            size_t l = tsvalue(stringVal)->len;

            if ( n != 0 && l >= MAX_SIZET - tl )
            {
                luaG_runerror( L, "string length overflow" );
            }

            tl += l;
        }

        char *buffer = luaZ_openspace(L, &stringEnv->buff, tl);
        tl = 0;

        for ( int i = n; i > 0; i-- )
        {  /* concat all strings */
            ValueAddress stringVal = index2adr( L, topOffset - i + 1 );

            TString *theString = tsvalue(stringVal);

            size_t l = theString->len;

            memcpy( buffer+tl, getstr(theString), l );
            tl += l;
        }
        
        // Write the result string.
        ValueAddress outString = index2adr( L, topOffset - n + 1 );

        TString *concatResultString = luaS_newlstr(L, buffer, tl);

        // No exception is throw in atomic prototypes like these.
        setsvalue(L, outString, concatResultString);

        // Since the concat result string is on the stack now, we can dereference it.
        concatResultString->DereferenceGC( L );
    }

    return n;
}

void luaS_globalgc (lua_State *L)
{
    globalStateStringEnv *stringEnv = GetGlobalStringEnv( G(L) );

    // Only proceed if we have the string environment.
    if ( stringEnv )
    {
        /* check size of buffer */
        if ( luaZ_sizebuffer(&stringEnv->buff) > LUA_MINBUFFER*2 )
        {  /* buffer too big? */
            size_t newsize = luaZ_sizebuffer(&stringEnv->buff) / 2;

            luaZ_resizebuffer(L, &stringEnv->buff, newsize);
        }
    }
}

void luaS_free (lua_State *L, TString *s)
{
    lua_delete <TString> ( L, s );
}

Udata::Udata( global_State *g, void *construction_params ) : GCObject( g )
{
    stringConstructionParams *params = (stringConstructionParams*)construction_params;

    size_t length = 0;

    if ( params )
    {
        length = params->length;
    }

    this->len = length;
    this->metatable = nullptr;
    this->env = nullptr;

    luaC_linktmu( g, this, LUA_TUSERDATA );
}

Udata::Udata( const Udata& right ) : GCObject( right )
{
    size_t dataLen = right.len;

    // Clone this userdata.
    this->len = dataLen;
    this->metatable = right.metatable;
    this->env = right.env;

    // Clone over the string stuff.
    if ( dataLen != 0 )
    {
        void *dataPtr = ( this + 1 );

        const void *srcDataPtr = ( &right + 1 );

        memcpy( dataPtr, srcDataPtr, dataLen );
    }

    luaC_linktmu( this->gstate, this, LUA_TUSERDATA );
}

Udata::~Udata( void )
{
    global_State *g = this->gstate;

    luaC_unlinktmu( g, this );
}

Udata *luaS_newudata (lua_State *L, size_t s, GCObject *e)
{
    // Attempt to get the string type information.
    stringTypeInfo_t *typeInfo = stringTypeInfo.GetPluginStruct( G(L)->config );

    // If we have no information about the string type, fail.
    if ( !typeInfo )
        return nullptr;

    // Check that we do not encounter number overflow!
    if (s > MAX_SIZET - sizeof(Udata))
    {
        luaM_toobig(L);
    }

    stringConstructionParams params;
    params.length = s;

    Udata *u = lua_new <Udata> ( L, typeInfo->udataTypeInfo, &params );

    if ( u )
    {
        u->env = e;
    }
    return u;
}

void luaS_freeudata (lua_State *L, Udata *u)
{
    lua_delete <Udata> ( L, u );
}

// Global state initialization.
void luaS_stateinit( lua_State *L )
{
    globalStateStringEnv *stringEnv = GetGlobalStringEnv( G(L) );

    if ( stringEnv )
    {
        luaZ_initbuffer( L, &stringEnv->buff );
    }
}

void luaS_stateshutdown( lua_State *L )
{
    globalStateStringEnv *stringEnv = GetGlobalStringEnv( G(L) );

    if ( stringEnv )
    {
        luaZ_freebuffer( L, &stringEnv->buff );
    }
}

// Module initialization.
void luaS_init( lua_config *cfg )
{
    return;
}

void luaS_shutdown( lua_config *cfg )
{
    return;
}

void luaS_libraryinit( void )
{
    stringTypeInfo.RegisterPlugin( namespaceFactory );
    stringEnvConnectingBridge.RegisterPluginStruct( namespaceFactory );
}

void luaS_libraryshutdown( void )
{
    stringEnvConnectingBridge.UnregisterPluginStruct();
    stringTypeInfo.UnregisterPlugin();
}