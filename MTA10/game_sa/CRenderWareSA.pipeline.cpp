/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRenderWareSA.pipeline.cpp
*  PURPOSE:     RenderWare pipeline (OS implementation) management
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include "RenderWare/RwRenderTools.hxx"
#include "RenderWare/RwInternals.h"

#include "CRenderWareSA.rmode.hxx"

#include "CRenderWareSA.state.rs.hxx"
#include "CRenderWareSA.state.tss.hxx"
#include "CRenderWareSA.state.sampler.hxx"
#include "CRenderWareSA.state.transf.hxx"
#include "CRenderWareSA.state.lighting.hxx"
#include "CRenderWareSA.state.stream.hxx"

// Definitions of device state managers.
RwRenderStateManager g_renderStateManager;
RwTextureStageStateManager g_textureStageStateManager;
RwSamplerStateManager g_samplerStateManager;
RwVertexStreamStateManager g_vertexStreamStateManager;

RwRenderStateLock::_rsLockDesc RwRenderStateLock::_rsLockInfo[210];

/*=========================================================
    RwD3D9ForceRenderState (MTA extension)

    Arguments:
        type - D3D9 RenderState id
        value - integer with the new RenderState value
    Purpose:
        Applies a RenderState and ignores further RenderState
        assignments until that RenderState is not required
        by MTA anymore.
=========================================================*/
struct _internalRenderStateDesc
{
    _internalRenderStateDesc( void )
    {
        value = 0;
        isForced = false;
    }

    deviceStateValue value;         // native GTA:SA renderstate
    deviceStateValue forceValue;    // value to be forced into the rendering device
    bool isForced;                  // boolean whether MTA wants only its renderstate active
};
static _internalRenderStateDesc _intRenderStates[210];

void RwD3D9ForceRenderState( D3DRENDERSTATETYPE type, DWORD value )
{
    RwRenderStateManager::stateAddress theAddress;
    theAddress.type = type;

    _internalRenderStateDesc& desc = _intRenderStates[type];

    // Notify the system that we force a RenderState
    if ( !desc.isForced )
    {
        desc.isForced = true;
        desc.value = g_renderStateManager.GetDeviceState( theAddress );
    }

    // Store the forced renderstate so we ensure sync with native code.
    desc.forceValue = value;

    // Set the MTA RenderState
    g_renderStateManager.SetDeviceStateChecked( theAddress, value );
}

/*=========================================================
    RwD3D9SetRenderState

    Arguments:
        type - D3D9 RenderState id
        value - integer with the new RenderState value
    Purpose:
        Called by the engine to set a RenderWare D3D9 pipeline
        RenderState. MTA RenderStates can take priority over
        GTA:SA ones, so they should be delayed if there is a
        conflict.
    Binary offsets:
        (1.0 US): 0x007FC2D0
        (1.0 EU): 0x007FC310
=========================================================*/
void __cdecl RwD3D9SetRenderState( D3DRENDERSTATETYPE type, DWORD value )
{
    // Actual bugfix.
    __asm push edx

    _internalRenderStateDesc& desc = _intRenderStates[ type ];

    // Check whether MTA wants to force a RenderState
    if ( desc.isForced )
    {
        // Store the internal value
        desc.value = value;
    }
    else    // Otherwise we just set it
    {
        RwRenderStateManager::stateAddress theAddress;
        theAddress.type = type;

        g_renderStateManager.SetDeviceStateChecked( theAddress, value );
    }

    __asm pop edx
}

/*=========================================================
    RwD3D9SetTextureStageState

    Arguments:
        stageId - index of the Direct3D 9 TSS
        stateType - type index of the TSS
        value - the new value to the TSS
    Purpose:
        Updates a texture stage state of the main Direct3D 9
        device. The entries are cached, so that unnecessary
        accesses are not made.
    Binary offsets:
        (1.0 US): 0x007FC340
        (1.0 EU): 0x007FC380
    Note:
        When dealing with API that tries to adapt to proprietory
        APIs like Direct3D, we better leave the usage as close to
        the original as possible. I had the idea to let this
        function return a DWORD, but decided against it since I
        realized that it did not match the GTA:SA representation.
=========================================================*/
void __cdecl RwD3D9SetTextureStageState( DWORD stageId, D3DTEXTURESTAGESTATETYPE stateType, DWORD value )
{
    RwTextureStageStateManager::stateAddress address;
    address.stageIndex = stageId;
    address.stageType = stateType;

    g_textureStageStateManager.SetDeviceStateChecked( address, value );
}

/*=========================================================
    RwD3D9GetTextureStageState

    Arguments:
        stageId - index of the Direct3D 9 TSS
        stateType - type index of the TSS
        value - pointer to the value where the TSS value
                should be written to
    Purpose:
        Retrieves the current Direct3D 9 device texture stage
        state. The value may not be written to the device yet
        but is pending for a push on RwD3D9ApplyDeviceStates.
    Binary offsets:
        (1.0 US): 0x007FC3A0
        (1.0 EU): 0x007FC3E0
    Note:
        When dealing with API that tries to adapt to proprietory
        APIs like Direct3D, we better leave the usage as close to
        the original as possible. I had the idea to let this
        function return a DWORD, but decided against it since I
        realized that it did not match the GTA:SA representation.
=========================================================*/
void __cdecl RwD3D9GetTextureStageState( DWORD stageId, D3DTEXTURESTAGESTATETYPE stateType, DWORD& value )
{
    RwTextureStageStateManager::stateAddress address;
    address.stageIndex = stageId;
    address.stageType = stateType;

    value = g_textureStageStateManager.GetDeviceState( address );
}

/*=========================================================
    RwD3D9GetRenderState

    Arguments:
        type - D3D9 RenderState id
    Purpose:
        Retrieves the current RenderWare D3D9 pipeline
        RenderState (for GTA:SA).
    Binary offsets:
        (1.0 US): 0x007FC320
        (1.0 EU): 0x007FC360
=========================================================*/
void __cdecl RwD3D9GetRenderState( D3DRENDERSTATETYPE type, DWORD& value )
{
    // Actual bugfix.
    __asm push edx

    _internalRenderStateDesc& desc = _intRenderStates[type];

    if ( desc.isForced )
        value = desc.forceValue;
    else
    {
        RwRenderStateManager::stateAddress theAddress;
        theAddress.type = type;

        value = g_renderStateManager.GetDeviceState( theAddress );
    }

    __asm pop edx
}

