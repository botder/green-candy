/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientPathNode.h
*  PURPOSE:     Path node entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CClientPathNode;

#ifndef __CCLIENTPATHNODE_H
#define __CCLIENTPATHNODE_H

#include "CClientManager.h"

#define LUACLASS_PATHNODE   47

class CClientPathNode : public CClientEntity
{
    friend class CClientPathManager;

public:
    enum ePathNodeStyle
    {
        PATH_STYLE_JUMP = 0,
        PATH_STYLE_SMOOTH
    };
                            CClientPathNode( CClientManager* pManager,
                                             CVector& vecPosition,
                                             CVector& vecRotation,
                                             int iTime,
                                             ElementID ID,
                                             lua_State *L,
                                             ePathNodeStyle PathStyle = PATH_STYLE_JUMP,
                                             CClientPathNode* pNextNode = NULL );
                            ~CClientPathNode();

    void                    Unlink();

    void                    DoPulse();

    eClientEntityType       GetType() const                                     { return CCLIENTPATHNODE; }
    
    inline void             GetPosition( CVector& pos ) const                   { pos = m_vecPosition; }
    inline void             SetPosition( const CVector& pos )                   { m_vecPosition = pos; }

    inline void             GetRotation( CVector& rot ) const                   { rot = m_vecRotation; }
    inline void             SetRotation( const CVector& rot )                   { m_vecRotation = rot; }

    inline int              GetTime() const                                     { return m_iTime; }
    inline ePathNodeStyle   GetStyle() const                                    { return m_Style; }

    inline CClientPathNode* GetPreviousNode()                                   { return m_pPreviousNode; }
    inline void             SetPreviousNode( CClientPathNode* pPathNode )       { m_pPreviousNode = pPathNode; }
    inline CClientPathNode* GetNextNode()                                       { return m_pNextNode; }
    inline void             SetNextNode( CClientPathNode* pPathNode )           { m_pNextNode = pPathNode; }
    inline void             SetNextNodeID( ElementID ID )                       { m_NextNodeID = ID; }
    
    inline void             AttachEntity( CClientEntity* pEntity )              { m_List.push_back ( pEntity ); }
    inline void             DetachEntity( CClientEntity* pEntity )              { if ( !m_List.empty() ) m_List.remove( pEntity ); }
    bool                    IsEntityAttached( CClientEntity* pEntity ) const;

    typedef std::list <CClientEntity*> attached_t;

    inline attached_t::iterator     AttachedIterBegin()                         { return m_List.begin(); }
    inline attached_t::iterator     AttachedIterEnd()                           { return m_List.end(); }
 
private:
    CClientPathManager*         m_pPathManager;

    CVector                     m_vecPosition;
    CVector                     m_vecRotation;
    int                         m_iTime;
    ePathNodeStyle              m_Style;
    ElementID                   m_NextNodeID;
    CClientPathNode             *m_pPreviousNode, *m_pNextNode;

    attached_t                  m_List;
};

#endif