/*
** $Id: ltable.c,v 2.32.1.2 2007/12/28 15:32:23 roberto Exp $
** Lua tables (hash)
** See Copyright Notice in lua.h
*/


/*
** Implementation of tables (aka arrays, objects, or hash tables).
** Tables keep its elements in two parts: an array part and a hash part.
** Non-negative integer keys are all candidates to be kept in the array
** part. The actual size of the array is the largest `n' such that at
** least half the slots between 0 and n are in use.
** Hash uses a mix of chained scatter table with Brent's variation.
** A main invariant of these tables is that, if an element is not
** in its main position (i.e. the `original' position that its hash gives
** to it), then the colliding element is in its own main position.
** Hence even when the load factor reaches 100%, performance remains good.
*/

#include "luacore.h"

#include "ldebug.h"
#include "ldo.h"
#include "lgc.h"
#include "lmem.h"
#include "lobject.h"
#include "lstate.h"
#include "ltable.h"
#include "lvm.h"

#include "lpluginutil.hxx"

#include "ltable.hxx"


/*
** max size of array part is 2^MAXBITS
*/
#if LUAI_BITSINT > 26
#define MAXBITS		26
#else
#define MAXBITS		(LUAI_BITSINT-2)
#endif

#define MAXASIZE	(1 << MAXBITS)


#define hashpow2(t,n)      (gnode(t, lmod((n), sizenode(t))))

#define hashstr(t,str)  hashpow2(t, (str)->hash)
#define hashboolean(t,p)        hashpow2(t, p)


/*
** for some types, it is better to avoid modulus by power of 2, as
** they tend to have many 2 factors.
*/
#define hashmod(t,n)	(gnode(t, ((n) % ((sizenode(t)-1)|1))))


#define hashpointer(t,p)	hashmod(t, IntPoint(p))

// Register the table type along with native implementation details.
PluginDependantStructRegister <tableTypeInfoPlugin, namespaceFactory_t> tableTypeInfo;

// We also need to register the access to the global_State Table environment.
tableEnvConnectingBridge_t tableEnvConnectingBridge;

// Lua table environment initializator.
void luaH_init( lua_config *cfg )
{
    return;
}

void luaH_shutdown( lua_config *cfg )
{
    return;
}

void luaH_libraryinit( void )
{
    tableTypeInfo.RegisterPlugin( namespaceFactory );
    tableEnvConnectingBridge.RegisterPluginStruct( namespaceFactory );
}

void luaH_libraryshutdown( void )
{
    tableEnvConnectingBridge.UnregisterPluginStruct();
    tableTypeInfo.UnregisterPlugin();
}

void luaH_clearRuntimeMemory( global_State *g )
{
    globalStateTableEnvPlugin *tableEnv = GetGlobalTableEnv( g );

    if ( tableEnv )
    {
        tableEnv->ClearMemory( g );
    }
}


/*
** hash for lua_Numbers
** atomic operation.
*/
static Node *hashnum (const tableNativeImplementation *t, lua_Number n)
{
    if (luai_numeq(n, 0))  /* avoid problems with -0 */
    {
        return gnode(t, 0);
    }

    const size_t numInts = ( sizeof( lua_Number ) / sizeof( unsigned int ) );

    union
    {
        lua_Number theNum;
        unsigned int a[ numInts ];
    };

    theNum = n;

    for ( size_t i = 1; i < numInts; i++ )
    {
        a[0] += a[i];
    }

    return hashmod(t, a[0]);
}



/*
** returns the `main' position of an element in a table (that is, the index
** of its hash value)
** atomic operation.
*/
static Node *mainposition (const tableNativeImplementation *t, const TValue *key)
{
    switch (ttype(key))
    {
    case LUA_TNUMBER:
        return hashnum(t, nvalue(key));
    case LUA_TSTRING:
        return hashstr(t, tsvalue(key));
    case LUA_TBOOLEAN:
    {
        int b = bvalue(key);
        return hashboolean(t, b);
    }
    case LUA_TLIGHTUSERDATA:
        return hashpointer(t, pvalue(key));
    default:
        return hashpointer(t, gcvalue(key));
    }
}


/*
** returns the index for `key' if `key' is an appropriate key to live in
** the array part of the table, -1 otherwise.
*/
static inline bool arrayindex( const TValue *key, size_t& idxOut )
{
    if (ttisnumber(key))
    {
        lua_Number n = nvalue(key);

        // Key has to be positive.
        if ( n > 0 )
        {
            // We truncate away the floating-point section (FLOOR op).
            size_t k = (size_t)n;

            // Check if the floating point value is an actual integer.
            if (luai_numeq(cast_num(k), n))
            {
                idxOut = k;
                return true;
            }
        }
    }
    return false;  /* `key' did not match some condition */
}

