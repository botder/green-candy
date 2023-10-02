/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientColShape.h
*  PURPOSE:     Shaped collision entity base class
*  DEVELOPERS:  Jax <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCLIENTCOLSHAPE_H
#define __CCLIENTCOLSHAPE_H

#include "CClientEntity.h"

#define LUACLASS_COLSHAPE   44

class CClientColCallback;
class CClientMarker;
class CClientPickup;

enum eColShapeType
{
    COLSHAPE_CIRCLE,
    COLSHAPE_CUBOID,
    COLSHAPE_SPHERE,
    COLSHAPE_RECTANGLE,
    COLSHAPE_POLYGON,
    COLSHAPE_TUBE,
};

class CClientColShape : public CClientEntity
{
    friend class CClientMarker;
    friend class CClientPickup;
public:
                                        CClientColShape( class CClientManager *manager, ElementID ID, lua_State *L, bool system );
                                        ~CClientColShape();

    void                                Unlink();

    virtual eColShapeType               GetShapeType() = 0;

    void                                DoPulse();
    bool                                IsAttachable() const;

    inline eClientEntityType            GetType() const                                                     { return CCLIENTCOLSHAPE; }

    virtual void                        GetPosition( CVector& pos ) const                                   { pos = m_vecPosition; }
    virtual void                        SetPosition( const CVector& pos );

    virtual bool                        DoHitDetection( const CVector& pos, float radius ) = 0;

    inline bool                         IsEnabled() const                                                   { return m_bIsEnabled; }
    inline void                         SetEnabled( bool bEnabled )                                         { m_bIsEnabled = bEnabled; }

    void                                CallHitCallback( CClientEntity& Entity );
    void                                CallLeaveCallback( CClientEntity& Entity );
    inline CClientColCallback*          SetHitCallback( CClientColCallback *pCallback )                     { return ( m_pCallback = pCallback ); }

    inline bool                         GetAutoCallEvent() const                                            { return m_bAutoCallEvent; }
    inline void                         SetAutoCallEvent( bool bAutoCallEvent )                             { m_bAutoCallEvent = bAutoCallEvent; }

    void                                AddCollider( CClientEntity *entity )                                { m_Colliders.push_back( entity ); }
    void                                RemoveCollider( CClientEntity *entity )                             { if ( !m_Colliders.empty() ) m_Colliders.remove( entity ); }
    bool                                ColliderExists( CClientEntity *entity ) const;
    void                                RemoveAllColliders( bool bNotify );

    typedef std::list <CClientEntity*> entities_t;

    entities_t::iterator                CollidersBegin()                                                    { return m_Colliders.begin(); }
    entities_t::iterator                CollidersEnd()                                                      { return m_Colliders.end(); }

    void                                SizeChanged();

protected:
    CVector                             m_vecPosition;
    CClientMarker*                      m_pOwningMarker;
    CClientPickup*                      m_pOwningPickup;

private:
    bool                                m_bIsEnabled;
    class CClientColManager*            m_pColManager;
    CClientColCallback*                 m_pCallback;
    bool                                m_bAutoCallEvent;  
    entities_t                          m_Colliders;    
};

#endif
