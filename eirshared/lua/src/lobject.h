/*
** $Id: lobject.h,v 2.20.1.2 2008/08/06 13:29:48 roberto Exp $
** Type definitions for Lua objects
** See Copyright Notice in lua.h
*/


#ifndef lobject_h
#define lobject_h


#include <stdarg.h>
#include <vector>
#include <map>
#include <sdk/rwlist.hpp>

#include "llimits.h"
#include "lua.h"
#include "lmem.h"
#include "lutils.h"

#include "ltypesys.h"


/* tags for values visible from Lua */
#define LAST_TAG	LUA_TRUNTIME

#define NUM_TAGS	(LAST_TAG+1)


/*
** Extra tags for non-values
** TODO: remove this, since it is a hack.
*/
#define LUA_TDEADKEY    (LAST_TAG+3)

/*
** Plugin variables.
*/
typedef StaticPluginClassFactory <global_State> globalStateFactory_t;

typedef globalStateFactory_t::pluginOffset_t globalStatePluginOffset_t;

// Virtual machine global configuration struct.
struct lua_config
{
    // The heart of the Lua runtime.
    LuaTypeSystem typeSys;  /* global type descriptor manager */

    // Important Lua types.
    LuaTypeSystem::typeInfoBase *gcobjTypeInfo;
    LuaTypeSystem::typeInfoBase *grayobjTypeInfo;

    // Factory for producing specialized Lua VMs.
    globalStateFactory_t globalStateFactory;

    // Properties for VM creation.
    bool isMultithreaded;

    // Allocation meta-data.
    void *allocData;    /* private allocation data used for memory requests */
};

// Plugin registry at module namespace level.
typedef StaticPluginClassFactory <lua_config> namespaceFactory_t;

typedef namespaceFactory_t::pluginOffset_t namespacePluginOffset_t;

extern namespaceFactory_t namespaceFactory;


/*
** Union of all collectable objects
*/
class GCObject;

/*
** Union of all Lua values
*/
typedef union
{
  GCObject *gc;
  void *p;
  lua_Number n;
  bool b;
} Value;


/*
** Tagged Values
*/
struct TValue
{
    Value value;
    int tt;
};

class TString;
class Table;
class Proto;
class Closure;
class Udata;
class Class;
class UpVal;
class LocVar;
class Dispatch;
class lua_State;


typedef TValue* StkId;  /* index to stack elements */
typedef const TValue* StkId_const;
struct global_State;

// List of garbage collectible items.
typedef SingleLinkedList <class GCObject> gcObjList_t;

// Stack definitions.
#include "lstack.h"

// Forward declarations.
FASTAPI void setnilvalue(TValue *obj);
FASTAPI void setobj(lua_State *L, TValue *dstVal, const TValue *srcVal);

struct rtStackMoveMan
{
    inline static void InitializeValue( lua_State *L, TValue *newValue )
    {
        setnilvalue( newValue );
    }

    inline static void SetItem( lua_State *L, const TValue *sourceItem, TValue *dstItem )
    {
        setobj( L, dstItem, sourceItem );
    }
};

// Abstract type of the stack.
typedef growingStack <TValue, rtStackMoveMan> rtStack_t;

// Stack user type.
template <typename stackType>
struct StackView
{
    typedef typename stackType::itemType_t itemType_t;
    typedef typename stackType::constItemType_t constItemType_t;
    typedef typename stackType::stackItem_t stackItem_t;
    typedef typename stackType::constStackItem_t constStackItem_t;
    typedef typename stackType::CtxItem CtxItem;

private:
    stackItem_t base;
    stackItem_t top;

    inline stackOffset_t RelativeToAbsoluteOffset( lua_State *L, stackType& theStack, stackOffset_t relativeOffset, bool isSecure )
    {
        theStack.EnsureLocked();

        // Convert the offset to a positive value/an array index.
        stackOffset_t relativeBaseOffset = -1;

        if ( this->base != nullptr )
        {
            if ( relativeOffset < 0 )
            {
                if ( this->top != nullptr )
                {
                    relativeBaseOffset = stackType::GetDeferredStackOffset( this->base, this->top ) + ( relativeOffset + 1 );
                }
            }
            else
            {
                relativeBaseOffset = relativeOffset;
            }
        }

        if ( isSecure == false )
        {
            lua_assert( relativeBaseOffset < this->GetUsageCount( L, theStack ) );
        }

        return relativeBaseOffset;
    }

public:
    inline StackView( void )
    {
        this->base = nullptr;
        this->top = nullptr;
    }

    inline void Initialize( stackItem_t base, stackItem_t top )
    {
        lua_assert( base != nullptr );

        this->base = base;
        this->top = top;
    }

    inline void InitializeEx( lua_State *L, stackType& theStack, stackOffset_t baseOffset, stackOffset_t viewSpan )
    {
        theStack.Lock( L );

        // Make sure we allocate enough items on the stack so we have its base.
        {
            stackOffset_t allocStackOffset = -1;

            bool hasTopOffset = theStack.GetTopOffset( L, allocStackOffset );

            if ( !hasTopOffset || allocStackOffset < baseOffset )
            {
                theStack.SetTopOffset( L, baseOffset );
            }
        }

        this->base = theStack.GetStackItem( L, baseOffset ).Pointer();
        this->top = nullptr;

        lua_assert( this->base != nullptr );

        IncrementTop( L, theStack, viewSpan );

        theStack.Unlock( L );
    }

    inline constStackItem_t Base( lua_State *L, stackType& theStack )
    {
        theStack.Lock( L );

        constStackItem_t theBase = this->base;

        theStack.Unlock( L );

        return theBase;
    }

    // WARNING: use this function cautiously! it can easily corrupt the runtime!
    inline stackItem_t BaseMutable( lua_State *L, stackType& theStack )
    {
        theStack.Lock( L );

        stackItem_t theBase = this->base;

        theStack.Unlock( L );

        return theBase;
    }

    inline constStackItem_t Top( lua_State *L, stackType& theStack )
    {
        theStack.Lock( L );

        constStackItem_t theTop = this->top;

        theStack.Unlock( L );

        return theTop;
    }

    inline stackItem_t TopMutable( lua_State *L, stackType& theStack )
    {
        theStack.EnsureLocked();

        return this->top;
    }

    inline void SetBaseOffsetAbsolute( lua_State *L, stackType& theStack, stackOffset_t byOffset )
    {
        theStack.Lock( L );
        {
            CtxItem stackItem = theStack.GetStackItem( L, byOffset );

            this->base = stackItem.Pointer();
        }
        theStack.Unlock( L );
    }

    inline stackOffset_t GetStackOffsetAbsolute( lua_State *L, stackType& theStack, constStackItem_t refPtr )
    {
        theStack.Lock( L );

        stackOffset_t offset = theStack.GetStackOffset( L, refPtr );

        theStack.Unlock( L );

        return offset;
    }

    inline stackOffset_t GetStackOffset( lua_State *L, stackType& theStack, constStackItem_t refPtr )
    {
        theStack.Lock( L );

        stackOffset_t offset = stackType::GetDeferredStackOffset( this->base, refPtr );

        theStack.Unlock( L );

        return offset;
    }

    inline CtxItem GetStackItem( lua_State *L, stackType& theStack, stackOffset_t offset )
    {
        theStack.Lock( L );

        stackOffset_t realOffset = RelativeToAbsoluteOffset( L, theStack, offset, true );

        stackOffset_t stackUsageCount = this->GetUsageCount( L, theStack );

        CtxItem result;

        if ( realOffset < stackUsageCount )
        {
            result = theStack.GetStackItemBasedOn( L, realOffset, this->base );
        }

        theStack.Unlock( L );

        return result;
    }

    inline CtxItem GetStackItemOutbounds( lua_State *L, stackType& theStack, stackOffset_t offset )
    {
        theStack.Lock( L );

        stackOffset_t realOffset = RelativeToAbsoluteOffset( L, theStack, offset, true );

        CtxItem result = theStack.GetStackItemBasedOn( L, realOffset, this->base );

        theStack.Unlock( L );

        return result;
    }

    inline void EnsureOutboundsSlots( lua_State *L, stackType& theStack, stackOffset_t maxIndex )
    {
        theStack.Lock( L );

        // Get the size that underlying stack must at least have.
        stackOffset_t reqStackSize = ( theStack.GetStackOffset( L, this->base ) + maxIndex + 1 );

        // Allocate the stack.
        stackOffset_t currentStackSize = theStack.GetUsageCount( L );

        if ( reqStackSize > currentStackSize )
        {
            theStack.SetTopOffset( L, reqStackSize - 1 );
        }

        theStack.Unlock( L );
    }

    inline void IncrementTop( lua_State *L, stackType& theStack, stackOffset_t n )
    {
        theStack.EnsureLocked();

        lua_assert( n >= 0 );

        for ( stackOffset_t index = 0; index < n; index++ )
        {
            if ( this->top == nullptr )
            {
                this->top = this->base;
            }
            else
            {
                lua_assert( this->top <= theStack.Top() );

                if ( this->top == theStack.Top() )
                {
                    CtxItem resultVal = theStack.ObtainItemLocked( L );

                    this->top = resultVal.Pointer();
                }
                else
                {
                    this->top++;
                }
            }
        }
    }

