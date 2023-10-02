/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        MTA10/core/CCompressorJobQueue.cpp
*  PURPOSE:     Threaded job queue
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CCompressorJobQueue.h"
#include "CFileFormat.h"
#include "SharedUtil.Thread.h"

#include <mutex>
#include <shared_mutex>


///////////////////////////////////////////////////////////////
//
// CCompressorJobQueueImpl
//
//
///////////////////////////////////////////////////////////////
class CCompressorJobQueueImpl : public CCompressorJobQueue
{
public:
    ZERO_ON_NEW
                                CCompressorJobQueueImpl       ( void );
    virtual                     ~CCompressorJobQueueImpl      ( void );

    virtual void                Initialize                  ( void );

    // Main thread functions
    virtual void                DoPulse                     ( void );
    virtual CCompressJobData*   AddCommand                  ( uint uiSizeX, uint uiSizeY, uint uiQuality, uint uiTimeSpentInQueue, PFN_SCREENSHOT_CALLBACK pfnScreenShotCallback, const CBuffer& buffer );
    virtual bool                PollCommand                 ( CCompressJobData* pJobData, uint uiTimeout );
    virtual bool                FreeCommand                 ( CCompressJobData* pJobData );

protected:
    void                        StopThread                  ( void );
    void                        RemoveUnwantedResults       ( void );
    void                        IgnoreJobResults            ( CCompressJobData* pJobData );

    // Other thread functions
    static void*                StaticThreadProc            ( void* pContext );
    void*                       ThreadProc                  ( void );
    void                        ProcessCommand              ( CCompressJobData* pJobData );
    void                        ProcessCompress             ( uint uiSizeX, uint uiSizeY, uint uiQuality, const CBuffer& inBuffer, CBuffer& outBuffer );

    // Main thread variables
    std::thread                             m_pServiceThread;
    std::set < CCompressJobData* >          m_IgnoreResultList;
    std::set < CCompressJobData* >          m_FinishedList;         // Result has been used, will be deleted next pulse

    bool                                    m_isInitialized;

    // Other thread variables
    // -none-
    
    // Shared variables
    struct
    {
        bool                                m_bTerminateThread;
        bool                                m_bThreadTerminated;
        std::list < CCompressJobData* >     m_CommandQueue;
        std::list < CCompressJobData* >     m_ResultQueue;
        std::mutex                          m_Mutex;
        std::condition_variable             m_CondIsItemOnQueue;
        
    } shared;
};


///////////////////////////////////////////////////////////////
// Object creation
///////////////////////////////////////////////////////////////
CCompressorJobQueue* NewCompressorJobQueue ( void )
{
    return new CCompressorJobQueueImpl();
}


///////////////////////////////////////////////////////////////
//
// CCompressorJobQueueImpl::CCompressorJobQueueImpl
//
// Init known database types and start the job service thread
//
///////////////////////////////////////////////////////////////
CCompressorJobQueueImpl::CCompressorJobQueueImpl ( void )
{
    // We have to start the thread later.
    m_isInitialized = false;
}

void CCompressorJobQueueImpl::Initialize( void )
{
    // Start the job queue processing thread.
    m_pServiceThread = std::thread( CCompressorJobQueueImpl::StaticThreadProc, this );

    m_isInitialized = true;
}


///////////////////////////////////////////////////////////////
//
// CCompressorJobQueueImpl::CCompressorJobQueueImpl
//
// Stop threads and delete everything
//
///////////////////////////////////////////////////////////////
CCompressorJobQueueImpl::~CCompressorJobQueueImpl ( void )
{
    // Stop the job queue processing thread
    StopThread ();

    // Thread is being deleted automatically.
}


///////////////////////////////////////////////////////////////
//
// CCompressorJobQueueImpl::StopThread
//
// Stop the job queue processing thread
//
///////////////////////////////////////////////////////////////
void CCompressorJobQueueImpl::StopThread ( void )
{
    // Stop the job queue processing thread
    shared.m_Mutex.lock();
    shared.m_bTerminateThread = true;
    shared.m_CondIsItemOnQueue.notify_all();
    shared.m_Mutex.unlock();

    for ( uint i = 0 ; i < 5000 ; i += 15 )
    {
        if ( shared.m_bThreadTerminated )
            break;

        Sleep ( 15 );
    }

    // If thread not stopped, (async) cancel it
    m_pServiceThread.join();
}