// Lua-based index checking.
FASTAPI bool isarrayindex( size_t index, size_t arraySize )
{
    return ( 0 < index && index <= arraySize );
}

// C-based index checking.
template <typename intKeyNumType>
FASTAPI bool isarrayindex_unsigned( const intKeyNumType& index, size_t arraySize )
{
    return ( cast(size_t, index-1) < cast(size_t, arraySize) );
}

/*
** returns the index of a `key' for table traversals. First goes all
** elements in the array part, then elements in the hash part.
** atomic operation.
*/
static inline size_t findindex( lua_State *L, tableNativeImplementation *t, ConstValueAddress& key )
{
    lua_assert( ttisnil(key) == false );

    // Check if it exists in the array directly.
    {
        size_t i;
        bool couldConv = arrayindex( key, i );

        if ( couldConv )
        {
            if ( isarrayindex( i, t->sizearray ) )  /* is `key' inside array part? */
            {
                return i - 1;  /* yes; that's the index (corrected to C) */
            }
        }
    }

    // Check if there is a dict node pointing at it.
    {
        Node *n = mainposition(t, key);

        do
        {  /* check whether `key' is somewhere in the chain */
            /* key may be dead already, but it is ok to use it in `next' */
            // TODO: fix this situation tht the key may be dead?
            if (luaO_rawequalObj(key2tval(n), key) || (ttype(gkey(n)) == LUA_TDEADKEY && iscollectable(key) && gcvalue(gkey(n)) == gcvalue(key)))
            {
                size_t i = (size_t)(n - gnode(t, 0));  /* key index in hash table */
                /* hash elements are numbered after array ones */
                return ( i + t->sizearray );
            }
            else
            {
                n = gnext(n);
            }
        } while (n);
    }

    luaG_runerror(L, "invalid key to " LUA_QL("next"));  /* key not found */

    return false;  /* to avoid warnings */
}

bool luaH_next( lua_State *L, Table *t )
{
    // Get the native implementation that we use for this table.
    tableNativeImplementation *nativeTable = tableNativeImplementation::GetNativeImplementation( t );

    if ( !nativeTable )
        return false;

    global_State *g = G(L);

    NativeExecutive::CReadWriteReadContextSafe <> readContext( nativeTable->GetRuntimeLock( g ) );

    size_t idx;
    {
        ConstValueAddress keyval = index2constadr( L, -1 );

        if ( ttisnil(keyval) )
        {
            // Beginning of traversal.
            idx = -1;
        }
        else
        {
            idx = findindex( L, nativeTable, keyval );  /* find original element */
        }

        popstack( L, 1 );
    }

    // Skip the first?
    idx++;

    size_t sizearray = nativeTable->sizearray;

    for ( ; idx < sizearray; idx++ )
    {  /* try first array part */
        TValue& curItem = nativeTable->array[idx];

        if ( !ttisnil( &curItem ) )
        {  /* a non-nil value? */
            pushnvalue( L, cast_num(idx+1) );
            pushtvalue( L, &curItem );
            return true;
        }
    }

    size_t sizen = sizenode(nativeTable);

    for ( idx -= sizearray; idx < sizen; idx++ )
    {  /* then hash part */
        Node *curNode = gnode(nativeTable, idx);
        TValue *curItem = gval(curNode);

        if ( !ttisnil( curItem ) )
        {  /* a non-nil value? */
            pushtvalue( L, key2tval(curNode) );
            pushtvalue( L, curItem );
            return true;
        }
    }

    return false;  /* no more elements */
}


/*
** {=============================================================
** Native Atomic Manipulation
** ==============================================================
*/

/*
** inserts a new key into a hash table; first, check whether key's main
** position is free. If not, check whether colliding node is in its main
** position or not: if it is not, move colliding node to an empty place and
** put new key in its main position; otherwise (colliding node is in its main
** position), new key goes to an empty position.
** atomic operation.
*/
template <typename valueType>
bool _findkeybystr( tableNativeImplementation *t, const TString *key, valueType*& valueOut );
template <typename valueType, typename keyType>
bool _findkeybynum( tableNativeImplementation *t, const keyType& key, valueType*& valueOut );

