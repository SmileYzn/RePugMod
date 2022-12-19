#pragma once

extern IReGameApi			*g_ReGameApi;
extern const ReGameFuncs_t	*g_ReGameFuncs;
extern IReGameHookchains	*g_ReGameHookchains;
extern CGameRules			*g_pGameRules;

extern bool ReGameDLL_Init();
extern bool ReGameDLL_Stop();

extern CGameRules *ReGameDLL_InstallGameRules(IReGameHook_InstallGameRules* chain);
extern bool ReGameDLL_CBasePlayer_GetIntoGame(IReGameHook_CBasePlayer_GetIntoGame* chain, CBasePlayer* Player);
extern BOOL ReGameDLL_HandleMenu_ChooseTeam(IReGameHook_HandleMenu_ChooseTeam* chain, CBasePlayer* Player, int Slot);
extern void ReGameDLL_InternalCommand(IReGameHook_InternalCommand* chain, edict_t* pEntity, const char* pcmd, const char* parg1);
extern void ReGameDLL_CBasePlayer_AddAccount(IReGameHook_CBasePlayer_AddAccount *chain, CBasePlayer* pthis, int amount, RewardType type, bool bTrackChange);
extern void ReGameDLL_CBasePlayer_Killed(IReGameHook_CBasePlayer_Killed* chain, CBasePlayer* pthis, entvars_t* pevAttacker, int iGib);
extern void ReGameDLL_CBasePlayer_SetAnimation(IReGameHook_CBasePlayer_SetAnimation* chain, CBasePlayer* pthis, PLAYER_ANIM playerAnim);
extern bool ReGameDLL_CBasePlayer_HasRestrictItem(IReGameHook_CBasePlayer_HasRestrictItem* chain, CBasePlayer* pthis, ItemID item, ItemRestType type);
extern void ReGameDLL_CSGameRules_GiveC4(IReGameHook_CSGameRules_GiveC4* chain);
extern void ReGameDLL_CSGameRules_OnRoundFreezeEnd(IReGameHook_CSGameRules_OnRoundFreezeEnd* chain);
extern bool ReGameDLL_RoundEnd(IReGameHook_RoundEnd* chain, int winStatus, ScenarioEventEndRound event, float tmDelay);
extern int  ReGameDLL_CBasePlayer_TakeDamage(IReGameHook_CBasePlayer_TakeDamage*chain, CBasePlayer* pthis, entvars_t *pevInflictor, entvars_t *pevAttacker, float& flDamage, int bitsDamageType);
extern void ReGameDLL_CSGameRules_RestartRound(IReGameHook_CSGameRules_RestartRound* chain);
extern bool ReGameDLL_CBasePlayer_MakeBomber(IReGameHook_CBasePlayer_MakeBomber* chain, CBasePlayer* pthis);
extern CBaseEntity* ReGameDLL_CBasePlayer_DropPlayerItem(IReGameHook_CBasePlayer_DropPlayerItem* chain, CBasePlayer* pthis, const char* pszItemName);
extern CGrenade* ReGameDLL_PlantBomb(IReGameHook_PlantBomb* chain, entvars_t* pevOwner, Vector& vecStart, Vector& vecVelocity);
extern void ReGameDLL_CGrenade_DefuseBombStart(IReGameHook_CGrenade_DefuseBombStart* chain, CGrenade* pthis, CBasePlayer* pPlayer);
extern void ReGameDLL_CGrenade_DefuseBombEnd(IReGameHook_CGrenade_DefuseBombEnd* chain, CGrenade* pthis, CBasePlayer* pPlayer, bool bDefused);
extern void ReGameDLL_CGrenade_ExplodeBomb(IReGameHook_CGrenade_ExplodeBomb* chain, CGrenade* pthis, TraceResult* ptr, int bitsDamageType);
extern void ReGameDLL_PlayerBlind(IReGameHook_PlayerBlind* chain, CBasePlayer* pPlayer, entvars_t* pevInflictor, entvars_t* pevAttacker, float fadeTime, float fadeHold, int alpha, Vector& color);