/*=========================================================
    RwD3D9FreeRenderState (MTA extension)

    Arguments:
        type - D3D9 RenderState id
    Purpose:
        Removes the forced assignment of a RenderWare D3D9
        pipeline RenderState by MTA and sets the GTA:SA
        RenderState value.
=========================================================*/
void RwD3D9FreeRenderState( D3DRENDERSTATETYPE type )
{
    _internalRenderStateDesc& desc = _intRenderStates[type];

    if ( !desc.isForced )
        return;

    // Notify we are not forcing a value anymore
    desc.isForced = false;

    // If a rogue renderstate change has happened, we cannot revert back.
    // Reverting back would screw the integrity of renderstates.
    // (rogue changes should be avoided at all cost, but happen often
    // since we do not control the whole GTA:SA codebase yet).
    DWORD curState = 0;

    RwRenderStateManager::stateAddress theAddress;
    theAddress.type = type;

    curState = g_renderStateManager.GetDeviceState( theAddress );

    if ( curState == desc.forceValue )
    {
        // Revert to the GTA:SA version
        g_renderStateManager.SetDeviceStateChecked( theAddress, desc.value );
    }
}

/*=========================================================
    RwD3D9FreeRenderStates (MTA extension)

    Purpose:
        Works through the internal queue of GTA:SA assigned
        RenderStates and reverts them to their internal
        values.
=========================================================*/
void RwD3D9FreeRenderStates( void )
{
    for ( unsigned int n = 0; n < 210; n++ )
    {
        D3DRENDERSTATETYPE type = (D3DRENDERSTATETYPE)n;
        _internalRenderStateDesc& desc = _intRenderStates[type];

        // Revert the value (if forced)
        RwD3D9FreeRenderState( type );
    }

    RwD3D9ApplyDeviceStates();
}

/*=========================================================
    RwD3D9ApplyDeviceStates

    Purpose:
        Applies the queued RenderWare D3D9 RenderStates to
        the device.
    Binary offsets:
        (1.0 US): 0x007FC200
        (1.0 EU): 0x007FC240
=========================================================*/
void __cdecl RwD3D9ApplyDeviceStates( void )
{
    IDirect3DDevice9 *renderDevice = GetRenderDevice();

    // Apply vertex stream information.
    g_vertexStreamStateManager.ApplyDeviceStates();
    
    // Apply RenderStates
    g_renderStateManager.ApplyDeviceStates();

    // Apply texture stage states
    g_textureStageStateManager.ApplyDeviceStates();

    // Apply sampler states.
    g_samplerStateManager.ApplyDeviceStates();

    // Apply lighting states.
    g_lightingStateManager.ApplyDeviceStates();

    // Apply transformations.
    g_transformationStateManager.ApplyDeviceStates();
}

/*=========================================================
    RwD3D9ValidateDeviceStates

    Purpose:
        Makes sure that the RenderWare states match the states
        that they are meant to have. This function should be
        used to debug MTA and GTA:SA renderstate interactions.

        Define DEBUG_DEVICE_STATE_INTEGRITY to use breakpoint
        on device state integrity violation. These violations
        are caused by MTA and have to be debugged.
=========================================================*/
void __cdecl RwD3D9ValidateDeviceStates( void )
{
    IDirect3DDevice9 *renderDevice = GetRenderDevice();

    // Validate renderstates.
    g_renderStateManager.ValidateDeviceStates();

    // Validate texture stage states.
    g_textureStageStateManager.ValidateDeviceStates();

    // Validate sampler states.
    g_samplerStateManager.ValidateDeviceStates();
}

/*=========================================================
    RwD3D9SetSamplerState

    Arguments:
        samplerId - index of the sampler to access
        stateType - type identifier of the parameter to update
        value - parameter to pass to the sampler
    Purpose:
        Updates a sampler parameter of the specified sampler.
        The change is directly updated to the GPU if it is
        changed.
    Binary offsets:
        (1.0 US): 0x007FC3C0
        (1.0 EU): 0x007FC400
    Note:
        GTA:SA does never change the sampler states during
        runtime.
=========================================================*/
void __cdecl RwD3D9SetSamplerState( DWORD samplerId, D3DSAMPLERSTATETYPE stateType, DWORD value )
{
    RwSamplerStateManager::stateAddress theAddress;
    theAddress.samplerId = samplerId;
    theAddress.stateType = stateType;

    g_samplerStateManager.SetDeviceStateChecked( theAddress, value );
}

/*=========================================================
    RwD3D9GetSamplerState

    Arguments:
        samplerId - index of the sampler to access
        stateType - type identifier of the parameter to update
        value - ptr to save the current parameter at
    Purpose:
        Retrieves the current sampler parameter from the
        specified sampler. The implementation assumes that its
        sampler states are active, so it returns data from its
        native table.
    Binary offsets:
        (1.0 US): 0x007FC400
        (1.0 EU): 0x007FC440
    Note:
        GTA:SA does never retrieve the current sampler states
        during runtime.
=========================================================*/
void __cdecl RwD3D9GetSamplerState( DWORD samplerId, D3DSAMPLERSTATETYPE stateType, DWORD& valueOut )
{
    RwSamplerStateManager::stateAddress theAddress;
    theAddress.samplerId = samplerId;
    theAddress.stateType = stateType;

    valueOut = g_samplerStateManager.GetDeviceState( theAddress );
}

/*=========================================================
    RwD3D9SetMaterial

    Arguments:
        material - device material data pointer
    Purpose:
        Sets the current device material for rendering. The
        implementation currently only supports one material.
    Binary offsets:
        (1.0 US): 0x007FC430
        (1.0 EU): 0x007FC470
=========================================================*/
// current runtime surface property snapshot.
static float _surfaceDiffuseLighting = 0.0f;        // Binary offsets: (1.0 US and 1.0 EU): 0x00C9A5FC
static float _surfaceAmbientLighting = 0.0f;        // Binary offsets: (1.0 US and 1.0 EU): 0x00C9A600
static RwColor _surfaceMaterialColor;               // Binary offsets: (1.0 US and 1.0 EU): 0x00C9A5F8
static unsigned int _surfaceRenderFlags = 0;        // Binary offsets: (1.0 US and 1.0 EU): 0x00C9A5F4
static float _surfaceAmbientColorRed = 0.0f;        // Binary offsets: (1.0 US and 1.0 EU): 0x00C9A5D8
static float _surfaceAmbientColorGreen = 0.0f;      // Binary offsets: (1.0 US and 1.0 EU): 0x00C9A5DC
static float _surfaceAmbientColorBlue = 0.0f;       // Binary offsets: (1.0 US and 1.0 EU): 0x00C9A5E0

static D3DMATERIAL9 _currentMaterial;               // Binary offsets: (1.0 US and 1.0 EU): 0x00C98AF8

inline D3DMATERIAL9& GetNativeCurrentMaterial( void )
{
    return _currentMaterial;
}

