/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientVehicleManager.h
*  PURPOSE:     Vehicle entity manager class header
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Cecill Etheredge <ijsf@gmx.net>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCLIENTVEHICLEMANAGER_H
#define __CCLIENTVEHICLEMANAGER_H

#include "CClientCommon.h"
#include "CClientVehicle.h"

class CClientManager;
class CClientVehicle;

extern const unsigned char g_ucMaxPassengers[];

class CClientVehicleManager
{
    friend class CClientManager;
    friend class CClientVehicle;
public:
    void                            DeleteAll();

    inline unsigned int             Count()                                 { return m_List.size(); }
    static CClientVehicle*          Get( ElementID ID );
    CClientVehicle*                 Get( CVehicle* pVehicle, bool bValidatePointer );
    CClientVehicle*                 GetSafe( CEntity * pEntity );
    CClientVehicle*                 GetClosest( CVector& vecPosition, float fRadius );

    void                            DoPulse();

    static bool                     IsValidModel( unsigned short model );
    static eClientVehicleType       GetVehicleType( unsigned short ulModel );
    static unsigned char            GetMaxPassengerCount( unsigned short ulModel );
    static unsigned char            ConvertIndexToGameSeat( unsigned short ulModel, unsigned char ucIndex );

    static bool                     HasTurret( unsigned short ulModel );
    static bool                     HasSirens( unsigned short ulModel );
    static bool                     HasLandingGears( unsigned short ulModel );
    static bool                     HasAdjustableProperty( unsigned short ulModel );
    static bool                     HasSmokeTrail( unsigned short ulModel );
    static bool                     HasTaxiLight( unsigned short ulModel );
    static bool                     HasSearchLight( unsigned short ulModel );
    static bool                     HasDamageModel( unsigned short ulModel );
    static bool                     HasDamageModel( enum eClientVehicleType Type );
    static bool                     HasDoors( unsigned short ulModel );

    static bool                     IsVehicleLimitReached();

    void                            RestreamVehicles( unsigned short usModel );

    typedef std::vector <CClientVehicle*> vehicles_t;
    typedef std::list <CClientVehicle*> attached_t;

    vehicles_t::const_iterator      IterBegin()                             { return m_List.begin(); }
    vehicles_t::const_iterator      IterEnd()                               { return m_List.end(); }
    vehicles_t::const_iterator      StreamedBegin()                         { return m_StreamedIn.begin(); }
    vehicles_t::const_iterator      StreamedEnd()                           { return m_StreamedIn.end(); }
    
private:
                                    CClientVehicleManager( CClientManager* pManager );
                                    ~CClientVehicleManager();

    inline void                     AddToList( CClientVehicle* pVehicle )    { m_List.push_back ( pVehicle ); };
    void                            RemoveFromList( CClientVehicle* pVehicle );

    void                            OnCreation( CClientVehicle* pVehicle );
    void                            OnDestruction( CClientVehicle* pVehicle );

    CClientManager*                 m_pManager;
    vehicles_t                      m_List;
    vehicles_t                      m_StreamedIn;
    attached_t                      m_Attached;
};

#endif
