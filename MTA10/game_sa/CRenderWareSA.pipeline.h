/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRenderWareSA.pipeline.h
*  PURPOSE:     RenderWare pipeline (OS implementation) management
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _RENDERWARE_PIPELINES_
#define _RENDERWARE_PIPELINES_

// D3D9 pipeline functions (use HOOK_* instead)
void __cdecl RwD3D9SetRenderState       ( D3DRENDERSTATETYPE type, DWORD value );
void __cdecl RwD3D9GetRenderState       ( D3DRENDERSTATETYPE type, DWORD& value );
void __cdecl RwD3D9ApplyDeviceStates    ( void );
void __cdecl RwD3D9ValidateDeviceStates ( void );

void __cdecl RwD3D9SetSamplerState      ( DWORD samplerId, D3DSAMPLERSTATETYPE stateType, DWORD value );
void __cdecl RwD3D9GetSamplerState      ( DWORD samplerId, D3DSAMPLERSTATETYPE stateType, DWORD& valueOut );

int __cdecl RwD3D9SetMaterial           ( const D3DMATERIAL9& material );
void __cdecl RwD3D9GetMaterial          ( D3DMATERIAL9& material );

int __cdecl RpD3D9SetSurfaceProperties  ( RpMaterialLighting& matLight, RwColor& matColor, unsigned int renderFlags );    // US exe: 0x007FC4D0
void RpD3D9InitializeMaterialEnvironment( void );

void __cdecl RwD3D9SetTextureStageState ( DWORD stageId, D3DTEXTURESTAGESTATETYPE stateType, DWORD value );
void __cdecl RwD3D9GetTextureStageState ( DWORD stageId, D3DTEXTURESTAGESTATETYPE stateType, DWORD& value );

// MTA extensions
void RwD3D9ForceRenderState             ( D3DRENDERSTATETYPE type, DWORD value );
void RwD3D9FreeRenderState              ( D3DRENDERSTATETYPE type );
void RwD3D9FreeRenderStates             ( void );

void RwD3D9InitializeCurrentStates      ( void );

struct RwD3D9RenderCallbackData
{
    RwD3D9RenderCallbackData( void )
    {
        // Let the runtime initialize the struct, because there is no valid default state.
        return;
    }

    RwD3D9RenderCallbackData( const RwD3D9RenderCallbackData& right )
    {
        operator = ( right );
    }

    void operator = ( const RwD3D9RenderCallbackData& right )
    {
        primitiveType = right.primitiveType;
        startVertex = right.startVertex;
        numPrimitives = right.numPrimitives;
        indexedNumVertice = right.indexedNumVertice;
        indexedStartIndex = right.indexedStartIndex;
    }

    D3DPRIMITIVETYPE                primitiveType;
    unsigned int                    startVertex;
    unsigned int                    numPrimitives;
    unsigned int                    indexedNumVertice, indexedStartIndex;
};

struct RwD3D9StreamInfo //size: 16 bytes
{
    AINLINE RwD3D9StreamInfo( void )
    {
        m_vertexBuf = NULL;
        m_offset = 0;
        m_stride = 0;
        unk4 = 0;
    }

    IDirect3DVertexBuffer9* m_vertexBuf;    // 0
    size_t m_offset;                        // 4
    size_t m_stride;                        // 8
    int unk4;                               // 12
};

struct RwD3D9Streams    //size: 32 bytes
{
    RwD3D9StreamInfo    streams[2];     // 0
};

void __cdecl RwD3D9SetStreamSource      ( DWORD streamIndex, IDirect3DVertexBuffer9 *streamPtr, size_t streamOffset, size_t streamStride );
void __cdecl RwD3D9SetStreams           ( const RwD3D9Streams& streams, int useOffset );
void __cdecl RwD3D9GetStreams           ( RwD3D9Streams& streamInfo );

typedef CPool <CEnvMapMaterialSA, 16000> CEnvMapMaterialPool;
typedef CPool <CEnvMapAtomicSA, 4000> CEnvMapAtomicPool;
typedef CPool <CSpecMapMaterialSA, 16000> CSpecMapMaterialPool;

namespace RenderWare
{
    // Nasty pools which limit rendering.
    inline CEnvMapMaterialPool*&    GetEnvMapMaterialPool   ( void )        { return *(CEnvMapMaterialPool**)0x00C02D28; }
    inline CEnvMapAtomicPool*&      GetEnvMapAtomicPool     ( void )        { return *(CEnvMapAtomicPool**)0x00C02D2C; }
    inline CSpecMapMaterialPool*&   GetSpecMapMaterialPool  ( void )        { return *(CSpecMapMaterialPool**)0x00C02D30; }

    // Global rendering utilities.
    D3DLIGHT9&  GetSpecialVehicleLight( void );
};

// Stack-based anonymous RenderState management
//todo: fix this (removed inlined RS changes from native code)
struct RwRenderStateLock
{
    struct _rsLockDesc
    {
        _rsLockDesc( void )
        {
            refCount = 0;
        }

        unsigned int refCount;
    };

    static _rsLockDesc _rsLockInfo[210];

    /*=========================================================
        RwRenderStateLock::constructor (MTA extension)

        Purpose:
            Sets a RenderState for RenderWare drawing operations
            and locks them from changing by the GTA:SA engine
            until the reference count reaches zero.
    =========================================================*/
    AINLINE RwRenderStateLock( D3DRENDERSTATETYPE type, DWORD value )
    {
        m_type = type;

        // Increase the reference count
        if ( ++_rsLockInfo[type].refCount > 1 )
        {
            // Only have to store the previous RenderState value now
            RwD3D9GetRenderState( type, m_prevValue );
        }

        // Apply our RenderState and save the original GTA:SA one.
        RwD3D9ForceRenderState( type, value );
    }

    /*=========================================================
        RwRenderStateLock::destructor (MTA extension)

        Purpose:
            Restore the status of said RenderState to the one previous
            to creation of this lock. If reference count reaches
            zero, 
    =========================================================*/
    AINLINE ~RwRenderStateLock( void )
    {
        // Free if not referenced anymore
        if ( --_rsLockInfo[m_type].refCount == 0 )
            RwD3D9FreeRenderState( m_type );
        else
            RwD3D9ForceRenderState( m_type, m_prevValue );
    }

    D3DRENDERSTATETYPE  m_type;
    DWORD               m_prevValue;
};

// Structure for fast stack based struct/class allocation.
template <class allocType, int max>
class StaticAllocator
{
    char mem[sizeof(allocType) * max];
    unsigned int count;

public:
    AINLINE StaticAllocator( void )
    {
        count = 0;
    }
    
    AINLINE void* Allocate( void )
    {
        assume( count < max );

        void *outMem = mem + (sizeof(allocType) * count++);

        assume( outMem != NULL );

        return outMem;
    }

    // Hack function; only use if performance critical code is used.
    AINLINE void Pop( void )
    {
        count--;
    }
};

void RenderWarePipeline_Init( void );
void RenderWarePipeline_Shutdown( void );

#endif //_RENDERWARE_PIPELINES_