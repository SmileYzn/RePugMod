#include "precompiled.h"

CStats gStats;

void CStats::Clear()
{
	this->m_Stats.clear();

	memset(this->m_Data, 0, sizeof(this->m_Data));

	memset(this->m_RoundHits, 0, sizeof(this->m_RoundHits));

	memset(this->m_RoundDamage, 0, sizeof(this->m_RoundDamage));

	memset(this->m_RoundDamageSelf, 0, sizeof(this->m_RoundDamageSelf));

	memset(this->m_RoundDamageTeam, 0, sizeof(this->m_RoundDamageTeam));

	memset(this->m_RoundFrags, 0, sizeof(this->m_RoundFrags));

	memset(this->m_RoundVersus, 0, sizeof(this->m_RoundVersus));

	this->m_RoundBombPlanter = -1;

	this->m_RoundBombDefuser = -1;
}

void CStats::GetIntoGame(CBasePlayer* Player)
{
	if (Player->edict())
	{
		int EntityIndex = Player->entindex();

		this->m_Data[EntityIndex].Clear();

		auto it = this->m_Stats.find(STRING(Player->edict()->v.netname));

		if (it != this->m_Stats.end())
		{
			this->m_Data[EntityIndex] = it->second;
			
			this->m_Stats.erase(it);
		}
	}
}

void CStats::Disconnected(edict_t* pEdict)
{
	if (!FNullEnt(pEdict))
	{
		int EntityIndex = ENTINDEX(pEdict);

		if (this->m_Data[EntityIndex].Rounds[ROUND_PLAY] > 0)
		{
			this->m_Stats.insert(std::make_pair(STRING(pEdict->v.netname), this->m_Data[EntityIndex]));

			this->m_Data[EntityIndex].Clear();
		}
	}
}

void CStats::AddAccount(CBasePlayer* Player, int amount, RewardType type, bool bTrackChange)
{
	if (gPugMod.IsLive())
	{
		if (Player)
		{
			if (type >= RT_NONE && type <= RT_VIP_RESCUED_MYSELF)
			{
				this->m_Data[Player->entindex()].Money[type] += amount;
			}
		}
	}
}

void CStats::TakeDamage(CBasePlayer* Player, entvars_t* pevInflictor, entvars_t* pevAttacker, float& flDamage, int bitsDamageType)
{
	if (gPugMod.IsLive())
	{
		if (Player)
		{
			if (pevAttacker)
			{
				auto Attacker = UTIL_PlayerByIndexSafe(ENTINDEX(pevAttacker));

				if (Attacker)
				{
					if (Player->entindex() != Attacker->entindex())
					{
						if (CSGameRules()->FPlayerCanTakeDamage(Player, Attacker))
						{
							auto AttackerIndex = Attacker->entindex();

							this->m_Data[AttackerIndex].Hits++;

							this->m_Data[AttackerIndex].Damage += Player->m_lastDamageAmount;

							auto VictimIndex = Player->entindex();

							this->m_Data[VictimIndex].DamageReceive += Player->m_lastDamageAmount;

							this->m_Data[AttackerIndex].HitBox[Player->m_LastHitGroup]++;

							this->m_Data[AttackerIndex].HitBoxDamage[Player->m_LastHitGroup] += Player->m_lastDamageAmount;

							this->m_RoundHits[AttackerIndex][Player->entindex()]++;

							this->m_RoundDamage[AttackerIndex][Player->entindex()] += Player->m_lastDamageAmount;

							this->m_RoundDamageSelf[AttackerIndex] += Player->m_lastDamageAmount;

							this->m_RoundDamageTeam[Attacker->m_iTeam] += Player->m_lastDamageAmount;

							auto ItemIndex = this->GetActiveWeapon(Attacker, true);

							if (ItemIndex)
							{
								this->m_Data[AttackerIndex].WeaponStats[ItemIndex][WEAPON_HIT]++;

								this->m_Data[AttackerIndex].WeaponStats[ItemIndex][WEAPON_DAMAGE] += Player->m_lastDamageAmount;
							}
						}
					}
				}
			}
		}
	}
}

