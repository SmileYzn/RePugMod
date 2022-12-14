#include "precompiled.h"

CStats gStats;

void CStats::ServerActivate()
{
	this->Reset(true);
}

void CStats::ServerDeactivate()
{
	this->Reset(true);
}

void CStats::Reset(bool FullReset)
{
	if (FullReset)
	{
		this->m_Event.clear();

		this->m_Data.clear();

		this->m_StartTime = std::time(NULL);
	}

	this->m_Round.clear();

	memset(m_RoundDamage, 0.0f, sizeof(m_RoundDamage));
}

void CStats::AddAccount(CBasePlayer* Player, int amount, RewardType type, bool bTrackChange)
{
	if (gPugMod.IsLive())
	{
		if (Player->IsPlayer())
		{
			auto Auth = GET_AUTH(Player->edict());

			if (Auth && Auth[0] != '\0')
			{
				this->m_Data[Auth].Money[type] += amount;
			}
		}
	}
}

void CStats::TakeDamage(CBasePlayer* Player, entvars_t* pevInflictor, entvars_t* pevAttacker, float& flDamage, int bitsDamageType)
{
	if (gPugMod.IsLive())
	{
		if (Player->IsPlayer())
		{
			if (pevAttacker)
			{
				auto Attacker = UTIL_PlayerByIndexSafe(ENTINDEX(pevAttacker));

				if (!FNullEnt(Attacker))
				{
					if (Attacker->IsPlayer())
					{
						if (Player->entindex() != Attacker->entindex())
						{
							if (CSGameRules()->FPlayerCanTakeDamage(Player, Attacker) && !Player->m_bKilledByBomb)
							{
								if (!(bitsDamageType & DMG_BLAST))
								{
									auto VictimAuth = GET_AUTH(Player->edict());

									auto AttackerAuth = GET_AUTH(Attacker->edict());

									auto DamageTaken = (int)(Player->m_iLastClientHealth - clamp(Player->edict()->v.health, 0.0f, Player->edict()->v.health));

									auto ItemIndex = (bitsDamageType & DMG_EXPLOSION) ? WEAPON_HEGRENADE : ((Attacker->m_pActiveItem) ? Attacker->m_pActiveItem->m_iId : WEAPON_NONE);

									if (VictimAuth && VictimAuth[0] != '\0')
									{
										this->m_Data[VictimAuth].Stats[STATS_DMG_RECV] += DamageTaken;

										if (ItemIndex != WEAPON_NONE)
										{
											this->m_Data[VictimAuth].Stats[STATS_DMG_RECV] += DamageTaken;

											this->m_Data[VictimAuth].WeaponStats[ItemIndex][WEAPON_DAMAGE_R] += DamageTaken;
										}
									}

									if (AttackerAuth && AttackerAuth[0] != '\0')
									{
										this->m_Data[AttackerAuth].Stats[STATS_HITS]++;

										this->m_Data[AttackerAuth].Stats[STATS_DMG_DONE] += DamageTaken;

										this->m_Data[AttackerAuth].HitBox[Player->m_LastHitGroup]++;

										this->m_Data[AttackerAuth].HitBoxDamage[Player->m_LastHitGroup] += DamageTaken;

										this->m_Round[AttackerAuth].Damage[Player->entindex()] += DamageTaken;

										this->m_Round[AttackerAuth].DamageSum += DamageTaken;

										this->m_RoundDamage[Attacker->m_iTeam] += DamageTaken;

										if (ItemIndex != WEAPON_NONE)
										{
											this->m_Data[AttackerAuth].WeaponStats[ItemIndex][WEAPON_HIT]++;

											this->m_Data[AttackerAuth].WeaponStats[ItemIndex][WEAPON_DAMAGE] += DamageTaken;
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

void CStats::Killed(CBasePlayer* Player, entvars_t* pevAttacker, int iGib)
{
	if (gPugMod.IsLive())
	{
		if (Player->IsPlayer())
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

						auto ItemIndex = (Player->m_bKilledByGrenade) ? WEAPON_HEGRENADE : ((Killer->m_pActiveItem) ? Killer->m_pActiveItem->m_iId : WEAPON_NONE);

						if (VictimIndex != KillerIndex)
						{
							auto VictimAuth = GET_AUTH(Player->edict());

							auto KillerAuth = GET_AUTH(Killer->edict());

							this->m_Data[KillerAuth].Stats[STATS_FRAGS]++;

							this->m_Round[KillerAuth].Frags++;

							if (Player->m_LastHitGroup == HITGROUP_HEAD)
							{
								this->m_Data[KillerAuth].Stats[STATS_HEADSHOTS]++;
							}

							this->m_Data[VictimAuth].Stats[STATS_DEATHS]++;

							if (Player->IsBlind())
							{
								if (!this->m_Round[VictimAuth].Flasher.empty())
								{
									if (this->m_Round[VictimAuth].Flasher.compare(KillerAuth) != 0)
									{
										this->m_Data[this->m_Round[VictimAuth].Flasher].Stats[STATS_FLASH_ASSIST]++;
									}
								}
							}

							int NumAliveTR = 0, NumAliveCT = 0, NumDeadTR = 0, NumDeadCT = 0;

							if (g_pGameRules)
							{
								CSGameRules()->InitializePlayerCounts(NumAliveTR, NumAliveCT, NumDeadTR, NumDeadCT);
							}

							if (!NumDeadTR && !NumDeadCT)
							{
								this->m_Data[KillerAuth].Stats[STATS_FR_FRAGS]++;

								this->m_Data[VictimAuth].Stats[STATS_FR_DEATHS]++;
							}

							if (ItemIndex != WEAPON_NONE)
							{
								this->m_Data[KillerAuth].WeaponStats[ItemIndex][WEAPON_KILL]++;

								this->m_Data[VictimAuth].WeaponStats[ItemIndex][WEAPON_DEATH]++;

								if (Player->m_LastHitGroup == HITGROUP_HEAD)
								{
									this->m_Data[KillerAuth].WeaponStats[ItemIndex][WEAPON_HEADSHOT]++;
								}

								if (ItemIndex != WEAPON_AWP)
								{
									if (Player->m_iLastClientHealth >= 100)
									{
										if (Player->m_lastDamageAmount >= 100)
										{
											this->m_Data[KillerAuth].HackStats[HACK_ONEHIT]++;
										}
									}
								}

								if (ItemIndex == WEAPON_AWP || ItemIndex == WEAPON_SCOUT || ItemIndex == WEAPON_G3SG1 || ItemIndex == WEAPON_SG550)
								{
									if (Killer->m_iClientFOV == DEFAULT_FOV)
									{
										this->m_Data[KillerAuth].HackStats[HACK_NOSCOP]++;
									}
								}

								if (ItemIndex != WEAPON_HEGRENADE)
								{
									if (!Killer->m_izSBarState[SBAR_ID_TARGETTYPE])
									{
										if (!Killer->IsObserver())
										{
											this->m_Data[KillerAuth].HackStats[HACK_VISION]++;
										}
									}
								}
							}

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
											auto TempAuth = GET_AUTH(Temp->edict());

											if (TempAuth && TempAuth[0] != '\0')
											{
												if (this->m_Round[TempAuth].Damage[VictimIndex] >= MANAGER_ASSISTANCE_DMG)
												{
													this->m_Data[TempAuth].Stats[STATS_ASSISTS]++;

													if (ItemIndex != WEAPON_NONE)
													{
														this->m_Data[TempAuth].WeaponStats[ItemIndex][WEAPON_ASSISTS]++;
													}
												}

												if (!this->m_Round[TempAuth].Versus)
												{
													if (Temp->m_iTeam == TERRORIST)
													{
														if (NumAliveTR == 1)
														{
															this->m_Round[TempAuth].Versus = NumAliveCT;
														}
													}
													else if (Temp->m_iTeam == CT)
													{
														if (NumAliveCT == 1)
														{
															this->m_Round[TempAuth].Versus = NumAliveTR;
														}
													}
												}
											}
										}
									}
								}
							}
						}

						this->AddRoundEvent(ROUND_NONE, Killer, Player, Killer->m_iTeam, Player->m_iTeam, (Player->m_LastHitGroup == HITGROUP_HEAD) ? 1 : 0, ItemIndex);
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
		if (Player->IsPlayer())
		{
			if ((playerAnim == PLAYER_ATTACK1) || (playerAnim == PLAYER_ATTACK2))
			{
				if (Player->m_pActiveItem)
				{
					if (Player->m_pActiveItem->m_iId && Player->m_pActiveItem->IsWeapon())
					{
						auto Auth = GET_AUTH(Player->edict());

						if (Auth && Auth[0] != '\0')
						{
							this->m_Data[Auth].Stats[STATS_SHOTS]++;

							this->m_Data[Auth].WeaponStats[Player->m_pActiveItem->m_iId][WEAPON_SHOT]++;
						}
					}
				}
			}
		}
	}
}

void CStats::MakeBomber(CBasePlayer* Player)
{
	if (gPugMod.IsLive())
	{
		if (Player->IsPlayer())
		{
			if (Player->m_bHasC4)
			{
				auto Auth = GET_AUTH(Player->edict());

				if (Auth && Auth[0] != '\0')
				{
					this->m_Data[Auth].BombStats[BOMB_CARRIER]++;
				}
			}
		}
	}
}

void CStats::DropBomb(CBasePlayer* Player)
{
	if (gPugMod.IsLive())
	{
		if (Player->IsPlayer())
		{
			auto Auth = GET_AUTH(Player->edict());

			if (Auth && Auth[0] != '\0')
			{
				this->m_Data[Auth].BombStats[BOMB_DROPPED]++;
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
			if (Player->IsPlayer())
			{
				auto Auth = GET_AUTH(Player->edict());

				if (Auth && Auth[0] != '\0')
				{
					if (Planted)
					{
						this->m_Data[Auth].BombStats[BOMB_PLANTED]++;

						this->m_Round[Auth].BombPlanter = Player->entindex();
					}
					else
					{
						this->m_Data[Auth].BombStats[BOMB_PLANTING]++;
					}
				}
			}
		}
	}
}

void CStats::DefuseBombStart(CBasePlayer* Player)
{
	if (gPugMod.IsLive())
	{
		if (Player->IsPlayer())
		{
			auto Auth = GET_AUTH(Player->edict());

			if (Auth && Auth[0] != '\0')
			{
				if (Player->m_bHasDefuser)
				{
					this->m_Data[Auth].BombStats[BOMB_DEFUSING_KIT]++;
				}
				else
				{
					this->m_Data[Auth].BombStats[BOMB_DEFUSING]++;
				}
			}
		}
	}
}

void CStats::DefuseBombEnd(CBasePlayer* Player, bool Defused)
{
	if (gPugMod.IsLive())
	{
		if (Player->IsPlayer())
		{
			if (Defused)
			{
				auto Auth = GET_AUTH(Player->edict());

				if (Auth && Auth[0] != '\0')
				{
					if (Player->m_bHasDefuser)
					{
						this->m_Data[Auth].BombStats[BOMB_DEFUSED_KIT]++;
					}
					else
					{
						this->m_Data[Auth].BombStats[BOMB_DEFUSED]++;

						this->m_Round[Auth].BombDefuser = Player->entindex();
					}
				}
			}
		}
	}
}

void CStats::ExplodeBomb(CGrenade* pThis, TraceResult* ptr, int bitsDamageType)
{
	if (gPugMod.IsLive())
	{
		for (auto const& Temp : this->m_Round)
		{
			if (Temp.second.BombPlanter)
			{
				this->m_Data[Temp.first].BombStats[BOMB_EXPLODED]++;
			}
		}
	}
}

void CStats::PlayerBlind(CBasePlayer* Player, entvars_t* pevInflictor, entvars_t* pevAttacker, float fadeTime, float fadeHold, int alpha, Vector& color)
{
	if (gPugMod.IsLive())
	{
		if (Player->IsAlive())
		{
			auto AttackerIndex = ENTINDEX(pevAttacker);

			auto Attacker = UTIL_PlayerByIndexSafe(AttackerIndex);

			if (Attacker)
			{
				if (Attacker->m_iTeam != Player->m_iTeam)
				{
					if (Player->IsBlind())
					{
						auto AttackerAuth = GET_AUTH(Attacker->edict());

						if (AttackerAuth && AttackerAuth[0] != '\0')
						{
							this->m_Data[AttackerAuth].FlashDuration += (Player->m_blindUntilTime - Player->m_blindStartTime);

							auto VictimAuth = GET_AUTH(Player->edict());

							if (VictimAuth && VictimAuth[0] != '\0')
							{
								this->m_Round[VictimAuth].Flasher = AttackerAuth;
							}
						}
					}
				}
			}
		}
	}
}

void CStats::RoundRestart()
{
	if (gPugMod.GetState() == PUG_STATE_FIRST_HALF)
	{
		if (g_pGameRules)
		{
			if (CSGameRules()->m_bCompleteReset)
			{
				this->Reset(true);
			}
		}
	}
}

void CStats::RoundFreezeEnd()
{
	if (gPugMod.IsLive())
	{
		this->Reset(false);
	}
}

void CStats::RoundEnd(int winStatus, ScenarioEventEndRound eventScenario, float tmDelay)
{
	if (gPugMod.IsLive())
	{
		if (winStatus == WINSTATUS_TERRORISTS || winStatus == WINSTATUS_CTS)
		{
			TeamName Winner = (winStatus == WINSTATUS_TERRORISTS) ? TERRORIST : CT;

			TeamName Losers = (winStatus != WINSTATUS_TERRORISTS) ? TERRORIST : CT;

			CBasePlayer* PlayerTrigger = NULL;

			if (eventScenario == ROUND_BOMB_DEFUSED || eventScenario == ROUND_TARGET_BOMB)
			{
				for (auto const& row : this->m_Round)
				{
					if (!row.first.empty())
					{
						if (row.second.BombDefuser)
						{
							this->m_Data[row.first].RoundWinShare += MANAGER_RWS_C4_DEFUSED;

							PlayerTrigger = UTIL_PlayerByIndexSafe(row.second.BombDefuser);
						}
						else if (row.second.BombPlanter)
						{
							this->m_Data[row.first].RoundWinShare += MANAGER_RWS_C4_EXPLODE;

							PlayerTrigger = UTIL_PlayerByIndexSafe(row.second.BombPlanter);
						}
					}
				}
			}

			this->AddRoundEvent(eventScenario, PlayerTrigger, NULL, Winner, Losers, 0, -1);

			for (int i = 1; i <= gpGlobals->maxClients; ++i)
			{
				auto Player = UTIL_PlayerByIndexSafe(i);

				if (Player)
				{
					if (Player->IsPlayer())
					{
						auto Auth = GET_AUTH(Player->edict());

						if (Auth && Auth[0] != '\0')
						{
							this->m_Data[Auth].Rounds[ROUND_PLAY]++;

							if (this->m_Round[Auth].Frags > 0)
							{
								this->m_Data[Auth].KillStreak[this->m_Round[Auth].Frags]++;
							}

							if (Player->m_iTeam == Winner)
							{
								if (Player->m_iTeam == TERRORIST)
								{
									this->m_Data[Auth].Rounds[ROUND_WIN_TR]++;
								}
								else
								{
									this->m_Data[Auth].Rounds[ROUND_WIN_CT]++;
								}

								if (this->m_Round[Auth].Versus > 0)
								{
									this->m_Data[Auth].Versus[this->m_Round[Auth].Versus]++;
								}

								if (this->m_Round[Auth].DamageSum)
								{
									float RoundWinShare = this->m_Round[Auth].DamageSum;

									if (this->m_RoundDamage[Winner] > 0.0f)
									{
										RoundWinShare = (RoundWinShare / this->m_RoundDamage[Winner]);
									}

									if (CSGameRules()->m_bBombDefused || CSGameRules()->m_bTargetBombed)
									{
										RoundWinShare = (MANAGER_RWS_MAP_TARGET * RoundWinShare);
									}

									this->m_Data[Auth].RoundWinShare += RoundWinShare;
								}
							}
							else
							{
								if (Player->m_iTeam == TERRORIST)
								{
									this->m_Data[Auth].Rounds[ROUND_LOSE_TR]++;
								}
								else
								{
									this->m_Data[Auth].Rounds[ROUND_LOSE_CT]++;
								}
							}
						}
					}
				}
			}
		}
	}
}

void CStats::AddRoundEvent(int Type, CBasePlayer* Killer, CBasePlayer* Victim, int KillerTeam, int VictimTeam, int IsHeadShot, int ItemIndex)
{
	if (gPugMod.IsLive())
	{
		P_ROUND_EVENT Event;

		Event.Round = gPugMod.GetRound();

		if (g_pGameRules)
		{
			Event.Time = CSGameRules()->GetRoundRemainingTimeReal();
		}

		Event.Type = Type;

		Event.Killer = "World";

		Event.Victim = "World";

		if (Killer && !FNullEnt(Killer->edict()))
		{
			Event.Killer = GET_AUTH(Killer->edict());

			Event.KillerOrigin = Killer->edict()->v.origin;
		}

		if (Victim && !FNullEnt(Victim->edict()))
		{
			Event.Victim = GET_AUTH(Victim->edict());

			Event.VictimOrigin = Killer->edict()->v.origin;
		}

		Event.Winner = KillerTeam;

		Event.Loser = VictimTeam;

		Event.IsHeadShot = IsHeadShot;

		Event.ItemIndex = ItemIndex;

		this->m_Event.push_back(Event);
	}
}
