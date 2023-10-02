/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientEntity.h
*  PURPOSE:     Base entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Chris McArthur <>
*               Jax <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CClientEntity;

#ifndef __CCLIENTENTITY_H
#define __CCLIENTENTITY_H

#include "CElementArray.h"
class LuaFunctionRef;

// Used to check fast version of getElementsByType
//#define CHECK_ENTITIES_FROM_ROOT  MTA_DEBUG

class CClientManager;

#define IS_PED(entity) ((entity)->GetType()==CCLIENTPLAYER||(entity)->GetType()==CCLIENTPED)
#define IS_PLAYER(entity) ((entity)->GetType()==CCLIENTPLAYER)
#define IS_REMOTE_PLAYER(player) (IS_PLAYER(player)&&!(player)->IsLocalPlayer())
#define IS_RADARMARKER(entity) ((entity)->GetType()==CCLIENTRADARMARKER)
#define IS_VEHICLE(entity) ((entity)->GetType()==CCLIENTVEHICLE)
#define IS_OBJECT(entity) ((entity)->GetType()==CCLIENTOBJECT)
#define IS_MARKER(entity) ((entity)->GetType()==CCLIENTMARKER)
#define IS_PICKUP(entity) ((entity)->GetType()==CCLIENTPICKUP)
#define IS_RADAR_AREA(entity) ((entity)->GetType()==CCLIENTRADARAREA)
#define IS_COLSHAPE(entity) ((entity)->GetType()==CCLIENTCOLSHAPE)
#define IS_PROJECTILE(entity) ((entity)->GetType()==CCLIENTPROJECTILE)
#define IS_GUI(entity) ((entity)->GetType()==CCLIENTGUI)
#define CHECK_CGUI(entity,type) (((CClientGUIElement*)entity)->GetCGUIElement()->GetType()==type)

enum eClientEntityType
{
    CCLIENTCAMERA,
    CCLIENTPLAYER,
    CCLIENTPLAYERMODEL____,
    CCLIENTVEHICLE,
    CCLIENTRADARMARKER,
    CCLIENTOBJECT,
    CCLIENTCIVILIAN,
    CCLIENTPICKUP,
    CCLIENTRADARAREA,
    CCLIENTMARKER,
    CCLIENTPATHNODE,
    CCLIENTWORLDMESH,
    CCLIENTTEAM,
    CCLIENTPED,
    CCLIENTPROJECTILE,
    CCLIENTGUI,
    CCLIENTSPAWNPOINT_DEPRECATED,
    CCLIENTCOLSHAPE,
    CCLIENTDUMMY, // anything user-defined
    SCRIPTFILE,
    CCLIENTDFF,
    CCLIENTCOL,
    CCLIENTTXD,
    CCLIENTSOUND,
    CCLIENTWATER,
    CCLIENTDXFONT,
    CCLIENTGUIFONT,
    CCLIENTTEXTURE,
    CCLIENTSHADER,
    CCLIENTUNKNOWN,
};

class CClientColShape;
class CClientPed;
class CCustomData;
class CElementGroup;
class CLuaMain;
class CMapEventManager;
typedef std::list <class CClientEntity*> CChildListType;

#define LUACLASS_ENTITY     40
#define LUACLASS_SYSENTITY  41

class CClientEntity : public LuaElement
{
public:
                                                CClientEntity( ElementID ID, bool system, lua_State *L );
    virtual                                     ~CClientEntity();

    // Static functions for Lua interfacing
    static LUA_DECLARE( entitychildAPI_notifyDestroy );
    static LUA_DECLARE( entity_setChild );

    virtual eClientEntityType                   GetType() const = 0;
    inline bool                                 IsLocalEntity() const                           { return m_ID >= MAX_SERVER_ELEMENTS; };

    // System entity? A system entity means it can't be removed by the server
    // or the client scripts.
    inline bool                                 IsSystemEntity() const                          { return m_bSystemEntity; };

    virtual void                                Unlink() = 0;

    // This is used for realtime synced elements. Whenever a position/rotation change is
    // forced from the server either in form of spawn or setElementPosition/rotation a new
    // value is assigned to this from the server. This value also comes with the sync packets.
    // If this value doesn't match the value from the sync packet, the packet should be
    // ignored. Note that if this value is 0, all sync packets should be accepted. This is
    // so we don't need this byte when the element is created first.
    inline unsigned char                        GetSyncTimeContext()                            { return m_ucSyncTimeContext; };
    inline void                                 SetSyncTimeContext( unsigned char ucContext )   { m_ucSyncTimeContext = ucContext; };
    bool                                        CanUpdateSync( unsigned char ucRemote );

    inline char*                                GetName()                                       { return m_szName; };
    inline void                                 SetName( const char* szName )                   { strncpy( m_szName, szName, MAX_ELEMENT_NAME_LENGTH ); };

    inline const char*                          GetTypeName()                                   { return m_szTypeName; };
    inline unsigned int                         GetTypeHash()                                   { return m_uiTypeHash; };
    void                                        SetTypeName( const char* szName );