    inline void DecrementTop( lua_State *L, stackType& theStack, stackOffset_t n )
    {
        theStack.EnsureLocked();

        lua_assert( n >= 0 );

        for ( stackOffset_t index = 0; index < n; index++ )
        {
            if ( this->top != nullptr )
            {
                if ( this->top > this->base )
                {
                    this->top--;
                }
                else if ( this->top == this->base )
                {
                    this->top = nullptr;
                }
            }
        }
    }

    // WARNING: this operation may throw a memory exception; secure important stack routines with proper locks!
    inline CtxItem ObtainItem( lua_State *L, stackType& theStack )
    {
        theStack.Lock( L );

        IncrementTop( L, theStack, 1 );

        stackOffset_t topOffset = theStack.GetStackOffset( L, this->top );

        CtxItem resultVal = theStack.GetStackItem( L, topOffset );

        lua_assert( this->top == resultVal.Pointer() );

        theStack.Unlock( L );

        return resultVal;
    }

    inline void PopCount( lua_State *L, stackType& theStack, stackOffset_t n )
    {
        theStack.Lock( L );

        this->DecrementTop( L, theStack, n );

        theStack.Unlock( L );
    }

    inline stackOffset_t GetUsageCountBasedOn( lua_State *L, stackType& theStack, constStackItem_t theBase )
    {
        theStack.Lock( L );

        stackOffset_t allocOffset = 0;

        if ( theBase != nullptr && this->top != nullptr )
        {
            allocOffset = stackType::GetDeferredStackOffset( theBase, this->top );

            allocOffset++;
        }

        theStack.Unlock( L );

        return allocOffset;
    }

    inline stackOffset_t GetUsageCount( lua_State *L, stackType& theStack )
    {
        theStack.Lock( L );

        stackOffset_t result = GetUsageCountBasedOn( L, theStack, this->base );

        theStack.Unlock( L );

        return result;
    }

    inline void CrossMoveTopExtern( lua_State *L, stackType& srcStack, itemType_t *dstArray, stackOffset_t numMove )
    {
        if ( numMove > 0 )
        {
            srcStack.Lock( L );
            {
                 // Make sure we actually have the required amount of items on our virtual stack.
                stackOffset_t currentStackUsageCount = this->GetUsageCount( L, srcStack );

                lua_assert( currentStackUsageCount >= numMove );

                if ( currentStackUsageCount >= numMove )
                {
                    // Perform an optimized copy of stack members.
                    stackType::NativeCrossCopyExtern( L, dstArray, this->top - (numMove - 1), numMove );

                    // Decrement the top by the number of items that should be moved
                    // to the destination stack.
                    this->DecrementTop( L, srcStack, numMove );
                }
            }
            srcStack.Unlock( L );
        }
    }

    inline void CrossCopyTopExtern( lua_State *L, stackType& srcStack, itemType_t *dstArray, stackOffset_t numCopy )
    {
        if ( numCopy > 0 )
        {
            srcStack.Lock( L );
            {
                // Make sure we have the necessary items that should be copied over.
                stackOffset_t currentStackUsageCount = this->GetUsageCount( L, srcStack );

                lua_assert( currentStackUsageCount >= numCopy );

                if ( currentStackUsageCount >= numCopy )
                {
                    // Copy items from the source stack onto the destination array.
                    stackType::NativeCrossCopyExtern( L, dstArray, this->top - (numCopy - 1), numCopy );
                }
            }
            srcStack.Unlock( L );
        }
    }

    inline void CrossMoveTop( lua_State *L, stackType& srcStack, StackView *dstView, stackType& dstStack, stackOffset_t numMove )
    {
        if ( numMove > 0 )
        {
            lua_assert( &srcStack != &dstStack );

            srcStack.Lock( L );
            dstStack.Lock( L );
            {
                // Make sure we actually have the required amount of items on our virtual stack.
                stackOffset_t currentStackUsageCount = this->GetUsageCount( L, srcStack );

                lua_assert( currentStackUsageCount >= numMove );

                if ( currentStackUsageCount >= numMove )
                {
                    // Move the items that are currently being pointed at by the srcStack top
                    // to the top of dstStack as pointed at by dstView.
                    {
                        // Preallocate the stack items on the destination stack view.
                        dstView->IncrementTop( L, dstStack, numMove );

                        stackOffset_t topRebaseCopyOffset = (numMove - 1);

                        // Perform an optimized copy of stack members.
                        stackType::NativeCrossCopyExtern( L, dstView->top - topRebaseCopyOffset, this->top - topRebaseCopyOffset, numMove );

                        // Decrement the top by the number of items that have been moved
                        // to the destination stack.
                        this->DecrementTop( L, srcStack, numMove );
                    }
                }
            }
            dstStack.Unlock( L );
            srcStack.Unlock( L );
        }
    }

    inline void CrossCopyTop( lua_State *L, stackType& srcStack, StackView *dstView, stackType& dstStack, stackOffset_t numCopy )
    {
        if ( numCopy > 0 )
        {
            srcStack.Lock( L );
            dstStack.Lock( L );
            {
                // Make sure we have the necessary items that should be copied over.
                stackOffset_t currentStackUsageCount = this->GetUsageCount( L, srcStack );

                lua_assert( currentStackUsageCount >= numCopy );

                if ( currentStackUsageCount >= numCopy )
                {
                    // Preallocate the stack items on the destination view.
                    dstView->IncrementTop( L, dstStack, numCopy );

                    stackOffset_t topRebaseCopyOffset = (numCopy - 1);

                    // Copy items from the source stack onto the destination stack based on their views.
                    stackType::NativeCrossCopyExtern( L, dstView->top - topRebaseCopyOffset, this->top - topRebaseCopyOffset, numCopy );
                }
            }
            dstStack.Unlock( L );
            srcStack.Unlock( L );
        }
    }

    // WARNING: returns absolute offset!
    inline stackOffset_t GetTopOffset( lua_State *L, stackType& theStack )
    {
        theStack.Lock( L );

        stackOffset_t resultOffset;

        if ( this->top != nullptr )
        {
            resultOffset = theStack.GetStackOffset( L, this->top );
        }
        else
        {
            resultOffset = theStack.GetStackOffset( L, this->base ) - 1;
        }

        theStack.Unlock( L );

        return resultOffset;
    }

    inline void SetTopOffset( lua_State *L, stackType& theStack, stackOffset_t topOffset )
    {
        theStack.Lock( L );

        // Update the top of this stack view.
        stackOffset_t curTopOff = this->GetTopOffset( L, theStack );

        stackOffset_t topDiff = ( topOffset - curTopOff );

        // Alright, we have to either increment or decrement the stack view top.
        if ( topDiff > 0 )
        {
            this->IncrementTop( L, theStack, topDiff );
        }
        else if ( topDiff < 0 )
        {
            this->DecrementTop( L, theStack, -topDiff );
        }

        theStack.Unlock( L );
    }

    inline stackOffset_t GetAbsoluteStackOffset( lua_State *L, stackType& theStack, stackOffset_t relativeOffset, bool isSecure = false )
    {
        theStack.Lock( L );

        stackOffset_t baseOffset = theStack.GetStackOffset( L, this->base );

        stackOffset_t relativeOffsetNormalized = this->RelativeToAbsoluteOffset( L, theStack, relativeOffset, isSecure );

        theStack.Unlock( L );

        return ( baseOffset + relativeOffsetNormalized );
    }

    inline stackOffset_t GetRelativeStackOffset( lua_State *L, stackType& theStack, stackOffset_t absoluteOffset )
    {
        theStack.Lock( L );

        stackOffset_t baseOffset = theStack.GetStackOffset( L, this->base );

        theStack.Unlock( L );

        return ( absoluteOffset - baseOffset );
    }

    inline void SetTopOffsetRelative( lua_State *L, stackType& theStack, stackOffset_t relativeOffset )
    {
        theStack.Lock( L );

        stackOffset_t absoluteOffset = this->GetAbsoluteStackOffset( L, theStack, relativeOffset, true );

        this->SetTopOffset( L, theStack, absoluteOffset );

        theStack.Unlock( L );
    }

    inline void InsertItemAtExternal( lua_State *L, stackType& theStack, stackOffset_t index, constItemType_t *itemToBeInserted )
    {
        theStack.Lock( L );
        {
            CtxItem toBeInsertedAtCtx = this->GetStackItem( L, theStack, index );

            // Increment the stack top.
            this->IncrementTop( L, theStack, 1 );

            stackItem_t toBeInsertedAt = toBeInsertedAtCtx.Pointer();
            stackItem_t stackTop = this->top;

            // Do the insertion.
            stackType::ShiftedInsert( L, stackTop, toBeInsertedAt, itemToBeInserted );
        }
        theStack.Unlock( L );
    }

    inline void TopInsertItem( lua_State *L, stackType& theStack, stackOffset_t indexToInsertAt )
    {
        theStack.Lock( L );
        {
            // Perform an item insertion from the top of this stack view to the indicated position.
            stackItem_t toBeInsertedAt = this->GetStackItem( L, theStack, indexToInsertAt ).Pointer();

            lua_assert( toBeInsertedAt != nullptr );

            if ( toBeInsertedAt != nullptr )
            {
                stackItem_t stackTop = this->top;

                lua_assert( stackTop != nullptr );

                // Save the top of the stack view to a temporary slot.
                itemType_t tmp;
                stackType::movman_t::SetItem( L, stackTop, &tmp );

                // Perform the insertion.
                stackType::ShiftedInsert( L, stackTop, toBeInsertedAt, &tmp );
            }
        }
        theStack.Unlock( L );
    }

