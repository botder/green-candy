// Multi-threading plugins for the MTA:Lua core.
#include "luacore.h"

#include "lstate.h"

#include "lmultithreading.hxx"

mtEnvConnectingBridge_t mtEnvConnectingBridge;

void luaMT_libraryinit( void )
{
    mtEnvConnectingBridge.RegisterPluginStruct( namespaceFactory );
}

void luaMT_libraryshutdown( void )
{
    mtEnvConnectingBridge.UnregisterPluginStruct();
}
