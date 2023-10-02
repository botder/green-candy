/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CParticleDataSA.cpp
*  PURPOSE:     Particle definition
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

// The_GTA: This code is not yet complete. The information descriptors (CEffectObjectInfo::CreateInfoStruct)
// have to be reverse engineered, too. I cut it out of the MTA:GREEN build due to time constraints.
// I hope other people will pick up my job. As guideline for reven, take my uploaded IDA 5.0 Pro databases (US and EU).

#include "StdInc.h"

CEffectDataSAInterface::CEffectDataSAInterface()
{
}

CEffectDataSAInterface::~CEffectDataSAInterface()
{
}

void* CEffectDataSAInterface::operator new ( size_t size )
{
    return pParticleSystem->m_memory.AllocateInt( size );
}

void CEffectDataSAInterface::operator delete ( void *ptr )
{
    return; // we cannot deallocate
}

CEffectBounds::CEffectBounds( float *bounds ) : m_min( bounds[0], bounds[1] ), m_max( bounds[2], bounds[3] )
{
    m_unk = 0;
}

CEffectBounds::~CEffectBounds()
{
}

void* CEffectBounds::operator new ( size_t size )
{
    return pParticleSystem->m_memory.AllocateInt( size );
}

void CEffectBounds::operator delete ( void *ptr )
{
    return; // we cannot deallocate
}

CEffectDefSAInterface::CEffectDefSAInterface()
{
    m_bounds = NULL;    // We allocate it during parsing, if even
}

CEffectDefSAInterface::~CEffectDefSAInterface()
{
}

void CEffectDefSAInterface::AssignBounds( float *bounds, float high_by )
{
    if ( m_bounds )
        return;

    if ( high_by < 0 )
        return;

    m_bounds = new CEffectBounds( bounds );
}

bool CEffectDefSAInterface::Parse( const char *filename, CFile *file, int ver )
{
    effTexInfo texInfo[8];
    char buffer[0x100];
    char token[0x20];
    char fname[0x80];
    char name[0x10];
    int num;
    float fp;
    float bounds[4];

    FileSystem::FileGetString( file, buffer, sizeof(buffer) );  // skip one line

    if ( ver > 100 )
    {
        FileSystem::FileGetString( file, buffer, sizeof(buffer) );
        sscanf( buffer, "%s %s", token, fname );
    }

    // Name
    FileSystem::FileGetString( file, buffer, sizeof(buffer) );
    sscanf( buffer, "%s %s", token, name );

    // Hashes are quick; quicker than byte string comparisons.
    m_hash = pGame->GetKeyGen()->GetUppercaseKey( name );

    // Length
    FileSystem::FileGetString( file, buffer, sizeof(buffer) );
    sscanf( buffer, "%s %f", token, &m_length );

    if ( ver >= 100 )
    {
        // Loop interval min
        FileSystem::FileGetString( file, buffer, sizeof(buffer) );
        sscanf( buffer, "%s %f", token, &m_loopIntervalMin );

        // Loop length
        FileSystem::FileGetString( file, buffer, sizeof(buffer) );
        sscanf( buffer, "%s %f", token, &m_loopLength );
    }
    else
    {
        // We kinda skip this
        m_loopIntervalMin = 0;
        m_loopLength = 0;
    }

    // Play mode
    FileSystem::FileGetString( file, buffer, sizeof(buffer) );
    sscanf( buffer, "%s %d", token, &num );

    m_playMode = num;

    // Cull distance
    FileSystem::FileGetString( file, buffer, sizeof(buffer) );
    sscanf( buffer, "%s %f", token, &fp );

    m_cullDistance = (unsigned short)( fp * 256 );

    if ( ver > 103 )
    {
        // Bounding sphere
        FileSystem::FileGetString( file, buffer, sizeof(buffer) );
        sscanf( buffer, "%f %f %f %f", &bounds[0], &bounds[1], &bounds[2], &bounds[3] );

        // Allocate it
        m_bounds = NULL;
        AssignBounds( bounds, bounds[3] );
    }

    // Num Primitives
    FileSystem::FileGetString( file, buffer, sizeof(buffer) );
    sscanf( buffer, "%d", &num );

    m_numPrimitives = num;
    m_primitives = (IEffectObjectSA**)pParticleSystem->m_memory.AllocateInt( (char)num * 4 );

    for ( unsigned int n = 0; n < m_numPrimitives; n++ )
    {
        CEffectEmitterSAInterface *obj;

        // Check whether this is an emitter descriptor
        FileSystem::FileGetString( file, buffer, sizeof(buffer) );
        sscanf( buffer, "%s", token );

        if ( strcmp( token, "FX_PRIM_EMITTER_DATA:" ) == 0 )
            obj = new CEffectEmitterSAInterface();
        else
            assert( 0 );

        m_primitives[n] = obj;

        FileSystem::FileGetString( file, buffer, sizeof(buffer) );

        // Parse the object's definition
        obj->Parse( (FILE*)file, ver, texInfo[n] );

        // Cache processing data
        obj->m_hasFlatData = obj->InfoExists( 0x4020 );
        obj->m_hasHeatHazeData = obj->InfoExists( 0x4008 );
    }

    if ( ver > 107 )
    {
        // Omit textures (unused parameter)
        FileSystem::FileGetString( file, buffer, sizeof(buffer) );
        sscanf( buffer, "%s %d", token, &num );

        if ( ver > 108 )
        {
            // Texture Container name (unused parameter)
            FileSystem::FileGetString( file, buffer, sizeof(buffer) );
            sscanf( buffer, "%s %s", token, name );
        }
    }

    // Apply the textures
    for ( unsigned int n = 0; n < m_numPrimitives; n++ )
        m_primitives[n]->AssignTextures( texInfo[n], ver );

    return false;
}

