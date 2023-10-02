/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientPedBase.h
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

#ifndef _CLIENT_PED_BASE_
#define _CLIENT_PED_BASE_

// todo.

#if 0
class CClientPedBase : public CClientGameEntity
{
public:
    inline void                 Unlink()                                                    {}

    bool                        GetMatrix( RwMatrix& Matrix ) const;
    bool                        SetMatrix( const RwMatrix& Matrix );
    virtual CSphere             GetWorldBoundingSphere();

    void                        GetPosition( CVector& vecPosition ) const;

    void                        GetRotationDegrees( CVector& vecRotation ) const;
    void                        GetRotationRadians( CVector& vecRotation ) const;
    void                        SetRotationDegrees( const CVector& vecRotation );
    void                        SetRotationRadians( const CVector& vecRotation );


protected:
    CPed*                       m_pPed;
};
#endif

#endif //_CLIENT_PED_BASE_