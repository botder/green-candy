/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientRwFrame.h
*  PURPOSE:     RenderWare frame management
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _CLIENT_RWFRAME_
#define _CLIENT_RWFRAME_

#define LUACLASS_RWFRAME    99

class CClientRwFrame : public CClientRwObject
{
public:
                                    CClientRwFrame( lua_State *L, CRwFrame& frame, CResource *owner );
                                    ~CClientRwFrame();

    CRwFrame&                       GetObject()                         { return m_frame; }
    const CRwFrame&                 GetObject() const                   { return m_frame; }

    void                            SetName( const char *name )         { m_frame.SetName( name ); }
    const char*                     GetName() const                     { return m_frame.GetName(); }
    unsigned int                    GetHash() const                     { return m_frame.GetHash(); }

    void                            MarkGC( lua_State *L );

    static LUA_DECLARE( unlinkParentFrame );
    static LUA_DECLARE( unlinkParentObject );
    static LUA_DECLARE( setChild );

protected:
    CRwFrame&                       m_frame;

public:
    typedef std::vector <CClientRwObject*> objects_t;
    typedef std::vector <CClientRwFrame*> children_t;

    objects_t                       m_objects;
    children_t                      m_children;
};

#endif //_CLIENT_RWFRAME_