void* CEffectDefSAInterface::operator new ( size_t size )
{
    return pParticleSystem->m_memory.AllocateInt( size );
}

void CEffectDefSAInterface::operator delete ( void *ptr )
{
    return; // we cannot deallocate
}

CEffectEmitterSAInterface::CEffectEmitterSAInterface()
{
    m_texture1 = NULL;
    m_texture2 = NULL;
    m_texture3 = NULL;
    m_texture4 = NULL;
    m_unk4 = 0;
}

CEffectEmitterSAInterface::~CEffectEmitterSAInterface()
{
}

bool CEffectEmitterSAInterface::InfoExists( unsigned short id ) const
{
    for ( int n = 0; n < m_info.m_numInfo; n++ )
    {
        if ( m_info.m_entries[n]->m_flags == id )
            return true;
    }

    return false;
}

IEffectInfo* CEffectObjectInfo::CreateInfoStruct( int id )
{
    IEffectInfo *eff = NULL;

    switch( id )
    {
    case 0x2080:
        // JITTER_DATA
        break;
    }

    return eff;
#if 0
    // We may work on this at another time, because it seems to be a runtime object conglomeration :D
    DWORD dwFunc = 0x004A7B00;
    IEffectInfo *eff;
    __asm
    {
        push _type
        mov ecx,this
        call dwFunc
        mov eff,eax
    }
    return eff;
#endif
}