template <typename valueType>
bool _findkey( tableNativeImplementation *t, const TValue *key, valueType*& valueOut )
{
    bool foundValue = false;

    TValue *retValue = nullptr;

    switch (ttype(key))
    {
    case LUA_TNIL:
        return false;
    case LUA_TSTRING:
        foundValue = _findkeybystr( t, tsvalue( key ), retValue );
        break;
    case LUA_TNUMBER:
    {
        int k;
        lua_Number n = nvalue(key);
        lua_number2int(k, n);

        if ( luai_numeq(cast_num(k), nvalue(key)) ) /* index is int? */
        {
            foundValue = _findkeybynum( t, k, retValue );  /* use specialized version */
            break;
        }
        /* else go through */
    }
    default:
    {
        Node *n = mainposition(t, key);

        do
        {  /* check whether `key' is somewhere in the chain */
            if (luaO_rawequalObj(key2tval(n), key))
            {
                retValue = gval(n);  /* that's it */

                foundValue = true;
                break;
            }
            else
            {
                n = gnext(n);
            }
        }
        while ( n != 0 );

        break;
    }
    }

    if ( foundValue )
    {
        valueOut = retValue;
    }

    return foundValue;
}

/*
** search function for integers
** atomic operation.
*/
template <typename valueType, typename keyType>
bool _findkeybynum( tableNativeImplementation *t, const keyType& key, valueType*& valueOut )
{
    bool valueFound = false;

    TValue *retValue = nullptr;

    /* (1 <= key && key <= t->sizearray) */
    if ( isarrayindex_unsigned( key, t->sizearray ) )
    {
        retValue = &t->array[key-1];

        valueFound = true;
    }
    else
    {
        lua_Number nk = cast_num(key);
        Node *n = hashnum(t, nk);

        do
        {  /* check whether `key' is somewhere in the chain */
            if ( ttisnumber(gkey(n)) && luai_numeq(nvalue(gkey(n)), nk) )
            {
                retValue = gval(n);  /* that's it */

                valueFound = true;
                break;
            }
            else
            {
                n = gnext(n);
            }
        }
        while ( n != 0 );
    }

    if ( valueFound )
    {
        valueOut = retValue;
    }

    return valueFound;
}

/*
** search function for strings
** atomic operation.
*/
template <typename valueType>
bool _findkeybystr( tableNativeImplementation *t, const TString *key, valueType*& valueOut )
{
    bool valueFound = false;

    TValue *retValue = nullptr;

    Node *n = hashstr(t, key);

    do
    {  /* check whether `key' is somewhere in the chain */
        if ( ttisstring(gkey(n)) && tsvalue(gkey(n)) == key )
        {
            retValue = gval(n);  /* that's it */

            valueFound = true;
            break;
        }
        else
        {
            n = gnext(n);
        }
    }
    while ( n != 0 );

    if ( valueFound )
    {
        valueOut = retValue;
    }

    return valueFound;
}

// atomic operation.
static inline Node* getfreepos (tableNativeImplementation *t)
{
    if ( t->lastfree == nullptr )
    {
        return nullptr;
    }

    while ( t->lastfree-- > t->node )
    {
        if ( ttisnil( gkey(t->lastfree) ) )
        {
            return t->lastfree;
        }
    }
    return nullptr;  /* could not find a free place */
}

// atomic operation.
static size_t computesizes( size_t nums[], size_t *narray )
{
    size_t a = 0;  /* number of elements smaller than 2^i */
    size_t na = 0;  /* number of elements to go to array part */
    size_t n = 0;  /* optimal size for array part */

    for ( size_t i = 0, twotoi = 1; ( twotoi / 2 ) < *narray; i++, twotoi *= 2 )
    {
        if (nums[i] > 0)
        {
            a += nums[i];

            if (a > twotoi/2)
            {  /* more than half elements present? */
                n = twotoi;  /* optimal size (till now) */
                na = a;  /* all elements smaller than n will go to array part */
            }
        }
        if (a == *narray)
        {
            break;  /* all elements already counted */
        }
    }

    *narray = n;

    lua_assert(*narray/2 <= na && na <= *narray);

    return na;
}

// atomic operation.
static size_t countint( const TValue *key, size_t *nums )
{
    size_t k;
    bool couldConv = arrayindex( key, k );

    if ( couldConv )
    {
        if ( isarrayindex( k, MAXASIZE ) )
        {  /* is `key' an appropriate array index? */
            nums[ ceillog2(k) ]++;  /* count as such */
            return 1;
        }
    }

    return 0;
}