    inline void TopSwapItem( lua_State *L, stackType& theStack, stackOffset_t indexToSwapWith )
    {
        theStack.Lock( L );
        {
            // Swaps the top of the stack view with the designated item.
            stackItem_t toBeSwappedWith = this->GetStackItem( L, theStack, indexToSwapWith ).Pointer();

            lua_assert( toBeSwappedWith != nullptr );

            if ( toBeSwappedWith != nullptr )
            {
                stackItem_t stackTop = this->top;

                lua_assert( stackTop != nullptr );

                // Save the top of the stack view to a temporary slot.
                itemType_t tmp;
                stackType::movman_t::SetItem( L, stackTop, &tmp );

                // Perform the swap.
                stackType::movman_t::SetItem( L, toBeSwappedWith, stackTop );
                stackType::movman_t::SetItem( L, &tmp, toBeSwappedWith );
            }
        }
        theStack.Unlock( L );
    }

    inline void RemoveItem( lua_State *L, stackType& theStack, stackOffset_t indexToRemoveAt )
    {
        theStack.Lock( L );
        {
            // Removes the item that is pointed at by indexToRemoveAt by collapsing the stack array.
            stackItem_t toRemoveAt = this->GetStackItem( L, theStack, indexToRemoveAt ).Pointer();

            if ( toRemoveAt != nullptr )
            {
                {
                    stackItem_t stackTop = this->top;

                    lua_assert( stackTop != nullptr );

                    // Collapse the stack array.
                    while ( ++toRemoveAt <= stackTop )
                    {
                        stackType::movman_t::SetItem( L, toRemoveAt, toRemoveAt - 1 );
                    }
                }

                // Decrement the stack view top.
                this->DecrementTop( L, theStack, 1 );
            }
        }
        theStack.Unlock( L );
    }

    inline void MoveItems( lua_State *L, stackType& theStack, stackOffset_t srcOffset, stackOffset_t dstOffset, stackOffset_t moveCount )
    {
        if ( srcOffset != dstOffset )
        {
            theStack.Lock( L );
            {
                stackOffset_t usageCount = this->GetUsageCount( L, theStack );

                for ( stackOffset_t n = 0; n < moveCount; n++ )
                {
                    stackOffset_t relative_seek = 0;

                    if ( srcOffset < dstOffset )
                    {
                        relative_seek = ( moveCount - 1 - n );
                    }
                    else
                    {
                        relative_seek = n;
                    }

                    stackOffset_t real_srcOffset = srcOffset + relative_seek;
                    stackOffset_t real_dstOffset = dstOffset + relative_seek;

                    real_srcOffset = this->RelativeToAbsoluteOffset( L, theStack, real_srcOffset );
                    real_dstOffset = this->RelativeToAbsoluteOffset( L, theStack, real_dstOffset );

                    constStackItem_t srcItem =
                        stackType::StackSelector(
                            real_srcOffset,
                            usageCount,
                            this->base,
                            this->top
                        );
                    stackItem_t dstItem =
                        stackType::StackSelector(
                            real_dstOffset,
                            usageCount,
                            this->base,
                            this->top
                        );

                    // Move the item over.
                    if ( srcItem != nullptr && dstItem != nullptr )
                    {
                        stackType::movman_t::SetItem( L, srcItem, dstItem );
                    }
                }
            }
            theStack.Unlock( L );
        }
    }

    inline void OnRebasing( lua_State *L, stackType& theStack, constStackItem_t oldStack )
    {
        stackItem_t theStackBase = theStack.Base();

        if ( theStackBase != nullptr )
        {
            this->top = theStack.GetDeferredStackItem( oldStack, this->top );
            this->base = theStack.GetDeferredStackItem( oldStack, this->base );

            lua_assert( this->base != nullptr );
        }
        else
        {
            this->top = nullptr;
            this->base = nullptr;
        }
    }

    inline bool IsInStack( lua_State *L, stackType& theStack, constStackItem_t refPtr )
    {
        bool isInStack = false;

        theStack.EnsureLocked();

        if ( this->base != nullptr && this->top != nullptr )
        {
            /* only ANSI way to check whether a pointer points to an array */
            for ( StkId iter = this->base; iter <= this->top; iter++ )
            {
                if ( refPtr == iter )
                {
                    isInStack = true;
                    break;
                }
            }
        }

        return isInStack;
    }

    inline void Clear( lua_State *L, stackType& theStack )
    {
        theStack.Lock( L );

        this->top = nullptr;

        theStack.Unlock( L );
    }

    // Stack base incrementation item.
    struct baseProtect
    {
    protected:
        inline void Initialize( lua_State *L, StackView *theView, stackType& theStack, stackOffset_t setToOffset )
        {
            this->theView = theView;

            theStack.Lock( L );

            this->_savedAbsoluteBase = theStack.GetStackOffset( L, theView->base );

            // Change the base.
            {
                CtxItem stackItem = theView->GetStackItem( L, theStack, setToOffset );

                theView->base = stackItem.Pointer();
            }

            theStack.Unlock( L );
        }

    public:
        inline baseProtect( lua_State *L, StackView *theView, stackType& theStack, stackOffset_t setToOffset ) : L( L ), theStack( theStack )
        {
            Initialize( L, theView, theStack, setToOffset );
        }

        inline ~baseProtect( void )
        {
            theStack.Lock( L );

            // Restore the stack base.
            {
                CtxItem stackItem = theStack.GetStackItem( L, this->_savedAbsoluteBase );

                theView->base = stackItem.Pointer();
            }

            theStack.Unlock( L );
        }

        inline baseProtect( const baseProtect& right ) : L( right.L ), theStack( right.theStack )
        {
            this->theView = right.theView;
            this->_savedAbsoluteBase = right._savedAbsoluteBase;
        }

        lua_State *L;
        stackType& theStack;
        StackView *theView;
        stackOffset_t _savedAbsoluteBase;
    };

    FASTAPI baseProtect ProtectSlots( lua_State *L, stackType& theStack, stackOffset_t protectOffset )
    {
        return baseProtect( L, this, theStack, protectOffset );
    }
};

// User-types for easier code writing.
typedef StackView <rtStack_t> RtStackView;

/*
** informations about a call
*/
enum eCallFrameModel
{
    CALLFRAME_STATIC,       // used by the Lua scripts
    CALLFRAME_DYNAMIC       // used by the C runtime
};

struct CallInfo
{
    RtStackView stack;   /* stack frame of this function */
    StkId func;  /* function index in the stack (absolute) */
    const Instruction *savedpc;
    int nresults;  /* expected number of results from this function */

    // Local variables of LClosure environments.
    int tailcalls;  /* number of tail calls lost under this entry */

    stackOffset_t lastResultTop;    // last top offset of a function result

    // Virtual stack top that can be set by opcodes.
    bool hasVirtualStackTop;
    stackOffset_t virtualStackTop;
};

struct ciStackMoveMan
{
    inline static void InitializeValue( lua_State *L, CallInfo *newValue )
    {
        newValue->func = nullptr;
        newValue->nresults = 0;
        newValue->savedpc = nullptr;
        newValue->tailcalls = 0;
        newValue->lastResultTop = -1;
        newValue->hasVirtualStackTop = false;
        newValue->virtualStackTop = -1;
    }

    inline static void SetItem( lua_State *L, const CallInfo *sourceItem, CallInfo *dstItem )
    {
        *dstItem = *sourceItem;
    }
};

typedef growingStack <CallInfo, ciStackMoveMan> ciStack_t;

// Typedefs for easier addressing of protected stack items.
typedef rtStack_t::CtxItem RtCtxItem;
typedef ciStack_t::CtxItem CiCtxItem;

// Type that holds garbage collectible Lua types.
// NOTE: this type must only be allocated by LuaTypeSystem!
// It must be preceeded by a LuaRTTI struct!
class GCObject abstract : public gcObjList_t::node
{
public:
    global_State *gstate;       // VM that this object belongs to.

    inline GCObject( global_State *g )
    {
        this->gstate = g;
        this->tt = LUA_TNONE;
        this->marked = 0;

        // At construction time, we want to keep an object alive forever.
        // Once an object is safely placed into the Lua environment, you have to decrement the GC ref count!
        this->gcRefCount = 1;

        // Flags that are internally used by the GC.
        this->gcflags.isGCActive = false;
        this->gcflags.isGCGrayActive = false;
    }

    inline GCObject( const GCObject& right ) : gcObjList_t::node()
    {
        this->gstate = right.gstate;
        this->tt = right.tt;
        this->marked = right.marked;

        // Even if we clone from another object, we do not inherit their GC list status.
        this->gcflags.isGCActive = false;
        this->gcflags.isGCGrayActive = false;
    }

    inline ~GCObject( void )
    {
        // Make sure we are not on the GC list.
        lua_assert( this->gcflags.isGCActive == false );
        lua_assert( this->gcflags.isGCGrayActive == false );
    }

    virtual TString*    GetTString()        { return nullptr; }
    virtual Udata*      GetUserData()       { return nullptr; }
    virtual Closure*    GetClosure()        { return nullptr; }
    virtual Table*      GetTable()          { return nullptr; }
    virtual Class*      GetClass()          { return nullptr; }
    virtual Proto*      GetProto()          { return nullptr; }
    virtual UpVal*      GetUpValue()        { return nullptr; }
    virtual LocVar*     GetLocVar()         { return nullptr; }
    virtual Dispatch*   GetDispatch()       { return nullptr; }
    virtual lua_State*  GetThread()         { return nullptr; }

