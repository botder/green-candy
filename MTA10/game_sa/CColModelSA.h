/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CColModelSA.h
*  PURPOSE:     Header file for collision model entity class
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Alberto Alonso <rydencillo@gmail.com>
*               Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_COLMODELSA
#define __CGAMESA_COLMODELSA

#include <game/CColModel.h>

// Collision stored surface type
struct EColSurface
{
    enum EColSurfaceValue
    {
        DEFAULT,
        TARMAC,
        TARMAC_FUCKED,
        TARMAC_REALLYFUCKED,
        PAVEMENT,
        PAVEMENT_FUCKED,
        GRAVEL,
        FUCKED_CONCRETE,
        PAINTED_GROUND,
        GRASS_SHORT_LUSH,
        GRASS_MEDIUM_LUSH,
        GRASS_LONG_LUSH,
        GRASS_SHORT_DRY,
        GRASS_MEDIUM_DRY,
        GRASS_LONG_DRY,
        GOLFGRASS_ROUGH,
        GOLFGRASS_SMOOTH,
        STEEP_SLIDYGRASS,
        STEEP_CLIFF,
        FLOWERBED,
        MEADOW,
        WASTEGROUND,
        WOODLANDGROUND,
        VEGETATION,
        MUD_WET,
        MUD_DRY,
        DIRT,
        DIRTTRACK,
        SAND_DEEP,
        SAND_MEDIUM,
        SAND_COMPACT,
        SAND_ARID,
        SAND_MORE,
        SAND_BEACH,
        CONCRETE_BEACH,
        ROCK_DRY,
        ROCK_WET,
        ROCK_CLIFF,
        WATER_RIVERBED,
        WATER_SHALLOW,
        CORNFIELD,
        HEDGE,
        WOOD_CRATES,
        WOOD_SOLID,
        WOOD_THIN,
        GLASS,
        GLASS_WINDOWS_LARGE,
        GLASS_WINDOWS_SMALL,
        EMPTY1,
        EMPTY2,
        GARAGE_DOOR,
        THICK_METAL_PLATE,
        SCAFFOLD_POLE,
        LAMP_POST,
        METAL_GATE,
        METAL_CHAIN_FENCE,
        GIRDER,
        FIRE_HYDRANT,
        CONTAINER,
        NEWS_VENDOR,
        WHEELBASE,
        CARDBOARDBOX,
        PED,
        CAR,
        CAR_PANEL,
        CAR_MOVINGCOMPONENT,
        TRANSPARENT_CLOTH,
        RUBBER,
        PLASTIC,
        TRANSPARENT_STONE,
        WOOD_BENCH,
        CARPET,
        FLOORBOARD,
        STAIRSWOOD,
        P_SAND,
        P_SAND_DENSE,
        P_SAND_ARID,
        P_SAND_COMPACT,
        P_SAND_ROCKY,
        P_SANDBEACH,
        P_GRASS_SHORT,
        P_GRASS_MEADOW,
        P_GRASS_DRY,
        P_WOODLAND,
        P_WOODDENSE,
        P_ROADSIDE,
        P_ROADSIDEDES,
        P_FLOWERBED,
        P_WASTEGROUND,
        P_CONCRETE,
        P_OFFICEDESK,
        P_711SHELF1,
        P_711SHELF2,
        P_711SHELF3,
        P_RESTUARANTTABLE,
        P_BARTABLE,
        P_UNDERWATERLUSH,
        P_UNDERWATERBARREN,
        P_UNDERWATERCORAL,
        P_UNDERWATERDEEP,
        P_RIVERBED,
        P_RUBBLE,
        P_BEDROOMFLOOR,
        P_KIRCHENFLOOR,
        P_LIVINGRMFLOOR,
        P_CORRIDORFLOOR,
        P_711FLOOR,
        P_FASTFOODFLOOR,
        P_SKANKYFLOOR,
        P_MOUNTAIN,
        P_MARSH,
        P_BUSHY,
        P_BUSHYMIX,
        P_BUSHYDRY,
        P_BUSHYMID,
        P_GRASSWEEFLOWERS,
        P_GRASSDRYTALL,
        P_GRASSLUSHTALL,
        P_GRASSGRNMIX,
        P_GRASSBRNMIX,
        P_GRASSLOW,
        P_GRASSROCKY,
        P_GRASSSMALLTREES,
        P_DIRTROCKY,
        P_DIRTWEEDS,
        P_GRASSWEEDS,
        P_RIVEREDGE,
        P_POOLSIDE,
        P_FORESTSTUMPS,
        P_FORESTSTICKS,
        P_FORRESTLEAVES,
        P_DESERTROCKS,
        P_FORRESTDRY,
        P_SPARSEFLOWERS,
        P_BUILDINGSITE,
        P_DOCKLANDS,
        P_INDUSTRIAL,
        P_INDUSTJETTY,
        P_CONCRETELITTER,
        P_ALLEYRUBISH,
        P_JUNKYARDPILES,
        P_JUNKYARDGRND,
        P_DUMP,
        P_CACTUSDENSE,
        P_AIRPORTGRND,
        P_CORNFIELD,
        P_GRASSLIGHT,
        P_GRASSLIGHTER,
        P_GRASSLIGHTER2,
        P_GRASSMID1,
        P_GRASSMID2,
        P_GRASSDARK,
        P_GRASSDARK2,
        P_GRASSDIRTMIX,
        P_RIVERBEDSTONE,
        P_RIVERBEDSHALLOW,
        P_RIVERBEDWEEDS,
        P_SEAWEED,
        DOOR,
        PLASTICBARRIER,
        PARKGRASS,
        STAIRSSTONE,
        STAIRSMETAL,
        STAIRSCARPET,
        FLOORMETAL,
        FLOORCONCRETE,
        BIN_BAG,
        THIN_METAL_SHEET,
        METAL_BARREL,
        PLASTIC_CONE,
        PLASTIC_DUMPSTER,
        METAL_DUMPSTER,
        WOOD_PICKET_FENCE,
        WOOD_SLATTED_FENCE,
        WOOD_RANCH_FENCE,
        UNBREAKABLE_GLASS,
        HAY_BALE,
        GORE,
        RAILTRACK,
    };

