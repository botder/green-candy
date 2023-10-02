/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CObjectSA.h
*  PURPOSE:     Header file for object entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_OBJECT
#define __CGAMESA_OBJECT

#include <game/CObject.h>
#include "CPhysicalSA.h"
#include "CObjectAttributesSA.h"

#define FUNC_CObject_Create             0x5A1F60
#define FUNC_CObject_Explode            0x5A1340

//size: 412
//padlevel: 6
class CObjectSAInterface : public CPhysicalSAInterface // + 372 = burn time stop , +348 = scale // +340 = health
{
public:
    void __thiscall             _PreRender( void );

    void __thiscall             SetupFixedLighting( void );

    void*   operator new( size_t );
    void    operator delete( void *ptr );

    BYTE                        m_pad4[4];              // 312
    unsigned char               m_unk40;                // 316
    BYTE                        m_pad2[3];              // 317

    union
    {
        unsigned int                m_objFlags;         // 320

        struct
        {
            // flags
            uint32 b0x01 : 1; // 320
            uint32 b0x02 : 1;
            uint32 b0x04 : 1;
            uint32 b0x08 : 1;
            uint32 b0x10 : 1;
            uint32 b0x20 : 1;
            uint32 bExploded : 1;
            uint32 b0x80 : 1;

            uint32 b0x100 : 1; // 321
            uint32 b0x200 : 1;
            uint32 b0x400 : 1;
            uint32 b0x800 : 1;
            uint32 b0x1000 : 1;
            uint32 b0x2000 : 1;
            uint32 b0x4000 : 1;
            uint32 b0x8000 : 1;

            uint32 b0x10000 : 1; // 322
            uint32 bUpdateScale : 1;
            uint32 b0x40000 : 1;
            uint32 b0x80000 : 1;
            uint32 b0x100000 : 1;
            uint32 b0x200000 : 1;
            uint32 bFadingOutClump : 1;
            uint32 bFixedLighting : 1;

            uint32 b0x1000000 : 1; // 323
            uint32 b0x2000000 : 1;
            uint32 b0x4000000 : 1;
            uint32 b0x8000000 : 1;
            uint32 b0x10000000 : 1;
            uint32 b0x20000000 : 1;
            uint32 b0x40000000 : 1;
            uint32 b0x80000000 : 1;
        };
    };
    unsigned char               m_collDamEffect;        // 324
    BYTE                        m_pad2_2[4];            // 325
    CColLighting                objLighting;            // 329
    BYTE                        m_pad6[10];             // 330
    float                       m_health;               // 340
    DWORD                       m_pad3;                 // 344
    float                       fScale;                 // 348

    dynamicObjectData*          m_dynData;              // 352
    BYTE                        m_pad[12];              // 356

    CEntitySAInterface*         pGarageDoorDummy;       // 368
    unsigned int                objEffectSysTime;       // 372
    BYTE                        m_pad5[36];             // 376
};

void Objects_Init();
void Objects_Shutdown();

class CObjectSA : public virtual CObject, public CPhysicalSA
{
public:
                                CObjectSA( CObjectSAInterface *obj );
                                ~CObjectSA();

    void* operator new ( size_t );
    void operator delete ( void *ptr );

    inline CObjectSAInterface*  GetInterface()                                  { return (CObjectSAInterface*)m_pInterface; }
    inline const CObjectSAInterface*    GetInterface() const                    { return (const CObjectSAInterface*)m_pInterface; }

    unsigned int                GetPoolIndex() const                            { return m_poolIndex; }

    void                        Explode         ( void );
    void                        Break           ( void );
    void                        SetScaleValue   ( float scale );
    void                        SetScale        ( const CVector *vec );
    float                       GetScaleValue   ( void ) const;
    const CVector*              GetScale        ( void ) const;
    void                        SetHealth       ( float fHealth )               { GetInterface()->m_health = fHealth; }
    float                       GetHealth       ( void ) const                  { return GetInterface()->m_health; }
    void                        SetModelIndex   ( unsigned short ulModel );

    CRpAtomicSA*                CloneAtomic     ( void ) const;

    inline void                 SetAlpha        ( unsigned char ucAlpha )       { m_ucAlpha = ucAlpha; }
    inline unsigned char        GetAlpha        ( void ) const                  { return m_ucAlpha; }

    bool                        IsAGangTag      ( void ) const                  { return m_bIsAGangTag; }

private:
    void                        CheckForGangTag ( void );

    unsigned char               m_ucAlpha;
    bool                        m_bIsAGangTag;
    unsigned int                m_poolIndex;

    CVector                     m_scaleVec;
    bool                        m_hasScaleVector;

    bool                        m_hasCustomAttributes;
};

#include "CObjectSA.render.h"

#define MAX_DYNAMIC_OBJECT_DATA 4000

extern dynamicObjectData g_dynObjData[MAX_DYNAMIC_OBJECT_DATA];

#endif
