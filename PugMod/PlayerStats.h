#pragma once

/**
* Bomb Group
*/
enum BombGroup
{
	BOMB_PLANTING = 0, // Test
	BOMB_PLANTED = 1, // Test
	BOMB_EXPLODED = 2, // Test
	BOMB_DEFUSING = 3, // Test
	BOMB_DEFUSED = 4, // Test
};

/**
* Round Group
*/
enum RoundGroup
{
	ROUND_PLAY = 0, // Test
	ROUND_WIN_TR = 1, // Test
	ROUND_LOSE_TR = 2, // Test
	ROUND_WIN_CT = 3, // Test
	ROUND_LOSE_CT = 4, // Test
};

/**
* Hitbox Group
*/
enum HitBoxGroup
{
	HITGROUP_GENERIC = 0, // Test
	HITGROUP_HEAD = 1, // Test
	HITGROUP_CHEST = 2, // Test
	HITGROUP_STOMACH = 3, // Test
	HITGROUP_LEFTARM = 4, // Test
	HITGROUP_RIGHTARM = 5, // Test
	HITGROUP_LEFTLEG = 6, // Test
	HITGROUP_RIGHTLEG = 7, // Test
	HITGROUP_SHIELD = 8, // Test
};

/**
* Weapon Stats Group
*/
enum WeaponStatsGroup
{
	WEAPON_KILL = 0, // Test
	WEAPON_DEATH = 1, // Test
	WEAPON_HEADSHOT = 2, // Test
	WEAPON_SHOT = 3, // Test
	WEAPON_HIT = 4, // Test
	WEAPON_DAMAGE = 5, // Test
};

/**
* Hack Stats Group
*/
enum HackStatsGroup
{
	HACK_VISION = 0, // Ok
	HACK_ONEHIT = 3, // Ok
	HACK_NOSCOP = 4, // Ok
};

/**
* Advanced Group
*/
enum AdvancedStatsGroup
{
	FIRST_ROUND_FRAGS	= 0, // TO DO: First player that kills someone in round
	FIRST_ROUND_DEATH	= 1, // TO DO: First player that has ben killed by someone
	TRADE_FRAGS			= 2, // TO DO: Player that kill enemy while other teammate die to give frag to him
	TRADE_DEATH			= 3, // TO DO: Player that was dead while other teammate kill her assasin
};

class CPlayerStats
{
public:
	void Clear();

	std::string Name;
	std::string Auth;

	int Frags = 0;												// Ok
	int Assists = 0;											// Ok
	int Deaths = 0;												// Ok
	int Headshot = 0;											// Ok
	int Shots = 0;												// Ok
	int Hits = 0;												// Ok
	int Damage = 0;												// Ok
	int DamageReceive = 0;										// Ok
	time_t JoinTime = 0;										// Ok
	float RoundWinShare = 0.0f;									// Ok
	int Rounds[ROUND_LOSE_CT + 1] = { 0 };						// Ok
	int BombStats[BOMB_DEFUSED + 1] = { 0 };					// Ok
	int HitBox[HITGROUP_SHIELD + 1] = { 0 };					// Ok
	int HitBoxDamage[HITGROUP_SHIELD + 1] = { 0 };				// Ok
	int WeaponStats[MAX_WEAPONS + 1][WEAPON_DAMAGE + 1] = { 0 };// Test
	int KillStreak[MAX_CLIENTS + 1] = { 0 };					// Test
	int Versus[MAX_CLIENTS + 1] = { 0 };						// Test
	int Money[RT_VIP_RESCUED_MYSELF + 1] = { 0 };				// OK
	int HackStats[HACK_NOSCOP + 1] = { 0 };						// Test
	int AdvancedStats[TRADE_DEATH + 1] = { 0 };					// To do
};
