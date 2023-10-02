/*
** $Id: lobject.c,v 2.22.1.1 2007/12/27 13:02:25 roberto Exp $
** Some generic functions over Lua objects
** See Copyright Notice in lua.h
*/

#include "luacore.h"

#include "ldo.h"
#include "lmem.h"
#include "lobject.h"
#include "lstate.h"
#include "lstring.h"
#include "lvm.h"
#include "ldebug.h"



// TODO: remove this, rather use a global_State variable.
const TValue luaO_nilobject_ = {{nullptr}, LUA_TNIL};


/*
** converts an integer to a "floating point byte", represented as
** (eeeeexxx), where the real value is (1xxx) * 2^(eeeee - 1) if
** eeeee != 0 and (xxx) otherwise.
*/
int luaO_int2fb (unsigned int x) {
  int e = 0;  /* expoent */
  while (x >= 16) {
    x = (x+1) >> 1;
    e++;
  }
  if (x < 8) return x;
  else return ((e+1) << 3) | (cast_int(x) - 8);
}


/* converts back */
int luaO_fb2int (int x) {
  int e = (x >> 3) & 31;
  if (e == 0) return x;
  else return ((x & 7)+8) << (e - 1);
}


int luaO_log2 (size_t x) {
  static const lu_byte log_2[256] = {
    0,1,2,2,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8
  };
  int l = -1;
  while (x >= 256) { l += 8; x >>= 8; }
  return l + log_2[x];

}


int luaO_rawequalObj (const TValue *t1, const TValue *t2) {
  if (ttype(t1) != ttype(t2)) return 0;
  else switch (ttype(t1)) {
    case LUA_TNIL:
      return 1;
    case LUA_TNUMBER:
      return luai_numeq(nvalue(t1), nvalue(t2));
    case LUA_TBOOLEAN:
      return bvalue(t1) == bvalue(t2);  /* boolean true must be 1 !! */
    case LUA_TLIGHTUSERDATA:
      return pvalue(t1) == pvalue(t2);
    default:
      lua_assert(iscollectable(t1));
      return gcvalue(t1) == gcvalue(t2);
  }
}


int luaO_str2d (const char *s, lua_Number *result) {
  char *endptr;
  *result = lua_str2number(s, &endptr);
  if (endptr == s) return 0;  /* conversion failed */
  if (*endptr == 'x' || *endptr == 'X')  /* maybe an hexadecimal constant? */
    *result = cast_num(strtoul(s, &endptr, 16));
  if (*endptr == '\0') return 1;  /* most common case */
  while (isspace(cast(unsigned char, *endptr))) endptr++;
  if (*endptr != '\0') return 0;  /* invalid trailing characters? */
  return 1;
}



static void pushstr (lua_State *L, const char *str)
{
    TString *theString = luaS_new(L, str);

    try
    {
        pushsvalue(L, theString);
    }
    catch( ... )
    {
        theString->DereferenceGC( L );
        throw;
    }

    // Since the string is now on the stack, we can dereference the string.
    theString->DereferenceGC( L );
}


/* this function handles only `%d', `%c', %f, %p, and `%s' formats */
const char *luaO_pushvfstring (lua_State *L, const char *fmt, va_list argp)
{
    int n = 1;

    pushstr(L, "");

    for (;;)
    {
        const char *e = strchr(fmt, '%');

        if ( e == nullptr )
        {
            break;
        }

        TString *newStrSection = luaS_newlstr(L, fmt, e-fmt);

        try
        {
            pushsvalue(L, newStrSection);
        }
        catch( ... )
        {
            // Handle pretty obscure stack expansion errors.
            newStrSection->DereferenceGC( L );
            throw;
        }

        // Since the new string section is on the stack now, we can dereference it.
        newStrSection->DereferenceGC( L );

        switch( *(e+1) )
        {
        case 's': {
            const char *s = va_arg(argp, char *);

            if ( s == nullptr )
            {
                s = "(null)";
            }

            pushstr(L, s);
            break;
        }
        case 'c': {
            char buff[2];
            buff[0] = cast(char, va_arg(argp, int));
            buff[1] = '\0';
            pushstr(L, buff);
            break;
        }
        case 'd': {
            pushnvalue(L, cast_num(va_arg(argp, int)));
            break;
        }
        case 'f': {
            pushnvalue(L, cast_num(va_arg(argp, l_uacNumber)));
            break;
        }
        case 'p': {
            char buff[4*sizeof(void *) + 8]; /* should be enough space for a `%p' */
            sprintf(buff, "%p", va_arg(argp, void *));
            pushstr(L, buff);
            break;
        }
        case '%': {
            pushstr(L, "%");
            break;
        }
        default: {
            char buff[3];
            buff[0] = '%';
            buff[1] = *(e+1);
            buff[2] = '\0';
            pushstr(L, buff);
            break;
        }
        }
        n += 2;
        fmt = e+2;
    }

    // Push the remainder of the string that has no formatting in it.
    pushstr(L, fmt);

    // Concatenate the string.
    luaV_concat( L, n+1, rt_stackcount( L ) - 1 );

    popstack( L, n );

    // Return the result string.
    ConstValueAddress topItem = index2constadr( L, -1 );

    return svalue( topItem );
}


const char *luaO_pushfstring (lua_State *L, const char *fmt, ...) {
  const char *msg;
  va_list argp;
  va_start(argp, fmt);
  msg = luaO_pushvfstring(L, fmt, argp);
  va_end(argp);
  return msg;
}


void luaO_chunkid (char *out, const char *source, size_t bufflen) {
  if (*source == '=') {
    strncpy(out, source+1, bufflen);  /* remove first char */
    out[bufflen-1] = '\0';  /* ensures null termination */
  }
  else {  /* out = "source", or "...source" */
    if (*source == '@') {
      size_t l;
      source++;  /* skip the `@' */
      bufflen -= sizeof(" '...' ");
      l = strlen(source);
      strcpy(out, "");
      if (l > bufflen) {
        source += (l-bufflen);  /* get last part of file name */
        strcat(out, "...");
      }
      strcat(out, source);
    }
    else {  /* out = [string "string"] */
      size_t len = strcspn(source, "\n\r");  /* stop at first newline */
      bufflen -= sizeof(" [string \"...\"] ");
      if (len > bufflen) len = bufflen;
      strcpy(out, "[string \"");
      if (source[len] != '\0') {  /* must truncate? */
        strncat(out, source, len);
        strcat(out, "...");
      }
      else
        strcat(out, source);
      strcat(out, "\"]");
    }
  }
}

ConstValueAddress luaO_getnilcontext( lua_State *L )
{
    return ConstValueAddress( L, L->NewStateValueContext( luaO_nilobject ) );
}

void GCObject::Index( lua_State *L, ConstValueAddress& key, ValueAddress& val )
{
    LocalValueAddress objval;
    setgcvalue( L, objval, this );

    ConstValueAddress tmobjval = luaT_gettmbyobj( L, objval, TM_INDEX );

    luaV_handle_index( L, objval.ConstCast(), tmobjval, key, val );
}

void GCObject::NewIndex( lua_State *L, ConstValueAddress& key, ConstValueAddress& val )
{
    LocalValueAddress objval;
    setgcvalue( L, objval, this );

    ConstValueAddress tmobjval = luaT_gettmbyobj( L, objval, TM_NEWINDEX );

    luaV_handle_newindex( L, objval.ConstCast(), tmobjval, key, val );
}

// Module initialization.
void luaO_init( lua_config *cfg )
{
    return;
}

void luaO_shutdown( lua_config *cfg )
{
    return;
}