int __cdecl RwD3D9SetMaterial( const D3DMATERIAL9& material )
{
    D3DMATERIAL9& currentMaterial = GetNativeCurrentMaterial();
    
    if ( memcmp( &currentMaterial, &material, sizeof( D3DMATERIAL9 ) ) != 0 )
    {
        // Update our local copy of the current material.
        currentMaterial = material;

        // Make sure surface rendering resets the material.
        _surfaceRenderFlags = 0;

        int iReturn = GetRenderDevice()->SetMaterial( &material );

        return ( iReturn >= 0 );
    }

    return true;
}

/*=========================================================
    RwD3D9GetMaterial (MTA extension)

    Arguments:
        material - device material data pointer
    Purpose:
        Returns the material that is currently assigned to
        the pipeline.
=========================================================*/
void __cdecl RwD3D9GetMaterial( D3DMATERIAL9& material )
{
    material = GetNativeCurrentMaterial();
}

/*=========================================================
    RpD3D9SetSurfaceProperties

    Arguments:
        matLight - lighting intensity container of the surface
        matColor - surface color
        renderFlags - rendering flags of the current context
    Purpose:
        Sets up the surface of the to-be-rendered geometry
        chunk. This function is generally used for all kinds
        of meshes.
    Binary offsets:
        (1.0 US): 0x007FC4D0
        (1.0 EU): 0x007FC510
=========================================================*/
int __cdecl RpD3D9SetSurfaceProperties( RpMaterialLighting& matLight, RwColor& matColor, unsigned int renderFlags )
{
    // Check whether any material parameter has changed.
    if ( _surfaceDiffuseLighting    == matLight.diffuse &&
         _surfaceAmbientLighting    == matLight.ambient &&
         _surfaceMaterialColor      == matColor &&
         _surfaceRenderFlags        == ( renderFlags & 0x48 ) &&
         _surfaceAmbientColorRed    == GetAmbientColor().r &&
         _surfaceAmbientColorGreen  == GetAmbientColor().g &&
         _surfaceAmbientColorBlue   == GetAmbientColor().b )
    {
        return true;
    }

    // Update current material parameters.
    _surfaceDiffuseLighting     = matLight.diffuse;
    _surfaceAmbientLighting     = matLight.ambient;
    _surfaceMaterialColor       = matColor;
    _surfaceRenderFlags         = ( renderFlags & 0x48 );
    _surfaceAmbientColorRed     = GetAmbientColor().r;
    _surfaceAmbientColorGreen   = GetAmbientColor().g;
    _surfaceAmbientColorBlue    = GetAmbientColor().b;

    // Push the new data onto the GPU.
    D3DMATERIAL9& surfMat = GetInlineSurfaceMaterial();
    
    // Color parameters that decide how data is pushed to the pipeline.
    bool useMaterialColor = false;
    bool putAmbientColor = false;
    bool putEmissiveColor = false;
    bool requireEmissiveColor = true;
    
    bool fillAmbientColorWithMaterialColor = false;
    bool useVertexColor = IS_ANY_FLAG( renderFlags, 0x08 );

    D3DMATERIALCOLORSOURCE ambientMaterialSource = D3DMCS_MATERIAL;
    D3DMATERIALCOLORSOURCE emissiveMaterialSource = D3DMCS_MATERIAL;

    // Calculate the color parameter values.
    if ( IS_ANY_FLAG( renderFlags, 0x40 ) && matColor != 0xFFFFFFFF )
    {
        // Calculate the real vehicle lighting color.
        useMaterialColor = true;

        if ( useVertexColor )
        {
            // Apply ambient color from matColor.
            fillAmbientColorWithMaterialColor = true;
            
            ambientMaterialSource = D3DMCS_COLOR1;

            putEmissiveColor = true;
        }
        else
        {
            // Default to standard pipeline.
            putAmbientColor = true;
        }
    }
    else
    {
        // We do not use the color, only brightness/intensity.
        if ( useVertexColor )
        {
            emissiveMaterialSource = D3DMCS_COLOR1;
        }
        else
        {
            requireEmissiveColor = false;
        }

        putAmbientColor = true;
    }

    // Generate diffuse color (RGBA).
    long double diffuseRed = 1.0f, diffuseGreen = 1.0f, diffuseBlue = 1.0f, diffuseAlpha = 1.0f;

    if ( useMaterialColor )
    {
        diffuseRed      *= (long double)matColor.r / 255.0f;
        diffuseGreen    *= (long double)matColor.g / 255.0f;
        diffuseBlue     *= (long double)matColor.b / 255.0f;
        diffuseAlpha    *= (long double)matColor.a / 255.0f;
    }

    // Modulate the diffuse color with its intensity.
    diffuseRed *= matLight.diffuse;
    diffuseGreen *= matLight.diffuse;
    diffuseBlue *= matLight.diffuse;

    // Now put it into the material.
    surfMat.Diffuse.r = (float)diffuseRed;  // convert to GPU precision.
    surfMat.Diffuse.g = (float)diffuseGreen;
    surfMat.Diffuse.b = (float)diffuseBlue;
    surfMat.Diffuse.a = (float)diffuseAlpha;

    // We ignore the specular color entirely.

    // Execute post-diffuse parameters.
    {
        DWORD gpuAmbientColor = 0xFFFFFFFF;

        if ( fillAmbientColorWithMaterialColor )
        {
            gpuAmbientColor = matColor.ToD3DColor();
        }

        RwD3D9SetRenderState( D3DRS_AMBIENT, gpuAmbientColor );
        RwD3D9SetRenderState( D3DRS_COLORVERTEX, useVertexColor );

        RwD3D9SetRenderState( D3DRS_AMBIENTMATERIALSOURCE, ambientMaterialSource );
        RwD3D9SetRenderState( D3DRS_EMISSIVEMATERIALSOURCE, emissiveMaterialSource );
    }

    // Generate ambient color (only RGB).
    long double ambientRed = GetAmbientColor().r, ambientGreen = GetAmbientColor().g, ambientBlue = GetAmbientColor().b;

    if ( useMaterialColor )
    {
        ambientRed      *= (long double)matColor.r / 255.0f;
        ambientGreen    *= (long double)matColor.g / 255.0f;
        ambientBlue     *= (long double)matColor.b / 255.0f;
    }

    // Modulate the ambient color with its intensity.
    ambientRed *= matLight.ambient;
    ambientGreen *= matLight.ambient;
    ambientBlue *= matLight.ambient;

    // Decide where to put the ambient color.
    float gpuAmbientRed = (float)ambientRed;    // convert to GPU precision.
    float gpuAmbientGreen = (float)ambientGreen;
    float gpuAmbientBlue = (float)ambientBlue;

    if ( putAmbientColor )
    {
        surfMat.Ambient.r = gpuAmbientRed;
        surfMat.Ambient.g = gpuAmbientGreen;
        surfMat.Ambient.b = gpuAmbientBlue;
    }
    else
    {
        // Reset ambient color.
        surfMat.Ambient.r = 0.0f;
        surfMat.Ambient.g = 0.0f;
        surfMat.Ambient.b = 0.0f;
    }

    // Only update emissive color if it is truly required.
    if ( requireEmissiveColor )
    {
        if ( putEmissiveColor )
        {
            surfMat.Emissive.r = gpuAmbientRed;
            surfMat.Emissive.g = gpuAmbientGreen;
            surfMat.Emissive.b = gpuAmbientBlue;
        }
        else
        {
            // Reset color as we are not using it.
            surfMat.Emissive.r = 0.0f;
            surfMat.Emissive.g = 0.0f;
            surfMat.Emissive.b = 0.0f;
        }
    }

    return RwD3D9SetMaterial( surfMat );
}

