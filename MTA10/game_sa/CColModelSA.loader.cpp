/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CColModelSA.loader.cpp
*  PURPOSE:     Collision model entity
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include "gamesa_renderware.h"

// Private functions.
_mallocNew_t    _mallocNew =            (_mallocNew_t)invalid_ptr;
_freeMemGame_t  _freeMemGame =          (_freeMemGame_t)invalid_ptr;


CColBoundingBox::CColBoundingBox( void )
{
    this->vecBoundMin = CVector( 1.0f, 1.0f, 1.0f );
    this->vecBoundMax = CVector( -1.0f, -1.0f, -1.0f );
}

// Version 1 structs for backwards compatibility.
struct TBounds
{
    float fRadius;
    CVector center;
    CVector min, max;
};

struct TSphere
{
    float radius;
    CVector center;
    CColSurfaceSA surface;
};

// Float packing.
inline short ColFloatToShort( float val )   { return (short)( val * 128.0f ); }
inline float ColShortToFloat( short val )   { return (float)val / 128.0f; }

struct TFace
{
    unsigned int a, b, c;
    CColSurfaceSA surface;
};

struct ColHeaderVersion1    //size: 40 bytes
{
    TBounds         boundingBox;                // 0
};
C_ASSERT( sizeof( ColHeaderVersion1 ) == 40 );

// The documentation presented at gtamodding is partially wrong for San Andreas.
// 14/01/2015: it has been updated and appears to be correct.
struct ColHeaderVersion2    //size: 76 bytes
{
    CColBoundingBox boundingBox;

    unsigned short  numColSpheres;                      // 40
    unsigned short  numColBoxes;                        // 42
    unsigned short  numColMeshFaces;                    // 44
    unsigned char   numWheels;                          // 46
    unsigned char   pad;                                // 47

    union
    {
        unsigned int        flags;                      // 48

        struct
        {
            unsigned int    unkFlag1 : 1;               // 48
            unsigned int    hasAnythingCollidable : 1;           
            unsigned int    unkFlag3 : 1;
            unsigned int    hasFaceGroups : 1;       
            unsigned int    hasShadowMeshFaces : 1;     // unused flag
        };
    };

    unsigned int    offsetColSpheres;                   // 52
    unsigned int    offsetColBoxes;                     // 56
    unsigned int    offsetSuspensionLines;              // 60
    unsigned int    offsetColMeshVertices;              // 64
    unsigned int    offsetColMeshFaces;                 // 68
    unsigned int    offsetTrianglePlanes;               // 72
};
C_ASSERT( sizeof( ColHeaderVersion2 ) == 76 );

struct ColHeaderVersion3 : public ColHeaderVersion2     //size: 88 bytes
{
    int             numShadowMeshFaces;         // 76

    unsigned int    offsetShadowMeshVertices;   // 80
    unsigned int    offsetShadowMeshFaces;      // 84
};
C_ASSERT( sizeof( ColHeaderVersion3 ) == 88 );

struct ColHeaderVersion4 : public ColHeaderVersion3     //size: 92 bytes
{
    int             unk;                        // 88
};
C_ASSERT( sizeof( ColHeaderVersion4 ) == 92 );

AINLINE unsigned int _CalculateVertexCount( CColTriangleSA *faces, int faceCount )
{
    unsigned int numVertices = 0;

    if ( faceCount != 0 )
    {
        if ( faceCount > 0 )
        {
            for ( int n = 0; n < faceCount; n++ )
            {
                CColTriangleSA *face = faces + n;

                if ( numVertices < face->v1 )
                {
                    numVertices = face->v1;
                }

                if ( numVertices < face->v2 )
                {
                    numVertices = face->v2;
                }

                if ( numVertices < face->v3 )
                {
                    numVertices = face->v3;
                }
            }
        }

        numVertices++;
    }

    return numVertices;
}

unsigned int CColDataSA::CalculateNumberOfShadowMeshVertices( void ) const
{
    // Calculates the number of shadow mesh vertices.
    // The number of shadow mesh vertices is equal to the highest index used in the shadow mesh (+1).
    return _CalculateVertexCount( pShadowMeshFaces, numShadowMeshFaces );
}

unsigned int CColDataSA::CalculateNumberOfCollisionMeshVertices( void ) const
{
    // Does exactly the same as CalculateNumberOfShadowMeshVertices but does it
    // for the collision mesh.
    return _CalculateVertexCount( pColTriangles, numColTriangles );
}

template <typename segmentHeaderType>
struct SegmentAllocator
{
    struct rwAllocatorType
    {
        static void* Allocate( size_t memSize )
        {
            return RwMalloc( memSize, 0 );
        }

        static void Free( void *memPtr )
        {
            RwFree( memPtr );
        }
    };

    typedef MemoryDataStream <rwAllocatorType> DataStream;

    inline void AllocateDataSegment( DataStream& theStream, int segmentSize, segmentHeaderType*& header )
    {
        theStream.SetSize( segmentSize + sizeof( segmentHeaderType ) );

        header = (segmentHeaderType*)theStream.ObtainData( sizeof( segmentHeaderType ) );
    }

