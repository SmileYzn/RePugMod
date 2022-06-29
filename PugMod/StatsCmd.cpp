#include "precompiled.h"

CStatsCmd gStatsCmd;

bool CStatsCmd::HP(CBasePlayer* Player)
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

bool CStatsCmd::Damage(CBasePlayer* Player)
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

						if(gStats.GetRoundHits(PlayerIndex, TargetIndex))
						{
							StatsCount++;

							gUtil.SayText
							(
								Player->edict(),
								TargetIndex,
								_T("Hit \3%s\1 %d time(s) (Damage %d)"),
								STRING(Players[i]->edict()->v.netname),
								gStats.GetRoundHits(PlayerIndex, TargetIndex),
								gStats.GetRoundDamage(PlayerIndex, TargetIndex)
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

bool CStatsCmd::Received(CBasePlayer* Player)
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

						if (gStats.GetRoundHits(TargetIndex, PlayerIndex))
						{
							StatsCount++;

							gUtil.SayText
							(
								Player->edict(),
								TargetIndex,
								_T("Hit by \3%s\1 %d time(s) (Damage %d)"),
								STRING(Players[i]->edict()->v.netname),
								gStats.GetRoundHits(TargetIndex, PlayerIndex),
								gStats.GetRoundDamage(TargetIndex, PlayerIndex)
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

bool CStatsCmd::Summary(CBasePlayer* Player)
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

							if (gStats.GetRoundHits(PlayerIndex, TargetIndex) || gStats.GetRoundHits(TargetIndex, PlayerIndex))
							{
								StatsCount++;

								gUtil.SayText
								(
									Player->edict(),
									TargetIndex,
									_T("(%d dmg / %d hits) to (%d dmg / %d hits) from \3%s\1 (%d HP)"),
									gStats.GetRoundDamage(PlayerIndex, TargetIndex),
									gStats.GetRoundHits(PlayerIndex, TargetIndex),
									gStats.GetRoundDamage(TargetIndex, PlayerIndex),
									gStats.GetRoundHits(TargetIndex, PlayerIndex),
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