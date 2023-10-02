/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CFireSA.h
*  PURPOSE:     Header file for fire class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Alberto Alonso <rydencillo@gmail.com>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_FIRE
#define __CGAMESA_FIRE

#include <game/CFire.h>
#include "Common.h"
#include "CEntitySA.h"

#define FUNC_Extinguish                 0x5393F0        //  ##SA##
#define FUNC_CreateFxSysForStrength     0x539360        //  ##SA##

class FxSystem_c; // we don't actually define this anywhere

class CFireSAInterface
{
public:
    bool                    m_active : 1;
    bool                    m_scripted : 1;
    bool                    m_makeNoise : 1;
    bool                    m_beingExtinguished : 1;
    bool                    m_fresh : 1;
    unsigned short          m_scriptReference;
    CVector                 m_position;
    CEntitySAInterface*     m_target;
    CEntitySAInterface*     m_creator;
    unsigned int            m_burnTime;
    float                   m_strength;
    char                    m_numGenerationsAllowed;
    unsigned char           m_clipDistance;

    FxSystem_c*             m_fxSysPtr;
};

class CFireSA : public CFire
{
private:
    CFireSAInterface*       m_interface;
public:
    // constructor
                                    CFireSA( CFireSAInterface *fire )               { m_interface = fire; }

    CFireSAInterface*               GetInterface()                                  { return m_interface; }
    const CFireSAInterface*         GetInterface() const                            { return m_interface; }

    void                            SetPosition( const CVector& pos );
    const CVector&                  GetPosition() const                             { return m_interface->m_position; }
    CEntity*                        GetCreator() const;
    CEntity*                        GetEntityOnFire() const;

    void                            Extinguish();
    void                            Ignite();
    void                            SetTimeToBurnOut( unsigned int time )           { m_interface->m_burnTime = time; }
    unsigned int                    GetTimeToBurnOut() const                        { return m_interface->m_burnTime; }

    bool                            IsIgnited() const                               { return m_interface->m_active; }
    bool                            IsFree() const                                  { return !m_interface->m_active && !m_interface->m_scripted; }
    bool                            IsBeingExtinguished() const                     { return m_interface->m_beingExtinguished; }

    void                            SetTarget( CEntity *entity );
    void                            SetSilent( bool bSilent )                       { m_interface->m_makeNoise = !bSilent; }

    float                           GetStrength() const                             { return m_interface->m_strength; }
    void                            SetStrength( float fStrength )                  { m_interface->m_strength = fStrength; }

    void                            SetNumGenerationsAllowed( char generations )    { m_interface->m_numGenerationsAllowed = generations; }
};

#endif
