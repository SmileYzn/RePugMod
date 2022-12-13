#include "precompiled.h"

CVoteStop gVoteStop;

void CVoteStop::ClientDisconnected(int EntityIndex)
{
	memset(this->m_VoteStop[EntityIndex], false, sizeof(this->m_VoteStop[EntityIndex]));
}

bool CVoteStop::Check(CBasePlayer* Player)
{
	if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
	{
		if (gPugMod.IsLive())
		{
			return !gTask.Exists(PUG_TASK_EXEC);
		}
	}

	return false;
}

int CVoteStop::GetVoteCount(TeamName Team)
{
	int VoteCount = 0;

	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		if (this->m_VoteStop[i][Team])
		{
			VoteCount++;
		}
	}

	return VoteCount;
}

void CVoteStop::VoteStop(CBasePlayer* Player)
{
	if (this->Check(Player))
	{
		if (gPugMod.IsLive())
		{
			auto PlayerIndex = Player->entindex();

			if (!this->m_VoteStop[PlayerIndex][Player->m_iTeam])
			{
				this->m_VoteStop[PlayerIndex][Player->m_iTeam] = true;

				int VoteCount = this->GetVoteCount(Player->m_iTeam);
				int VotesNeed = (int)(gPlayer.GetNum(Player->m_iTeam) * gCvars.GetVotePercentage()->value);
				int VotesLack = (VotesNeed - VoteCount);

				if (VotesLack)
				{
					gUtil.SayText(nullptr, PlayerIndex, _T("\3%s\1 voted for surrender: \4%d\1 of \4%d\1 vote(s) to stop the match."), STRING(Player->edict()->v.netname), VoteCount, VotesNeed);
					gUtil.SayText(nullptr, PlayerIndex, _T("Say \3.vote\1 to vote for stop the match."));
				}
				else
				{
					gUtil.SayText(nullptr, PlayerIndex, _T("Game Over! The \3%s\1 Surrendered!"), PUG_MOD_TEAM_STR[Player->m_iTeam]);

					gPugMod.EndGame(Player->m_iTeam == TERRORIST ? CT : TERRORIST);
				}
			}
			else
			{
				gUtil.SayText(Player->edict(), PlayerIndex, _T("You already voted to stop the match."));
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
