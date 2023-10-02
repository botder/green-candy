/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CAnimManager.h
*  PURPOSE:     Animation manager interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CAnimManager_H
#define __CAnimManager_H

// Get correct values
#define MAX_ANIM_GROUPS     200
#define MAX_ANIMATIONS      500
#define MAX_ANIM_BLOCKS     294

typedef unsigned long AnimationId;

class CAnimBlendAssocGroup;
class CAnimBlendHierarchy;
class CAnimBlock;
class CAnimBlendAssociation;
struct RpClump;
struct RwStream;
struct AnimAssocDefinition;
struct AnimDescriptor;

class CAnimManager
{
    friend class CAnimBlendAssociation;

public:
    virtual void                        Initialize() = 0;
    virtual void                        Shutdown() = 0;

    virtual unsigned int                GetNumAnimations() const = 0;
    virtual unsigned int                GetNumAnimBlocks() const = 0;
    virtual unsigned int                GetNumAnimAssocDefinitions() const = 0;

    virtual CAnimBlendHierarchy*        GetAnimation( int ID ) = 0;
    virtual CAnimBlendHierarchy*        GetAnimation( const char * szName, CAnimBlock * pBlock ) = 0;
    virtual CAnimBlendHierarchy*        GetAnimation( unsigned int uiIndex, CAnimBlock * pBlock ) = 0;
    
    virtual CAnimBlock*                 GetAnimationBlock( int ID ) = 0;
    virtual CAnimBlock*                 GetAnimationBlock( const char * szName ) = 0;
    virtual int                         GetAnimationBlockIndex( const char * szName ) = 0;
        
    virtual CAnimBlendAssocGroup*       GetAnimBlendAssoc( AssocGroupId groupID ) = 0;
    virtual unsigned int                GetFirstAssocGroup( const char * szName ) const = 0;

    virtual const char*                 GetAnimGroupName( AssocGroupId groupID ) = 0;
    virtual const char*                 GetAnimBlockName( AssocGroupId groupID ) = 0;

    virtual CAnimBlendAssociation*      CreateAnimAssociation( AssocGroupId animGroup, AnimationId animID ) = 0;
    virtual CAnimBlendAssociation*      GetAnimAssociation( AssocGroupId animGroup, AnimationId animID ) = 0;
    virtual CAnimBlendAssociation*      GetAnimAssociation( AssocGroupId animGroup, const char * szAnimName ) = 0;
    
    virtual void                        AddAnimBlockRef( int ID ) = 0;
    virtual void                        RemoveAnimBlockRef( int ID ) = 0;
    virtual void                        RemoveAnimBlockRefWithoutDelete( int ID ) = 0;
    virtual unsigned int                GetNumRefsToAnimBlock( int ID ) const = 0;
    virtual void                        RemoveAnimBlock( int ID ) = 0;
    
    virtual AnimAssocDefinition*        AddAnimAssocDefinition( const char * szBlockName, const char * szAnimName, AssocGroupId animGroup, AnimationId animID, AnimDescriptor * pDescriptor ) = 0;
    virtual void                        ReadAnimAssociationDefinitions() = 0;
    virtual void                        CreateAnimAssocGroups() = 0;

    virtual void                        UncompressAnimation( CAnimBlendHierarchy * pHierarchy ) = 0;
    virtual void                        RemoveFromUncompressedCache( CAnimBlendHierarchy * pHierarchy ) = 0;

    virtual void                        LoadAnimFile( const char * szFile ) = 0;
    virtual void                        LoadAnimFiles() = 0;
    virtual void                        RemoveLastAnimFile() = 0;
};

#endif
