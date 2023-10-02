/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CColPointSA.h
*  PURPOSE:     Header file for collision point class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_COLPOINT
#define __CGAMESA_COLPOINT

#include <game/CColPoint.h>
#include <CVector.h>

class CColPointSAInterface
{
public:
	CVector         Position;       // 0
	float           fUnknown1;      // 12 
	CVector         Normal;         // 16
	float           fUnknown2;      // 28
	EColSurface     ucSurfaceTypeA; // 32
	uint8           ucPieceTypeA;   // 33
	CColLighting    lightingA;      // 34
	EColSurface     ucSurfaceTypeB; // 35
	uint8           ucPieceTypeB;   // 36
	CColLighting    lightingB;      // 37
	uint8           pad1;           // 38
	uint8           pad2;           // 39
	float           fDepth;         // 40
};

class CColPointSA : public CColPoint
{
private:
    CColPointSAInterface * internalInterface;
public:
    CColPointSAInterface * GetInterface() { return this->internalInterface; }
    CColPointSA();
    CColPointSA(CColPointSAInterface * pInterface ) { this->internalInterface = pInterface; };

    CVector * GetPosition()  { return &this->GetInterface()->Position; };
    VOID SetPosition(CVector * vecPosition)  { MemCpyFast (&this->GetInterface()->Position, vecPosition, sizeof(CVector)); };

    CVector * GetNormal() { return &this->GetInterface()->Normal; };
    VOID SetNormal(CVector * vecNormal) { MemCpyFast (&this->GetInterface()->Normal, vecNormal, sizeof(CVector)); };

    BYTE GetSurfaceTypeA() { return this->GetInterface()->ucSurfaceTypeA; };
    BYTE GetSurfaceTypeB() { return this->GetInterface()->ucSurfaceTypeB; };

    VOID SetSurfaceTypeA(BYTE bSurfaceType) { this->GetInterface()->ucSurfaceTypeA = (EColSurfaceValue)bSurfaceType; };
    VOID SetSurfaceTypeB(BYTE bSurfaceType) { this->GetInterface()->ucSurfaceTypeB = (EColSurfaceValue)bSurfaceType; };

    BYTE GetPieceTypeA() { return this->GetInterface()->ucPieceTypeA; };
    BYTE GetPieceTypeB() { return this->GetInterface()->ucPieceTypeB; };

    VOID SetPieceTypeA(BYTE bPieceType) { this->GetInterface()->ucPieceTypeA = bPieceType; };
    VOID SetPieceTypeB(BYTE bPieceType) { this->GetInterface()->ucPieceTypeB = bPieceType; };

    BYTE GetLightingA() { return *(BYTE*)&this->GetInterface()->lightingA; };
    BYTE GetLightingB() { return *(BYTE*)&this->GetInterface()->lightingB; };

    VOID SetLightingA(BYTE bLighting) { *(BYTE*)&this->GetInterface()->lightingA = bLighting; };
    VOID SetLightingB(BYTE bLighting) { *(BYTE*)&this->GetInterface()->lightingB = bLighting; };

    FLOAT GetDepth() { return this->GetInterface()->fDepth; };
    VOID SetDepth(FLOAT fDepth) { this->GetInterface()->fDepth = fDepth; };

    VOID Destroy() { if ( this->internalInterface ) delete this->internalInterface; delete this; }
};

#endif
