#include "precompiled.h"

CVoteRestart gVoteRestart;

void CVoteRestart::ClientDisconnected(int EntityIndex)
{
	this->m_Votes[EntityIndex] = false;
}

bool CVoteRestart::Check(CBasePlayer* Player)
{
	if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
	{
		if (gCvars.GetVoteRestartLimit()->value)
		{
			if (gPugMod.IsLive())
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

int CVoteRestart::GetVoteCount()
{
	int VoteCount = 0;

	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		if (this->m_Votes[i])
		{
			VoteCount++;
		}
	}

	return VoteCount;
}

void CVoteRestart::VoteRestart(CBasePlayer* Player)
{
	if (this->Check(Player))
	{
		int State = gPugMod.GetState();
		int Limit = gCvars.GetVoteRestartLimit()->value;
		const char* StateName = gPugMod.GetStateName();
		auto PlayerIndex = Player->entindex();

		if (this->m_RestartCount[State] < (int)Limit)
		{
			if (!this->m_Votes[PlayerIndex])
			{
				this->m_Votes[PlayerIndex] = true;

				int VoteCount = this->GetVoteCount();
				int VotesNeed = (int)(gPlayer.GetNum() * gCvars.GetVotePercentage()->value);
				int VotesLack = (VotesNeed - VoteCount);

				if (VotesLack)
				{
					gUtil.SayText(NULL, PlayerIndex, "\3%s\1 from \3%s\1 voted to restart \4%s\1 period: \4%d\1 of \4%d\1 vote(s) to restart period.", STRING(Player->edict()->v.netname), PUG_MOD_TEAM_STR[Player->m_iTeam], StateName, VoteCount, VotesNeed);
					gUtil.SayText(NULL, PlayerIndex, "Say \3.vote\1 for a restart \3%s\1 period.", StateName);
				}
				else
				{
					this->m_RestartCount[State]++;
					
					memset(this->m_Votes, false, sizeof(this->m_Votes));

					gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, "The \4%s\1 period will be restarted: Get Ready!.", StateName);

					gPugMod.RestarPeriod(NULL);
				}
			}
			else
			{
				gUtil.SayText(Player->edict(), PlayerIndex, "You already voted for restart \4%s\1 period.", StateName);
			}
		}
		else
		{
			gUtil.SayText(Player->edict(), PlayerIndex, "Can't restart  \4%s\1 period more than \4%d\1 time(s).", StateName, Limit);
		}
	}
	else
	{
		gUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, _T("Unable to use this command now."));
	}
}