    // Enum in a byte
    EColSurface     ( void )                       {}
    EColSurface     ( EColSurfaceValue newValue )  { m_ucValue = newValue; }
    operator uchar  ( void ) const                 { return m_ucValue; }
    uchar   m_ucValue;
};
typedef EColSurface::EColSurfaceValue EColSurfaceValue;

// Collision stored lighting
struct CColLighting
{
    uchar   day:4;    // 0-15
    uchar   night:4;  // 0-15
};

#define FUNC_CColModel_Constructor      0x40FB60
#define FUNC_CColModel_Destructor       0x40F700

typedef struct
{
    CVector     vecMin;
    CVector     vecMax;
    CVector     vecOffset;
    FLOAT       fRadius;
} CBoundingBoxSA;


struct CColSurfaceSA
{
    EColSurface     material;
    unsigned char   flags;
    unsigned char   unknown;
    CColLighting    light;
};

// Placeholder type for a dynamic struct.
typedef void CColSuspensionDataSA;

struct CColSuspensionLineSA
{
    BYTE pad[32];
};

struct CColSuspensionConeSA
{
    BYTE pad[36];
};

inline size_t GetSuspensionLineDataSize( bool isCone )
{
    size_t theSize;

    if ( isCone )
    {
        theSize = sizeof( CColSuspensionConeSA );
    }
    else
    {
        theSize = sizeof( CColSuspensionLineSA );
    }

    return theSize;
}


struct CColSphereSA
{
    inline CColSphereSA( float fRadius, const CVector& vecCenter, EColSurface material, unsigned char flags, unsigned char unknown )
    {
        this->fRadius = fRadius;
        this->vecCenter = vecCenter;
        this->surface.material = material;
        this->surface.flags = flags;
        this->surface.unknown = unknown;
    }

    CVector         vecCenter;
    float           fRadius;
    CColSurfaceSA   surface;
};


struct CColBoxSA
{
    inline CColBoxSA( const CVector& min, const CVector& max, EColSurface material, unsigned char flags, unsigned char unknown )
    {
        this->min = min;
        this->max = max;
        this->surface.material = material;
        this->surface.flags = flags;
        this->surface.unknown = unknown;
    }

    CVector         min;
    CVector         max;
    CColSurfaceSA   surface;
};


struct CColVertexSA
{
    inline CColVertexSA( short a, short b, short c )
    {
        this->a = a;
        this->b = b;
        this->c = c;
    }

    short a, b, c;
};


typedef struct
{
    unsigned short  v1;
    unsigned short  v2;
    unsigned short  v3;
    EColSurface     material;
    CColLighting    lighting;
} CColTriangleSA;


