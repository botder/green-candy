/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/gamesa_renderware.h
*  PURPOSE:     RenderWare interface mappings to Grand Theft Auto: San Andreas
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*  RenderWare is © Criterion Software
*
*****************************************************************************/

#ifndef __GAMESA_RENDERWARE
#define __GAMESA_RENDERWARE

#include "RenderWare.h"

class CColModelSAInterface;

/*****************************************************************************/
/** Renderware functions                                                    **/
/*****************************************************************************/

/* RenderWare function defines */
typedef RwError*                (__cdecl *RwErrorGet_t)                         (RwError *code);

// Utility functions
typedef void                    (__cdecl *RwDeviceSystemRequest_t)              (RwRenderSystem& rend, int objectID, unsigned int& result, int, int);
typedef void                    (__cdecl *RwPrefetch_t)                         ();
typedef void                    (__cdecl *RwFlushLoader_t)                      ();
typedef unsigned int            (__cdecl *RwPluginRegistryReadDataChunks_t)     (void *unk, RwStream *stream, void *obj);
typedef void*                   (__cdecl *RwAllocAligned_t)                     (size_t size, unsigned int align);
typedef void                    (__cdecl *RwFreeAligned_t)                      (void *ptr);
typedef RwExtension*            (__cdecl *RwCreateExtension_t)                  (unsigned int id, unsigned int count, size_t size, int unk3);

typedef void*                   (__cdecl *RwIm3DTransform_t)                    (RwVertex *pVerts, unsigned int numVerts, RwMatrix *ltm, unsigned int flags);
typedef int                     (__cdecl *RwIm3DRenderIndexedPrimitive_t)       (RwPrimitiveType primType, unsigned short *indices, int numIndices);
typedef int                     (__cdecl *RwIm3DEnd_t)                          ();

// Vector functions
typedef float                   (__cdecl *RwV3dNormalize_t)                     (RwV3d *out, const RwV3d *in);
typedef RwV3d*                  (__cdecl *RwV3dTransformVector_t)               (RwV3d *out, const RwV3d *in, const RwMatrix *matrix);

// Matrix functions
typedef RwMatrix*               (__cdecl *RwMatrixCreate_t)                     ();
typedef RwMatrix*               (__cdecl *RwMatrixInvert_t)                     (RwMatrix *dst, const RwMatrix *src);
typedef RwMatrix*               (__cdecl *RwMatrixTranslate_t)                  (RwMatrix *matrix, const RwV3d *translation, RwTransformOrder order);
typedef RwMatrix*               (__cdecl *RwMatrixScale_t)                      (RwMatrix *matrix, const RwV3d *translation, RwTransformOrder order);
typedef float                   (__cdecl *RwMatrixUnknown_t)                    (const RwMatrix& matrix, const RwMatrix& matrix2, unsigned short flags);

// Object functions
typedef void                    (__cdecl *RwObjectRegister_t)                   (void *group, RwObject *obj);

// Stream functions
typedef RwStream*               (__cdecl *RwStreamReadChunkHeaderInfo_t)        (RwStream *stream, RwChunkHeader& header);
typedef int                     (__cdecl *RwStreamFindChunk_t)                  (RwStream *stream, unsigned int type, unsigned int *lengthOut, unsigned int *versionOut);

// Frame functions
typedef RwFrame*                (__cdecl *RwFrameCreate_t)                      ();
typedef RwFrame*                (__cdecl *RwFrameCloneRecursive_t)              (const RwFrame *frame, const RwFrame *root);
typedef const RwMatrix*         (__cdecl *RwFrameGetLTM_t)                      (RwFrame *frame);
typedef RwFrame*                (__cdecl *RwFrameTranslate_t)                   (RwFrame *frame, const RwV3d *v, RwTransformOrder order);
typedef RwFrame*                (__cdecl *RwFrameScale_t)                       (RwFrame *frame, const RwV3d *v, RwTransformOrder order);
typedef void                    (__cdecl *RwFrameOrient_t)                      (RwFrame *frame, float unk, float unk2, CVector& unk3);
typedef RwFrame*                (__cdecl *RwFrameSetIdentity_t)                 (RwFrame *frame);
typedef void                    (__cdecl *RwFrameCloneHierarchy_t)              (RwFrame *frame);
typedef void                    (__cdecl *RwFrameDestroy_t)                     (RwFrame *frame);

// Material functions
typedef RpMaterial*             (__cdecl *RpMaterialCreate_t)                   ();
typedef int                     (__cdecl *RpMaterialDestroy_t)                  (RpMaterial *mat);