///////////////////////////////////////////////////////////////
//
// CCompressorJobQueueImpl::AddCommand
//
// AddCommand to queue
// Can't fail
//
///////////////////////////////////////////////////////////////
CCompressJobData* CCompressorJobQueueImpl::AddCommand ( uint uiSizeX, uint uiSizeY, uint uiQuality, uint uiTimeSpentInQueue, PFN_SCREENSHOT_CALLBACK pfnScreenShotCallback, const CBuffer& buffer )
{
    // Create command
    CCompressJobData* pJobData = new CCompressJobData ();
    pJobData->command.uiSizeX = uiSizeX;
    pJobData->command.uiSizeY = uiSizeY;
    pJobData->command.uiQuality = uiQuality;
    pJobData->command.buffer = buffer;

    pJobData->SetCallback ( pfnScreenShotCallback, uiTimeSpentInQueue );

    // Add to queue
    shared.m_Mutex.lock();
    pJobData->stage = EJobStage::COMMAND_QUEUE;
    shared.m_CommandQueue.push_back ( pJobData );
    shared.m_CondIsItemOnQueue.notify_all();
    shared.m_Mutex.unlock();

    return pJobData;
}


///////////////////////////////////////////////////////////////
//
// CCompressorJobQueueImpl::DoPulse
//
// Check if any callback functions are due
//
///////////////////////////////////////////////////////////////
void CCompressorJobQueueImpl::DoPulse ( void )
{
    if ( !m_isInitialized )
    {
        this->Initialize();
    }

    shared.m_Mutex.lock();

again:
    // Delete finished
    for ( std::set < CCompressJobData* >::iterator iter = m_FinishedList.begin () ; iter != m_FinishedList.end () ; )
    {
        CCompressJobData* pJobData = *iter;
        m_FinishedList.erase ( iter++ );
        // Check not refed
        assert ( !ListContains ( shared.m_CommandQueue, pJobData ) );
        assert ( !ListContains ( shared.m_ResultQueue, pJobData ) );
        assert ( !MapContains ( m_IgnoreResultList, pJobData ) );
        assert ( !MapContains ( m_FinishedList, pJobData ) );

        assert ( !pJobData->HasCallback () );
        SAFE_DELETE( pJobData );
    }

    // Remove ignored
    RemoveUnwantedResults ();

    // Do pending callbacks
    for ( std::list < CCompressJobData* >::iterator iter = shared.m_ResultQueue.begin () ; iter != shared.m_ResultQueue.end () ; ++iter )
    {
        CCompressJobData* pJobData = *iter;

        if ( pJobData->HasCallback () )
        {
            shared.m_Mutex.unlock();
            pJobData->ProcessCallback ();
            shared.m_Mutex.lock();

            // Redo from the top to ensure everything is consistent
            goto again;
        }
    }

    shared.m_Mutex.unlock();
}


///////////////////////////////////////////////////////////////
//
// CCompressorJobQueueImpl::RemoveUnwantedResults
//
// Check result queue items for match with ignore list items.
// * Must be called from inside a locked section *
//
///////////////////////////////////////////////////////////////
void CCompressorJobQueueImpl::RemoveUnwantedResults ( void )
{
    if ( m_IgnoreResultList.empty () )
        return;

    bool finished = false;

    while ( !finished )
    {
        finished = true;

        for ( std::list < CCompressJobData* >::iterator iter = shared.m_ResultQueue.begin () ; iter != shared.m_ResultQueue.end () ; )
        {
            CCompressJobData* pJobData = *iter;
            if ( MapContains ( m_IgnoreResultList, pJobData ) )
            {
                // Found result to ignore, remove from result and ignore lists, add to finished list
                iter = shared.m_ResultQueue.erase ( iter );
                MapRemove ( m_IgnoreResultList, pJobData );
                pJobData->stage = EJobStage::FINISHED;
                MapInsert ( m_FinishedList, pJobData );

                // Do callback incase any cleanup is needed
                if ( pJobData->HasCallback () )
                {
                    shared.m_Mutex.unlock();
                    pJobData->ProcessCallback ();
                    shared.m_Mutex.lock();
                    
                    finished = false;
                    break;
                }
            }
            else
                ++iter;
        }
    }
}


