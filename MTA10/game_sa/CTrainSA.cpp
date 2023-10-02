/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CTrainSA.cpp
*  PURPOSE:     Train entity
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CTrainSA::CTrainSA( CTrainSAInterface *train ) : CVehicleSA( train )
{
    m_isDerailable = true;

    for ( unsigned int i = 0; i < MAX_DOORS; i++ )
        m_doors[i] = new CDoorSA( &GetInterface()->m_doors[i] );
}

CTrainSA::~CTrainSA()
{
    for ( unsigned int n = 0; n < MAX_DOORS; n++ )
        delete m_doors[n];
}

void CTrainSA::SetPosition( float x, float y, float z )
{
    CEntitySA::SetPosition( x, y, z );

    // If it's a train, recalculate its rail position parameter (does not affect derailed state)
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = FUNC_CVehicle_RecalcOnRailDistance;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}

void CTrainSA::SetMoveSpeed( const CVector& vecMoveSpeed )
{
    GetInterface()->m_vecLinearVelocity = vecMoveSpeed;

    // INACCURATE. Use Get/SetTrainSpeed instead of Get/SetMoveSpeed. (Causes issue #4829).
#if 0
    if ( IsDerailed() )
        return;

    CVehicleSAInterface* pInterf = GetVehicleInterface ();

    // Find the rail node we are on
    DWORD dwNumNodes = ((DWORD *)ARRAY_NumRailTrackNodes) [ pInterf->m_ucRailTrackID ];
    SRailNodeSA* pNode = ( (SRailNodeSA **) ARRAY_RailTrackNodePointers ) [ pInterf->m_ucRailTrackID ];
    SRailNodeSA* pNodesEnd = &pNode [ dwNumNodes ];

    while ( (float)pNode->sRailDistance / 3.0f <= pInterf->m_fTrainRailDistance && pNode < pNodesEnd )
    {
        pNode++;
    }

    if ( pNode >= pNodesEnd )
        return;

    // Get the direction vector between the nodes the train is between
    CVector vecNode1 ( (float)(pNode - 1)->sX / 8.0f, (float)(pNode - 1)->sY / 8.0f, (float)(pNode - 1)->sZ / 8.0f );
    CVector vecNode2 ( (float)pNode->sX / 8.0f, (float)pNode->sY / 8.0f, (float)pNode->sZ / 8.0f );
    CVector vecDirection = vecNode2 - vecNode1;
    vecDirection.Normalize ();
    // Set the speed
    pInterf->m_fTrainSpeed = vecDirection.DotProduct ( vecMoveSpeed );
#endif
}

CTrain* CTrainSA::GetNextTrainCarriage() const
{
    return (CTrainSA*)pGame->GetPools()->GetVehicle( GetInterface()->m_nextCarriage );
}

void CTrainSA::SetNextTrainCarriage( CTrain *next )
{
    CTrainSA *veh = dynamic_cast <CTrainSA*> ( next );

    if ( !veh )
    {
        GetInterface()->m_nextCarriage = NULL;
        return;
    }

    GetInterface()->m_nextCarriage = veh->GetInterface();

    if ( veh->GetPreviousTrainCarriage() != this )
        veh->SetPreviousTrainCarriage( this );
}

CTrain* CTrainSA::GetPreviousTrainCarriage() const
{
    return (CTrainSA*)pGame->GetPools()->GetVehicle( GetInterface()->m_prevCarriage );
}

void CTrainSA::SetPreviousTrainCarriage( CTrain *previous )
{
    CTrainSA *veh = dynamic_cast <CTrainSA*> ( previous );

    if ( !veh )
    {
        GetInterface()->m_prevCarriage = NULL;
        return;
    }

    GetInterface()->m_prevCarriage = veh->GetInterface();

    if ( veh->GetNextTrainCarriage() != this )
        veh->SetNextTrainCarriage( this );
}

void CTrainSA::SetDerailed( bool bDerailed )
{
    DWORD dwThis = (DWORD)m_pInterface;

    if ( bDerailed )
    {
        GetInterface()->m_trainFlags.bIsDerailed = true;
        GetInterface()->physicalFlags &= ~0x20004;
        return;
    }

    GetInterface()->m_trainFlags.bIsDerailed = false;
    GetInterface()->physicalFlags &= 0x20004;

    // Recalculate the on-rail distance from the start node (train position parameter, m_fTrainRailDistance)
    DWORD dwFunc = FUNC_CVehicle_RecalcOnRailDistance;
    _asm
    {
        mov     ecx,dwThis
        call    dwFunc
    }

    // Reset the speed
    GetInterface()->m_trainSpeed = 0.0f;
}

void CTrainSA::SetRailTrack( unsigned char track )
{
    if ( track > NUM_RAILTRACKS-1 )
        return;

    CTrainSAInterface *train = GetInterface();

    train->m_railTrackID = track;

    if ( IsDerailed () )
        return;

    DWORD dwFunc = FUNC_CVehicle_RecalcOnRailDistance;
    _asm
    {
        mov ecx, train
        call dwFunc
    }
}

CDoorSA* CTrainSA::GetDoor( unsigned char ucDoor )
{
    if ( ucDoor > MAX_DOORS-1 )
        return NULL;

    return m_doors[ucDoor];
}