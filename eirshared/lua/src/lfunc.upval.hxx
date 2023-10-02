#ifndef _LUA_UPVALUE_MANAGED_SLOT_
#define _LUA_UPVALUE_MANAGED_SLOT_

/*
** Upvalues
*/

class UpVal : public GCObject
{
public:
    UpVal( global_State *g, void *construction_params );

    ~UpVal( void );

    void MarkGC( global_State *g );

    TValue *v;  /* points to stack or to its own value */
    union DataUnion
    {
        inline DataUnion( void ) : value()      {}

        TValue value;  /* the value (when closed) */

        RwListEntry <UpVal> l;  /* node (when open) */
    } u;
};

// Private UpVal API.
LUAI_FUNC void luaC_linkupval (lua_State *L, UpVal *uv);
LUAI_FUNC void luaC_unlinkupval (lua_State *L, UpVal *uv);

#endif //_LUA_UPVALUE_MANAGED_SLOT_