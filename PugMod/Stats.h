#pragma once

#define GET_AUTH(Edict) !(Edict->v.flags & FL_FAKECLIENT) ? GETPLAYERAUTHID(Edict) : STRING(Edict->v.netname)

// Round Win Share
constexpr auto MANAGER_ASSISTANCE_DMG = 50.0f;	// Mininum damage to take assistance for player
constexpr auto MANAGER_RWS_MAP_TARGET = 0.70f;	// Round Win Share: Amount added to each player of winner team if map objective is complete by winner team
constexpr auto MANAGER_RWS_C4_EXPLODE = 0.30f;	// Round Win Share: Extra amount added to player from winner team that planted the bomb and bomb explode
constexpr auto MANAGER_RWS_C4_DEFUSED = 0.30f;	// Round Win Share: Extra amount added to player from winner team that defused the bomb;

/**
* Stats Group
*/
enum Stats
{
	STATS_FRAGS = 0,	// [Ok] Frags
	STATS_ASSISTS = 1,	// [Ok] Assists
	STATS_DEATHS = 2,	// [Ok] Deaths
	STATS_HEADSHOTS = 3,	// [Ok] Headshots
	STATS_SHOTS = 4,	// [Ok] Shots
	STATS_HITS = 5,	// [Ok] Hits
	STATS_DMG_DONE = 6,	// [Ok] Damage done to other players
	STATS_DMG_RECV = 7,	// [Ok] Damage received from other players
	STATS_FLASH_ASSIST = 8,	// [Ok] Flash assistences that caused a enemy kill
	STATS_FR_FRAGS = 9,	// [Ok] First player that kills someone in round
	STATS_FR_DEATHS = 10,	// [Ok] First player that has ben killed by someone
	STAST_TR_FRAGS = 11,	// [] TO DO: Player that kill enemy while other teammate die to give frag to him
	STATS_TR_DEATHS = 12,	// [] TO DO: Player that was dead while other teammate kill her assasin
	STATS_LAST = 13,	// [] Unused
};

/**
* Bomb Group
*/
enum Bomb
{
	BOMB_CARRIER = 0,		// Spawns with bomb
	BOMB_DROPPED = 1,		// Bomb dropped
	BOMB_PLANTING = 2,		// Try to plant
	BOMB_PLANTED = 3,		// Bomb plants
	BOMB_EXPLODED = 4,		// Bomb explosions
	BOMB_DEFUSING = 5,		// Try to defuse
	BOMB_DEFUSED = 6,		// Bomb defused
	BOMB_DEFUSING_KIT = 7,	// Try to defuse (With defuse kit)
	BOMB_DEFUSED_KIT = 8,   // Bomb Defused (With defuse kit)
	BOMB_LAST = 9,			// Unused
};

/**
* Round Group
*/
enum Round
{
	ROUND_PLAY = 0,	// Rounds Play
	ROUND_WIN_TR = 1,	// Rounds win as TR
	ROUND_LOSE_TR = 2,	// Rounds lose as TR
	ROUND_WIN_CT = 3,	// Rounds win as CT
	ROUND_LOSE_CT = 4,	// Rounds lose as CT
	ROUND_LAST = 5,	// Unused
};

/**
* Hitbox Group (Hitbox places for hits and damage count)
*/
enum HitBox
{
	HITGROUP_GENERIC = 0, // Generic
	HITGROUP_HEAD = 1, // Head
	HITGROUP_CHEST = 2, // Chest
	HITGROUP_STOMACH = 3, // Stomach
	HITGROUP_LEFTARM = 4, // Left Arm
	HITGROUP_RIGHTARM = 5, // Right Arm
	HITGROUP_LEFTLEG = 6, // Left Leg
	HITGROUP_RIGHTLEG = 7, // Right Leg
	HITGROUP_SHIELD = 8, // Shield
	HITGROUP_LAST = 9, // Unused
};

/**
* Weapon Stats Group
*/
enum WeaponStats
{
	WEAPON_KILL = 0, // Frags
	WEAPON_ASSISTS = 1, // Assists
	WEAPON_DEATH = 2, // Deaths
	WEAPON_HEADSHOT = 3, // Headshots
	WEAPON_SHOT = 4, // Shots
	WEAPON_HIT = 5, // Hits
	WEAPON_DAMAGE = 6, // Damage
	WEAPON_DAMAGE_R = 7, // Damage Received
	WEAPON_LAST = 8, // Unused
};

