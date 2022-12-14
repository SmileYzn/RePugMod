#include "precompiled.h"

IReGameApi			*g_ReGameApi;
const ReGameFuncs_t	*g_ReGameFuncs;
IReGameHookchains	*g_ReGameHookchains;
CGameRules			*g_pGameRules;

bool ReGameDLL_Init()
{
	const char *szGameDLLModule = GET_GAME_INFO(PLID, GINFO_DLL_FULLPATH);

	if (!szGameDLLModule)
	{
		LOG_CONSOLE(PLID, "[%s] Failed to get GameDLL module", Plugin_info.logtag);
		return false;
	}

	CSysModule *gameModule = Sys_LoadModule(szGameDLLModule);

	if (!gameModule)
	{
		LOG_CONSOLE(PLID, "[%s] Failed to locate GameDLL module", Plugin_info.logtag);
		return false;
	}

	CreateInterfaceFn ifaceFactory = Sys_GetFactory(gameModule);

	if (!ifaceFactory)
	{
		LOG_CONSOLE(PLID, "[%s] Failed to locate interface factory in GameDLL module", Plugin_info.logtag);
		return false;
	}

	int retCode = 0;

	g_ReGameApi = (IReGameApi *)ifaceFactory(VRE_GAMEDLL_API_VERSION, &retCode);

	if (!g_ReGameApi)
	{
		LOG_CONSOLE(PLID, "[%s] Failed to locate retrieve rehlds api interface from GameDLL module, return code is %d", Plugin_info.logtag, retCode);
		return false;
	}

	int majorVersion = g_ReGameApi->GetMajorVersion();
	int minorVersion = g_ReGameApi->GetMinorVersion();

	if (majorVersion != REGAMEDLL_API_VERSION_MAJOR)
	{
		LOG_CONSOLE(PLID, "[%s] ReGameDLL API major version mismatch; expected %d, real %d", Plugin_info.logtag, REGAMEDLL_API_VERSION_MAJOR, majorVersion);

		if (majorVersion < REGAMEDLL_API_VERSION_MAJOR)
		{
			LOG_CONSOLE(PLID, "[%s] Please update the ReGameDLL up to a major version API >= %d", Plugin_info.logtag, REGAMEDLL_API_VERSION_MAJOR);
		}
		else if (majorVersion > REGAMEDLL_API_VERSION_MAJOR)
		{
			LOG_CONSOLE(PLID, "[%s] Please update the %s up to a major version API >= %d", Plugin_info.logtag, Plugin_info.logtag, majorVersion);
		}

		return false;
	}

	if (minorVersion < REGAMEDLL_API_VERSION_MINOR)
	{
		LOG_CONSOLE(PLID, "[%s] ReGameDLL API minor version mismatch; expected at least %d, real %d", Plugin_info.logtag, REGAMEDLL_API_VERSION_MINOR, minorVersion);
		LOG_CONSOLE(PLID, "[%s] Please update the ReGameDLL up to a minor version API >= %d", Plugin_info.logtag, REGAMEDLL_API_VERSION_MINOR);
		return false;
	}

	g_ReGameFuncs = g_ReGameApi->GetFuncs();

	g_ReGameHookchains = g_ReGameApi->GetHookchains();

	if (!g_ReGameApi->BGetICSEntity(CSENTITY_API_INTERFACE_VERSION))
	{
		LOG_CONSOLE(PLID, "[%s] Interface CCSEntity API version '%s' not found", Plugin_info.logtag, CSENTITY_API_INTERFACE_VERSION);
		return false;
	}

	LOG_CONSOLE(PLID, "[%s] Re-GameDLL API successfully initialized.", Plugin_info.logtag);

	g_ReGameHookchains->InstallGameRules()->registerHook(ReGameDLL_InstallGameRules);

	g_ReGameHookchains->CBasePlayer_GetIntoGame()->registerHook(ReGameDLL_CBasePlayer_GetIntoGame);

	g_ReGameHookchains->HandleMenu_ChooseTeam()->registerHook(ReGameDLL_HandleMenu_ChooseTeam);

	g_ReGameHookchains->InternalCommand()->registerHook(ReGameDLL_InternalCommand);

	g_ReGameHookchains->CBasePlayer_AddAccount()->registerHook(ReGameDLL_CBasePlayer_AddAccount);

	g_ReGameHookchains->CBasePlayer_HasRestrictItem()->registerHook(ReGameDLL_CBasePlayer_HasRestrictItem);

	g_ReGameHookchains->CSGameRules_GiveC4()->registerHook(ReGameDLL_CSGameRules_GiveC4);

	g_ReGameHookchains->CSGameRules_OnRoundFreezeEnd()->registerHook(ReGameDLL_CSGameRules_OnRoundFreezeEnd);

	g_ReGameHookchains->RoundEnd()->registerHook(ReGameDLL_RoundEnd);

	g_ReGameHookchains->CBasePlayer_TakeDamage()->registerHook(ReGameDLL_CBasePlayer_TakeDamage);

	g_ReGameHookchains->CBasePlayer_Killed()->registerHook(ReGameDLL_CBasePlayer_Killed);

	g_ReGameHookchains->CBasePlayer_SetAnimation()->registerHook(ReGameDLL_CBasePlayer_SetAnimation);

	g_ReGameHookchains->CSGameRules_RestartRound()->registerHook(ReGameDLL_CSGameRules_RestartRound);

	g_ReGameHookchains->CBasePlayer_MakeBomber()->registerHook(ReGameDLL_CBasePlayer_MakeBomber);

	g_ReGameHookchains->CBasePlayer_DropPlayerItem()->registerHook(ReGameDLL_CBasePlayer_DropPlayerItem);

	g_ReGameHookchains->PlantBomb()->registerHook(ReGameDLL_PlantBomb);

	g_ReGameHookchains->CGrenade_DefuseBombStart()->registerHook(ReGameDLL_CGrenade_DefuseBombStart);

	g_ReGameHookchains->CGrenade_DefuseBombEnd()->registerHook(ReGameDLL_CGrenade_DefuseBombEnd);

	g_ReGameHookchains->CGrenade_ExplodeBomb()->registerHook(ReGameDLL_CGrenade_ExplodeBomb);

	g_ReGameHookchains->PlayerBlind()->registerHook(ReGameDLL_PlayerBlind);

	return true;
}

