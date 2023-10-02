/*
** $Id: lundump.c,v 2.7.1.4 2008/04/04 19:51:41 roberto Exp $
** load precompiled Lua chunks
** See Copyright Notice in lua.h
*/

#include "luacore.h"

#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "lmem.h"
#include "lobject.h"
#include "lstring.h"
#include "lundump.h"
#include "lzio.h"

typedef struct {
 lua_State* L;
 ZIO* Z;
 Mbuffer* b;
 const char* name;
} LoadState;

#ifdef LUAC_TRUST_BINARIES
#define IF(c,s)
#define error(S,s)
#else
#define IF(c,s)		if (c) error(S,s)

static void error(LoadState* S, const char* why)
{
    why = luaO_pushfstring( S->L, "%s: %s in precompiled chunk", S->name, why );
    throw lua_exception( S->L, LUA_ERRSYNTAX, why );
}
#endif

#define LoadMem(S,b,n,size)	LoadBlock(S,b,(n)*(size))
#define	LoadByte(S)		(lu_byte)LoadChar(S)
#define LoadVar(S,x)		LoadMem(S,&x,1,sizeof(x))
#define LoadVector(S,b,n,size)	LoadMem(S,b,n,size)

static void LoadBlock(LoadState* S, void* b, size_t size)
{
 size_t r=luaZ_read(S->Z,b,size);
 IF (r!=0, "unexpected end");
}

static int LoadChar(LoadState* S)
{
 char x;
 LoadVar(S,x);
 return x;
}

static int LoadInt(LoadState* S)
{
 int x;
 LoadVar(S,x);
 IF (x<0, "bad integer");
 return x;
}

static lua_Number LoadNumber(LoadState* S)
{
 lua_Number x;
 LoadVar(S,x);
 return x;
}

static TString* LoadString(LoadState* S)
{
    size_t size;
    LoadVar(S,size);

    if (size==0)
    {
        return nullptr;
    }
    
    char *s = luaZ_openspace( S->L, S->b, size );

    LoadBlock( S, s, size );

    return luaS_newlstr( S->L, s, size-1 );     /* remove trailing '\0' */
}

static void LoadCode(LoadState* S, Proto* f)
{
    int n = LoadInt(S);
    f->code = luaM_newvector <Instruction> ( S->L, n );
    f->sizecode = n;
    LoadVector( S, f->code, n, sizeof(Instruction) );
}

static Proto* LoadFunction(LoadState* S, TString* p);

static void LoadConstants(LoadState* S, Proto* f)
{
    int numValues = LoadInt(S);

    f->k = luaM_newvector <TValue> ( S->L, numValues );
    f->sizek = numValues;

    for (int i = 0; i < numValues; i++)
    {
        setnilvalue( &f->k[i] );
    }
    for (int i = 0; i < numValues; i++)
    {
        TValue *o = &f->k[i];

        int t = LoadChar(S);

        switch (t)
        {
        case LUA_TNIL:
            setnilvalue(o);
            break;
        case LUA_TBOOLEAN:
            setbvalue(o,LoadChar(S)!=0);
            break;
        case LUA_TNUMBER:
            setnvalue(o,LoadNumber(S));
            break;
        case LUA_TSTRING:
        {
            TString *loadedString = LoadString(S);

            // Atomic prototypes like these are guarranteed to throw no exceptions.
            setsvalue2n( S->L, o, loadedString );

            // Since the loaded string is now stored in the Proto and the proto is referenced by default, we can dereference the string.
            loadedString->DereferenceGC( S->L );
            break;
        }
        default:
            error(S,"bad constant");
            break;
        }
    }

    int numProtos = LoadInt(S);

    f->p = luaM_newvector <Proto*> ( S->L, numProtos );
    f->sizep = numProtos;

    for (int i = 0; i < numProtos; i++)
    {
        f->p[i] = nullptr;
    }
    for (int i = 0; i < numProtos; i++)
    {
        Proto *subProto = LoadFunction(S,f->source);

        f->p[i] = subProto;

        // Since we store the subproto in an actual already reference proto, we can dereference it.
        subProto->DereferenceGC( S->L );
    }
}