    virtual void        MarkGC( global_State *g )       { }

    // Global indexing and new-indexing routines (since every object can potencially be accessed)
    virtual void        Index( lua_State *L, ConstValueAddress& key, ValueAddress& val );
    virtual void        NewIndex( lua_State *L, ConstValueAddress& key, ConstValueAddress& val );

    lu_byte tt;
    lu_byte marked;

    // Special GC flags.
    struct
    {
        unsigned char isGCActive : 1;
        unsigned char isGCGrayActive : 1;
    } gcflags;

private:
    volatile unsigned int gcRefCount;   // reference count to keep an object alive by C++ runtime.

public:
    void ReferenceGC( lua_State *L );
    void DereferenceGC( lua_State *L );

    unsigned int GetGCRefCount( void ) const    { return this->gcRefCount; }
};

typedef SingleLinkedList <class GrayObject> grayObjList_t;

class GrayObject abstract : public GCObject
{
public:
    inline GrayObject( global_State *g ) : GCObject( g )
    {
        return;
    }

    inline GrayObject( const GrayObject& right ) : GCObject( right ), gclist()
    {
        return;
    }

    inline ~GrayObject( void )
    {
        return;
    }

    void                MarkGC( global_State *g );
    virtual size_t      Propagate( global_State *g ) = 0;

    virtual bool        GCRequiresBackBarrier( void ) const = 0;

    grayObjList_t::node gclist;
};

/*
** String headers for string table
*/
#ifdef LUA_USE_C_MACROS

#define _sizestring( baseSize, n )          (baseSize + (n+1)*sizeof(char))
#define _sizeudata( baseSize, n )           (baseSize + (n)*sizeof(char))

#else

FASTAPI size_t _sizestring( size_t baseSize, size_t n )     { return (baseSize + (n+1)*sizeof(char)); }
FASTAPI size_t _sizeudata( size_t baseSize, size_t n )      { return (baseSize + (n)*sizeof(char)); }

#endif //LUA_USE_C_MACROS

class LUA_MAXALIGN TString : public GCObject
{
public:
    TString( global_State *g, void *construction_params );

    // cannot clone strings; they are unique.
    TString( const TString& right );

    ~TString( void );

    lu_byte reserved;
    unsigned int hash;
    size_t len;
};

class LUA_MAXALIGN Udata: public GCObject
{
public:
    Udata( global_State *g, void *construction_params );

    Udata( const Udata& right );

    ~Udata( void );

    void MarkGC( global_State *g );

    Table *metatable;
    GCObject *env;
    size_t len;
};

/*
** Function Prototypes
*/
class Proto : public GrayObject
{
public:
    Proto( global_State *g, void *construction_params );

    Proto( const Proto& right );

    ~Proto( void );

    int TraverseGC( global_State *g );
    size_t Propagate( global_State *g );

    bool GCRequiresBackBarrier( void ) const                { return true; }

    TValue *k;  /* constants used by the function */
    Instruction *code;
    Proto **p;  /* functions defined inside the function */
    int *lineinfo;  /* map from opcodes to source lines */
    LocVar *locvars;  /* information about local variables */
    TString **upvalues;  /* upvalue names */
    TString  *source;
    size_t sizeupvalues;
    size_t sizek;  /* size of `k' */
    size_t sizecode;
    size_t sizelineinfo;
    size_t sizep;  /* size of `p' */
    size_t sizelocvars;
    int linedefined;
    int lastlinedefined;
    lu_byte nups;  /* number of upvalues */
    lu_byte numparams;
    lu_byte is_vararg;
    lu_byte maxstacksize;
};


/* masks for new-style vararg */
#define VARARG_HASARG		1
#define VARARG_ISVARARG		2
#define VARARG_NEEDSARG		4


class LocVar
{
public:
    TString *varname;
    size_t startpc;  /* first point where variable is active */
    size_t endpc;    /* first point where variable is dead */
};


/*
** Closures
*/

enum eClosureLockingMethod : unsigned char
{
    CLLOCK_GLOBAL,
    CLLOCK_CONTEXT
};

class CClosure;
class LClosure;

class Closure abstract : public GrayObject
{
public:
                            Closure         ( global_State *g );

                            // cannot clone closures; they are too complicated.
                            Closure         ( const Closure& right );

    virtual                 ~Closure        ( void );

    int                     TraverseGC      ( global_State *g );

    bool                    GCRequiresBackBarrier( void ) const { return true; }

    virtual TValue*         ReadUpValue     ( unsigned char index ) = 0;

    inline void             SetEnvLocked    ( bool locked )     { isEnvLocked = locked; }
    inline bool             IsEnvLocked     ( void ) const      { return isEnvLocked; }

    Closure*                GetClosure      ( void )            { return this; }
    virtual CClosure*       GetCClosure     ( void )            { return nullptr; }
    virtual LClosure*       GetLClosure     ( void )            { return nullptr; }

    union
    {
        unsigned char genFlags;
        struct
        {
            bool isC : 1;
            bool isEnvLocked : 1;
        };
    };
    lu_byte nupvalues;
    GCObject *env;
};

class CClosure abstract : public Closure
{
public:
    CClosure( global_State *g, void *construction_params );

    ~CClosure( void );

    int TraverseGC( global_State *g );
    size_t Propagate( global_State *g );

    CClosure* GetCClosure()     { return this; }

    lua_CFunction f;
    GCObject *accessor; // Usually the storage of the thread
};

class CClosureBasic : public CClosure
{
public:
    CClosureBasic( global_State *g, void *construction_params );

    ~CClosureBasic();

    size_t Propagate( global_State *g );

    TValue* ReadUpValue( unsigned char index );

    TValue upvalues[1];
};

class LClosure : public Closure
{
public:
    LClosure( global_State *g, void *construction_params );

    ~LClosure();

    int TraverseGC( global_State *g );
    size_t Propagate( global_State *g );

    TValue* ReadUpValue( unsigned char idx );

    LClosure* GetLClosure()     { return this; }

    Proto *p;
    UpVal *upvals[1];
};


/*
** Tables
*/

class Table : public GrayObject
{
public:
    Table( global_State *g, void *construction_params );

    Table( const Table& right );

    ~Table( void );

    bool GCRequiresBackBarrier( void ) const    { return true; }

    int TraverseGC( global_State *g );
    size_t Propagate( global_State *g );

    Table* GetTable()   { return this; }

    void    Index( lua_State *L, ConstValueAddress& key, ValueAddress& sval );
    void    NewIndex( lua_State *L, ConstValueAddress& key, ConstValueAddress& val );

    Table *metatable;
};

#include "lstrtable.h"

// Native C++ index wrapper
class Dispatch abstract : public GrayObject
{
public:
    Dispatch( global_State *g );

    Dispatch( const Dispatch& right );

    ~Dispatch( void );

    size_t  Propagate( global_State *g );

    Dispatch*   GetDispatch()               { return this; }
};

#include "ldispatch.h"

struct _methodRegisterInfo
{
    _methodRegisterInfo( void )
    {
        numUpValues = 0;
        isTrans = false;
    }

    unsigned char numUpValues;
    bool isTrans;
    unsigned char transID;
};

struct _methodCacheEntry : _methodRegisterInfo
{
    lua_CFunction   method;
};

class Class : public GCObject, public virtual ILuaClass
{
public:
    Class( global_State *g, void *construction_params );

    Class( const Class& right );

    ~Class( void );

    void    Propagate( lua_State *L );

    void    MarkGC( global_State *g );
    int     TraverseGC( global_State *g );

    void    Index( lua_State *L, ConstValueAddress& key, ValueAddress& val );
    void    NewIndex( lua_State *L, ConstValueAddress& key, ConstValueAddress& val );

    Class*  GetClass()   { return this; }

    unsigned int    GetRefCount() const     { return inMethod; }

    void    IncrementMethodStack( lua_State *lua );
    void    DecrementMethodStack( lua_State *lua );
    void    ClearReferences( lua_State *lua );
    void    CheckDestruction( lua_State *lua );
    bool    PreDestructor( lua_State *L );

    void    Push( lua_State *L );
    void    PushMethod( lua_State *L, const char *key );
    void    PushSuperMethod( lua_State *L );

    void    CallMethod( lua_State *L, const char *key );

    void    SetTransmit( int type, void *entity );
    bool    GetTransmit( int type, void*& entity );
    int     GetTransmit() const;
    bool    IsTransmit( int type ) const;

    Dispatch*   AcquireEnvDispatcher( lua_State *L );
    Dispatch*   AcquireEnvDispatcherEx( lua_State *L, GCObject *env );

    Closure*    GetMethod( lua_State *L, TString *name, Table*& table );
    void    SetMethod( lua_State *L, TString *name, Closure *method, Table *table );

    FASTAPI void    RegisterMethod( lua_State *L, TString *name, bool handlers = false );
    FASTAPI void    RegisterMethod( lua_State *L, TString *methName, lua_CFunction proto, _methodRegisterInfo& info, bool handlers = false );
    FASTAPI void    RegisterMethodAt( lua_State *L, TString *methName, lua_CFunction proto, Table *methodTable, _methodRegisterInfo& info, bool handlers = false );

