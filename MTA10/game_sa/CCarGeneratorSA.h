/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CCarEnterExitSA.h
*  PURPOSE:     Header file for car generator class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_CARGENERATOR
#define __CGAMESA_CARGENERATOR

#define WIN32_LEAN_AND_MEAN

#include <game/CCarGenerator.h>
#include <CVector.h>

class CCarGeneratorSAInterface
{
    short VisibleModel;
    unsigned char car_remap1, car_remap2;

    short nX, nY, nZ;
    unsigned char rot;
    unsigned char ChanceOfCarAlarm;
    unsigned char ChanceOfCarLocked;
    
    unsigned char bWaitUntilFarFromPlayer:1;
    unsigned char bHighPriority:1;
    unsigned char bActive:1;
    unsigned char bPlayerHasAlreadyOwnedCar:1;
    unsigned char bIgnorePopulationLimit:1;
    
    unsigned short MinDelay;
    unsigned short MaxDelay;
    unsigned int NextGen;
    
    short LatestCarID;
    unsigned short GenerateCount;
    unsigned char m_level;
    unsigned char m_used;
};

class CCarGeneratorSA : public CCarGenerator
{
private:
    CCarGeneratorSAInterface*       m_interface;

public:
    CCarGeneratorSA( CCarGeneratorSAInterface *_interface )
    {
        m_interface = _interface;
    }

    unsigned int        GetModelIndex               ( void );
    void                GetModelIndex               ( modelId_t index );

    const CVector&      GetPosition                 ( CVector& pos );
    void                SetPosition                 ( const CVector& pos );

    unsigned char       GetChanceOfCarAlarm         ( void );
    void                SetChanceOfCarAlarm         ( unsigned char alarm );

    unsigned char       GetChanceOfCarLocked        ( void );
    void                SetChanceOfCarLocked        ( unsigned char chance );

    bool                GetWaitUntilFarFromPlayer   ( void );
    void                SetWaitUntilFarFromPlayer   ( bool waitUntilPlayerFar );

    bool                IsHighPriority              ( void );
    void                SetHighPriority             ( bool highPriority );

    bool                IsActive                    ( void );
    void                SetActive                   ( bool active );

    bool                HasPlayerAlreadyOwnedCar    ( void );
    void                SetHasPlayerAlreadyOwnedCar ( bool hasOwned );

    bool                GetIgnorePopulationLimit    ( void );
    void                SetIgnorePopulationLimit    ( bool ignore );

    unsigned int        GetMinimumDelay             ( void );
    void                SetMinimumDelay             ( unsigned int minDelay );

    unsigned int        GetMaximumDelay             ( void );
    void                SetMaximumDelay             ( unsigned int maxDelay );

    unsigned int        GetNextGenTime              ( void );
    void                SetNextGenTime              ( unsigned int time);
};

#endif
