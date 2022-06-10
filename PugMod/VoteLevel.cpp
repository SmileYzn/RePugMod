#include "precompiled.h"

CVoteLevel gVoteLevel;

void CVoteLevel::ClientDisconnected(int EntityIndex)
{
	memset(this->m_Votes[EntityIndex], false, sizeof(this->m_Votes[EntityIndex]));
}

bool CVoteLevel::Check(CBasePlayer* Player)
{
	if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
	{
		if (this->m_Maps.empty())
		{
			this->m_Maps = gUtil.LoadMapList(VOTE_MENU_MAPS_FILE, false);
		}

		if (gPugMod.IsLive() || gPugMod.GetState() == PUG_STATE_WARMUP)
		{
			if (!gTask.Exists(PUG_TASK_EXEC) && !gTask.Exists(PUG_TASK_VOTE))
			{
				return true;
			}
		}
	}

	return false;
}

void CVoteLevel::Menu(CBasePlayer* Player)
{
	if (this->Check(Player))
	{
		auto PlayerIndex = Player->entindex();

		gMenu[PlayerIndex].Create(_T("Vote Map"), true, (void*)this->MenuHandle);

		char Text[128] = { 0 };

		int VotesNeed = (int)(gCvars.GetPlayersMin()->value * gCvars.GetVotePercentage()->value);

		for (std::size_t i = 0; i < this->m_Maps.size(); i++)
		{
			snprintf(Text, sizeof(Text), "%s \\y\\R%2.0f%%", this->m_Maps[i].c_str(), (float)((this->GetVoteCount(i) * 100) / VotesNeed));

			gMenu[PlayerIndex].AddItem(i, Text, this->m_Votes[PlayerIndex][i]);
		}

		gMenu[PlayerIndex].Show(PlayerIndex);
	}
	else
	{
		gUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, _T("Unable to use this command now."));
	}
}

void CVoteLevel::MenuHandle(int EntityIndex, P_MENU_ITEM Item)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		gVoteLevel.VoteLevel(Player, Item.Info, Item.Disabled);
	}
}

int CVoteLevel::GetVoteCount(int MapIndex)
{
	int VoteCount = 0;

	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		if (this->m_Votes[i][MapIndex])
		{
			VoteCount++;
		}
	}

	return VoteCount;
}

void CVoteLevel::VoteLevel(CBasePlayer* Player, int MapIndex, bool Disabled)
{
	if (!Disabled)
	{
		auto PlayerIndex = Player->entindex();

		this->m_Votes[PlayerIndex][MapIndex] = true;

		int VoteCount = this->GetVoteCount(MapIndex);
		int VotesNeed = (int)(gCvars.GetPlayersMin()->value * gCvars.GetVotePercentage()->value);
		int VotesLack = (VotesNeed - VoteCount);

		if (VotesLack)
		{
			gUtil.SayText(NULL, PlayerIndex, _T("\3%s\1 nomitated \4%s\1: \4%d\1 of \4%d\1 votes to change map."), STRING(Player->edict()->v.netname), this->m_Maps[MapIndex].c_str(), VoteCount, VotesLack);
			gUtil.SayText(NULL, PlayerIndex, _T("Say \3.vote\1 to nominate a map."));
		}
		else
		{
			gTask.Create(PUG_TASK_EXEC, 5.0f, false, (void*)SERVER_COMMAND, gUtil.VarArgs("changelevel %s\n", this->m_Maps[MapIndex].c_str()));

			gUtil.SayText(NULL, PlayerIndex, _T("Changing map to \4%s\1..."), this->m_Maps[MapIndex].c_str());
		}
	}
	else
	{
		gUtil.SayText(Player->edict(), Player->entindex(), _T("Already nominated \3%s\1..."), this->m_Maps[MapIndex].c_str());
	}
}