struct CColFaceGroupSA
{
    CVector min, max;
    unsigned short StartFace, EndFace;
};


struct CColFaceGroupHeaderSA
{
    unsigned int numFaceGroups;
};


typedef struct
{
    BYTE pad0 [ 12 ];
} CColTrianglePlaneSA;


typedef struct
{
    union
    {
        char version[4];
        unsigned int checksum;
    };

    DWORD size;
    char name[22];
    unsigned short modelId;
} ColModelFileHeader;

class CColFileSA
{
public:
    CColFileSA( const char *name ) : m_bounds( MAX_REGION, -MAX_REGION, -MAX_REGION, MAX_REGION )
    {
        m_loaded = false;
        m_sectorLoad = false;
        m_isProcedural = false;
        m_isInterior = false;

        m_refs = 0;

        // Do not store the name, but check it for properties.
        // Apparently these properties are hardcoded.
        if ( stricmp( name, "procobj" ) == 0 || stricmp( name, "proc_int" ) == 0 || stricmp( name, "proc_int2" ) == 0 )
        {
            m_isProcedural = true;
        }

        if ( strnicmp( name, "int_la", 6 ) == 0 ||
             strnicmp( name, "int_sf", 6 ) == 0 ||
             strnicmp( name, "int_veg", 7 ) == 0 ||
             strnicmp( name, "int_cont", 8 ) == 0 ||
             strnicmp( name, "gen_int1", 8 ) == 0 ||
             strnicmp( name, "gen_int2", 8 ) == 0 ||
             strnicmp( name, "gen_int3", 8 ) == 0 ||
             strnicmp( name, "gen_int4", 8 ) == 0 ||
             strnicmp( name, "gen_int5", 8 ) == 0 ||
             strnicmp( name, "gen_intb", 8 ) == 0 ||
             strnicmp( name, "savehous", 8 ) == 0 ||
             stricmp( name, "props" ) == 0 ||
             stricmp( name, "props2" ) == 0 ||      // Okay, I am unsure whether I caught all of the namechecking due to secuROM obfuscation
                                                    // If there is a filename missing, feel free to append it here!
             strnicmp( name, "levelmap", 8 ) == 0 ||
             strnicmp( name, "stadint", 7 ) == 0 )
        {
            m_isInterior = true;
        }
    }

    void* operator new ( size_t );
    void operator delete ( void *ptr );

    // Utilities.
    void __thiscall ExtendBounds( const CBounds2D& bounds );

    // Sectorizer dependencies.
    inline bool     IsSectorFlagged( void ) const           { return m_refs > 0 || m_sectorLoad; }
    inline void     FlagSector( bool flagged )              { m_sectorLoad = flagged; }
    inline bool     IsLoaded( void ) const                  { return m_loaded; }
    inline bool     IsStreamingDisabled( void ) const       { return false; }

    inline void     OnStreamOut( void )                     {}

    CBounds2D                       m_bounds;           // 0

    BYTE                            m_pad[18];          // 16

    typedef ValueRange <short> indexRange_t;

    indexRange_t                    m_range;            // 34
    unsigned short                  m_refs;             // 38

    bool                            m_loaded;           // 40
    bool                            m_sectorLoad;       // 41, set by streaming to indicate that it wants to load it
    bool                            m_isProcedural;     // 42
    bool                            m_isInterior;       // 43
};

struct CColDataSA
{
    CColDataSA( void );

    struct trianglePlanesListNode
    {
        DWORD pad;
        trianglePlanesListNode *next;
        trianglePlanesListNode *prev;
    };

    trianglePlanesListNode* GetTrianglePlanesListNode( void );

    void __thiscall     Clone( const CColDataSA *source );

    void __thiscall     SegmentedClear( void );
    void __thiscall     UnsegmentedClear( void );

    CColFaceGroupHeaderSA*  GetFaceGroupHeader( void );
    CColFaceGroupSA*        GetFaceGroup( unsigned int groupIndex );

    inline size_t       GetSuspensionLineSize( void ) const     { return GetSuspensionLineDataSize( this->hasSuspensionCones ); }

    short                           numSpheres;             // 0
    short                           numBoxes;               // 2
    short                           numColTriangles;        // 4
    char                            ucNumWheels;            // 6

    union
    {
        unsigned char               flags;                  // 7

