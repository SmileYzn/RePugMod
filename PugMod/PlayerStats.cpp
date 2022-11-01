#include "precompiled.h"

void CPlayerStats::Clear()
{
	this->Frags = 0;

	this->Assists = 0;

	this->Deaths = 0;

	this->Headshot = 0;

	this->Shots = 0;

	this->Hits = 0;

	this->Damage = 0;

	this->DamageReceive = 0;

	this->JoinTime = gpGlobals->time;

	this->GameTime = 0.0f;

	memset(this->Rounds, 0, sizeof(this->Rounds));

	this->RoundWinShare = 0.0f;

	memset(this->BombStats, 0, sizeof(this->BombStats));

	memset(this->HitBox, 0, sizeof(this->HitBox));

	memset(this->WeaponStats, 0, sizeof(this->WeaponStats));

	memset(this->KillStreak, 0, sizeof(this->KillStreak));

	memset(this->Versus, 0, sizeof(this->Versus));

	memset(this->Money, 0, sizeof(this->Money));

	memset(this->HackStats, 0, sizeof(this->HackStats));
}