// atomic operation.
static size_t numusearray( const tableNativeImplementation *t, size_t *nums )
{
    size_t ttlg = 1;  /* 2^lg */
    size_t ause = 0;  /* summation of `nums' */
    size_t i = 1;  /* count to traverse all array keys */

    for ( size_t lg = 0; lg <= MAXBITS; lg++, ttlg *= 2 )
    {  /* for each slice */
        size_t lc = 0;  /* counter */
        size_t lim = ttlg;

        if ( lim > t->sizearray )
        {
            lim = t->sizearray;  /* adjust upper limit */

            if (i > lim)
            {
                break;  /* no more elements to count */
            }
        }
        /* count elements in range (2^(lg-1), 2^lg] */
        for ( ; i <= lim; i++ )
        {
            if ( !ttisnil(&t->array[i-1]) )
            {
                lc++;
            }
        }

        nums[lg] += lc;
        ause += lc;
    }

    return ause;
}

// atomic operation.
static size_t numusehash (const tableNativeImplementation *t, size_t *nums, size_t *pnasize)
{
    size_t totaluse = 0;  /* total number of elements */
    size_t ause = 0;  /* summation of `nums' */
    size_t i = sizenode(t);

    while ( i-- )
    {
        Node *n = &t->node[i];

        if ( !ttisnil(gval(n)) )
        {
            ause += countint(key2tval(n), nums);
            totaluse++;
        }
    }
    *pnasize += ause;

    return totaluse;
}

// special atomic operation.
static void setarrayvector( lua_State *L, tableNativeImplementation *t, size_t size )
{
    size_t oldsize = t->sizearray;

    luaM_reallocvector( L, t->array, oldsize, size );

    for ( size_t i = oldsize; i < size; i++ )
    {
        setnilvalue(&t->array[i]);
    }
    t->sizearray = size;
}

// special atomic operation
static void setnodevector (lua_State *L, tableNativeImplementation *t, size_t size)
{
    unsigned int lsize;

    if ( size == 0 )
    {  /* no elements to hash part? */
        t->node = GetDummyNode( G(L) );  /* use common `dummynode' */
        lsize = 0;
    }
    else
    {
        lsize = (unsigned int)ceillog2(size);

        if ( lsize > MAXBITS )
        {
            luaG_runerror(L, "table overflow");
        }

        size = twoto(lsize);
        t->node = luaM_newvector <Node> (L, size);

        // Initialize the vector.
        for ( size_t i = 0; i < size; i++ )
        {
            Node *n = gnode(t, i);

            gnext(n) = nullptr;

            setnilvalue(gkey(n));
            setnilvalue(gval(n));
        }
    }

    t->lsizenode = (decltype(t->lsizenode))(lsize);
    t->lastfree = gnode(t, size);  /* all positions are free */
}

static void resize (lua_State *L, Table *h, tableNativeImplementation *t, size_t nasize, size_t nhsize);

// special atomic operation.
static void rehash (lua_State *L, Table *h, tableNativeImplementation *t, const TValue *ek)
{
    size_t nums[MAXBITS+1];  /* nums[i] = number of keys between 2^(i-1) and 2^i */

    for ( int i = 0; i <= MAXBITS; i++ )
    {
        nums[i] = 0;  /* reset counts */
    }

    size_t nasize = numusearray(t, nums);  /* count keys in array part */
    size_t totaluse = nasize;  /* all those keys are integer keys */
    totaluse += numusehash(t, nums, &nasize);  /* count keys in hash part */
    /* count extra key */
    nasize += countint(ek, nums);
    totaluse++;

    /* compute new size for array part */
    size_t na = computesizes(nums, &nasize);
    /* resize the table to new computed sizes */
    resize(L, h, t, nasize, totaluse - na);
}

// special atomic operation.
static TValue* newkey (lua_State *L, Table *h, tableNativeImplementation *t, const TValue *key)
{
    // Get the dummy node.
    const Node *dummynode = GetDummyNode( G(L) );

    Node *mp = mainposition(t, key);

    if ( !ttisnil(gval(mp)) || mp == dummynode )
    {
        Node *othern;
        Node *n = getfreepos(t);  /* get a free place */

        if (n == nullptr)
        {  /* cannot find a free place? */
            rehash(L, h, t, key);  /* grow table */

            TValue *newVal = nullptr;

            bool hasFoundKey = _findkey( t, key, newVal );

            if ( !hasFoundKey )
            {
                newVal = newkey( L, h, t, key ); /* re-insert key into grown table */
            }

            lua_assert( newVal != nullptr );
            return newVal;
        }

        lua_assert(n != dummynode);

        othern = mainposition(t, key2tval(mp));

        if (othern != mp)
        {  /* is colliding node out of its main position? */
            /* yes; move colliding node into free position */
            while (gnext(othern) != mp)
            {
                othern = gnext(othern);  /* find previous */
            }

            gnext(othern) = n;  /* redo the chain with `n' in place of `mp' */
            *n = *mp;  /* copy colliding node into free pos. (mp->next also goes) */
            gnext(mp) = nullptr;  /* now `mp' is free */

            setnilvalue(gval(mp));
        }
        else
        {  /* colliding node is in its own main position */
            /* new node will go into free position */
            gnext(n) = gnext(mp);  /* chain new position */
            gnext(mp) = n;
            mp = n;
        }
    }
    gkey(mp)->value = key->value; gkey(mp)->tt = key->tt;
    luaC_barriert(L, h, key);

    lua_assert(ttisnil(gval(mp)));
    return gval(mp);
}