    void    RegisterMethod( lua_State *L, const char *name, bool handlers = false );
    void    RegisterMethod( lua_State *L, const char *name, lua_CFunction proto, int nupval, bool handlers = false );
    void    RegisterInterface( lua_State *L, const luaL_Reg *intf, int nupval, bool handlers = true );
    void    RegisterInterfaceAt( lua_State *L, const luaL_Reg *intf, int nupval, Table *methodTable, bool handlers = true );
    void    RegisterMethodTrans( lua_State *L, const char *name, lua_CFunction proto, int nupval, int trans, bool handlers = false );
    void    RegisterInterfaceTrans( lua_State *L, const luaL_Reg *intf, int nupval, int trans, bool handlers = true );
    void    RegisterInterfaceTransAt( lua_State *L, const luaL_Reg *intf, int nupval, int trans, Table *methodTable, bool handlers = true );
    void    RegisterLightMethod( lua_State *L, const char *name );
    void    RegisterLightMethodTrans( lua_State *L, const char *name, int trans );
    void    RegisterLightInterface( lua_State *L, const luaL_Reg *intf, void *udata );
    void    RegisterLightInterfaceTrans( lua_State *L, const luaL_Reg *intf, void *udata, int trans );

    void    EnvPutFront( lua_State *L );
    void    EnvPutBack( lua_State *L );

    bool    IsDestroying() const;
    bool    IsDestroyed() const;

    bool    IsRootedIn( Class *root ) const;
    bool    IsRootedIn( lua_State *L, int idx ) const;

    void    PushEnvironment( lua_State *L );
    void    PushOuterEnvironment( lua_State *L );
    void    PushInternStorage( lua_State *L );
    void    PushChildAPI( lua_State *L );
    void    PushParent( lua_State *L );
    ConstValueAddress   GetEnvValue( lua_State *L, const TValue *key );
    ConstValueAddress   GetEnvValueString( lua_State *L, const char *key );

    void    RequestDestruction();

    ValueAddress        SetSuperMethod( lua_State *L );
    ConstValueAddress   GetSuperMethod( lua_State *L );

    Dispatch *env;
    Dispatch *outenv;
    Table *storage;
    Table *internStorage;
    Class *parent;
    Class *childAPI;
    unsigned int inMethod;
    unsigned int refCount;
	bool reqDestruction : 1;
	bool destroyed : 1;
	bool destroying : 1;
	unsigned char transCount : 5;

    typedef Closure* (*forceSuperCallback)( lua_State *L, Closure *newMethod, Class *j, Closure *prevMethod );
    typedef Closure* (*forceSuperCallbackNative)( lua_State *L, lua_CFunction proto, Class *j, Closure *prevMethod, _methodRegisterInfo& info );

    struct forceSuperItem
    {
        forceSuperCallback  cb;
        forceSuperCallbackNative    cbNative;
    };

    typedef StringTable <forceSuperItem> ForceSuperTable;

    ForceSuperTable *forceSuper;
    ClassStringTable *methodCache;

#pragma pack(1)
	struct trans_t
	{
		unsigned char id;
		void *ptr;
	};
#pragma pack()

	trans_t *trans;

    RwList <Class> children;
    size_t childCount;
    RwListEntry <Class> child_iter;

    typedef std::vector <GCObject*> envList_t;
    envList_t envInherit;

    // Cached values
    Closure *destructor;
};

/*
** `per thread' state
*/

/* extra stack space to handle TM calls and some other extras */
#define EXTRA_STACK   5


#define BASIC_CI_SIZE           8

#define BASIC_STACK_SIZE        (2*LUA_MINSTACK)



class lua_State : public GrayObject, virtual public ILuaState
{
public:
    lua_State( global_State *g ) : GrayObject( g )
    {
        // Set the runtime state.
        defaultAlloc.SetThread( this );
    }

    lua_State( const lua_State& right );

    ~lua_State( void );

    // lua_State is always the main thread
    virtual void    SetMainThread( bool enabled )       {}
    virtual bool    IsThread()                          { return false; }

    virtual void    SetYieldDisabled( bool disable )    {}
    virtual bool    IsYieldDisabled()                   { return true; }

    bool GCRequiresBackBarrier( void ) const    { return true; }

    size_t Propagate( global_State *g );

    // Private expansion of the stacks.
    struct rtStackExp_t : public rtStack_t
    {
        inline rtStackExp_t( void )
        {
            // Lets do things clean.
            this->runtimeThread = nullptr;
        }

        void OnRebasing( lua_State *L, stackItem_t oldStack );
        void OnResizing( lua_State *L, stackOffset_t oldSize );
        void OnChangeTop( lua_State *L, stackItem_t oldTop );

        // The stack must be linked to a runtime Lua thread.
        lua_State *runtimeThread;
    };

    rtStackExp_t rtStack; /* stack of the runtime TValues */
    ciStack_t ciStack; /* stack of the callinfo frames */

    RtStackView& GetCurrentStackFrame( void )
    {
        return this->ciStack.Top()->stack;
    }

    const Instruction *savedpc;  /* 'savedpc' of current function */
    unsigned short nCcalls;  /* number of nested C calls */
    lu_byte hookmask;
    bool allowhook;
    int basehookcount;
    int hookcount;
    lua_Hook hook;
    TValue l_gt;  /* table of globals */
    TValue env;  /* temporary place for environments */
    gcObjList_t openupval;  /* list of open upvalues in this stack */
    stackOffset_t errfunc;  /* current error handling function (stack index) */
    TValue storage;
    Table *mt[NUM_TAGS];  /* metatables for basic types */

    // State-locked variable access.
    // The variable must not be reallocatable.
    // If no support for value-based locking is available (yet), this locking is sufficient.
    template <typename valueType>
    struct StateValueContext : public DataContext <valueType>
    {
        valueType *valPtr;

        unsigned long refCount;

        inline StateValueContext( lua_State *L, valueType *valPtr )
        {
            this->valPtr = valPtr;
            this->refCount = 0;
        }

        void Reference( lua_State *L )
        {
            this->refCount++;

            // TODO
        }

        void Dereference( lua_State *L )
        {
            // TODO

            this->refCount--;
        }

        valueType* const* GetValuePointer( lua_State *L )
        {
            return &valPtr;
        }
    };

    template <typename dataType>
    FASTAPI StateValueContext <dataType>* NewStateValueContext( dataType *valPtr )
    {
        return new StateValueContext <dataType> ( this, valPtr );
    }

    FASTAPI ValueAddress GetTemporaryValue( void )
    {
        return ValueAddress( this, NewStateValueContext( &env ) );
    }

    // Memory allocator for class memory.
    // It allocates memory with thread focus.
    struct LuaThreadRuntimeAllocator
    {
        lua_State *runtimeThread;

        inline LuaThreadRuntimeAllocator( void )
        {
            this->runtimeThread = nullptr;
        }

        inline ~LuaThreadRuntimeAllocator( void )
        {
            return;
        }

        inline void SetThread( lua_State *theThread )
        {
            this->runtimeThread = theThread;
        }

        inline void* Allocate( size_t memSize )
        {
            return luaM_realloc_( this->runtimeThread, nullptr, 0, memSize );
        }

        inline void Free( void *ptr, size_t memSize )
        {
            luaM_realloc_( this->runtimeThread, ptr, memSize, 0 );
        }
    };
    LuaThreadRuntimeAllocator defaultAlloc;
};

typedef StructAddress <rtStack_t> RtStackAddr;
typedef StructAddress <ciStack_t> CiStackAddr;

struct LocalValueAddress : public ValueAddress
{
    TValue theValue;
    TValue *theValuePtr;

    inline LocalValueAddress( void ) : ValueAddress( &theValuePtr ), theValuePtr( &theValue )
    {
        return;
    }
};

// General stuff.
struct stringtable
{
    gcObjList_t *hash;
    lu_int32 nuse;  /* number of elements */
    int size;
};

class lua_Thread;
class GrayObject;
class TString;
class Closure;

/*
** 'global state', shared by all threads of this state
*/
struct global_State
{
    stringtable strt;  /* hash table for strings */
    lu_byte currentwhite;

    lu_mem totalbytes;  /* number of bytes currently allocated */

    TValue l_registry;
    lua_State *mainthread;
    TString *tmname[TM_N];  /* array with tag-method names */
    Closure *events[LUA_NUM_EVENTS];

    RwList <lua_Thread> threads; /* all existing threads in this machine */

    lua_config *config; /* configuration shared across global states for specialized VM creation */
    bool hasUniqueConfig;

    // Immutable configuration values.
    bool isMultithreaded;   // if true, the runtime must support parallel execution at well-defined points.
};

// Macros to create and destroy Lua types accordingly.
template <typename classType>
FASTAPI classType* lua_new( lua_State *L, LuaTypeSystem::typeInfoBase *theType, void *construct_params = nullptr )
{
    classType *outObj = nullptr;
    {
        global_State *g = L->gstate;

        LuaTypeSystem& typeSys = g->config->typeSys;

        LuaRTTI *rtObj = typeSys.Construct( g, theType, construct_params );

        if ( rtObj )
        {
            outObj = (classType*)typeSys.GetObjectFromTypeStruct( rtObj );
        }
    }
    return outObj;
}

template <typename classType>
FASTAPI void lua_delete( lua_State *L, classType *obj )
{
    global_State *g = L->gstate;

    LuaTypeSystem& typeSys = g->config->typeSys;

    LuaRTTI *rtObj = typeSys.GetTypeStructFromObject( obj );

    if ( rtObj )
    {
        typeSys.Destroy( g, rtObj );
    }
}

// Helper macros for using lua_State and global_State.
#ifdef LUA_USE_C_MACROS