        struct
        {
            unsigned char           hasSuspensionCones : 1; // 7, DK22Pac has produced very valuable research here, I did not know what this flag meant.
                                                            // Kudos to him.
            unsigned char           hasFaceGroups : 1;      
            unsigned char           hasShadowMeshFaces : 1; 
        };
    };

    CColSphereSA*                   pColSpheres;            // 8
    CColBoxSA*                      pColBoxes;              // 12
    CColSuspensionDataSA*           pSuspensionLines;       // 16
    CColVertexSA*                   pColVertices;           // 20
    CColTriangleSA*                 pColTriangles;          // 24
    CColTrianglePlaneSA*            pColTrianglePlanes;     // 28

    int                             numShadowMeshFaces;     // 32
    unsigned int                    numShadowMeshVertices;  // 36
    CColVertexSA*                   pShadowMeshVertices;    // 40
    CColTriangleSA*                 pShadowMeshFaces;       // 44

    unsigned int        CalculateNumberOfShadowMeshVertices( void ) const;
    unsigned int        CalculateNumberOfCollisionMeshVertices( void ) const;
};

struct CColBoundingBox : public CBoundingBox
{
    CColBoundingBox( void );
};

class CColModelSAInterface
{
public:
                                    CColModelSAInterface    ( void );
                                    ~CColModelSAInterface   ( void );

    void __thiscall                 _DestructorHook         ( void );

    void __thiscall                 UnsegmentizeData        ( void );
    void __thiscall                 ReleaseData             ( void );

    CColModelSAInterface*           Clone                   ( void );

    void*   operator new ( size_t );
    void    operator delete ( void *ptr );

    CColBoundingBox                 m_bounds;                   // 0
    unsigned char                   m_colPoolIndex;             // 40
    bool                            m_isCollidable : 1;         // 41, if true, this collision interface has spheres, boxes or triangles
    bool                            m_isColDataSegmented : 1;   //     if true, pColData is the only allocation used (all pointers inside point to sub segments in memory)
    bool                            m_releaseDataOnDestroy : 1; //     if true, the destructor releases the collision data
    BYTE                            m_pad[2];                   // 42
    CColDataSA*                     pColData;                   // 44
};

#include "CColModelSA.loader.h"

// Utility functions.
void __cdecl Collision_Preload( void );

// Quad tree of all collision files.
typedef CQuadTreeNodeSAInterface <CColFileSA> colFileQuadTreeNode_t;

namespace Collision
{
    // The idea of a collision quad tree conflicts with our dynamical collision assignment system.
    // We have to update the GTA:SA logic so that it does not unload colFiles which we are still
    // using.
    // Update: since collision sectors have reference counts, its all good. :)
    inline colFileQuadTreeNode_t*   GetCollisionQuadTree( void )
    {
        return *(colFileQuadTreeNode_t**)0x0096555C;
    }
};

// Utility functions.
unsigned int __cdecl GetColInterfaceUseCount( CColModelSAInterface *colModel );

// Module Initialization.
void ColModel_Init( void );
void ColModel_Shutdown( void );


class CColModelSA : public CColModel
{
    friend class CStreamingSA;
public:

    struct colImport_t
    {
        modelId_t               modelIndex;
        CColModelSAInterface*   replaceCollision;
        CColModelSAInterface*   originalCollision;
        bool                    isOriginalDynamic;
    };
    typedef std::list <colImport_t> colImports_t;

                                    CColModelSA         ( void );
                                    CColModelSA         ( CColModelSAInterface *pInterface, bool destroy = false );
                                    ~CColModelSA        ( void );

    inline CColModelSAInterface*    GetInterface        ( void )                        { return m_pInterface; }

    bool                            Replace             ( modelId_t id );
    bool                            IsReplaced          ( modelId_t id ) const;
    colImport_t*                    FindImport          ( modelId_t id, colImports_t::iterator& findIter );
    const colImport_t*              FindImport          ( modelId_t id ) const;
    bool                            Restore             ( modelId_t id );
    void                            RestoreAll          ( void );

    imports_t                       GetImportList       ( void ) const;

    void                            SetOriginal         ( modelId_t modelIndex, CColModelSAInterface *col, bool isDynamic );
    CColModelSAInterface*           GetOriginal         ( modelId_t modelIndex, bool& originalDynamic );

private:
    CColModelSAInterface*           m_pInterface;
    bool                            m_bDestroyInterface;

    colImports_t                    m_imported;
};

// Module initialization.
void Collision_Init( void );
void Collision_Shutdown( void );

#endif
