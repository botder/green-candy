/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CAnimBlockSA.h
*  PURPOSE:     Header file for animation block class
*  DEVELOPERS:  Jax <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CAnimBlockSA_H
#define __CAnimBlockSA_H

#include <game/CAnimBlock.h>
#include "Common.h"

class CAnimBlendAssocGroupSA;

#define ARRAY_AnimBlock             0x00B5D4A0

class CAnimBlockSAInterface // 32 bytes
{
public:
    char                m_name[16];         // 0
    bool                m_loaded;           // 16
    BYTE                m_pad[1];           // 17
    unsigned short      m_references;       // 18
    unsigned int        m_animationIndex;   // 20
    unsigned int        m_count;            // 24
    unsigned int        m_animInfoIndex;    // 28

    void                                Reference           ( void )                    { m_references++; }
    void                                Dereference         ( void )                    { m_references--; }
    unsigned short                      GetRefCount         ( void )                    { return m_references; }

    bool                                IsLoaded            ( void ) const              { return m_loaded; }
    unsigned int                        GetAnimIndex        ( void ) const              { return m_animationIndex; }
    unsigned int                        GetCount            ( void ) const              { return m_count; }
    unsigned int                        GetAnimInfoIndex    ( void ) const              { return m_animInfoIndex; }

    int                                 GetIndex            ( void );

    CAnimBlendHierarchySAInterface*     GetAnimation        ( unsigned int hash );
};

class CAnimBlockSA : public CAnimBlock
{
    friend class CAnimBlendAssocGroupSA;
public:
                                        CAnimBlockSA            ( CAnimBlockSAInterface * pInterface )     { m_pInterface = pInterface; }

    CAnimBlockSAInterface*              GetInterface            ()    { return m_pInterface; }
    char*                               GetName                 ()    { return m_pInterface->m_name; }
    int                                 GetIndex                ()    { return m_pInterface->GetIndex (); }
    void                                AddRef                  ()    { m_pInterface->m_references++; }
    unsigned short                      GetRefs                 ()    { return m_pInterface->m_references; }
    bool                                IsLoaded                ()    { return m_pInterface->m_loaded; }

protected:
    CAnimBlockSAInterface *             m_pInterface;
};

#endif