/*=========================================================
    RpD3D9InitializeMaterialEnvironment

    Purpose:
        Sets up runtime material parameters that are
        related to GPU device states.
=========================================================*/
void RpD3D9InitializeMaterialEnvironment( void )
{
    // Initialize the current material.
    GetRenderDevice_Native()->GetMaterial( &GetNativeCurrentMaterial() );

    // Initialize surface environment.
    _surfaceDiffuseLighting = 0.0f;
    _surfaceAmbientLighting = 0.0f;
    _surfaceAmbientColorRed = 0.0f;
    _surfaceAmbientColorGreen = 0.0f;
    _surfaceAmbientColorBlue = 0.0f;

    _surfaceRenderFlags = 0;
    _surfaceMaterialColor = 0x00000000;
}

/*=========================================================
    RwD3D9SetStreamSource

    Arguments:
        streamIndex - index of the vertex stream to update
        streamPtr - GPU data pointer to the stream data
        streamOffset - offset into the vertex stream to set
        streamStride - stride for the vertex stream
    Purpose:
        Updates the Direct3D 9 device vertex stream buffers.
        This function caches the current stream buffers so that
        unnecessary updates to the GPU are avoided.
    Binary offsets:
        (1.0 US): 0x007FA030
        (1.0 EU): 0x007FA070
=========================================================*/
void __cdecl RwD3D9SetStreamSource( DWORD streamIndex, IDirect3DVertexBuffer9 *streamPtr, size_t streamOffset, size_t streamStride )
{
    RwVertexStreamStateManager::stateAddress address;
    address.streamNumber = streamIndex;

    RwD3D9StreamInfo info;
    info.m_vertexBuf = streamPtr;
    info.m_offset = streamOffset;
    info.m_stride = streamStride;

    g_vertexStreamStateManager.SetDeviceStateChecked( address, info );
}

/*=========================================================
    RwD3D9SetStreams

    Arguments:
        streamInfo - descriptor of stream to set as current
        useOffset - boolean whether to use offseting into
                    the stream buffers
    Purpose:
        Updates the Direct3D 9 device vertex stream buffers.
        This function caches the current stream buffers so that
        unnecessary updates to the GPU are avoided.
    Binary offsets:
        (1.0 US): 0x007FA090
        (1.0 EU): 0x007FA0D0
=========================================================*/
void __cdecl RwD3D9SetStreams( const RwD3D9Streams& streams, int useOffset )
{
    IDirect3DDevice9 *renderDevice = GetRenderDevice_Native();

    // Loop through all stream interfaces (there can be maximally two)
    for ( unsigned int n = 0; n < 2; n++ )
    {
        const RwD3D9StreamInfo& info = streams.streams[n];

        RwVertexStreamStateManager::stateAddress address;
        address.streamNumber = n;

        // The caller decides whether we use byte offsets in this stream.
        size_t streamOffset = ( useOffset ) ? info.m_offset : 0;

        // Decide whether the stream interface needs updating at all.
        if ( IDirect3DVertexBuffer9 *vertexBuf = info.m_vertexBuf )
        {
            RwD3D9SetStreamSource( n, vertexBuf, streamOffset, info.m_stride );
        }
        else
        {
            // Clear the vertex stream if it has been unloaded in the given streams info.
            if ( g_vertexStreamStateManager.GetDeviceState( address ).m_vertexBuf )
            {
                RwD3D9SetStreamSource( n, NULL, 0, 0 );
            }
        }
    }
}

/*=========================================================
    RwD3D9GetStreams (MTA extension)

    Arguments:
        streamInfo - pointer to write streams information into
    Purpose:
        Retrieves the current streams information that is
        active at the driver.
=========================================================*/
void __cdecl RwD3D9GetStreams( RwD3D9Streams& streamInfo )
{
    for ( unsigned int n = 0; n < 2; n++ )
    {
        RwVertexStreamStateManager::stateAddress address;
        address.streamNumber = n;

        streamInfo.streams[n] = g_vertexStreamStateManager.GetDeviceState( address );
    }
}

/*=========================================================
    RwD3D9SetCurrentPixelShader

    Arguments:
        pixelShader - device fragment shader pointer
    Purpose:
        Sets the active fragment shader that is used for
        rendering video data.
    Binary offsets:
        (1.0 US): 0x007F9FF0
        (1.0 EU): 0x007FA030
=========================================================*/
inline IDirect3DPixelShader9*& GetCurrentPixelShader( void )
{
    return *(IDirect3DPixelShader9**)0x008E244C;
}

IDirect3DPixelShader9* __cdecl RwD3D9GetCurrentPixelShader( void )
{
    return GetCurrentPixelShader();
}

void __cdecl RwD3D9SetCurrentPixelShader( IDirect3DPixelShader9 *pixelShader )
{
    IDirect3DPixelShader9*& currentPixelShader = GetCurrentPixelShader();

    if ( pixelShader != currentPixelShader )
    {
        bool success = GetRenderDevice_Native()->SetPixelShader( pixelShader ) >= 0;

        currentPixelShader = ( success ) ? pixelShader : (IDirect3DPixelShader9*)-1;
    }
}

void __cdecl RwD3D9UnsetPixelShader( void )
{
    IDirect3DPixelShader9*& currentPixelShader = GetCurrentPixelShader();

    if ( currentPixelShader )
    {
        currentPixelShader = NULL;

        GetRenderDevice_Native()->SetPixelShader( NULL );
    }
}

/*=========================================================
    RwD3D9SetCurrentVertexShader

    Arguments:
        vertexShader - device vertex shader pointer
    Purpose:
        Sets the active vertex shader that is used for
        blending video data.
    Binary offsets:
        (1.0 US): 0x007F9FB0
        (1.0 EU): 0x007F9FF0
=========================================================*/
inline IDirect3DVertexShader9*& GetCurrentVertexShader( void )
{
    return *(IDirect3DVertexShader9**)0x008E2448;
}

