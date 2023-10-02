/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CObjectSA.cpp
*  PURPOSE:     Object entity
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Christian Myhre Lundheim <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "gamesa_renderware.h"

extern CBaseModelInfoSAInterface **ppModelInfo;

void Objects_Init()
{
    ObjectAttributes_Init();
}

void Objects_Shutdown()
{
    ObjectAttributes_Shutdown();
}

CObjectSA::CObjectSA( CObjectSAInterface *obj )
{
    DEBUG_TRACE("CObjectSA::CObjectSA( CObjectSAInterface *obj )");

    m_pInterface = obj;

    // Setup some flags
    m_doNotRemoveFromGame = false;
    obj->m_unk40 = 6;
    BOOL_FLAG( obj->m_entityFlags, ENTITY_DISABLESTREAMING, true );

    m_poolIndex = (*ppObjectPool)->GetIndex( obj );
    mtaObjects[m_poolIndex] = this;

    m_ucAlpha = 255;
    m_hasCustomAttributes = false;

    m_hasScaleVector = false;

    CheckForGangTag ();
}

CObjectSA::~CObjectSA()
{
    DEBUG_TRACE("CObjectSA::~CObjectSA()");

    if ( m_hasCustomAttributes )
        mtaObjectDataPool->Free( GetInterface()->m_dynData );

    mtaObjects[m_poolIndex] = NULL;
}

void* CObjectSA::operator new ( size_t )
{
    return mtaObjectPool->Allocate();
}

void CObjectSA::operator delete ( void *ptr )
{
    return mtaObjectPool->Free( (CObjectSA*)ptr );
}

void CObjectSA::Explode()
{
    DWORD dwFunc = FUNC_CObject_Explode;
    DWORD dwThis = (DWORD)GetInterface();

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}

void CObjectSA::Break()
{
    // Works only if health is 0
    DWORD dwFunc = 0x5A0D90;
    DWORD dwThis = (DWORD)GetInterface ();

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}

void CObjectSA::SetScaleValue( float val )
{
    GetInterface()->fScale = val;

    m_hasScaleVector = false;
}

void CObjectSA::SetScale( const CVector *vec )
{
    if ( !vec )
        m_hasScaleVector = false;
    else
    {
        m_scaleVec = *vec;
        
        m_hasScaleVector = true;
    }
}

const CVector* CObjectSA::GetScale( void ) const
{
    if ( m_hasScaleVector )
        return &m_scaleVec;

    return NULL;
}

float CObjectSA::GetScaleValue( void ) const
{
    return GetInterface()->fScale;
}

void CObjectSA::SetModelIndex( unsigned short ulModel )
{
    m_pInterface->SetModelIndex( ulModel );

    CheckForGangTag();
}

CRpAtomicSA* CObjectSA::CloneAtomic() const
{
    return new CRpAtomicSA( RpAtomicClone( (RpAtomic*)GetInterface()->GetRwObject() ) );
}

void CObjectSA::CheckForGangTag()
{
    switch( GetModelIndex() )
    {
    case 1524: case 1525: case 1526: case 1527:
    case 1528: case 1529: case 1530: case 1531:
        m_bIsAGangTag = true;
        break;
    default:
        m_bIsAGangTag = false; 
        break;
    }
}

void* CObjectSAInterface::operator new( size_t )
{
    return (*ppObjectPool)->Allocate();
}

void CObjectSAInterface::operator delete( void *ptr )
{
    (*ppObjectPool)->Free( (CObjectSAInterface*)ptr );
}