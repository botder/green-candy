/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CExplosionManagerSA.cpp
*  PURPOSE:     Explosion manager
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CExplosionManagerSA::CExplosionManagerSA()
{
    DEBUG_TRACE("CExplosionManagerSA::CExplosionManagerSA()");

    for( unsigned int i = 0; i < MAX_EXPLOSIONS; i++ )
        m_explosions[i] = new CExplosionSA( (CExplosionSAInterface*)ARRAY_Explosions + i );
}

CExplosionManagerSA::~CExplosionManagerSA()
{
    for ( unsigned int i = 0; i < MAX_EXPLOSIONS; i++ )
        delete m_explosions[i];
}

/**
 * \todo Test this, replace with CExplosion::AddExplosion code if possible in order to ensure correct pointer
 */
CExplosion* CExplosionManagerSA::AddExplosion( CEntity *entity, CEntity *owner, eExplosionType eType, const CVector& pos, unsigned int actDelay, bool makeSound, float camShake, bool noDamage )
{
    DEBUG_TRACE("CExplosion* CExplosionManagerSA::AddExplosion( CEntity *entity, CEntity *owner, eExplosionType eType, const CVector& pos, unsigned int actDelay, bool makeSound, float camShake, bool noDamage )");

    CEntitySAInterface *entInt = entity ? dynamic_cast <CEntitySA*> ( entity )->GetInterface() : NULL;
    CEntitySAInterface *ownInt = owner ? dynamic_cast <CEntitySA*> ( owner )->GetInterface() : NULL;

    float fX = pos.fX, fY = pos.fY, fZ = pos.fZ;        
    CExplosion *explosion = CExplosionManagerSA::FindFreeExplosion();
    bool bReturn;
    DWORD dwFunc = FUNC_CExplosion_AddExplosion;
    _asm
    {
        push    noDamage
        push    camShake
        push    makeSound
        push    actDelay
        push    fZ
        push    fY
        push    fX
        push    eType
        push    ownInt
        push    entInt

        // OUR CALL
        push    returnhere // simulate a call, by pusing our return address
        // NOW the code is actually inside CExplosion__AddExplosion, but may be patched by Multiplayer
        sub     esp, 0x1C
        push    ebx
        push    ebp
        push    esi
        mov     ebx, dwFunc // Now jump in 6 bytes later (6 bytes might be used for our patch-jump in Multiplayer)
        add     ebx, 6
        jmp     ebx
        
returnhere:
        add     esp, 0x28
        mov     bReturn, al
    }

    return bReturn ? explosion : NULL;
}

/**
 * \todo Need to simulate this manually (loop and IsNear...)
 */
void CExplosionManagerSA::RemoveAllExplosionsInArea( const CVector& pos, float radius )
{
    DEBUG_TRACE("void CExplosionManagerSA::RemoveAllExplosionsInArea( const CVector& pos, float radius )");
    
/*  DWORD dwFunction = FUNC_RemoveAllExplosionsInArea;
    CVector * vecPos = (CVector *)vecPosition;
    FLOAT fX = vecPos->fX;
    FLOAT fY = vecPos->fY;
    FLOAT fZ = vecPos->fZ;
    
    _asm
    {
        push    fRadius
        push    fZ
        push    fY
        push    fX
        call    dwFunction
        add     esp, 0x10
    }*/
}

void CExplosionManagerSA::RemoveAllExplosions()
{
    DEBUG_TRACE("void CExplosionManagerSA::RemoveAllExplosions()");

    for( unsigned int i = 0; i < MAX_EXPLOSIONS; i++ )
    {
        if( m_explosions[i]->IsActive() )
            m_explosions[i]->Remove();
    }
}

CExplosion* CExplosionManagerSA::GetExplosion( unsigned int id )
{
    DEBUG_TRACE("CExplosion* CExplosionManagerSA::GetExplosion( unsigned int id )");

    if ( id > MAX_EXPLOSIONS-1 )
        return NULL;

    return m_explosions[id];
}

CExplosion* CExplosionManagerSA::FindFreeExplosion()
{
    DEBUG_TRACE("CExplosion* CExplosionManagerSA::FindFreeExplosion()");

    for( unsigned int i = 0; i < MAX_EXPLOSIONS; i++ )
    {
        if( !m_explosions[i]->IsActive() )
            return m_explosions[i];
    }

    return NULL;
}
