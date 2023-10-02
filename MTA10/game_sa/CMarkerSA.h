/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CMarkerSA.h
*  PURPOSE:     Header file for marker entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_MARKER
#define __CGAMESA_MARKER

#include <game/CMarker.h>
#include "Common.h"

/**
 * \todo Ignore this class?
 */
class CEntryExit;

class CMarkerSAInterface
{
public:
    unsigned int        m_colorID;
    
    long                m_poolIndex; // parameter is used when Blip Type is CAR, CHAR or OBJECT
    CVector             m_position; // parameter is used when Blip Type is COORDS 

    unsigned short      m_referenceIndex;

    float               m_markerScale;
    unsigned short      m_blipScale;
    CEntryExit*         m_entryExit;

    unsigned char       m_sprite;

    unsigned char       m_bright : 1;
    unsigned char       m_tracking : 1;
    unsigned char       m_shortRange : 1;
    unsigned char       m_friendly : 1;    
    unsigned char       m_blipRemain : 1;
    unsigned char       m_blipFade : 1;
    unsigned char       m_coordBlipAppearance : 2;

    unsigned char       m_displayFlag : 2;
    unsigned char       m_blipType : 4;
};

class CMarkerSA : public CMarker
{
public:
    /* Our Functions */
                                CMarkerSA( CMarkerSAInterface *marker )     { m_interface = marker; };

    CMarkerSAInterface*         GetInterface()                              { return m_interface; };
    const CMarkerSAInterface*   GetInterface() const                        { return m_interface; }

    void                        Init();

    void                        SetSprite( eMarkerSprite Sprite )           { m_interface->m_sprite = Sprite; }
    void                        SetDisplay( eMarkerDisplay wDisplay )       { m_interface->m_displayFlag = wDisplay; }
    void                        SetScale( unsigned short scale );
    void                        SetColor( eMarkerColor color );
    void                        SetColor( const SColor color );
    void                        Remove();
    bool                        IsActive() const                            { return m_interface->m_blipType != MARKER_TYPE_UNUSED; }
    void                        SetPosition( const CVector& pos )           { m_interface->m_position = pos; }
    const CVector&              GetPosition() const                         { return m_interface->m_position; }
    void                        SetEntity( CVehicle *vehicle );
    void                        SetEntity( CPed *ped );
    void                        SetEntity( CObject *object );
    CEntity*                    GetEntity() const;

private:
    CMarkerSAInterface*         m_interface;
};

#endif
