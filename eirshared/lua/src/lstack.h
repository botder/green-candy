// Stack utilities for Lua.
#ifndef _LUA_STACK_UTILITIES_
#define _LUA_STACK_UTILITIES_

#include "lobject.h"

#include <algorithm>

template <typename dataType>
struct DataContext
{
    virtual void Reference( lua_State *L ) = 0;
    virtual void Dereference( lua_State *L ) = 0;

    virtual dataType* const* GetValuePointer( lua_State *L ) = 0;
};

typedef DataContext <TValue> ValueContext;
typedef DataContext <const TValue> ConstValueContext;

template <typename structType>
struct StructAddress
{
    typedef typename std::remove_const <structType>::type nonconst_structType;

    friend struct StructAddress <nonconst_structType>;
    friend struct StructAddress <const structType>;

    typedef DataContext <structType> StructContext;

private:
    lua_State *L;
    StructContext *valContext;
    mutable structType *const *address;

public:
    inline StructAddress( void )
    {
        this->L = nullptr;
        this->valContext = nullptr;
        this->address = nullptr;
    }

    inline explicit StructAddress( structType **valPtr )
    {
        this->L = nullptr;
        this->valContext = nullptr;
        this->address = valPtr;
    }

    template <typename dep_struct_type, typename = typename std::enable_if <std::is_same <dep_struct_type, nonconst_structType>::value && !std::is_same <dep_struct_type, structType>::value>::type>
    inline StructAddress( StructAddress <dep_struct_type>&& right )
    {
        this->L = right.L;
        this->valContext = (DataContext <structType>*)right.valContext;
        this->address = right.address;

        right.L = nullptr;
        right.valContext = nullptr;
        right.address = nullptr;
    }

    inline StructAddress( lua_State *L, StructContext *valCtx )
    {
        if ( valCtx )
        {
            valCtx->Reference( L );
        }

        this->L = L;
        this->valContext = valCtx;
        this->address = nullptr;
    }

    inline StructAddress( StructAddress&& right )
    {
        this->L = right.L;
        this->valContext = right.valContext;
        this->address = right.address;

        right.L = nullptr;
        right.valContext = nullptr;
        right.address = nullptr;
    }
    inline StructAddress( const StructAddress& right )
    {
        StructContext *valCtx = right.valContext;
        lua_State *L = right.L;

        if ( valCtx )
        {
            valCtx->Reference( L );
        }

        this->L = L;
        this->valContext = valCtx;
        this->address = right.address;
    }

    inline StructAddress& operator = ( StructAddress&& right )
    {
        this->~StructAddress();

        new (this) StructAddress( std::move( right ) );

        return *this;
    }
    inline StructAddress& operator = ( const StructAddress& right )
    {
        this->~StructAddress();

        new (this) StructAddress( right );

        return *this;
    }

    inline ~StructAddress( void )
    {
        if ( this->valContext )
        {
            this->valContext->Dereference( L );
        }
    }

    inline structType* Pointer( lua_State *L ) const
    {
        if ( !this->address )
        {
            if ( StructContext *ctx = this->valContext )
            {
                this->address = ctx->GetValuePointer( this->L );
            }
        }

        if ( !this->address )
        {
            return nullptr;
        }

        return *this->address;
    }

    inline operator structType* ( void ) const
    {
        return Pointer( this->L );
    }

    inline structType* operator -> ( void ) const
    {
        return Pointer( this->L );
    }

    inline StructAddress <const structType>& ConstCast( void )
    {
        return (StructAddress <const structType>&)*this;
    }
};

typedef StructAddress <TValue> ValueAddress;
typedef StructAddress <const TValue> ConstValueAddress;

typedef ptrdiff_t stackOffset_t;

template <typename dataType, typename movementManagerType>
struct growingStack
{
    typedef dataType itemType_t;
    typedef const dataType constItemType_t;
    typedef dataType* stackItem_t;
    typedef const dataType* constStackItem_t;

    typedef movementManagerType movman_t;

private:
    dataType *base;     // start of stack
    dataType *end;      // end of stack
    stackItem_t top;    // top of stack
    stackOffset_t size; // size of this stack

