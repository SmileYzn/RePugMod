#pragma once

extern IReGameApi			*g_ReGameApi;
extern const ReGameFuncs_t	*g_ReGameFuncs;
extern IReGameHookchains	*g_ReGameHookchains;
extern CGameRules			*g_pGameRules;

extern bool ReGameDLL_Init();
extern bool ReGameDLL_Stop();

extern CGameRules *ReGameDLL_InstallGameRules(IReGameHook_InstallGameRules *chain);
extern bool ReGameDLL_CBasePlayer_GetIntoGame(IReGameHook_CBasePlayer_GetIntoGame* chain, CBasePlayer * Player);
extern void ReGameDLL_InternalCommand(IReGameHook_InternalCommand* chain, edict_t* pEntity, const char* pcmd, const char* parg1);
extern void ReGameDLL_CBasePlayer_AddAccount(IReGameHook_CBasePlayer_AddAccount *chain, CBasePlayer* pthis, int amount, RewardType type, bool bTrackChange);
extern bool ReGameDLL_CBasePlayer_HasRestrictItem(IReGameHook_CBasePlayer_HasRestrictItem* chain, CBasePlayer* pthis, ItemID item, ItemRestType type);
extern void ReGameDLL_CSGameRules_GiveC4(IReGameHook_CSGameRules_GiveC4* chain);
extern void ReGameDLL_CSGameRules_OnRoundFreezeEnd(IReGameHook_CSGameRules_OnRoundFreezeEnd* chain);
extern bool ReGameDLL_RoundEnd(IReGameHook_RoundEnd* chain, int winStatus, ScenarioEventEndRound event, float tmDelay);
extern int  ReGameDLL_CBasePlayer_TakeDamage(IReGameHook_CBasePlayer_TakeDamage*chain, CBasePlayer* pthis, entvars_t *pevInflictor, entvars_t *pevAttacker, float& flDamage, int bitsDamageType);
extern void ReGameDLL_CSGameRules_RestartRound(IReGameHook_CSGameRules_RestartRound* chain);