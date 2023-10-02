/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientRwObject.h
*  PURPOSE:     RenderWare object virtualization
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _CLIENT_RWOBJECT_
#define _CLIENT_RWOBJECT_

#define LUACLASS_RWOBJECT   97

class CClientRwFrame;

class CClientRwObject abstract : public LuaClass
{
    friend class CClientRwFrame;
    friend class CResourceManager;
    friend class CResource;
public:
                                    CClientRwObject( lua_State *L, CRwObject& object );
                                    ~CClientRwObject();

    CRwObject&                      GetObject()                     { return m_object; }
    const CRwObject&                GetObject() const               { return m_object; }

    void                            SetOwner( CResource *res );
    CResource*                      GetOwner()                      { return m_owner; }

    void                            SetRoot( CClientRwFrame *root ) { m_root = root; }
    CClientRwFrame*                 GetRoot()                       { return m_root; }

	// Enable Garbage Collection on this element
	void							DisableKeepAlive()				{ m_keepAlive = false; }

    // Garbage Collector event
    void							MarkGC( lua_State *L );

protected:
    void                            UnlinkOwner();

    CRwObject&          m_object;

    CResource*          m_owner;
    RwListEntry <CClientRwObject>   m_ownerObjects;

	bool				m_keepAlive;

public:
    CClientRwFrame*     m_parent;
    CClientRwFrame*     m_root;
};

#endif //_CLIENT_RWOBJECT_