bool CEffectObjectInfo::Parse( CFile *file, int ver )
{
    char buffer[0x100];
    char token[0x80];

    FileSystem::FileGetString( file, buffer, sizeof(buffer) );  // skip a line

    // Number of entries
    FileSystem::FileGetString( file, buffer, sizeof(buffer) );
    sscanf( buffer, "%s %d", token, &m_numInfo );

    m_version = 0xFF;
    m_version2 = 0xFF;

    // Allocate space for the info structure pointers
    m_entries = (IEffectInfo**)pParticleSystem->m_memory.AllocateInt( m_numInfo * 4 );

    for ( int n = 0; n < m_numInfo; n++ )
    {
        IEffectInfo *info;
        int timeModePart = 1;

#define TIMEMODE_PARAM(id) { if ( ver > 0 ) { FileSystem::FileGetString( file, buffer, sizeof(buffer) ); sscanf( buffer, "%s %d", token, &timeModePart ); } info = CreateInfoStruct( id ); }

        // Read the tag and parse it
        FileSystem::FileGetString( file, buffer, sizeof(buffer) );
        sscanf( buffer, "%s", token );

        // Pretty much all of the effect object properties or abilities
             if ( strcmp( token, "FX_INFO_EMRATE_DATA:" ) == 0 )        info = CreateInfoStruct( 0x1001 );
        else if ( strcmp( token, "FX_INFO_EMSIZE_DATA:" ) == 0 )        info = CreateInfoStruct( 0x1004 );
        else if ( strcmp( token, "FX_INFO_EMSPEED_DATA:" ) == 0 )       info = CreateInfoStruct( 0x1008 );
        else if ( strcmp( token, "FX_INFO_EMDIR_DATA:" ) == 0 )         info = CreateInfoStruct( 0x1010 );
        else if ( strcmp( token, "FX_INFO_EMANGLE_DATA:" ) == 0 )       info = CreateInfoStruct( 0x1020 );
        else if ( strcmp( token, "FX_INFO_EMLIFE_DATA:" ) == 0 )        info = CreateInfoStruct( 0x1040 );
        else if ( strcmp( token, "FX_INFO_EMPOS_DATA:" ) == 0 )         info = CreateInfoStruct( 0x1080 );
        else if ( strcmp( token, "FX_INFO_EMWEATHER_DATA:" ) == 0 )     info = CreateInfoStruct( 0x1100 );
        else if ( strcmp( token, "FX_INFO_EMROTATION_DATA:" ) == 0 )    info = CreateInfoStruct( 0x1200 );
        else if ( strcmp( token, "FX_INFO_NOISE_DATA:" ) == 0 )         TIMEMODE_PARAM( 0x2001 )
        else if ( strcmp( token, "FX_INFO_FORCE_DATA:" ) == 0 )         TIMEMODE_PARAM( 0x2002 )
        else if ( strcmp( token, "FX_INFO_FRICTION_DATA:" ) == 0 )      TIMEMODE_PARAM( 0x2004 )
        else if ( strcmp( token, "FX_INFO_ATTRACTPT_DATA:" ) == 0 )     TIMEMODE_PARAM( 0x2008 )
        else if ( strcmp( token, "FX_INFO_ATTRACTLINE_DATA:" ) == 0 )   TIMEMODE_PARAM( 0x2010 )
        else if ( strcmp( token, "FX_INFO_GROUNDCOLLIDE_DATA:" ) == 0 ) TIMEMODE_PARAM( 0x2020 )
        else if ( strcmp( token, "FX_INFO_WIND_DATA:" ) == 0 )          TIMEMODE_PARAM( 0x2040 )
        else if ( strcmp( token, "FX_INFO_JITTER_DATA:" ) == 0 )        TIMEMODE_PARAM( 0x2080 )
        else if ( strcmp( token, "FX_INFO_ROTSPEED_DATA:" ) == 0 )      TIMEMODE_PARAM( 0x2100 )
        else if ( strcmp( token, "FX_INFO_FLOAT_DATA:" ) == 0 )         TIMEMODE_PARAM( 0x2200 )
        else if ( strcmp( token, "FX_INFO_UNDERWATER_DATA:" ) == 0 )    TIMEMODE_PARAM( 0x2400 )
        else if ( strcmp( token, "FX_INFO_COLOUR_DATA:" ) == 0 )        TIMEMODE_PARAM( 0x4001 )
        else if ( strcmp( token, "FX_INFO_SIZE_DATA:" ) == 0 )          TIMEMODE_PARAM( 0x4002 )
        else if ( strcmp( token, "FX_INFO_SPRITERECT_DATA:" ) == 0 )    TIMEMODE_PARAM( 0x4004 )
        else if ( strcmp( token, "FX_INFO_HEATHAZE_DATA:" ) == 0 )      TIMEMODE_PARAM( 0x4008 )
        else if ( strcmp( token, "FX_INFO_TRAIL_DATA:" ) == 0 )         TIMEMODE_PARAM( 0x4010 )
        else if ( strcmp( token, "FX_INFO_FLAT_DATA:" ) == 0 )          TIMEMODE_PARAM( 0x4020 )
        else if ( strcmp( token, "FX_INFO_DIR_DATA:" ) == 0 )           TIMEMODE_PARAM( 0x4040 )
        else if ( strcmp( token, "FX_INFO_ANIMTEX_DATA:" ) == 0 )       TIMEMODE_PARAM( 0x4080 )
        else if ( strcmp( token, "FX_INFO_COLOURRANGE_DATA:" ) == 0 )   TIMEMODE_PARAM( 0x4100 )
        else if ( strcmp( token, "FX_INFO_SELFLIT_DATA:" ) == 0 )       TIMEMODE_PARAM( 0x4200 )
        else if ( strcmp( token, "FX_INFO_COLOURBRIGHT_DATA:" ) == 0 )  TIMEMODE_PARAM( 0x4400 )
        else if ( strcmp( token, "FX_INFO_SMOKE_DATA:" ) == 0 )         TIMEMODE_PARAM( 0x8001 )    // It's over 8000!!!11

        m_entries[n] = info;

        // TODO: Parse the object's definitions
        //info->Parse();

        // We have a blank line for oversight purposes
        FileSystem::FileGetString( file, buffer, sizeof(buffer) );

        info->m_timeModePart = timeModePart;

        unsigned short flags = info->m_flags;

        if ( flags & 0x1000 )
            continue;

        if ( flags & 0x2000 )
        {
            if ( m_version == 0xFF )
                m_version = ver;
        }
        else if ( flags & 0xC000 )
        {
            if ( m_version2 == 0xFF )
                m_version2 = ver;
        }
    }

    if ( m_version2 == 0xFF )
        m_version2 = m_numInfo;

    if ( m_version == 0xFF )
        m_version = m_version2;

    return true;
}

