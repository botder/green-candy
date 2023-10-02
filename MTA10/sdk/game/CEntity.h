/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CEntity.h
*  PURPOSE:     Base entity interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_ENTITY
#define __CGAME_ENTITY

#include "Common.h"
#include <CVector.h>

enum eEntityType
{
    ENTITY_TYPE_NOTHING,
    ENTITY_TYPE_BUILDING,
    ENTITY_TYPE_VEHICLE,
    ENTITY_TYPE_PED,
    ENTITY_TYPE_OBJECT,
    ENTITY_TYPE_DUMMY,
    ENTITY_TYPE_NOTINPOOLS
};

enum eEntityStatus
{
    STATUS_PLAYER,
    STATUS_PLAYER_PLAYBACKFROMBUFFER,
    STATUS_SIMPLE,
    STATUS_PHYSICS,
    STATUS_ABANDONED,
    STATUS_WRECKED,
    STATUS_TRAIN_MOVING,
    STATUS_TRAIN_NOT_MOVING,
    STATUS_HELI,
    STATUS_PLANE,
    STATUS_PLAYER_REMOTE,
    STATUS_PLAYER_DISABLED,
    STATUS_TRAILER,
    STATUS_SIMPLE_TRAILER
};

class CColModel;
class CTexture;

typedef bool (__cdecl*entityReferenceCallback_t)( class CEntity *entity );
typedef void (__cdecl*gameEntityPreRenderCallback_t)( void );
typedef void (__cdecl*gameEntityRenderCallback_t)( void );
typedef void (__cdecl*gameEntityRenderUnderwaterCallback_t)( void );
typedef void (__cdecl*gameEntityPostProcessCallback_t)( void );

enum eRenderModeValueType
{
    RMODE_VOID,
    RMODE_BOOLEAN,
    RMODE_FLOAT,
    RMODE_INT
};

struct rModeResult
{
    inline rModeResult( void )
    {
        successful = false;
    }

    inline rModeResult( bool result )
    {
        successful = result;
    }

    inline rModeResult( bool result, const char *failureReason )
    {
        successful = result;
        debugMsg = failureReason;
    }
    
    bool successful;
    std::string debugMsg;
};

enum eEntityRenderMode
{
    ENTITY_RMODE_LIGHTING,
    ENTITY_RMODE_LIGHTING_AMBIENT,
    ENTITY_RMODE_LIGHTING_DIRECTIONAL,
    ENTITY_RMODE_LIGHTING_POINT,
    ENTITY_RMODE_LIGHTING_SPOT,
    ENTITY_RMODE_LIGHTING_MATERIAL,
    ENTITY_RMODE_REFLECTION,
    ENTITY_RMODE_ALPHACLAMP,
    ENTITY_RMODE_MAX
};

class CEntity
{
public:
    virtual                             ~CEntity () {};

    virtual void                        SetPosition( float x, float y, float z ) = 0;
    virtual void                        SetPosition( const CVector& pos ) = 0;
    virtual void                        GetPosition( CVector& pos ) const = 0;
    virtual void                        Teleport( float x, float y, float z ) = 0;

    virtual void                        ProcessControl() = 0;
    virtual void                        SetupLighting() = 0;
    virtual void                        Render() = 0;

    virtual bool                        IsRwObjectPresent() const = 0;

    virtual void                        SetOrientation( float fX, float fY, float fZ )=0;
    virtual void                        FixBoatOrientation() = 0;
    virtual void                        GetMatrix( RwMatrix& matrix ) const = 0;
    virtual void                        SetMatrix( const RwMatrix& matrix ) = 0;

    virtual unsigned short              GetModelIndex() const = 0;
    virtual eEntityType                 GetEntityType() const = 0;

    virtual float                       GetBasingDistance() const = 0;

    virtual void                        SetEntityStatus( eEntityStatus bStatus ) = 0;
    virtual eEntityStatus               GetEntityStatus() const = 0;

    virtual bool                        IsOnScreen() const = 0;

    virtual void                        SetUnderwater( bool bUnderwater ) = 0;
    virtual bool                        GetUnderwater() const = 0;

    virtual bool                        IsVisible() const = 0;
    virtual void                        SetVisible( bool bVisible ) = 0;

    virtual rModeResult                 SetEntityRenderModeBool ( eEntityRenderMode rMode, bool value ) = 0;
    virtual rModeResult                 SetEntityRenderModeInt ( eEntityRenderMode rMode, int value ) = 0;
    virtual rModeResult                 SetEntityRenderModeFloat ( eEntityRenderMode rMode, float value ) = 0;

    virtual rModeResult                 GetEntityRenderModeBool ( eEntityRenderMode rMode, bool& value ) const = 0;
    virtual rModeResult                 GetEntityRenderModeInt ( eEntityRenderMode rMode, int& value ) const = 0;
    virtual rModeResult                 GetEntityRenderModeFloat ( eEntityRenderMode rMode, float& value ) const = 0;

    virtual rModeResult                 ResetEntityRenderMode ( eEntityRenderMode rMode ) = 0;

    virtual void                        SetDoNotRemoveFromGameWhenDeleted( bool bDoNotRemoveFromGame ) = 0;

    virtual bool                        IsUsingCollision() const = 0;
    virtual void                        SetUsesCollision( bool enabled ) = 0;
    virtual bool                        IsBackfaceCulled() const = 0;
    virtual void                        SetBackfaceCulled( bool enabled ) = 0;

    virtual void                        SetColModel( CColModel *col ) = 0;
    virtual CColModel*                  GetColModel() = 0;

    virtual void                        ReplaceTexture( const char *name, CTexture *tex ) = 0;
    virtual void                        RestoreTexture( const char *name ) = 0;

    virtual bool                        IsStatic() const = 0;
    virtual void                        SetStatic( bool enabled ) = 0;

    virtual void                        SetAlpha( unsigned char alpha ) = 0;

    virtual bool                        IsFading( void ) const = 0;
    virtual float                       GetFadingAlpha( void ) const = 0;

    virtual void*                       GetStoredPointer() const = 0;
    virtual void                        SetStoredPointer( void *pointer ) = 0;

    virtual unsigned char               GetAreaCode() const = 0;
    virtual void                        SetAreaCode( unsigned char areaCode ) = 0;

    virtual bool                        IsStaticWaitingForCollision() const = 0;
    virtual void                        SetStaticWaitingForCollision( bool bStatic ) = 0;

    virtual bool                        IsCollidableWith( CEntity *entity ) const = 0;
    virtual void                        SetCollidableWith( CEntity *entity, bool enable ) = 0;
};

#endif
