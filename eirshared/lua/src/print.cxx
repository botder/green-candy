/*
** $Id: print.c,v 1.55a 2006/05/31 13:30:05 lhf Exp $
** print bytecodes
** See Copyright Notice in lua.h
*/

#include "luacore.h"

#include "ldebug.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lundump.h"

#include <iostream>

#define Sizeof(x)	((int)sizeof(x))
#define CVOID(p)    ((const void*)(p))

static void PrintString(const TString* ts)
{
 const char* s=getstr(ts);
 size_t i,n=ts->len;
 putchar('"');
 for (i=0; i<n; i++)
 {
  int c=s[i];
  switch (c)
  {
   case '"': printf("\\\""); break;
   case '\\': printf("\\\\"); break;
   case '\a': printf("\\a"); break;
   case '\b': printf("\\b"); break;
   case '\f': printf("\\f"); break;
   case '\n': printf("\\n"); break;
   case '\r': printf("\\r"); break;
   case '\t': printf("\\t"); break;
   case '\v': printf("\\v"); break;
   default:	if (isprint((unsigned char)c))
   			putchar(c);
		else
			printf("\\%03u",(unsigned char)c);
  }
 }
 putchar('"');
}

static void PrintConstant( const Proto* f, size_t i )
{
    const TValue *o = &f->k[i];

    switch( ttype(o) )
    {
    case LUA_TNIL:
        printf( "nil" );
        break;
    case LUA_TBOOLEAN:
        printf( bvalue(o) ? "true" : "false" );
        break;
    case LUA_TNUMBER:
        printf( LUA_NUMBER_FMT, nvalue(o) );
        break;
    case LUA_TSTRING:
        PrintString( tsvalue(o) );
        break;
    default:				/* cannot happen */
        printf( "? type=%d", ttype(o) );
        break;
    }
}

static void PrintCode(const Proto* f)
{
    const Instruction *code = f->code;

    size_t n = f->sizecode;

    for ( size_t pc = 0; pc < n; pc++ )
    {
        Instruction i = code[pc];
        
        OpCode o = GET_OPCODE(i);
        
        int a = GETARG_A(i);
        int b = GETARG_B(i);
        int c = GETARG_C(i);
        int bx = GETARG_Bx(i);
        int sbx = GETARG_sBx(i);
        int line = getline( f, pc );
        
        std::cout << "\t" << ( pc + 1 ) << "\t";
        
        if ( line > 0 )
        {
            printf( "[%d]\t", line );
        }
        else
        {
            printf("[-]\t");
        }
        
        printf( "%-9s\t", luaP_getopname(o) );
        
        switch (getOpMode(o))
        {
        case iABC:
            printf("%d",a);
        
            if ( getBMode(o) != OpArgN )
            {
                printf( " %d",ISK(b) ? (-1-INDEXK(b)) : b );
            }
            if ( getCMode(o) != OpArgN )
            {
                printf( " %d",ISK(c) ? (-1-INDEXK(c)) : c );
            }
            break;
        case iABx:
            if ( getBMode(o) == OpArgK )
            {
                printf( "%d %d", a, -1-bx );
            }
            else
            {
                printf( "%d %d", a, bx );
            }
            break;
        case iAsBx:
            if ( o == OP_JMP )
            {
                printf( "%d", sbx );
            }
            else
            {
                printf( "%d %d", a, sbx );
            }
            break;
        }
        
        switch (o)
        {
        case OP_LOADK:
            printf( "\t; " );
            PrintConstant( f, bx );
            break;
        case OP_GETUPVAL:
        case OP_SETUPVAL:
            printf( "\t; %s", ( f->sizeupvalues > 0) ? getstr( f->upvalues[b] ) : "-" );
            break;
        case OP_GETGLOBAL:
        case OP_SETGLOBAL:
            printf( "\t; %s", svalue(&f->k[bx]) );
            break;
        case OP_GETTABLE:
        case OP_SELF:
            if (ISK(c))
            {
                printf( "\t; " );
                PrintConstant( f, INDEXK(c) );
            }
            break;
        case OP_SETTABLE:
        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
        case OP_POW:
        case OP_EQ:
        case OP_LT:
        case OP_LE:
            if ( ISK(b) || ISK(c) )
            {
                printf("\t; ");
        
                if ( ISK(b) )
                {
                    PrintConstant( f, INDEXK(b) );
                }
                else
                {
                    printf("-");
                }
        
                printf(" ");
        
                if ( ISK(c) )
                {
                    PrintConstant( f, INDEXK(c) );
                }
                else
                {
                    printf("-");
                }
            }
            break;
        case OP_JMP:
        case OP_FORLOOP:
        case OP_FORPREP:
            std::cout << "\t; to " << ( sbx + pc + 2 );
            break;
        case OP_CLOSURE:
            printf( "\t; %p", CVOID( f->p[bx] ) );
            break;
        case OP_SETLIST:
            if ( c == 0 )
            {
                printf( "\t; %d", (int)code[ ++pc ] );
            }
            else
            {
                printf( "\t; %d", c );
            }
            break;
        default:
            break;
        }

        printf("\n");
    }
}

#define SS(x)	(x==1)?"":"s"
#define S(x)	x,SS(x)

static void PrintHeader(const Proto* f)
{
 const char* s=getstr(f->source);
 if (*s=='@' || *s=='=')
  s++;
 else if (*s==LUA_SIGNATURE[0])
  s="(bstring)";
 else
  s="(string)";
 printf("\n%s <%s:%d,%d> (%zu instruction%s, %zu bytes at %p)\n",
 	(f->linedefined==0)?"main":"function",s,
	f->linedefined,f->lastlinedefined,
	S(f->sizecode),f->sizecode*Sizeof(Instruction),CVOID(f));
 printf("%d%s param%s, %d slot%s, %d upvalue%s, ",
	f->numparams,f->is_vararg?"+":"",SS(f->numparams),
	S(f->maxstacksize),S(f->nups));
 printf("%zu local%s, %zu constant%s, %zu function%s\n",
	S(f->sizelocvars),S(f->sizek),S(f->sizep));
}

static void PrintConstants( const Proto* f )
{
    size_t n = f->sizek;

    printf( "constants (%zu) for %p:\n", n, CVOID(f) );

    for ( size_t i = 0; i < n; i++ )
    {
        printf( "\t%zu\t", i+1 );

        PrintConstant( f, i );

        printf( "\n" );
    }
}

static void PrintLocals( const Proto* f )
{
    size_t n = f->sizelocvars;

    printf( "locals (%zu) for %p:\n", n, CVOID(f) );

    for ( size_t i = 0; i < n; i++ )
    {
        printf(
            "\t%zu\t%s\t%zu\t%zu\n",
            i,
            getstr( f->locvars[i].varname ),
            f->locvars[i].startpc + 1,
            f->locvars[i].endpc + 1
        );
    }
}

static void PrintUpvalues( const Proto* f )
{
    size_t n = f->sizeupvalues;

    printf( "upvalues (%zu) for %p:\n", n, CVOID(f) );

    if ( f->upvalues == nullptr )
        return;

    for ( size_t i = 0; i < n; i++ )
    {
        printf( "\t%zu\t%s\n", i, getstr(f->upvalues[i]) );
    }
}

void luaU_print( const Proto* f, bool full )
{
    PrintHeader(f);
    PrintCode(f);

    if ( full )
    {
        PrintConstants(f);
        PrintLocals(f);
        PrintUpvalues(f);
    }

    size_t n = f->sizep;

    for ( size_t i = 0; i < n; i++ )
    {
        luaU_print( f->p[i], full );
    }
}
