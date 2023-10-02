/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPedModelInfoSA.cpp
*  PURPOSE:     Modelinfo for ped entities
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Jax <>
*               Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "gamesa_renderware.h"

// TODO!
CPedModelInfoSAInterface::CPedModelInfoSAInterface( void )
{
    // TEMP HACK <3
    *(DWORD*)this = VAR_CPedModelInfo_VTBL;

    // Initialize according to constructor.
    pHitColModel = NULL;

    // Initialize according to static rules.
    // Apparently we crash if we do not set proper values ;)
    motionAnimGroup = 0;
    pedType = 0;
    pedStats = NULL;
    bCanDriveCars = true;
    pedFlags = 0;
    bFirstRadioStation = 0;
    bSecondRadioStation = 0;
    bIsInRace = false;
    sVoiceType = -1;
    sFirstVoice = -1;
    sLastVoice = -1;
    sNextVoice = -1;

    Init();
}

CPedModelInfoSAInterface::~CPedModelInfoSAInterface( void )
{

}

eModelType CPedModelInfoSAInterface::GetModelType( void )
{
    return MODEL_PED;
}

void CPedModelInfoSAInterface::DeleteRwObject( void )
{
    CClumpModelInfoSAInterface::DeleteRwObject();

    if ( pColModel )
    {
        delete pColModel;

        pColModel = NULL;
    }
}

void CPedModelInfoSAInterface::SetAnimFile( const char *name )
{

}

void CPedModelInfoSAInterface::ConvertAnimFileIndex( void )
{

}

int CPedModelInfoSAInterface::GetAnimFileIndex( void )
{
    return -1;
}

CPedModelInfoSA::CPedModelInfoSA( void ) : CModelInfoSA ()
{
    m_pPedModelInterface = new CPedModelInfoSAInterface;

    // Set a collision model.
    m_pPedModelInterface->SetColModel( (CColModelSAInterface*)VAR_CTempColModels_ModelPed1, false );
}

CPedModelInfoSA::~CPedModelInfoSA( void )
{

}

void CPedModelInfoSA::SetMotionAnimGroup( AssocGroupId animGroup )
{
    m_pPedModelInterface->motionAnimGroup = animGroup;
}