    template <typename structType, typename headerType>
    inline structType* GetDataFromSegment( DataStream& stream, unsigned int offset )
    {
        if ( offset != 0 )
        {
            size_t game_offset =
                ( sizeof(headerType) - ( sizeof(segmentHeaderType) - ( sizeof(ColModelFileHeader) - 4 ) ) );

            size_t stream_offset = ( offset - game_offset );

            return (structType*)stream.GetData( stream_offset, sizeof( structType ) );
        }

        return NULL;
    }

    template <typename structType>
    inline structType* ObtainSegmentData( DataStream& stream, int structCount )
    {
        size_t data_size = structCount * sizeof( structType );

        structType *dataBlock = (structType*)( stream.ObtainData( data_size ) );

        return dataBlock;
    }
};

template <typename structType>
inline structType* AllocateDataCount( unsigned int count )
{
    if ( count == 0 )
        return NULL;

    return (structType*)RwMalloc( count * sizeof( structType ), 0 );
}

template <typename structType>
inline void CopyArray( const structType *srcArray, structType *dstArray, size_t count )
{
    if ( srcArray && dstArray )
    {
        for ( size_t n = 0; n < count; n++ )
        {
            dstArray[ n ] = srcArray[ n ];
        }
    }
}

template <typename streamDataType, typename outputType, typename processorType>
inline outputType* ProcessArrayFromStream( const char*& streamPtr, int arrayCount, processorType& proc )
{
    if ( arrayCount > 0 )
    {
        void *mem = RwMalloc( sizeof( outputType ) * arrayCount, 0 );

        for ( int n = 0; n < arrayCount; n++ )
        {
            const streamDataType& streamData = *(const streamDataType*)streamPtr;

            proc.CreateInstance( streamData, (outputType*)mem + n );

            ( (const streamDataType*&)streamPtr )++;
        }

        proc.SetActualCount( arrayCount );

        return (outputType*)mem;
    }
    
    return NULL;
}

struct TSphereProc
{
    inline void CreateInstance( const TSphere& oldSphere, CColSphereSA *newSphere )
    {
        new (newSphere) CColSphereSA
            ( oldSphere.radius, oldSphere.center, oldSphere.surface.material, oldSphere.surface.flags, oldSphere.surface.unknown );
    }

    inline void SetActualCount( int count )
    {
        return;
    }
};

struct TBoxProc
{
    inline void CreateInstance( const CColBoxSA& colBox, CColBoxSA *newBox )
    {
        new (newBox)
            CColBoxSA( colBox.min, colBox.max, colBox.surface.material, colBox.surface.flags, colBox.surface.unknown );
    }

    inline void SetActualCount( int count )
    {
        return;
    }
};

struct TVertexProc
{
    inline void CreateInstance( const CVector& oldVertex, CColVertexSA *newVertex )
    {
        new (newVertex) CColVertexSA(
            ColFloatToShort( oldVertex.fX ),
            ColFloatToShort( oldVertex.fY ),
            ColFloatToShort( oldVertex.fZ )
        );
    }

    inline void SetActualCount( int count )
    {
        return;
    }
};

struct TFaceProc
{
    CColDataSA *colData;
    int actualCount;

    inline TFaceProc( CColDataSA *colData ) : colData( colData )
    {
        actualCount = 0;
    }

    inline void CreateInstance( const TFace& oldFace, CColTriangleSA *newFacePtr )
    {
        CColTriangleSA& newFace = *newFacePtr;
        newFace.v1 = oldFace.a;
        newFace.v2 = oldFace.b;
        newFace.v3 = oldFace.c;
        newFace.material = oldFace.surface.material;
        newFace.lighting = oldFace.surface.light;
    }

    inline void SetActualCount( int count )
    {
        colData->numColTriangles = actualCount;
    }
};

void __cdecl LoadCollisionModel( const char *pBuffer, CColModelSAInterface *pCollision, const char *colName )
{
    const ColHeaderVersion1& colHeader = *(const ColHeaderVersion1*)pBuffer;

    // Copy the bounding box data over.
    pCollision->m_bounds.fRadius = colHeader.boundingBox.fRadius;
    pCollision->m_bounds.vecBoundOffset = colHeader.boundingBox.center;
    pCollision->m_bounds.vecBoundMax = colHeader.boundingBox.max;
    pCollision->m_bounds.vecBoundMin = colHeader.boundingBox.min;

    CColDataSA *colData = new (_mallocNew( sizeof( CColDataSA ) )) CColDataSA;

    pCollision->pColData = colData;

    pBuffer += sizeof( ColHeaderVersion1 );

    short numColSpheres = *(unsigned short*)pBuffer;

    pBuffer += sizeof( int );

    colData->numSpheres = numColSpheres;

    {
        TSphereProc proc;

        colData->pColSpheres = ProcessArrayFromStream <TSphere, CColSphereSA> ( pBuffer, numColSpheres, proc );
    }

    char numWheels = *(char*)pBuffer;

    pBuffer += sizeof( int );

    colData->ucNumWheels = numWheels;

    if ( numWheels > 0 )
    {
        // We do not care about suspension line data of the old version.
        pBuffer += ( sizeof( CVector ) * 2 ) * numWheels;
    }
    else
    {
        colData->pSuspensionLines = NULL;
    }

    // Always come with no suspension line data.
    colData->ucNumWheels = 0;
    colData->pSuspensionLines = NULL;

    short numColBoxes = *(short*)pBuffer;

    pBuffer += sizeof( int );

    colData->numBoxes = numColBoxes;

    {
        TBoxProc proc;

        colData->pColBoxes = ProcessArrayFromStream <CColBoxSA, CColBoxSA> ( pBuffer, numColBoxes, proc );
    }

    int numColVertices = *(int*)pBuffer;

    pBuffer += sizeof( int );

    {
        TVertexProc proc;

        colData->pColVertices = ProcessArrayFromStream <CVector, CColVertexSA> ( pBuffer, numColVertices, proc );
    }

    short numColFaces = *(short*)pBuffer;

    pBuffer += sizeof( int );

    colData->numColTriangles = numColFaces;

    {
        TFaceProc proc( colData );

        colData->pColTriangles = ProcessArrayFromStream <TFace, CColTriangleSA> ( pBuffer, numColFaces, proc );
    }

    // Version 1 has no shadow mesh.
    colData->hasShadowMeshFaces = false;
    colData->pShadowMeshFaces = NULL;
    colData->pShadowMeshVertices = NULL;

    colData->numShadowMeshVertices = 0;

    // Check whether this collision has anything collidable.
    pCollision->m_isCollidable = ( colData->numSpheres != 0 || colData->numBoxes != 0 || colData->numColTriangles != 0 );
}