IDirect3DVertexShader9* __cdecl RwD3D9GetCurrentVertexShader( void )
{
    return GetCurrentVertexShader();
}

void __cdecl RwD3D9SetCurrentVertexShader( IDirect3DVertexShader9 *vertexShader )
{
    IDirect3DVertexShader9*& currentVertexShader = GetCurrentVertexShader();

    if ( currentVertexShader != vertexShader )
    {
        bool success = GetRenderDevice_Native()->SetVertexShader( vertexShader ) >= 0;

        currentVertexShader = ( success ) ? vertexShader : (IDirect3DVertexShader9*)-1;
    }
}

/*=========================================================
    RwD3D9SetCurrentIndexBuffer

    Arguments:
        indexBuf - device index buffer pointer
    Purpose:
        Sets the index buffer that holds pointers to vertice
        data inside of the vertex streams.
    Binary offsets:
        (1.0 US): 0x007FA1C0
        (1.0 EU): 0x007FA200
=========================================================*/
inline IDirect3DIndexBuffer9*& GetCurrentIndexBuffer( void )
{
    return *(IDirect3DIndexBuffer9**)0x008E2450;
}

IDirect3DIndexBuffer9* __cdecl RwD3D9GetCurrentIndexBuffer( void )
{
    return GetCurrentIndexBuffer();
}

void __cdecl RwD3D9SetCurrentIndexBuffer( IDirect3DIndexBuffer9 *indexBuf )
{
    IDirect3DIndexBuffer9*& curIndexBuf = GetCurrentIndexBuffer();

    if ( curIndexBuf != indexBuf )
    {
        curIndexBuf = indexBuf;

        GetRenderDevice_Native()->SetIndices( indexBuf );
    }
}

/*=========================================================
    RwD3D9SetCurrentVertexDeclaration

    Arguments:
        vertexDecl - device vertex declaration pointer
    Purpose:
        Sets the active device vertex declaration. It specifies
        how the vertex data is formated.
    Binary offsets:
        (1.0 US): 0x007F9F70
        (1.0 EU): 0x007F9FB0
=========================================================*/
inline IDirect3DVertexDeclaration9*& GetCurrentVertexDeclaration( void )
{
    return *(IDirect3DVertexDeclaration9**)0x008E2444;
}

inline DWORD& GetCurrentFixedVertexFunction( void )
{
    return *(DWORD*)0x008E2440;
}

IDirect3DVertexDeclaration9* __cdecl RwD3D9GetCurrentVertexDeclaration( void )
{
    return GetCurrentVertexDeclaration();
}

void __cdecl RwD3D9SetCurrentVertexDeclaration( IDirect3DVertexDeclaration9 *vertexDecl )
{
    IDirect3DVertexDeclaration9*& curVertexDecl = GetCurrentVertexDeclaration();

    if ( curVertexDecl != vertexDecl )
    {
        GetCurrentFixedVertexFunction() = -1;

        bool success = GetRenderDevice_Native()->SetVertexDeclaration( vertexDecl ) >= 0;

        curVertexDecl = ( success ) ? vertexDecl : (IDirect3DVertexDeclaration9*)-1;

#if 0
        D3DVERTEXELEMENT9 elements[MAXD3DDECLLENGTH];
        UINT numElements;

        vertexDecl->GetDeclaration( elements, &numElements );

        for ( unsigned int n = 0; n < numElements; n++ )
        {
            D3DVERTEXELEMENT9& element = elements[n];

            __asm nop
        }
#endif
    }
}

void __cdecl RpD3D9DrawIndexedPrimitive( D3DPRIMITIVETYPE primitiveType, INT baseVertexIndex, UINT minVertexIndex, UINT numVertice, UINT startIndex, UINT primCount )
{
    // Update the render and texture states before drawing.
    RwD3D9ApplyDeviceStates();

    GetRenderDevice_Native()->DrawIndexedPrimitive( primitiveType, baseVertexIndex, minVertexIndex, numVertice, startIndex, primCount );
}

void __cdecl RpD3D9DrawPrimitive( D3DPRIMITIVETYPE primitiveType, UINT startVertex, UINT numPrimitives )
{
    // Update the render and texture states before drawing.
    RwD3D9ApplyDeviceStates();

    GetRenderDevice_Native()->DrawPrimitive( primitiveType, startVertex, numPrimitives );
}

/*=========================================================
    RwD3D9EnableClippingIfNeeded

    Arguments:
        renderObject - the RenderWare object to clip
        renderType - type of the RenderWare object
    Purpose:
        Decides whether the given RenderWare object can be clipped.
        Polygons are clipped when they are outside of the screen.
        A simple frustum check decides this.
    Binary offsets:
        (1.0 US): 0x007F9F70
        (1.0 EU): 0x007F9FB0
=========================================================*/
inline void RwD3D9EnableClippingIfNeeded( RwObject *renderObject, eRwType renderType )
{
    bool isObjectVisible = false;
    RwCamera *currentCam = RenderWare::GetInterface()->m_renderCam;

    if ( renderType == RW_ATOMIC )
    {
        isObjectVisible = RwD3D9CameraIsSphereFullyInsideFrustum( currentCam, ((RpAtomic*)renderObject)->GetWorldBoundingSphere() );
    }
    else
    {
        // Unknown type rendering!
        isObjectVisible = RwD3D9CameraIsBBoxFullyInsideFrustum( currentCam, ((CVector*) ((char*)renderObject + 96) ) );
    }

    // Clip polygons if the object is not visible.
    // todo: fix this; it bugs for old GPUs!
    //RwD3D9SetRenderState( D3DRS_CLIPPING, !isObjectVisible );
    RwD3D9SetRenderState( D3DRS_CLIPPING, true );
}

/*=========================================================
    HOOK_DefaultAtomicRenderingCallback

    Purpose:
        Sets up the atomic pipeline, prepares the device information
        and initializes the material, atomic and specular material
        custom pools.
    Binary offsets:
        (1.0 US): 0x00756DF0
        (1.0 EU): 0x00756E40
=========================================================*/
struct GenericVideoPassRender
{
    __forceinline GenericVideoPassRender( unsigned int& _renderFlags ) : renderFlags( _renderFlags )
    {
        // Store some render states.
        RwD3D9GetRenderState( D3DRS_DITHERENABLE, ditheringEnabled );
        RwD3D9GetRenderState( D3DRS_SHADEMODE, shadeMode );

        // Change render states.
        RwD3D9RenderStateSetVertexAlphaEnabled( false );

        RwD3D9SetRenderState( D3DRS_TEXTUREFACTOR, 0xFF000000 );
        RwD3D9SetRenderState( D3DRS_DITHERENABLE, FALSE );
        RwD3D9SetRenderState( D3DRS_SHADEMODE, D3DSHADE_FLAT );

        RwD3D9SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
        RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
        RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
        RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
        RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );

        RwD3D9SetTexture( 0, NULL );

        // Keep track of alpha modulation status.
        hasAlphaModulation = false;
    }

    __forceinline ~GenericVideoPassRender( void )
    {
        // Restore some render states.
        RwD3D9SetRenderState( D3DRS_SHADEMODE, shadeMode );
        RwD3D9SetRenderState( D3DRS_DITHERENABLE, ditheringEnabled );
    }

    __forceinline void OnRenderPass( RwRenderPass *rtPass )
    {
        RpMaterial *curMat = rtPass->m_useMaterial;

        if ( IS_FLAG( renderFlags, 0x84 ) && RwTextureHasAlpha( curMat->texture ) )
        {
            RwD3D9SetTexture( curMat->texture, 0 );

            if ( !hasAlphaModulation )
            {
                RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );

                hasAlphaModulation = true;
            }
        }
        else
        {
            if ( hasAlphaModulation )
            {
                RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );

                hasAlphaModulation = false;

                RwD3D9SetTexture( NULL, 0 );
            }
        }
    }
    
    __forceinline bool IsProperlyDepthSorted( void )
    {
        // Since this template is used for any world model, we assume by default that they
        // are not properly depth sorted, as seen on the GTA:SA trees. We ask the mod whether
        // it wants to have these instances alpha fixed.
        // todo: since this process is expensive, it'd be cool if we somehow knew which
        // models are totally opaque using a geometry flag.
        return !AlphaSort_IsEnabled();
    }

    __forceinline bool CanRenderOpaque( void )
    {
        return AlphaSort_CanRenderOpaquePrimitives();
    }

    __forceinline bool CanRenderTranslucent( void )
    {
        return AlphaSort_CanRenderTranslucentPrimitives();
    }

    __forceinline bool CanRenderDepth( void )
    {
        return AlphaSort_CanRenderDepthLayer();
    }

    __forceinline unsigned int GetRenderAlphaClamp( void )
    {
        return GetWorldObjectAlphaClamp();
    }

    unsigned int& renderFlags;
    DWORD ditheringEnabled;
    DWORD shadeMode;
    bool hasAlphaModulation;
};

struct AlphaTexturedVideoPassRender
{
    __forceinline AlphaTexturedVideoPassRender( unsigned int& _renderFlags, DWORD& _lightValue ) : renderFlags( _renderFlags ), lightValue( _lightValue )
    {
        // Store current vertex alpha status.
        hasVertexAlpha = RwD3D9RenderStateIsVertexAlphaEnabled() ? true : false;

        // Store the current state flags
        // These are used for optimization purposes.
        // The initial value should be a flag that is not used and invalid.
        curStateFlags = 0x80000000;
    }

    __forceinline void TerminateSecondStage( void )
    {
        if ( curStateFlags == 0x03 )
        {
            RwD3D9SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
            RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
        }
    }

    __forceinline ~AlphaTexturedVideoPassRender( void )
    {
        TerminateSecondStage();
    }

    __forceinline void OnRenderPass( RwRenderPass *rtPass )
    {
        RpMaterial *curMat = rtPass->m_useMaterial;

        // Update device vertex alpha status.
        // Did Rockstar even know how to fully utilize RenderWare?
        bool needsVertexAlpha = RwD3D9IsVertexAlphaRenderingRequiredEx( rtPass, curMat );

        if ( needsVertexAlpha != hasVertexAlpha )
        {
            hasVertexAlpha = needsVertexAlpha;

            RwD3D9RenderStateSetVertexAlphaEnabled( needsVertexAlpha );
        }

        // Update surface properties.
        unsigned int stateFlags = 0;

        if ( !RwD3D9UpdateRenderPassSurfaceProperties( rtPass, lightValue, curMat, renderFlags ) && IS_ANY_FLAG( renderFlags, 0x40 ) && curMat->color != 0xFFFFFFFF )
        {
            stateFlags |= 0x02;

            RwD3D9SetRenderState( D3DRS_TEXTUREFACTOR, curMat->color.ToD3DColor() );
        }

        // Update texture status.
        RwTexture *matTex = curMat->texture;

        if ( matTex && IS_ANY_FLAG( renderFlags, 0x84 ) )
        {
            RwD3D9SetTexture( matTex, 0 );

            stateFlags |= 0x01;
        }
        else
            RwD3D9SetTexture( NULL, 0 );

        // Change render states depending on the current status that we set above.
        if ( stateFlags != curStateFlags )
        {
            bool requireSecondStage = false;

            if ( IS_ANY_FLAG( stateFlags, 0x01 ) )
            {
                if ( !IS_ANY_FLAG( curStateFlags, 0x01 ) )
                {
                    RwD3D9SetTexturedRenderStates( true );
                }

                if ( IS_ANY_FLAG( stateFlags, 0x02 ) )
                {
                    RwD3D9SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_MODULATE );
                    RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT );
                    RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TFACTOR );

                    RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
                    RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_CURRENT );
                    RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );

                    requireSecondStage = true;
                }
            }
            else
            {
                if ( IS_ANY_FLAG( stateFlags, 0x02 ) )
                {
                    RwD3D9SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
                    RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
                    RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );

                    RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
                    RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
                    RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );
                }
                else
                {
                    RwD3D9SetTexturedRenderStates( false );
                }
            }

            if ( !requireSecondStage )
                TerminateSecondStage();

            // We changed state.
            curStateFlags = stateFlags;
        }
    }

    __forceinline bool IsProperlyDepthSorted( void )
    {
        // Since this template is used for any world model, we assume by default that they
        // are not properly depth sorted, as seen on the GTA:SA trees. We ask the mod whether
        // it wants to have these instances alpha fixed.
        // todo: since this process is expensive, it'd be cool if we somehow knew which
        // models are totally opaque using a geometry flag.
        return !AlphaSort_IsEnabled();
    }

    __forceinline bool CanRenderOpaque( void )
    {
        return AlphaSort_CanRenderOpaquePrimitives();
    }

    __forceinline bool CanRenderTranslucent( void )
    {
        return AlphaSort_CanRenderTranslucentPrimitives();
    }

    __forceinline bool CanRenderDepth( void )
    {
        return AlphaSort_CanRenderDepthLayer();
    }

    __forceinline unsigned int GetRenderAlphaClamp( void )
    {
        return GetWorldObjectAlphaClamp();
    }

    unsigned int& renderFlags;
    DWORD& lightValue;
    bool hasVertexAlpha;
    unsigned int curStateFlags;
};

