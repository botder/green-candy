#include "StdInc.h"

CHeliSA::CHeliSA( CHeliSAInterface *intf ) : CAutomobileSA( intf )
{
    return;
}

CHeliSA::~CHeliSA( void )
{
    return;
}

void CHeliSA::SetWinchType( eWinchType winchType )
{
    GetInterface()->m_winchType = winchType;
}

void CHeliSA::PickupEntityWithWinch( CEntity *entity )
{
    return;
}

void CHeliSA::ReleasePickedUpEntityWithWinch( void )
{
    return;
}

void CHeliSA::SetRopeHeightForHeli( float height )
{
    return;
}

CPhysical* CHeliSA::QueryPickedUpEntityWithWinch( void )
{
    return NULL;
}