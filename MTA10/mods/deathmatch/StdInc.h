#pragma message("Compiling precompiled header.\n")

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <mmsystem.h>
#include <winsock.h>

#include <dbgheap.h>

#include <algorithm>
#include <list>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <vector>
#include <cstdio>
#include <cstring>

// SDK includes
#define MTA_CLIENT
#include "SharedUtil.h"
#include <utils/CBuffer.h>
#include <sdk/MemoryUtils.h>
#include <core/CCoreInterface.h>
#include <core/CExceptionInformation.h>
#include <xml/CXML.h>
#include <xml/CXMLNode.h>
#include <xml/CXMLFile.h>
#include <xml/CXMLAttributes.h>
#include <xml/CXMLAttribute.h>
#include <net/CNet.h>
#include <net/packetenums.h>
#include <game/CGame.h>
#include <CVector.h>
#include <CSphere.h>
#include <CBox.h>
#include <ijsify.h>
#include <Common.h>
#include <CQuat.h>
#include "net/SyncStructures.h"

// Shared logic includes
#include <Utils.h>
#include <logic/include.h>
#include <lua/CLuaElement.h>
#include <CClientCommon.h>
#include <CClientManager.h>
#include <CClient3DMarker.h>
#include <CClientCheckpoint.h>
#include <CClientColShape.h>
#include <CClientColCircle.h>
#include <CClientColCuboid.h>
#include <CClientColSphere.h>
#include <CClientColRectangle.h>
#include <CClientColPolygon.h>
#include <CClientColTube.h>
#include <CClientCorona.h>
#include <CClientRwObject.h>
#include <CClientRwFrame.h>
#include <CClientAtomic.h>
#include <CClientLight.h>
#include <CClientRwCamera.h>
#include <CClientDFF.h>
#include <CClientColModel.h>
#include <CClientDummy.h>
#include <CClientEntity.h>
#include <derived/include.h>    // DERIVED
#include <CClientVehicleComponent.h>
#include <CClientVehicle.h>
#include <CClientSpatialDatabase.h>
#include <CClientExplosionManager.h>
#include <CClientPedBase.h>
#include <CClientPed.h>
#include <CClientObject.h>
#include <CClientPlayerClothes.h>
#include <CClientPlayerVoice.h>
#include <CClientProjectileManager.h>
#include <CClientStreamSector.h>
#include <CClientStreamSectorRow.h>
#include <CClientTask.h>
#include <CClientTXD.h>
#include <CClientWater.h>
#include <CClientRenderElement.h>
#include <CClientDxFont.h>
#include <CClientGuiFont.h>
#include <CClientMaterial.h>
#include <CClientGameTexture.h>
#include <CClientTexture.h>
#include <CClientShader.h>
#include <CElementArray.h>
#include <CMapEventManager.h>
#include <CModelNames.h>
#include <CWeaponNames.h>
#include <CVehicleNames.h>
#include <lua/CLuaCFunctions.h>
#include <lua/CLuaArguments.h>
#include <lua/CLuaMain.h>
#include <animation/CEasingCurve.h>
#include <lua/CLuaFunctionParseHelpers.h>
#include <CScriptArgReader.h>
#include <luadefs/CLuaDefs.h>
#include <luadefs/CLuaTaskDefs.h>
#include <luadefs/CLuaFxDefs.h>

// Shared includes
#include <animation/TInterpolation.h>
#include <animation/CPositionRotationAnimation.h>

// Deathmatch includes
#include "Client.h"
#include "ClientCommands.h"
#include "CClient.h"
#include "CEvents.h"
#include "HeapTrace.h"
#include "logic/CClientGame.h"
#include "net/Packets.h"
#include "logic/CClientEntityRefManager.h"
#include "logic/CDeathmatchVehicle.h"
#include "logic/CClientPerfStatManager.h"
#include "logic/CResource.h"
#include "logic/CStaticFunctionDefinitions.h"
#include "../../version.h"