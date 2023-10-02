// Lua common utilities (lists, etc).
#ifndef _LUA_COMMON_UTILITIES_
#define _LUA_COMMON_UTILITIES_

template <typename dataType>
struct SingleLinkedList
{
    // Integrate this into a struct!
    struct node
    {
        inline node( void )
        {
#ifdef _DEBUG
            next = nullptr;
#endif
        }

        node *next;
    };

    inline SingleLinkedList( void )
    {
        root = nullptr;
    }

    inline void SwapWith( SingleLinkedList& list )
    {
        node *myRoot = this->root;
        node *theirRoot = list.root;

        this->root = theirRoot;
        list.root = myRoot;
    }

    inline void SuspendTo( SingleLinkedList& list )
    {
        node *suspensionPoint = list.GetTail();

        if ( suspensionPoint )
        {
            suspensionPoint->next = this->root;
        }
        else
        {
            list.root = this->root;
        }
        this->root = nullptr;
    }

    static inline void InsertAfter( node *listNode, node *theNode )
    {
        theNode->next = listNode->next;
        listNode->next = theNode;
    }

    inline void Insert( node *theNode )
    {
        theNode->next = root;
        root = theNode;
    }

    inline void Clear( void )
    {
        root = nullptr;
    }

    inline node* GetFirst( void )
    {
        return root;
    }

    inline node* GetTail( void )
    {
        node *tail = this->root;

        if ( tail )
        {
            while ( node *nextNode = tail->next )
            {
                tail = nextNode;
            }
        }

        return tail;
    }

    inline bool IsEmpty( void ) const
    {
        return ( root == nullptr );
    }

    inline void RemoveFirst( void )
    {
        node *rootNode = this->root;

        if ( rootNode )
        {
            this->root = rootNode->next;
        }
    }

    struct iterator
    {
        node *iterNode;

        inline iterator( node *listNode )
        {
            iterNode = listNode;
        }

        inline void Increment( void )
        {
            iterNode = iterNode->next;
        }

        inline node* Resolve( void ) const
        {
            return iterNode;
        }

        inline bool IsEnd( void ) const
        {
            return ( iterNode == nullptr );
        }
    };

    inline iterator GetIterator( void )
    {
        return iterator( root );
    }

    struct removable_iterator
    {
        node **iter;

        inline removable_iterator( void )
        {
            this->iter = nullptr;
        }

        inline removable_iterator( node **iter )
        {
            this->iter = iter;
        }

        inline void operator = ( const removable_iterator& right )
        {
            this->iter = right.iter;
        }

        inline node* Resolve( void ) const
        {
            return *iter;
        }

        inline bool IsEnd( void ) const
        {
            return ( *iter == nullptr );
        }

        inline void Increment( void )
        {
            iter = &(*iter)->next;
        }

        inline void Remove( void )
        {
            node **remove_node_ptr = this->iter;

            // Overwrite the next field.
            *remove_node_ptr = (*this->iter)->next;
        }

        inline void Insert( node *theNode )
        {
            theNode->next = *this->iter;

            *this->iter = theNode;
        }
    };

    inline removable_iterator GetRemovableIterator( void )
    {
        return removable_iterator( &root );
    }

    inline void Remove( node *theNode )
    {
        node *prevNode = root;

        if ( prevNode == nullptr )
            return;

        if ( prevNode == theNode )
        {
            root = nullptr;
        }
        else
        {
            for ( iterator iter = iterator( root ); !iter.IsEnd(); iter.Increment() )
            {
                node *myNode = iter.Resolve();

                if ( myNode == theNode )
                {
                    prevNode->next = myNode->next;
                    break;
                }

                prevNode = myNode;
            }
        }
    }

    node *root;
};



#endif //_LUA_COMMON_UTILITIES_