// Geometry functions
typedef RpGeometry*             (__cdecl *RpGeometryCreate_t)                   (int numverts, int numtriangles, unsigned int format);
typedef void                    (__cdecl *RpGeometryAddRef_t)                   (RpGeometry *geom);
typedef RpSkeleton*             (__cdecl *RpGeometryGetSkeleton_t)              (RpGeometry *geom);
typedef const RpGeometry*       (__cdecl *RpGeometryTriangleSetVertexIndices_t) (const RpGeometry *geo, RpTriangle *tri, unsigned short v1, unsigned short v2, unsigned short v3);
typedef RpGeometry*             (__cdecl *RpGeometryUnlock_t)                   (RpGeometry *geo);
typedef RpGeometry*             (__cdecl *RpGeometryLock_t)                     (RpGeometry *geo, int lockmode);
typedef RpGeometry*             (__cdecl *RpGeometryTransform_t)                (RpGeometry *geo, const RwMatrix *matrix);
typedef RpGeometry*             (__cdecl *RpGeometryTriangleSetMaterial_t)      (RpGeometry *geo, RpTriangle *tri, RpMaterial *mat);
typedef RpGeometry*             (__cdecl *RpGeometryStreamRead_t)               (RpGeometry *geom);
typedef int                     (__cdecl *RpGeometryDestroy_t)                  (RpGeometry *geo);

// Atomic functions
typedef RpAtomic*               (__cdecl *RpAtomicCreate_t)                     ();
typedef RpAtomic*               (__cdecl *RpAtomicClone_t)                      (const RpAtomic *atomic);
typedef RpAtomic*               (__cdecl *RpAtomicSetGeometry_t)                (RpAtomic *atomic, RpGeometry *geometry, unsigned int flags);
typedef const RwSphere&         (__cdecl *RpAtomicGetWorldBoundingSphere_t)     (RpAtomic *atomic);
typedef void                    (__cdecl *RpAtomicSetupObjectPipeline_t)        (RpAtomic *atomic);
typedef void                    (__cdecl *RpAtomicSetupVehiclePipeline_t)       (RpAtomic *atomic);
typedef RpAtomic*               (__cdecl *RpAtomicRender_t)                     (RpAtomic *atomic);
typedef int                     (__cdecl *RpAtomicDestroy_t)                    (RpAtomic *atomic);

// Light functions
typedef RpLight*                (__cdecl *RpLightSetRadius_t)                   (RpLight *light, float radius);
typedef float                   (__cdecl *RpLightGetConeAngle_t)                (const RpLight *light);
typedef void                    (__cdecl *RpLightDestroy_t)                     (RpLight *light);

// Camera functions
typedef RwCamera*               (__cdecl *RwCameraClone_t)                      (RwCamera *camera);
typedef void                    (__cdecl *RwCameraDestroy_t)                    (RwCamera *camera);

// Clump functions
typedef RpClump*                (__cdecl *RpClumpClone_t)                       (const RpClump *clone);
typedef void                    (__cdecl *RpClumpGetBoneTransform_t)            (RpClump *clump, CVector *offsets);
typedef void                    (__cdecl *RpClumpSetupFrameCallback_t)          (RpClump *clump, unsigned int hierarchyId);
typedef RpClump*                (__cdecl *RpClumpStreamRead_t)                  (RwStream *stream);
typedef int                     (__cdecl *RpClumpDestroy_t)                     (RpClump *clump);

// Raster functions
typedef RwRaster*               (__cdecl *RwRasterCreate_t)                     (int width, int height, int depth, RwRasterType type);
typedef RwRaster*               (__cdecl *RwRasterUnlock_t)                     (RwRaster *raster);
typedef RwRaster*               (__cdecl *RwRasterLock_t)                       (RwRaster *raster, unsigned char level, int lockmode);
typedef void                    (__cdecl *RwRasterDestroy_t)                    (RwRaster *raster);

// Texture functions
typedef RwTexture*              (__cdecl *RwTextureCreate_t)                    (RwRaster *raster);
typedef int                     (__cdecl *RwTextureDestroy_t)                   (RwTexture *texture);

// TexDictionary functions
typedef RwTexDictionary*        (__cdecl *RwTexDictionaryStreamRead_t)          (RwStream *stream);
typedef RwTexture*              (__cdecl *RwTexDictionaryAddTexture_t)          (RwTexDictionary *dict, RwTexture *texture);
typedef int                     (__cdecl *RwTexDictionaryDestroy_t)             (RwTexDictionary *txd);