// atomic operation.
static TValue* settablenative( lua_State *L, Table *h, tableNativeImplementation *t, const TValue *key )
{
    TValue *returnValue = nullptr;

    bool valueExists = _findkey( t, key, returnValue );

    // Reset optimization flags of the table.
    t->flags = 0;

    if ( !valueExists )
    {
        if (ttisnil(key))
        {
            luaG_runerror(L, "table index is nil");
        }
        else if (ttisnumber(key) && luai_numisnan(nvalue(key)))
        {
            luaG_runerror(L, "table index is NaN");
        }

        returnValue = newkey(L, h, t, key);
    }

    return returnValue;
}

// atomic operation.
static TValue* settablenativestr( lua_State *L, Table *h, tableNativeImplementation *t, TString *key )
{
    TValue *returnValue = nullptr;

    bool hasValue = _findkeybystr( t, key, returnValue );

    if ( !hasValue )
    {
        TValue k;
        setsvalue(L, &k, key);
        returnValue = newkey(L, h, t, &k);
    }

    return returnValue;
}

// atomic operation.
template <typename keyIntType>
static TValue* settablenativenum( lua_State *L, Table *h, tableNativeImplementation *t, const keyIntType& key )
{
    TValue *returnValue = nullptr;

    bool valueExists = _findkeybynum( t, key, returnValue );

    if ( !valueExists )
    {
        TValue k;
        setnvalue(&k, cast_num(key));
        returnValue = newkey(L, h, t, &k);
    }

    return returnValue;
}

// special atomic operation.
static void resize (lua_State *L, Table *h, tableNativeImplementation *t, size_t nasize, size_t nhsize)
{
    size_t oldasize = t->sizearray;
    size_t oldhsize = t->lsizenode;
    Node *nold = t->node;  /* save old hash ... */
    TValue *arrold = t->array;

    if ( nasize > oldasize )  /* array part must grow? */
    {
        setarrayvector(L, t, nasize);
    }

    /* create new hash part with appropriate size */
    setnodevector(L, t, nhsize);

    if ( nasize < oldasize )
    {  /* array part must shrink? */
        t->sizearray = nasize;

        /* re-insert elements from vanishing slice */
        for ( size_t i = nasize; i < oldasize; i++ )
        {
            if ( !ttisnil(&arrold[i]) )
            {
                TValue *numObj = settablenativenum(L, h, t, i+1);

                setobjt2t(L, numObj, &arrold[i]);
            }
        }

        /* shrink array */
        luaM_reallocvector(L, t->array, oldasize, nasize);
    }

    /* re-insert elements from hash part */
    {
        size_t i = twoto(oldhsize);

        while ( i > 0 )
        {
            Node *old = nold+(i-1);

            if ( !ttisnil(gval(old)) )
            {
                TValue *tableItemPtr = settablenative(L, h, t, key2tval(old));

                setobjt2t( L, tableItemPtr, gval(old) );
            }

            i--;
        }
    }

    if ( nold != GetDummyNode( G(L) ) )
    {
        luaM_freearray( L, nold, twoto(oldhsize) );  /* free old array */
    }

    // Update the dynamic pointers, if any base changed.
    TValue *arrnew = t->array;
    Node *nodenew = t->node;

    if ( arrold != arrnew || nold != nodenew )
    {
        // If anything has changed, go ahead and update.
        LIST_FOREACH_BEGIN( TableValueAccessContext, t->mutableDynamicValues.root, managerNode )

            item->UpdateContextValue( arrold, nold, arrnew, nodenew, oldasize, oldhsize, nasize, nhsize );

        LIST_FOREACH_END

        LIST_FOREACH_BEGIN( TableValueConstAccessContext, t->immutableDynamicValues.root, managerNode )

            item->UpdateContextValue( arrold, nold, arrnew, nodenew, oldasize, oldhsize, nasize, nhsize );

        LIST_FOREACH_END
    }
}

