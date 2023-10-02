/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CAnimManagerSA.cpp
*  PURPOSE:     Animation manager
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "gamesa_renderware.h"

using std::list;

struct animInfo
{
    char            name[16];   // 0
    unsigned int    unk1;       // 16
    unsigned int    unk2;       // 20
    const char**    animNames;  // 24
    void*           unk3;       // 28
    char            assoc[16];  // 32
};

static animInfo *const VAR_AnimInfoArray = (animInfo*)0x008AA5B8; 
#define ANIM_INFO_ARRAY_SIZE    119

CAnimManagerSA::CAnimManagerSA()
{
    memset( m_pAnimAssocGroups, 0, sizeof ( m_pAnimAssocGroups ) );
    memset( m_pAnimations, 0, sizeof ( m_pAnimations ) );
    memset( m_pAnimBlocks, 0, sizeof ( m_pAnimBlocks ) );
}

CAnimManagerSA::~CAnimManagerSA()
{
    for ( unsigned int i = 0 ; i < MAX_ANIM_GROUPS ; i++ )
        if ( m_pAnimAssocGroups [ i ] ) delete m_pAnimAssocGroups [ i ];
    for ( unsigned int i = 0 ; i < MAX_ANIMATIONS ; i++ )
        if ( m_pAnimations [ i ] ) delete m_pAnimations [ i ];
    for ( unsigned int i = 0 ; i < MAX_ANIM_BLOCKS ; i++ )
        if ( m_pAnimBlocks [ i ] ) delete m_pAnimBlocks [ i ];
}

void CAnimManagerSA::Initialize ()
{
    DWORD dwFunc = FUNC_CAnimManager_Initialize;
    _asm
    {
        call    dwFunc
    }
}

void CAnimManagerSA::Shutdown()
{
    DWORD dwFunc = FUNC_CAnimManager_Shutdown;
    _asm
    {
        call    dwFunc
    }
}

RwExtension* CAnimManagerSA::CreateAnimation( RpAnimHierarchy *animInfo )
{
    RwExtension *anim;

    if ( !animInfo )
        return NULL;

    anim = pGame->GetRwExtensionManager()->Allocate( RW_EXTENSION_HANIM, 0, 0, 0 );

    if ( !anim )
        return NULL;

    anim->count = anim->count * 2;
    return anim;
}

unsigned int CAnimManagerSA::GetNumAnimations() const
{
    return *(unsigned int*)VAR_CAnimManager_NumAnimations;
}

unsigned int CAnimManagerSA::GetNumAnimBlocks() const
{
    return *(unsigned int*)VAR_CAnimManager_NumAnimBlocks;
}

unsigned int CAnimManagerSA::GetNumAnimAssocDefinitions() const
{
    return *(unsigned int*)VAR_CAnimManager_NumAnimAssocDefinitions;
}

CAnimBlendHierarchy * CAnimManagerSA::GetAnimation ( int ID )
{
    CAnimBlendHierarchySAInterface * pInterface;
    DWORD dwFunc = FUNC_CAnimManager_GetAnimation_int;
    _asm
    {
        push    ID
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x4
    }
    return GetAnimBlendHierarchy ( pInterface );
}


CAnimBlendHierarchy * CAnimManagerSA::GetAnimation ( const char * szName, CAnimBlock * pBlock )
{
    CAnimBlendHierarchySAInterface * pInterface;
    DWORD dwFunc = FUNC_CAnimManager_GetAnimation_str_block;
    CAnimBlockSAInterface * pBlockInterface = pBlock->GetInterface ();
    _asm
    {
        push    pBlockInterface
        push    szName
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x8
    }
    return GetAnimBlendHierarchy ( pInterface );
}


CAnimBlendHierarchy * CAnimManagerSA::GetAnimation ( unsigned int uiIndex, CAnimBlock * pBlock )
{
    CAnimBlendHierarchySAInterface * pInterface;
    DWORD dwFunc = FUNC_CAnimManager_GetAnimation_int_block;
    CAnimBlockSAInterface * pBlockInterface = pBlock->GetInterface ();
    _asm
    {
        push    pBlockInterface
        push    uiIndex
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x8
    }
    return GetAnimBlendHierarchy ( pInterface );
}


CAnimBlock * CAnimManagerSA::GetAnimationBlock ( int ID )
{
    CAnimBlockSAInterface * pInterface;
    DWORD dwFunc = FUNC_CAnimManager_GetAnimationBlock_int;
    _asm
    {
        push    ID
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x4
    }
    return GetAnimBlock ( pInterface );
}


