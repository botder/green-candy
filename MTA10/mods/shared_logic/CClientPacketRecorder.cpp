/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientPacketRecorder.cpp
*  PURPOSE:     Client packet recorder and "demo" replay class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               The_GTA <quiret@gmx.de>
*
*****************************************************************************/

#include <StdInc.h>

unsigned char m_ucNextPacketID = 0;

CClientPacketRecorder::CClientPacketRecorder( CClientManager* pManager )
{
    // Init
    m_pManager = pManager;
    m_pfnPacketHandler = NULL;

    m_lRelative = 0;
    m_lFrames = 0;
    m_bPlaying = false;
    m_bRecording = false;
    m_bFrameBased = false;

    m_ulCurrentOffset = 0;
    m_lNextPacketTime = 0;
    m_uiFrameSkip = 1;
}

CClientPacketRecorder::~CClientPacketRecorder()
{
    Stop ();
}

void CClientPacketRecorder::SetPacketHandler( PPACKETHANDLER pfnPacketHandler )
{
    m_pfnPacketHandler = pfnPacketHandler;
}

void CClientPacketRecorder::StartPlayback( const char *szInput, bool bFrameBased )
{
    // Delete the filename
    m_filename.clear();

    // Did we get a filename?
    if ( !szInput )
        return;

    // Create a new filename
    m_filename = szInput;

    // Set the relative time/frame
    if ( bFrameBased )
        m_lRelative = m_lFrames;
    else
        m_lRelative = CClientTime::GetTime ();

    // We're now playing
    m_bPlaying = true;
    m_bFrameBased = bFrameBased;
}

void CClientPacketRecorder::StartRecord( const char *szOutput, bool bFrameBased )
{
    // Delete the filename
    m_filename.clear();

    // Did we get a filename?
    if ( !szOutput )
        return;

    // Create a new filename
    m_filename = szOutput;

    // Empty the file
    modFileRoot->Delete( szOutput );

    // Set the relative time/frame
    if ( bFrameBased )
        m_lRelative = m_lFrames;
    else
        m_lRelative = CClientTime::GetTime ();

    // We're now recording
    m_bRecording = true;
    m_bFrameBased = bFrameBased;
}

void CClientPacketRecorder::Stop()
{
    // Delete the filename
    m_filename.clear();

    // Reset
    m_lRelative = 0;
    m_bPlaying = false;
    m_bRecording = false;
    m_bFrameBased = false;
    m_ulCurrentOffset = 0;
    m_lNextPacketTime = 0;
}

bool CClientPacketRecorder::IsPlaying()
{
    return m_bPlaying;
}

bool CClientPacketRecorder::IsRecording()
{
    return m_bRecording;
}

bool CClientPacketRecorder::IsPlayingOrRecording()
{
    return m_bPlaying || m_bRecording;
}

bool CClientPacketRecorder::IsFrameBased()
{
    return m_bFrameBased;
}

void CClientPacketRecorder::SetFrameSkip( unsigned int uiFrameSkip )
{
    m_uiFrameSkip = uiFrameSkip;
}

void CClientPacketRecorder::RecordPacket( unsigned char ucPacketID, NetBitStreamInterface& bitStream )
{
    if ( !m_bRecording )
        return;

    // Open our file
    CFile *file = modFileRoot->Open( m_filename, "ab+" );

    if ( !file )
        return;

    // Write our timestamp
    if ( m_bFrameBased )
        file->WriteInt( m_lFrames - m_lRelative );
    else
        file->WriteInt( CClientTime::GetTime () - m_lRelative );

    // Write the packet ID
    file->WriteByte( ucPacketID );

    // Write the size of the bitstream
    size_t size = bitStream.GetNumberOfBytesUsed();
    file->WriteInt( size );

    // Write the content of the bitstream to the file
    char c = 0;

    for ( unsigned long i = 0; i < size; i++ )
    {
        bitStream.Read( c );
        file->WriteByte( c );
    }

    // Reset the readpointer so the client can use it
    bitStream.ResetReadPointer ();

    // Close the file
    delete file;
}

void CClientPacketRecorder::RecordLocalData( CClientPlayer* pLocalPlayer )
{
    if ( !m_bRecording )
        return;

    // Open our file
    CFile *file = modFileRoot->Open( m_filename, "ab+" );

    if ( !file )
        return;

    // Write our timestamp
    if ( m_bFrameBased )
        file->WriteInt( m_lFrames - m_lRelative );
    else
        file->WriteInt( CClientTime::GetTime() - m_lRelative );

//  fwrite ( &ulTimeStamp, sizeof ( unsigned long ), 1, pFile );

    // Write the packet ID
    file->WriteByte( (unsigned char)0xFE );

    // Grab the vehicle
    CClientVehicle *pVehicle = pLocalPlayer->GetOccupiedVehicle();

    if ( pVehicle )
    {
        // Read out the matrix, movespeed and turnspeed
        RwMatrix matrix;
        CVector speed;
        CVector turnSpeed;
        pVehicle->GetMatrix( matrix );
        pVehicle->GetMoveSpeed( speed );
        pVehicle->GetTurnSpeed( turnSpeed );

        // Write it
        file->Write( &matrix.vRight, sizeof(float), 3 );
        file->Write( &matrix.vFront, sizeof(float), 3 );
        file->Write( &matrix.vUp, sizeof(float), 3 );
        file->Write( &matrix.vPos, sizeof(float), 3 );

        file->Write( &speed, sizeof(float), 3 );
        file->Write( &turnSpeed, sizeof(float), 3 );

        file->WriteFloat( pVehicle->GetHealth() );

        // Write the controller state
        CControllerState cs = g_pGame->GetPad()->GetState();

        file->Write( &cs, sizeof(cs), 1 );
    }

    // Close the file
    delete file;
}

