#include "precompiled.h"

CStats gStats;

void CStats::ClientGetIntoGame(CBasePlayer* Player)
{
	if (gPugMod.IsLive())
	{
		auto EntityIndex = Player->entindex();

		for (int i = 1; i <= gpGlobals->maxClients; ++i)
		{
			this->m_Stats[EntityIndex][i][PUG_STATS_HIT] = 0;
			this->m_Stats[EntityIndex][i][PUG_STATS_DMG] = 0;
		}
	}
}

void CStats::RoundStart()
{
	if (gPugMod.IsLive())
	{
		for (int i = 1; i <= gpGlobals->maxClients; ++i)
		{
			for (int j = 1; j <= gpGlobals->maxClients; ++j)
			{
				this->m_Stats[i][j][PUG_STATS_HIT] = 0;
				this->m_Stats[i][j][PUG_STATS_DMG] = 0;
			}
		}
	}
}

void CStats::RoundEnd(int winStatus, ScenarioEventEndRound event, float tmDelay)
{
	if (winStatus != WINSTATUS_NONE)
	{
		if (gCvars.GetStatsRoundEnd()->value)
		{
			if (gPugMod.IsLive())
			{
				CBasePlayer* Players[MAX_CLIENTS] = { NULL };

				auto Num = gPlayer.GetList(Players, true);

				for (int i = 0; i < Num; i++)
				{
					auto Player = Players[i];

					if (Player)
					{
						if (!Player->IsBot())
						{
							auto PlayerIndex = Player->entindex();

							for (int j = 0; j < Num; j++)
							{
								auto TargetIndex = Players[j]->entindex();

								if (this->m_Stats[PlayerIndex][TargetIndex][PUG_STATS_HIT] || this->m_Stats[TargetIndex][PlayerIndex][PUG_STATS_HIT])
								{
									gUtil.ClientPrint
									(
										Players[i]->edict(),
										PRINT_CONSOLE,
										_T("(%d dmg / %d hits) to (%d dmg / %d hits) from %s (%d HP)"),
										this->m_Stats[PlayerIndex][TargetIndex][PUG_STATS_DMG],
										this->m_Stats[PlayerIndex][TargetIndex][PUG_STATS_HIT],
										this->m_Stats[TargetIndex][PlayerIndex][PUG_STATS_DMG],
										this->m_Stats[TargetIndex][PlayerIndex][PUG_STATS_HIT],
										STRING(Players[j]->edict()->v.netname),
										Players[j]->IsAlive() ? (int)Players[j]->edict()->v.health : 0
									);
								}
							}
						}
					}
				}
			}
		}
	}
}

void CStats::CBasePlayer_TakeDamage(CBasePlayer* pthis, entvars_t* pevInflictor, entvars_t* pevAttacker, float & flDamage, int bitsDamageType)
{
	if (g_pGameRules)
	{
		if (gPugMod.IsLive())
		{
			auto pEdict = ENT(pevAttacker);

			if (pEdict)
			{
				auto pAttacker = (CBaseEntity*)GET_PRIVATE(pEdict);

				if (pAttacker)
				{
					int AttackerIndex = pAttacker->entindex();

					if (AttackerIndex >= 1 && AttackerIndex <= gpGlobals->maxClients)
					{
						int VictimIndex = pthis->entindex();

						if (AttackerIndex != VictimIndex)
						{
							if (CSGameRules()->FPlayerCanTakeDamage(pthis, pAttacker))
							{
								this->m_Stats[AttackerIndex][VictimIndex][PUG_STATS_HIT]++;

								this->m_Stats[AttackerIndex][VictimIndex][PUG_STATS_DMG] += (int)flDamage;
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

bool CStats::Damage(CBasePlayer * Player)
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

						if (this->m_Stats[PlayerIndex][TargetIndex][PUG_STATS_HIT])
						{
							StatsCount++;

							gUtil.SayText
							(
								Player->edict(),
								TargetIndex,
								_T("Hit \3%s\1 %d time(s) (Damage %d)"),
								STRING(Players[i]->edict()->v.netname),
								this->m_Stats[PlayerIndex][TargetIndex][PUG_STATS_HIT],
								this->m_Stats[PlayerIndex][TargetIndex][PUG_STATS_DMG]
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

bool CStats::Received(CBasePlayer * Player)
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

						if (this->m_Stats[TargetIndex][PlayerIndex][PUG_STATS_HIT])
						{
							StatsCount++;

							gUtil.SayText
							(
								Player->edict(),
								TargetIndex,
								_T("Hit by \3%s\1 %d time(s) (Damage %d)"),
								STRING(Players[i]->edict()->v.netname),
								this->m_Stats[TargetIndex][PlayerIndex][PUG_STATS_HIT],
								this->m_Stats[TargetIndex][PlayerIndex][PUG_STATS_DMG]
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

							if (this->m_Stats[PlayerIndex][TargetIndex][PUG_STATS_HIT] || this->m_Stats[TargetIndex][PlayerIndex][PUG_STATS_HIT])
							{
								StatsCount++;

								gUtil.SayText
								(
									Player->edict(),
									TargetIndex,
									_T("(%d dmg / %d hits) to (%d dmg / %d hits) from \3%s\1 (%d HP)"),
									this->m_Stats[PlayerIndex][TargetIndex][PUG_STATS_DMG],
									this->m_Stats[PlayerIndex][TargetIndex][PUG_STATS_HIT],
									this->m_Stats[TargetIndex][PlayerIndex][PUG_STATS_DMG],
									this->m_Stats[TargetIndex][PlayerIndex][PUG_STATS_HIT],
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