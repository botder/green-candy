/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPlayerPedSA.h
*  PURPOSE:     Header file for player ped entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               Alberto Alonso <rydencillo@gmail.com>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               aru <>
*               Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_PLAYERPED
#define __CGAMESA_PLAYERPED

#include <game/CPlayerPed.h>
#include "CWantedSA.h"

#define FUNC_SetInitialState            0x60CD20
#define FUNC_ClearWeaponTarget          0x533B30

#define FUNC_CPedOperatorNew            0x5E4720 // ##SA##
#define FUNC_CPlayerPedConstructor      0x60D5B0 // ##SA##

#define FUNC_CPlayerPedDestructor       0x6093B0 // ##SA##
#define FUNC_CPlayerPedOperatorDelete   0x5E4760 // ##SA##

#define FUNC_CPlayerPed_ReApplyMoveAnims    0x609650

class CPlayerPedSAInterface : public CPedSAInterface // CPlayerPed 1956 bytes in SA
{
public:
    CPlayerPedSAInterface( unsigned int playerIndex, unsigned int unk );

    void                OnFrame( void );
    bool                ShouldBeStealthAiming( void );
};

class CPlayerPedSA : public virtual CPlayerPed, public CPedSA
{
public:
                                CPlayerPedSA( CPlayerPedSAInterface *ped, unsigned short modelId, bool isLocal );
                                ~CPlayerPedSA();

    inline CPlayerPedSAInterface* GetInterface()                        { return (CPlayerPedSAInterface*)m_pInterface; }
    inline const CPlayerPedSAInterface* GetInterface() const            { return (const CPlayerPedSAInterface*)m_pInterface; }

    CPadSAInterface&            GetJoypad();

    void                        OnFrame();

    CWanted*                    GetWanted();

    float                       GetSprintEnergy() const;
    void                        SetSprintEnergy( float energy );

    void                        SetInitialState();

    eMoveAnim                   GetMoveAnim() const                             { return (eMoveAnim)GetInterface()->m_moveAnimGroup; }
    void                        SetMoveAnim( eMoveAnim iAnimGroup );

private:
    bool                                m_bIsLocal;
    class CPlayerPedDataSAInterface*    m_pData;
    CWantedSA*                          m_pWanted;
    bool                                m_stealthAiming;
};

#endif