// todo: is there even a "default" rendering callback?
// after all, every pipeline is stand-alone.
void __cdecl HOOK_DefaultAtomicRenderingCallback( RwRenderCallbackTraverse *rtnative, RwObject *renderObject, eRwType renderType, unsigned int renderFlags )
{
    if ( !RenderCallbacks::IsRenderingEnabled() )
        return;

    RwRenderCallbackTraverseImpl *rtinfo = &rtnative->m_impl;

    // Make sure we are not rendering using any pixel shader.
    RwD3D9UnsetPixelShader();

    // Set clipping status.
    RwD3D9EnableClippingIfNeeded( renderObject, renderType );

    // Update the current index buffer (only if set)
    if ( IDirect3DIndexBuffer9 *indexBuf = rtinfo->m_indexBuffer )
        RwD3D9SetCurrentIndexBuffer( indexBuf );

    // Update the vertex streams.
    RwD3D9SetStreams( rtinfo->m_vertexStreams, rtinfo->m_useOffsets );

    // Set the vertex type.
    RwD3D9SetCurrentVertexDeclaration( rtinfo->m_vertexDecl );

    // Perform the rendering.
    {
        // Make sure we tell rasterizazion contexts about our rendering object.
        // This is done for instance-based optimizations.
        RwD3D9OnRenderingContextEstablish( renderObject );

        // Do we want vertex alpha?
        // Kind of an important flag here.
        DWORD lightValue;

        bool enableAlpha = RwD3D9IsAlphaRenderingRequired( renderFlags, lightValue );

        if ( enableAlpha )
        {
            _RenderVideoDataGeneric( rtinfo, GenericVideoPassRender( renderFlags ) );
        }
        else
        {
            _RenderVideoDataGeneric( rtinfo, AlphaTexturedVideoPassRender( renderFlags, lightValue ) );
        }

        // Tell rasterizers that the contextual rendering phase ended.
        RwD3D9OnRenderingContextBreak();
    }
}

/*=========================================================
    RwD3D9InitializeCurrentStates

    Purpose:
        Sets up the RenderWare Direct3D 9 runtime with the
        current device states.
    Note:
        This function has been inlined into device initialization
        and device reset functions.
=========================================================*/
void RwD3D9InitializeCurrentStates( void )
{
    // The_GTA: fixed current-ness of device states by actually querying the device.

    // Set up the current renderstates.
    g_renderStateManager.Initialize();

    // Set up the current TSS values.
    g_textureStageStateManager.Initialize();

    // Set up the sampler states array.
    g_samplerStateManager.Initialize();

    // Initialize material environment.
    RpD3D9InitializeMaterialEnvironment();

    // MTA extension: set up different state managers.
    g_transformationStateManager.Initialize();
    g_vertexStreamStateManager.Initialize();
}

/*=========================================================
    RwD3D9InitializePipelineStates

    Purpose:
        Called when the usage of expensive device resources
        should be reset.
    Binary offsets:
        (1.0 US): 0x007F6B00
        (1.0 EU): 0x007F6B40
=========================================================*/
void __cdecl RwD3D9InitializePipelineStates( void )
{
    // Invalidate runtime resources.
    GetCurrentFixedVertexFunction() = 0xFFFFFFFF;
    GetCurrentVertexDeclaration() = (IDirect3DVertexDeclaration9*)0xFFFFFFFF;
    GetCurrentVertexShader() = (IDirect3DVertexShader9*)0xFFFFFFFF;
    GetCurrentPixelShader() = (IDirect3DPixelShader9*)0xFFFFFFFF;
    GetCurrentIndexBuffer() = (IDirect3DIndexBuffer9*)0xFFFFFFFF;

    // Invalidate vertex streams.
    g_vertexStreamStateManager.Invalidate();
}

/*=========================================================
    RwD3D9ReleaseDeviceResources

    Purpose:
        Called when the runtime wants to release all resources
        that it has been using inside the Direct3D 9 driver.
    Binary offsets:
        (1.0 US): 0x007F7F70
        (1.0 EU): 0x007F7FB0
=========================================================*/
inline IDirect3DSurface9*& GetMainRenderTarget( void )
{
    return *(IDirect3DSurface9**)0x00C97C30;
}

inline IDirect3DSurface9** GetDeviceRenderTargets( void )
{
    return (IDirect3DSurface9**)0x00C98090;
}

void __cdecl RwD3D9SetRenderTarget( DWORD targetIndex, IDirect3DSurface9 *renderSurface )
{
    IDirect3DSurface9*& currentSurface = GetDeviceRenderTargets()[ targetIndex ];

    if ( currentSurface != renderSurface )
    {
        currentSurface = renderSurface;

        GetRenderDevice()->SetRenderTarget( targetIndex, renderSurface );
    }
}

inline IDirect3DSurface9*& GetMainDepthStencilSurface( void )
{
    return *(IDirect3DSurface9**)0x00C97C2C;
}

inline IDirect3DSurface9*& GetCurrentDepthStencilSurface( void )
{
    return *(IDirect3DSurface9**)0x00C9808C;
}

void __cdecl RwD3D9SetDepthStencilSurface( IDirect3DSurface9 *depthStencilSurface )
{
    IDirect3DSurface9*& currentSurface = GetCurrentDepthStencilSurface();

    if ( currentSurface != depthStencilSurface )
    {
        currentSurface = depthStencilSurface;

        GetRenderDevice()->SetDepthStencilSurface( depthStencilSurface );
    }
}

inline void RwD3D9InitializeDeviceCommon( void )
{
    RwD3D9InitializePipelineStates();

    // We do not require transformation free list management anymore (US offset: 0x007F7FB1)

    // Invalidate transformations.
    g_transformationStateManager.Invalidate();
}

int __cdecl RwD3D9ReleaseDeviceResources( void )
{
    RwD3D9InitializeDeviceCommon();

    // Set resources to blank.
    IDirect3DDevice9 *renderDevice = GetRenderDevice_Native();

    for ( unsigned int n = 0; n < 8; n++ )
    {
        renderDevice->SetTexture( n, NULL );
    }

    renderDevice->SetIndices( NULL );

    g_vertexStreamStateManager.ClearDevice();

    renderDevice->SetPixelShader( NULL );
    renderDevice->SetVertexDeclaration( NULL );
    renderDevice->SetVertexShader( NULL );

    // Reset render targets.
    for ( unsigned int n = 0; n < 4; n++ )
    {
        IDirect3DSurface9 *defaultSurface = NULL;

        if ( n == 0 )
        {
            defaultSurface = GetMainRenderTarget();
        }

        RwD3D9SetRenderTarget( n, defaultSurface );
    }

    RwD3D9SetDepthStencilSurface( GetMainDepthStencilSurface() );
    
    // Close remaining management structures.
    switch( pGame->GetGameVersion() )
    {
    case VERSION_EU_10:
        ((void (__cdecl*)( void ))0x007FB630)();
        ((void (__cdecl*)( void ))0x0080DFF0)();
        ((void (__cdecl*)( void ))0x007F5880)();

        break;
    case VERSION_US_10:
        ((void (__cdecl*)( void ))0x007FB5F0)();
        ((void (__cdecl*)( void ))0x0080DFB0)();
        ((void (__cdecl*)( void ))0x007F5840)();
        
        break;
    }

    ((void (__cdecl*)( void ))0x004CB640)();

    RwFreeListPurgeAll();
    return true;
}