void __cdecl LoadCollisionModelVer2( const char *pBuffer, int bufferSize, CColModelSAInterface *pCollision, const char *colName )
{
    ColHeaderVersion2 colHeader = *(ColHeaderVersion2*)pBuffer;

    // Set up the collision bounding box.
    pCollision->m_bounds = colHeader.boundingBox;

    pCollision->m_isCollidable = colHeader.hasAnythingCollidable;

    bufferSize -= sizeof( ColHeaderVersion2 );

    if ( bufferSize == 0 )
        return;

    // Allocate the data segment.
    CColDataSA *colData = NULL;

    SegmentAllocator <CColDataSA>::DataStream memStream;

    SegmentAllocator <CColDataSA> segAlloc;

    segAlloc.AllocateDataSegment( memStream, bufferSize, colData );

    void *colBuffer = memStream.GetData( sizeof( CColDataSA ), bufferSize );

    pCollision->pColData = colData;

    pBuffer += sizeof( ColHeaderVersion2 );

    memcpy( colBuffer, pBuffer, bufferSize );

    // Initialize the collision data.
    colData->numSpheres = colHeader.numColSpheres;
    colData->numBoxes = colHeader.numColBoxes;
    colData->ucNumWheels = colHeader.numWheels;
    colData->numColTriangles = colHeader.numColMeshFaces;

    colData->hasSuspensionCones = false;
    colData->hasShadowMeshFaces = false;
    colData->hasFaceGroups = colHeader.hasFaceGroups;

    // Read actual collision data now.
    colData->pColSpheres =          segAlloc.GetDataFromSegment <CColSphereSA, ColHeaderVersion2>           ( memStream, colHeader.offsetColSpheres );
    colData->pColBoxes =            segAlloc.GetDataFromSegment <CColBoxSA, ColHeaderVersion2>              ( memStream, colHeader.offsetColBoxes );
    colData->pSuspensionLines =     segAlloc.GetDataFromSegment <CColSuspensionLineSA, ColHeaderVersion2>   ( memStream, colHeader.offsetSuspensionLines );
    colData->pColVertices =         segAlloc.GetDataFromSegment <CColVertexSA, ColHeaderVersion2>           ( memStream, colHeader.offsetColMeshVertices );
    colData->pColTriangles =        segAlloc.GetDataFromSegment <CColTriangleSA, ColHeaderVersion2>         ( memStream, colHeader.offsetColMeshFaces );

    // Null out data that is not available in version 2
    colData->pColTrianglePlanes = NULL;

    colData->hasShadowMeshFaces = false;
    colData->pShadowMeshVertices = NULL;
    colData->pShadowMeshFaces = NULL;
    
    colData->numShadowMeshFaces = 0;
    colData->numShadowMeshVertices = 0;

    pCollision->m_isColDataSegmented = true;

    // Detah data from the memory stream.
    memStream.DetachData();
}

