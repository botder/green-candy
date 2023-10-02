/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/TaskGoToSA.h
*  PURPOSE:     Go-to game tasks
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_TASKGOTO
#define __CGAMESA_TASKGOTO

#include <game/TaskGoTo.h>
#include <game/CPathFind.h>

#include "TaskSA.h"

// temporary
class CAnimBlendAssociation;
typedef DWORD CTaskUtilityLineUpPedWithCar;

#define FUNC_CTaskComplexWanderStandard__Constructor            0x48E4F0
#define FUNC_CTaskComplexWanderStandard__Destructor             0x48E600


#define FUNC_CTaskSimpleCarSetPedOut__PositionPedOutOfCollision     0x6479B0

// ##############################################################################
// ## Name:    CTaskComplexWander                                    
// ## Purpose: Generic task that makes peds wander around. Can't be used 
// ##          directly, use a superclass of this instead.
// ##############################################################################

class CTaskComplexWanderSAInterface : public CTaskComplexSAInterface
{
public:
    virtual int __thiscall              GetWanderType() = 0;
    virtual void __thiscall             Scan() = 0;
    virtual void __thiscall             UpdateDirection() = 0;
    virtual void __thiscall             UpdatePathNodes() = 0;

// protected
    int m_iMoveState;
    unsigned char m_iDir;
    float m_targetRadius;
       
    CNodeAddress m_LastNode;
    CNodeAddress m_NextNode;

    int m_lastUpdateDirFrameCount;

    unsigned char m_bWanderSensibly     :1;
// private
    unsigned char m_bNewDir             :1;
    unsigned char m_bNewNodes           :1;
    unsigned char m_bAllNodesBlocked    :1;
};

class CTaskComplexWanderSA : public CTaskComplexSA, public virtual CTaskComplexWander
{
public:
    CTaskComplexWanderSA () {};
    
    CNodeAddress *      GetNextNode();
    CNodeAddress *      GetLastNode();

    int                 GetWanderType();

    CTaskComplexWanderSAInterface*  GetInterface() { return (CTaskComplexWanderSAInterface*)m_interface; }
};

// ##############################################################################
// ## Name:    CTaskComplexWanderStandard                                    
// ## Purpose: Standard class used for making normal peds wander around
// ##############################################################################

class CTaskComplexWanderStandardSAInterface : public CTaskComplexWanderSAInterface
{
public:
// private  
    CTaskTimer m_timer;
    int m_iMinNextScanTime;
};

class CTaskComplexWanderStandardSA : public CTaskComplexWanderSA, public virtual CTaskComplexWanderStandard
{
public:
    CTaskComplexWanderStandardSA ( void ) {};
    CTaskComplexWanderStandardSA ( const int iMoveState, const unsigned char iDir, const bool bWanderSensibly=true );
};

#define MAX_GOTO_TASK_SIZE ( max(sizeof(CTaskComplexWanderSA),sizeof(CTaskComplexWanderStandardSA)) )

#endif