/*
** {=============================================================
** Rehash
** ==============================================================
*/

void luaH_resizearray (lua_State *L, Table *t, size_t nasize)
{
    tableNativeImplementation *nativeTable = tableNativeImplementation::GetNativeImplementation( t );

    if ( nativeTable )
    {
        global_State *g = G(L);

        NativeExecutive::CReadWriteWriteContextSafe <> writeContext( nativeTable->GetRuntimeLock( g ) );

        int nsize = (nativeTable->node == GetDummyNode( g )) ? 0 : sizenode(nativeTable);
        resize(L, t, nativeTable, nasize, nsize);
    }
}

void luaH_ensureslots (lua_State *L, Table *h, size_t last)
{
    tableNativeImplementation *nativeTable = tableNativeImplementation::GetNativeImplementation( h );

    if ( nativeTable )
    {
        if ( last > nativeTable->sizearray )  /* needs more space? */
        {
            luaH_resizearray(L, h, last);  /* pre-alloc it at once */
        }
    }
}


/*
** }=============================================================
*/

Table::Table( global_State *g, void *construction_params ) : GrayObject( g )
{
    // Initialize the table.
    luaC_link(g, this, LUA_TTABLE);
    this->metatable = nullptr;
}

Table::Table( const Table& right ) : GrayObject( right )
{
    luaC_link( this->gstate, this, LUA_TTABLE );
    this->metatable = right.metatable;

    // Data is cloned using the native implementation methods.
}

Table::~Table( void )
{
    global_State *g = this->gstate;

    // Cleaning up is done by the native table implementation.

    luaC_unlink( g, this );
}

Table *luaH_new (lua_State *L, size_t narray, size_t nhash)
{
    global_State *g = G(L);

    // Get the table type info plugin.
    tableTypeInfoPlugin *typeInfo = tableTypeInfo.GetPluginStruct( g->config );

    // No point in creating table if we cannot get our type information.
    if ( !typeInfo )
        return nullptr;

    // Construct the table using its factory.
    Table *t = lua_new <Table> ( L, typeInfo->tableTypeInfo );

    if ( t )
    {
        tableNativeImplementation *nativeTable = tableNativeImplementation::GetNativeImplementation( t );

        if ( nativeTable )
        {
            setarrayvector(L, nativeTable, narray);
            setnodevector(L, nativeTable, nhash);
        }
    }
    return t;
}

void luaH_free (lua_State *L, Table *t)
{
    // Destroy the table.
    lua_delete <Table> ( L, t );
}

template <typename getDispatcherType, typename customKeyItemType>
static inline ConstValueAddress TableGetFunctionHelper( lua_State *L, Table *t, const customKeyItemType& keyItem, const getDispatcherType& dispatch )
{
    ConstValueAddress retAddr;

    tableNativeImplementation *nativeTable = tableNativeImplementation::GetNativeImplementation( t );

    if ( nativeTable )
    {
        const TValue *retValue = luaO_nilobject;

        global_State *g = G(L);

        NativeExecutive::CReadWriteWriteContextSafe <> writeContext( nativeTable->GetRuntimeLock( g ) );

        // Give access to the value.
        dispatch.AtomicGetItemFromTable( nativeTable, keyItem, retValue );

        {
            TableValueConstAccessContext *ctx = NewTableValueConstAccessContext( L, t, nativeTable );

            dispatch.AtomicSetToTValue( L, &ctx->key, keyItem );
            ctx->value = retValue;

            retAddr = ConstValueAddress( L, ctx );
        }
    }
    else
    {
        retAddr = luaO_getnilcontext( L );
    }

    return retAddr;
}

struct GetTableItemByIntegerDispatch
{
    inline bool AtomicGetItemFromTable( tableNativeImplementation *tableImpl, int key, const TValue*& retValue ) const
    {
        return _findkeybynum( tableImpl, key, retValue );
    }

    inline void AtomicSetToTValue( lua_State *L, TValue *keyValue, int key ) const
    {
        setnvalue( keyValue, cast_num(key) );
    }
};

ConstValueAddress luaH_getnum (lua_State *L, Table *t, int key)
{
    GetTableItemByIntegerDispatch dispatch;

    return TableGetFunctionHelper( L, t, key, dispatch );
}

struct GetTableItemBySizetDispatch
{
    inline bool AtomicGetItemFromTable( tableNativeImplementation *tableImpl, size_t key, const TValue*& retValue ) const
    {
        return _findkeybynum( tableImpl, key, retValue );
    }

