/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CCheckpointsSA.cpp
*  PURPOSE:     Checkpoint entity manager
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CCheckpointsSA::CCheckpointsSA()
{
    DEBUG_TRACE("CCheckpointsSA::CCheckpointsSA()");

    for( unsigned int i = 0; i < MAX_CHECKPOINTS; i++ )
        m_checkpoints[i] = new CCheckpointSA( (CCheckpointSAInterface*)ARRAY_CHECKPOINTS + i );
}

CCheckpointsSA::~CCheckpointsSA()
{
    for ( unsigned int i = 0; i < MAX_CHECKPOINTS; i++ )
        delete m_checkpoints[i];
}

CCheckpointSA* CCheckpointsSA::CreateCheckpoint( unsigned int id, unsigned short type, const CVector& pos, const CVector& dir, float size, float pulseFraction, const SColor color )
{
    DEBUG_TRACE("CCheckpointSA* CCheckpointsSA::CreateCheckpoint( unsigned int id, unsigned short type, const CVector& pos, const CVector& dir, float size, float pulseFraction, const SColor color )");

    CCheckpointSA *pt = FindFreeMarker();

    if ( !pt )
        return NULL;

    pt->SetIdentifier( id );
    pt->Activate();
    pt->SetType( type );
    pt->SetPosition( pos );
    pt->SetPointDirection( dir );
    pt->SetSize( size );
    pt->SetColor( color );
    pt->SetPulsePeriod( 1024 );
    pt->SetPulseFraction( pulseFraction );
    pt->SetRotateRate( 1 );
    return pt;
}

CCheckpointSA* CCheckpointsSA::FindFreeMarker()
{
    DEBUG_TRACE("CCheckpointSA* CCheckpointsSA::FindFreeMarker()");

    for ( unsigned int i = 0; i < MAX_CHECKPOINTS; i++ )
    {
        if ( !m_checkpoints[i]->IsActive() ) 
            return m_checkpoints[i];
    }

    return NULL;
}
