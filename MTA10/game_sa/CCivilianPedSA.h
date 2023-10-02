/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CCivilianPedSA.h
*  PURPOSE:     Header file for civilian ped entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_CIVILIANPED
#define __CGAMESA_CIVILIANPED

#include <game/CCivilianPed.h>

#define FUNC_ClearWeaponTarget          0x533B30

#define FUNC_CPedOperatorNew            0x5E4720 // ##SA##
#define FUNC_CCivilianPedConstructor    0x5DDB70 // ##SA##

#define FUNC_CCivilianPedDestructor     0x5DDBE0 // ##SA##
#define FUNC_CCivilianPedOperatorDelete 0x5E4760 // ##SA##

#define SIZEOF_CCIVILIANPED             1948

// TODO: sort out interfacing issues
class CCivilianPedSAInterface : public CPedSAInterface 
{
public:

};

class CCivilianPedSA : public virtual CCivilianPed, public CPedSA
{
private:

public:
                                        CCivilianPedSA      ( CCivilianPedSAInterface *ped, modelId_t modelID );
                                        ~CCivilianPedSA     ();

    bool                                IsManagedByGame     ( void ) const                  { return true; }
};

#endif