CAnimBlock * CAnimManagerSA::GetAnimationBlock ( const char * szName )
{
    CAnimBlockSAInterface * pInterface;
    DWORD dwFunc = FUNC_CAnimManager_GetAnimationBlock_str;
    _asm
    {
        push    szName
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x4
    }
    return GetAnimBlock ( pInterface );
}


int CAnimManagerSA::GetAnimationBlockIndex( const char *szName )
{
    int iReturn;
    DWORD dwFunc = FUNC_CAnimManager_GetAnimationBlockIndex;
    _asm
    {
        push    szName
        call    dwFunc
        mov     iReturn, eax
        add     esp, 0x4
    }
    return iReturn;
}

static unsigned int *const VAR_NumAnimBlocks = (unsigned int*)VAR_CAnimManager_NumAnimBlocks;

unsigned int CAnimManagerSA::RegisterAnimBlock( const char *name )
{
    CAnimBlockSAInterface *anim = GetAnimBlockByName( name );

    if ( anim )
        return anim->GetIndex();

    unsigned int index = (*VAR_NumAnimBlocks)++;
    CAnimBlockSAInterface& block = *( (CAnimBlockSAInterface*)ARRAY_AnimBlock + index );

    strncpy( block.m_name, name, 16 );
    block.m_count = 0;
    block.m_animInfoIndex = GetFirstAssocGroup( name );
    return index;
}

CAnimBlendAssocGroup * CAnimManagerSA::GetAnimBlendAssoc ( AssocGroupId groupID )
{ 
    CAnimBlendAssocGroupSAInterface * pInterface;
    DWORD dwFunc = FUNC_CAnimManager_GetAnimBlendAssoc;
    _asm
    {
        push    groupID
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x4
    }
    return GetAnimBlendAssocGroup ( pInterface );
}

unsigned int CAnimManagerSA::GetFirstAssocGroup( const char *name ) const
{
    for ( unsigned int n = 0; n < ANIM_INFO_ARRAY_SIZE; n++ )
    {
        if ( stricmp( VAR_AnimInfoArray[n].name, name ) == 0 )
            return n;
    }

    return ANIM_INFO_ARRAY_SIZE - 1;
}

const char * CAnimManagerSA::GetAnimGroupName ( AssocGroupId groupID )
{
    const char * szReturn;
    DWORD dwFunc = FUNC_CAnimManager_GetAnimGroupName;
    _asm
    {
        push    groupID
        call    dwFunc
        mov     szReturn, eax
        add     esp, 0x4
    }
    return szReturn;
}


const char * CAnimManagerSA::GetAnimBlockName ( AssocGroupId groupID )
{
    const char * szReturn;
    DWORD dwFunc = FUNC_CAnimManager_GetAnimBlockName;
    _asm
    {
        push    groupID
        call    dwFunc
        mov     szReturn, eax
        add     esp, 0x4
    }
    return szReturn;
}


CAnimBlendAssociation * CAnimManagerSA::CreateAnimAssociation ( AssocGroupId animGroup, AnimationId animID )
{
    CAnimBlendAssociationSAInterface * pInterface;
    DWORD dwFunc = FUNC_CAnimManager_CreateAnimAssociation;
    _asm
    {
        push    animID
        push    animGroup
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x8
    }
    return GetAnimBlendAssociation ( pInterface );
}


CAnimBlendAssociation * CAnimManagerSA::GetAnimAssociation ( AssocGroupId animGroup, AnimationId animID )
{
    CAnimBlendAssociationSAInterface * pInterface;
    DWORD dwFunc = FUNC_CAnimManager_GetAnimAssociation;
    _asm
    {
        push    animID
        push    animGroup
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x8
    }
    return GetAnimBlendAssociation ( pInterface );
}


CAnimBlendAssociation * CAnimManagerSA::GetAnimAssociation ( AssocGroupId animGroup, const char * szAnimName )
{
    CAnimBlendAssociationSAInterface * pInterface;
    DWORD dwFunc = FUNC_CAnimManager_GetAnimAssociation_str;
    _asm
    {
        push    szAnimName
        push    animGroup
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x8
    }
    return GetAnimBlendAssociation ( pInterface );
}


CAnimBlendAssociation * CAnimManagerSA::AddAnimation ( RpClump * pClump, AssocGroupId animGroup, AnimationId animID )
{
    CAnimBlendAssociationSAInterface * pInterface;
    DWORD dwFunc = FUNC_CAnimManager_AddAnimation;
    _asm
    {
        push    animID
        push    animGroup
        push    pClump
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0xC
    }
    return GetAnimBlendAssociation ( pInterface );
}


