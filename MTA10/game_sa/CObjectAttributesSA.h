/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CObjectAttributesSA.h
*  PURPOSE:     Header file for object attributes system
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _OBJECT_ATTRIBUTES_SA_
#define _OBJECT_ATTRIBUTES_SA_

class CEffectDefSAInterface;

// This is the container used by the engine to store OBJECT.DAT entries.
struct dynamicObjectData    //size: 80 bytes
{
    float mass;                         // 0
    float turnMass;                     // 4
    float airResistance;                // 8
    float elasticity;                   // 12
    float percSubmerged;                // 16
    float uproot;                       // 20
    float CDMult;                       // 24, collision damage multiplier
    unsigned char CDEff;                // 28, collision damage effect
    unsigned char SpCDR;                // 29, special collision damage reaction
    unsigned char CamAv;                // 30

    unsigned char explosion;            // 31, bool
    unsigned char fxType;               // 32

    BYTE pad4[3];                       // 33
    CVector fxOffset;                   // 36

    CEffectDefSAInterface *effDef;      // 48, pointer to 2dfx effect def
    float smashMult;                    // 52
    CVector smashVelocity;              // 56
    float smashRand;                    // 68

    unsigned int b_gun;                 // 72, gun hit reaction
    unsigned int b_spk;                 // 76, produce sparks bool
};

void ObjectAttributes_Init();
void ObjectAttributes_Shutdown();

#endif //_OBJECT_ATTRIBUTES_SA_