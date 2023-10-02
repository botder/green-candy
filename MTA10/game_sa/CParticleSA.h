/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CParticleSA.h
*  PURPOSE:     Header file for particle entity class
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_PARTICLE
#define __CGAMESA_PARTICLE

#include <game/CParticle.h>

enum eParticleStatus
{
    PARTICLE_STOPPED = 0,

    _FORCE_DWORD = 0xFFFFFFFF
};

class CParticleSAInterface
{
public:
    virtual                     ~CParticleSAInterface();

    virtual void __thiscall     Unk() = 0;
    virtual void __thiscall     Unk2() = 0;
    virtual void __thiscall     Remove();

    unsigned int                m_unk;
    unsigned int                m_unk2;
    unsigned int                m_unk3;
    eParticleStatus             m_status;
};

class CParticleSA : public CParticle
{
private:
    CParticleSAInterface        * internalInterface;
public:
};

#endif
