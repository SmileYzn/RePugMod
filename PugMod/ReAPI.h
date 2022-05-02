#pragma once

extern IRehldsApi*			g_RehldsApi;
extern const RehldsFuncs_t* g_RehldsFuncs;
extern IRehldsServerData*	g_RehldsData;
extern IRehldsHookchains*	g_RehldsHookchains;
extern IRehldsServerStatic* g_RehldsSvs;

extern bool ReAPI_Init();
extern bool ReAPI_Stop();

extern void ReAPI_SV_Frame(IRehldsHook_SV_Frame* chain);
extern void ReAPI_ClientConnected(IRehldsHook_ClientConnected* chain,IGameClient* client);
extern void ReAPI_SV_DropClient(IRehldsHook_SV_DropClient * chain, IGameClient * client, bool crash, const char* Reason);