///////////////////////////////////////////////////////////////
//
// CCompressorJobQueueImpl::PollCommand
//
// Find result for previous command
// Returns false if result not ready.
//
///////////////////////////////////////////////////////////////
bool CCompressorJobQueueImpl::PollCommand ( CCompressJobData* pJobData, uint uiTimeout )
{
    bool bFound = false;

    std::unique_lock <std::mutex> lockHoldCommand( shared.m_Mutex );

    while ( true )
    {
        // Remove ignored before checking
        RemoveUnwantedResults ();

        // Find result with the required job handle
        for ( std::list < CCompressJobData* >::iterator iter = shared.m_ResultQueue.begin () ; iter != shared.m_ResultQueue.end () ; ++iter )
        {
            if ( pJobData == *iter )
            {
                // Found result. Remove from the result queue and flag return value
                shared.m_ResultQueue.erase ( iter );
                pJobData->stage = EJobStage::FINISHED;
                MapInsert ( m_FinishedList, pJobData );

                // Do callback incase any cleanup is needed
                if ( pJobData->HasCallback () )
                {
                    lockHoldCommand.unlock();
                    pJobData->ProcessCallback ();
                    lockHoldCommand.lock();
                }

                bFound = true;
                break;
            }
        }

        if ( bFound || uiTimeout == 0 )
        {
            lockHoldCommand.unlock();
            break;
        }

        shared.m_CondIsItemOnQueue.wait( lockHoldCommand,
            [&]()
        {
            return ( this->shared.m_bTerminateThread || this->shared.m_ResultQueue.empty() == false );
        });

        // If not infinite, break after next check
        if ( uiTimeout != (uint)-1 )
            uiTimeout = 0;
    }

    // Make sure if wait was infinite, we have a result
    assert ( uiTimeout != (uint)-1 || bFound );

    lockHoldCommand.release();

    return bFound;
}


///////////////////////////////////////////////////////////////
//
// CCompressorJobQueueImpl::FreeCommand
//
// Throw away result when this job is done
// Returns false if jobHandle not correct
//
///////////////////////////////////////////////////////////////
bool CCompressorJobQueueImpl::FreeCommand ( CCompressJobData* pJobData )
{
    if ( MapContains ( m_IgnoreResultList, pJobData ) )
        return false;       // Already ignoring query handle

    // if in command or result queue, then put in ignore result list
    bool bFound;
    {
        std::unique_lock <std::mutex> lockFetchCommand( shared.m_Mutex );

        bFound = ListContains ( shared.m_CommandQueue, pJobData ) || ListContains ( shared.m_ResultQueue, pJobData );
    }

    if ( !bFound )
    {
        // Must be in finished list
        if ( !MapContains ( m_FinishedList, pJobData ) )
            OutputDebugLine ( "FreeCommand: Serious problem #2 here\n" );
        return false; 
    }

    IgnoreJobResults ( pJobData );
    return true;
}


///////////////////////////////////////////////////////////////
//
// CCompressorJobQueueImpl::IgnoreJobResult
//
// Throw away the result for this job
// * Assumed job data is in either m_CommandQueue or m_ResultQueue *
//
///////////////////////////////////////////////////////////////
void CCompressorJobQueueImpl::IgnoreJobResults ( CCompressJobData* pJobData )
{
    dassert ( pJobData->stage <= EJobStage::RESULT );
    dassert ( !MapContains ( m_FinishedList, pJobData ) );
    MapInsert ( m_IgnoreResultList, pJobData );
}



//
//
//
// Job servicing thread
//
//
//