#define G(L)	(L->gstate)

#else

FASTAPI global_State* G( lua_State *L )         { return L->gstate; }

#endif

/* Macros to test type */
#ifdef LUA_USE_C_MACROS

#define ttype(o)	((o)->tt)
#define setttype(obj, tt) (ttype(obj) = (tt))
#define iscollectable(o)	(ttype(o) >= LUA_TSTRING)

#define ttisnil(o)	(ttype(o) == LUA_TNIL)
#define ttisnumber(o)	(ttype(o) == LUA_TNUMBER)
#define ttisstring(o)	(ttype(o) == LUA_TSTRING)
#define ttistable(o)	(ttype(o) == LUA_TTABLE)
#define ttisfunction(o)	(ttype(o) == LUA_TFUNCTION)
#define ttisboolean(o)	(ttype(o) == LUA_TBOOLEAN)
#define ttisuserdata(o)	(ttype(o) == LUA_TUSERDATA)
#define ttisclass(o)    (ttype(o) == LUA_TCLASS)
#define ttisdispatch(o) (ttype(o) == LUA_TDISPATCH)
#define ttisthread(o)	(ttype(o) == LUA_TTHREAD)
#define ttislightuserdata(o)	(ttype(o) == LUA_TLIGHTUSERDATA)

#else

FASTAPI int& ttype( TValue *o )                 { return o->tt; }
FASTAPI int ttype( const TValue *o )            { return o->tt; }
FASTAPI void setttype(TValue *obj, int tt)      { ttype(obj) = tt; }
FASTAPI bool iscollectable(const TValue *o)     { return ttype(o) >= LUA_TSTRING; }

FASTAPI bool ttisnil( const TValue *o )             { return ttype( o ) == LUA_TNIL; }
FASTAPI bool ttisnumber( const TValue *o )          { return ttype( o ) == LUA_TNUMBER; }
FASTAPI bool ttisstring( const TValue *o )          { return ttype( o ) == LUA_TSTRING; }
FASTAPI bool ttistable( const TValue *o )           { return ttype( o ) == LUA_TTABLE; }
FASTAPI bool ttisfunction( const TValue *o )        { return ttype( o ) == LUA_TFUNCTION; }
FASTAPI bool ttisboolean( const TValue *o )         { return ttype( o ) == LUA_TBOOLEAN; }
FASTAPI bool ttisuserdata( const TValue *o )        { return ttype( o ) == LUA_TUSERDATA; }
FASTAPI bool ttisclass( const TValue *o )           { return ttype( o ) == LUA_TCLASS; }
FASTAPI bool ttisdispatch( const TValue *o )        { return ttype( o ) == LUA_TDISPATCH; }
FASTAPI bool ttisthread( const TValue *o )          { return ttype( o ) == LUA_TTHREAD; }
FASTAPI bool ttislightuserdata( const TValue *o )   { return ttype( o ) == LUA_TLIGHTUSERDATA; }

#endif //LUA_USE_C_MACROS

// A special cast operator that supports safe casting and C-style casting depending on
// compilation settings.
template <typename resultType>
FASTAPI resultType* gcobj_cast( GCObject *obj )
{
    return
#if defined(_DEBUG) && defined(LUA_OBJECT_VERIFICATION)
        dynamic_cast <resultType*>
#else
        (resultType*)
#endif
            ( obj );
}

template <typename resultType>
FASTAPI const resultType* gcobj_cast( const GCObject *obj )
{
    return
#if defined(_DEBUG) && defined(LUA_OBJECT_VERIFICATION)
        dynamic_cast <const resultType*>
#else
        (const resultType*)
#endif
            ( obj );
}

#ifdef LUA_USE_C_MACROS
#define gcobj(o)    (gcobj_cast <GCObject> (o))
#define sobj(o)     (gcobj_cast <TString> (o))
#define uobj(o)     (gcobj_cast <Udata> (o))
#define clobj(o)    (gcobj_cast <Closure> (o))
#define hobj(o)     (gcobj_cast <Table> (o))
#define thobj(o)    (gcobj_cast <lua_State> (o))
#define qobj(o)     (gcobj_cast <Dispatch> (o))
#define jobj(o)     (gcobj_cast <Class> (o))
#define ptobj(o)    (gcobj_cast <Proto> (o))
#else
FASTAPI GCObject*   gcobj(GCObject *o)      { return gcobj_cast <GCObject> ( o ); }
FASTAPI TString*    sobj(GCObject *o)       { return gcobj_cast <TString> ( o ); }
FASTAPI Udata*      uobj(GCObject *o)       { return gcobj_cast <Udata> ( o ); }
FASTAPI Closure*    clobj(GCObject *o)      { return gcobj_cast <Closure> ( o ); }
FASTAPI Table*      hobj(GCObject *o)       { return gcobj_cast <Table> ( o ); }
FASTAPI lua_State*  thobj(GCObject *o)      { return gcobj_cast <lua_State> ( o ); }
FASTAPI Dispatch*   qobj(GCObject *o)       { return gcobj_cast <Dispatch> ( o ); }
FASTAPI Class*      jobj(GCObject *o)       { return gcobj_cast <Class> ( o ); }
FASTAPI Proto*      ptobj(GCObject *o)      { return gcobj_cast <Proto> ( o ); }

FASTAPI const GCObject*     gcobj(const GCObject *o)    { return gcobj_cast <const GCObject> ( o ); }
FASTAPI const TString*      sobj(const GCObject *o)     { return gcobj_cast <const TString> ( o ); }
FASTAPI const Udata*        uobj(const GCObject *o)     { return gcobj_cast <const Udata> ( o ); }
FASTAPI const Closure*      clobj(const GCObject *o)    { return gcobj_cast <const Closure> ( o ); }
FASTAPI const Table*        hobj(const GCObject *o)     { return gcobj_cast <const Table> ( o ); }
FASTAPI const lua_State*    thobj(const GCObject *o)    { return gcobj_cast <const lua_State> ( o ); }
FASTAPI const Dispatch*     qobj(const GCObject *o)     { return gcobj_cast <const Dispatch> ( o ); }
FASTAPI const Class*        jobj(const GCObject *o)     { return gcobj_cast <const Class> ( o ); }
FASTAPI const Proto*        ptobj(const GCObject *o)    { return gcobj_cast <const Proto> ( o ); }
#endif //LUA_USE_C_MACROS

/* Macros to access values */
#ifdef LUA_USE_C_MACROS
#define gcvalue(o)	check_exp(iscollectable(o), gcobj((o)->value.gc))
#define pvalue(o)	check_exp(ttislightuserdata(o), (o)->value.p)
#define nvalue(o)	check_exp(ttisnumber(o), (o)->value.n)
#define rawtsvalue(o)	check_exp(ttisstring(o), sobj((o)->value.gc))
#define tsvalue(o)	(rawtsvalue(o))
#define rawuvalue(o)	check_exp(ttisuserdata(o), uobj((o)->value.gc))
#define uvalue(o)	(rawuvalue(o))
#define clvalue(o)	check_exp(ttisfunction(o), clobj((o)->value.gc))
#define hvalue(o)	check_exp(ttistable(o), hobj((o)->value.gc))
#define bvalue(o)	check_exp(ttisboolean(o), (o)->value.b)
#define thvalue(o)	check_exp(ttisthread(o), thobj((o)->value.gc))
#define qvalue(o)   check_exp(ttisdispatch(o), qobj((o)->value.gc))
#define jvalue(o)   check_exp(ttisclass(o), jobj((o)->value.gc))

#define l_isfalse(o)	(ttisnil(o) || (ttisboolean(o) && bvalue(o) == 0))
#else
FASTAPI lua_Number      nvalue(const TValue *o)         { return check_exp(ttisnumber(o), (o)->value.n); }
FASTAPI TString*        rawtsvalue(const TValue *o)     { return check_exp(ttisstring(o), sobj((o)->value.gc)); }
FASTAPI bool            bvalue(const TValue *o)         { return check_exp(ttisboolean(o), (o)->value.b); }

FASTAPI GCObject*       gcvalue(const TValue *o)        { return check_exp(iscollectable(o), gcobj((o)->value.gc)); }
FASTAPI void*           pvalue(const TValue *o)         { return check_exp(ttislightuserdata(o), (o)->value.p); }
FASTAPI TString*        tsvalue(const TValue *o)        { return check_exp(ttisstring(o), sobj((o)->value.gc)); }
FASTAPI char*           rawuvalue(const TValue *o)      { return (char*)check_exp(ttisuserdata(o), uobj((o)->value.gc)); }
FASTAPI Udata*          uvalue(const TValue *o)         { return check_exp(ttisuserdata(o), uobj((o)->value.gc)); }
FASTAPI Closure*        clvalue(const TValue *o)        { return check_exp(ttisfunction(o), clobj((o)->value.gc)); }
FASTAPI Table*          hvalue(const TValue *o)         { return check_exp(ttistable(o), hobj((o)->value.gc)); }
FASTAPI lua_State*      thvalue(const TValue *o)        { return check_exp(ttisthread(o), thobj((o)->value.gc)); }
FASTAPI Dispatch*       qvalue(const TValue *o)         { return check_exp(ttisdispatch(o), qobj((o)->value.gc)); }
FASTAPI Class*          jvalue(const TValue *o)         { return check_exp(ttisclass(o), jobj((o)->value.gc)); }

