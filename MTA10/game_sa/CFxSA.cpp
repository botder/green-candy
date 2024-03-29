/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CFxSA.cpp
*  PURPOSE:     Game effects handling
*       Rockstar Games has created a shared interface so that the engine could
*       quickly create commonly used particles. It is memory-efficient rather
*       than performance efficient. You guys know lag-smoke? 2dfx in GTA:SA is
*       known to cause lag. Maybe we can spot the reason if we analyze this class.
*  DEVELOPERS:  Jax <>
*               Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CFxSAInterface *g_effectManager = NULL;

CFxSAInterface::CFxSAInterface()
{
    // We need the particle system
    pParticleSystem->Init();

    

    m_count = 0;
}

CFxSAInterface::~CFxSAInterface()
{
    pParticleSystem->Shutdown();
}

void CFxSAInterface::AssociateGameTranslators()
{
    
}

CFxSA::CFxSA( CFxSAInterface *intf )
{
    m_interface = intf;
    g_effectManager = intf;

    return;

    // Do not let GTA SA load effects
    *(unsigned char*)FUNC_InitParticles = 0xC4;

    // Construct it ourselves
    new (intf) CFxSAInterface();
}

CFxSA::~CFxSA()
{
    return;

    m_interface->~CFxSAInterface();
}

void CFxSA::AddBlood ( CVector & vecPosition, CVector & vecDirection, int iCount, float fBrightness )
{
    CVector * pvecPosition = &vecPosition;
    CVector * pvecDirection = &vecDirection;
    DWORD dwThis = ( DWORD ) m_interface;
    DWORD dwFunc = FUNC_CFx_AddBlood;
    _asm
    {
        mov     ecx, dwThis
        push    fBrightness
        push    iCount
        push    pvecDirection
        push    pvecPosition
        call    dwFunc
    }
}


void CFxSA::AddWood ( CVector & vecPosition, CVector & vecDirection, int iCount, float fBrightness )
{
    CVector * pvecPosition = &vecPosition;
    CVector * pvecDirection = &vecDirection;
    DWORD dwThis = ( DWORD ) m_interface;
    DWORD dwFunc = FUNC_CFx_AddWood;
    _asm
    {
        mov     ecx, dwThis
        push    fBrightness
        push    iCount
        push    pvecDirection
        push    pvecPosition
        call    dwFunc
    }
}


void CFxSA::AddSparks ( CVector & vecPosition, CVector & vecDirection, float fForce, int iCount, CVector vecAcrossLine, unsigned char ucBlurIf0, float fSpread, float fLife )
{
    CVector * pvecPosition = &vecPosition;
    CVector * pvecDirection = &vecDirection;
    float fX = vecAcrossLine.fX, fY = vecAcrossLine.fY, fZ = vecAcrossLine.fZ;
    DWORD dwThis = ( DWORD ) m_interface;
    DWORD dwFunc = FUNC_CFx_AddSparks;
    _asm
    {
        mov     ecx, dwThis
        push    fLife
        push    fSpread
        push    ucBlurIf0
        push    fZ
        push    fY
        push    fX
        push    iCount
        push    fForce
        push    pvecDirection
        push    pvecPosition
        call    dwFunc
    }
}


void CFxSA::AddTyreBurst ( CVector & vecPosition, CVector & vecDirection )
{
    CVector * pvecPosition = &vecPosition;
    CVector * pvecDirection = &vecDirection;
    DWORD dwThis = ( DWORD ) m_interface;
    DWORD dwFunc = FUNC_CFx_AddTyreBurst;
    _asm
    {
        mov     ecx, dwThis
        push    pvecDirection
        push    pvecPosition
        call    dwFunc
    }
}


void CFxSA::AddBulletImpact ( CVector & vecPosition, CVector & vecDirection, int iSmokeSize, int iSparkCount, float fSmokeIntensity )
{
    CVector * pvecPosition = &vecPosition;
    CVector * pvecDirection = &vecDirection;
    DWORD dwThis = ( DWORD ) m_interface;
    DWORD dwFunc = FUNC_CFx_AddBulletImpact;
    _asm
    {
        mov     ecx, dwThis
        push    fSmokeIntensity
        push    iSparkCount
        push    iSmokeSize
        push    pvecDirection
        push    pvecPosition
        call    dwFunc
    }
}


