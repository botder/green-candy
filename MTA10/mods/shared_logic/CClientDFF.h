/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientDFF.h
*  PURPOSE:     .dff model handling class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CClientDFF;

#ifndef __CCLIENTDFF_H
#define __CCLIENTDFF_H

#define LUACLASS_DFF    23

class CClientDFF : public CClientRwObject
{
    friend class CClientAtomic;
public:
                                    CClientDFF( lua_State *L, CModel& model, CResource *owner );
                                    ~CClientDFF();

    bool                            ReplaceModel( unsigned short id );
    bool                            HasReplaced( unsigned short id ) const;
    void                            RestoreModel( unsigned short id );
    void                            RestoreModels();

    void                            MarkGC( lua_State *L );

protected:
    void                            RestreamAll() const;

public:
    CModel&                         m_model;

    typedef std::list <CClientAtomic*> atomics_t;
    typedef std::list <CClientLight*> lights_t;
    typedef std::list <CClientRwCamera*> cameras_t;

    atomics_t                       m_atomics;
    lights_t                        m_lights;
    cameras_t                       m_cameras;
};

#endif