FASTAPI bool            l_isfalse(const TValue *o)      { return (ttisnil(o) || (ttisboolean(o) && bvalue(o) == 0)); }
#endif //LUA_USE_C_MACROS

/*
** some userful bit tricks
*/
#ifdef LUA_USE_C_MACROS

#define resetbits(x,m)	((x) &= cast(lu_byte, ~(m)))
#define setbits(x,m)	((x) |= (m))
#define testbits(x,m)	((x) & (m))
#define bitmask(b)	(1<<(b))
#define bit2mask(b1,b2)	(bitmask(b1) | bitmask(b2))
#define l_setbit(x,b)	setbits(x, bitmask(b))
#define resetbit(x,b)	resetbits(x, bitmask(b))
#define testbit(x,b)	testbits(x, bitmask(b))
#define set2bits(x,b1,b2)	setbits(x, (bit2mask(b1, b2)))
#define reset2bits(x,b1,b2)	resetbits(x, (bit2mask(b1, b2)))
#define test2bits(x,b1,b2)	testbits(x, (bit2mask(b1, b2)))

#else

template <typename bitMask> FASTAPI void resetbits(bitMask& x, bitMask m)                   { ((x) &= cast(lu_byte, ~(m))); }
template <typename bitMask> FASTAPI void setbits(bitMask& x, bitMask m)                     { ((x) |= (m)); }
template <typename bitMask> FASTAPI bool testbits(bitMask x, bitMask m)                     { return ( ((x) & (m)) != 0 ); }

template <typename bitMaskType> FASTAPI bitMaskType bitmask(bitMaskType b)                  { return ((bitMaskType)1<<(b)); }

template <typename bitMask> FASTAPI bitMask bit2mask(bitMask b1, bitMask b2)                { return (bitmask(b1) | bitmask(b2)); }

template <typename bitMask> FASTAPI void l_setbit(bitMask& x, bitMask b)                    { setbits(x, bitmask(b)); }
template <typename bitMask> FASTAPI void resetbit(bitMask& x, bitMask b)                    { resetbits(x, bitmask(b)); }
template <typename bitMask> FASTAPI bool testbit(bitMask x, bitMask b)                      { return testbits(x, bitmask(b)); }
template <typename bitMask> FASTAPI void set2bits(bitMask& x, bitMask b1, bitMask b2)       { setbits(x, (bit2mask(b1, b2))); }
template <typename bitMask> FASTAPI void reset2bits(bitMask& x, bitMask b1, bitMask b2)     { resetbits(x, (bit2mask(b1, b2))); }
template <typename bitMask> FASTAPI bool test2bits(bitMask x, bitMask b1, bitMask b2)       { return testbits(x, (bit2mask(b1, b2))); }

#endif //LUA_USE_C_MACROS


// TODO: improve this flag mess.

/*
** Layout for bit use in `marked' field:
** bit 0 - object is white (type 0)
** bit 1 - object is white (type 1)
** bit 2 - object is black
** bit 3 - for userdata: has been finalized
** bit 3 - for tables: has weak keys
** bit 4 - for tables: has weak values
** bit 5 - object is fixed (should not be collected)
** bit 6 - object is "super" fixed (only the main thread)
*/

/*
** garbage collection internals.
*/
#define WHITE0BIT	    ((lu_byte)0)
#define WHITE1BIT	    ((lu_byte)1)
#define BLACKBIT	    ((lu_byte)2)
#define FINALIZEDBIT	((lu_byte)3)
#define KEYWEAKBIT	    ((lu_byte)3)
#define VALUEWEAKBIT	((lu_byte)4)
#define FIXEDBIT	    ((lu_byte)5)
// free bits: 6 and 7
#define WHITEBITS	    ((lu_byte)bit2mask(WHITE0BIT, WHITE1BIT))

FASTAPI bool iswhite(const GCObject *x)         { return test2bits((lu_byte)(x)->marked, WHITE0BIT, WHITE1BIT); }
FASTAPI bool isblack(const GCObject *x)         { return testbit((lu_byte)(x)->marked, BLACKBIT); }
FASTAPI bool isgray(const GCObject *x)          { return (!isblack(x) && !iswhite(x)); }

FASTAPI lu_byte otherwhite(global_State *g)     { return g->currentwhite ^ WHITEBITS; }

FASTAPI bool isdead(global_State *g, const GCObject *v)
{
    // NOTE: this method is not an absolute check.
    // it just reflects the current state of the GC runtime.

    bool isAlive = false;

    if ( isAlive == false )
    {
        if ( ((v)->marked & otherwhite(g) & WHITEBITS) == 0 )
        {
            isAlive = true;
        }
    }

    if ( isAlive == false )
    {
        if ( v->GetGCRefCount() != 0 )
        {
            isAlive = true;
        }
    }

    return !isAlive;
}

FASTAPI void changewhite(GCObject *x)   { ((x)->marked ^= WHITEBITS); }
FASTAPI void gray2black(GCObject *x)    { l_setbit((x)->marked, BLACKBIT); }

FASTAPI bool valiswhite(const TValue *x)    { return (iscollectable(x) && iswhite(gcvalue(x))); }

FASTAPI void checkconsistency(TValue *obj)
{ lua_assert(!iscollectable(obj) || (ttype(obj) == (obj)->value.gc->tt)); }

FASTAPI void checkliveness(global_State *g, TValue *obj)
{ lua_assert(!iscollectable(obj) || ((ttype(obj) == (obj)->value.gc->tt) && !isdead(g, (obj)->value.gc))); }


/* Macros to set values */
FASTAPI void setnilvalue(TValue *obj)       { obj->tt = LUA_TNIL; }
FASTAPI void setnvalue(TValue *obj, lua_Number num )
{
    obj->value.n = num;
    obj->tt = LUA_TNUMBER;
}
FASTAPI void setpvalue(TValue *obj, void *ptr )
{
    obj->value.p = ptr;
    obj->tt = LUA_TLIGHTUSERDATA;
}
FASTAPI void setbvalue(TValue *obj, bool bval )
{
    obj->value.b = bval;
    obj->tt = LUA_TBOOLEAN;
}
FASTAPI void setgcvalue(lua_State *L, TValue *val, GCObject *obj)
{
    val->value.gc = obj;
    val->tt = obj->tt;
    checkliveness(G(L),val);
}
template <int typeDesc, typename objType>
FASTAPI void setvalue(lua_State *L, TValue *val, objType *obj)
{
    val->value.gc = obj;
    val->tt = typeDesc;
    checkliveness(G(L),val);
}
FASTAPI void setsvalue(lua_State *L, TValue *val, TString *obj)         { setvalue <LUA_TSTRING> ( L, val, obj ); }
FASTAPI void setuvalue(lua_State *L, TValue *val, Udata *obj)           { setvalue <LUA_TUSERDATA> ( L, val, obj ); }
FASTAPI void setthvalue(lua_State *L, TValue *val, lua_State *obj)      { setvalue <LUA_TTHREAD> ( L, val, obj ); }
FASTAPI void setclvalue(lua_State *L, TValue *val, Closure *obj)        { setvalue <LUA_TFUNCTION> ( L, val, obj ); }
FASTAPI void sethvalue(lua_State *L, TValue *val, Table *obj)           { setvalue <LUA_TTABLE> ( L, val, obj ); }
FASTAPI void setptvalue(lua_State *L, TValue *val, Proto *obj)          { setvalue <LUA_TOBJECT> ( L, val, obj ); }
FASTAPI void setqvalue(lua_State *L, TValue *val, Dispatch *obj)        { setvalue <LUA_TDISPATCH> ( L, val, obj ); }
FASTAPI void setjvalue(lua_State *L, TValue *val, Class *obj)           { setvalue <LUA_TCLASS> ( L, val, obj ); }
FASTAPI void setobj(lua_State *L, TValue *dstVal, const TValue *srcVal)
{
    dstVal->value = srcVal->value;
    dstVal->tt = srcVal->tt;
    checkliveness(G(L),dstVal);
}

struct CtxStkItem
{
    lua_State *L;
    rtStack_t& theStack;

    StkId item;

    inline CtxStkItem( lua_State *L ) : L( L ), theStack( L->rtStack )
    {
        theStack.Lock( L );

        this->item = L->GetCurrentStackFrame().ObtainItem( L, theStack ).Pointer();
    }