/*=========================================================
    RwD3D9AcquireD3DDevice

    Purpose:
        Initializes the Direct3D device environment and returns
        the pointer to it.
    Binary offsets:
        (1.0 US): 0x007F9D50
        (1.0 EU): 0x007F9D90
=========================================================*/
IDirect3DDevice9* __cdecl RwD3D9AcquireD3DDevice( void )
{
    // Initialize stuff.
    RwD3D9InitializeDeviceCommon();

    g_transformationStateManager.Initialize();
    g_vertexStreamStateManager.Initialize();

    return GetRenderDevice();
}

/*=========================================================
    HOOK_InitDeviceSystem

    Purpose:
        Sets up the atomic pipeline, prepares the device information
        and initializes the material, atomic and specular material
        custom pools.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x005DA020
=========================================================*/
D3DLIGHT9& RenderWare::GetSpecialVehicleLight( void )
{
    return *(D3DLIGHT9*)0x00C02CB0;
}

static int __cdecl HOOK_InitDeviceSystem( void )
{
    RwPipeline *defPipeline;

    // Pah, might analyze those chunks way later :P
    // I am here because of the pools!
    __asm
    {
        // Initialize pipelines
        mov eax,0x005D9F80
        call eax
        mov defPipeline,eax
    }

    if ( !defPipeline )
        return 0;

    *(RwPipeline**)0x00C02D24 = defPipeline;

    int iReturn;    // did you know that "iRet" is a reserved value in VC++ ASM?

    __asm
    {
        // Initialize device information
        mov eax,0x005D8980
        call eax
        mov iReturn,eax
    }

    if ( !iReturn )
        return 0;

    // Initialize the special vehicle light interface.
    memset( &RenderWare::GetSpecialVehicleLight(), 0, sizeof(D3DLIGHT9) );

    // Finally initialize the pools!
    RenderWare::GetEnvMapMaterialPool() = new CEnvMapMaterialPool;
    RenderWare::GetEnvMapAtomicPool() = new CEnvMapAtomicPool;
    RenderWare::GetSpecMapMaterialPool() = new CSpecMapMaterialPool;
    return 1;
}

void RenderWarePipeline_Init( void )
{
    // Depending on game version
    switch( pGame->GetGameVersion() )
    {
    case VERSION_EU_10:
        HookInstall( 0x007FC310, (DWORD)RwD3D9SetRenderState, 5 );
        HookInstall( 0x007FC360, (DWORD)RwD3D9GetRenderState, 5 );
        HookInstall( 0x007FC380, (DWORD)RwD3D9SetTextureStageState, 5 );
        HookInstall( 0x007FC3E0, (DWORD)RwD3D9GetTextureStageState, 5 );
        HookInstall( 0x007FC400, (DWORD)RwD3D9SetSamplerState, 5 );
        HookInstall( 0x007FC440, (DWORD)RwD3D9GetSamplerState, 5 );
        HookInstall( 0x007FC470, (DWORD)RwD3D9SetMaterial, 5 );
        HookInstall( 0x007FC240, (DWORD)RwD3D9ApplyDeviceStates, 5 );
        HookInstall( 0x007FA070, (DWORD)RwD3D9SetStreamSource, 5 );
        HookInstall( 0x007FA0D0, (DWORD)RwD3D9SetStreams, 5 );
        HookInstall( 0x00756E40, (DWORD)HOOK_DefaultAtomicRenderingCallback, 5 );
        HookInstall( 0x007FCB00, (DWORD)RwD3D9InitializeDeviceStates, 5 );
        HookInstall( 0x007FD140, (DWORD)RwD3D9ResetDeviceStates, 5 );
        HookInstall( 0x007F6B40, (DWORD)RwD3D9InitializePipelineStates, 5 );
        HookInstall( 0x007F7FB0, (DWORD)RwD3D9ReleaseDeviceResources, 5 );
        HookInstall( 0x007F9D90, (DWORD)RwD3D9AcquireD3DDevice, 5 );
        //HookInstall( 0x007FC510, (DWORD)RpD3D9SetSurfaceProperties, 5 );
        break;
    case VERSION_US_10:
        HookInstall( 0x007FC2D0, (DWORD)RwD3D9SetRenderState, 5 );
        HookInstall( 0x007FC320, (DWORD)RwD3D9GetRenderState, 5 );
        HookInstall( 0x007FC340, (DWORD)RwD3D9SetTextureStageState, 5 );
        HookInstall( 0x007FC3A0, (DWORD)RwD3D9GetTextureStageState, 5 );
        HookInstall( 0x007FC3C0, (DWORD)RwD3D9SetSamplerState, 5 );
        HookInstall( 0x007FC400, (DWORD)RwD3D9GetSamplerState, 5 );
        HookInstall( 0x007FC430, (DWORD)RwD3D9SetMaterial, 5 );
        HookInstall( 0x007FC200, (DWORD)RwD3D9ApplyDeviceStates, 5 );
        HookInstall( 0x007FA030, (DWORD)RwD3D9SetStreamSource, 5 );
        HookInstall( 0x007FA090, (DWORD)RwD3D9SetStreams, 5 );
        HookInstall( 0x00756DF0, (DWORD)HOOK_DefaultAtomicRenderingCallback, 5 );
        HookInstall( 0x007FCAC0, (DWORD)RwD3D9InitializeDeviceStates, 5 );
        HookInstall( 0x007FD100, (DWORD)RwD3D9ResetDeviceStates, 5 );
        HookInstall( 0x007F6B00, (DWORD)RwD3D9InitializePipelineStates, 5 );
        HookInstall( 0x007F7F70, (DWORD)RwD3D9ReleaseDeviceResources, 5 );
        HookInstall( 0x007F9D50, (DWORD)RwD3D9AcquireD3DDevice, 5 );
        //HookInstall( 0x007FC4D0, (DWORD)RpD3D9SetSurfaceProperties, 5 );
        break;
    }

    HookInstall( 0x005DA020, (DWORD)HOOK_InitDeviceSystem, 5 );

    RenderBucket::Initialize();
}

void RenderWarePipeline_Shutdown( void )
{
    RenderBucket::Shutdown();
}