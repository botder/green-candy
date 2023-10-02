/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CProjectileInfoSA.cpp
*  PURPOSE:     Projectile type information
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CGameSA * pGame;
/**
 * This destroys all the projectiles in the world
 */
void CProjectileInfoSA::RemoveAllProjectiles (  )
{
    DEBUG_TRACE("void CProjectileInfoSA::RemoveAllProjectiles (  )");
    DWORD dwFunction = FUNC_RemoveAllProjectiles;
    _asm
    {
        call    dwFunction
    }
}

/**
 * Gets a projectile class for a specific projectile ID
 * @param ID valid projectile ID (0-31)
 * @return CProjectile * for the requested projectile, or NULL if the projectile slot is empty or the ID is invalid
 * \todo Check this CProjectile array is how it says in the function
 */
CProjectile* CProjectileInfoSA::GetProjectile( unsigned short id )
{
    DEBUG_TRACE("CProjectile* CProjectileInfoSA::GetProjectile( unsigned short id )");

    if ( id > 31 )
        return NULL;

    return dynamic_cast <CProjectile*> ( pGame->GetPools()->GetObject( (CProjectileSAInterface*)(ARRAY_CProjectile + id * sizeof(void*)) ) );
}

CProjectile* CProjectileInfoSA::GetProjectile( void *projectilePointer )
{
    // This must be destroyed later
    return new CProjectileSA((CProjectileSAInterface*)projectilePointer);
}

CProjectileInfo* CProjectileInfoSA::GetProjectileInfo( void *info )
{
    return m_info[((DWORD)info - ARRAY_CProjectileInfo) / sizeof(CProjectileInfoSAInterface)];
}

static void RemoveProjectile( CProjectileInfoSAInterface *info, CProjectileSAInterface *intf )
{
    switch( info->m_type )
    {
    case WEAPONTYPE_GRENADE:
        pGame->GetExplosionManager()->AddExplosion(
            NULL, pGame->GetPools()->GetEntity( info->m_owner ), EXP_TYPE_GRENADE, intf->Placeable.GetPosition(), 0, true, -1, false );
        break;
    case WEAPONTYPE_MOLOTOV:
        pGame->GetExplosionManager()->AddExplosion(
            NULL, pGame->GetPools()->GetEntity( info->m_owner ), EXP_TYPE_MOLOTOV, intf->Placeable.GetPosition(), 0, true, -1, false );

        pGame->GetAudio()->PushEntityAudio( intf );
        break;
    }

    info->m_active = false;
}

void CProjectileInfoSA::RemoveProjectile ( CProjectileInfo * pProjectileInfo, CProjectile * pProjectile )
{
    DWORD dwFunc = FUNC_RemoveProjectile;
    CProjectileInfoSAInterface * projectileInfoInterface = ((CProjectileInfoSA*)pProjectileInfo)->m_interface;

    CProjectileSA* pProjectileSA = dynamic_cast < CProjectileSA* > ( pProjectile );
    if ( !pProjectileSA ) return;

    CEntitySAInterface * projectileInterface = pProjectileSA->GetInterface();

    // Has it not already been removed by GTA?
    if ( pProjectileInfo->IsActive () )
    {
        _asm
        {
            push    projectileInterface
            push    projectileInfoInterface
            call    dwFunc
            add     esp, 8
        }
    }
}

CProjectileInfo* CProjectileInfoSA::GetNextFreeProjectileInfo()
{
    for ( int i = 0; i < PROJECTILE_INFO_COUNT; i++ )
    {
        if ( m_info[i]->m_interface->m_type == 0 )
            return m_info[i];
    }
    return NULL;
}

CProjectileInfo* CProjectileInfoSA::GetProjectileInfo( unsigned short id )
{
    return m_info[id];
}

/**
 * Creates a projectile based on an entity's position at a specified offset from the entity's position (VC)
 * @param creator The entity that is emmitting the projectile
 * @param eWeapon The type of projectile
 * @param vecOffset How far away from the creator entity the projectile is created
 * @param fForce How fast the projectile is moving when it is created
 * \note The projectile is created at the same angle as the creator entity. If the creator entity is the player
 * and the camera is in standard 3rd person mode, then the camera's rotation is used as the basis for the angle.
 * SA: public: static bool __cdecl CProjectileInfo::AddProjectile(class CEntity *,enum eWeaponType,class CVector,float,class CVector *,class CEntity *)
 */


bool CProjectileInfoSA::AddProjectile( CEntity *creator, eWeaponType eWeapon, const CVector& vecOrigin, float fForce, const CVector& targetPos, CEntity *target )
{
    DEBUG_TRACE("bool CProjectileInfoSA::AddProjectile( CEntity *creator, eWeaponType eWeapon, const CVector& vecOrigin, float fForce, const CVector& target, CEntity *target )");
    
    DWORD dwFunction = FUNC_AddProjectile;
    DWORD dwReturn = 0;
    CEntitySAInterface *creatorSA = NULL;

    if ( creator != NULL )   
    {
        CEntitySA* pCreatorSA = dynamic_cast < CEntitySA* > ( creator );
        if ( pCreatorSA )
        {
            creatorSA = pCreatorSA->GetInterface();
            pGame->GetWorld()->IgnoreEntity( creator );
        }
    }

    CEntitySAInterface *targetSA = NULL;
    
    if ( target != NULL )
    {
        CEntitySA *pTargetEntitySA = dynamic_cast < CEntitySA* > ( target );
        if ( pTargetEntitySA )
            targetSA = pTargetEntitySA->GetInterface ();
    }

    
    _asm
    {
        push    targetSA
        push    targetPos
        push    fForce
        lea     eax,vecOrigin
        push    [eax+8]
        push    [eax+4]
        push    [eax]
        push    eWeapon
        push    creatorSA
        call    dwFunction
        add     esp, 32
        mov     dwReturn, eax
    }

    pGame->GetWorld()->IgnoreEntity( NULL );
    return dwReturn != 0;
}

CEntity* CProjectileInfoSA::GetTarget() const
{
    CEntitySAInterface* pTargetInterface = m_interface->m_target;
    CEntity* pTarget = NULL;

    if ( !pTargetInterface )
        return NULL;

    switch ( pTargetInterface->nType )
    {
    case ENTITY_TYPE_PED:       return pGame->GetPools()->GetPed( pTargetInterface );
    case ENTITY_TYPE_VEHICLE:   return pGame->GetPools()->GetVehicle( pTargetInterface );
    case ENTITY_TYPE_OBJECT:    return pGame->GetPools()->GetObject( pTargetInterface );
    }

    return NULL;
}

void CProjectileInfoSA::SetTarget ( CEntity* pEntity )
{
    CEntitySA* pEntitySA = dynamic_cast < CEntitySA* > ( pEntity );
    if ( pEntitySA )
        m_interface->m_target = pEntitySA->GetInterface();
}

bool CProjectileInfoSA::IsActive() const
{
    return m_interface->m_active;
}