static void LoadDebug(LoadState* S, Proto* f)
{
    int numLineInfo = LoadInt(S);
    f->lineinfo = luaM_newvector <int> ( S->L, numLineInfo );
    f->sizelineinfo = numLineInfo;

    LoadVector( S, f->lineinfo, numLineInfo, sizeof(int) );

    int numLocVars = LoadInt(S);
    f->locvars = luaM_newvector <LocVar> ( S->L, numLocVars );
    f->sizelocvars = numLocVars;

    for (int i = 0; i < numLocVars; i++)
    {
        f->locvars[i].varname = nullptr;
    }
    for (int i = 0; i < numLocVars; i++)
    {
        TString *varNameString = LoadString(S);

        f->locvars[i].varname = varNameString;

        // Since the variable name string is now stored in a reference prototype, we can dereference the string.
        varNameString->DereferenceGC( S->L );

        f->locvars[i].startpc = LoadInt(S);
        f->locvars[i].endpc = LoadInt(S);
    }

    int numUpValueNames = LoadInt(S);
    f->upvalues = luaM_newvector <TString*> ( S->L, numUpValueNames );
    f->sizeupvalues = numUpValueNames;

    for (int i = 0; i < numUpValueNames; i++)
    {
        f->upvalues[i] = nullptr;
    }
    for (int i = 0; i < numUpValueNames; i++)
    {
        TString *upValueNameString = LoadString(S);

        f->upvalues[i] = upValueNameString;

        // Since the upvalue name string is now stored in a reference prototype, we can dereference it.
        upValueNameString->DereferenceGC( S->L );
    }
}

static Proto* LoadFunction(LoadState* S, TString* p)
{
    callstack_ref cref( *S->L );

    Proto *f = luaF_newproto( S->L );

    try
    {
        TString *sourceString = LoadString(S);

        f->source = sourceString;

        if ( sourceString == nullptr )
        {
            f->source = p;
        }
        else
        {
            sourceString->DereferenceGC( S->L );
        }

        f->linedefined = LoadInt(S);
        f->lastlinedefined = LoadInt(S);
        f->nups = LoadByte(S);
        f->numparams = LoadByte(S);
        f->is_vararg = LoadByte(S);
        f->maxstacksize = LoadByte(S);

        LoadCode( S, f );
        LoadConstants( S, f );
        LoadDebug( S, f );

        IF ( !luaG_checkcode(f), "bad code" );
    }
    catch( ... )
    {
        // We dereference as the construction of that proto has failed.
        f->DereferenceGC( S->L );
        throw;
    }

    // We do not dereference the proto, since we return it.
    return f;
}

static void LoadHeader(LoadState* S)
{
 char h[LUAC_HEADERSIZE];
 char s[LUAC_HEADERSIZE];
 luaU_header(h);
 LoadBlock(S,s,LUAC_HEADERSIZE);
 IF (memcmp(h,s,LUAC_HEADERSIZE)!=0, "bad header");
}

/*
** load precompiled chunk
*/
Proto* luaU_undump (lua_State* L, ZIO* Z, Mbuffer* buff, const char* name)
{
    LoadState S;

    if ( *name=='@' || *name=='=' )
    {
        S.name  =name+1;
    }
    else if ( *name==LUA_SIGNATURE[0] )
    {
        S.name = "binary string";
    }
    else
    {
        S.name = name;
    }

    S.L = L;
    S.Z = Z;
    S.b = buff;

    LoadHeader(&S);

    Proto *resultProto = nullptr;

    TString *unkLiteral = luaS_newliteral( L, "=?" );

    try
    {
        resultProto = LoadFunction( &S, unkLiteral );
    }
    catch( ... )
    {
        // Everything is principally allowed to fail.
        unkLiteral->DereferenceGC( L );
        throw;
    }

    unkLiteral->DereferenceGC( L );

    return resultProto;
}

/*
* make header
*/
void luaU_header (char* h)
{
 int x=1;
 memcpy(h,LUA_SIGNATURE,sizeof(LUA_SIGNATURE)-1);
 h+=sizeof(LUA_SIGNATURE)-1;
 *h++=(char)LUAC_VERSION;
 *h++=(char)LUAC_FORMAT;
 *h++=(char)*(char*)&x;				/* endianness */
 *h++=(char)sizeof(int);
 *h++=(char)sizeof(size_t);
 *h++=(char)sizeof(Instruction);
 *h++=(char)sizeof(lua_Number);
 *h++=(char)(((lua_Number)0.5)==0);		/* is lua_Number integral? */
}
