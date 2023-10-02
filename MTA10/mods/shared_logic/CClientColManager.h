/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientColManager.h
*  PURPOSE:     Collision entity manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCLIENTCOLMANAGER_H
#define __CCLIENTCOLMANAGER_H

#include "CClientCommon.h"
#include <list>

class CClientColShape;
class CClientEntity;

class CClientColManager
{
    friend class CClientColShape;
public:
                                    ~CClientColManager();

    void                            DoPulse();
    void                            DoHitDetection( const CVector& pos, float radius, CClientEntity *element, CClientColShape *pJustThis = NULL, bool children = false );

    bool                            Exists( CClientColShape* pShape );

    inline unsigned int             Count() const                                           { return m_List.size(); }

protected:
    inline void                     AddToList( CClientColShape *shape )                     { m_List.push_back( shape ); }
    void                            RemoveFromList( CClientColShape *shape );
    void                            DoHitDetectionOld( const CVector& pos, float radius, CClientEntity *element, CClientColShape *pJustThis = NULL, bool bChildren = false );
    void                            DoHitDetectionNew( const CVector& pos, float radius, CClientEntity *element, CClientColShape *pJustThis = NULL, bool bChildren = false );
    void                            DoHitDetectionForColShape( CClientColShape *shape );
    void                            DoHitDetectionForEntity( const CVector& pos, float fRadius, CClientEntity* pEntity );
    void                            HandleHitDetectionResult( bool bHit, CClientColShape* pShape, CClientEntity* pEntity );

    typedef std::vector <CClientColShape*> colshapes_t;

    colshapes_t m_List;
};

#endif