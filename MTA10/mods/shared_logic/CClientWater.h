/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientWater.h
*  PURPOSE:     Water polygon class
*  DEVELOPERS:  arc_
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCLIENTWATER_H
#define __CCLIENTWATER_H

#define LUACLASS_WATER  57

class CClientWater : public CClientEntity
{
public:
                                CClientWater( CClientManager *pManager, ElementID ID, lua_State *L, bool system, CVector& vecBL, CVector& vecBR, CVector& vecTL, CVector& vecTR, bool bShallow = false );
                                CClientWater( CClientManager *pManager, ElementID ID, lua_State *L, bool system, CVector& vecL, CVector& vecR, CVector& vecTB, bool bShallow = false );
                                ~CClientWater();

    bool                        Valid() const                                    { return m_pPoly != NULL; }

    eClientEntityType           GetType() const                                  { return CCLIENTWATER; }
    size_t                      GetNumVertices() const;
    void                        GetPosition( CVector& vecPosition ) const;
    bool                        GetVertexPosition( unsigned int idx, CVector& vecPosition ) const;
    void                        SetPosition( const CVector& pos );
    bool                        SetVertexPosition( unsigned int idx, const CVector& pos, void *pChangeSource = NULL );
    void                        Unlink();

private:
    void                        InstanceLua( bool system );

    CWaterPoly*                 m_pPoly;
    CClientWaterManager*        m_pWaterManager;

    friend class CClientWaterManager;
};

#endif