    mutable unsigned int lockCount;

public:
    inline growingStack( void )
    {
        this->base = nullptr;
        this->end = nullptr;
        this->top = nullptr;
        this->size = (stackOffset_t)0;

        this->lockCount = 0;

        this->_refObject._theStack = this;
    }

private:
    // Reference-based struct address version of this stack.
    struct StackRefAddress : public DataContext <growingStack>
    {
        mutable growingStack *_theStack;

        void Reference( lua_State *L )
        {
            _theStack->Lock( L );
        }

        void Dereference( lua_State *L )
        {
            _theStack->Unlock( L );
        }

        growingStack* const* GetValuePointer( lua_State *L )
        {
            return &_theStack;
        }
    };

    StackRefAddress _refObject;

public:
    inline DataContext <growingStack>* GetValueContext( void )
    {
        return &_refObject;
    }

    // Helpers for object orriented locking code.
    FASTAPI StructAddress <growingStack> LockedAcquisition( lua_State *L )
    {
        StructAddress <growingStack> returnVal ( L, this->GetValueContext() );
        return returnVal;
    }

    inline ~growingStack( void )
    {
        // Make sure there are no alive context items that are valid in the context of this stack.
        lua_assert( LIST_EMPTY( m_contextItems.root ) == true );

        lua_assert( this->base == nullptr );
    }

    inline void Initialize( global_State *g, lua_State *L, stackOffset_t stackSize )
    {
        UNUSED(g);

        SetSize( L, stackSize );

        // TODO: add more initializers.
    }

    inline void Shutdown( global_State *g, lua_State *L )
    {
        // Invalidate all context items that still belong to this thread.
        LIST_FOREACH_BEGIN( CtxItem, m_contextItems.root, node )

            item->ownerStack = nullptr;
            item->stackItem = nullptr;
            item->L = nullptr;

            this->Unlock( L );

        LIST_FOREACH_END

        LIST_CLEAR( m_contextItems.root );

        SetSize( L, 0 );

        // Call shutdown on acceleration structures.
        _cachedVirtualStackItem.Shutdown( g );
        _cachedVirtualStackItemConst.Shutdown( g );
    }

    inline stackItem_t Top( void ) const
    {
        return this->top;
    }

    inline stackItem_t Base( void ) const
    {
        return this->base;
    }

    inline stackItem_t Tail( void ) const
    {
        return this->end;
    }

    inline stackOffset_t GetFreeCount( lua_State *L ) const
    {
        stackOffset_t freeCount = 0;

        Lock( L );

        if ( this->top == nullptr )
        {
            freeCount = GetSize();
        }
        else
        {
            freeCount = ( Tail() - Base() ) + 1;
        }

        Unlock( L );

        return freeCount;
    }

private:
    FASTAPI static void InitializeRange( lua_State *L, stackItem_t startItem, stackItem_t endItem )
    {
        for ( stackItem_t iter = startItem; iter <= endItem; iter++ )
        {
            movementManagerType::InitializeValue( L, iter );
        }
    }

public:
    inline void SetTop( lua_State *L, stackItem_t topItem )
    {
        if ( this->top != topItem )
        {
            Lock( L );

            stackItem_t oldTop = this->top;

            if ( oldTop != topItem )
            {
                VerifyValidItem( topItem );

                if ( this->top )
                {
                    // If the valid stack item range is growing, then initialize the values it grows to.
                    InitializeRange( L, this->top + 1, topItem );
                }
                else
                {
                    // The stack has never been initialized before.
                    // In this case initialize it once.
                    InitializeRange( L, this->base, topItem );
                }

                this->top = topItem;

                // Notify the runtime about the changed top item.
                this->OnChangeTop( L, oldTop );
            }

            Unlock( L );
        }
    }

    class CtxItem
    {
    protected:
        friend struct growingStack;

        lua_State *L;
        growingStack *ownerStack;
        stackItem_t stackItem;

        // Must be constructed under a locked stack!
        inline void Setup( lua_State *L, growingStack *stack, stackItem_t theItem )
        {
            lua_assert( this->ownerStack == nullptr );

            this->L = L;
            this->ownerStack = stack;
            this->stackItem = theItem;

            stack->Lock( L );

            // Add ourselves to the stack.
            LIST_APPEND( stack->m_contextItems.root, this->node );
        }

    public:
        inline CtxItem( void )
        {
            // We cannot fill this struct unless it is filled by the stack!
            this->L = nullptr;
            this->ownerStack = nullptr;
            this->stackItem = nullptr;
        }

