/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CEntitySA.h
*  PURPOSE:     Header file for base entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Kent Simon <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               Christian Myhre Lundheim <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_ENTITY
#define __CGAMESA_ENTITY

#include "COffsets.h"
#include <game/CEntity.h>
#include <CVector2D.h>
#include <CVector.h>

#define FUNC_SetRwObjectAlpha                               0x5332C0

#define FUNC_SetOrientation                                 0x439A80

#define FUNC_CMatrix__ConvertToEulerAngles                  0x59A840
#define FUNC_CMatrix__ConvertFromEulerAngles                0x59AA40

#define FUNC_IsOnScreen                                     0x534540
#define FUNC_IsVisible                                      0x536BC0


// not in CEntity really
#define FUNC_RpAnimBlendClumpGetAssociation                 0x4D6870

#include "CPlaceableSA.h"


/** 
 * \todo Move CReferences (and others below?) into it's own file
 */
class CReferences
{
    CEntity     * pEntity;
};

struct CRect {
    float fX1, fY1, fX2, fY2;
};

// Entity flags
#define ENTITY_COLLISION        0x00000001
#define ENTITY_COLL_PROCESSED   0x00000002
#define ENTITY_STATIC           0x00000004
#define ENTITY_CONTACTED        0x00000008
#define ENTITY_STUCK            0x00000010
#define ENTITY_NOCOLLHIT        0x00000020
#define ENTITY_PROCESSLATER     0x00000040
#define ENTITY_VISIBLE          0x00000080
    
#define ENTITY_BIG              0x00000100
#define ENTITY_USEDAMAGE        0x00000200
#define ENTITY_DISABLESTREAMING 0x00000400
#define ENTITY_INVALID          0x00000800
#define ENTITY_COLL_WORLD       0x00001000
#define ENTITY_RENDERING        0x00002000
#define ENTITY_RENDER_LAST      0x00004000
#define ENTITY_FADE             0x00008000
    
#define ENTITY_NOSHADOWCAST     0x00010000
#define ENTITY_OFFSCREEN        0x00020000
#define ENTITY_WAITFORCOLL      0x00040000
#define ENTITY_NOSTREAM         0x00080000
#define ENTITY_UNDERWATER       0x00100000
#define ENTITY_PRERENDERED      0x00200000
#define ENTITY_DYNAMIC          0x00400000  // for buildings
#define ENTITY_CACHED_SKELETON  0x00800000
    
#define ENTITY_ROADSIGN         0x01000000
#define ENTITY_LOWLOD           0x02000000
#define ENTITY_PROC_OBJECT      0x04000000
#define ENTITY_BACKFACECULL     0x08000000
#define ENTITY_LIGHTING         0x10000000
#define ENTITY_UNIMPORTANT      0x20000000
#define ENTITY_TUNNEL           0x40000000
#define ENTITY_TUNNELTRANSITION 0x80000000


class CEntitySAInterface : public CPlaceableSAInterface
{
public:
                                    CEntitySAInterface      ( void );
                                    ~CEntitySAInterface     ( void );                       // 0

    virtual void __thiscall                 AddRect( CBounds2D rect );                              // 4
    virtual bool __thiscall                 AddToWorld();                                           // 8
    virtual void __thiscall                 RemoveFromWorld();                                      // 12
    virtual void __thiscall                 SetStatic( bool enabled );                              // 16
    virtual void __thiscall                 SetModelIndex( modelId_t id );                          // 20
    virtual void __thiscall                 SetModelIndexNoCreate( modelId_t id );                  // 24
    virtual void __thiscall                 CreateRwObject();                                       // 28
    virtual void __thiscall                 DeleteRwObject();                                       // 32
    virtual const CBounds2D& __thiscall     GetBoundingBox( CBounds2D& box );                       // 36
    virtual void __thiscall                 ProcessControl();                                       // 40
    virtual void __thiscall                 ProcessCollision();                                     // 44
    virtual void __thiscall                 ProcessShift();                                         // 48
    virtual bool __thiscall                 TestCollision();                                        // 52
    virtual void __thiscall                 Teleport( float x, float y, float z, int unk );         // 56
    virtual void __thiscall                 PreFrame();                                             // 60
    virtual bool __thiscall                 Frame();                                                // 64
    virtual void __thiscall                 PreRender();                                            // 68
    virtual void __thiscall                 Render();                                               // 72
    virtual unsigned char __thiscall        SetupLighting();                                        // 76
    virtual void __thiscall                 RemoveLighting( unsigned char id );                     // 80
    virtual void __thiscall                 Invalidate();                                           // 84

