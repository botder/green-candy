/*
** $Id: lmem.c,v 1.70.1.1 2007/12/27 13:02:25 roberto Exp $
** Interface to Memory Manager
** See Copyright Notice in lua.h
*/


#include "luacore.h"

#include "lapi.h"
#include "ldebug.h"
#include "ldo.h"
#include "lmem.h"
#include "lobject.h"

// Include garbage collector internals.
#include "lgc.internal.hxx"



/*
** About the realloc function:
** void * frealloc (void *ud, void *ptr, size_t osize, size_t nsize);
** (`osize' is the old size, `nsize' is the new size)
**
** Lua ensures that (ptr == NULL) iff (osize == 0).
**
** * frealloc(ud, NULL, 0, x) creates a new block of size `x'
**
** * frealloc(ud, p, x, 0) frees the block `p'
** (in this specific case, frealloc must return NULL).
** particularly, frealloc(ud, NULL, 0, 0) does nothing
** (which is equivalent to free(NULL) in ANSI C)
**
** frealloc returns NULL if it cannot create or reallocate the area
** (any reallocation to an equal or smaller size cannot fail!)
*/



#define MINSIZEARRAY	4


void *luaM_growaux_ (lua_State *L, void *block, size_t *size, size_t size_elems, size_t limit, const char *errormsg)
{
    void *newblock;
    size_t newsize;

    if (*size >= limit/2)
    {  /* cannot double it? */
        if (*size >= limit)  /* cannot grow even a little? */
        {
            luaG_runerror(L, errormsg);
        }

        newsize = limit;  /* still have at least one free place */
    }
    else
    {
        newsize = (*size)*2;

        if (newsize < MINSIZEARRAY)
        {
            newsize = MINSIZEARRAY;  /* minimum size */
        }
    }

    newblock = luaM_reallocv(L, block, *size, newsize, size_elems);

    *size = newsize;  /* update only when everything else is OK */
    return newblock;
}


void *luaM_toobig (lua_State *L) {
  luaG_runerror(L, "memory allocation error: block too big");
  return nullptr;  /* to avoid warnings */
}



/*
** generic allocation routine.
*/
#include "lstate.lowlevel.hxx"

void *luaM_realloc__(global_State *g, void *block, size_t osize, size_t nsize)
{
    lua_assert((osize == 0) == (block == nullptr));

    // Get the native allocation data.
    GlobalStateAllocPluginData *allocData = (GlobalStateAllocPluginData*)g->config->allocData;

    if ( !allocData )
        return nullptr;

    block = allocData->_memAlloc.ReAlloc( block, osize, nsize );

    if (block == nullptr && nsize > 0)
        return nullptr;

    // Make sure that when we do not want any data anymore (size == 0), we do not have a memory pointer anymore.
    // If we have data though, we must have a block pointer!
    lua_assert((nsize == 0) == (block == nullptr));

    g->totalbytes = (g->totalbytes - osize) + nsize;
    return block;
}

// For backwards compatibility.
void *luaM_realloc_ (lua_State *L, void *block, size_t osize, size_t nsize)
{
    void *memBlock = luaM_realloc__( G(L), block, osize, nsize );

    if ( memBlock == nullptr && nsize > 0 )
        throw lua_exception( L, LUA_ERRMEM, "memory allocation failure" );

    return memBlock;
}

// Module initialization.
void luaM_init( lua_config *cfg )
{
    return;
}

void luaM_shutdown( lua_config *cfg )
{
    return;
}