        inline CtxItem( CtxItem&& right )
        {
            growingStack *ownerStack = right.ownerStack;

            this->L = right.L;
            this->ownerStack = ownerStack;
            this->stackItem = right.stackItem;

            if ( ownerStack )
            {
                this->node.moveFrom( std::move( right.node ) );
            }

            right.L = nullptr;
            right.ownerStack = nullptr;
            right.stackItem = nullptr;
        }
        inline CtxItem( const CtxItem& right )
        {
            // Prepare the items.
            this->L = nullptr;
            this->ownerStack = nullptr;
            this->stackItem = nullptr;

            if ( right.stackItem != nullptr )
            {
                right.ownerStack->Lock( right.L );

                this->L = right.L;
                this->ownerStack = right.ownerStack;

                // Add us to the owner stack.
                LIST_APPEND( this->ownerStack->m_contextItems.root, this->node );
            }

            this->stackItem = right.stackItem;
        }

        inline ~CtxItem( void )
        {
            if ( ownerStack != nullptr )
            {
                // Make sure we remove ourselves from the owner stack.
                LIST_REMOVE( this->node );

                ownerStack->Unlock( L );

                this->ownerStack = nullptr;
                this->L = nullptr;
                this->stackItem = nullptr;
            }
        }

        inline void Lock( lua_State *L )
        {
            this->ownerStack->Lock( L );
        }

        inline void Unlock( lua_State *L )
        {
            this->ownerStack->Unlock( L );
        }

        inline growingStack* GetStack( void )
        {
            return this->ownerStack;
        }

        inline void Set( const dataType& value )
        {
            movementManagerType::SetItem( L, &value, stackItem );
        }

        inline void Get( dataType& outValue )
        {
            movementManagerType::SetItem( L, stackItem, &outValue );
        }

        inline stackItem_t* ValuePointer( void )
        {
            return &this->stackItem;
        }

        inline const stackItem_t Pointer( void ) const
        {
            return this->stackItem;
        }

        inline CtxItem& operator = ( CtxItem&& right )
        {
            this->~CtxItem();

            new (this) CtxItem( std::move( right ) );

            return *this;
        }
        inline CtxItem& operator = ( const CtxItem& right )
        {
            this->~CtxItem();

            new (this) CtxItem( right );

            return *this;
        }

        RwListEntry <CtxItem> node;
    };

    RwList <CtxItem> m_contextItems;

private:
    // Descriptor to avoid virtualizing the stack methods and to only provide
    // virtualization if necessary.
    template <typename newDataType>
    struct VirtualStackItemContextTemplate : public DataContext <newDataType>
    {
        LuaCachedConstructedClassAllocator <VirtualStackItemContextTemplate> *_usedAlloc;

        CtxItem stackItem;

        unsigned long refCount;

        inline VirtualStackItemContextTemplate( void )
        {
            this->refCount = 0;
        }

        void Reference( lua_State *L )
        {
            this->refCount++;
        }

        void Dereference( lua_State *L )
        {
            if ( this->refCount-- == 1 )
            {
                // Delete itself.
                _usedAlloc->Free( L, this );
            }
        }

        newDataType* const* GetValuePointer( lua_State *L )
        {
            return stackItem.ValuePointer();
        }
    };

public:
    typedef VirtualStackItemContextTemplate <dataType> VirtualStackItemContext;
    typedef VirtualStackItemContextTemplate <const dataType> ConstVirtualStackItemContext;

    // Remember to call Initialize and Shutdown
    LuaCachedConstructedClassAllocator <VirtualStackItemContext> _cachedVirtualStackItem;
    LuaCachedConstructedClassAllocator <ConstVirtualStackItemContext> _cachedVirtualStackItemConst;

    inline VirtualStackItemContext* GetNewVirtualStackItem( lua_State *L, CtxItem& theValue )
    {
        VirtualStackItemContext *vItem = _cachedVirtualStackItem.Allocate( L );

        vItem->stackItem = theValue;
        vItem->_usedAlloc = &_cachedVirtualStackItem;

        //vItem->Reference( L );

        return vItem;
    }