    unsigned char __thiscall        _SetupLighting          ( void );
    void __thiscall                 _RemoveLighting         ( unsigned char id );

    modelId_t                       GetModelIndex           ( void ) const          { return m_nModelIndex; }
    CBaseModelInfoSAInterface*      GetModelInfo            ( void ) const          { return ppModelInfo[GetModelIndex()]; }

    void                            GetPosition             ( CVector& pos ) const;
    const CVector&                  GetLODPosition          ( void ) const
    {
        if ( m_pLod )
            return m_pLod->Placeable.GetPosition();

        return Placeable.GetPosition();
    }

    CEntitySAInterface*             GetFinalLOD             ( void )
    {
        CEntitySAInterface *lod = m_pLod;

        if ( !lod )
            return this;

        while ( CEntitySAInterface *nextLOD = lod->m_pLod )
            lod = nextLOD;

        return lod;
    }

    float __thiscall                GetBasingDistance       ( void ) const;

    void                            SetAlpha                ( unsigned char alpha );
    CColModelSAInterface* __thiscall    GetColModel         ( void ) const;
    const CVector& __thiscall       GetCollisionOffset      ( CVector& out ) const;
    const CBounds2D& __thiscall     _GetBoundingBox         ( CBounds2D& out ) const;
    void __thiscall                 GetCenterPoint          ( CVector& out ) const;

    float __thiscall                GetRadius               ( void ) const;

    float                           GetFadingAlpha          ( void ) const;

    void __thiscall                 SetOrientation          ( float x, float y, float z );

    bool __thiscall                 IsOnScreen              ( void ) const;
    bool __thiscall                 CheckScreenValidity     ( void ) const;

    void __thiscall                 UpdateRwMatrix          ( void );
    void __thiscall                 UpdateRwFrame           ( void );

    bool __thiscall                 IsInStreamingArea       ( void ) const;

    inline RwObject*                GetRwObject             ( void )                { return m_pRwObject; }
    inline const RwObject*          GetRwObject             ( void ) const          { return m_pRwObject; }

    // System functions for communication with the mods.
    bool                            Reference               ( void );
    void                            Dereference             ( void );

    RwObject*               m_pRwObject;            // 24

    union
    {
        struct
        {
            /********** BEGIN CFLAGS **************/
            unsigned long bUsesCollision : 1;           // does entity use collision
            unsigned long bCollisionProcessed : 1;  // has object been processed by a ProcessEntityCollision function
            unsigned long bIsStatic : 1;                // is entity static
            unsigned long bHasContacted : 1;            // has entity processed some contact forces
            unsigned long bIsStuck : 1;             // is entity stuck
            unsigned long bIsInSafePosition : 1;        // is entity in a collision free safe position
            unsigned long bWasPostponed : 1;            // was entity control processing postponed
            unsigned long bIsVisible : 1;               //is the entity visible
            
            unsigned long bIsBIGBuilding : 1;           // Set if this entity is a big building
            unsigned long bRenderDamaged : 1;           // use damaged LOD models for objects with applicable damage
            unsigned long bStreamingDontDelete : 1; // Dont let the streaming remove this 
            unsigned long bRemoveFromWorld : 1;     // remove this entity next time it should be processed
            unsigned long bHasHitWall : 1;              // has collided with a building (changes subsequent collisions)
            unsigned long bImBeingRendered : 1;     // don't delete me because I'm being rendered
            unsigned long bDrawLast :1;             // draw object last
            unsigned long bDistanceFade :1;         // Fade entity because it is far away
            
