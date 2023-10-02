/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CTrainSA.h
*  PURPOSE:     Train entity header
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _CTrainSA_
#define _CTrainSA_

#include <game/CTrain.h>

#define FUNC_CTrain__GetDoorAngleOpenRatio      0x6F59C0

//006F6BD0  int GetTrainNodeNearPoint(float x, float y, float z, int* pTrackID) places track ID in *pTrackID and returns node ID
#define FUNC_GetTrainNodeNearPoint              0x6F6BD0

class CTrainSAInterface : public CVehicleSAInterface
{
public:
    unsigned char               m_trackNodeID;                          // 1440, Current node on train tracks
    BYTE                        m_pad[3];                               // 1441
    float                       m_trainSpeed;                           // 1444, Speed along rails
    float                       m_trainRailDistance;                    // 1448, Distance along rail starting from first rail node
    float                       m_distanceNextCarriage;                 // 1452
    DWORD                       m_pad2[2];                              // 1456
    CTrainFlags                 m_trainFlags;                           // 1464
    DWORD                       m_pad3;                                 // 1468
    unsigned char               m_railTrackID;                          // 1472
    BYTE                        m_pad4[15];                             // 1473
    CTrainSAInterface*          m_prevCarriage;                         // 1488
    CTrainSAInterface*          m_nextCarriage;                         // 1492

    CDoorSAInterface            m_doors[MAX_DOORS];                     // 1496
};

class CTrainSA : public virtual CTrain, public CVehicleSA
{
public:
                                CTrainSA( CTrainSAInterface *train );
                                ~CTrainSA();

    inline CTrainSAInterface*   GetInterface()                                      { return (CTrainSAInterface*)m_pInterface; }
    inline const CTrainSAInterface* GetInterface() const                            { return (const CTrainSAInterface*)m_pInterface; }

    void                        SetPosition( float x, float y, float z );
    void                        SetMoveSpeed( const CVector& moveSpeed );

    CDoorSA*                    GetDoor( unsigned char ucDoor );

    CTrainSAInterface*          GetNextCarriageInTrain() const                      { return GetInterface()->m_nextCarriage; }
    CTrain*                     GetNextTrainCarriage() const;
    void                        SetNextTrainCarriage( CTrain *next );
    CTrainSAInterface*          GetPreviousCarriageInTrain() const                  { return GetInterface()->m_prevCarriage; }
    CTrain*                     GetPreviousTrainCarriage() const;
    void                        SetPreviousTrainCarriage( CTrain *prev );

    bool                        IsDerailed() const                                  { return GetInterface()->m_trainFlags.bIsDerailed; }
    void                        SetDerailed( bool bDerailed );
    inline bool                 IsDerailable() const                                { return m_isDerailable; }
    inline void                 SetDerailable( bool enable )                        { m_isDerailable = enable; }
    float                       GetTrainSpeed() const                               { return GetInterface()->m_trainSpeed; }
    void                        SetTrainSpeed( float fSpeed )                       { GetInterface()->m_trainSpeed = fSpeed; }
    bool                        GetTrainDirection() const                           { return GetInterface()->m_trainFlags.bDirection != 0; }
    void                        SetTrainDirection( bool bDirection )                { GetInterface()->m_trainFlags.bDirection = bDirection; }
    unsigned char               GetRailTrack() const                                { return GetInterface()->m_railTrackID; }
    void                        SetRailTrack( unsigned char track );

protected:
    bool                        m_isDerailable;
    CDoorSA*                    m_doors[MAX_DOORS];
};

#endif //_CTrainSA_