void __cdecl LoadCollisionModelVer3( const char *pBuffer, int bufferSize, CColModelSAInterface *pCollision, const char *colName )
{
    ColHeaderVersion3 colHeader = *(ColHeaderVersion3*)pBuffer;

    // Set up the collision bounding box.
    pCollision->m_bounds = colHeader.boundingBox;

    pCollision->m_isCollidable = colHeader.hasAnythingCollidable;

    bufferSize -= sizeof( ColHeaderVersion3 );

    if ( bufferSize == 0 )
        return;

    // Allocate the data segment.
    CColDataSA *colData = NULL;

    SegmentAllocator <CColDataSA>::DataStream memStream;

    SegmentAllocator <CColDataSA> segAlloc;

    segAlloc.AllocateDataSegment( memStream, bufferSize, colData );

    void *colBuffer = memStream.GetData( sizeof( CColDataSA ), bufferSize );

    pCollision->pColData = colData;

    pBuffer += sizeof( ColHeaderVersion3 );

    memcpy( colBuffer, pBuffer, bufferSize );

    // Initialize the collision data.
    colData->numSpheres = colHeader.numColSpheres;
    colData->numBoxes = colHeader.numColBoxes;
    colData->ucNumWheels = colHeader.numWheels;
    colData->numColTriangles = colHeader.numColMeshFaces;

    colData->hasSuspensionCones = false;
    colData->hasFaceGroups = colHeader.hasFaceGroups;
    colData->numShadowMeshFaces = colHeader.numShadowMeshFaces;

    // Read actual collision data now.
    colData->pColSpheres =          segAlloc.GetDataFromSegment <CColSphereSA, ColHeaderVersion3>           ( memStream, colHeader.offsetColSpheres );
    colData->pColBoxes =            segAlloc.GetDataFromSegment <CColBoxSA, ColHeaderVersion3>              ( memStream, colHeader.offsetColBoxes );
    colData->pSuspensionLines =     segAlloc.GetDataFromSegment <CColSuspensionLineSA, ColHeaderVersion3>   ( memStream, colHeader.offsetSuspensionLines );
    colData->pColVertices =         segAlloc.GetDataFromSegment <CColVertexSA, ColHeaderVersion3>           ( memStream, colHeader.offsetColMeshVertices );
    colData->pColTriangles =        segAlloc.GetDataFromSegment <CColTriangleSA, ColHeaderVersion3>         ( memStream, colHeader.offsetColMeshFaces );

    colData->pShadowMeshVertices =  segAlloc.GetDataFromSegment <CColVertexSA, ColHeaderVersion3>           ( memStream, colHeader.offsetShadowMeshVertices );
    colData->pShadowMeshFaces =     segAlloc.GetDataFromSegment <CColTriangleSA, ColHeaderVersion3>         ( memStream, colHeader.offsetShadowMeshFaces );

    // Set up the shadow mesh.
    colData->hasShadowMeshFaces =
        ( colHeader.offsetShadowMeshFaces != 0 && colHeader.offsetShadowMeshVertices != 0 && colHeader.numShadowMeshFaces > 0 );

    unsigned int numShadowMeshVertices = 0;

    if ( colData->hasShadowMeshFaces )
        numShadowMeshVertices = colData->CalculateNumberOfShadowMeshVertices();

    colData->numShadowMeshVertices = numShadowMeshVertices;

    // Triangle planes not used in this version.
    colData->pColTrianglePlanes = NULL;

    pCollision->m_isColDataSegmented = true;

    // Detach the memory from the data stream.
    memStream.DetachData();
}

void __cdecl LoadCollisionModelVer4( const char *pBuffer, int bufferSize, CColModelSAInterface *pCollision, const char *colName )
{
    ColHeaderVersion4 colHeader = *(ColHeaderVersion4*)pBuffer;

    // Set up the collision bounding box.
    pCollision->m_bounds = colHeader.boundingBox;

    pCollision->m_isCollidable = colHeader.hasAnythingCollidable;

    bufferSize -= sizeof( ColHeaderVersion4 );

    if ( bufferSize == 0 )
        return;

    // Allocate the data segment.
    CColDataSA *colData = NULL;

    SegmentAllocator <CColDataSA>::DataStream memStream;

    SegmentAllocator <CColDataSA> segAlloc;

    segAlloc.AllocateDataSegment( memStream, bufferSize, colData );

    void *colBuffer = memStream.GetData( sizeof( CColDataSA ), bufferSize );

    pCollision->pColData = colData;

    pBuffer += sizeof( ColHeaderVersion4 );

    memcpy( colBuffer, pBuffer, bufferSize );

    // Initialize the collision data.
    colData->numSpheres = colHeader.numColSpheres;
    colData->numBoxes = colHeader.numColBoxes;
    colData->ucNumWheels = colHeader.numWheels;
    colData->numColTriangles = colHeader.numColMeshFaces;

    colData->hasSuspensionCones = false;
    colData->hasFaceGroups = colHeader.hasFaceGroups;
    colData->numShadowMeshFaces = colHeader.numShadowMeshFaces;

    // Read actual collision data now.
    colData->pColSpheres =          segAlloc.GetDataFromSegment <CColSphereSA, ColHeaderVersion4>           ( memStream, colHeader.offsetColSpheres );
    colData->pColBoxes =            segAlloc.GetDataFromSegment <CColBoxSA, ColHeaderVersion4>              ( memStream, colHeader.offsetColBoxes );
    colData->pSuspensionLines =     segAlloc.GetDataFromSegment <CColSuspensionLineSA, ColHeaderVersion4>   ( memStream, colHeader.offsetSuspensionLines );
    colData->pColVertices =         segAlloc.GetDataFromSegment <CColVertexSA, ColHeaderVersion4>           ( memStream, colHeader.offsetColMeshVertices );
    colData->pColTriangles =        segAlloc.GetDataFromSegment <CColTriangleSA, ColHeaderVersion4>         ( memStream, colHeader.offsetColMeshFaces );

    colData->pShadowMeshVertices =  segAlloc.GetDataFromSegment <CColVertexSA, ColHeaderVersion4>           ( memStream, colHeader.offsetShadowMeshVertices );
    colData->pShadowMeshFaces =     segAlloc.GetDataFromSegment <CColTriangleSA, ColHeaderVersion4>         ( memStream, colHeader.offsetShadowMeshFaces );

    // Set up the shadow mesh.
    colData->hasShadowMeshFaces =
        ( colHeader.offsetShadowMeshFaces != 0 && colHeader.offsetShadowMeshVertices != 0 && colHeader.numShadowMeshFaces > 0 );

    unsigned int numShadowMeshVertices = 0;

    if ( colData->hasShadowMeshFaces )
        numShadowMeshVertices = colData->CalculateNumberOfShadowMeshVertices();

    colData->numShadowMeshVertices = numShadowMeshVertices;

    // Triangle planes not used in this version.
    colData->pColTrianglePlanes = NULL;

    pCollision->m_isColDataSegmented = true;

    // Detach the memory from the data stream.
    memStream.DetachData();
}

