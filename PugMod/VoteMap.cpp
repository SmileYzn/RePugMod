#include "precompiled.h"

CVoteMap gVoteMap;

void CVoteMap::Load()
{
	this->m_Data.clear();
	this->m_Vote.clear();
}

void CVoteMap::Init()
{
	this->m_Vote.clear();

	this->m_Data = gUtil.LoadMapList(VOTE_MAP_FILE, gCvars.GetVoteMapSelf()->value ? true : false);
	
	CBasePlayer* Players[32] = { NULL };

	auto Num = gPlayer.GetList(Players);

	for (int i = 0; i < Num; i++)
	{
		auto Player = Players[i];

		if (Player)
		{
			auto EntityIndex = Player->entindex();

			gMenu[EntityIndex].Create(_T("Vote Map:"), false, (void*)this->MenuHandle);

			gMenu[EntityIndex].AddList(this->m_Data);

			gMenu[EntityIndex].Show(EntityIndex);
		}
	}

	gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, _T("Starting Vote Map."));

	gTask.Create(PUG_TASK_VOTE, gCvars.GetVoteDelay()->value, false, (void*)this->Stop);

	gTask.Create(PUG_TASK_LIST, 0.5f, true, (void*)this->List, this);
}

void CVoteMap::MenuHandle(int EntityIndex, int ItemIndex, bool Disabled, const char* Option)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		gVoteMap.AddVote(ItemIndex, 1);

		gUtil.SayText(NULL, Player->entindex(), _T("\3%s\1 choosed \3%s\1"), STRING(Player->edict()->v.netname), Option);

		if (gVoteMap.GetCount() >= gPlayer.GetNum(false))
		{
			gVoteMap.Stop();
		}
	}
}


void CVoteMap::Stop()
{
	MENU_RESET_ALL();

	gTask.Remove(PUG_TASK_LIST);

	gTask.Remove(PUG_TASK_VOTE);

	int Winner = gVoteMap.GetWinner();

	if (Winner == -1)
	{
		gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, _T("The map choice has failed: \3No votes."));

		gVoteMap.RandomMap(true);
	}
	else 
	{
		gTask.Create(PUG_TASK_EXEC, 5.0f, false, (void*)SERVER_COMMAND, (void*)gUtil.VarArgs("changelevel %s\n", gVoteMap.GetItem(Winner)));

		gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, _T("Changing map to \4%s\1..."), gVoteMap.GetItem(Winner));
	}
}

void CVoteMap::List(CVoteMap* VoteMap)
{
	char VoteList[511] = { 0 };

	for (std::size_t MapIndex = 0; MapIndex < VoteMap->m_Data.size(); MapIndex++)
	{
		if (VoteMap->m_Vote[MapIndex])
		{
			snprintf(VoteList, sizeof(VoteList), "%s%s [%d]\n", VoteList, VoteMap->m_Data[MapIndex].c_str(), VoteMap->m_Vote[MapIndex]);
		}
	}
	
	gUtil.HudMessage(NULL, gUtil.HudParam(0, 255, 0, 0.23, 0.02, 0, 0.0, 0.53, 0.0, 0.0, 1), _T("Choose the map (%d):"),(int)gTask.Timeleft(PUG_TASK_VOTE));

	gUtil.HudMessage(NULL, gUtil.HudParam(255, 255, 225, 0.23, 0.02, 0, 0.0, 0.53, 0.0, 0.0, 2), "\n%s", strlen(VoteList) ? VoteList : _T("No votes."));
}

int CVoteMap::GetCount()
{
	int Count = 0;

	for (std::size_t i = 0; i < this->m_Data.size(); i++)
	{
		Count += this->m_Vote[i];
	}

	return Count;
}

int CVoteMap::GetWinner()
{
	int Winner = 0, WinnerVotes = 0;

	for (std::size_t i = 0; i < this->m_Data.size(); i++)
	{
		if (this->m_Vote[i] > WinnerVotes)
		{
			Winner = i;
			WinnerVotes = this->m_Vote[i];
		}
		else if (this->m_Vote[i] == WinnerVotes)
		{
			if (RANDOM_LONG(0, 1))
			{
				Winner = i;
				WinnerVotes = this->m_Vote[i];
			}
		}
	}

	return this->m_Vote[Winner] == 0 ? -1 : Winner;
}

const char* CVoteMap::GetItem(int ItemIndex)
{
	return this->m_Data[ItemIndex].c_str(); 
}

int CVoteMap::RandomMap(bool Change)
{
	int Random = RANDOM_LONG(0, this->m_Data.size() - 1);

	if (Change)
	{
		gTask.Create(PUG_TASK_EXEC, 5.0f, false, (void*)SERVER_COMMAND, (void*)gUtil.VarArgs("changelevel %s\n", this->m_Data[Random].c_str()));

		gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, _T("Changing map to \4%s\1..."), this->m_Data[Random].c_str());
	}

	return Random;
}