            unsigned long bDontCastShadowsOn : 1;       // Dont cast shadows on this object
            unsigned long bOffscreen : 1;               // offscreen flag. This can only be trusted when it is set to true
            unsigned long bIsStaticWaitingForCollision : 1; // this is used by script created entities - they are static until the collision is loaded below them
            unsigned long bDontStream : 1;              // tell the streaming not to stream me
            unsigned long bUnderwater : 1;              // this object is underwater change drawing order
            unsigned long bHasPreRenderEffects : 1; // Object has a prerender effects attached to it
            unsigned long bIsTempBuilding : 1;          // whether or not the building is temporary (i.e. can be created and deleted more than once)
            unsigned long bDontUpdateHierarchy : 1; // Don't update the aniamtion hierarchy this frame
            
            unsigned long bHasRoadsignText : 1;     // entity is roadsign and has some 2deffect text stuff to be rendered
            unsigned long bDisplayedSuperLowLOD : 1;
            unsigned long bIsProcObject : 1;            // set object has been generate by procedural object generator
            unsigned long bBackfaceCulled : 1;          // has backface culling on
            unsigned long bLightObject : 1;         // light object with directional lights
            unsigned long bUnimportantStream : 1;       // set that this object is unimportant, if streaming is having problems
            unsigned long bTunnel : 1;          // Is this model part of a tunnel
            unsigned long bTunnelTransition : 1;        // This model should be rendered from within and outside of the tunnel
            /********** END CFLAGS **************/
        };
        unsigned long m_entityFlags;
    };

    unsigned short          m_randomSeed;           // 32
    short                   m_nModelIndex;          // 34
    CReferences*            m_references;           // 36

    struct streamingEntityReference_t;
    
    streamingEntityReference_t* m_streamingRef;     // 40
    
    unsigned short          m_nScanCode;            // 44
    unsigned char           m_iplIndex;             // 46, used to define which IPL file object is in
    unsigned char           m_areaCode;             // 47, used to define what objects are visible at this point
    
    union
    {
        CEntitySAInterface* m_pLod;                 // 48
        unsigned int        m_iLodIndex;
    };
    unsigned char           numLodChildren;         // 52
    unsigned char           numLodChildrenRendered; // 53

    //********* BEGIN CEntityInfo **********//
    BYTE                    nType : 3;              // 54 ( see ENTITY_TYPE_* )
    BYTE                    nStatus : 5;  
    //********* END CEntityInfo **********//

    unsigned char           m_pad;                  // 55

    /* IMPORTANT: Do not mess with interfaces. */
};

#include "CEntitySA.render.h"

namespace Entity
{
    void    SetReferenceCallbacks( entityReferenceCallback_t addRef, entityReferenceCallback_t delRef );
};

void Entity_Init( void );
void Entity_Shutdown( void );

#include "CEntitySA.rmode.h"

class CEntitySA : public virtual CEntity
{
    friend class COffsets;
public:
                                CEntitySA();
    virtual                     ~CEntitySA();

    CEntitySAInterface*         m_pInterface;

    inline CEntitySAInterface*  GetInterface()                                  { return m_pInterface; }
    inline const CEntitySAInterface*    GetInterface() const                    { return m_pInterface; }

    void                        SetPosition( float x, float y, float z );
    void                        SetPosition( const CVector& pos );
    void                        GetPosition( CVector& pos ) const;
    void                        Teleport( float x, float y, float z );

    void                        ProcessControl();
    void                        SetupLighting();
    void                        Render()                                        { m_pInterface->Render(); }

    bool                        IsRwObjectPresent() const                       { return m_pInterface->GetRwObject() != NULL; }

    void                        SetOrientation( float x, float y, float z );
    void                        FixBoatOrientation();        // eAi you might want to rename this
    void                        GetMatrix( RwMatrix& mat ) const;
    void                        SetMatrix( const RwMatrix& mat );

    unsigned short              GetModelIndex() const;
    eEntityType                 GetEntityType() const;

    float                       GetBasingDistance() const;

    bool                        IsOnScreen() const;

    void                        SetUnderwater( bool bUnderwater )               { BOOL_FLAG( m_pInterface->m_entityFlags, ENTITY_UNDERWATER, bUnderwater ); }
    bool                        GetUnderwater() const                           { return IS_FLAG( m_pInterface->m_entityFlags, ENTITY_UNDERWATER ); }

    bool                        IsVisible() const                               { return IS_FLAG( m_pInterface->m_entityFlags, ENTITY_VISIBLE ); }
    void                        SetVisible( bool enabled )                      { BOOL_FLAG( m_pInterface->m_entityFlags, ENTITY_VISIBLE, enabled ); }