void CStats::Killed(CBasePlayer* Player, entvars_t* pevAttacker, int iGib)
{
	if (gPugMod.IsLive())
	{
		if (Player)
		{
			if (!Player->m_bKilledByBomb)
			{
				auto Killer = UTIL_PlayerByIndexSafe(ENTINDEX(pevAttacker));

				if (Killer)
				{
					if (Killer->IsPlayer())
					{
						auto VictimIndex = Player->entindex();

						auto KillerIndex = Killer->entindex();

						if (VictimIndex != KillerIndex)
						{
							this->m_Data[VictimIndex].Deaths++;

							this->m_Data[KillerIndex].Frags++;

							this->m_RoundFrags[KillerIndex]++;

							if (Player->m_bHeadshotKilled)
							{
								this->m_Data[KillerIndex].Headshot++;
							}

							auto ItemIndex = this->GetActiveWeapon(Killer, true);

							if (ItemIndex)
							{
								this->m_Data[KillerIndex].WeaponStats[ItemIndex][WEAPON_KILL]++;

								this->m_Data[VictimIndex].WeaponStats[ItemIndex][WEAPON_DEATH]++;

								if (Player->m_bHeadshotKilled)
								{
									this->m_Data[KillerIndex].WeaponStats[ItemIndex][WEAPON_HEADSHOT]++;
								}

								if (ItemIndex != WEAPON_AWP)
								{
									if (Player->m_iLastClientHealth >= 100)
									{
										if (Player->m_lastDamageAmount >= 100)
										{
											this->m_Data[KillerIndex].HackStats[HACK_ONEHIT]++;
										}
									}
								}

								if (ItemIndex == WEAPON_AWP || ItemIndex == WEAPON_SCOUT || ItemIndex == WEAPON_G3SG1 || ItemIndex == WEAPON_SG550)
								{
									if (Killer->m_iClientFOV == DEFAULT_FOV)
									{
										this->m_Data[KillerIndex].HackStats[HACK_NOSCOP]++;
									}
								}

								if (ItemIndex != WEAPON_HEGRENADE)
								{
									if (!Killer->m_izSBarState[SBAR_ID_TARGETTYPE])
									{
										if (!Killer->IsObserver())
										{
											this->m_Data[KillerIndex].HackStats[HACK_VISION]++;
										}
									}
								}
							}

							int NumAliveTR, NumAliveCT = 0;

							gPlayer.GetNum(NumAliveTR, NumAliveCT);

							for (int i = 1; i <= gpGlobals->maxClients; ++i)
							{
								auto Temp = UTIL_PlayerByIndexSafe(i);

								if (Temp)
								{
									if (Temp->m_iTeam == TERRORIST || Temp->m_iTeam == CT)
									{
										auto TempIndex = Temp->entindex();

										if (TempIndex != Killer->entindex())
										{
											if (this->m_RoundDamage[TempIndex][VictimIndex] >= MANAGER_ASSISTANCE_DMG)
											{
												this->m_Data[TempIndex].Assists++;
											}
										}

										if (!this->m_RoundVersus[TempIndex])
										{
											if (Temp->m_iTeam == TERRORIST)
											{
												if (NumAliveTR == 1)
												{
													this->m_RoundVersus[TempIndex] = NumAliveCT;
												}
											}
											else if (Temp->m_iTeam == CT)
											{
												if (NumAliveCT == 1)
												{
													this->m_RoundVersus[TempIndex] = NumAliveTR;
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void CStats::SetAnimation(CBasePlayer* Player, PLAYER_ANIM playerAnim)
{
	if (gPugMod.IsLive())
	{
		if (Player)
		{
			if (playerAnim == PLAYER_ATTACK1 || playerAnim == PLAYER_ATTACK2)
			{
				auto ItemIndex = this->GetActiveWeapon(Player, false);

				if (ItemIndex)
				{
					auto EntIndex = Player->entindex();

					this->m_Data[EntIndex].Shots++;

					this->m_Data[EntIndex].WeaponStats[ItemIndex][WEAPON_SHOT]++;
				}
			}
		}
	}
}

void CStats::PlantBomb(entvars_t* pevOwner, bool Planted)
{
	if (gPugMod.IsLive())
	{
		auto Player = UTIL_PlayerByIndexSafe(ENTINDEX(pevOwner));

		if (Player)
		{
			auto EntIndex = Player->entindex();

			if (Planted)
			{
				this->m_RoundBombPlanter = EntIndex;

				this->m_Data[EntIndex].BombStats[BOMB_PLANTED]++;
			}
			else
			{
				this->m_RoundBombPlanter = -1;

				this->m_Data[EntIndex].BombStats[BOMB_PLANTING]++;
			}
		}
	}
}

void CStats::DefuseBombStart(CBasePlayer* Player)
{
	if (gPugMod.IsLive())
	{
		if (Player)
		{
			auto EntIndex = Player->entindex();

			this->m_Data[EntIndex].BombStats[BOMB_DEFUSING]++;
		}
	}
}

void CStats::DefuseBombEnd(CBasePlayer* Player, bool Defused)
{
	if (gPugMod.IsLive())
	{
		if (Player)
		{
			if (Defused)
			{
				auto EntIndex = Player->entindex();

				this->m_Data[EntIndex].BombStats[BOMB_DEFUSED]++;

				this->m_RoundBombDefuser = EntIndex;
			}
		}
	}
}

void CStats::ExplodeBomb(CGrenade* pThis, TraceResult* ptr, int bitsDamageType)
{
	if (gPugMod.IsLive())
	{
		if (this->m_RoundBombPlanter != -1)
		{
			auto Player = UTIL_PlayerByIndexSafe(this->m_RoundBombPlanter);

			if (Player)
			{
				auto EntIndex = Player->entindex();

				this->m_Data[EntIndex].BombStats[BOMB_EXPLODED]++;
			}
		}
	}
}

void CStats::RoundFreezeEnd()
{
	if (gPugMod.IsLive())
	{
		memset(this->m_RoundHits, 0, sizeof(this->m_RoundHits));

		memset(this->m_RoundDamage, 0, sizeof(this->m_RoundDamage));

		memset(this->m_RoundDamageSelf, 0, sizeof(this->m_RoundDamageSelf));

		memset(this->m_RoundDamageTeam, 0, sizeof(this->m_RoundDamageTeam));

		memset(this->m_RoundFrags, 0, sizeof(this->m_RoundFrags));

		memset(this->m_RoundVersus, 0, sizeof(this->m_RoundVersus));

		this->m_RoundBombPlanter = -1;

		this->m_RoundBombDefuser = -1;
	}
}

void CStats::RoundEnd(int winStatus, ScenarioEventEndRound event, float tmDelay)
{
	if (gPugMod.IsLive())
	{
		if (winStatus == WINSTATUS_TERRORISTS || winStatus == WINSTATUS_CTS)
		{
			if (event == ROUND_BOMB_DEFUSED)
			{
				if (this->m_RoundBombDefuser != -1)
				{
					auto Defuser = UTIL_PlayerByIndexSafe(this->m_RoundBombDefuser);

					if (Defuser)
					{
						auto DefuserIndex = Defuser->entindex();

						this->m_Data[DefuserIndex].RoundWinShare += MANAGER_RWS_C4_DEFUSED;
					}
				}
			}
			else if (event == ROUND_TARGET_BOMB)
			{
				if (this->m_RoundBombPlanter != -1)
				{
					auto Planter = UTIL_PlayerByIndexSafe(this->m_RoundBombPlanter);

					if (Planter)
					{
						auto PlanterIndex = Planter->entindex();

						this->m_Data[PlanterIndex].RoundWinShare += MANAGER_RWS_C4_EXPLODE;
					}
				}
			}

			TeamName Winner = (winStatus == WINSTATUS_TERRORISTS) ? TERRORIST : CT;

			for (int i = 1; i <= gpGlobals->maxClients; ++i)
			{
				auto Temp = UTIL_PlayerByIndexSafe(i);

				if (Temp)
				{
					auto TempIndex = Temp->entindex();

					this->m_Data[TempIndex].Rounds[ROUND_PLAY]++;

					if (this->m_RoundFrags[TempIndex] > 0)
					{
						this->m_Data[TempIndex].KillStreak[this->m_RoundFrags[TempIndex]]++;
					}

					if (Temp->m_iTeam == Winner)
					{
						this->m_Data[TempIndex].Rounds[(Temp->m_iTeam == TERRORIST) ? ROUND_WIN_TR : ROUND_WIN_CT]++;

						if (this->m_RoundVersus[TempIndex] > 0)
						{
							this->m_Data[TempIndex].Versus[this->m_RoundVersus[TempIndex]]++;
						}

						if (this->m_RoundDamageSelf[TempIndex] > 0)
						{
							float RoundWinShare = (float)((float)this->m_RoundDamageSelf[TempIndex] / (float)this->m_RoundDamageTeam[Winner]);

							if (event == ROUND_BOMB_DEFUSED || event == ROUND_TARGET_BOMB)
							{
								RoundWinShare = (MANAGER_RWS_MAP_TARGET * RoundWinShare);
							}

							this->m_Data[TempIndex].RoundWinShare += RoundWinShare;
						}
					}
					else
					{
						this->m_Data[TempIndex].Rounds[(Temp->m_iTeam == TERRORIST) ? ROUND_LOSE_TR : ROUND_LOSE_CT]++;
					}
				}
			}
		}
	}
}

int CStats::GetRoundHits(int AttackerIndex, int TargetIndex)
{
	return this->m_RoundHits[AttackerIndex][TargetIndex];
}

int CStats::GetRoundDamage(int AttackerIndex, int TargetIndex)
{
	return this->m_RoundDamage[AttackerIndex][TargetIndex];
}

CPlayerStats CStats::GetData(int EntityIndex)
{
	return this->m_Data[EntityIndex];
}

int CStats::GetActiveWeapon(CBasePlayer* Player, bool AllowKnife)
{
	if (Player->m_pActiveItem)
	{
		if (Player->m_pActiveItem->IsWeapon())
		{
			auto ItemSlot = Player->m_pActiveItem->iItemSlot();

			if (ItemSlot == PRIMARY_WEAPON_SLOT || ItemSlot == PISTOL_SLOT || ItemSlot == KNIFE_SLOT || ItemSlot == GRENADE_SLOT)
			{
				auto ItemIndex = Player->m_pActiveItem->m_iId;

				if (ItemIndex != WEAPON_SMOKEGRENADE && ItemIndex != WEAPON_FLASHBANG)
				{
					if (!AllowKnife && ItemIndex == WEAPON_KNIFE)
					{
						return WEAPON_NONE;
					}

					return ItemIndex;
				}
			}
		}
	}

	return WEAPON_NONE;
}
//
//void CStats::LogStats()
//{
//	if (gCvars.GetLogStats())
//	{
//		//
//		// Log Match Stats
//		ALERT
//		(
//			at_logged,
//			"[Match][%s][%s][%d][%d][%d][%d]\n",
//			CVAR_GET_STRING("hostname"),
//			CVAR_GET_STRING("net_address"),
//			static_cast<long long>(std::time(NULL)),
//			gPugMod.GetScores(TERRORIST),
//			gPugMod.GetScores(CT)
//		);
//		//
//		// Set all player stats to map container
//		for (int i = 1; i <= gpGlobals->maxClients; ++i)
//		{
//			auto Temp = UTIL_PlayerByIndexSafe(i);
//
//			if (Temp)
//			{
//				this->Disconnected(Temp->edict());
//			}
//		}
//		//
//		// Log stats of all players
//		for (auto const& Player : this->m_Stats)
//		{
//			//
//			// Log Player Stats
//			ALERT
//			(
//				at_logged,
//				"[Player][%s][%d][%d][%d][%d][%d][%d][%d][%d][%f][%f][%f]\n",
//				Player.first.c_str(),
//				Player.second.Frags,
//				Player.second.Assists,
//				Player.second.Deaths,
//				Player.second.Headshot,
//				Player.second.Shots,
//				Player.second.Hits,
//				Player.second.Damage,
//				Player.second.DamageReceive,
//				Player.second.JoinTime,
//				Player.second.GameTime,
//				Player.second.RoundWinShare
//			);
//			//
//			// Log Round Stats
//			ALERT
//			(
//				at_logged,
//				"[Round][%s][%d][%d][%d][%d][%d]\n",
//				Player.first.c_str(),
//				Player.second.Rounds[ROUND_PLAY],
//				Player.second.Rounds[ROUND_WIN_TR],
//				Player.second.Rounds[ROUND_LOSE_TR],
//				Player.second.Rounds[ROUND_WIN_CT],
//				Player.second.Rounds[ROUND_LOSE_CT]
//			);
//			//
//			// Log Bomb Stats
//			ALERT
//			(
//				at_logged,
//				"[Bomb][%s][%d][%d][%d][%d][%d]\n",
//				Player.first.c_str(),
//				Player.second.BombStats[BOMB_PLANTING],
//				Player.second.BombStats[BOMB_PLANTED],
//				Player.second.BombStats[BOMB_EXPLODED],
//				Player.second.BombStats[BOMB_DEFUSING],
//				Player.second.BombStats[BOMB_DEFUSED]
//			);
//			//
//			// Log HitBox Stats
//			ALERT
//			(
//				at_logged,
//				"[HitBox][%s][%d][%d][%d][%d][%d][%d][%d][%d][%d]\n",
//				Player.first.c_str(),
//				Player.second.HitBox[HITGROUP_GENERIC],
//				Player.second.HitBox[HITGROUP_HEAD],
//				Player.second.HitBox[HITGROUP_CHEST],
//				Player.second.HitBox[HITGROUP_STOMACH],
//				Player.second.HitBox[HITGROUP_LEFTARM],
//				Player.second.HitBox[HITGROUP_RIGHTARM],
//				Player.second.HitBox[HITGROUP_LEFTLEG],
//				Player.second.HitBox[HITGROUP_RIGHTLEG],
//				Player.second.HitBox[HITGROUP_SHIELD]
//			);
//			//
//			// Log HitBox Damage
//			ALERT
//			(
//				at_logged,
//				"[HitBoxDamage][%s][%d][%d][%d][%d][%d][%d][%d][%d][%d]\n",
//				Player.first.c_str(),
//				Player.second.HitBoxDamage[HITGROUP_GENERIC],
//				Player.second.HitBoxDamage[HITGROUP_HEAD],
//				Player.second.HitBoxDamage[HITGROUP_CHEST],
//				Player.second.HitBoxDamage[HITGROUP_STOMACH],
//				Player.second.HitBoxDamage[HITGROUP_LEFTARM],
//				Player.second.HitBoxDamage[HITGROUP_RIGHTARM],
//				Player.second.HitBoxDamage[HITGROUP_LEFTLEG],
//				Player.second.HitBoxDamage[HITGROUP_RIGHTLEG],
//				Player.second.HitBoxDamage[HITGROUP_SHIELD]
//			);
//			//
//			// Log Weapon Stats
//			for (int WeaponType = 0; WeaponType < MAX_WEAPONS; WeaponType++)
//			{
//				if (Player.second.WeaponStats[WeaponType][WEAPON_SHOT] > 0)
//				{
//					ALERT
//					(
//						at_logged,
//						"[Weapon][%s][%d][%d][%d][%d][%d][%d][%d]\n",
//						Player.first.c_str(),
//						WeaponType,
//						Player.second.WeaponStats[WeaponType][WEAPON_KILL],
//						Player.second.WeaponStats[WeaponType][WEAPON_DEATH],
//						Player.second.WeaponStats[WeaponType][WEAPON_HEADSHOT],
//						Player.second.WeaponStats[WeaponType][WEAPON_SHOT],
//						Player.second.WeaponStats[WeaponType][WEAPON_HIT],
//						Player.second.WeaponStats[WeaponType][WEAPON_DAMAGE]
//					);
//				}
//			}
//			//
//			// Log KillStreak
//			ALERT
//			(
//				at_logged,
//				"[KillStreak][%s][%d][%d][%d][%d][%d]\n",
//				Player.first.c_str(),
//				Player.second.KillStreak[1],
//				Player.second.KillStreak[2],
//				Player.second.KillStreak[3],
//				Player.second.KillStreak[4],
//				Player.second.KillStreak[5]
//			);
//			//
//			// Log Versus
//			ALERT
//			(
//				at_logged,
//				"[Versus][%s][%d][%d][%d][%d][%d]\n",
//				Player.first.c_str(),
//				Player.second.Versus[1],
//				Player.second.Versus[2],
//				Player.second.Versus[3],
//				Player.second.Versus[4],
//				Player.second.Versus[5]
//			);
//			//
//			// Log Money
//			ALERT
//			(
//				at_logged,
//				"[Money][%s][%d][%d][%d][%d][%d][%d][%d][%d][%d][%d][%d][%d][%d][%d][%d]\n",
//				Player.first.c_str(),
//				Player.second.Money[RT_NONE],
//				Player.second.Money[RT_ROUND_BONUS],
//				Player.second.Money[RT_PLAYER_RESET],
//				Player.second.Money[RT_PLAYER_JOIN],
//				Player.second.Money[RT_PLAYER_SPEC_JOIN],
//				Player.second.Money[RT_PLAYER_BOUGHT_SOMETHING],
//				Player.second.Money[RT_HOSTAGE_TOOK],
//				Player.second.Money[RT_HOSTAGE_RESCUED],
//				Player.second.Money[RT_HOSTAGE_DAMAGED],
//				Player.second.Money[RT_HOSTAGE_KILLED],
//				Player.second.Money[RT_TEAMMATES_KILLED],
//				Player.second.Money[RT_ENEMY_KILLED],
//				Player.second.Money[RT_INTO_GAME],
//				Player.second.Money[RT_VIP_KILLED],
//				Player.second.Money[RT_VIP_RESCUED_MYSELF]
//			);
//			//
//			// Log HackStats
//			ALERT
//			(
//				at_logged,
//				"[Hack][%s][%d][%d][%d]\n",
//				Player.first.c_str(),
//				Player.second.HackStats[HACK_VISION],
//				Player.second.HackStats[HACK_ONEHIT],
//				Player.second.HackStats[HACK_NOSCOP]
//			);
//		}
//	}
//}