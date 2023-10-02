/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientPedBase.cpp
*  PURPOSE:     Ped entity base class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*               Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

//todo: create a ped base class so civs and peds share functionality

#if 0
bool CClientPedBase::GetMatrix ( RwMatrix& Matrix ) const
{
    // Are we frozen?
    if ( IsFrozen () )
    {
        Matrix = m_matFrozen;
    }    
    else
    if ( m_pPed )
    {
        m_pPed->GetMatrix( Matrix );
    }
    else
    {
        Matrix = m_Matrix;
    }

    return true;
}

bool CClientPedBase::SetMatrix ( const RwMatrix& Matrix )
{
    if ( m_pPed )
    {
        m_pPed->SetMatrix( Matrix );
    }

    if ( m_Matrix.vPos != Matrix.vPos )
    {
        UpdateStreamPosition ( Matrix.vPos );
    }
    m_Matrix = Matrix;
    m_matFrozen = Matrix;

    return true;
}

CSphere CClientPedBase::GetWorldBoundingSphere ( void )
{
    CSphere sphere;
    CModelInfo* pModelInfo = g_pGame->GetModelInfo ( GetModel () );
    if ( pModelInfo )
    {
        if ( const CBoundingBox *bounds = pModelInfo->GetBoundingBox() )
        {
            sphere.vecPosition = bounds->vecBoundOffset;
            sphere.fRadius = bounds->fRadius;
        }
    }
    sphere.vecPosition += GetStreamPosition ();
    return sphere;
}

void CClientPedBase::GetPosition( CVector& vecPosition ) const
{
    const CClientVehicle * pVehicle = GetRealOccupiedVehicle ();

    // Are we frozen?
    if ( IsFrozen () )
    {
        vecPosition = m_matFrozen.vPos;
    }    
    // Streamed in?
    else if ( m_pPed )
    {
        m_pPed->GetPosition ( vecPosition );
    }
    // In a vehicle?    
    else if ( pVehicle )
    {
        pVehicle->GetPosition ( vecPosition );
    }
    // Attached to an entity?
    else if ( m_pAttachedToEntity )
    {
        m_pAttachedToEntity->GetPosition ( vecPosition );
        vecPosition += m_vecAttachedPosition;
    }
    // None of the above?
    else
    {
        vecPosition = m_Matrix.vPos;
    }
}

void CClientPedBase::GetRotationDegrees ( CVector& vecRotation ) const
{
    // Grab our rotations in radians
    GetRotationRadians ( vecRotation );

    // Convert it to degrees
    vecRotation.fX = RAD2DEG( vecRotation.fX );;
    vecRotation.fY = RAD2DEG( vecRotation.fY );
    vecRotation.fZ = RAD2DEG( vecRotation.fZ );
}

void CClientPedBase::GetRotationRadians( CVector& vecRotation ) const
{
    RwMatrix matTemp;
    GetMatrix ( matTemp );
    matTemp.GetRotationRad( vecRotation.fX, vecRotation.fY, vecRotation.fZ );
}

void CClientPedBase::SetRotationDegrees( const CVector& vecRotation )
{
    // Convert from degrees to radians
    CVector vecTemp;
    vecTemp.fX = DEG2RAD( vecRotation.fX );
    vecTemp.fY = DEG2RAD( vecRotation.fY );
    vecTemp.fZ = DEG2RAD( vecRotation.fZ );

    // Set the rotation as radians
    SetRotationRadians ( vecTemp );

    // HACK: set again the z rotation to work on ground
    SetCurrentRotation ( vecTemp.fZ );
    if ( !IS_PLAYER ( this ) )
        SetCameraRotation ( vecTemp.fZ );
}

void CClientPedBase::SetRotationRadians( const CVector& vecRotation )
{
    // Grab the matrix, apply the rotation to it and set it again
    RwMatrix matTemp;
    GetMatrix ( matTemp );
    matTemp.SetRotationRad( vecRotation.fX, vecRotation.fY, vecRotation.fZ );
    SetMatrix ( matTemp );
}
#endif