// TODO: add generic serialization support to this collision logic.

// MTA extension.
CColModelSAInterface* CColModelSAInterface::Clone( void )
{
    // To clone the interface, we must follow the rules of allocation.
    // We cannot simply copy the fields.
    CColModelSAInterface *clone = new CColModelSAInterface;

    // If allocation failed, we bail.
    if ( !clone )
        return NULL;

    // Copy bounding information.
    clone->m_bounds = this->m_bounds;
    
    // Clone collision data.
    CColDataSA *srcColData = this->pColData;
    CColDataSA *dstColData = NULL;

    if ( srcColData != NULL )
    {
        CColSphereSA            *dstColSphereArray = NULL;
        CColBoxSA               *dstColBoxArray = NULL;
        CColSuspensionDataSA    *dstColSuspensionLineArray = NULL;
        CColVertexSA            *dstColVertexArray = NULL;
        CColTriangleSA          *dstColTriangleArray = NULL;
        CColFaceGroupSA         *dstColFaceGroupArray = NULL;
        CColFaceGroupHeaderSA   *dstColFaceGroupHeader = NULL;
        CColTrianglePlaneSA     *dstColTrianglePlaneArray = NULL;

        CColVertexSA            *dstColShadowMeshVertexArray = NULL;
        CColTriangleSA          *dstColShadowMeshFaceArray = NULL;

        int numSpheres = srcColData->numSpheres;
        int numBoxes = srcColData->numBoxes;
        int numWheels = srcColData->ucNumWheels;
        int numTriangles = srcColData->numColTriangles;
        int numFaceGroups = 0;

        int numVertices = srcColData->CalculateNumberOfCollisionMeshVertices();

        int numShadowMeshVertices = srcColData->numShadowMeshVertices;
        int numShadowMeshFaces = srcColData->numShadowMeshFaces;

        // Face group orriented data.
        CColFaceGroupHeaderSA *srcFaceGroupHeader = srcColData->GetFaceGroupHeader();
        bool hasFaceGroups = srcColData->hasFaceGroups;

        // Suspension line data.
        bool hasSuspensionCones = srcColData->hasSuspensionCones;

        // Allocate depending on allocation method.
        size_t sphereArraySize =                sizeof( CColSphereSA ) * numSpheres;
        size_t boxArraySize =                   sizeof( CColBoxSA ) * numBoxes;
        size_t suspensionLineArraySize =        GetSuspensionLineDataSize( hasSuspensionCones ) * numWheels;
        size_t vertexArraySize =                sizeof( CColVertexSA ) * numVertices;
        size_t triangleArraySize =              sizeof( CColTriangleSA ) * numTriangles;

        size_t shadowMeshVerticeArraySize =     sizeof( CColVertexSA ) * numShadowMeshVertices;
        size_t shadowMeshFaceArraySize =        sizeof( CColTriangleSA ) * numShadowMeshFaces;

        if ( this->m_isColDataSegmented )
        {
            // Calculate the size of all data.
            size_t dataBufferSize = 
                sphereArraySize +
                boxArraySize +
                suspensionLineArraySize +
                vertexArraySize +
                triangleArraySize +
                shadowMeshVerticeArraySize +
                shadowMeshFaceArraySize;

            // Check for face group existance.
            if ( srcFaceGroupHeader )
            {
                numFaceGroups = srcFaceGroupHeader->numFaceGroups;

                dataBufferSize += sizeof( CColFaceGroupSA ) * numFaceGroups;
                dataBufferSize += sizeof( CColFaceGroupHeaderSA );
            }

            SegmentAllocator <CColDataSA> segAlloc;

            SegmentAllocator <CColDataSA>::DataStream memStream;

            segAlloc.AllocateDataSegment( memStream, dataBufferSize, dstColData );

            if ( dstColData == NULL )
                goto cloneFail;

            // Set up the data pointers.
            dstColSphereArray =             segAlloc.ObtainSegmentData <CColSphereSA>           ( memStream, numSpheres );
            dstColBoxArray =                segAlloc.ObtainSegmentData <CColBoxSA>              ( memStream, numBoxes );
            dstColVertexArray =             segAlloc.ObtainSegmentData <CColVertexSA>           ( memStream, numVertices );

            // The suspension lines are complicated to copy, because the struct has a dynamic size.
            CColSuspensionDataSA *actualSuspLines = NULL;

            if ( hasSuspensionCones )
            {
                actualSuspLines =           segAlloc.ObtainSegmentData <CColSuspensionConeSA> ( memStream, numWheels );
            }
            else
            {
                actualSuspLines =           segAlloc.ObtainSegmentData <CColSuspensionLineSA> ( memStream, numWheels );
            }

            dstColSuspensionLineArray = actualSuspLines;

            if ( srcFaceGroupHeader )
            {
                dstColFaceGroupArray =      segAlloc.ObtainSegmentData <CColFaceGroupSA>        ( memStream, numFaceGroups );
                dstColFaceGroupHeader =     segAlloc.ObtainSegmentData <CColFaceGroupHeaderSA>  ( memStream, 1 );
            }
            dstColTriangleArray =           segAlloc.ObtainSegmentData <CColTriangleSA>         ( memStream, numTriangles );

            dstColShadowMeshVertexArray =   segAlloc.ObtainSegmentData <CColVertexSA>           ( memStream, numShadowMeshVertices );
            dstColShadowMeshFaceArray =     segAlloc.ObtainSegmentData <CColTriangleSA>         ( memStream, numShadowMeshFaces );

            // Detach the memory stream.
            memStream.DetachData();
        }
        else
        {
            // We allocate separate buffers for each data.
            dstColData = new (_mallocNew( sizeof( CColDataSA ) )) CColDataSA;

            if ( dstColData == NULL )
                goto cloneFail;

            dstColSphereArray =             AllocateDataCount <CColSphereSA>            ( numSpheres );
            dstColBoxArray =                AllocateDataCount <CColBoxSA>               ( numBoxes );
            dstColVertexArray =             AllocateDataCount <CColVertexSA>            ( numVertices );
            dstColTriangleArray =           AllocateDataCount <CColTriangleSA>          ( numTriangles );

            // Handle suspension lines now.
            CColSuspensionDataSA *actualSuspLines = NULL;

            if ( hasSuspensionCones )
            {
                actualSuspLines =           AllocateDataCount <CColSuspensionConeSA>  ( numWheels );
            }
            else
            {
                actualSuspLines =           AllocateDataCount <CColSuspensionLineSA>  ( numWheels );
            }

            dstColSuspensionLineArray = actualSuspLines;

            dstColShadowMeshVertexArray =   AllocateDataCount <CColVertexSA>            ( numShadowMeshVertices );
            dstColShadowMeshFaceArray =     AllocateDataCount <CColTriangleSA>          ( numShadowMeshFaces );

            // Make sure we have no face groups.
            hasFaceGroups = false;
        }
        clone->m_isColDataSegmented = this->m_isColDataSegmented;

        // Copy over general stuff.
        dstColData->numSpheres = numSpheres;
        dstColData->numBoxes = numBoxes;
        dstColData->ucNumWheels = numWheels;
        dstColData->numColTriangles = numTriangles;

        dstColData->numShadowMeshVertices = numShadowMeshVertices;
        dstColData->numShadowMeshFaces = numShadowMeshFaces;

        dstColData->hasSuspensionCones = srcColData->hasSuspensionCones;
        dstColData->hasFaceGroups = hasFaceGroups;
        dstColData->hasShadowMeshFaces = srcColData->hasShadowMeshFaces;

        // Copy over collision objects.
        CopyArray( srcColData->pColSpheres,             dstColSphereArray, numSpheres );
        CopyArray( srcColData->pColBoxes,               dstColBoxArray, numBoxes );
        CopyArray( srcColData->pColVertices,            dstColVertexArray, numVertices );
        CopyArray( srcColData->pColTriangles,           dstColTriangleArray, numTriangles );

        // The suspension lines are most complicated to copy.
        if ( hasSuspensionCones )
        {
            const CColSuspensionConeSA *srcSuspLines = (const CColSuspensionConeSA*)srcColData->pSuspensionLines;
            CColSuspensionConeSA *dstSuspLines = (CColSuspensionConeSA*)dstColSuspensionLineArray;

            CopyArray( srcSuspLines, dstSuspLines, numWheels );
        }
        else
        {
            const CColSuspensionLineSA *srcSuspLines = (const CColSuspensionLineSA*)srcColData->pSuspensionLines;
            CColSuspensionLineSA *dstSuspLines = (CColSuspensionLineSA*)dstColSuspensionLineArray;

            CopyArray( srcSuspLines, dstSuspLines, numWheels );
        }

        if ( srcFaceGroupHeader )
        {
            CColFaceGroupSA *srcFaceGroupArray = (CColFaceGroupSA*)srcFaceGroupHeader - numFaceGroups;

            CopyArray( srcFaceGroupArray,               dstColFaceGroupArray, numFaceGroups );
            CopyArray( srcFaceGroupHeader,              dstColFaceGroupHeader, 1 );
        }

        CopyArray( srcColData->pShadowMeshVertices,     dstColShadowMeshVertexArray, numShadowMeshVertices );
        CopyArray( srcColData->pShadowMeshFaces,        dstColShadowMeshFaceArray, numShadowMeshFaces );

        // Set the array pointers to the destination struct.
        dstColData->pColSpheres = dstColSphereArray;
        dstColData->pColBoxes = dstColBoxArray;
        dstColData->pSuspensionLines = dstColSuspensionLineArray;
        dstColData->pColVertices = dstColVertexArray;
        dstColData->pColTriangles = dstColTriangleArray;
        dstColData->pColTrianglePlanes = NULL;
        
        dstColData->pShadowMeshVertices = dstColShadowMeshVertexArray;
        dstColData->pShadowMeshFaces = dstColShadowMeshFaceArray;
    }

    // Copy over remaining settings.
    clone->m_isCollidable = this->m_isCollidable;
    clone->m_colPoolIndex = this->m_colPoolIndex;

    clone->pColData = dstColData;
    return clone;

cloneFail:
    delete clone;
    
    return NULL;
}