bool ReGameDLL_Stop()
{
	g_ReGameHookchains->InstallGameRules()->unregisterHook(ReGameDLL_InstallGameRules);

	g_ReGameHookchains->CBasePlayer_GetIntoGame()->unregisterHook(ReGameDLL_CBasePlayer_GetIntoGame);

	g_ReGameHookchains->HandleMenu_ChooseTeam()->unregisterHook(ReGameDLL_HandleMenu_ChooseTeam);

	g_ReGameHookchains->InternalCommand()->unregisterHook(ReGameDLL_InternalCommand);

	g_ReGameHookchains->CBasePlayer_AddAccount()->unregisterHook(ReGameDLL_CBasePlayer_AddAccount);

	g_ReGameHookchains->CBasePlayer_HasRestrictItem()->unregisterHook(ReGameDLL_CBasePlayer_HasRestrictItem);

	g_ReGameHookchains->CSGameRules_GiveC4()->unregisterHook(ReGameDLL_CSGameRules_GiveC4);

	g_ReGameHookchains->CSGameRules_OnRoundFreezeEnd()->unregisterHook(ReGameDLL_CSGameRules_OnRoundFreezeEnd);

	g_ReGameHookchains->RoundEnd()->unregisterHook(ReGameDLL_RoundEnd);

	g_ReGameHookchains->CBasePlayer_TakeDamage()->unregisterHook(ReGameDLL_CBasePlayer_TakeDamage);

	g_ReGameHookchains->CBasePlayer_Killed()->unregisterHook(ReGameDLL_CBasePlayer_Killed);

	g_ReGameHookchains->CBasePlayer_SetAnimation()->unregisterHook(ReGameDLL_CBasePlayer_SetAnimation);

	g_ReGameHookchains->CSGameRules_RestartRound()->unregisterHook(ReGameDLL_CSGameRules_RestartRound);

	g_ReGameHookchains->CBasePlayer_MakeBomber()->unregisterHook(ReGameDLL_CBasePlayer_MakeBomber);

	g_ReGameHookchains->CBasePlayer_DropPlayerItem()->unregisterHook(ReGameDLL_CBasePlayer_DropPlayerItem);

	g_ReGameHookchains->PlantBomb()->unregisterHook(ReGameDLL_PlantBomb);

	g_ReGameHookchains->CGrenade_DefuseBombStart()->unregisterHook(ReGameDLL_CGrenade_DefuseBombStart);

	g_ReGameHookchains->CGrenade_DefuseBombEnd()->unregisterHook(ReGameDLL_CGrenade_DefuseBombEnd);

	g_ReGameHookchains->CGrenade_ExplodeBomb()->unregisterHook(ReGameDLL_CGrenade_ExplodeBomb);

	g_ReGameHookchains->PlayerBlind()->unregisterHook(ReGameDLL_PlayerBlind);

	return true;
}