    inline VirtualStackItemContext* GetNewVirtualStackItem( lua_State *L, CtxItem&& theValue )
    {
        VirtualStackItemContext *vItem = _cachedVirtualStackItem.Allocate( L );

        vItem->stackItem = std::move( theValue );
        vItem->_usedAlloc = &_cachedVirtualStackItem;

        //vItem->Reference( L );

        return vItem;
    }

    inline ConstVirtualStackItemContext* GetNewVirtualStackItemConst( lua_State *L, CtxItem& theValue )
    {
        ConstVirtualStackItemContext *vItem = _cachedVirtualStackItemConst.Allocate( L );

        vItem->stackItem = theValue;
        vItem->_usedAlloc = &_cachedVirtualStackItemConst;

        //vItem->Reference( L );

        return vItem;
    }

    inline ConstVirtualStackItemContext* GetNewVirtualStackItemConst( lua_State *L, CtxItem&& theValue )
    {
        ConstVirtualStackItemContext *vItem = _cachedVirtualStackItemConst.Allocate( L );

        vItem->stackItem = std::move( theValue );
        vItem->_usedAlloc = &_cachedVirtualStackItemConst;

        //vItem->Reference( L );

        return vItem;
    }

    static FASTAPI stackItem_t StackSelector( stackOffset_t offset, stackOffset_t maxResolve, stackItem_t theBase, stackItem_t theTop )
    {
        stackItem_t result = nullptr;

        if ( offset >= 0 )
        {
            if ( offset < maxResolve )
            {
                result = theBase + offset;
            }
        }
        else if ( offset < 0 )
        {
            stackOffset_t real_offset = ( offset + 1 );

            if ( -real_offset < maxResolve )
            {
                result = theTop + real_offset;
            }
        }

        return result;
    }

    inline stackItem_t ResolveStackItem( lua_State *L, stackOffset_t offset, stackOffset_t maxResolve, stackItem_t theBase )
    {
        Lock( L );

        VerifyValidItem( theBase );

        stackItem_t result = StackSelector( offset, maxResolve, theBase, this->Top() );

        if ( result != nullptr )
        {
            VerifyValidItem( result );
        }

        Unlock( L );

        return result;
    }

    inline CtxItem GetStackItemBasedOn( lua_State *L, stackOffset_t offset, stackItem_t theBase )
    {
        CtxItem theItem;

        Lock( L );

        VerifyValidItem( theBase );

        stackOffset_t useCount = GetUsageCountBasedOn( L, theBase );

        stackItem_t result = ResolveStackItem( L, offset, useCount, theBase );

        if ( result != nullptr )
        {
            theItem.Setup( L, this, result );
        }

        Unlock( L );

        return theItem;
    }

    inline CtxItem GetStackItem( lua_State *L, stackOffset_t offset )
    {
        return GetStackItemBasedOn( L, offset, this->Base() );
    }

    inline void SetTopOffset( lua_State *L, stackOffset_t topOffset )
    {
        Lock( L );

        if ( topOffset > 0 )
        {
            EnsureSlots( L, topOffset );
        }

        stackItem_t newTop = ResolveStackItem( L, topOffset, this->GetSize(), this->base );

        lua_assert( newTop != nullptr );

        SetTop( L, newTop );

        Unlock( L );
    }

    inline bool GetTopOffsetBasedOn( lua_State *L, constStackItem_t theBase, stackOffset_t& topOffset ) const
    {
        bool success = false;

        if ( this->top )
        {
            Lock( L );

            if ( stackItem_t curTopItem = this->top )
            {
                VerifyValidItem( theBase );

                topOffset = GetDeferredStackOffset( theBase, curTopItem );

                success = true;
            }

            Unlock( L );
        }

        return success;
    }

    inline bool GetTopOffset( lua_State *L, stackOffset_t& topOffset ) const
    {
        return GetTopOffsetBasedOn( L, this->Base(), topOffset );
    }

    inline stackOffset_t GetUsageCountBasedOn( lua_State *L, constStackItem_t theBase ) const
    {
        stackOffset_t topOffset = 0;

        bool topSuccess = GetTopOffsetBasedOn( L, theBase, topOffset );

        if ( topSuccess )
        {
            topOffset++;
        }

        return topOffset;
    }

    inline stackOffset_t GetUsageCount( lua_State *L ) const
    {
        return GetUsageCountBasedOn( L, this->Base() );
    }