// Binary offsets: (1.0 US and 1.0 EU): 0x0040F120
void CColDataSA::Clone( const CColDataSA *source )
{
    // First clone the spheres.
    short srcNumSpheres = source->numSpheres;

    if ( srcNumSpheres == 0 )
    {
        this->numSpheres = 0;

        if ( CColSphereSA *oldSpheres = this->pColSpheres )
        {
            RwFree( oldSpheres );
        }

        this->pColSpheres = NULL;
    }
    else
    {
        if ( this->numSpheres != srcNumSpheres )
        {
            this->numSpheres = srcNumSpheres;

            if ( CColSphereSA *oldSpheres = this->pColSpheres )
            {
                RwFree( oldSpheres );
            }

            this->pColSpheres = (CColSphereSA*)RwMalloc( sizeof( CColSphereSA ) * this->numSpheres, 0 );
        }

        CopyArray( source->pColSpheres, this->pColSpheres, this->numSpheres );
    }

    // Next the number of wheels, a.k.a. suspension lines/cones.
    char srcNumWheels = source->ucNumWheels;

    if ( srcNumWheels == 0 )
    {
        this->ucNumWheels = 0;

        if ( CColSuspensionDataSA *oldSuspLines = this->pSuspensionLines )
        {
            RwFree( oldSuspLines );
        }

        this->pSuspensionLines = NULL;
    }
    else
    {
        if ( srcNumWheels != this->ucNumWheels || this->hasSuspensionCones == false )
        {
            this->ucNumWheels = srcNumWheels;

            if ( CColSuspensionDataSA *oldSuspLines = this->pSuspensionLines )
            {
                RwFree( oldSuspLines );
            }

            CColSuspensionDataSA *actualSuspData = NULL;

            if ( this->hasSuspensionCones )
            {
                actualSuspData = (CColSuspensionDataSA*)RwMalloc( sizeof( CColSuspensionConeSA ) * this->ucNumWheels, 0 );
            }
            else
            {
                actualSuspData = (CColSuspensionDataSA*)RwMalloc( sizeof( CColSuspensionLineSA ) * this->ucNumWheels, 0 );
            }

            this->pSuspensionLines = actualSuspData;
        }

        if ( this->hasSuspensionCones )
        {
            const CColSuspensionConeSA *srcConeData = (const CColSuspensionConeSA*)source->pSuspensionLines;
            CColSuspensionConeSA *dstConeData = (CColSuspensionConeSA*)this->pSuspensionLines;

            CopyArray( srcConeData, dstConeData, this->ucNumWheels );
        }
        else
        {
            const CColSuspensionLineSA *srcLineData = (const CColSuspensionLineSA*)source->pSuspensionLines;
            CColSuspensionLineSA *dstLineData = (CColSuspensionLineSA*)this->pSuspensionLines;

            CopyArray( srcLineData, dstLineData, this->ucNumWheels );
        }
    }

    // Move over flags.
    this->hasSuspensionCones = source->hasSuspensionCones;
    this->hasShadowMeshFaces = source->hasShadowMeshFaces;

    // Move over boxes.
    short srcNumBoxes = source->numBoxes;

    if ( srcNumBoxes == 0 )
    {
        this->numBoxes = 0;

        if ( CColBoxSA *oldBoxes = this->pColBoxes )
        {
            RwFree( oldBoxes );
        }

        this->pColBoxes = NULL;
    }
    else
    {
        if ( this->numBoxes != srcNumBoxes )
        {
            this->numBoxes = srcNumBoxes;

            if ( CColBoxSA *oldBoxes = this->pColBoxes )
            {
                RwFree( oldBoxes );
            }

            this->pColBoxes = (CColBoxSA*)RwMalloc( sizeof( CColBoxSA ) * this->numBoxes, 0 );
        }

        CopyArray( source->pColBoxes, this->pColBoxes, this->numBoxes );
    }

    // Now the triangles.
    short srcNumColTriangles = source->numColTriangles;

    if ( srcNumColTriangles == 0 )
    {
        // Set the triangle count to zero.
        this->numColTriangles = 0;

        // Free the triangles.
        if ( CColTriangleSA *oldTriangles = this->pColTriangles )
        {
            RwFree( oldTriangles );
        }

        this->pColTriangles = NULL;

        // Now free the vertice.
        if ( CColVertexSA *oldVertice = this->pColVertices )
        {
            RwFree( oldVertice );
        }

        this->pColVertices = NULL;
    }
    else
    {
        // First update the vertices.
        unsigned short srcNumVertices = (unsigned short)source->CalculateNumberOfCollisionMeshVertices();

        if ( CColVertexSA *oldVertice = this->pColVertices )
        {
            RwFree( oldVertice );
        }

        if ( srcNumVertices > 0 )
        {
            this->pColVertices = (CColVertexSA*)RwMalloc( sizeof( CColVertexSA ) * srcNumVertices, 0 );

            CopyArray( source->pColVertices, this->pColVertices, srcNumVertices );
        }

        // Next update the triangles.
        if ( source->numColTriangles != this->numColTriangles )
        {
            this->numColTriangles = source->numColTriangles;

            if ( CColTriangleSA *oldTriangles = this->pColTriangles )
            {
                RwFree( oldTriangles );
            }

            this->pColTriangles = (CColTriangleSA*)RwMalloc( sizeof( CColTriangleSA ) * this->numColTriangles, 0 );
        }

        CopyArray( source->pColTriangles, this->pColTriangles, this->numColTriangles );
    }

    // Now the shadow mesh stuff.
    int srcNumShadowMeshFaces = source->numShadowMeshFaces;

    if ( srcNumShadowMeshFaces == 0 || source->hasShadowMeshFaces == false )
    {
        // Set the counts to zero.
        this->numShadowMeshFaces = 0;
        this->numShadowMeshVertices = 0;

        // Release shadow mesh faces.
        if ( CColTriangleSA *oldShadowTriangles = this->pShadowMeshFaces )
        {
            RwFree( oldShadowTriangles );
        }

        this->pShadowMeshFaces = NULL;

        // Release shadow mesh vertice.
        if ( CColVertexSA *oldShadowVertice = this->pShadowMeshVertices )
        {
            RwFree( oldShadowVertice );
        }

        this->pShadowMeshVertices = NULL;
    }
    else
    {
        unsigned short srcNumShadowVertices = (unsigned short)source->CalculateNumberOfShadowMeshVertices();

        if ( CColVertexSA *oldShadowVertice = this->pShadowMeshVertices )
        {
            RwFree( oldShadowVertice );
        }

        if ( srcNumShadowVertices > 0 )
        {
            this->pShadowMeshVertices = (CColVertexSA*)RwMalloc( sizeof( CColVertexSA ) * srcNumShadowVertices, 0 );

            CopyArray( source->pShadowMeshVertices, this->pShadowMeshVertices, srcNumShadowVertices );
        }

        // Now update the shadow triangles.
        if ( source->numShadowMeshFaces != this->numShadowMeshFaces )
        {
            this->numShadowMeshFaces = source->numShadowMeshFaces;

            if ( CColTriangleSA *oldShadowTriangles = this->pShadowMeshFaces )
            {
                RwFree( oldShadowTriangles );
            }

            this->pShadowMeshFaces = (CColTriangleSA*)RwMalloc( sizeof( CColTriangleSA ) * this->numShadowMeshFaces, 0 );
        }

        CopyArray( source->pShadowMeshFaces, this->pShadowMeshFaces, this->numShadowMeshFaces );

        this->numShadowMeshVertices = source->numShadowMeshVertices;
    }
}

