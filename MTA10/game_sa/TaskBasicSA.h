/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/TaskBasicSA.h
*  PURPOSE:     Basic game tasks
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_TASKBASIC
#define __CGAMESA_TASKBASIC

#include <game/TaskBasic.h>

#include "TaskSA.h"

#define FUNC_CTaskComplexUseMobilePhone__Constructor        0x6348A0
#define FUNC_CTaskSimpleRunAnim__Constructor                0x61A900
#define FUNC_CTaskSimpleRunNamedAnim__Constructor           0x61A990
#define FUNC_CTaskComplexDie__Constructor                   0x630040
#define FUNC_CTaskSimpleStealthKill__Constructor            0x6225F0
#define FUNC_CTaskSimpleDead__Constructor                   0x630590
#define FUNC_CTaskComplexSunbathe__Constructor              0x631F80
#define FUNC_CTASKSimplePlayerOnFoot__Constructor           0x685750
#define FUNC_CTASKComplexFacial__Constructor                0x690D20


///////////////////////
//Use a mobile phone
///////////////////////

class CTaskComplexUseMobilePhoneSAInterface : public CTaskComplexSAInterface
{
public:
    int m_iDuration;
    CTaskTimer m_timer;
    bool m_bIsAborting;
    bool m_bQuit;
};


class CTaskComplexUseMobilePhoneSA : public CTaskComplexSA, public virtual CTaskComplexUseMobilePhone
{
public:
    CTaskComplexUseMobilePhoneSA ( void ) {};
    CTaskComplexUseMobilePhoneSA ( const int iDuration ); // Default is -1
};


// temporary
class CAnimBlendAssociation;
class CAnimBlendHierarchy;
typedef unsigned long AnimationId;


class CTaskSimpleAnimSAInterface: public CTaskSimpleSAInterface
{
public:
    CAnimBlendAssociation*      m_pAnim;
    char                        m_bIsFinished       :1;
    char                        m_bDontInterrupt    :1;
    char                        m_bHoldLastFrame    :1;

    // These flags are used in CTaskSimpleRunAnim only
    char                        m_bDontBlendOut     :1;

    // These flags are used in  CTaskSimpleRunNamedAnim only
    char                        m_bRunInSequence    :1;
    char                        m_bOffsetAtEnd      :1;
    char                        m_bOffsetAvailable  :1;
};

class CTaskSimpleAnimSA : public CTaskSimpleSA, public virtual CTaskSimpleAnim
{
public:
    CTaskSimpleAnimSA ( void ) {};
};

class CTaskSimpleRunAnimSA : public CTaskSimpleSA, public virtual CTaskSimpleRunAnim
{
public:
    CTaskSimpleRunAnimSA()  {}
    CTaskSimpleRunAnimSA( AssocGroupId animGroup, AnimationId animID, float fBlendDelta, int iTaskType, const char *pTaskName, bool bHoldLastFrame = false );
};

#define ANIM_NAMELEN 24
#define ANIMBLOCK_NAMELEN 16

class CTaskSimpleRunNamedAnimSAInterface: public CTaskSimpleAnimSAInterface
{
public:
    char                    m_animName [ANIM_NAMELEN];
    char                    m_animGroupName [ANIMBLOCK_NAMELEN];

    float                   m_fBlendDelta;
    CAnimBlendHierarchy*    m_pAnimHierarchy;
    int                     m_iTime;
    CTaskTimer              m_timer;
    CVector                 m_offsetAtEnd;
    int                     m_flags;    
    short                   m_animID;   
};

class CTaskSimpleRunNamedAnimSA : public CTaskSimpleAnimSA, public virtual CTaskSimpleRunNamedAnim
{
public:
    CTaskSimpleRunNamedAnimSA() {};
    CTaskSimpleRunNamedAnimSA( const char *pAnimName,
                               const char *pAnimGroupName,
                               int flags,
                               float fBlendDelta,
                               int iTime = -1,
                               bool bDontInterrupt = false,
                               bool bRunInSequence = false,
                               bool bOffsetPed = false,
                               bool bHoldLastFrame = false );
};

class CTaskComplexDieSAInterface : public CTaskComplexSAInterface
{
public:
};

