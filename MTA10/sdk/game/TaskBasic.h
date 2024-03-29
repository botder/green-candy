/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/TaskBasic.h
*  PURPOSE:     Basic task interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_TASKBASIC
#define __CGAME_TASKBASIC

#include "Task.h"

class CTaskComplexUseMobilePhone : public virtual CTaskComplex
{
public:
    virtual                     ~CTaskComplexUseMobilePhone()   {};
};

class CTaskSimpleAnim : public virtual CTaskSimple
{
public:
    virtual                     ~CTaskSimpleAnim()  {};
};


class CTaskSimpleRunAnim : public virtual CTaskSimpleAnim
{
public:
    virtual                     ~CTaskSimpleRunAnim()   {};
};


class CTaskSimpleRunNamedAnim : public virtual CTaskSimpleAnim
{
public:
    virtual                     ~CTaskSimpleRunNamedAnim()  {};
};

class CTaskComplexDie : public virtual CTaskComplex
{
public:
    virtual                     ~CTaskComplexDie()  {};
};

class CTaskSimpleStealthKill : public virtual CTaskSimple
{
public:
    virtual                     ~CTaskSimpleStealthKill()   {};
};

class CTaskSimpleDead : public virtual CTaskSimple
{
public:
    virtual                     ~CTaskSimpleDead() {};
};

enum eSunbatherType
{
    SUNBATHER_MALE_1=0,
    SUNBATHER_MALE_2,
    SUNBATHER_FEMALE_1,
    SUNBATHER_FEMALE_2,
    SUNBATHER_FEMALE_3,
};

class CTaskComplexSunbathe : public virtual CTaskComplex
{
public:
    virtual                     ~CTaskComplexSunbathe() {};
    virtual void                SetEndTime( unsigned int time ) = 0;
};

////////////////////
// Player on foot //
////////////////////
class CTaskSimplePlayerOnFoot : public virtual CTaskSimple
{
public:
    virtual                     ~CTaskSimplePlayerOnFoot() {};
};

////////////////////
// Complex facial //
////////////////////
class CTaskComplexFacial : public virtual CTaskComplex
{
public:
    virtual                     ~CTaskComplexFacial() {};
};

#endif
