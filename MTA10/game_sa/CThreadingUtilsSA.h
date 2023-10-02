/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CThreadingUtilsSA.h
*  PURPOSE:     Classes for threading support
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _THREADING_SUPPORT_UTILITIES_
#define _THREADING_SUPPORT_UTILITIES_

// Thread-safe reference counting object holder, which is mutable.
// Assumes that the reference counting routines are thread-safe themselves.
template <typename refCountDataType>
struct acquisitionProtect
{
    // Thread-local immutable lockless reference counting object holder that holds a referenced
    // data pointer of any type.
    // Assumes that the reference counting routines are thread-safe themselves.
    // This acts as a safe pointer to "refCountDataType" (a.k.a. "smart pointer")
    class acquiredData
    {
        friend struct acquisitionProtect <refCountDataType>;

        // This method must only be used by acquisitionProtect::AcquireData !
        inline acquiredData( refCountDataType *referencedObject )
        {
            this->refObject = referencedObject;
        }

    public:
        inline acquiredData( void )
        {
            this->refObject = NULL;
        }

        inline acquiredData( const acquiredData& right )
        {
            this->refObject = right.refObject;

            if ( refCountDataType *ourData = this->refObject )
            {
                ourData->Reference();
            }
        }

        inline ~acquiredData( void )
        {
            if ( refCountDataType *ourData = this->refObject )
            {
                ourData->Dereference();
            }
        }

        inline void operator = ( const acquiredData& right )
        {
            refCountDataType *ourObject = this->refObject;
            refCountDataType *newObject = right.refObject;

            if ( ourObject != newObject )
            {
                if ( ourObject )
                {
                    ourObject->Dereference();
                }

                this->refObject = newObject;

                if ( newObject )
                {
                    newObject->Reference();
                }
            }
        }

        inline refCountDataType* operator -> ( void ) const
        {
            assume( refObject != NULL );
            return refObject;
        }

        inline operator bool ( void ) const
        {
            return ( refObject != NULL );
        }

        inline bool operator == ( const acquiredData& right ) const
        {
            return ( this->refObject == right.refObject );
        }

    protected:
        refCountDataType *refObject;
    };

    inline acquisitionProtect( void )
    {
        this->refCountData = NULL;
    }

    inline acquisitionProtect( refCountDataType *theData )
    {
        this->refCountData = theData;
    }

    inline ~acquisitionProtect( void )
    {
        SetData( NULL );
    }

    inline void SetData( refCountDataType *theData )
    {
        bool isInCriticalSection = true;

        EnterCriticalSection( &acquire_lock );

        refCountDataType *oldData = this->refCountData;

        if ( oldData != theData )
        {
            if ( oldData )
            {
                this->refCountData = NULL;

                if ( isInCriticalSection )
                {
                    LeaveCriticalSection( &acquire_lock );

                    isInCriticalSection = false;
                }

                oldData->Dereference();
            }

            if ( theData )
            {
                theData->Reference();

                if ( !isInCriticalSection )
                {
                    EnterCriticalSection( &acquire_lock );

                    isInCriticalSection = true;
                }

                refCountData = theData;

                if ( isInCriticalSection )
                {
                    LeaveCriticalSection( &acquire_lock );

                    isInCriticalSection = false;
                }
            }
        }

        if ( isInCriticalSection )
        {
            LeaveCriticalSection( &acquire_lock );
        }
    }

    inline bool HasData( void )
    {
        return ( this->refCountData != NULL );
    }

    inline acquiredData AcquireData( void ) const
    {
        refCountDataType *returnVal = NULL;

        bool isInCriticalSection = true;

        EnterCriticalSection( &acquire_lock );

        if ( refCountDataType *ourData = this->refCountData )
        {
            ourData->Reference();

            LeaveCriticalSection( &acquire_lock );

            isInCriticalSection = false;

            returnVal = ourData;
        }

        if ( isInCriticalSection )
        {
            LeaveCriticalSection( &acquire_lock );
        }

        return returnVal;
    }

    typedef updatableCachedDataList <acquiredData> updatableList_t;

protected:
    volatile refCountDataType *refCountData;

    mutable CRITICAL_SECTION acquire_lock;
};

#endif //_THREADING_SUPPORT_UTILITIES_