// Binary offsets: (1.0 US and 1.0 EU): 0x0040F6E0
CColDataSA::trianglePlanesListNode* CColDataSA::GetTrianglePlanesListNode( void )
{
    return *(trianglePlanesListNode**)( ALIGN_SIZE( (unsigned int)pColTrianglePlanes + numColTriangles * 10, (unsigned int)4 ) );
}

inline CColDataSA::trianglePlanesListNode& GetFreeTrianglePlanesNode( void )
{
    return *(CColDataSA::trianglePlanesListNode*)0x00965944;
}

// Binary offsets: (1.0 US and 1.0 EU): 0x00416400
void CColDataSA::SegmentedClear( void )
{
    // Deallocate the triangle planes array.
    if ( pColTrianglePlanes )
    {
        // Get the triangle planes list node.
        trianglePlanesListNode *node = GetTrianglePlanesListNode();

        LIST_REMOVE( *node );
        LIST_APPEND( GetFreeTrianglePlanesNode(), *node );

        RwFree( pColTrianglePlanes );

        pColTrianglePlanes = NULL;
    }
}

template <typename structType>
inline void SafeFree( structType *data )
{
    if ( data )
    {
        RwFree( data );
    }
}

// Binary offsets: (1.0 US and 1.0 EU): 0x0040F070
void CColDataSA::UnsegmentedClear( void )
{
    SafeFree( pColSpheres );
    SafeFree( pColBoxes );
    SafeFree( pSuspensionLines );
    SafeFree( pColVertices );
    SafeFree( pColTriangles );

    SafeFree( pShadowMeshVertices );
    SafeFree( pShadowMeshFaces );

    SegmentedClear();

    // Clear data fields.
    numSpheres = 0;
    ucNumWheels = 0;
    numBoxes = 0;
    numColTriangles = 0;

    pColSpheres = NULL;
    pColBoxes = NULL;
    pSuspensionLines = NULL;
    pColVertices = NULL;
    pColTriangles = NULL;

    pShadowMeshVertices = NULL;
    pShadowMeshFaces = NULL;
}

CColFaceGroupHeaderSA* CColDataSA::GetFaceGroupHeader( void )
{
    if ( !hasFaceGroups || !pColTriangles )
        return NULL;

    return ( (CColFaceGroupHeaderSA*)pColTriangles - 1 );
}

CColFaceGroupSA* CColDataSA::GetFaceGroup( unsigned int groupIndex )
{
    CColFaceGroupHeaderSA *faceHeader = GetFaceGroupHeader();

    if ( !faceHeader )
        return NULL;

    if ( groupIndex >= faceHeader->numFaceGroups )
        return NULL;

    return (CColFaceGroupSA*)faceHeader - ( groupIndex + 1 );
}