// Scene functions
typedef RwScene*                (__cdecl *RwSceneRender_t)                      (RwScene *scene);
typedef RwScene*                (__cdecl *RwSceneAddAtomic_t)                   (RwScene *scene, RpAtomic *atomic);
typedef RwScene*                (__cdecl *RwSceneAddClump_t)                    (RwScene *scene, RpClump *clump);
typedef RwScene*                (__cdecl *RwSceneAddLight_t)                    (RwScene *scene, RpLight *light);
typedef void                    (__cdecl *RwSceneRemoveLight_t)                 (RwScene *scene, RpLight *light);
typedef void                    (__cdecl *RwSceneDestroy_t)                     (RwScene *scene);

// Dict functions
typedef RtDict*                 (__cdecl *RtDictSchemaStreamReadDict_t)         (RtDictSchema *schema, RwStream *stream);
typedef void                    (__cdecl *RtDictDestroy_t)                      (RtDict *dict);

// Animation functions
typedef bool                    (__cdecl *RwAnimationInit_t)                    (RpAnimation *anim, RwExtension *ext);
typedef bool                    (__cdecl *RwSkeletonUpdate_t)                   (RpSkeleton *skel);

// Memory functions
typedef void*                   (__cdecl *RwMalloc_t)                           (size_t memSize, unsigned int flags);
typedef void*                   (__cdecl *RwRealloc_t)                          (void *memptr, size_t newSize);
typedef void                    (__cdecl *RwFree_t)                             (void *memptr);
typedef void                    (__cdecl *RwCalloc_t)                           (unsigned int count, unsigned int blockSize);

/*****************************************************************************/
/** Renderware function mappings                                            **/
/*****************************************************************************/

// Utility functions
extern RwDeviceSystemRequest_t                  RwDeviceSystemRequest;
extern RwErrorGet_t                             RwErrorGet;
extern RwAllocAligned_t                         RwAllocAligned;
extern RwFreeAligned_t                          RwFreeAligned;
extern RwCreateExtension_t                      RwCreateExtension;
extern RwPrefetch_t                             RwPrefetch;
extern RwFlushLoader_t                          RwFlushLoader;
extern RwPluginRegistryReadDataChunks_t         RwPluginRegistryReadDataChunks;

extern RwIm3DTransform_t                        RwIm3DTransform;
extern RwIm3DRenderIndexedPrimitive_t           RwIm3DRenderIndexedPrimitive;
extern RwIm3DEnd_t                              RwIm3DEnd;

// Vector functions
extern RwV3dNormalize_t                         RwV3dNormalize;
extern RwV3dTransformVector_t                   RwV3dTransformVector;

// Matrix functions
extern RwMatrixCreate_t                         RwMatrixCreate;
extern RwMatrixInvert_t                         RwMatrixInvert;
extern RwMatrixTranslate_t                      RwMatrixTranslate;
extern RwMatrixScale_t                          RwMatrixScale;
extern RwMatrixUnknown_t                        RwMatrixUnknown;

// Object functions
extern RwObjectRegister_t                       RwObjectRegister;

// Stream functions
extern RwStreamFindChunk_t                      RwStreamFindChunk;
extern RwStreamReadChunkHeaderInfo_t            RwStreamReadChunkHeaderInfo;

// Frame functions
extern RwFrameCreate_t                          RwFrameCreate;
extern RwFrameCloneRecursive_t                  RwFrameCloneRecursive;
extern RwFrameCloneHierarchy_t                  RwFrameCloneHierarchy;
extern RwFrameGetLTM_t                          RwFrameGetLTM;
extern RwFrameSetIdentity_t                     RwFrameSetIdentity;
extern RwFrameTranslate_t                       RwFrameTranslate;
extern RwFrameScale_t                           RwFrameScale;
extern RwFrameOrient_t                          RwFrameOrient;
extern RwFrameDestroy_t                         RwFrameDestroy;
RW_PLUGIN_INTERFACE_EXTERN( RwFrame );

// Material functions
extern RpMaterialCreate_t                       RpMaterialCreate;
extern RpMaterialDestroy_t                      RpMaterialDestroy;
RW_PLUGIN_INTERFACE_EXTERN( RpMaterial );