CAnimBlendAssociation * CAnimManagerSA::AddAnimation ( RpClump * pClump, CAnimBlendHierarchy * pHierarchy, int ID )
{
    CAnimBlendAssociationSAInterface * pInterface;
    DWORD dwFunc = FUNC_CAnimManager_AddAnimation_hier;
    CAnimBlendHierarchySAInterface * pHierarchyInterface = pHierarchy->GetInterface ();
    _asm
    {
        push    ID
        push    pHierarchyInterface
        push    pClump
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0xC
    }
    return GetAnimBlendAssociation ( pInterface );
}


CAnimBlendAssociation * CAnimManagerSA::AddAnimationAndSync ( RpClump * pClump, CAnimBlendAssociation * pAssociation, AssocGroupId animGroup, AnimationId animID )
{
    CAnimBlendAssociationSAInterface * pInterface;
    DWORD dwFunc = FUNC_CAnimManager_AddAnimationAndSync;
    CAnimBlendAssociationSAInterface * pAssociationInterface = pAssociation->GetInterface ();
    _asm
    {
        push    animID
        push    animGroup
        push    pAssociationInterface
        push    pClump
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x10
    }
    return GetAnimBlendAssociation ( pInterface );
}


CAnimBlendAssociation * CAnimManagerSA::BlendAnimation ( RpClump * pClump, AssocGroupId animGroup, AnimationId animID, float fBlendDelta )
{
    CAnimBlendAssociationSAInterface * pInterface;
    DWORD dwFunc = FUNC_CAnimManager_BlendAnimation;
    _asm
    {
        push    fBlendDelta
        push    animID
        push    animGroup
        push    pClump
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x10
    }
    return GetAnimBlendAssociation ( pInterface );
}

CAnimBlendAssociation* CAnimManagerSA::BlendAnimation( RpClump *clump, CAnimBlendHierarchySAInterface *hier, int id, float delta )
{
    CAnimBlendAssociationSAInterface * pInterface;
    DWORD dwFunc = FUNC_CAnimManager_BlendAnimation_hier;
    _asm
    {
        push    delta
        push    id
        push    hier
        push    clump
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x10
    }
    return GetAnimBlendAssociation ( pInterface );
}

CAnimBlendAssociation * CAnimManagerSA::BlendAnimation ( RpClump * pClump, CAnimBlendHierarchy * pHierarchy, int ID, float fBlendDelta )
{
    return BlendAnimation( pClump, pHierarchy->GetInterface(), ID, fBlendDelta );
}

void CAnimManagerSA::AddAnimBlockRef ( int ID )
{
    DWORD dwFunc = FUNC_CAnimManager_AddAnimBlockRef;
    _asm
    {
        push    ID
        call    dwFunc
        add     esp, 0x4
    }
}


void CAnimManagerSA::RemoveAnimBlockRef ( int ID )
{
    DWORD dwFunc = FUNC_CAnimManager_RemoveAnimBlockRef;
    _asm
    {
        push    ID
        call    dwFunc
        add     esp, 0x4
    }
}


void CAnimManagerSA::RemoveAnimBlockRefWithoutDelete ( int ID )
{
    DWORD dwFunc = FUNC_CAnimManager_RemoveAnimBlockRefWithoutDelete;
    _asm
    {
        push    ID
        call    dwFunc
        add     esp, 0x4
    }
}

unsigned int CAnimManagerSA::GetNumRefsToAnimBlock( int ID ) const
{
    unsigned int iReturn;
    DWORD dwFunc = FUNC_CAnimManager_GetNumRefsToAnimBlock;
    _asm
    {
        push    ID
        call    dwFunc
        mov     iReturn, eax
        add     esp, 0x4
    }
    return iReturn;
}

void CAnimManagerSA::RemoveAnimBlock ( int ID )
{
    DWORD dwFunc = FUNC_CAnimManager_RemoveAnimBlock;
    _asm
    {
        push    ID
        call    dwFunc
        add     esp, 0x4
    }
}

CAnimBlockSAInterface* CAnimManagerSA::GetAnimBlock( unsigned short id )
{
    if ( id > MAX_ANIM_BLOCKS-1 )
        return NULL;

    return (CAnimBlockSAInterface*)ARRAY_AnimBlock + id;
}