    inline static void MinUpdate( stackItem_t& updField, stackItem_t possibleMin )
    {
        if ( updField > possibleMin )
        {
            updField = possibleMin;
        }
    }

    inline static void MaxUpdate( stackItem_t& updField, stackItem_t possibleMax )
    {
        if ( updField < possibleMax )
        {
            updField = possibleMax;
        }
    }

    inline void MinUpdateTop( lua_State *L, stackItem_t possibleMin )
    {
        Lock( L );

        VerifyStackItem( possibleMin );

        MinUpdate( this->top, possibleMin );

        Unlock( L );
    }

    inline void MaxUpdateTop( lua_State *L, stackItem_t possibleMax )
    {
        Lock( L );

        VerifyStackItem( possibleMax );

        InitializeRange( L, this->top, possibleMax );

        MaxUpdate( this->top, possibleMax );

        Unlock( L );
    }

    inline stackOffset_t GetSize( void ) const
    {
        return this->size;
    }

    inline bool IsEmpty( lua_State *L ) const
    {
        bool isEmpty = false;

        Lock( L );

        isEmpty = ( this->top == nullptr );

        Unlock( L );

        return isEmpty;
    }

    inline bool IsFull( lua_State *L ) const
    {
        bool isFull = false;

        Lock( L );

        isFull = ( this->top == this->end );

        Unlock( L );

        return isFull;
    }

    inline void VerifyValidItem( constStackItem_t item ) const
    {
        EnsureLocked();

        // Verify that the item is properly allocated on this stack.
        lua_assert( item >= this->base && item <= Tail() );
    }

    inline void VerifyStackItem( constStackItem_t item ) const
    {
        EnsureLocked();

        // Verify that the item is really on the stack.
        lua_assert( item >= this->base && item <= Top() );
    }

private:
    inline void IncrementTop( lua_State *L, stackOffset_t n )
    {
        EnsureLocked();

        lua_assert( n >= 0 );

        stackItem_t currentTop = this->Top();

        for ( stackOffset_t iter = 0; iter < n; iter++ )
        {
            if ( currentTop == nullptr )
            {
                currentTop = this->Base();
            }
            else
            {
                ++currentTop;
            }
        }

        SetTop( L, currentTop );
    }

    inline void DecrementTop( lua_State *L, stackOffset_t n )
    {
        EnsureLocked();

        stackItem_t currentTop = this->Top();

        if ( currentTop != nullptr )
        {
            lua_assert( n >= 0 );

            stackItem_t currentBase = this->Base();

            for ( stackOffset_t iter = 0; iter < n; iter++ )
            {
                if ( currentTop == currentBase )
                {
                    currentTop = nullptr;
                    break;
                }
                else
                {
                    --currentTop;
                }
            }

            SetTop( L, currentTop );
        }
    }

public:
    inline static stackOffset_t GetDeferredStackOffset( constStackItem_t base, constStackItem_t item )
    {
        return ( item - base );
    }

    inline stackItem_t GetDeferredStackItem( constStackItem_t oldBase, constStackItem_t oldItem ) const
    {
        stackItem_t theBase = this->base;

        if ( theBase == nullptr )
        {
            return nullptr;
        }

        return ( theBase + GetDeferredStackOffset( oldBase, oldItem ) );
    }

    inline stackOffset_t GetStackOffset( lua_State *L, constStackItem_t item ) const
    {
        stackOffset_t offset = 0;

        Lock( L );

        VerifyStackItem( item );

        offset = GetDeferredStackOffset( this->base, item );

        Unlock( L );

        return offset;
    }

    inline void Grow( lua_State *L, stackOffset_t n )
    {
        lua_assert( n >= 0 );

        stackOffset_t stackSize = GetSize();

        if ( n <= stackSize )  /* double size is enough? */
        {
            SetSize( L, 2 * stackSize );
        }
        else
        {
            SetSize( L, stackSize + n );
        }
    }