bool CEffectEmitterSAInterface::ParseData( CFile *file, int ver, effTexInfo& texInfo )
{
    char buffer[0x100];
    char token[0x80];

    union
    {
        float axis[12];
        int num;
    };

    // Beginning of base data
    FileSystem::FileGetString( file, buffer, sizeof(buffer) );  // skip descriptor
    FileSystem::FileGetString( file, buffer, sizeof(buffer) );  // skip name

    // Read the matrix
    FileSystem::FileGetString( file, buffer, sizeof(buffer) );
    sscanf( buffer, "%s %f %f %f %f %f %f %f %f %f %f %f %f", token,
        &axis[0], &axis[1], &axis[2], &axis[3], &axis[4], &axis[5], &axis[6], &axis[7], &axis[8], &axis[9], &axis[10], &axis[11] );

    // Check it for identity matrix and no offset
    if ( axis[0] == 1 && axis[1] == 0 && axis[2] == 0 &&
         axis[3] == 0 && axis[4] == 1 && axis[5] == 0 &&
         axis[6] == 0 && axis[7] == 0 && axis[8] == 1 &&
         axis[9] == 0 && axis[10] == 0 && axis[11] == 0 )
    {
        // We optimize math
        m_encMatrix = NULL;
    }
    else
    {
        // Store an encrypted matrix representation for memory saving
        m_encMatrix = new (pParticleSystem->m_memory.Allocate( sizeof( *m_encMatrix ), 2 )) effEncMatrix;
        m_encMatrix->vals[0] = (unsigned short)( axis[0] * 0x7FFF ); m_encMatrix->vals[1] = (unsigned short)( axis[1] * 0x7FFF ); m_encMatrix->vals[2] = (unsigned short)( axis[2] * 0x7FFF );
        m_encMatrix->vals[3] = (unsigned short)( axis[3] * 0x7FFF ); m_encMatrix->vals[4] = (unsigned short)( axis[4] * 0x7FFF ); m_encMatrix->vals[5] = (unsigned short)( axis[5] * 0x7FFF );
        m_encMatrix->vals[6] = (unsigned short)( axis[6] * 0x7FFF ); m_encMatrix->vals[7] = (unsigned short)( axis[7] * 0x7FFF ); m_encMatrix->vals[8] = (unsigned short)( axis[8] * 0x7FFF );
        m_encMatrix->vals[9] = (unsigned short)( axis[9] * 0x7FFF ); m_encMatrix->vals[10] = (unsigned short)( axis[10] * 0x7FFF ); m_encMatrix->vals[11] = (unsigned short)( axis[11] * 0x7FFF );
    }

    // First texture
    FileSystem::FileGetString( file, buffer, sizeof(buffer) );
    sscanf( buffer, "%s %s", token, texInfo.tex1 );

    if ( ver > 101 )
    {
        // Do three more textures
        FileSystem::FileGetString( file, buffer, sizeof(buffer) );
        sscanf( buffer, "%s %s", token, texInfo.tex2 );

        FileSystem::FileGetString( file, buffer, sizeof(buffer) );
        sscanf( buffer, "%s %s", token, texInfo.tex3 );

        FileSystem::FileGetString( file, buffer, sizeof(buffer) );
        sscanf( buffer, "%s %s", token, texInfo.tex4 );
    }

    // Alpha blend
    FileSystem::FileGetString( file, buffer, sizeof(buffer) );
    sscanf( buffer, "%s %d", token, &num );

    m_alphaBlend = ( num != 0 );

    // Source Blend ID
    FileSystem::FileGetString( file, buffer, sizeof(buffer) );
    sscanf( buffer, "%s %d", token, &num );

    m_srcBlendID = num;

    // Destination Blend ID
    FileSystem::FileGetString( file, buffer, sizeof(buffer) );
    sscanf( buffer, "%s %d", token, &num );

    m_dstBlendID = num;

    // Continue with the items
    m_info.Parse( file, ver );
    return true;
}