CGameRules *ReGameDLL_InstallGameRules(IReGameHook_InstallGameRules *chain)
{
	auto gamerules = chain->callNext();

	if (!g_ReGameApi->BGetIGameRules(GAMERULES_API_INTERFACE_VERSION))
	{
		LOG_CONSOLE(PLID, "[%s] Interface GameRules API version '%s' not found", Plugin_info.logtag, GAMERULES_API_INTERFACE_VERSION);
	}
	else
	{
		g_pGameRules = gamerules;
	}
	
	return gamerules;
}

bool ReGameDLL_CBasePlayer_GetIntoGame(IReGameHook_CBasePlayer_GetIntoGame* chain, CBasePlayer* Player)
{
	bool ret = chain->callNext(Player);

	gReady.ClientGetIntoGame(Player);

	return ret;
}

BOOL ReGameDLL_HandleMenu_ChooseTeam(IReGameHook_HandleMenu_ChooseTeam* chain, CBasePlayer* Player, int Slot)
{
	if (gPugMod.ClientJoinTeam(Player, Slot))
	{
		Slot = 0;
	}

	return chain->callNext(Player, Slot);
}

void ReGameDLL_InternalCommand(IReGameHook_InternalCommand* chain, edict_t* pEntity, const char* pcmd, const char* parg1)
{
	auto Player = UTIL_PlayerByIndexSafe(ENTINDEX(pEntity));

	if (Player)
	{
		if (gAntiFlood.ClientCommand(Player, pcmd, parg1))
		{
			return;
		}

		if (gCommands.ClientCommand(Player, pcmd, parg1))
		{
			return;
		}
	}

	chain->callNext(pEntity, pcmd, parg1);
}

void ReGameDLL_CBasePlayer_AddAccount(IReGameHook_CBasePlayer_AddAccount *chain, CBasePlayer* pthis, int amount, RewardType type, bool bTrackChange)
{
	if (gPugMod.ClientAddAccount(pthis, amount, type, bTrackChange))
	{
		amount = 0;
	}

	gStats.AddAccount(pthis, amount, type, bTrackChange);

	chain->callNext(pthis, amount, type, bTrackChange);
}

void ReGameDLL_CBasePlayer_Killed(IReGameHook_CBasePlayer_Killed* chain, CBasePlayer* pthis, entvars_t* pevAttacker, int iGib)
{
	gStats.Killed(pthis, pevAttacker, iGib);

	chain->callNext(pthis, pevAttacker, iGib);
}

bool ReGameDLL_CBasePlayer_HasRestrictItem(IReGameHook_CBasePlayer_HasRestrictItem *chain, CBasePlayer* pthis, ItemID item, ItemRestType type)
{
	auto ret = chain->callNext(pthis, item, type);

	if (gPugMod.ClientHasRestrictItem(pthis, item, type))
	{
		return true;
	}

	if (gKnifeRound.ClientHasRestrictItem(pthis, item, type))
	{
		return true;
	}

	return ret;
}

void ReGameDLL_CSGameRules_GiveC4(IReGameHook_CSGameRules_GiveC4* chain)
{
	if (!gKnifeRound.IsRunning())
	{
		chain->callNext();
	}
}

void ReGameDLL_CSGameRules_OnRoundFreezeEnd(IReGameHook_CSGameRules_OnRoundFreezeEnd* chain)
{
	chain->callNext();

	gPugMod.RoundStart();

	gVotePause.RoundStart();

	gStatsCommand.RoundStart();

	gStats.RoundFreezeEnd();
}