    inline void AtomicSetToTValue( lua_State *L, TValue *keyValue, size_t key ) const
    {
        setnvalue( keyValue, cast_num(key) );
    }
};

ConstValueAddress luaH_getznum( lua_State *L, Table *t, size_t key )
{
    GetTableItemBySizetDispatch dispatch;

    return TableGetFunctionHelper( L, t, key, dispatch );
}

struct GetTableItemByStringDispatch
{
    inline bool AtomicGetItemFromTable( tableNativeImplementation *tableImpl, TString *key, const TValue*& retValue ) const
    {
        return _findkeybystr( tableImpl, key, retValue );
    }

    inline void AtomicSetToTValue( lua_State *L, TValue *keyValue, TString *key ) const
    {
        setsvalue( L, keyValue, key );
    }
};

ConstValueAddress luaH_getstr (lua_State *L, Table *t, TString *key)
{
    GetTableItemByStringDispatch dispatch;

    return TableGetFunctionHelper( L, t, key, dispatch );
}

/*
** main search function
*/
struct GetTableItemGenericDispatch
{
    inline bool AtomicGetItemFromTable( tableNativeImplementation *tableImpl, const TValue *key, const TValue*& retValue ) const
    {
        return _findkey( tableImpl, key, retValue );
    }

    inline void AtomicSetToTValue( lua_State *L, TValue *keyValue, const TValue *key ) const
    {
        setobj( L, keyValue, key );
    }
};

ConstValueAddress luaH_get (lua_State *L, Table *t, const TValue *key)
{
    GetTableItemGenericDispatch dispatch;

    return TableGetFunctionHelper( L, t, key, dispatch );
}

template <typename setDispatcherType, typename keyItemType>
static inline ValueAddress TableSetFunctionHelper( lua_State *L, Table *t, const keyItemType& keyItem, const setDispatcherType& dispatch )
{
    ValueAddress retAddr;

    tableNativeImplementation *nativeTable = tableNativeImplementation::GetNativeImplementation( t );

    if ( nativeTable )
    {
        global_State *g = G(L);

        NativeExecutive::CReadWriteWriteContextSafe <> writeContext( nativeTable->GetRuntimeLock( g ) );

        TValue *returnValue = dispatch.AtomicSetItemFromTable( L, t, nativeTable, keyItem );

        // Give access to the value.
        {
            TableValueAccessContext *ctx = NewTableValueAccessContext( L, t, nativeTable );

            dispatch.AtomicSetToTValue( L, &ctx->key, keyItem );
            ctx->value = returnValue;

            retAddr = ValueAddress( L, ctx );
        }
    }

    return retAddr;
}

struct SetTableItemGenericDispatch : public GetTableItemGenericDispatch
{
    inline TValue* AtomicSetItemFromTable( lua_State *L, Table *table, tableNativeImplementation *tableImpl, const TValue *key ) const
    {
        return settablenative( L, table, tableImpl, key );
    }
};

ValueAddress luaH_set (lua_State *L, Table *t, const TValue *key)
{
    SetTableItemGenericDispatch dispatch;

    return TableSetFunctionHelper( L, t, key, dispatch );
}

struct SetTableItemByIntegerDispatch : public GetTableItemByIntegerDispatch
{
    inline TValue* AtomicSetItemFromTable( lua_State *L, Table *table, tableNativeImplementation *tableImpl, int key ) const
    {
        return settablenativenum( L, table, tableImpl, key );
    }
};

ValueAddress luaH_setnum (lua_State *L, Table *t, int key)
{
    SetTableItemByIntegerDispatch dispatch;

    return TableSetFunctionHelper( L, t, key, dispatch );
}

struct SetTableItemBySizetDispatch : public GetTableItemBySizetDispatch
{
    inline TValue* AtomicSetItemFromTable( lua_State *L, Table *table, tableNativeImplementation *tableImpl, size_t key ) const
    {
        return settablenativenum( L, table, tableImpl, key );
    }
};

ValueAddress luaH_setznum (lua_State *L, Table *t, size_t key)
{
    SetTableItemBySizetDispatch dispatch;

    return TableSetFunctionHelper( L, t, key, dispatch );
}

struct SetTableItemByStringDispatch : public GetTableItemByStringDispatch
{
    inline TValue* AtomicSetItemFromTable( lua_State *L, Table *table, tableNativeImplementation *tableImpl, TString *key ) const
    {
        return settablenativestr( L, table, tableImpl, key );
    }
};

ValueAddress luaH_setstr (lua_State *L, Table *t, TString *key)
{
    SetTableItemByStringDispatch dispatch;

    return TableSetFunctionHelper( L, t, key, dispatch );
}

