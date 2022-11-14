#include "precompiled.h"

#include "sys_shared.cpp"
#include "interface.cpp"

IRehldsApi*				g_RehldsApi;
const RehldsFuncs_t*	g_RehldsFuncs;
IRehldsServerData*		g_RehldsData;
IRehldsHookchains*		g_RehldsHookchains;
IRehldsServerStatic*	g_RehldsSvs;

bool ReAPI_Init()
{
	if (!IS_DEDICATED_SERVER())
	{
		LOG_CONSOLE(PLID, "[%s] ReHLDS API Can only run on Half-Life Dedicated Server");
		return false;
	}

#ifdef WIN32
	CSysModule* engineModule = Sys_LoadModule("swds.dll");
#else
	CSysModule* engineModule = Sys_LoadModule("engine_i486.so");
#endif

	if (!engineModule)
	{
		LOG_CONSOLE(PLID, "[%s] Failed to locate engine module", Plugin_info.logtag);
		return false;
	}

	CreateInterfaceFn ifaceFactory = Sys_GetFactory(engineModule);

	if (!ifaceFactory)
	{
		LOG_CONSOLE(PLID, "[%s] Failed to locate interface factory in engine module", Plugin_info.logtag);
		return false;
	}

	int retCode = 0;

	g_RehldsApi = (IRehldsApi*)ifaceFactory(VREHLDS_HLDS_API_VERSION, &retCode);

	if (!g_RehldsApi)
	{
		LOG_CONSOLE(PLID, "[%s] Failed to locate retrieve rehlds api interface from engine module, return code is %d", Plugin_info.logtag, retCode);
		return false;
	}

	int majorVersion = g_RehldsApi->GetMajorVersion();
	int minorVersion = g_RehldsApi->GetMinorVersion();

	if (majorVersion != REHLDS_API_VERSION_MAJOR)
	{
		LOG_CONSOLE(PLID, "[%s] ReHLDS API major version mismatch; expected %d, real %d", Plugin_info.logtag, REHLDS_API_VERSION_MAJOR, majorVersion);

		if (majorVersion < REHLDS_API_VERSION_MAJOR)
		{
			LOG_CONSOLE(PLID, "[%s] Please update the ReHLDS up to a major version API >= %d", Plugin_info.logtag, REHLDS_API_VERSION_MAJOR);
		}

		else if (majorVersion > REHLDS_API_VERSION_MAJOR)
		{
			LOG_CONSOLE(PLID, "[%s] Please update the %s up to a major version API >= %d", Plugin_info.logtag, Plugin_info.logtag, majorVersion);
		}

		return false;
	}

	if (minorVersion < REHLDS_API_VERSION_MINOR)
	{
		LOG_CONSOLE(PLID, "[%s] ReHLDS API minor version mismatch; expected at least %d, real %d", Plugin_info.logtag, REHLDS_API_VERSION_MINOR, minorVersion);
		LOG_CONSOLE(PLID, "[%s] Please update the ReHLDS up to a minor version API >= %d", Plugin_info.logtag, REHLDS_API_VERSION_MINOR);
		return false;
	}

	g_RehldsFuncs = g_RehldsApi->GetFuncs();

	g_RehldsData = g_RehldsApi->GetServerData();

	g_RehldsHookchains = g_RehldsApi->GetHookchains();

	g_RehldsSvs = g_RehldsApi->GetServerStatic();

	LOG_CONSOLE(PLID, "[%s] Re-HLDS API successfully initialized.", Plugin_info.logtag);

	g_RehldsHookchains->SV_Frame()->registerHook(ReAPI_SV_Frame);

	g_RehldsHookchains->ClientConnected()->registerHook(ReAPI_ClientConnected);

	g_RehldsHookchains->SV_DropClient()->registerHook(ReAPI_SV_DropClient);

	gLibCurl.Init();

	return true;
}

bool ReAPI_Stop()
{
	g_RehldsHookchains->SV_Frame()->unregisterHook(ReAPI_SV_Frame);

	g_RehldsHookchains->ClientConnected()->unregisterHook(ReAPI_ClientConnected);

	g_RehldsHookchains->SV_DropClient()->unregisterHook(ReAPI_SV_DropClient);

	gLibCurl.Stop();

	return true;
}

void ReAPI_SV_Frame(IRehldsHook_SV_Frame* chain)
{
	chain->callNext();

	gTask.Think();

	gLibCurl.Frame();
}

void ReAPI_ClientConnected(IRehldsHook_ClientConnected* chain, IGameClient* client)
{
	chain->callNext(client);

	gPugMod.ClientConnected(client->GetEdict());

	gAntiRetry.ClientConnected(client->GetEdict());

	gWebapi.ClientConnected(client->GetEdict());
}

void ReAPI_SV_DropClient(IRehldsHook_SV_DropClient* chain, IGameClient* client, bool crash, const char* Reason)
{
	auto pEntity = client->GetEdict();

	if (!FNullEnt(pEntity))
	{
		auto EntityIndex = ENTINDEX(pEntity);

		if (EntityIndex)
		{
			gReady.ClientDisconnected(EntityIndex);

			gCaptain.ClientDisconnected(EntityIndex);

			gVoteKick.ClientDisconnected(EntityIndex);

			gVoteLevel.ClientDisconnected(EntityIndex);

			gVotePause.ClientDisconnected(EntityIndex);

			gVoteStop.ClientDisconnected(EntityIndex);

			gAntiRetry.ClientDisconnected(pEntity, Reason);

			gPugMod.ClientDisconnected(EntityIndex);

			gStats.Disconnected(EntityIndex);
		}
	}

	chain->callNext(client, crash, Reason);
}