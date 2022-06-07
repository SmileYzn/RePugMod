#include "precompiled.h"

CVoteMap gVoteMap;

void CVoteMap::Init()
{
	this->m_Data.clear();

	auto ItemList = gUtil.LoadMapList(VOTE_MAP_FILE, gCvars.GetVoteMapSelf()->value ? true : false);

	for (auto Option : ItemList)
	{
		this->m_Data.insert(std::make_pair(Option, 0));
	}

	CBasePlayer* Players[MAX_CLIENTS] = { NULL };

	auto Num = gPlayer.GetList(Players, true);

	for (int i = 0; i < Num; i++)
	{
		auto Player = Players[i];

		if (Player)
		{
			auto EntityIndex = Player->entindex();

			gMenu[EntityIndex].Create(_T("Vote Map:"), false, (void*)this->MenuHandle);

			gMenu[EntityIndex].AddList(ItemList);

			gMenu[EntityIndex].Show(EntityIndex);
		}
	}

	gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, _T("Starting Vote Map."));

	gTask.Create(PUG_TASK_VOTE, gCvars.GetVoteDelay()->value, false, (void*)this->Stop);

	gTask.Create(PUG_TASK_LIST, 0.5f, true, (void*)this->List);
}

void CVoteMap::AddVote(std::string Item, int Vote)
{
	auto it = this->m_Data.find(Item);

	if (it != this->m_Data.end())
	{
		it->second += Vote;
	}
}

std::map<std::string, int> CVoteMap::GetVote()
{
	return this->m_Data;
}

void CVoteMap::MenuHandle(int EntityIndex, P_MENU_ITEM Item)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		gVoteMap.AddVote(Item.Text, 1);

		gUtil.SayText(NULL, Player->entindex(), _T("\3%s\1 choosed \3%s\1"), STRING(Player->edict()->v.netname), Item.Text.c_str());

		if (gVoteMap.GetCount() >= gPlayer.GetNum(false))
		{
			gVoteMap.Stop();
		}
	}
}

void CVoteMap::Stop()
{
	CBasePlayer* Players[MAX_CLIENTS] = { NULL };

	int Num = gPlayer.GetList(Players, true);

	for (int i = 0; i < Num; i++)
	{
		auto Player = Players[i];

		if (Player)
		{
			if (!Player->IsBot())
			{
				int EntityIndex = Player->entindex();

				gMenu[EntityIndex].Hide(EntityIndex);
			}
		}
	}

	gTask.Remove(PUG_TASK_LIST);

	gTask.Remove(PUG_TASK_VOTE);

	auto Winner = gVoteMap.GetWinner();

	if (Winner.length())
	{
		gTask.Create(PUG_TASK_EXEC, 5.0f, false, (void*)gVoteMap.Changelevel);

		gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, _T("Changing map to \4%s\1..."), Winner.c_str());
	}
	else
	{
		gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, _T("The map choice has failed: \3No votes."));

		gVoteMap.RandomMap();
	}
}

void CVoteMap::Changelevel()
{
	auto Winner = gVoteMap.GetWinner();

	if (Winner.length())
	{
		gUtil.ServerCommand("changelevel %s", Winner.c_str());
	}
}

void CVoteMap::List()
{
	std::string VoteList;

	for (auto const& Item : gVoteMap.GetVote())
	{
		if (Item.second > 0)
		{
			VoteList += Item.first;
			VoteList += " - ";
			VoteList += std::to_string(Item.second);
			VoteList += " ";
			VoteList += (Item.second > 1) ? _T("votes") : _T("vote");
			VoteList += "\n";
		}
	}

	gUtil.HudMessage(NULL, gUtil.HudParam(0, 255, 0, 0.23, 0.02, 0, 0.0, 0.53, 0.0, 0.0, 1), _T("Choose the map (%d):"), (int)gTask.Timeleft(PUG_TASK_VOTE));

	gUtil.HudMessage(NULL, gUtil.HudParam(255, 255, 225, 0.23, 0.02, 0, 0.0, 0.53, 0.0, 0.0, 2), "\n%s", VoteList.length() ? VoteList.c_str() : _T("No Votes."));
}

int CVoteMap::GetCount()
{
	int Count = 0;

	for (auto const& Item : this->m_Data)
	{
		Count += Item.second;
	}

	return Count;
}

std::string CVoteMap::GetWinner()
{
	std::string Winner = "";

	int WinnerVotes = 0;

	for (auto const& Item : this->m_Data)
	{
		if (Item.second > WinnerVotes)
		{
			Winner = Item.first;
			WinnerVotes = Item.second;
		}
		else if (Item.second == WinnerVotes)
		{
			if (RANDOM_LONG(0, 1))
			{
				Winner = Item.first;
				WinnerVotes = Item.second;
			}
		}
	}

	return Winner;
}

int CVoteMap::RandomMap()
{
	std::srand(std::time(0));

	auto MapList = gUtil.LoadMapList(VOTE_MAP_FILE, false);

	int Random = (std::rand() % MapList.size());

	gTask.Create(PUG_TASK_EXEC, 5.0f, false, (void*)SERVER_COMMAND, gUtil.VarArgs("changelevel %s\n", MapList[Random].c_str()));

	gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, _T("Changing map to \4%s\1..."), MapList[Random].c_str());

	return Random;
}