    inline void EnsureSlots( lua_State *L, stackOffset_t n )
    {
        Lock( L );

        stackOffset_t stackSize = GetSize();
        stackOffset_t usedSlots = GetUsageCount( L );

        stackOffset_t availableSlots = ( stackSize - usedSlots );

        // If the stack does not have enough available slots...
        if ( availableSlots < n )
        {
            // ... the stack has to (at least) grow by the number of slots that are missing.
            Grow( L, n - availableSlots );
        }

        Unlock( L );
    }

protected:
    virtual void OnRebasing( lua_State *L, stackItem_t oldBase )
    {
        // Overwrite this function to do your own rebasing.
        // Do not forget to call this function though!
        stackOffset_t newSize = GetSize();

        if ( newSize != 0 )
        {
            if ( stackItem_t oldTop = this->top )
            {
                this->top = GetDeferredStackItem( oldBase, oldTop );
            }
        }
        else
        {
            this->top = nullptr;
        }

        // Fix all context items.
        LIST_FOREACH_BEGIN( CtxItem, m_contextItems.root, node )

            item->stackItem = GetDeferredStackItem( oldBase, item->stackItem );

        LIST_FOREACH_END
    }

    virtual void OnResizing( lua_State *L, stackOffset_t oldSize )
    {
        // Make sure you call this function if you overwrite this!
        return;
    }

    virtual void OnChangeTop( lua_State *L, stackItem_t oldTop )
    {
        // Make sure you call this function if you overwrite this!
        return;
    }

//private:
public:
    inline void SetSize( lua_State *L, stackOffset_t newSize )
    {
        lua_assert( newSize >= 0 );

        stackOffset_t oldSize = this->GetSize();

        if ( oldSize != newSize )
        {
            stackItem_t oldBase = Base();

            luaM_reallocvector( L, this->base, this->size, (size_t)newSize );

            this->size = newSize;

            if ( newSize != 0 )
            {
                lua_assert( this->base != nullptr );

                this->end = this->base + newSize - 1;
            }
            else
            {
                lua_assert( this->base == nullptr );

                this->end = nullptr;
            }

            // Notify about rebasing.
            if ( oldBase != this->base )
            {
                this->OnRebasing( L, oldBase );
            }

            // Notify about resizing.
            this->OnResizing( L, oldSize );
        }
    }

private:
    inline void EnsureSlot( lua_State *L )
    {
        Lock( L );

        // Make sure we have enough space on the stack.
        if ( IsFull( L ) )
        {
            // Grow it by double.
            SetSize( L, GetSize() * 2 );
        }

        Unlock( L );
    }

public:
    inline void Push( lua_State *L, const dataType& value )
    {
        Lock( L );

        // Save the value into our structure.
        movementManagerType::SetItem( L, &value, ObtainItem( L ) );

        Unlock( L );
    }

    inline void PopCountBasedOn( lua_State *L, constStackItem_t theBase, stackOffset_t count )
    {
        Lock( L );

        stackOffset_t onStackCount = this->GetUsageCountBasedOn( L, theBase );

        stackOffset_t realPopCount = std::min( count, onStackCount );

        lua_assert( realPopCount == count );

        DecrementTop( L, realPopCount );

        Unlock( L );
    }

    inline void PopCount( lua_State *L, stackOffset_t count )
    {
        PopCountBasedOn( L, this->Base(), count );
    }

    inline void Pop( lua_State *L )
    {
        PopCount( L, 1 );
    }

    inline void Lock( lua_State *L ) const
    {
        lockCount++;
    }

    inline bool RequiresLocking( void ) const
    {
        return true;
    }

    inline void Unlock( lua_State *L ) const
    {
        lua_assert( lockCount != 0 );

        lockCount--;
    }

    inline bool IsLocked( void ) const
    {
        return ( lockCount != 0 );
    }

    inline void EnsureLocked( void ) const
    {
        lua_assert( IsLocked() == true );
    }

    inline stackItem_t ObtainItem( lua_State *L )
    {
        // Make sure stack is locked before calling this function!
        EnsureLocked();

        EnsureSlot( L );

        IncrementTop( L, 1 );

        return this->Top();
    }

    inline CtxItem ObtainItemLocked( lua_State *L )
    {
        Lock( L );

        EnsureSlot( L );

        IncrementTop( L, 1 );

        CtxItem returnVal = GetStackItem( L, -1 );

        Unlock( L );

        return returnVal;
    }

    inline void VerifyIntegrity( lua_State *L )
    {
#ifdef _DEBUG
        Lock( L );

        lua_assert( GetDeferredStackOffset( this->base, this->end ) == GetSize() - 1 );

        Unlock( L );
#endif //_DEBUG
    }