bool CEffectEmitterSAInterface::Parse( FILE *fp, int ver, effTexInfo& texInfo )
{
    // Since we initialize the fx, we should not care about the "FILE" definiton
    // Use our internal format instead!
    CFile *file = (CFile*)fp;
    char buffer[0x100];
    char token[0x80];
    float num;

    ParseData( file, ver, texInfo );

    // Lod Start
    FileSystem::FileGetString( file, buffer, sizeof(buffer) );
    sscanf( buffer, "%s %f", token, &num );

    m_lodStart = (unsigned short)( num * 64.0f );

    // Lod End
    FileSystem::FileGetString( file, buffer, sizeof(buffer) );
    sscanf( buffer, "%s %f", token, &num );

    m_lodEnd = (unsigned short)( num * 64.0f );
    return true;
}

bool CEffectEmitterSAInterface::AssignTextures( const effTexInfo& texInfo, int ver )
{
    char nameBuff[0x40];

    snprintf( nameBuff, sizeof(nameBuff), "%sm", texInfo.tex1 );

    if ( !( m_texture1 = RwFindTexture( texInfo.tex1, nameBuff ) ) )
    {
        m_texture1 = RwFindTexture( texInfo.tex1, NULL );
    }

    // Secondary textures are only available in later versions
    if ( ver > 101 )
    {
        if ( strcmp( texInfo.tex2, "NULL" ) != 0 )
        {
            snprintf( nameBuff, sizeof(nameBuff), "%sm", texInfo.tex2 );

            if ( !( m_texture2 = RwFindTexture( texInfo.tex2, nameBuff ) ) )
                m_texture2 = RwFindTexture( texInfo.tex2, NULL );
        }

        if ( strcmp( texInfo.tex3, "NULL" ) != 0 )
        {
            snprintf( nameBuff, sizeof(nameBuff), "%sm", texInfo.tex3 );

            if ( !( m_texture3 = RwFindTexture( texInfo.tex3, nameBuff ) ) )
                m_texture3 = RwFindTexture( texInfo.tex3, NULL );
        }

        if ( strcmp( texInfo.tex4, "NULL" ) == 0 )
        {
            snprintf( nameBuff, sizeof(nameBuff), "%sm", texInfo.tex4 );

            if ( !( m_texture4 = RwFindTexture( texInfo.tex4, nameBuff ) ) )
                m_texture4 = RwFindTexture( texInfo.tex4, NULL );
        }
    }

    return true;
}

void* CEffectEmitterSAInterface::Create()
{
    return NULL;
}

void* CEffectEmitterSAInterface::operator new ( size_t size )
{
    return pParticleSystem->m_memory.AllocateInt( size );
}

void CEffectEmitterSAInterface::operator delete ( void *ptr )
{
    return; // we cannot deallocate
}