class CTaskComplexDieSA : public CTaskComplexSA, public virtual CTaskComplexDie
{
public:
    CTaskComplexDieSA ( void ) {};
    CTaskComplexDieSA ( const eWeaponType eMeansOfDeath/*=WEAPONTYPE_UNARMED*/,
                        const AssocGroupId animGroup=0/*ANIM_STD_PED*/,
                        const AnimationId anim=0/*ANIM_STD_KO_FRONT*/, 
                        const float fBlendDelta=4.0f,
                        const float fAnimSpeed=0.0f,
                        const bool bBeingKilledByStealth=false,
                        const bool bFallingToDeath=false,
                        const int iFallToDeathDir=0,
                        const bool bFallToDeathOverRailing=false );
};

class CTaskSimpleStealthKillInterface : public CTaskSimpleSAInterface
{
public:
};

class CTaskSimpleStealthKillSA : public CTaskSimpleSA, public virtual CTaskSimpleStealthKill
{
public:
    CTaskSimpleStealthKillSA ( void ) {};
    CTaskSimpleStealthKillSA ( bool bKiller,
                               class CPed * pPed,
                               const AssocGroupId animGroup );
};

class CTaskSimpleDeadSAInterface : public CTaskSimpleSAInterface
{
public:
    unsigned int    uiDeathTimeMS;
    bool            bUnk2;
};

class CTaskSimpleDeadSA : public CTaskSimpleSA, public virtual CTaskSimpleDead
{
public:
    CTaskSimpleDeadSA ( void ) {};
    CTaskSimpleDeadSA ( unsigned int uiDeathTimeMS, bool bUnk2 );
};



class CAnimBlock;

class CTaskComplexSunbatheSAInterface : public CTaskComplexSAInterface
{
public:
    bool                m_bStartStanding;
    bool                m_bBathing;
    bool                m_bBeachAnimsReferenced;
    bool                m_bSunbatheAnimsReferenced;
    bool                m_bAborted;
    CTaskTimer          m_BathingTimer;
    eSunbatherType      m_SunbatherType;
    CAnimBlock*         m_pBeachAnimBlock;
    CAnimBlock*         m_pSunbatheAnimBlock;
    unsigned int        m_BeachAnimBlockIndex;
    unsigned int        m_SunbatheAnimBlockIndex;
    CObject*            m_pTowel;
};

class CTaskComplexSunbatheSA : public CTaskComplexSA, public virtual CTaskComplexSunbathe
{
public:
                        CTaskComplexSunbatheSA ( void ) {};
                        CTaskComplexSunbatheSA ( class CObject* pTowel, const bool bStartStanding );

    void                SetEndTime ( unsigned int time );

    CTaskComplexSunbatheSAInterface*    GetInterface() { return (CTaskComplexSunbatheSAInterface*)m_interface; }
};


////////////////////
// Player on foot //
////////////////////
class CTaskSimplePlayerOnFootSAInterface : public CTaskSimpleSAInterface
{
public:
    BYTE m_Pad[28];
};

class CTaskSimplePlayerOnFootSA : public CTaskSimpleSA, public virtual CTaskSimplePlayerOnFoot
{
public:
    CTaskSimplePlayerOnFootSA ( void );
};


////////////////////
// Complex facial //
////////////////////
class CTaskComplexFacialSAInterface : public CTaskComplexSAInterface
{
public:
    BYTE m_Pad[32];
};

class CTaskComplexFacialSA : public CTaskComplexSA, public virtual CTaskComplexFacial
{
public:
    CTaskComplexFacialSA ( void );
};

#define MAX_BASIC_TASK_SIZE ( max(sizeof(CTaskComplexDieSA), max(sizeof(CTaskComplexFacialSA), max(sizeof(CTaskComplexSunbatheSA), \
    max(sizeof(CTaskComplexUseMobilePhoneSA), max(sizeof(CTaskSimpleAnimSA), max(sizeof(CTaskSimpleDeadSA), max(sizeof(CTaskSimplePlayerOnFootSA), \
    max(sizeof(CTaskSimpleRunAnimSA), max(sizeof(CTaskSimpleRunNamedAnimSA), sizeof(CTaskSimpleStealthKillSA)))))))))) )

#endif