    inline CClientEntity*                       GetParent()                                     { return m_pParent; };
    bool                                        SetParent( CClientEntity* pParent );
    void                                        ClearChildren();

    CChildListType ::const_iterator             IterBegin()                                     { return m_Children.begin (); }
    CChildListType ::const_iterator             IterEnd()                                       { return m_Children.end (); }

    inline ElementID                            GetID()                                         { return m_ID; };
    void                                        SetID( ElementID ID );

    void                                        PushCustomData( lua_State *L, const char *key, bool inherit );
    bool                                        GetCustomDataString( const char * szKey, SString& strOut, bool bInheritData );
    bool                                        GetCustomDataFloat( const char * szKey, float& fOut, bool bInheritData );
    bool                                        GetCustomDataInt( const char * szKey, int& iOut, bool bInheritData );
    bool                                        GetCustomDataBool( const char * szKey, bool& bOut, bool bInheritData );
    bool                                        DeleteCustomData( const char* szName, bool bRecursive );

    virtual bool                                GetMatrix( RwMatrix& matrix ) const;
    virtual bool                                SetMatrix( const RwMatrix& matrix );

    virtual void                                GetPosition( CVector& vecPosition ) const = 0;
    void                                        GetPositionRelative( CClientEntity * pOrigin, CVector& vecPosition ) const;
    virtual void                                SetPosition( const CVector& vecPosition ) = 0;
    void                                        SetPositionRelative( CClientEntity * pOrigin, const CVector& vecPosition );
    virtual void                                Teleport( const CVector& vecPosition )          { SetPosition(vecPosition); }

    virtual inline unsigned short               GetDimension()                                  { return m_usDimension; }
    virtual void                                SetDimension( unsigned short usDimension )      { m_usDimension = usDimension; }

    virtual void                                ModelRequestCallback( CModelInfo* pModelInfo )  {};

    virtual bool                                IsOutOfBounds();
    inline CModelInfo*                          GetModelInfo()                                  { return m_pModelInfo; };
    
    typedef std::list <CClientEntity*> attachments_t;

    inline CClientEntity*                       GetAttachedTo()                                 { return m_pAttachedToEntity; }
    virtual void                                AttachTo( CClientEntity * pEntity );
    virtual void                                GetAttachedOffsets( CVector & vecPosition, CVector & vecRotation );
    virtual void                                SetAttachedOffsets( CVector & vecPosition, CVector & vecRotation );
    inline void                                 AddAttachedEntity( CClientEntity* pEntity )     { m_AttachedEntities.push_back ( pEntity ); }
    inline void                                 RemoveAttachedEntity( CClientEntity* pEntity )  { if ( !m_AttachedEntities.empty() ) m_AttachedEntities.remove ( pEntity ); }
    bool                                        IsEntityAttached( CClientEntity* pEntity );
    attachments_t::const_iterator               AttachedEntitiesBegin()                         { return m_AttachedEntities.begin (); }
    attachments_t::const_iterator               AttachedEntitiesEnd()                           { return m_AttachedEntities.end (); }
    void                                        ReattachEntities();
    virtual bool                                IsAttachable();
    virtual bool                                IsAttachToable();
    virtual void                                DoAttaching();

    bool                                        AddEvent( CLuaMain *main, const char *name, const LuaFunctionRef& ref, bool propagated );
    bool                                        CallEvent( const char *name, lua_State *callee, unsigned int argCount, bool childCall = false ); // NOTE: do not use childCall anymore

private:
    void                                        CallEventNoParent( lua_State *callee, unsigned int argCount, const char *name, CClientEntity *source );
    void                                        CallParentEvent( lua_State *callee, unsigned int argCount, const char *name, CClientEntity *source );

public:
    bool                                        DeleteEvent( CLuaMain *main, const char *name = NULL, const LuaFunctionRef *ref = NULL );
    void                                        DeleteEvents( CLuaMain* pLuaMain, bool bRecursive );
    void                                        DeleteAllEvents();

    void                                        CleanUpForVM( CLuaMain* pLuaMain, bool bRecursive );


    CClientEntity*                              FindChild( const char* szName, unsigned int uiIndex, bool bRecursive );
    CClientEntity*                              FindChildIndex( const char* szType, unsigned int uiIndex, unsigned int& uiCurrentIndex, bool bRecursive );
    CClientEntity*                              FindChildByType( const char* szType, unsigned int uiIndex, bool bRecursive );
    CClientEntity*                              FindChildByTypeIndex( unsigned int uiTypeHash, unsigned int uiIndex, unsigned int& uiCurrentIndex, bool bRecursive );
    void                                        FindAllChildrenByType( const char* szType, lua_State* luaVM, bool bStreamedIn = false );
    void                                        FindAllChildrenByTypeIndex( unsigned int uiTypeHash, lua_State* luaVM, unsigned int& uiIndex, bool bStreamedIn = false );

    inline unsigned int                         CountChildren()                                 { return m_Children.size(); };

    void                                        GetChildren( lua_State* luaVM );

