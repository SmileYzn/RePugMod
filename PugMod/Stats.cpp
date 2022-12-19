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

void CStats::Save()
{
	// Has API URL
	if (gCvars.GetApiUrl()->string && gCvars.GetApiUrl()->string[0] != '\0')
	{
		// Has API Stats flag
		if (gCvars.GetApiStats()->value)
		{
			// JSON data
			nlohmann::ordered_json StatsData;
			//
			// Match Data
			StatsData["server"] =
			{
				{"hostname", CVAR_GET_STRING("hostname")},
				{"address", CVAR_GET_STRING("net_address")},
				{"map", STRING(gpGlobals->mapname)},
				{"start", static_cast<long long>(this->m_StartTime)},
				{"end", static_cast<long long>(std::time(NULL))},
				{"round", gPugMod.GetRound()},
				{"winner", (int)gPugMod.GetWinner()},
				{"tr", gPugMod.GetScores(TERRORIST)},
				{"ct", gPugMod.GetScores(CT)},
			};
			//
			// Round Events
			for (auto& Event : gStats.m_Event)
			{
				StatsData["rounds"][std::to_string(Event.Round)].push_back
				({
					{"round",Event.Round},
					{"time",Event.Time},
					{"type",Event.Type},
					{"winner",Event.Winner},
					{"loser",Event.Loser},
					{"killer",Event.Killer.c_str()},
					{"killer_origin",{Event.KillerOrigin[0],Event.KillerOrigin[1],Event.KillerOrigin[2]}},
					{"victim",Event.Victim.c_str()},
					{"victim_origin",{Event.VictimOrigin[0],Event.VictimOrigin[1],Event.VictimOrigin[2]}},
					{"isHeadShot",Event.IsHeadShot},
					{"itemIndex",Event.ItemIndex},
				});
			}
			//
			// Player data
			for (auto const& Data : gStats.m_Data)
			{
				// Auth ID
				const char* Auth = Data.first.c_str();

				// If has valid auth id, save data
				if (Auth && Auth[0] != '\0')
				{
					// Player Stats
					StatsData["players"][Auth]["stats"] =
					{
						{"frags",Data.second.Stats[STATS_FRAGS]},
						{"assists",Data.second.Stats[STATS_ASSISTS]},
						{"deaths",Data.second.Stats[STATS_DEATHS]},
						{"hs",Data.second.Stats[STATS_HEADSHOTS]},
						{"shots",Data.second.Stats[STATS_SHOTS]},
						{"hits",Data.second.Stats[STATS_HITS]},
						{"damage",Data.second.Stats[STATS_DMG_DONE]},
						{"damage_recv",Data.second.Stats[STATS_DMG_RECV]},
						{"flash_assists",Data.second.Stats[STATS_FLASH_ASSIST]},
						{"fr_frags",Data.second.Stats[STATS_FR_FRAGS]},
						{"fr_deaths",Data.second.Stats[STATS_FR_DEATHS]},
						{"tr_frags",Data.second.Stats[STAST_TR_FRAGS]},
						{"tr_deaths",Data.second.Stats[STATS_TR_DEATHS]},
					};
					//
					// Match Stats
					StatsData["players"][Auth]["match"] =
					{
						{"rws",Data.second.RoundWinShare},
						{"flash_time",Data.second.FlashDuration},
					};
					//
					// Round Stats
					StatsData["players"][Auth]["round"] =
					{
						{"play",Data.second.Rounds[ROUND_PLAY]},
						{"win_tr",Data.second.Rounds[ROUND_WIN_TR]},
						{"lose_tr",Data.second.Rounds[ROUND_LOSE_TR]},
						{"win_ct",Data.second.Rounds[ROUND_WIN_CT]},
						{"lose_ct",Data.second.Rounds[ROUND_LOSE_CT]},
					};
					//
					// Bomb Stats
					StatsData["players"][Auth]["bomb"] =
					{
						{"carrier",Data.second.BombStats[BOMB_CARRIER]},
						{"dropped",Data.second.BombStats[BOMB_DROPPED]},
						{"planting",Data.second.BombStats[BOMB_PLANTING]},
						{"planted",Data.second.BombStats[BOMB_PLANTED]},
						{"explode",Data.second.BombStats[BOMB_EXPLODED]},
						{"defusing",Data.second.BombStats[BOMB_DEFUSING]},
						{"defused",Data.second.BombStats[BOMB_DEFUSED]},
						{"defusing_kit",Data.second.BombStats[BOMB_DEFUSING_KIT]},
						{"defused_kit",Data.second.BombStats[BOMB_DEFUSED_KIT]},
					};
					//
					// Hitbox
					StatsData["players"][Auth]["hitbox"] =
					{
						{"generic",Data.second.HitBox[HITGROUP_GENERIC]},
						{"head",Data.second.HitBox[HITGROUP_HEAD]},
						{"chest",Data.second.HitBox[HITGROUP_CHEST]},
						{"stomach",Data.second.HitBox[HITGROUP_STOMACH]},
						{"leftarm",Data.second.HitBox[HITGROUP_LEFTARM]},
						{"rightarm",Data.second.HitBox[HITGROUP_RIGHTARM]},
						{"leftleg",Data.second.HitBox[HITGROUP_LEFTLEG]},
						{"rightleg",Data.second.HitBox[HITGROUP_RIGHTLEG]},
						{"dmg_generic",Data.second.HitBoxDamage[HITGROUP_GENERIC]},
						{"dmg_head",Data.second.HitBoxDamage[HITGROUP_HEAD]},
						{"dmg_chest",Data.second.HitBoxDamage[HITGROUP_CHEST]},
						{"dmg_stomach",Data.second.HitBoxDamage[HITGROUP_STOMACH]},
						{"dmg_leftarm",Data.second.HitBoxDamage[HITGROUP_LEFTARM]},
						{"dmg_rightarm",Data.second.HitBoxDamage[HITGROUP_RIGHTARM]},
						{"dmg_leftleg",Data.second.HitBoxDamage[HITGROUP_LEFTLEG]},
						{"dmg_rightleg",Data.second.HitBoxDamage[HITGROUP_RIGHTLEG]},
					};
					//
					// Streaks And versus
					StatsData["players"][Auth]["streak"] =
					{
						{"k1",Data.second.KillStreak[1]},
						{"k2",Data.second.KillStreak[2]},
						{"k3",Data.second.KillStreak[3]},
						{"k4",Data.second.KillStreak[4]},
						{"k5",Data.second.KillStreak[5]},
						{"v1",Data.second.Versus[1]},
						{"v2",Data.second.Versus[2]},
						{"v3",Data.second.Versus[3]},
						{"v4",Data.second.Versus[4]},
						{"v5",Data.second.Versus[5]},
					};
					//
					// Money
					StatsData["players"][Auth]["money"] =
					{
						{"none",Data.second.Money[RT_NONE]},
						{"round_bonus",Data.second.Money[RT_ROUND_BONUS]},
						{"player_reset",Data.second.Money[RT_PLAYER_RESET]},
						{"player_join",Data.second.Money[RT_PLAYER_JOIN]},
						{"player_spec_join",Data.second.Money[RT_PLAYER_SPEC_JOIN]},
						{"player_bought_something",Data.second.Money[RT_PLAYER_BOUGHT_SOMETHING]},
						{"hostage_took",Data.second.Money[RT_HOSTAGE_TOOK]},
						{"hostage_rescued",Data.second.Money[RT_HOSTAGE_RESCUED]},
						{"hostage_damaged",Data.second.Money[RT_HOSTAGE_DAMAGED]},
						{"hostage_killed",Data.second.Money[RT_HOSTAGE_KILLED]},
						{"teammates_killed",Data.second.Money[RT_TEAMMATES_KILLED]},
						{"enemy_killed",Data.second.Money[RT_ENEMY_KILLED]},
						{"into_game",Data.second.Money[RT_INTO_GAME]},
						{"vip_killed",Data.second.Money[RT_VIP_KILLED]},
						{"vip_rescued_myself",Data.second.Money[RT_VIP_RESCUED_MYSELF]},
					};
					//
					// Hack
					StatsData["players"][Auth]["hack"] =
					{
						{"vision",Data.second.HackStats[HACK_VISION]},
						{"onehit",Data.second.HackStats[HACK_ONEHIT]},
						{"noscope",Data.second.HackStats[HACK_NOSCOP]},
					};
					//
					// Weapon Stats
					if (g_ReGameApi)
					{
						// Loop all weapons
						for (int WeaponIndex = WEAPON_P228; WeaponIndex <= WEAPON_P90; WeaponIndex++)
						{
							// If has shots or damage
							if (Data.second.WeaponStats[WeaponIndex][WEAPON_SHOT] && Data.second.WeaponStats[WeaponIndex][WEAPON_DAMAGE])
							{
								// Get Weapon Info
								auto info = g_ReGameApi->GetWeaponInfo(WeaponIndex);
								//
								// If has data
								if (info)
								{
									// If has entity name
									if (info->entityName != nullptr)
									{
										StatsData["players"][Auth]["weapon"][info->entityName] =
										{
											{"frags",Data.second.WeaponStats[WeaponIndex][WEAPON_KILL]},
											{"assists",Data.second.WeaponStats[WeaponIndex][WEAPON_ASSISTS]},
											{"deaths",Data.second.WeaponStats[WeaponIndex][WEAPON_DEATH]},
											{"hs",Data.second.WeaponStats[WeaponIndex][WEAPON_HEADSHOT]},
											{"shots",Data.second.WeaponStats[WeaponIndex][WEAPON_SHOT]},
											{"hits",Data.second.WeaponStats[WeaponIndex][WEAPON_HIT]},
											{"damage",Data.second.WeaponStats[WeaponIndex][WEAPON_DAMAGE]},
											{"damage_recv",Data.second.WeaponStats[WeaponIndex][WEAPON_DAMAGE_R]},
										};
									}
								}
							}
						}
					}
				}
			}
			//
			// Send to web server
			if (StatsData.size())
			{
				char Link[512] = { 0 };

				Q_snprintf(Link, sizeof(Link), "%s?stats=true", gCvars.GetApiUrl()->string);

				if (Link[0] && Link[0] != '\0')
				{
					gLibCurl.PostJSON(Link, StatsData.dump(), NULL, 0);
				}
			}
		}
	}
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
		if (Player->IsPlayer() && !Player->IsAlive())
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
					}

					this->m_Data[Auth].RoundWinShare += MANAGER_RWS_C4_DEFUSED;

					auto mp_give_c4_frags = CVAR_GET_POINTER("mp_give_c4_frags");

					this->m_Data[Auth].Stats[STATS_FRAGS] += mp_give_c4_frags ? (int)mp_give_c4_frags->value : 3;
				}
			}
		}
	}
}