bool ReGameDLL_RoundEnd(IReGameHook_RoundEnd* chain, int winStatus, ScenarioEventEndRound event, float tmDelay)
{
	auto ret = chain->callNext(winStatus, event, tmDelay);

	gStats.RoundEnd(winStatus, event, tmDelay);

	gKnifeRound.RoundEnd(winStatus, event, tmDelay);

	gPugMod.RoundEnd(winStatus, event, tmDelay);

	return ret;
}

int ReGameDLL_CBasePlayer_TakeDamage(IReGameHook_CBasePlayer_TakeDamage *chain, CBasePlayer* pthis, entvars_t *pevInflictor, entvars_t *pevAttacker, float& flDamage, int bitsDamageType)
{
	int ret = chain->callNext(pthis, pevInflictor, pevAttacker, flDamage, bitsDamageType);

	gStatsCommand.TakeDamage(pthis, pevInflictor, pevAttacker, flDamage, bitsDamageType);

	gStats.TakeDamage(pthis, pevInflictor, pevAttacker, flDamage, bitsDamageType);

	return ret;
}

void ReGameDLL_CBasePlayer_SetAnimation(IReGameHook_CBasePlayer_SetAnimation* chain, CBasePlayer* pthis, PLAYER_ANIM playerAnim)
{
	chain->callNext(pthis, playerAnim);

	gStats.SetAnimation(pthis, playerAnim);
}

void ReGameDLL_CSGameRules_RestartRound(IReGameHook_CSGameRules_RestartRound * chain)
{
	gPugMod.RoundRestart(true);

	gStats.RoundRestart();

	chain->callNext();

	gPugMod.RoundRestart(false);

	gVotePause.RoundRestart();
}

bool ReGameDLL_CBasePlayer_MakeBomber(IReGameHook_CBasePlayer_MakeBomber* chain, CBasePlayer* pthis)
{
	auto ret = chain->callNext(pthis);

	gStats.MakeBomber(pthis);

	return ret;
}

CBaseEntity* ReGameDLL_CBasePlayer_DropPlayerItem(IReGameHook_CBasePlayer_DropPlayerItem* chain, CBasePlayer* pthis, const char* pszItemName)
{
	auto ret = chain->callNext(pthis, pszItemName);

	if (pszItemName && pszItemName[0] != '\0')
	{
		if (Q_strcmp(pszItemName, "weapon_c4") == 0)
		{
			gStats.DropBomb(pthis);
		}
	}

	return ret;
}

CGrenade* ReGameDLL_PlantBomb(IReGameHook_PlantBomb* chain, entvars_t* pevOwner, Vector& vecStart, Vector& vecVelocity)
{
	gStats.PlantBomb(pevOwner, false);

	auto ret = chain->callNext(pevOwner, vecStart, vecVelocity);

	gStats.PlantBomb(pevOwner, true);

	return ret;
}

void ReGameDLL_CGrenade_DefuseBombStart(IReGameHook_CGrenade_DefuseBombStart* chain, CGrenade* pthis, CBasePlayer* pPlayer)
{
	chain->callNext(pthis, pPlayer);

	gStats.DefuseBombStart(pPlayer);
}

void ReGameDLL_CGrenade_DefuseBombEnd(IReGameHook_CGrenade_DefuseBombEnd* chain, CGrenade* pthis, CBasePlayer* pPlayer, bool bDefused)
{
	chain->callNext(pthis, pPlayer, bDefused);

	gStats.DefuseBombEnd(pPlayer, bDefused);
}

void ReGameDLL_CGrenade_ExplodeBomb(IReGameHook_CGrenade_ExplodeBomb* chain, CGrenade* pthis, TraceResult* ptr, int bitsDamageType)
{
	chain->callNext(pthis, ptr, bitsDamageType);

	gStats.ExplodeBomb(pthis, ptr, bitsDamageType);
}

void ReGameDLL_PlayerBlind(IReGameHook_PlayerBlind* chain, CBasePlayer* pPlayer, entvars_t* pevInflictor, entvars_t* pevAttacker, float fadeTime, float fadeHold, int alpha, Vector& color)
{
	chain->callNext(pPlayer, pevInflictor, pevAttacker, fadeTime, fadeHold, alpha, color);

	gStats.PlayerBlind(pPlayer, pevInflictor, pevAttacker, fadeTime, fadeHold, alpha, color);
}