    typedef std::list <CClientColShape*> colshapes_t;

    void                                        AddCollision( CClientColShape* pShape )         { m_Collisions.push_back ( pShape ); }
    void                                        RemoveCollision( CClientColShape* pShape )      { m_Collisions.remove ( pShape ); }
    bool                                        CollisionExists( CClientColShape* pShape );
    void                                        RemoveAllCollisions( bool bNotify = false );
    colshapes_t::iterator                       CollisionsBegin()                               { return m_Collisions.begin (); }
    colshapes_t::iterator                       CollisionsEnd()                                 { return m_Collisions.end (); }

    inline CElementGroup*                       GetElementGroup()                               { return m_pElementGroup; }
    inline void                                 SetElementGroup( CElementGroup * elementGroup ){ m_pElementGroup = elementGroup; }

    static unsigned int                         GetTypeID( const char* szTypeName );

    CMapEventManager*                           GetEventManager()                               { return m_pEventManager; };

    // Returns true if this class is inherited by CClientStreamElement
    virtual bool                                IsStreamingCompatibleClass()                    { return false; };

    inline void                                 AddOriginSourceUser( CClientPed * pModel )      { m_OriginSourceUsers.push_back ( pModel ); }
    inline void                                 RemoveOriginSourceUser( CClientPed * pModel )   { m_OriginSourceUsers.remove ( pModel ); }

    inline void                                 AddContact( CClientPed * pModel )               { m_Contacts.push_back ( pModel ); }
    inline void                                 RemoveContact( CClientPed * pModel )            { m_Contacts.remove ( pModel ); }

    virtual CEntity*                            GetGameEntity()                                 { return NULL; }
    virtual const CEntity*                      GetGameEntity() const                           { return NULL; }

    bool                                        IsCollidableWith( CClientEntity *entity ) const;
    void                                        SetCollidableWith( CClientEntity *entity, bool enable );

    bool                                        IsDoubleSided();
    void                                        SetDoubleSided( bool bEnable );

    // Game layer functions for CEntity/CPhysical
    virtual void                                InternalAttachTo( CClientEntity * pEntity );
    bool                                        IsStatic();
    void                                        SetStatic( bool bStatic );
    unsigned char                               GetInterior();
    virtual void                                SetInterior( unsigned char ucInterior );
    bool                                        IsOnScreen();

    // Spatial database
    virtual CSphere                             GetWorldBoundingSphere();
    virtual void                                UpdateSpatialData();

    float                                       GetDistanceBetweenBoundingSpheres( CClientEntity* pOther );

    CIntrusiveListNode < CClientEntity >        m_FromRootNode;     // Our node entry in the 'EntitiesFromRoot' list

protected:
    CClientManager*                             m_pManager;
    CClientEntity*                              m_pParent;
    CChildListType                              m_Children;

    ElementID                                   m_ID;
    CVector                                     m_vecRelativePosition;

    unsigned short                              m_usDimension;

private:
    unsigned int                                m_uiTypeHash;
    char                                        m_szTypeName[MAX_TYPENAME_LENGTH + 1];
    char                                        m_szName[MAX_ELEMENT_NAME_LENGTH + 1];

protected:
    unsigned char                               m_ucSyncTimeContext;

    CClientEntity*                              m_pAttachedToEntity;
    CVector                                     m_vecAttachedPosition;
    CVector                                     m_vecAttachedRotation;
    attachments_t                               m_AttachedEntities;

    bool                                        m_bSystemEntity;
    CMapEventManager*                           m_pEventManager;
    CModelInfo*                                 m_pModelInfo;
    colshapes_t                                 m_Collisions;
    CElementGroup*                              m_pElementGroup;
    std::list < CClientPed * >                  m_OriginSourceUsers;
    std::list < CClientPed * >                  m_Contacts;
    unsigned char                               m_ucInterior;
    bool                                        m_bDoubleSided;
    bool                                        m_bDoubleSidedInit;

    typedef std::vector <CClientEntity*> collisionEntities_t;
    collisionEntities_t                         m_notCollidableWith;

public:
    // Optimization for getElementsByType starting at root
    static void                     StartupEntitiesFromRoot();

private:
    static bool                     IsFromRoot( CClientEntity* pEntity );
    static void                     AddEntityFromRoot( unsigned int uiTypeHash, CClientEntity* pEntity, bool bDebugCheck = true );
    static void                     RemoveEntityFromRoot( unsigned int uiTypeHash, CClientEntity* pEntity );
    static void                     GetEntitiesFromRoot( unsigned int uiTypeHash, lua_State* luaVM, bool bStreamedIn );

#if CHECK_ENTITIES_FROM_ROOT
    static void                     _CheckEntitiesFromRoot      ( unsigned int uiTypeHash );
    void                            _FindAllChildrenByTypeIndex ( unsigned int uiTypeHash, std::map < CClientEntity*, int >& mapResults );
    static void                     _GetEntitiesFromRoot        ( unsigned int uiTypeHash, std::map < CClientEntity*, int >& mapResults );
#endif

};

#endif
