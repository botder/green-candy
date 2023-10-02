/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientMarker.h
*  PURPOSE:     Marker entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCLIENTMARKER_H
#define __CCLIENTMARKER_H

#include "CClientStreamElement.h"
#include "CClientMarkerCommon.h"
#include "CClient3DMarker.h"
#include "CClientCheckpoint.h"
#include "CClientCorona.h"
#include "CClientColShape.h"
#include "CClientColCallback.h"

class CClientMarkerManager;

#define LUACLASS_MARKER     59

class CClientMarker : public CClientStreamElement, private CClientColCallback
{
    friend class CClientMarkerManager;
    friend class CClientColShape;
public:
    enum eMarkerType
    {
        MARKER_CHECKPOINT,
        MARKER_RING,
        MARKER_CYLINDER,
        MARKER_ARROW,
        MARKER_CORONA,
        MARKER_INVALID = 0xFF,
    };

                                CClientMarker( class CClientManager* pManager, ElementID ID, lua_State *L, bool system, int iMarkerType );
                                ~CClientMarker();

    void                        Unlink();

    void                        GetPosition( CVector& vecPosition ) const;
    void                        SetPosition( const CVector& vecPosition );
    bool                        SetMatrix( const RwMatrix& matrix );

    void                        DoPulse();

    inline eClientEntityType    GetType() const                                     { return CCLIENTMARKER; }

    CClientMarker::eMarkerType  GetMarkerType() const;
    void                        SetMarkerType( CClientMarker::eMarkerType eType );

    class CClient3DMarker*      Get3DMarker();
    class CClientCheckpoint*    GetCheckpoint();
    class CClientCorona*        GetCorona();

    bool                        IsHit( const CVector& vecPosition ) const;
    bool                        IsHit( CClientEntity* pEntity ) const;

    bool                        IsVisible() const;
    void                        SetVisible( bool bVisible );

    SColor                      GetColor() const;
    void                        SetColor( const SColor color );

    float                       GetSize() const;
    void                        SetSize( float fSize );

    static int                  StringToType( const char* szString );
    static bool                 TypeToString( unsigned int uiType, char* szString );

    static bool                 IsLimitReached();

    inline CClientColShape*     GetColShape() { return m_pCollision; }

    void                        Callback_OnCollision( CClientColShape& Shape, CClientEntity& Entity );
    void                        Callback_OnLeave( CClientColShape& Shape, CClientEntity& Entity );

    virtual CSphere             GetWorldBoundingSphere();

protected:
    void                        StreamIn( bool bInstantly );
    void                        StreamOut();

private:
    void                        CreateOfType( int iType );

    CClientMarkerManager*       m_pMarkerManager;
    CClientMarkerCommon*        m_pMarker;

    static unsigned int         m_uiStreamedInMarkers;

    CClientColShape*            m_pCollision;
};

#endif
