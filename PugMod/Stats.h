#pragma once

#define MANAGER_ASSISTANCE_DMG  50		// Mininum damage to take assistance for player
#define MANAGER_RWS_MAP_TARGET  0.7f	// Round Win Share: Amount added to each player of winner team if map objective is complete by winner team
#define MANAGER_RWS_C4_EXPLODE  0.3f	// Round Win Share: Extra amount added to player from winner team that planted the bomb and bomb explode
#define MANAGER_RWS_C4_DEFUSED  0.3f	// Round Win Share: Extra amount added to player from winner team that defused the bomb

class CStats
{
public:
	void Clear();

	void GetIntoGame(CBasePlayer* Player);
	void Disconnected(edict_t* pEdict);
	void AddAccount(CBasePlayer* Player, int amount, RewardType type, bool bTrackChange);
	void TakeDamage(CBasePlayer* Player, entvars_t* pevInflictor, entvars_t* pevAttacker, float& flDamage, int bitsDamageType);
	void Killed(CBasePlayer* Player, entvars_t* pevAttacker, int iGib);
	void SetAnimation(CBasePlayer* Player, PLAYER_ANIM playerAnim);
	void PlantBomb(entvars_t* pevOwner, bool Planted);
	void DefuseBombStart(CBasePlayer* Player);
	void DefuseBombEnd(CBasePlayer* Player, bool Defused);
	void ExplodeBomb(CGrenade* pThis, TraceResult* ptr, int bitsDamageType);
	void RoundFreezeEnd();
	void RoundEnd(int winStatus, ScenarioEventEndRound event, float tmDelay);

	int GetRoundHits(int AttackerIndex, int TargetIndex);
	int GetRoundDamage(int AttackerIndex, int TargetIndex);

	CPlayerStats GetData(int EntityIndex);

private:
	int GetActiveWeapon(CBasePlayer* Player, bool AllowKnife);

	CPlayerStats m_Data[MAX_CLIENTS + 1];

	std::map<std::string, CPlayerStats> m_Stats;

	int m_RoundHits[MAX_CLIENTS + 1][MAX_CLIENTS + 1] = { 0 };
	int m_RoundDamage[MAX_CLIENTS + 1][MAX_CLIENTS + 1] = { 0 };
	int m_RoundDamageSelf[MAX_CLIENTS + 1];
	int m_RoundDamageTeam[SPECTATOR + 1] = { 0 };
	int m_RoundFrags[MAX_CLIENTS + 1] = { 0 };
	int m_RoundVersus[MAX_CLIENTS + 1] = { 0 };
	int m_RoundBombPlanter = -1;
	int m_RoundBombDefuser = -1;
};

extern CStats gStats;
