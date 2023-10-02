/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CParticleObjectSA.h
*  PURPOSE:     Particle emitter
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_PARTICLEOBJECT
#define __CGAMESA_PARTICLEOBJECT

#include <game/CParticleObject.h>

class CParticleObjectSAInterface    //size: 260
{
public:
                                CParticleObjectSAInterface();
                                ~CParticleObjectSAInterface();

    void                        Stop();
    void                        DestroyParticles();

    BYTE                        m_pad3[76];             // 4
    unsigned char               m_unk;                  // 80
    unsigned char               m_unk2;                 // 81

    BYTE                        m_pad4[2];              // 82
    unsigned int                m_unk3;                 // 84
};

class CParticleObjectSA : public CParticleObject
{
private:
//  CParticleObjectSAInterface      * internalInterface;
public:
};

#endif
