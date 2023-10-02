/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CModelInfoSA.h
*  PURPOSE:     Header file for entity model information handler class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_MODELINFO
#define __CGAMESA_MODELINFO

#include <game/CModelInfo.h>
#include <game/Common.h>

class CPedModelInfoSA;
class CPedModelInfoSAInterface;

#define     FUNC_CStreaming__HasModelLoaded 0x4044C0

// CModelInfo/ARRAY_ModelInfo __thiscall to load/replace vehicle models
#define     FUNC_LoadVehicleModel           0x4C95C0

#define     DWORD_AtomicsReplacerModelID    0xB71840
#define     FUNC_AtomicsReplacer            0x537150

#define     ARRAY_ModelInfo                 0xA9B0C8
#define     CLASS_CText                     0xC1B340
#define     FUNC_CText_Get                  0x6A0050
#define     FUNC_GetModelFlags              0x4044E0
#define     FUNC_GetBoundingBox             0x4082F0

#define     FUNC_RemoveRef                  0x4C4BB0  
#define     FUNC_IsVehicleModelType         0x4c5c80

#define     FUNC_FlushRequestList           0x40E4E0

#define     FUNC_HasVehicleUpgradeLoaded    0x407820
#define     FUNC_RequestVehicleUpgrade      0x408C70

#define     FUNC_CVehicleModelInfo__GetNumRemaps        0x4C86B0

#define     FUNC_SetColModel                0x4C4BC0
#define     FUNC_AddPedModel                0x4c67a0
#define     VAR_CTempColModels_ModelPed1    0x968DF0

#include "CModelInfoSA.base.h"
#include "CModelInfoSA.ide.h"

void    ModelInfo_Init( void );
void    ModelInfo_Shutdown( void );


class CModelInfoSA : public CModelInfo
{
protected:
    CBaseModelInfoSAInterface*      m_pInterface;
    unsigned short                  m_modelID;
    DWORD                           m_dwReferences;
    CColModel*                      m_pCustomColModel;
    CColModelSAInterface*           m_pOriginalColModelInterface;

public:
    static std::map <unsigned short, int> ms_RestreamTxdIDMap;

                                    CModelInfoSA();
                                    CModelInfoSA( unsigned short id );

    CBaseModelInfoSAInterface*      GetInterface()                          { return m_pInterface; }
    CPedModelInfoSAInterface*       GetPedModelInfoInterface()              { return (CPedModelInfoSAInterface*)GetInterface(); }
    const CPedModelInfoSAInterface* GetPedModelInfoInterface() const        { return (const CPedModelInfoSAInterface*)m_pInterface; }

    unsigned short                  GetModel() const                        { return m_modelID; }

    eVehicleType                    GetVehicleType() const;
    bool                            IsBoat() const;
    bool                            IsCar() const;
    bool                            IsTrain() const;
    bool                            IsHeli() const;
    bool                            IsPlane() const;
    bool                            IsBike() const;
    bool                            IsBmx() const;
    bool                            IsTrailer() const;
    bool                            IsVehicle() const;
    bool                            IsUpgrade() const;

    bool                            IsPed() const;
    bool                            IsObject() const;

    const char*                     GetNameIfVehicle() const;

    void                            Request( bool bAndLoad = false, bool bWaitForLoad = false, bool bHighPriority = false );
    void                            Remove();
    unsigned char                   GetLevelFromPosition( const CVector& vecPosition ) const;
    bool                            IsLoaded() const;
    unsigned char                   GetFlags() const;
    const CBoundingBox*             GetBoundingBox() const;
    bool                            IsValid();
    float                           GetDistanceFromCentreOfMassToBaseOfModel() const;
    unsigned short                  GetTextureDictionaryID() const;
    void                            SetTextureDictionaryID( unsigned short usID );
    float                           GetLODDistance() const;
    void                            SetLODDistance( float fDistance );
    void                            RestreamIPL();
    static void                     StaticFlushPendingRestreamIPL();

    void                            AddRef( bool bWaitForLoad, bool bHighPriority = false );
    int                             GetRefCount() const                     { return (int)m_dwReferences; }
    void                            RemoveRef( bool bRemoveExtraGTARef = false );
    void                            MaybeRemoveExtraGTARef();
    void                            DoRemoveExtraGTARef();

    // CVehicleModelInfo specific
    short                           GetAvailableVehicleMod( unsigned short usSlot ) const;
    bool                            IsUpgradeAvailable( eVehicleUpgradePosn posn ) const;
    void                            SetCustomCarPlateText( const char *szText );
    unsigned int                    GetNumRemaps() const;
    void*                           GetVehicleSuspensionData() const;
    void*                           SetVehicleSuspensionData( void *pSuspensionLines );

    // Upgrades only!
    void                            RequestVehicleUpgrade();

    // ONLY use for peds
    void                            GetVoice( short* psVoiceType, short* psVoice ) const;
    void                            GetVoice( const char** pszVoiceType, const char** szVoice ) const;
    void                            SetVoice( short sVoiceType, short sVoice );
    void                            SetVoice( const char* szVoiceType, const char* szVoice );

    inline void                     SetModelID( unsigned short id )         { m_modelID = id; }

    void                            MakePedModel( const char *texture );
};

#endif