CAnimBlockSAInterface* CAnimManagerSA::GetAnimBlockByName( const char *name ) const
{
    for ( unsigned int n = 0; n < GetNumAnimBlocks(); n++ )
    {
        CAnimBlockSAInterface *anim = (CAnimBlockSAInterface*)ARRAY_AnimBlock + n;

        if ( stricmp( anim->m_name, name ) == 0 )
            return anim;
    }

    return NULL;
}

int CAnimManagerSA::GetAnimBlockIndex( const char *name ) const
{
    CAnimBlockSAInterface *anim = GetAnimBlockByName( name );

    if ( !anim )
        return -1;

    return ((int)anim - ARRAY_AnimBlock) / sizeof(anim) + 1;
}

AnimAssocDefinition * CAnimManagerSA::AddAnimAssocDefinition ( const char * szBlockName, const char * szAnimName, AssocGroupId animGroup, AnimationId animID, AnimDescriptor * pDescriptor )
{
    AnimAssocDefinition * pReturn;
    DWORD dwFunc = FUNC_CAnimManager_AddAnimAssocDefinition;
    _asm
    {
        push    pDescriptor
        push    animID
        push    animGroup
        push    szAnimName
        push    szBlockName
        call    dwFunc
        mov     pReturn, eax
        add     esp, 0x14
    }
    return NULL;
}


void CAnimManagerSA::ReadAnimAssociationDefinitions ( void )
{
    DWORD dwFunc = FUNC_CAnimManager_ReadAnimAssociationDefinitions;
    _asm
    {
        call    dwFunc
    }
}


void CAnimManagerSA::CreateAnimAssocGroups ( void )
{
    DWORD dwFunc = FUNC_CAnimManager_CreateAnimAssocGroups;
    _asm
    {
        call    dwFunc
    }
}


void CAnimManagerSA::UncompressAnimation ( CAnimBlendHierarchy * pHierarchy )
{
    DWORD dwFunc = FUNC_CAnimManager_UncompressAnimation;
    CAnimBlendHierarchySAInterface * pHierarchyInterface = pHierarchy->GetInterface ();
    _asm
    {
        push    pHierarchyInterface
        call    dwFunc
        add     esp, 0x4
    }
}


void CAnimManagerSA::RemoveFromUncompressedCache ( CAnimBlendHierarchy * pHierarchy )
{
    DWORD dwFunc = FUNC_CAnimManager_RemoveFromUncompressedCache;
    CAnimBlendHierarchySAInterface * pHierarchyInterface = pHierarchy->GetInterface ();
    _asm
    {
        push    pHierarchyInterface
        call    dwFunc
        add     esp, 0x4
    }
}


void CAnimManagerSA::LoadAnimFile ( const char * szFile )
{
    DWORD dwFunc = FUNC_CAnimManager_LoadAnimFile;
    _asm
    {
        push    szFile
        call    dwFunc
        add     esp, 0x4
    }
}


void CAnimManagerSA::LoadAnimFile ( RwStream * pStream, bool b1, const char * sz1 )
{
    DWORD dwFunc = FUNC_CAnimManager_LoadAnimFile_stream;
    _asm
    {
        push    sz1
        push    b1
        push    pStream
        call    dwFunc
        add     esp, 0xC
    }
}


void CAnimManagerSA::LoadAnimFiles ( void )
{
    DWORD dwFunc = FUNC_CAnimManager_LoadAnimFiles;
    _asm
    {
        call    dwFunc
    }
}


void CAnimManagerSA::RemoveLastAnimFile ( void )
{
    DWORD dwFunc = FUNC_CAnimManager_RemoveLastAnimFile;
    _asm
    {
        call    dwFunc
    }
}


bool CAnimManagerSA::HasAnimGroupLoaded ( AssocGroupId groupID )
{
    bool bReturn;
    DWORD dwFunc = FUNC_HasAnimGroupLoaded;
    _asm
    {
        push    groupID
        call    dwFunc
        mov     bReturn, al
        add     esp, 0x4
    }
    return bReturn;
}

#define ANTIHACK_BLA    0x00B5F878

CAnimBlendAssociationSA* CAnimManagerSA::RpAnimBlendClumpGetFirstAssociation ( RpClump * pClump )
{
    CAnimBlendAssociationSAInterface * pInterface;    
    DWORD dwFunc = FUNC_RpAnimBlendClumpGetFirstAssociation;
    _asm
    {
        push    pClump
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x4
    }
    return GetAnimBlendAssociation ( pInterface );
}

