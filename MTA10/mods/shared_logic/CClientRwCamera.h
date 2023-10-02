/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientRwCamera.h
*  PURPOSE:     RenderWare camera virtualization
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _CLIENT_RW_CAMERA_
#define _CLIENT_RW_CAMERA_

#define LUACLASS_RWCAMERA   102

class CClientRwCamera : public CClientRwObject
{
public:
                                    CClientRwCamera( lua_State *L, CRwCamera& cam );
                                    ~CClientRwCamera();

    void                            SetModel( CClientDFF *clump );

    CClientDFF* m_clump;
    CRwCamera&  m_cam;
};

#endif //_CLIENT_RW_CAMERA_