void CStats::ExplodeBomb(CGrenade* pThis, TraceResult* ptr, int bitsDamageType)
{
	if (gPugMod.IsLive())
	{
		if (pThis->m_bIsC4)
		{
			if (pThis->pev->owner)
			{
				auto Player = UTIL_PlayerByIndexSafe(ENTINDEX(pThis->pev->owner));

				if (Player)
				{
					auto Auth = GET_AUTH(Player->edict());

					if (Auth && Auth[0] != '\0')
					{
						this->m_Data[Auth].BombStats[BOMB_EXPLODED]++;

						this->m_Data[Auth].RoundWinShare += MANAGER_RWS_C4_EXPLODE;

						auto mp_give_c4_frags = CVAR_GET_POINTER("mp_give_c4_frags");

						this->m_Data[Auth].Stats[STATS_FRAGS] += mp_give_c4_frags ? (int)mp_give_c4_frags->value : 3;
					}
				}
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

void CStats::OnEvent(GameEventType event, CBaseEntity* pEntity, class CBaseEntity* pEntityOther)
{
	if (event == EVENT_PLAYER_DIED || event == EVENT_BOMB_PLANTED || event == EVENT_BOMB_DEFUSED || event == EVENT_BOMB_EXPLODED || event == EVENT_TERRORISTS_WIN || event == EVENT_CTS_WIN)
	{
		if (gPugMod.IsLive())
		{
			P_ROUND_EVENT Event = { 0 };

			Event.Round = gPugMod.GetRound();

			if (g_pGameRules)
			{
				Event.Time = CSGameRules()->GetRoundRemainingTimeReal();
			}

			Event.Type = event;

			switch (event)
			{
				case EVENT_PLAYER_DIED: // tell bots the player is killed (argumens: 1 = victim, 2 = killer)
				{
					if (pEntity && pEntityOther)
					{
						auto Victim = UTIL_PlayerByIndexSafe(pEntity->entindex());

						auto Killer = UTIL_PlayerByIndexSafe(pEntityOther->entindex());

						if (Victim && Killer)
						{
							Event.Killer = GET_AUTH(Killer->edict());
							Event.KillerOrigin = Killer->edict()->v.origin;

							Event.Victim = GET_AUTH(Victim->edict());
							Event.VictimOrigin = Victim->edict()->v.origin;

							Event.Winner = Killer->m_iTeam;

							Event.Loser = Victim->m_iTeam;

							Event.IsHeadShot = Victim->m_bHeadshotKilled;

							Event.ItemIndex = (Victim->m_bKilledByGrenade) ? WEAPON_HEGRENADE : ((Killer->m_pActiveItem) ? Killer->m_pActiveItem->m_iId : WEAPON_NONE);
						}
					}

					break;
				}
				case EVENT_BOMB_PLANTED: // tell bots the bomb has been planted (argumens: 1 = planter, 2 = NULL)
				{
					auto Planter = UTIL_PlayerByIndexSafe(pEntity->entindex());

					if (Planter)
					{
						Event.Killer = GET_AUTH(Planter->edict());

						Event.KillerOrigin = Planter->edict()->v.origin;
					}

					Event.Winner = TERRORIST;

					Event.Loser = CT;

					Event.IsHeadShot = 0;

					Event.ItemIndex = WEAPON_C4;

					break;
				}
				case EVENT_BOMB_DEFUSED: // tell the bots the bomb is defused (argumens: 1 = defuser, 2 = NULL)
				{
					auto Defuser = UTIL_PlayerByIndexSafe(pEntity->entindex());

					if(Defuser)
					{
						Event.Killer = GET_AUTH(Defuser->edict());

						Event.KillerOrigin = Defuser->edict()->v.origin;

						Event.IsHeadShot = Defuser->m_bHasDefuser ? 1 : 0;
					}

					Event.Winner = CT;

					Event.Loser = TERRORIST;

					Event.ItemIndex = WEAPON_NONE;

					break;
				}
				case EVENT_BOMB_EXPLODED: // let the bots hear the bomb exploding (argumens: 1 = NULL, 2 = NULL)
				{
					Event.Winner = TERRORIST;

					Event.Loser = CT;

					Event.IsHeadShot = false;

					Event.ItemIndex = WEAPON_C4;

					break;
				}
				case EVENT_TERRORISTS_WIN: // tell bots the terrorists won the round (argumens: 1 = NULL, 2 = NULL)
				{
					Event.Winner = TERRORIST;

					Event.Loser = CT;

					Event.IsHeadShot = false;

					Event.ItemIndex = WEAPON_NONE;

					break;
				}
				case EVENT_CTS_WIN: // tell bots the CTs won the round (argumens: 1 = NULL, 2 = NULL)
				{
					Event.Winner = CT;

					Event.Loser = TERRORIST;

					Event.IsHeadShot = false;

					Event.ItemIndex = WEAPON_NONE;

					break;
				}
			}

			this->m_Event.push_back(Event);
		}
	}

	if (event == EVENT_ROUND_START || event == EVENT_BUY_TIME_START)
	{
		if (g_pGameRules)
		{
			LOG_CONSOLE(PLID, "[%s] CSGameRules()->m_bMapHasBuyZone: %d", __func__, CSGameRules()->m_bMapHasBuyZone);
			if (CSGameRules()->m_bMapHasBuyZone)
			{
				edict_t* BuyZone = nullptr;

				while ((BuyZone = FIND_ENTITY_BY_CLASSNAME(BuyZone, "func_buyzone")) != nullptr)
				{
					LOG_CONSOLE
					(
						PLID,
						"[%s] (%s) (%d)",
						__func__,
						STRING(BuyZone->v.classname),
						ENTINDEX(BuyZone)
					);
				}
			}
		}
	}
}

void CStats::CheckMapConditions()
{
	//if (g_pGameRules)
	//{
	//	if (g_ReGameFuncs)
	//	{
	//		if (CSGameRules()->m_bMapHasBombTarget)
	//		{
	//			char bomb_target[3][20] =
	//			{
	//				"func_bomb_target",
	//				"info_bomb_target",
	//			};

	//			CBaseEntity* BombTarget = nullptr;

	//			for (int i = 0; i < sizeof(bomb_target); i++)
	//			{
	//				while ((BombTarget = g_ReGameFuncs->UTIL_FindEntityByString(BombTarget, "classname", bomb_target[i])) != nullptr)
	//				{
	//					CBaseEntity* PlayerStart = nullptr;

	//					while ((PlayerStart = g_ReGameFuncs->UTIL_FindEntityByString(PlayerStart, "classname", "info_player_start")) != nullptr)
	//					{
	//						LOG_CONSOLE
	//						(
	//							PLID,
	//							"[%s] (%s)(%d) -> (%s) (%d) Distance: %f",
	//							__func__,
	//							STRING(PlayerStart->pev->classname),
	//							PlayerStart->entindex(),
	//							STRING(BombTarget->pev->classname),
	//							BombTarget->entindex(),
	//							(PlayerStart->Center() - BombTarget->Center()).Length()
	//						);
	//						//
	//						break;
	//					}
	//				}
	//			}
	//		}
	//	}
	//}
}