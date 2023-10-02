/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRenderWareSA.lighting.h
*  PURPOSE:     GTA:SA lighting management
*               Extended to properly support dynamic lighting
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _RENDERWARE_LIGHTING_MANAGEMENT_
#define _RENDERWARE_LIGHTING_MANAGEMENT_

// Lighting system exports.
int __cdecl     RpD3D9LightsEqual           ( const D3DLIGHT9& left, const D3DLIGHT9& right );
int __cdecl     RpD3D9SetLight              ( int lightIndex, const D3DLIGHT9& lightStruct );
int __cdecl     RpD3D9GetLight              ( int lightIndex, D3DLIGHT9& lightStruct );
int __cdecl     RpD3D9DirLightEnable        ( RpLight *light );
int __cdecl     RpD3D9LocalLightEnable      ( RpLight *light );
int __cdecl     RpD3D9EnableLight           ( int lightIndex, int phase );
int __cdecl     RpD3D9GetLightEnable        ( int lightIndex );
int __cdecl     RpD3D9GlobalLightsEnable    ( unsigned char flags );
bool            RpD3D9GlobalLightingPrePass ( void );
void            RpD3D9CacheLighting         ( void );
void __cdecl    RpD3D9ResetLightStatus      ( void );
void __cdecl    RpD3D9LightingOnDeviceReset ( void );
void __cdecl    RpD3D9EnableLights          ( bool enable, int unused );

// API exports.
void __cdecl                RpLightSetAttenuation   ( RpLight *light, const CVector& atten );
const CVector& __cdecl      RpLightGetAttenuation   ( const RpLight *light );

void __cdecl                RpLightSetFalloff       ( RpLight *light, float falloff );
float __cdecl               RpLightGetFalloff       ( const RpLight *light );

void __cdecl                RpLightSetLightIndex    ( RpLight *light, int index );
int __cdecl                 RpLightGetLightIndex    ( const RpLight *light );

// Utility namespace.
namespace D3D9Lighting
{
    void                SetGlobalLightingAlwaysEnabled  ( bool enabled );
    bool                IsGlobalLightingAlwaysEnabled   ( void );

    void                SetLocalLightingAlwaysEnabled   ( bool enabled );
    bool                IsLocalLightingAlwaysEnabled    ( void );

    void                SetShaderLightingMode           ( eShaderLightingMode mode );
    eShaderLightingMode GetShaderLightingMode           ( void );
};

struct nativeLightInfo  //size: 108 bytes
{
    D3DLIGHT9   native;     // 0
    int         active;     // 104

    inline bool operator == ( const nativeLightInfo& right ) const
    {
        return RpD3D9LightsEqual( native, right.native ) != 0;
    }
};

namespace D3D9Lighting
{
    struct lightIndexArrayManager
    {
        inline void InitField( int& lightIndex )
        {
            lightIndex = 0;
        }
    };

    struct availableLightIndexArrayManager
    {
        inline void InitField( int& lightIndex )
        {
            return;
        }
    };

    struct nativeLightInfoArrayManager
    {
        inline void InitField( nativeLightInfo& info )
        {
            // By default, lights get added to phase 0.
            // Phase 0 sets them to "not rendered" for their first cycle occurance.
            // This phase index describes on which list (primary or swap) the light
            // resides on. Sort of like a scan-code algorithm (as seen in the
            // entity world streaming, CEntitySAInterface::m_scanCode).
            info.active = false;
        }
    };

    struct deviceLightInfoArrayManager
    {
        inline void InitField( D3DLIGHT9& lightStruct )
        {
            return;
        }
    };

    typedef growableArray <int, 8, 0x103050D, lightIndexArrayManager, unsigned int> lightIndexArray;
    typedef growableArray <int, 0x100, 0x103050D, availableLightIndexArrayManager, int> availableLightIndexArray;
    typedef growableArray <nativeLightInfo, 8, 0x1030411, nativeLightInfoArrayManager, int> nativeLightInfoArray;

    struct lightState
    {
        lightState( void );
        ~lightState( void );

        void Shutdown( void );

        void Clear( void );
        bool SetLight( int lightIndex, const D3DLIGHT9& lightInfo );
        bool GetLight( int lightIndex, D3DLIGHT9& lightInfo );
        bool EnableLight( int lightIndex, bool enable );
        bool IsLightEnabled( int lightIndex );

        inline int GetFreeLightIndex( void )
        {
            return incrementalLightIndex++;
        }

        inline int GetLightIndex( RpLight *light )
        {
            int preferedLightIndex = RpLightGetLightIndex( light );
            unsigned int foundAt = 0;

            return ( preferedLightIndex < 0 || !activeGlobalLights.Find( preferedLightIndex, foundAt ) ) ? GetFreeLightIndex() : preferedLightIndex;
        }

        void ResetActiveLights( void );
        bool ActivateDirLight( RpLight *light );
        bool ActivateLocalLight( RpLight *light );

        void SwapWith( lightState& right );
        void Set( lightState& right );

        D3DLIGHT9 dirLightStruct;                           // Binary offsets: (1.0 US and 1.0 EU): 0x00C92648
        D3DLIGHT9 localLightStruct;                         // Binary offsets: (1.0 US and 1.0 EU): 0x00C925E0

        typedef growableArray <D3DLIGHT9, 8, 0x1030411, deviceLightInfoArrayManager, int> deviceLightInfoArray;

        lightIndexArray             activeGlobalLights;     // Array of light indice that are active
        deviceLightInfoArray        nativeLights;           // Array of light data that is set

        int                         incrementalLightIndex;

        RwColorFloat                ambientLightColor;
    };
};

// Module initialization.
void RenderWareLighting_Init( void );
void RenderWareLighting_Shutdown( void );

void RenderWareLighting_Reset( void );

void RenderWareLighting_InitShaders( void );
void RenderWareLighting_ResetShaders( void );
void RenderWareLighting_ShutdownShaders( void );

#endif //_RENDERWARE_LIGHTING_MANAGEMENT_