/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CModelInfo.h
*  PURPOSE:     Entity model info interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_MODELINFO
#define __CGAME_MODELINFO

#include <CVector.h>

#include "CColModel.h"
class CPedModelInfo;

enum eVehicleType : unsigned int
{
    VEHICLE_CAR,
    VEHICLE_MONSTERTRUCK,
    VEHICLE_QUADBIKE,
    VEHICLE_HELI,
    VEHICLE_PLANE,
    VEHICLE_BOAT,
    VEHICLE_TRAIN,
    VEHICLE_UNKNOWN,
    VEHICLE_FAKEPLANE,    // what is this?
    VEHICLE_BIKE,
    VEHICLE_BICYCLE,
    VEHICLE_AUTOMOBILETRAILER
};

class CBoundingBox //size: 40
{
public:
    CVector vecBoundMin;            // 0
    CVector vecBoundMax;            // 12
    CVector vecBoundOffset;         // 24, offset to the center of model
    float   fRadius;                // 36, radius encapsulating whole model
};

struct CBounds2D
{
    CBounds2D( void )
    { }

    CBounds2D( float minX, float maxY, float maxX, float minY ) : m_minX( minX ), m_maxY( maxY ), m_maxX( maxX ), m_minY( minY )
    { }

    CBounds2D( const CBounds2D& right ) : m_minX( right.m_minX ), m_maxY( right.m_maxY ), m_maxX( right.m_maxX ), m_minY( right.m_minY )
    { }

    // Is bbox inside this?
    // Method used by R*.
    // Takes into account that min is lower vector and max is upper vector.
    inline bool IsInside( const CBounds2D& bbox ) const
    {
        return ( bbox.m_minX <= m_maxX && bbox.m_maxX >= m_minX &&
                 bbox.m_minY <= m_maxY && bbox.m_maxY >= m_minY );
    }

    inline bool IsInside( const CVector2D& pos ) const
    {
        return ( pos.fX >= m_minX && pos.fX <= m_maxX &&
                 pos.fY >= m_minY && pos.fY <= m_maxY );
    }

    // Binary offset: 0x004042D0
    inline bool ContainsPoint( const CVector2D& pos, float subtract ) const
    {
        return CBounds2D( m_minX - subtract, subtract + m_maxY, subtract + m_maxX, m_minY - subtract ).IsInside( pos );
    }

    inline void AddBounds( const CBounds2D& bounds )
    {
        if ( bounds.m_minX < m_minX )
        {
            m_minX = bounds.m_minX;
        }

        if ( bounds.m_maxX > m_maxX )
        {
            m_maxX = bounds.m_maxX;
        }

        if ( bounds.m_minY < m_minY )
        {
            m_minY = bounds.m_minY;
        }

        if ( bounds.m_maxY > m_maxY )
        {
            m_maxY = bounds.m_maxY;
        }
    }

    inline void ExtendBy( float extendBy )
    {
        m_minX -= extendBy;
        m_maxX += extendBy;
        
        m_minY -= extendBy;
        m_maxY += extendBy;
    }

    // R*: fuck yea, let us confuse dem pplz by switching things 'round, and not using vectors.
    float m_minX, m_maxY;
    float m_maxX, m_minY;
};


enum eVehicleUpgradePosn
{
    VEHICLE_UPGRADE_POSN_BONET = 0,
    VEHICLE_UPGRADE_POSN_VENT,
    VEHICLE_UPGRADE_POSN_SPOILER,
    VEHICLE_UPGRADE_POSN_SIDESKIRT,
    VEHICLE_UPGRADE_POSN_FRONT_BULLBARS,
    VEHICLE_UPGRADE_POSN_REAR_BULLBARS,
    VEHICLE_UPGRADE_POSN_HEADLIGHTS,
    VEHICLE_UPGRADE_POSN_ROOF,
    VEHICLE_UPGRADE_POSN_NITRO,
    VEHICLE_UPGRADE_POSN_HYDRAULICS,
    VEHICLE_UPGRADE_POSN_STEREO,
    VEHICLE_UPGRADE_POSN_UNKNOWN,
    VEHICLE_UPGRADE_POSN_WHEELS,
    VEHICLE_UPGRADE_POSN_EXHAUST,
    VEHICLE_UPGRADE_POSN_FRONT_BUMPER,
    VEHICLE_UPGRADE_POSN_REAR_BUMPER,
    VEHICLE_UPGRADE_POSN_MISC,
};

class CModelInfo
{
public:
    virtual unsigned short                  GetModel() const = 0;

    virtual eVehicleType                    GetVehicleType() const = 0;
    virtual bool                            IsBoat() const = 0;
    virtual bool                            IsCar() const = 0;
    virtual bool                            IsTrain() const = 0;
    virtual bool                            IsHeli() const = 0;
    virtual bool                            IsPlane() const = 0;
    virtual bool                            IsBike() const = 0;
    virtual bool                            IsBmx() const = 0;
    virtual bool                            IsTrailer() const = 0;
    virtual bool                            IsVehicle() const = 0;
    virtual bool                            IsUpgrade() const = 0;

    virtual bool                            IsPed() const = 0;
    virtual bool                            IsObject() const = 0;

    virtual const char*                     GetNameIfVehicle() const = 0;

    virtual void                            Request( bool bAndLoad = false, bool bWaitForLoad = false, bool bHighPriority = false ) = 0;
    virtual void                            Remove() = 0;
    virtual unsigned char                   GetLevelFromPosition( const CVector& vecPosition ) const = 0;
    virtual bool                            IsLoaded() const = 0;
    virtual unsigned char                   GetFlags() const = 0;
    virtual const CBoundingBox*             GetBoundingBox() const = 0;
    virtual bool                            IsValid() = 0;
    virtual float                           GetDistanceFromCentreOfMassToBaseOfModel() const = 0;
    virtual unsigned short                  GetTextureDictionaryID() const = 0;
    virtual void                            SetTextureDictionaryID( unsigned short usID ) = 0;
    virtual float                           GetLODDistance() const = 0;
    virtual void                            SetLODDistance( float fDistance ) = 0;
    virtual void                            RestreamIPL() = 0;

    virtual void                            AddRef( bool bWaitForLoad, bool bHighPriority = false ) = 0;
    virtual int                             GetRefCount() const = 0;
    virtual void                            RemoveRef( bool bRemoveExtraGTARef = false ) = 0;
    virtual void                            MaybeRemoveExtraGTARef() = 0;
    virtual void                            DoRemoveExtraGTARef() = 0;

    // CVehicleModelInfo specific
    virtual short                           GetAvailableVehicleMod( unsigned short usSlot ) const = 0;
    virtual bool                            IsUpgradeAvailable( eVehicleUpgradePosn posn ) const = 0;
    virtual void                            SetCustomCarPlateText( const char *szText ) = 0;
    virtual unsigned int                    GetNumRemaps() const = 0;
    virtual void*                           GetVehicleSuspensionData() const = 0;
    virtual void*                           SetVehicleSuspensionData( void *pSuspensionLines ) = 0;

    // Upgrades only!
    virtual void                            RequestVehicleUpgrade() = 0;

    // ONLY use for peds
    virtual void                            GetVoice( short* psVoiceType, short* psVoice ) const = 0;
    virtual void                            GetVoice( const char** pszVoiceType, const char** szVoice ) const = 0;
    virtual void                            SetVoice( short sVoiceType, short sVoice ) = 0;
    virtual void                            SetVoice( const char* szVoiceType, const char* szVoice ) = 0;
};

#endif