    rModeResult                 SetEntityRenderModeBool ( eEntityRenderMode rMode, bool value );
    rModeResult                 SetEntityRenderModeInt ( eEntityRenderMode rMode, int value );
    rModeResult                 SetEntityRenderModeFloat ( eEntityRenderMode rMode, float value );

    rModeResult                 GetEntityRenderModeBool ( eEntityRenderMode rMode, bool& value ) const;
    rModeResult                 GetEntityRenderModeInt ( eEntityRenderMode rMode, int& value ) const;
    rModeResult                 GetEntityRenderModeFloat ( eEntityRenderMode rMode, float& value ) const;

    rModeResult                 ResetEntityRenderMode ( eEntityRenderMode rMode );

    unsigned char               GetAreaCode() const;
    void                        SetAreaCode( unsigned char areaCode );

    void                        SetEntityStatus( eEntityStatus bStatus );
    eEntityStatus               GetEntityStatus() const;

    RwObject*                   GetRwObject() const;

    void                        SetDoNotRemoveFromGameWhenDeleted( bool b )     { m_doNotRemoveFromGame = b; };

    bool                        IsStatic() const                                { return IS_FLAG( m_pInterface->m_entityFlags, ENTITY_STATIC ); }
    void                        SetStatic( bool enabled )                       { BOOL_FLAG( m_pInterface->m_entityFlags, ENTITY_STATIC, enabled ); }
    bool                        IsUsingCollision() const                        { return IS_FLAG( m_pInterface->m_entityFlags, ENTITY_COLLISION ); }
    void                        SetUsesCollision( bool enabled )                { BOOL_FLAG( m_pInterface->m_entityFlags, ENTITY_COLLISION, enabled ); }

    void                        SetColModel( CColModel *col )                   { m_colModel = (CColModelSA*)col; };
    CColModelSA*                GetColModel()                                   { return m_colModel; }

    void                        ReplaceTexture( const char *name, CTexture *tex );
    void                        RestoreTexture( const char *name );

    bool                        IsBackfaceCulled() const                        { return IS_FLAG( m_pInterface->m_entityFlags, ENTITY_BACKFACECULL ); }
    void                        SetBackfaceCulled( bool enabled )               { BOOL_FLAG( m_pInterface->m_entityFlags, ENTITY_BACKFACECULL, enabled ); }

    void                        SetAlpha( unsigned char alpha );

    bool                        IsFading( void ) const                          { return IS_FLAG( m_pInterface->m_entityFlags, ENTITY_FADE ); }
    float                       GetFadingAlpha( void ) const;

    void                        MatrixConvertFromEulerAngles( float x, float y, float z, int unk );
    void                        MatrixConvertToEulerAngles( float& x, float& y, float& z, int unk ) const;

    void*                       GetStoredPointer() const                        { return m_pStoredPointer; };
    void                        SetStoredPointer( void *pointer )               { m_pStoredPointer = pointer; };

    bool                        IsStaticWaitingForCollision() const             { return IS_FLAG( m_pInterface->m_entityFlags, ENTITY_WAITFORCOLL ); }
    void                        SetStaticWaitingForCollision( bool enabled )    { BOOL_FLAG( m_pInterface->m_entityFlags, ENTITY_WAITFORCOLL, enabled ); }

    bool                        IsCollidableWith( CEntity *entity ) const;
    void                        SetCollidableWith( CEntity *entity, bool enable );

protected:
    void                        SetInterface( CEntitySAInterface *intf )        { m_pInterface = intf; };

    static unsigned long        FUNC_CClumpModelInfo__GetFrameFromId;

    void*                       m_pStoredPointer;

    typedef std::map <CEntitySAInterface*, bool> disabledColl_t;
    disabledColl_t              m_disabledColl;

    CColModelSA*                m_colModel;

    typedef std::map <std::string, RwTexture*> textureMap_t;
    textureMap_t                m_originalTextures;

public:
    bool                        m_doNotRemoveFromGame; // when deleted, if this is true, it won't be removed from the game

    // Render modes.
    entityRenderModes_t         m_renderModes;
};

#endif
