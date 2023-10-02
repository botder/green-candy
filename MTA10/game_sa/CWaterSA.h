/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CWaterSA.h
*  PURPOSE:     Control the lakes and seas
*  DEVELOPERS:  arc_
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CWATERSA_H
#define __CWATERSA_H

// -------------------------------
// SA interfaces

#pragma pack(push)
#pragma pack(1)

class CWaterVertexSAInterface
{
public:
    short           m_sX;
    short           m_sY;
    float           m_fZ;
    float           m_fUnknown;
    float           m_fHeight;
    char            m_cFlowX;
    char            m_cFlowY;
    WORD            m_pad;   
};

class CWaterPolySAInterface
{
public:
    unsigned short  m_wVertexIDs[3];
};

class CWaterQuadSAInterface : public CWaterPolySAInterface
{
public:
    unsigned short  m_wFourthVertexIDDummy;
    unsigned short  m_wFlags;
};

class CWaterTriangleSAInterface : public CWaterPolySAInterface
{
public:
    unsigned short  m_wFlags;
};

#pragma pack(pop)

// -------------------------------
// Interface wrappers

class CWaterVertexSA : public CWaterVertex
{
public:
                                        CWaterVertexSA()                                        { m_pInterface = NULL; }
                                        CWaterVertexSA( CWaterVertexSAInterface *intf )         { m_pInterface = intf; }

    CWaterVertexSAInterface*            GetInterface()                                          { return m_pInterface; }
    const CWaterVertexSAInterface*      GetInterface() const                                    { return m_pInterface; }

    void                                SetInterface( CWaterVertexSAInterface *intf )           { m_pInterface = intf; }

    unsigned short                      GetID() const;

    void                                GetPosition( CVector& vec ) const;
    bool                                SetPosition( const CVector& vec, void *pChangeSource = NULL );

protected:
    CWaterVertexSAInterface*            m_pInterface;
};

class CWaterPolySA : public CWaterPoly
{
public:
    CWaterPolySAInterface*              GetInterface()                                          { return m_pInterface; }
    const CWaterPolySAInterface*        GetInterface() const                                    { return m_pInterface; }

    virtual void                        SetInterface( CWaterPolySAInterface *intf ) = 0;

    virtual EWaterPolyType              GetType() const = 0;
    virtual size_t                      GetNumVertices() const = 0;

    unsigned short                      GetID() const                                           { return m_wID; }
    CWaterVertex*                       GetVertex( unsigned int index );
    bool                                ContainsPoint( float x, float y ) const;

protected:
    CWaterPolySAInterface*              m_pInterface;
    unsigned short                      m_wID;
};

class CWaterQuadSA : public CWaterPolySA
{
public:
                                        CWaterQuadSA()                                          { m_pInterface = NULL; m_wID = ~0; }
                                        CWaterQuadSA( CWaterPolySAInterface *intf )             { SetInterface( intf ); }

    CWaterQuadSAInterface*              GetInterface()                                          { return (CWaterQuadSAInterface*)m_pInterface; }
    const CWaterQuadSAInterface*        GetInterface() const                                    { return (CWaterQuadSAInterface*)m_pInterface; }

    void                                SetInterface( CWaterPolySAInterface *intf );

    EWaterPolyType                      GetType() const                                         { return WATER_POLY_QUAD; }
    size_t                              GetNumVertices() const                                  { return 4; }
};

class CWaterTriangleSA : public CWaterPolySA
{
public:
                                        CWaterTriangleSA()                                      { m_pInterface = NULL; m_wID = ~0; }
                                        CWaterTriangleSA( CWaterPolySAInterface *intf )         { SetInterface( intf ); }

    CWaterTriangleSAInterface*          GetInterface()                                          { return (CWaterTriangleSAInterface*)m_pInterface; }
    const CWaterTriangleSAInterface*    GetInterface() const                                    { return (CWaterTriangleSAInterface*)m_pInterface; }

    void                                SetInterface( CWaterPolySAInterface *intf );

    EWaterPolyType                      GetType() const                                         { return WATER_POLY_TRIANGLE; }
    size_t                              GetNumVertices() const                                  { return 3; }
};

#endif