/**
* Hack Stats Group
*/
enum HackStats
{
	HACK_VISION = 0, // Killed with some object bettwen killer and victim vision
	HACK_ONEHIT = 3, // One hit shot (Can be with any weapon except AWP)
	HACK_NOSCOP = 4, // No scope frags
	HACK_LAST = 5, // Unused
};

/**
* Player Stats
*/
struct P_STATS
{
	void Reset()
	{
		this->RoundWinShare = 0.0f;

		this->FlashDuration = 0.0f;

		memset(this->Stats, 0, sizeof(this->Stats));

		memset(this->Rounds, 0, sizeof(this->Rounds));

		memset(this->BombStats, 0, sizeof(this->BombStats));

		memset(this->HitBox, 0, sizeof(this->HitBox));

		memset(this->WeaponStats, 0, sizeof(this->WeaponStats));

		memset(this->KillStreak, 0, sizeof(this->KillStreak));

		memset(this->Versus, 0, sizeof(this->Versus));

		memset(this->Money, 0, sizeof(this->Money));

		memset(this->HackStats, 0, sizeof(this->HackStats));
	}

	float RoundWinShare = 0.0f;
	float FlashDuration = 0.0f;

	int Stats[STATS_LAST] = { 0 };
	int Rounds[ROUND_LAST] = { 0 };
	int BombStats[BOMB_LAST] = { 0 };
	int HitBox[HITGROUP_LAST] = { 0 };
	int HitBoxDamage[HITGROUP_LAST] = { 0 };
	int WeaponStats[MAX_WEAPONS + 1][WEAPON_LAST] = { 0 };
	int KillStreak[MAX_CLIENTS + 1] = { 0 };
	int Versus[MAX_CLIENTS + 1] = { 0 };
	int Money[RT_VIP_RESCUED_MYSELF + 1] = { 0 };
	int HackStats[HACK_LAST] = { 0 };
};

/**
* Round Events
*/
typedef struct
{
	int			Round;			// Round Count
	float		Time;			// Round Time Seconds;
	int			Type;			// ROUND_NONE for player events, ScenarioEventEndRound for round events
	int			Winner;			// Winner team of event
	int			Loser;			// Loser team of event
	std::string Killer;			// Killer
	std::string Victim;			// Victim
	Vector		KillerOrigin;	// Killer Position
	Vector		VictimOrigin;	// Victim Position
	int			IsHeadShot;		// HeadShot
	int			ItemIndex;		// Weapon
} P_ROUND_EVENT, * LP_ROUND_EVENT;

/**
* Round Data
*/
typedef struct
{
	float		Damage[MAX_CLIENTS + 1];
	float		DamageSum;
	int			Frags;
	int			Versus;
	std::string Flasher;
} P_ROUND_DATA, * LP_ROUND_DATA;

class CStats
{
public:
	void Save();

	void Reset(bool FullReset);

	void ServerActivate();
	void ServerDeactivate();
	void AddAccount(CBasePlayer* Player, int amount, RewardType type, bool bTrackChange);
	void TakeDamage(CBasePlayer* Player, entvars_t* pevInflictor, entvars_t* pevAttacker, float& flDamage, int bitsDamageType);
	void PlayerKilled(CBasePlayer* pVictim, entvars_t* pevKiller, entvars_t* pevInflictor);
	void SetAnimation(CBasePlayer* Player, PLAYER_ANIM playerAnim);
	void MakeBomber(CBasePlayer* Player);
	void DropBomb(CBasePlayer* Player);
	void PlantBomb(entvars_t* pevOwner, bool Planted);
	void DefuseBombStart(CBasePlayer* Player);
	void DefuseBombEnd(CBasePlayer* Player, bool Defused);
	void ExplodeBomb(CGrenade* pThis, TraceResult* ptr, int bitsDamageType);
	void PlayerBlind(CBasePlayer* Player, entvars_t* pevInflictor, entvars_t* pevAttacker, float fadeTime, float fadeHold, int alpha, Vector& color);
	void RoundRestart();
	void RoundFreezeEnd();
	void RoundEnd(int winStatus, ScenarioEventEndRound eventScenario, float tmDelay);
	void OnEvent(GameEventType event, CBaseEntity* pEntity, class CBaseEntity* pEntityOther);

	std::map<std::string, P_STATS>			m_Data;
	std::vector<P_ROUND_EVENT>				m_Event;
	std::map<std::string, P_ROUND_DATA>		m_Round;
	float									m_RoundDamage[SPECTATOR + 1] = { 0.0f };
	time_t									m_StartTime = 0;
};

extern CStats gStats;
