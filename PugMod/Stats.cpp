#include "precompiled.h"

CStats gStats;

void CStats::RoundStart()
{
	memset(this->m_Hits, 0, sizeof(this->m_Hits));

	memset(this->m_Damage, 0, sizeof(this->m_Damage));
}

void CStats::TakeDamage(CBasePlayer* Player, entvars_t* pevInflictor, entvars_t* pevAttacker, float& flDamage, int bitsDamageType)
{
	if (g_pGameRules)
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
									auto DamageTaken = (int)(Player->m_iLastClientHealth - clamp(Player->edict()->v.health, 0.0f, Player->edict()->v.health));

									this->m_Hits[Attacker->entindex()][Player->entindex()]++;

									this->m_Damage[Attacker->entindex()][Player->entindex()] += DamageTaken;
								}
							}
						}
					}
				}
			}
		}
	}
}

bool CStats::HP(CBasePlayer* Player)
{
	if (g_pGameRules)
	{
		if (gPugMod.IsLive())
		{
			if (!Player->IsAlive() || CSGameRules()->m_bRoundTerminating || CSGameRules()->IsFreezePeriod())
			{
				if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
				{
					auto StatsCount = 0;

					CBasePlayer* Players[MAX_CLIENTS] = { NULL };

					auto Num = gPlayer.GetList(Players, true);

					for (int i = 0; i < Num; i++)
					{
						auto Target = Players[i];

						if (Target)
						{
							if (Player->m_iTeam != Target->m_iTeam)
							{
								if (Target->IsAlive())
								{
									StatsCount++;

									gUtil.SayText
									(
										Player->edict(),
										Target->entindex(),
										_T("\3%s\1 with %d HP (%d AP)"),
										STRING(Target->edict()->v.netname),
										(int)Target->edict()->v.health,
										(int)Target->edict()->v.armorvalue
									);
								}
							}
						}
					}

					if (!StatsCount)
					{
						gUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, _T("No one is alive."));
					}

					return true;
				}
			}
		}
	}

	gUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, _T("Unable to use this command now."));

	return false;
}

bool CStats::Damage(CBasePlayer* Player)
{
	if (g_pGameRules)
	{
		if (gPugMod.IsLive())
		{
			if (!Player->IsAlive() || CSGameRules()->m_bRoundTerminating || CSGameRules()->IsFreezePeriod())
			{
				if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
				{
					auto StatsCount = 0;

					auto PlayerIndex = Player->entindex();

					CBasePlayer* Players[MAX_CLIENTS] = { NULL };

					auto Num = gPlayer.GetList(Players, true);

					for (int i = 0; i < Num; i++)
					{
						auto TargetIndex = Players[i]->entindex();

						if (this->m_Hits[PlayerIndex][TargetIndex])
						{
							StatsCount++;

							gUtil.SayText
							(
								Player->edict(),
								TargetIndex,
								_T("Hit \3%s\1 %d time(s) (Damage %d)"),
								STRING(Players[i]->edict()->v.netname),
								this->m_Hits[PlayerIndex][TargetIndex],
								this->m_Damage[PlayerIndex][TargetIndex]
							);
						}
					}

					if (!StatsCount)
					{
						gUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, _T("You do not hit anyone in this round."));
					}

					return true;
				}
			}
		}
	}

	gUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, _T("Unable to use this command now."));

	return false;
}

bool CStats::Received(CBasePlayer* Player)
{
	if (g_pGameRules)
	{
		if (gPugMod.IsLive())
		{
			if (!Player->IsAlive() || CSGameRules()->m_bRoundTerminating || CSGameRules()->IsFreezePeriod())
			{
				if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
				{
					auto StatsCount = 0;

					auto PlayerIndex = Player->entindex();

					CBasePlayer* Players[MAX_CLIENTS] = { NULL };

					auto Num = gPlayer.GetList(Players, true);

					for (int i = 0; i < Num; i++)
					{
						auto TargetIndex = Players[i]->entindex();

						if (this->m_Hits[TargetIndex][PlayerIndex])
						{
							StatsCount++;

							gUtil.SayText
							(
								Player->edict(),
								TargetIndex,
								_T("Hit by \3%s\1 %d time(s) (Damage %d)"),
								STRING(Players[i]->edict()->v.netname),
								this->m_Hits[TargetIndex][PlayerIndex],
								this->m_Damage[TargetIndex][PlayerIndex]
							);
						}
					}

					if (!StatsCount)
					{
						gUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, _T("You were not reached in this round."));
					}

					return true;
				}
			}
		}
	}

	gUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, _T("Unable to use this command now."));

	return false;
}

bool CStats::Summary(CBasePlayer* Player)
{
	if (g_pGameRules)
	{
		if (gPugMod.IsLive())
		{
			if (!Player->IsAlive() || CSGameRules()->m_bRoundTerminating || CSGameRules()->IsFreezePeriod())
			{
				if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
				{
					auto StatsCount = 0;

					auto PlayerIndex = Player->entindex();

					CBasePlayer* Players[MAX_CLIENTS] = { NULL };

					auto Num = gPlayer.GetList(Players, true);

					for (int i = 0; i < Num; i++)
					{
						auto Target = Players[i];

						if (Target)
						{
							auto TargetIndex = Target->entindex();

							if (this->m_Hits[PlayerIndex][TargetIndex] || this->m_Hits[TargetIndex][PlayerIndex])
							{
								StatsCount++;

								gUtil.SayText
								(
									Player->edict(),
									TargetIndex,
									_T("(%d dmg / %d hits) to (%d dmg / %d hits) from \3%s\1 (%d HP)"),
									this->m_Damage[PlayerIndex][TargetIndex],
									this->m_Hits[PlayerIndex][TargetIndex],
									this->m_Damage[TargetIndex][PlayerIndex],
									this->m_Hits[TargetIndex][PlayerIndex],
									STRING(Target->edict()->v.netname),
									Target->IsAlive() ? (int)Target->edict()->v.health : 0
								);
							}
						}
					}

					if (!StatsCount)
					{
						gUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, _T("No stats in this round."));
					}

					return true;
				}
			}
		}
	}

	gUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, _T("Unable to use this command now."));

	return false;
}