static inline size_t unbound_search( lua_State *L, Table *t, size_t j )
{
    size_t i = j;  /* i is zero or a present index */

    j++;

    /* find `i' and `j' such that i is present and j is not */
    while ( true )
    {
        ConstValueAddress jval = luaH_getznum( L, t, j );

        if ( ttisnil(jval) )
            break;

        i = j;
        j *= 2;

        if ( j > cast(size_t, MAX_INT) )
        {  /* overflow? */
            /* table was built with bad purposes: resort to linear search */
            i = 1;

            while ( true )
            {
                ConstValueAddress ival = luaH_getznum( L, t, i );

                if ( ttisnil(ival) )
                    break;

                i++;
            }

            return i - 1;
        }
    }
    /* now do a binary search between them */
    while ( j - i > 1 )
    {
        size_t m = (i+j)/2;

        ConstValueAddress mval = luaH_getznum( L, t, m );

        if ( ttisnil(mval) )
        {
            j = m;
        }
        else
        {
            i = m;
        }
    }
    return i;
}


/*
** Try to find a boundary in table `t'. A `boundary' is an integer index
** such that t[i] is non-nil and t[i+1] is nil (and 0 if t[1] is nil).
*/
size_t luaH_getn( lua_State *L, Table *t )
{
    size_t num = 0;

    tableNativeImplementation *nativeTable = tableNativeImplementation::GetNativeImplementation( t );

    if ( nativeTable )
    {
        global_State *g = G(L);

        bool hasLock = true;

        nativeTable->EnterCriticalReadRegion( g );

        size_t j = nativeTable->sizearray;

        if ( j > 0 && ttisnil(&nativeTable->array[j - 1]) )
        {
            /* there is a boundary in the array part: (binary) search for it */
            size_t i = 0;

            while ( j - i > 1 )
            {
                size_t m = (i+j)/2;

                if ( ttisnil( &nativeTable->array[m - 1] ) )
                {
                    j = m;
                }
                else
                {
                    i = m;
                }
            }

            num = i;
        }
        /* else must find a boundary in hash part */
        else if ( nativeTable->node == GetDummyNode( G(L) ) )  /* hash part is empty? */
        {
            num = j;  /* that is easy... */
        }
        else
        {
            if ( hasLock )
            {
                nativeTable->LeaveCriticalReadRegion( g );

                hasLock = false;
            }

            // Fall back.
            num = unbound_search(L, t, j);
        }

        if ( hasLock )
        {
            nativeTable->LeaveCriticalReadRegion( g );

            hasLock = false;
        }
    }

    return num;
}

void Table::Index( lua_State *L, ConstValueAddress& key, ValueAddress& val )
{
    ConstValueAddress res = luaH_get( L, this, key ); /* do a primitive get */
    ConstValueAddress tm;

    if ( !ttisnil(res) || (tm = fasttm( L, metatable, TM_INDEX )) == nullptr )
    {
        setobj2s(L, val, res);
    }
    else if ( ttisfunction( tm ) )
    {
        RtStackAddr rtStack = L->rtStack.LockedAcquisition( L );

        luaD_checkstack( L, 3 );    // we allocate the stack here!

        // Working with the new stack
        pushclvalue( L, clvalue( tm ) );
        pushhvalue( L, this );
        pushtvalue( L, key );
        lua_call( L, 2, 1 );

        // Store at appropriate position
        popstkvalue( L, val );
    }
    else
        luaV_gettable( L, tm, key, val );
}

void Table::NewIndex( lua_State *L, ConstValueAddress& key, ConstValueAddress& val )
{
    ConstValueAddress tm;

    bool isnil = false;
    {
        ConstValueAddress oldval = luaH_get( L, this, key );

        isnil = ttisnil( oldval );
    }

    if ( !isnil || ( tm = fasttm( L, metatable, TM_NEWINDEX )) == nullptr )
    {
        ValueAddress newval = luaH_set( L, this, key );

        setobj2t(L, newval, val);
    }
    else if ( ttisfunction( tm ) )
    {
        RtStackAddr rtStack = L->rtStack.LockedAcquisition( L );

        luaD_checkstack( L, 4 );    // allocate a new stack

        // Work with the new stack
        pushclvalue( L, clvalue( tm ) );
        pushhvalue( L, this );
        pushtvalue( L, key );
        pushtvalue( L, val );
        lua_call( L, 3, 0 );
    }
    else
    {
        // Repeat the process with the index object
        // For security reasons, we should increment the callstack depth
        callstack_ref indexRef( *L );

        luaV_settable( L, tm, key, val );
    }
}