    inline void Clear( void )
    {
        // TODO
        lua_assert( 0 );
    }

    FASTAPI static void NativeCrossCopyExtern( lua_State *L, stackItem_t to, constStackItem_t from, stackOffset_t moveCount )
    {
        for ( stackOffset_t n = 0; n < moveCount; n++ )
        {
            constStackItem_t sourceValue = from + n;

            stackItem_t dstValue = to + n;

            movementManagerType::SetItem( L, sourceValue, dstValue );
        }
    }

    FASTAPI static void NativeCrossCopy( lua_State *L, growingStack& toStack, constStackItem_t from, stackOffset_t copyCount )
    {
        toStack.Lock( L );

        // Ensure that we have enough items on the stack that are required for the copy.
        toStack.EnsureSlots( L, copyCount );

        // Copy over to the top of the stack.
        NativeCrossCopyExtern( L, toStack.Top(), from, copyCount );

        // Increment the top of the stack by the copy count.
        toStack.IncrementTop( L, copyCount );

        toStack.Unlock( L );
    }

    inline void CrossMoveTop( lua_State *L, growingStack& toStack, stackOffset_t numMove )
    {
        if ( numMove > 0 )
        {
            lua_assert( this != &toStack );
            lua_assert( numMove >= 0 );

            Lock( L );
            toStack.Lock( L );

            stackOffset_t currentStackUsageCount = this->GetUsageCount( L );

            lua_assert( currentStackUsageCount >= numMove );    // make sure we have enough items to move over

            if ( currentStackUsageCount >= numMove )
            {
                NativeCrossCopy( L, toStack, this->Top() - (numMove - 1), numMove );

                this->PopCount( L, numMove );
            }

            toStack.Unlock( L );
            Unlock( L );
        }
    }

    inline void CrossCopyTop( lua_State *L, growingStack& toStack, stackOffset_t numCopy ) const
    {
        if ( numCopy > 0 )
        {
            Lock( L );
            toStack.Lock( L );

            stackOffset_t currentStackUsageCount = this->GetUsageCount( L );

            lua_assert( currentStackUsageCount >= numCopy );    // make sure we have enough items to copy over

            if ( currentStackUsageCount >= numCopy )
            {
                NativeCrossCopy( L, toStack, this->Top() - (numCopy - 1), numCopy );
            }

            toStack.Unlock( L );
            Unlock( L );
        }
    }

    inline void CrossMoveTopExtern( lua_State *L, dataType *dstArray, stackOffset_t numMove )
    {
        if ( numMove > 0 )
        {
            Lock( L );

            stackOffset_t currentStackUsageCount = this->GetUsageCount( L );

            lua_assert( currentStackUsageCount >= numMove );    // make sure we have enough items to move over

            if ( currentStackUsageCount >= numMove )
            {
                DecrementTop( L, numMove );

                NativeCrossCopyExtern( L, dstArray, Top() + 1, numMove );
            }

            Unlock( L );
        }
    }

    inline void CrossCopyTopExtern( lua_State *L, dataType *dstArray, stackOffset_t numCopy ) const
    {
        if ( numCopy > 0 )
        {
            lua_assert( numCopy >= 0 );

            Lock( L );

            stackOffset_t currentStackUsageCount = this->GetUsageCount( L );

            lua_assert( currentStackUsageCount >= numCopy );    // make sure we have enough items to copy over

            if ( currentStackUsageCount >= numCopy )
            {
                NativeCrossCopyExtern( L, dstArray, Top() - (numCopy - 1), numCopy );
            }

            Unlock( L );
        }
    }

    inline void RemoveItemBasedOn( lua_State *L, stackItem_t theBase, stackOffset_t index )
    {
        Lock( L );

        {
            stackItem_t removePtr = GetStackItemBasedOn( L, index, theBase ).Pointer();
            stackItem_t topPtr = this->Top();

            while ( ++removePtr < topPtr )
            {
                movementManagerType::SetItem( L, removePtr, removePtr - 1 );
            }
        }

        DecrementTop( L, 1 );

        Unlock( L );
    }

