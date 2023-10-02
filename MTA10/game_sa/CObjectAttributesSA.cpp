/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CObjectAttributesSA.cpp
*  PURPOSE:     Object attributes system
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

dynamicObjectData g_dynObjData[MAX_DYNAMIC_OBJECT_DATA];
extern CBaseModelInfoSAInterface **ppModelInfo;

/*=========================================================
    _Objects_LoadAttributes

    Arguments:
        filepath - location of OBJECT.DAT (usually 'DATA\\OBJECT.DAT')
        unused - unknown parameter
    Purpose:
        Called by the engine to load the object attributes file.
        There are MAX_DYNAMIC_OBJECT_DATA maximum unique entries.
        First entry in g_dynObjData are the default attributes for
        every object without definition.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x005B5360
=========================================================*/
static void __cdecl _Objects_LoadAttributes( const char *filepath, bool unused )
{
    dynamicObjectData& firstObj = g_dynObjData[0];
    firstObj.mass = 99999;
    firstObj.turnMass = 99999;
    firstObj.airResistance = 0.99000001f;
    firstObj.elasticity = 0.1f;
    firstObj.percSubmerged = 1.6f;
    firstObj.uproot = 0;
    firstObj.CDMult = 1;
    firstObj.CDEff = 0;
    firstObj.SpCDR = 0;
    firstObj.CamAv = 2;

    dynamicObjectData *obj = &g_dynObjData[1];
    *obj = firstObj;
    obj->CamAv = 0;

    obj = &g_dynObjData[2];
    *obj = firstObj;
    obj->SpCDR = 4;

    obj = &g_dynObjData[3];
    *obj = firstObj;
    obj->SpCDR = 4;
    obj->CamAv = 0;

    obj = &g_dynObjData[4];
    *obj = firstObj;
    obj->SpCDR = 5;

    CFile *file = OpenGlobalStream( filepath, "rb" );

    obj++;

    unsigned int num = 5;

    while ( const char *line = FileMgr::GetConfigLine( file ) )
    {
        const char c = *line;

        if ( c == '\0' || c == ';' )
            continue;

        if ( c == '*' )
            break;

        memset( obj, 0, sizeof(*obj) );

        char name[0x100];
        float percSubmerged;
        unsigned int CDEff;
        unsigned int SpCDR;
        unsigned int CamAv;
        unsigned int Expl;
        unsigned int fxType;
        char fxName[0x100];

        // Very big + powerful sscanf :D
        sscanf( line, "%s %f %f %f %f %f %f %f %d %d %d %d %d %f %f %f %s %f %f %f %f %f %d %d",    // fixed gun mode and spark parameters (R* slip-up)
            name, &obj->mass, &obj->turnMass, &obj->airResistance, &obj->elasticity,
            &percSubmerged, &obj->uproot, &obj->CDMult, &CDEff, &SpCDR, &CamAv, &Expl, &fxType,
            &obj->fxOffset[0], &obj->fxOffset[1], &obj->fxOffset[2], fxName,
            &obj->smashMult, &obj->smashVelocity[0], &obj->smashVelocity[1], &obj->smashVelocity[2], &obj->smashRand,
            &obj->b_gun, &obj->b_spk );

        obj->effDef = NULL;
        obj->fxType = fxType;

        if ( obj->fxType )
            obj->effDef = pParticleSystem->GetBlueprintByName( fxName );

        obj->SpCDR = SpCDR;
        obj->CDEff = CDEff;
        obj->CamAv = CamAv;
        obj->explosion = Expl;

        obj->percSubmerged = 100.0f / percSubmerged * obj->mass * 0.008f;

        CBaseModelInfoSAInterface *model = Streaming::GetModelByHash( pGame->GetKeyGen()->GetUppercaseKey( name ), NULL );

        if ( !model )
            continue;

        if ( obj->mass == firstObj.mass && obj->CDMult == firstObj.CDMult && obj->CDEff == firstObj.CDEff )
        {
            // Some optimizations, apparrently?
            if ( obj->SpCDR == firstObj.SpCDR )
            {
                model->usDynamicIndex = obj->CamAv == 0 ? 1 : 0;
                continue;
            }
            else if ( obj->SpCDR == g_dynObjData[1].SpCDR )
            {
                model->usDynamicIndex = obj->CamAv == 0 ? 3 : 2;
                continue;
            }
        }

        for ( unsigned int n = 0; n < num; n++ )
        {
            dynamicObjectData& simDef = g_dynObjData[n];

            if ( obj->mass == simDef.mass && obj->turnMass == simDef.turnMass && obj->airResistance == simDef.airResistance &&
                 obj->elasticity == simDef.elasticity && obj->percSubmerged == simDef.percSubmerged && obj->uproot == simDef.uproot &&
                 obj->CDMult == simDef.CDMult && obj->CDEff == simDef.CDEff && obj->SpCDR == simDef.SpCDR && obj->CamAv == simDef.CamAv &&
                 obj->explosion == simDef.explosion && obj->fxType == simDef.fxType && obj->fxOffset == simDef.fxOffset && 
                 obj->effDef == simDef.effDef && obj->smashMult == simDef.smashMult && obj->smashVelocity == simDef.smashVelocity &&
                 obj->smashRand == simDef.smashRand && obj->b_gun == simDef.b_gun && obj->b_spk == simDef.b_spk )
            {
                model->usDynamicIndex = (short)n;
                goto reloop;
            } 
        }

        if ( num != MAX_DYNAMIC_OBJECT_DATA )
        {
            model->usDynamicIndex = (short)num++;
            obj++;
        }

reloop:
        continue;
    }

    delete file;
}