CAnimBlendAssociationSA* CAnimManagerSA::RpAnimBlendClumpGetAssociation ( RpClump * pClump, const char * szAnimName )
{
    CAnimBlendAssociationSAInterface * pInterface;    
    DWORD dwFunc = FUNC_RpAnimBlendClumpGetAssociation_str;
    _asm
    {
        push    szAnimName
        push    pClump
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x8
    }
    return GetAnimBlendAssociation ( pInterface );
}

CAnimBlendAssociationSA* CAnimManagerSA::RpAnimBlendClumpGetAssociation ( RpClump * pClump, AnimationId animID )
{
    CAnimBlendAssociationSAInterface * pInterface;    
    DWORD dwFunc = FUNC_RpAnimBlendClumpGetAssociation_int;
    _asm
    {
        push    animID
        push    pClump
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x8
    }
    return GetAnimBlendAssociation ( pInterface );
}

CAnimBlendAssociationSA* CAnimManagerSA::RpAnimBlendGetNextAssociation ( CAnimBlendAssociation * pAssociation )
{
    CAnimBlendAssociationSAInterface * pInterface;
    DWORD dwFunc = FUNC_RpAnimBlendGetNextAssociation;
    CAnimBlendAssociationSAInterface * pAssociationInterface = pAssociation->GetInterface ();
    _asm
    {
        push    pAssociationInterface
        call    dwFunc
        mov     pInterface, eax
        add     esp, 0x4
    }
    return GetAnimBlendAssociation ( pInterface );
}

unsigned int CAnimManagerSA::RpAnimBlendClumpGetNumAssociations( RpClump *pClump ) const
{
    int iReturn;
    DWORD dwFunc = FUNC_RpAnimBlendClumpGetNumAssociations;
    _asm
    {
        push    pClump
        call    dwFunc
        mov     iReturn, eax
        add     esp, 0x4
    }
    return iReturn;
}

void CAnimManagerSA::RpAnimBlendClumpUpdateAnimations ( RpClump * pClump, float f1, bool b1 )
{
    DWORD dwFunc = FUNC_RpAnimBlendClumpUpdateAnimations;
    _asm
    {
        push    b1
        push    f1
        push    pClump
        call    dwFunc
        add     esp, 0xC
    }
}


CAnimBlendAssociationSA* CAnimManagerSA::GetAnimBlendAssociation( CAnimBlendAssociationSAInterface *pInterface )
{
    if ( pInterface )
    {
        std::list <CAnimBlendAssociationSA*> ::const_iterator iter = m_Associations.begin ();
        for ( ; iter != m_Associations.end(); ++iter )
        {
            if ( (*iter)->GetInterface() == pInterface )
            {
                return *iter;
            }
        }
        CAnimBlendAssociationSA* pAssociation = new CAnimBlendAssociationSA ( pInterface );
        m_Associations.push_back ( pAssociation );
        return pAssociation;
    }
    return NULL;
}

CAnimBlendAssocGroupSA* CAnimManagerSA::GetAnimBlendAssocGroup( CAnimBlendAssocGroupSAInterface *pInterface )
{
    if ( pInterface )
    {        
        AssocGroupId groupID = pInterface->groupID;
        if ( !m_pAnimAssocGroups [ groupID ] )
        {
            m_pAnimAssocGroups [ groupID ] = new CAnimBlendAssocGroupSA ( pInterface );
        }
        return m_pAnimAssocGroups [ groupID ];
    }
    return NULL;
}

CAnimBlockSA* CAnimManagerSA::GetAnimBlock( CAnimBlockSAInterface *pInterface )
{
    if ( pInterface )
    {
        unsigned int ID = pInterface->GetIndex ();
        if ( ID < MAX_ANIM_BLOCKS )
        {
            if ( !m_pAnimBlocks [ ID ] )
            {
                m_pAnimBlocks [ ID ] = new CAnimBlockSA ( pInterface );
            }
            return m_pAnimBlocks [ ID ];
        }
    }
    return NULL;
}

CAnimBlendHierarchySA* CAnimManagerSA::GetAnimBlendHierarchy( CAnimBlendHierarchySAInterface *pInterface )
{
    if ( pInterface )
    {
        unsigned int ID = pInterface->GetIndex ();
        if ( ID < MAX_ANIMATIONS )
        {
            if ( !m_pAnimations [ ID ] )
            {
                m_pAnimations [ ID ] = new CAnimBlendHierarchySA ( pInterface );
            }
            return m_pAnimations [ ID ];
        }
    }
    return NULL;
}
