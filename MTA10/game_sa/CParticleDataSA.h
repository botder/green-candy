/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CParticleDataSA.h
*  PURPOSE:     Particle definition
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _CParticleDataSA_
#define _CParticleDataSA_

enum ePartObjectStatus : unsigned int
{
    PARTOBJ_STATUS_STOPPED = 2
};

class CEffectDataSAInterface : public CSimpleList::Item
{
public:
                                CEffectDataSAInterface();
                                ~CEffectDataSAInterface();

    void* operator new ( size_t size );
    void operator delete ( void *ptr );

    BYTE                        m_pad[48];              // 12
};

class CEffectBounds
{
public:
                                CEffectBounds( float *bounds );
                                ~CEffectBounds();

    void* operator new ( size_t );
    void operator delete ( void *ptr );

    CVector2D m_min;
    CVector2D m_max;
    float m_unk;
};

struct effTexInfo
{
    char tex1[0x20];
    char tex2[0x20];
    char tex3[0x20];
    char tex4[0x20];
};

struct effEncMatrix
{
    short vals[12];
};

class IEffectInfo abstract
{
public:
    virtual                         ~IEffectInfo()  {}

    virtual void __thiscall         Parse( FILE *fp, int ver ) = 0;
    virtual void __thiscall         Calculate( float a, float b, float c, float d, bool unk, void *unkStruct ) const = 0;

    void* operator new( size_t memSize )
    {
        return pParticleSystem->m_memory.AllocateInt( memSize );
    }

    void operator delete( void *ptr )
    {
        // We cannot deallocate
    }

    union
    {
        struct
        {
            unsigned short              m_flags;        // 4
            unsigned char               m_timeModePart; // 6
        };
        unsigned int m_id;                              // 4
    };

    
};

class IEffectObjectSA abstract
{
public:
    virtual                         ~IEffectObjectSA()    {}

    virtual bool __thiscall         Parse( FILE *fp, int ver, effTexInfo& texInfo ) = 0;
    virtual bool __thiscall         AssignTextures( const effTexInfo& info, int ver ) = 0;
    virtual void* __thiscall        Create() = 0;

    // some methods are missing yet...
};

class CEffectDefSAInterface : public CSimpleList::Item
{
public:
                                CEffectDefSAInterface();
                                ~CEffectDefSAInterface();

    void                        AssignBounds( float *bounds, float high_by );
    bool                        Parse( const char *filename, CFile *file, int num );

    void* operator new ( size_t );
    void operator delete ( void *ptr );

    unsigned int                m_hash;                 // 8
    float                       m_length;               // 12
    float                       m_loopIntervalMin;      // 16
    float                       m_loopLength;           // 20
    unsigned short              m_cullDistance;         // 24
    unsigned char               m_playMode;             // 26
    unsigned char               m_numPrimitives;        // 27
    IEffectObjectSA**           m_primitives;           // 28

    CEffectBounds*              m_bounds;               // 32

    BYTE                        m_pad2[24];             // 36
};

// Include thousands of info classes...!
#include "CParticleDataSA.info.h"

class CEffectObjectInfo // size: 10 bytes
{
public:
    bool                        Parse( CFile *file, int ver );

private:
    IEffectInfo*                CreateInfoStruct( int type );

public:

    int                         m_numInfo;              // 0
    IEffectInfo**               m_entries;              // 4
    unsigned char               m_version;              // 8
    unsigned char               m_version2;             // 9
};

class CEffectEmitterSAInterface : public IEffectObjectSA    // size: 64 bytes
{
public:
                                    CEffectEmitterSAInterface();
                                    ~CEffectEmitterSAInterface();

    bool                            Parse( FILE *fp, int ver, effTexInfo& texInfo );
    bool                            AssignTextures( const effTexInfo& info, int ver );
    void*                           Create();

    bool                            InfoExists( unsigned short id ) const;

private:
    bool                            ParseData( CFile *file, int ver, effTexInfo& texInfo );

public:

    void* operator new ( size_t );
    void operator delete ( void *ptr );

    unsigned char               m_unk4;                 // 4

    unsigned char               m_srcBlendID;           // 5
    unsigned char               m_dstBlendID;           // 6
    bool                        m_alphaBlend;           // 7
    effEncMatrix*               m_encMatrix;            // 8
    
    RwTexture*                  m_texture1;             // 12
    RwTexture*                  m_texture2;             // 16
    RwTexture*                  m_texture3;             // 20
    RwTexture*                  m_texture4;             // 24

    BYTE                        m_pad2[4];              // 28
    CSimpleList                 m_items;                // 32

    CEffectObjectInfo           m_info;                 // 44

    BYTE                        m_pad3[2];              // 54
    unsigned short              m_lodStart;             // 56
    unsigned short              m_lodEnd;               // 58
    
    bool                        m_hasFlatData;          // 60
    bool                        m_hasHeatHazeData;      // 61

    BYTE                        m_pad4[2];              // 62
};

#endif //_CParticleDataSA_