void CClientPacketRecorder::ReadLocalData( CFile *file )
{
    // Grab the local player
    CClientPlayer* pPlayer = m_pManager->GetPlayerManager()->GetLocalPlayer();

    if ( !pPlayer )
        return;

    // Grab the vehicle
    CClientVehicle* pVehicle = pPlayer->GetOccupiedVehicle();

    if ( !pVehicle )
        return;

    // Temp vars
    RwMatrix matrix;
    CVector speed;
    CVector turnSpeed;

    // Read it out
    file->Read( &matrix.vRight, sizeof(float), 3 );
    file->Read( &matrix.vFront, sizeof(float), 3 );
    file->Read( &matrix.vUp, sizeof(float), 3 );
    file->Read( &matrix.vPos, sizeof(float), 3 );

    file->Read( &speed, sizeof(float), 3 );
    file->Read( &turnSpeed, sizeof(float), 3 );

    float fHealth;
    file->ReadFloat( fHealth );

    pVehicle->SetHealth( fHealth );

    CControllerState cs;
    file->Read( &cs, sizeof(cs), 1 );

    // Set it
    pVehicle->SetMatrix( matrix );
    pVehicle->SetMoveSpeed( speed );
    pVehicle->SetTurnSpeed( turnSpeed );

    g_pGame->GetPad()->Disable( true );
    g_pGame->GetPad()->SetState( cs );
}

void CClientPacketRecorder::DoPulse()
{
    // Count the frame
    m_lFrames += m_uiFrameSkip;

    // Are we playing?
    if ( !m_bPlaying || !m_pfnPacketHandler )
        return;

    // Time to play the next packet?
    long lCurTime = (long)CClientTime::GetTime () - m_lRelative;
    //g_pCore->GetConsole()->Printf("current time:%u\n",lCurTime);

    if ( m_bFrameBased )
        __asm int 3;

    while ( ( m_bFrameBased && ( m_lFrames - m_lRelative >= m_lNextPacketTime ) ) || ( !m_bFrameBased && ( lCurTime >= m_lNextPacketTime ) ) )
    {
        // Load the file
        CFile *file = modFileRoot->Open( m_filename.c_str(), "rb" );
        
        if ( !file )
            break;

        // Seek to our current offset + the bytes occupied by the time?
        file->Seek( m_ulCurrentOffset, SEEK_SET );

        unsigned long ulTimeStamp;
        file->ReadStruct( ulTimeStamp );

        // Reached end of file?
        if ( file->IsEOF() )
        {
            delete file;

            Stop();
            return;
        }

        // Read out the packet id
        unsigned char id;
        file->ReadByte( id );

        // Is it 0xFE (local player data) or 0xFF (local keys)?
        switch( id )
        {
        case 0xFE:
            ReadLocalData( file );
            break;

        case 0xFD:
            _asm int 3;

            Stop ();
            return;

        default:
            // Read out number of bytes in the packet
            unsigned long ulSize;
            file->ReadStruct( ulSize );

            // Create a bitstream
            NetBitStreamInterface *pBitStream = g_pNet->AllocateNetBitStream();

            if ( !pBitStream )
                break;

            // Write the filedata to the bitstream
            for ( unsigned long i = 0; i < ulSize; i++ )
            {
                unsigned char byte;
                file->ReadByte( byte );

                pBitStream->Write( byte );
            }

            // Send it to the packethandler
            //g_pCore->GetConsole()->Printf("(time: %u, current: %u) %u\n",ulTimeStamp,lCurTime,ucPacketID);
            m_pfnPacketHandler( id, *pBitStream );

            // Destroy the bitstream
            g_pNet->DeallocateNetBitStream( pBitStream );
            break;
        }

        // Remember the new offset and read out the time for the next packet
        m_ulCurrentOffset = file->Tell();
        file->ReadStruct( m_lNextPacketTime );
        file->ReadStruct( m_ucNextPacketID );
        //g_pCore->GetConsole()->Printf("next time: %u\n",m_lNextPacketTime);

        // Reached end of file?
        if ( file->IsEOF() )
        {
            delete file;

            Stop ();
            return;
        }

        // Close the file
        delete file;
    }
}