// Geometry functions
extern RpGeometryCreate_t                       RpGeometryCreate;
extern RpGeometryAddRef_t                       RpGeomtryAddRef;
extern RpGeometryGetSkeleton_t                  RpGeometryGetSkeleton;
extern RpGeometryTriangleSetVertexIndices_t     RpGeometryTriangleSetVertexIndices;
extern RpGeometryTriangleSetMaterial_t          RpGeometryTriangleSetMaterial;
extern RpGeometryUnlock_t                       RpGeometryUnlock;
extern RpGeometryLock_t                         RpGeometryLock;
extern RpGeometryTransform_t                    RpGeometryTransform;
extern RpGeometryRegisterPlugin_t               RpGeometryRegisterPlugin;
extern RpGeometryDestroy_t                      RpGeometryDestroy;
RW_PLUGIN_INTERFACE_EXTERN( RpGeometry );

// Atomic functions
extern RpAtomicCreate_t                         RpAtomicCreate;
extern RpAtomicClone_t                          RpAtomicClone;
extern RpAtomicSetGeometry_t                    RpAtomicSetGeometry;
extern RpAtomicGetWorldBoundingSphere_t         RpAtomicGetWorldBoundingSphere;
extern RpAtomicSetupObjectPipeline_t            RpAtomicSetupObjectPipeline;
extern RpAtomicSetupVehiclePipeline_t           RpAtomicSetupVehiclePipeline;
extern RpAtomicRender_t                         RpAtomicRender;
extern RpAtomicDestroy_t                        RpAtomicDestroy;
RW_PLUGIN_INTERFACE_EXTERN( RpAtomic );

// Light functions
extern RpLightSetRadius_t                       RpLightSetRadius;
extern RpLightGetConeAngle_t                    RpLightGetConeAngle;
extern RpLightDestroy_t                         RpLightDestroy;
RW_PLUGIN_INTERFACE_EXTERN( RpLight );

// Camera functions
extern RwCameraClone_t                          RwCameraClone;
extern RwCameraDestroy_t                        RwCameraDestroy;
RW_PLUGIN_INTERFACE_EXTERN( RwCamera );

// Clump functions
extern RpClumpClone_t                           RpClumpClone;
extern RpClumpGetBoneTransform_t                RpClumpGetBoneTransform;
extern RpClumpSetupFrameCallback_t              RpClumpSetupFrameCallback;
extern RpClumpStreamRead_t                      RpClumpStreamRead;
extern RpClumpDestroy_t                         RpClumpDestroy;
RW_PLUGIN_INTERFACE_EXTERN( RpClump );

// Raster functions
extern RwRasterCreate_t                         RwRasterCreate;
extern RwRasterUnlock_t                         RwRasterUnlock;
extern RwRasterLock_t                           RwRasterLock;
extern RwRasterDestroy_t                        RwRasterDestroy;
RW_PLUGIN_INTERFACE_EXTERN( RwRaster );

// Texture functions
extern RwTextureCreate_t                        RwTextureCreate;
extern RwTextureDestroy_t                       RwTextureDestroy;

// TexDictionary functions
extern RwTexDictionaryStreamRead_t              RwTexDictionaryStreamRead;
extern RwTexDictionaryDestroy_t                 RwTexDictionaryDestroy;

// Scene functions
extern RwSceneRender_t                          RwSceneRender;
extern RwSceneAddAtomic_t                       RwSceneAddAtomic;
extern RwSceneAddLight_t                        RwSceneAddLight;
extern RwSceneAddClump_t                        RwSceneAddClump;
extern RwSceneRemoveLight_t                     RwSceneRemoveLight;
extern RwSceneDestroy_t                         RwSceneDestroy;
RW_PLUGIN_INTERFACE_EXTERN( RwScene );

// Dict functions
extern RtDictSchemaStreamReadDict_t             RtDictSchemaStreamReadDict;
extern RtDictDestroy_t                          RtDictDestroy;

// Animation functions
extern RwAnimationInit_t                        RwAnimationInit;
extern RwSkeletonUpdate_t                       RwSkeletonUpdate;

// Memory functions
extern RwMalloc_t                               RwMalloc;
extern RwRealloc_t                              RwRealloc;
extern RwFree_t                                 RwFree;
extern RwCalloc_t                               RwCalloc;

/*****************************************************************************/
/** GTA function definitions                                                **/
/*****************************************************************************/

typedef RwTexture*          (__cdecl *CClothesBuilder_CopyTexture_t)    (RwTexture *texture);

extern CClothesBuilder_CopyTexture_t    CClothesBuilder_CopyTexture;

#endif //__GAMESA_RENDERWARE