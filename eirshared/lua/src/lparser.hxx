#ifndef _LUA_PARSER_INTERNAL_DEFINITIONS_
#define _LUA_PARSER_INTERNAL_DEFINITIONS_

/*
** Expression descriptor
*/

enum expkind
{
    VVOID,	/* no value */
    VNIL,
    VTRUE,
    VFALSE,
    VK,		/* info = index of constant in `k' */
    VKNUM,	/* nval = numerical value */
    VLOCAL,	/* info = local register */
    VUPVAL,       /* info = index of upvalue in `upvalues' */
    VGLOBAL,	/* info = index of table; aux = index of global name in `k' */
    VINDEXED,	/* info = table register; aux = index register (or `k') */
    VJMP,		/* info = instruction pc */
    VRELOCABLE,	/* info = instruction pc */
    VNONRELOC,	/* info = result register */
    VCALL,	/* info = instruction pc */
    VVARARG	/* info = instruction pc */
};

struct expdesc
{
    expkind k;
    union
    {
        struct { int info, aux; } s;
        lua_Number nval;
    } u;
    int t;  /* patch list of `exit when true' */
    int f;  /* patch list of `exit when false' */
};

struct upvaldesc
{
    lu_byte k;
    lu_byte info;
};

/*
** nodes for block list (list of active blocks)
*/
struct BlockCnt : public SingleLinkedList <BlockCnt>::node
{
    int breaklist;  /* list of jumps out of this loop */
    lu_byte nactvar;  /* # active locals outside the breakable structure */
    lu_byte upval;  /* true if some variable in the block is an upvalue */
    lu_byte isbreakable;  /* true if `block' is a loop */
};

/* state needed to generate code for a given function */
struct FuncState : public SingleLinkedList <FuncState>::node
{
    Proto *f;  /* current function header */
    Table *h;  /* table to find (and reuse) elements in `k' */
    struct LexState *ls;  /* lexical state */
    lua_State *L;  /* copy of the Lua state */
    SingleLinkedList <BlockCnt> blockList; /* chain of blocks */
    unsigned int pc;  /* next position to code (equivalent to `ncode') */
    unsigned int lasttarget;   /* `pc' of last `jump target' */
    int jpc;  /* list of pending jumps to `pc' */
    int freereg;  /* first free register */
    unsigned int nk;  /* number of elements in `k' */
    unsigned int np;  /* number of elements in `p' */
    unsigned short nlocvars;  /* number of elements in `locvars' */
    lu_byte nactvar;  /* number of active local variables */
    upvaldesc upvalues[LUAI_MAXUPVALUES];  /* upvalues */
    unsigned short actvar[LUAI_MAXVARS];  /* declared-variable stack */
};

inline FuncState* GetCurrentFuncState( LexState *ls )
{
    return (FuncState*)ls->fsList.GetFirst();
}

#endif //_LUA_PARSER_INTERNAL_DEFINITIONS_