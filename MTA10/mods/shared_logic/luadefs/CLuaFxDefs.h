/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/luadefs/CLuaFxDefs.h
*  PURPOSE:     Lua fx definitions class header
*  DEVELOPERS:  Jax <>
*               The_GTA <quiret@gmx.de>
*
*****************************************************************************/

#ifndef __CLuaFxDefs_H
#define __CLuaFxDefs_H

namespace CLuaFunctionDefs
{
    void LoadFXFunctions();

    LUA_DECLARE( fxAddBlood );
    LUA_DECLARE( fxAddWood );
    LUA_DECLARE( fxAddSparks );
    LUA_DECLARE( fxAddTyreBurst );
    LUA_DECLARE( fxAddBulletImpact );
    LUA_DECLARE( fxAddPunchImpact );
    LUA_DECLARE( fxAddDebris );
    LUA_DECLARE( fxAddGlass );
    LUA_DECLARE( fxAddWaterHydrant );
    LUA_DECLARE( fxAddGunshot );
    LUA_DECLARE( fxAddTankFire );
    LUA_DECLARE( fxAddWaterSplash );
    LUA_DECLARE( fxAddBulletSplash );
    LUA_DECLARE( fxAddFootSplash );
};

#endif