void CFxSA::AddPunchImpact ( CVector & vecPosition, CVector & vecDirection, int i )
{
    CVector * pvecPosition = &vecPosition;
    CVector * pvecDirection = &vecDirection;
    DWORD dwThis = ( DWORD ) m_interface;
    DWORD dwFunc = FUNC_CFx_AddPunchImpact;
    _asm
    {
        mov     ecx, dwThis
        push    i
        push    pvecDirection
        push    pvecPosition
        call    dwFunc
    }
}


void CFxSA::AddDebris ( CVector & vecPosition, RwColor & rwColor, float fDebrisScale, int iCount )
{
    CVector * pvecPosition = &vecPosition;
    RwColor * pColor = &rwColor;
    DWORD dwThis = ( DWORD ) m_interface;
    DWORD dwFunc = FUNC_CFx_AddDebris;
    _asm
    {
        mov     ecx, dwThis
        push    iCount
        push    fDebrisScale
        push    pColor
        push    pvecPosition
        call    dwFunc
    }
}


void CFxSA::AddGlass ( CVector & vecPosition, RwColor & rwColor, float fDebrisScale, int iCount )
{
    CVector * pvecPosition = &vecPosition;
    RwColor * pColor = &rwColor;
    DWORD dwThis = ( DWORD ) m_interface;
    DWORD dwFunc = FUNC_CFx_AddGlass;   
    _asm
    {
        mov     ecx, dwThis
        push    iCount
        push    fDebrisScale
        push    pColor
        push    pvecPosition
        call    dwFunc
    }
}


void CFxSA::TriggerWaterHydrant ( CVector & vecPosition )
{
    CVector * pvecPosition = &vecPosition;
    DWORD dwThis = ( DWORD ) m_interface;
    DWORD dwFunc = FUNC_CFx_TriggerWaterHydrant; 
    _asm
    {
        mov     ecx, dwThis
        push    pvecPosition
        call    dwFunc
    }
}


void CFxSA::TriggerGunshot ( CEntity * pEntity, CVector & vecPosition, CVector & vecDirection, bool bIncludeSparks )
{
    DWORD dwEntity = ( pEntity ) ? ( DWORD )dynamic_cast <CEntitySA*> ( pEntity )->GetInterface () : NULL;
    CVector * pvecPosition = &vecPosition;
    CVector * pvecDirection = &vecDirection;
    DWORD dwThis = ( DWORD ) m_interface;
    DWORD dwFunc = FUNC_CFx_TriggerGunshot;
        _asm
    {
        mov     ecx, dwThis
        push    bIncludeSparks
        push    pvecDirection
        push    pvecPosition
        push    dwEntity
        call    dwFunc
    }
}


void CFxSA::TriggerTankFire ( CVector & vecPosition, CVector & vecDirection )
{
    CVector * pvecPosition = &vecPosition;
    CVector * pvecDirection = &vecDirection;
    DWORD dwThis = ( DWORD ) m_interface;
    DWORD dwFunc = FUNC_CFx_TriggerTankFire;
    _asm
    {
        mov     ecx, dwThis
        push    pvecDirection
        push    pvecPosition
        call    dwFunc
    }
}


void CFxSA::TriggerWaterSplash ( CVector & vecPosition )
{
    CVector * pvecPosition = &vecPosition;
    DWORD dwThis = ( DWORD ) m_interface;
    DWORD dwFunc = FUNC_CFx_TriggerWaterSplash;
    _asm
    {
        mov     ecx, dwThis
        push    pvecPosition
        call    dwFunc
    }
}


void CFxSA::TriggerBulletSplash ( CVector & vecPosition )
{
    CVector * pvecPosition = &vecPosition;
    DWORD dwThis = ( DWORD ) m_interface;
    DWORD dwFunc = FUNC_CFx_TriggerBulletSplash;
    _asm
    {
        mov     ecx, dwThis
        push    pvecPosition
        call    dwFunc
    }
}


void CFxSA::TriggerFootSplash ( CVector & vecPosition )
{
    CVector * pvecPosition = &vecPosition;
    DWORD dwThis = ( DWORD ) m_interface;
    DWORD dwFunc = FUNC_CFx_TriggerFootSplash;    
    _asm
    {
        mov     ecx, dwThis
        push    pvecPosition
        call    dwFunc
    }
}