/*=========================================================
    _Object_PrepareDynamicPhysics

    Arguments:
        model - model id of the object
        obj - the object's interface
    Purpose:
        Called by the engine when it wants to assign object
        attributes to an object (in the object's constructor).
    Binary offsets:
        (1.0 US and 1.0 EU): 0x005A2D00
=========================================================*/
static void __cdecl _Object_PrepareDynamicPhysics( modelId_t model, CObjectSAInterface *obj )
{
    CBaseModelInfoSAInterface *info = ppModelInfo[model];

    if ( info->usDynamicIndex == -1 )
    {
        obj->m_mass = 99999;
        obj->m_turnMass = 99999;
        obj->physicalFlags &= ~0x02;
        obj->physicalFlags |= 0x80000C;
        obj->m_dynData = &g_dynObjData[0];
        return;
    }

    dynamicObjectData *data = &g_dynObjData[info->usDynamicIndex];

    obj->m_dynData = data;
    obj->m_mass = data->mass;
    obj->m_turnMass = data->turnMass;
    obj->m_airResistance = data->airResistance;
    obj->m_elasticity = data->elasticity;
    obj->m_buoyancyConstant = data->percSubmerged;
    obj->m_collDamEffect = data->CDEff;

    if ( data->mass >= 99998 )
    {
        obj->physicalFlags &= ~0x02;
        obj->physicalFlags |= 0x0C;

        if ( data->CDEff == 0 )
            obj->physicalFlags |= 0x800000;
    }

    if ( data->SpCDR == 6 )
    {
        obj->physicalFlags |= 0x20;
        obj->m_objFlags &= ~0xC000;
    }
    else if ( data->SpCDR == 7 )
    {
        obj->m_objFlags &= ~0x8000;
        obj->m_objFlags |= 0x4000;
        obj->physicalFlags |= 0x2C;
    }
    else if ( data->SpCDR == 8 )
    {
        obj->physicalFlags |= 0x40;
        obj->m_centerOfMass = CVector( 0, 0, info->pColModel->m_bounds.vecBoundMin[2] );
    }
    else if ( data->SpCDR == 9 )
    {
        obj->physicalFlags &= ~0x02;
        obj->physicalFlags |= 0x80;
    }
}

void ObjectAttributes_Init()
{
    // Load the attributes ourselves.
    // We need a seperate structure to save original attributes to, restoring them at mod unload.
    // Introducing a new callback framework to game_sa: mod init + mod unload
    HookInstall( 0x005B5360, (DWORD)_Objects_LoadAttributes, 5 );
    HookInstall( 0x005A2D00, (DWORD)_Object_PrepareDynamicPhysics, 5 );

    // Set the default dynamic data each created object will receive
    *(dynamicObjectData**)0x0059F85D = g_dynObjData;
}

void ObjectAttributes_Shutdown()
{
}