    inline void RemoveItem( lua_State *L, stackOffset_t index )
    {
        RemoveItemBasedOn( L, this->Base(), index );
    }

public:
    FASTAPI static void ShiftedInsert( lua_State *L, stackItem_t stackTop, stackItem_t toBeInsertedAt, const dataType *itemToBeInserted )
    {
        // Push all items up one slot from the insertion position and put the new item at
        // the insertion position.

        // Shift items upwards.
        for ( stackItem_t iter = stackTop - 1; iter >= toBeInsertedAt; iter-- )
        {
            movementManagerType::SetItem( L, iter, iter + 1 );
        }

        // Insert the item.
        movementManagerType::SetItem( L, itemToBeInserted, toBeInsertedAt );
    }

    // WARNING: itemToBeInserted has to be external/not stack resident
    inline void InsertItemAtExternalBasedOn( lua_State *L, stackItem_t theBase, stackOffset_t index, const dataType *itemToBeInserted )
    {
        Lock( L );
        {
            CtxItem toBeInsertedAtCtx = GetStackItemBasedOn( L, index, theBase );

            // Increment the stack top.
            IncrementTop( L, 1 );

            stackItem_t toBeInsertedAt = toBeInsertedAtCtx.Pointer();
            stackItem_t stackTop = this->Top();

            // Do the insertion.
            ShiftedInsert( L, stackTop, toBeInsertedAt, itemToBeInserted );
        }
        Unlock( L );
    }

    inline void InsertItemAtExternal( lua_State *L, stackOffset_t index, const dataType *itemToBeInserted )
    {
        InsertItemAtExternalBasedOn( L, this->Base(), index, itemToBeInserted );
    }

    inline void TopInsertItemBasedOn( lua_State *L, stackItem_t theBase, stackOffset_t index )
    {
        Lock( L );
        {
            stackItem_t toBeInsertedAt = GetStackItemBasedOn( L, index, theBase ).Pointer();
            stackItem_t stackTop = this->Top();

            // Save the item to be inserted in a temporary slot.
            dataType tmp;
            movementManagerType::SetItem( L, stackTop, &tmp );

            // Do the insertion.
            ShiftedInsert( L, stackTop, toBeInsertedAt, &tmp );
        }
        Unlock( L );
    }

    inline void TopInsertItem( lua_State *L, stackOffset_t index )
    {
        TopInsertItemBasedOn( L, this->Base(), index );
    }

    inline void TopSwapItemBasedOn( lua_State *L, stackItem_t theBase, stackOffset_t index )
    {
        Lock( L );

        stackItem_t swapLeft = GetStackItemBasedOn( L, index, theBase ).Pointer();
        stackItem_t swapRight = this->Top();

        // Save the item to be swapped in a temporary slot.
        dataType tmp;
        movementManagerType::SetItem( L, swapLeft, &tmp );

        // Perform the swap.
        movementManagerType::SetItem( L, swapRight, swapLeft );
        movementManagerType::SetItem( L, &tmp, swapRight );

        Unlock( L );
    }

    inline void TopSwapItem( lua_State *L, stackOffset_t index )
    {
        TopSwapItemBasedOn( L, this->Base(), index );
    }

    struct stack_iterator
    {
        friend struct growingStack;

        lua_State *L;
        growingStack& theStack;
        dataType *curIter;
        dataType *endIter;

        inline stack_iterator( lua_State *L, growingStack& theStack, stackItem_t startIter, stackItem_t endIter ) : theStack( theStack )
        {
            this->L = L;

            this->curIter = startIter;
            this->endIter = endIter;
        }

        inline ~stack_iterator( void )
        {
            this->theStack.Unlock( this->L );
        }

        inline bool IsEnd( void ) const
        {
            return ( this->curIter == nullptr ) || ( this->curIter > this->endIter );
        }

        inline dataType* Resolve( void ) const
        {
            return this->curIter;
        }

        inline void Increment( void )
        {
            this->curIter++;
        }
    };

    inline stack_iterator GetRangedIterator( lua_State *L, stackItem_t start, stackItem_t end )
    {
        Lock( L );

        VerifyStackItem( start );
        VerifyStackItem( end );

        return stack_iterator( L, *this, start, end );
    }

    inline stack_iterator GetIterator( lua_State *L )
    {
        Lock( L );

        return stack_iterator( L, *this, Base(), Tail() );
    }
};

#endif //_LUA_STACK_UTILITIES_
