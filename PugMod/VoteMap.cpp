#include "precompiled.h"

CVoteMap gVoteMap;

void CVoteMap::Load()
{
	this->m_Data.clear();
	this->m_Maps.clear();

	this->m_Maps = gUtil.LoadMapList(VOTE_MAP_FILE, gCvars.GetVoteMapSelf()->value ? true : false);

	int ItemIndex = 0;

	for (auto MapName : this->m_Maps)
	{
		this->m_Data.insert(std::make_pair(ItemIndex, P_VOTE_MAP_ITEM(ItemIndex, 0, MapName)));

		ItemIndex++;
	}
}

void CVoteMap::Init()
{
	this->Load();

	CBasePlayer* Players[MAX_CLIENTS] = { NULL };

	auto Num = gPlayer.GetList(Players, true);

	for (int i = 0; i < Num; i++)
	{
		auto Player = Players[i];

		if (Player)
		{
			auto EntityIndex = Player->entindex();

			gMenu[EntityIndex].Create(_T("Vote Map:"), false, (void*)this->MenuHandle);

			gMenu[EntityIndex].AddList(this->m_Maps);

			gMenu[EntityIndex].Show(EntityIndex);
		}
	}

	gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, _T("Starting Vote Map."));

	gTask.Create(PUG_TASK_VOTE, gCvars.GetVoteDelay()->value, false, (void*)this->Stop);

	gTask.Create(PUG_TASK_LIST, 0.5f, true, (void*)this->List);
}

void CVoteMap::AddVote(int Item, int Vote)
{
	auto it = this->m_Data.find(Item);

	if (it != this->m_Data.end())
	{
		it->second.Votes += Vote;
	}

	this->List();
}

auto CVoteMap::GetVote()
{
	return this->m_Data;
}

void CVoteMap::MenuHandle(int EntityIndex, P_MENU_ITEM Item)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		gVoteMap.AddVote(Item.Info, 1);

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

	gTask.Remove(PUG_TASK_VOTE);

	auto Winner = gVoteMap.GetWinner();

	if (Winner.Votes > 0)
	{
		gUtil.ChangelevelDelay(5.0, Winner.Name.c_str());

		gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, _T("Changing map to \4%s\1..."), Winner.Name.c_str());
	}
	else
	{
		gTask.Remove(PUG_TASK_LIST);

		gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, _T("The map choice has failed: \3No votes."));

		gTask.Create(PUG_TASK_NEXT, 5.0f, false, (void*)gVoteMap.Fail);
	}
}

void CVoteMap::Fail()
{
	switch ((int)gCvars.GetVoteMapFailType()->value)
	{
		case 1:
		{
			gVoteMap.Init();
			break;
		}
		case 2:
		{
			gVoteMap.RandomMap();
			break;
		}
		default:
		{
			gPugMod.NextState(0.0f);
			break;
		}
	}
}

void CVoteMap::List()
{
	std::string VoteList;

	for (auto const& Item : gVoteMap.GetVote())
	{
		if (Item.second.Votes > 0)
		{
			VoteList += Item.second.Name;
			VoteList += " - ";
			VoteList += std::to_string(Item.second.Votes);
			VoteList += " ";
			VoteList += (Item.second.Votes > 1) ? _T("votes") : _T("vote");
			VoteList += "\n";
		}
	}

	auto TimeLeft = gTask.Timeleft(PUG_TASK_VOTE);

	if (TimeLeft > 0)
	{
		gUtil.HudMessage(NULL, gUtil.HudParam(0, 255, 0, 0.23, 0.02, 0, 0.0, 0.53, 0.0, 0.0, 1), _T("Choose the map (%.0f):"), TimeLeft);
	}
	else
	{
		gUtil.HudMessage(NULL, gUtil.HudParam(0, 255, 0, 0.23, 0.02, 0, 0.0, 0.53, 0.0, 0.0, 1), _T("Choose the map:"));
	}

	gUtil.HudMessage(NULL, gUtil.HudParam(255, 255, 225, 0.23, 0.02, 0, 0.0, 0.53, 0.0, 0.0, 2), "\n%s", VoteList.length() ? VoteList.c_str() : _T("No votes..."));
}

int CVoteMap::GetCount()
{
	int Count = 0;

	for (auto const& Item : this->m_Data)
	{
		Count += Item.second.Votes;
	}

	return Count;
}

P_VOTE_MAP_ITEM CVoteMap::GetWinner()
{
	int Winner = 0;
	int WinnerVotes = 0;

	for (auto const& Item : this->m_Data)
	{
		if (Item.second.Votes > WinnerVotes)
		{
			Winner = Item.first;
			WinnerVotes = Item.second.Votes;
		}
		else if (Item.second.Votes == WinnerVotes)
		{
			if (RANDOM_LONG(0, 1))
			{
				Winner = Item.first;
				WinnerVotes = Item.second.Votes;
			}
		}
	}

	return this->m_Data.at(Winner);
}

void CVoteMap::RandomMap()
{
	if (!this->m_Data.empty())
	{
		std::srand(std::time(0));

		auto it = this->m_Data.begin();

		std::advance(it, std::rand() % this->m_Data.size());

		gUtil.ChangelevelDelay(5.0f, it->second.Name.c_str());

		gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, _T("Changing map to \4%s\1..."), it->second.Name.c_str());
	}
}