///////////////////////////////////////////////////////////////
//
// CCompressorJobQueueImpl::StaticThreadProc
//
// static function
// Thread entry point
//
///////////////////////////////////////////////////////////////
void* CCompressorJobQueueImpl::StaticThreadProc ( void* pContext )
{
    return ((CCompressorJobQueueImpl*)pContext)->ThreadProc ();
}


///////////////////////////////////////////////////////////////
//
// CCompressorJobQueueImpl::ThreadProc
//
// Job service loop
//
///////////////////////////////////////////////////////////////
void* CCompressorJobQueueImpl::ThreadProc ( void )
{
    std::unique_lock <std::mutex> lockServiceThread( shared.m_Mutex );

    while ( !shared.m_bTerminateThread )
    {
        // Is there a waiting command?
        shared.m_CondIsItemOnQueue.wait( lockServiceThread,
            [&]()
        {
            return ( shared.m_CommandQueue.empty() == false || shared.m_bTerminateThread );
        });

        if ( !shared.m_CommandQueue.empty () )
        {
            // Get next command
            CCompressJobData* pJobData = shared.m_CommandQueue.front ();
            pJobData->stage = EJobStage::PROCCESSING;
            lockServiceThread.unlock();

            // Process command
            ProcessCommand ( pJobData );

            // Store result
            lockServiceThread.lock();
            // Check command has not been cancelled (this should not be possible)
            if ( !shared.m_CommandQueue.empty () && pJobData == shared.m_CommandQueue.front () )
            {
                // Remove command
                shared.m_CommandQueue.pop_front ();
                // Add result
                pJobData->stage = EJobStage::RESULT;
                shared.m_ResultQueue.push_back ( pJobData );
            }
            shared.m_CondIsItemOnQueue.notify_all();
        }
    }

    shared.m_bThreadTerminated = true;

    return NULL;
}


///////////////////////////////////////////////////////////////
//
// CCompressorJobQueueImpl::ProcessCommand
//
//
//
///////////////////////////////////////////////////////////////
void CCompressorJobQueueImpl::ProcessCommand ( CCompressJobData* pJobData )
{
    uint uiSizeX = pJobData->command.uiSizeX;
    uint uiSizeY = pJobData->command.uiSizeY;
    uint uiQuality = pJobData->command.uiQuality;
    const CBuffer& inBuffer = pJobData->command.buffer;
    CBuffer& outBuffer = pJobData->result.buffer;

    // JPEG compress here
    JpegEncode ( uiSizeX, uiSizeY, uiQuality, inBuffer.GetData (), inBuffer.GetSize (), outBuffer );

    pJobData->result.status = EJobResult::SUCCESS;
}




//
//
//  CCompressJobData
//
//
//


///////////////////////////////////////////////////////////////
//
// CCompressJobData::SetCallback
//
// Set callback for a job data
// Returns false if callback could not be set
//
///////////////////////////////////////////////////////////////
bool CCompressJobData::SetCallback ( PFN_SCREENSHOT_CALLBACK pfnScreenShotCallback, uint uiTimeSpentInQueue )
{
    if ( callback.bSet )
        return false;   // One has already been set

    if ( this->stage > EJobStage::RESULT )
        return false;   // Too late to set a callback now

    // Set new
    callback.uiTimeSpentInQueue = uiTimeSpentInQueue;
    callback.pfnScreenShotCallback = pfnScreenShotCallback;
    callback.bSet = true;
    return true;
}


///////////////////////////////////////////////////////////////
//
// CCompressJobData::HasCallback
//
// Returns true if callback has been set and has not been called yet
//
///////////////////////////////////////////////////////////////
bool CCompressJobData::HasCallback ( void )
{
    return callback.bSet && !callback.bDone;
}


///////////////////////////////////////////////////////////////
//
// CCompressJobData::ProcessCallback
//
// Do callback
//
///////////////////////////////////////////////////////////////
void CCompressJobData::ProcessCallback ( void )
{
    assert ( HasCallback () );
    callback.bDone = true;
    callback.pfnScreenShotCallback( result.buffer, callback.uiTimeSpentInQueue );
}