    inline ~CtxStkItem( void )
    {
        theStack.Unlock( L );
    }
};
FASTAPI void pushnilvalue( lua_State *L )
{
    CtxStkItem ctxItem( L ); setnilvalue( ctxItem.item );
}
FASTAPI void pushnvalue( lua_State *L, lua_Number n )
{
    CtxStkItem ctxItem( L ); setnvalue( ctxItem.item, n );
}
FASTAPI void pushpvalue( lua_State *L, void *ptr )
{
    CtxStkItem ctxItem( L ); setpvalue( ctxItem.item, ptr );
}
FASTAPI void pushbvalue( lua_State *L, bool b )
{
    CtxStkItem ctxItem( L ); setbvalue( ctxItem.item, b );
}
FASTAPI void pushgcvalue( lua_State *L, GCObject *obj )
{
    CtxStkItem ctxItem( L ); setgcvalue( L, ctxItem.item, obj );
}
FASTAPI void pushtvalue( lua_State *L, const TValue *o )
{
    CtxStkItem ctxItem( L ); setobj( L, ctxItem.item, o );
}
template <int typeDesc, typename objType>
FASTAPI void pushvalue( lua_State *L, objType *obj )
{
    CtxStkItem ctxItem( L ); setvalue <typeDesc, objType> ( L, ctxItem.item, obj );
}
FASTAPI void pushsvalue( lua_State *L, TString *obj )                   { pushvalue <LUA_TSTRING> ( L, obj ); }
FASTAPI void pushuvalue( lua_State *L, Udata *obj )                     { pushvalue <LUA_TUSERDATA> ( L, obj ); }
FASTAPI void pushthvalue( lua_State *L, lua_State *obj )                { pushvalue <LUA_TTHREAD> ( L, obj ); }
FASTAPI void pushclvalue( lua_State *L, Closure *obj )                  { pushvalue <LUA_TFUNCTION> ( L, obj ); }
FASTAPI void pushhvalue( lua_State *L, Table *obj )                     { pushvalue <LUA_TTABLE> ( L, obj ); }
FASTAPI void pushptvalue( lua_State *L, Proto *obj )                    { pushvalue <LUA_TOBJECT> ( L, obj ); }
FASTAPI void pushqvalue( lua_State *L, Dispatch *obj )                  { pushvalue <LUA_TDISPATCH> ( L, obj ); }
FASTAPI void pushjvalue( lua_State *L, Class *obj )                     { pushvalue <LUA_TCLASS> ( L, obj ); }

FASTAPI stackOffset_t rt_stackcount( lua_State *L )                     { return L->GetCurrentStackFrame().GetUsageCount( L, L->rtStack ); }

FASTAPI stackOffset_t ci_alloccount( lua_State *L )                     { return L->GetCurrentStackFrame().GetUsageCount( L, L->rtStack ); }

// Function to resolve stack indices.
LUAI_FUNC stackOffset_t index2stackindex( lua_State *L, int idx );
LUAI_FUNC RtCtxItem index2stackadr( lua_State *L, int idx, bool isSecure = false );
LUAI_FUNC ValueAddress fetchstackadr( lua_State *L, stackOffset_t idx );


FASTAPI void readstkvalue( lua_State *L, TValue *dst, int idx )
{
    RtCtxItem stackItem = index2stackadr( L, idx );

    stackItem.Get( *dst );
}
// TODO: this function is not thread safe!!!
FASTAPI void popstkvalue( lua_State *L, TValue *dst )
{
    readstkvalue( L, dst, -1 );

    L->ciStack.Top()->stack.PopCount( L, L->rtStack, 1 );
}
FASTAPI void popstack( lua_State *L, stackOffset_t count )
{
    L->ciStack.Top()->stack.PopCount( L, L->rtStack, count );
}


/*
** different types of sets, according to destination
*/
#ifdef LUA_USE_C_MACROS

/* from stack to (same) stack */
#define setobjs2s	setobj
/* to stack (not from same stack) */
#define setobj2s	setobj
#define setsvalue2s	setsvalue
#define sethvalue2s	sethvalue
#define setptvalue2s	setptvalue
/* from table to same table */
#define setobjt2t	setobj
/* to table */
#define setobj2t	setobj
/* to new object */
#define setobj2n	setobj
#define setsvalue2n	setsvalue

#else

/* from stack to (same) stack */
FASTAPI void setobjs2s(lua_State *L, StkId dstVal, StkId_const srcVal)          { setobj(L, dstVal, srcVal); }
/* to stack (not from same stack) */
FASTAPI void setobj2s(lua_State *L, StkId dstVal, const TValue *srcVal)         { setobj(L, dstVal, srcVal); }
FASTAPI void setsvalue2s(lua_State *L, StkId val, TString *obj)                 { setsvalue(L, val, obj); }
FASTAPI void sethvalue2s(lua_State *L, StkId val, Table *obj)                   { sethvalue(L, val, obj); }
FASTAPI void setptvalue2s(lua_State *L, StkId val, Proto *obj)                  { setptvalue(L, val, obj); }
/* from table to same table */
FASTAPI void setobjt2t(lua_State *L, TValue *dstVal, const TValue *srcVal)      { setobj(L, dstVal, srcVal); }
/* to table */
FASTAPI void setobj2t(lua_State *L, TValue *dstVal, const TValue *srcVal)       { setobj(L, dstVal, srcVal); }
/* to new object */
FASTAPI void setobj2n(lua_State *L, TValue *dstVal, const TValue *srcVal)       { setobj(L, dstVal, srcVal); }
FASTAPI void setsvalue2n(lua_State *L, TValue *val, TString *obj)               { setsvalue(L, val, obj); }

#endif //LUA_USE_C_MACROS

// Macros for stack management.
FASTAPI ValueAddress newstackslot( lua_State *L )
{
    RtStackView& currentStackFrame = L->GetCurrentStackFrame();

    RtCtxItem stackItem = currentStackFrame.ObtainItem( L, L->rtStack );

    ValueContext *ctx = L->rtStack.GetNewVirtualStackItem( L, std::move( stackItem ) );

    return ValueAddress( L, ctx );
}

// Macros to access lua_State stuff.
FASTAPI Closure*    curr_func(lua_State *L)     { return (clvalue(L->ciStack.Top()->func)); }
FASTAPI Closure*    ci_func(CallInfo *ci)       { return (clvalue((ci)->func)); }
FASTAPI bool        f_isLua(CallInfo *ci)       { return (!ci_func(ci)->isC); }
FASTAPI bool        isLua(CallInfo *ci)         { return (ttisfunction((ci)->func) && f_isLua(ci)); }

// Macros for closures.
#ifdef LUA_USE_C_MACROS

#define sizeCclosure(n)	(cast(int, sizeof(CClosureBasic)) + \
                         cast(int, sizeof(TValue)*((n)-1)))

#define sizeCmethod(n)  (cast(int, sizeof(CClosureMethod)) + \
                         cast(int, sizeof(TValue)*((n)-1)))

#define sizeCmethodt(n) (cast(int, sizeof(CClosureMethodTrans)) + \
                         cast(int, sizeof(TValue)*((n)-1)))

#define sizeLclosure(n)	(cast(int, sizeof(LClosure)) + \
                         cast(int, sizeof(TValue*)*((n)-1)))

#define iscfunction(o)	(ttype(o) == LUA_TFUNCTION && clvalue(o)->isC)
#define isLfunction(o)	(ttype(o) == LUA_TFUNCTION && !clvalue(o)->isC)

#else

FASTAPI size_t sizeCclosure( int n )            { return sizeof(CClosureBasic) + ( sizeof(TValue)*((n)-1) ); }
FASTAPI size_t sizeLclosure( int n )            { return sizeof(LClosure) + ( sizeof(TValue*)*((n)-1) ); }

FASTAPI bool iscfunction(const TValue *o)       { return (ttype(o) == LUA_TFUNCTION && clvalue(o)->isC); }
FASTAPI bool isLfunction(const TValue *o)       { return (ttype(o) == LUA_TFUNCTION && !clvalue(o)->isC); }

#endif //LUA_USE_C_MACROS

// TString and Udata helper macros.
#ifdef LUA_USE_C_MACROS

#define sizestring(s)	(_sizestring(sizeof(TString),s->len))
#define sizeudata(u)	(_sizeudata(sizeof(Udata),u->len))

#define getstr(ts)	cast(const char *, (ts) + 1)
#define svalue(o)       getstr(rawtsvalue(o))

#else

FASTAPI size_t sizestring( TString *s )         { return (_sizestring(sizeof(TString),s->len)); }
FASTAPI size_t sizeudata( Udata *u )            { return (_sizeudata(sizeof(Udata),u->len)); }

FASTAPI const char* getstr( const TString *ts )             { return cast(const char *, (ts) + 1); }
FASTAPI const char* svalue( const TValue *o )               { return getstr( rawtsvalue( o ) ); }

#endif //LUA_USE_C_MACROS

// Lua state helper macros.
#ifdef LUA_USE_C_MACROS

/* table of globals */
#define gt(L)	(&L->l_gt)

/* registry */
#define registry(L)	(&G(L)->l_registry)

#else

/* table of globals */
FASTAPI TValue* gt( lua_State *L )          { return &L->l_gt; }

/* registry */
FASTAPI TValue* registry( lua_State *L )    { return &G(L)->l_registry; }

#endif //LUA_USE_C_MACROS

/*
** `module' operation for hashing (size is always a power of 2)
*/
#define lmod(s,size)        (check_exp((size&(size-1))==0, (cast(int, (s) & ((size)-1)))))


// TODO: remove this, rather use a global_State variable.
#define luaO_nilobject		(&luaO_nilobject_)

LUAI_DATA const TValue luaO_nilobject_;

LUAI_FUNC int luaO_log2 (size_t x);
LUAI_FUNC int luaO_int2fb (unsigned int x);
LUAI_FUNC int luaO_fb2int (int x);
LUAI_FUNC int luaO_rawequalObj (const TValue *t1, const TValue *t2);
LUAI_FUNC int luaO_str2d (const char *s, lua_Number *result);
LUAI_FUNC const char *luaO_pushvfstring (lua_State *L, const char *fmt, va_list argp);
LUAI_FUNC const char *luaO_pushfstring (lua_State *L, const char *fmt, ...);
LUAI_FUNC void luaO_chunkid (char *out, const char *source, size_t len);
LUAI_FUNC ConstValueAddress luaO_getnilcontext( lua_State *L );

// Module initialization.
LUAI_FUNC void luaO_init( lua_config *cfg );
LUAI_FUNC void luaO_shutdown( lua_config *cfg );


#endif

