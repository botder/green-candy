/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CModelInfoSA.ide.cpp
*  PURPOSE:     Model info IDE management
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

static void __cdecl ConvertIDECompositeToInfoFlags( CBaseModelInfoSAInterface *info, unsigned int flags )
{
    BOOL_FLAG( info->renderFlags, RENDER_LAST, flags & ( OBJECT_ALPHA1 | OBJECT_ALPHA2 ) );
    BOOL_FLAG( info->renderFlags, RENDER_ADDITIVE, flags & OBJECT_ALPHA2 );
    BOOL_FLAG( info->renderFlags, RENDER_NOSHADOW, flags & OBJECT_NOSHADOW );
    BOOL_FLAG( info->renderFlags, RENDER_NOCULL, flags & OBJECT_NOCULL );
    BOOL_FLAG( info->renderFlags, RENDER_BACKFACECULL, !( flags & OBJECT_NOBACKFACECULL ) );
}

static void __cdecl ConvertIDEModelToInfoFlags( CBaseModelInfoSAInterface *info, unsigned int flags )
{
    // Composite flags apply to clumps and atomic models
    ConvertIDECompositeToInfoFlags( info, flags );

    BOOL_FLAG( info->renderFlags, RENDER_STATIC, flags & OBJECT_WETEFFECT );
 
    if ( flags & OBJECT_BREAKGLASS )
    {
        info->atomicType = 4;
    }

    if ( flags & OBJECT_BREAKGLASS_CRACK )
    {
        info->atomicType = 5;
    }

    if ( flags & OBJECT_GARAGE )
    {
        info->atomicType = 7;
    }

    if ( flags & OBJECT_VEGETATION )
    {
        info->atomicType = 1;
    }

    if ( flags & OBJECT_BIG_VEGETATION )
    {
        info->atomicType = 2;
    }

    BOOL_FLAG( info->collFlags, COLL_AUDIO, flags & OBJECT_USE_POLYSHADOW );

    if ( flags & OBJECT_GRAFFITI )
    {
        info->atomicType = 6;
    }

    if ( flags & OBJECT_STATUE )
    {
        info->atomicType = 11;
    }

    if ( flags & OBJECT_UNKNOWN_2 )
    {
        info->atomicType = 10;
    }
}

void ModelInfoIDE_Init()
{
    // Investigation of model flags
    HookInstall( 0x005B3AD0, (DWORD)ConvertIDECompositeToInfoFlags, 5 );
    HookInstall( 0x005B3B20, (DWORD)ConvertIDEModelToInfoFlags, 5 );
}

void ModelInfoIDE_Shutdown()
{

}