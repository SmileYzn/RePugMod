#include "precompiled.h"

CVotePause gVotePause;

void CVotePause::Clear()
{
	this->m_Pause = UNASSIGNED;

	memset(this->m_Votes, 0, sizeof(this->m_Votes));
	memset(this->m_Count, 0, sizeof(this->m_Count));
}

void CVotePause::ClientDisconnected(int EntityIndex)
{
	memset(this->m_Votes[EntityIndex], false, sizeof(this->m_Votes[EntityIndex]));
}

bool CVotePause::Check(CBasePlayer* Player)
{
	if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
	{
		if (gPugMod.IsLive())
		{
			if (gCvars.GetVotePauseTime()->value > 0)
			{
				if (!gTask.Exists(PUG_TASK_EXEC) && !gTask.Exists(PUG_TASK_VOTE))
				{
					return true;
				}
			}
		}
	}

	return false;
}

int CVotePause::GetVoteCount(TeamName Team)
{
	int VoteCount = 0;

	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		if (this->m_Votes[i][Team])
		{
			VoteCount++;
		}
	}

	return VoteCount;
}

void CVotePause::VotePause(CBasePlayer* Player)
{
	if (g_pGameRules)
	{
		if (this->Check(Player))
		{
			int State = gPugMod.GetState();

			if (State == PUG_STATE_FIRST_HALF || State == PUG_STATE_SECOND_HALF || State == PUG_STATE_OVERTIME)
			{
				if (gCvars.GetVotePauseTime()->value)
				{
					TeamName PauseTeam = this->GetPauseTeam();

					if (PauseTeam == UNASSIGNED || PauseTeam == Player->m_iTeam)
					{
						if (PauseTeam == Player->m_iTeam || !CSGameRules()->IsFreezePeriod())
						{
							auto VotePauseLimit = gCvars.GetVotePauseLimit()->value;

							if (this->m_Count[Player->m_iTeam][State] <= VotePauseLimit)
							{
								auto PlayerIndex = Player->entindex();

								if (!this->m_Votes[PlayerIndex][Player->m_iTeam])
								{
									this->m_Votes[PlayerIndex][Player->m_iTeam] = true;

									int VoteCount = this->GetVoteCount(Player->m_iTeam);
									int VotesNeed = (int)(gPlayer.GetNum(Player->m_iTeam) * gCvars.GetVotePercentage()->value);
									int VotesLack = (VotesNeed - VoteCount);

									if (VotesLack)
									{
										gUtil.SayText(NULL, PlayerIndex, _T("\3%s\1 from voted for a timeout: \4%d\1 of \4%d\1 vote(s) to run timeout."), STRING(Player->edict()->v.netname), VoteCount, VotesNeed);
										gUtil.SayText(NULL, PlayerIndex, _T("Say \3.vote\1 for a timeout."));
									}
									else
									{
										this->m_Pause = Player->m_iTeam;

										this->m_Count[Player->m_iTeam][State]++;

										gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, _T("The \3%s\1 team paused the game."), PUG_MOD_TEAM_STR[Player->m_iTeam]);

										gUtil.SayText(NULL, PlayerIndex, _T("Match will pause for \4%d\1 seconds on next round."), (int)gCvars.GetVotePauseTime()->value);
									}
								}
								else
								{
									gUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, _T("You already voted for a timeout."));
								}
							}
							else
							{
								gUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, _T("The \3%s\1 team can't pause the game more than %d times."), PUG_MOD_TEAM_STR[Player->m_iTeam], (int)VotePauseLimit);
							}
						}
						else
						{
							gUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, _T("Unable to pause the match while is in freezetime period."));
						}
					}
					else
					{
						gUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, _T("The \3%s\1 team already paused the game."), PUG_MOD_TEAM_STR[Player->m_iTeam]);
					}
				}
				else
				{
					gUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, _T("Unable to vote to pause the match."));
				}
			}
			else
			{
				gUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, _T("Unable to use this command now."));
			}
		}
		else
		{
			gUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, _T("Unable to use this command now."));
		}
	}
}

TeamName CVotePause::GetPauseTeam()
{
	return this->m_Pause;
}

void CVotePause::RoundRestart()
{
	if (gPugMod.IsLive())
	{
		if (gCvars.GetVotePauseTime()->value > 0)
		{
			if (this->GetPauseTeam() != UNASSIGNED)
			{
				memset(this->m_Votes, false, sizeof(this->m_Votes));

				gUtil.SetRoundTime((int)gCvars.GetVotePauseTime()->value, true);

				gTask.Create(PUG_TASK_PAUS, 0.5, true, (void*)this->VotePauseTimer);
			}
		}
	}
}

void CVotePause::RoundStart()
{
	if (this->GetPauseTeam() != UNASSIGNED)
	{
		this->m_Pause = UNASSIGNED;
	}
}

void CVotePause::VotePauseTimer()
{
	if (g_pGameRules)
	{
		time_t RemainTime = (time_t)(gCvars.GetVotePauseTime()->value - (gpGlobals->time - CSGameRules()->m_fRoundStartTime));

		if (RemainTime > 0)
		{
			struct tm* tm_info = localtime(&RemainTime);

			if (tm_info)
			{
				char Time[32] = { 0 };

				if (strftime(Time, sizeof(Time), "%M:%S", tm_info) > 0)
				{
					gUtil.HudMessage(NULL, gUtil.HudParam(0, 255, 0, -1.0, 0.2, 0, 0.6, 0.6), _T("%s PAUSED MATCH\n%s"), PUG_MOD_TEAM_STR[gVotePause.GetPauseTeam()], Time);
				}
			}
		}
		else
		{
			gTask.Remove(PUG_TASK_PAUS);

			gUtil.SetRoundTime((int)CVAR_GET_FLOAT("mp_freezetime"), true);
		}
	}
}