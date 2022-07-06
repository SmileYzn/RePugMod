#include "precompiled.h"

CVoteTeam gVoteTeam;

void CVoteTeam::Init()
{
	this->m_Data.clear();

	this->m_Data.insert(std::make_pair(0, P_VOTE_TEAM_ITEM(0, 0, _T("Leaders Sorted"))));
	this->m_Data.insert(std::make_pair(1, P_VOTE_TEAM_ITEM(1, 0, _T("Random"))));
	this->m_Data.insert(std::make_pair(2, P_VOTE_TEAM_ITEM(2, 0, _T("Not Sorted"))));
	this->m_Data.insert(std::make_pair(3, P_VOTE_TEAM_ITEM(3, 0, _T("Skill Sorted"))));
	this->m_Data.insert(std::make_pair(4, P_VOTE_TEAM_ITEM(4, 0, _T("Swap Teams"))));
	this->m_Data.insert(std::make_pair(5, P_VOTE_TEAM_ITEM(5, 0, _T("Knife Round"))));

	CBasePlayer* Players[MAX_CLIENTS] = { NULL };

	auto Num = gPlayer.GetList(Players, true);

	for (int i = 0; i < Num; i++)
	{
		auto Player = Players[i];

		if (Player)
		{
			auto EntityIndex = Player->entindex();

			gMenu[EntityIndex].Create(_T("Game Mode:"), false, (void*)this->MenuHandle);

			for (auto const& [Key, Item] : this->m_Data)
			{
				gMenu[EntityIndex].AddItem(Key, Item.Name);
			}

			gMenu[EntityIndex].Show(EntityIndex);
		}
	}

	gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, _T("Select teams started."));

	gTask.Create(PUG_TASK_VOTE, gCvars.GetVoteDelay()->value, false, (void*)this->Stop);

	gTask.Create(PUG_TASK_LIST, 0.5f, true, (void*)this->List);
}

void CVoteTeam::AddVote(int Item, int Vote)
{
	auto it = this->m_Data.find(Item);

	if (it != this->m_Data.end())
	{
		it->second.Votes += Vote;
	}

	this->List();
}

auto CVoteTeam::GetVote()
{
	return this->m_Data;
}

void CVoteTeam::MenuHandle(int EntityIndex, P_MENU_ITEM Item)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		gVoteTeam.AddVote(Item.Info, 1);

		gUtil.SayText(NULL, Player->entindex(), _T("\3%s\1 choosed \3%s\1"), STRING(Player->edict()->v.netname), Item.Text.c_str());

		if (gVoteTeam.GetCount() >= gPlayer.GetNum(false))
		{
			gVoteTeam.Stop();
		}
	}
}

void CVoteTeam::Stop()
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

	auto GameMode = gVoteTeam.GetWinner();

	if (GameMode.Votes > 0)
	{
		gVoteTeam.SetMode(GameMode.Index);
	}
	else
	{
		gPugMod.NextState(3.0);

		gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, _T("The choice of the teams failed: \3No votes."));
	}
}

void CVoteTeam::List()
{
	std::string VoteList;

	for (auto const& [Key, Item] : gVoteTeam.GetVote())
	{
		if (Item.Votes > 0)
		{
			VoteList += Item.Name;
			VoteList += " - ";
			VoteList += std::to_string(Item.Votes);
			VoteList += " ";
			VoteList += (Item.Votes > 1) ? _T("votes") : _T("vote");
			VoteList += "\n";
		}
	}

	gUtil.HudMessage(NULL, gUtil.HudParam(0, 255, 0, 0.23, 0.02, 0, 0.0, 0.53, 0.0, 0.0, 1), _T("Game Mode (%d):"), (int)gTask.Timeleft(PUG_TASK_VOTE));

	gUtil.HudMessage(NULL, gUtil.HudParam(255, 255, 225, 0.23, 0.02, 0, 0.0, 0.53, 0.0, 0.0, 2), "\n%s", VoteList.length() ? VoteList.c_str() : _T("No Votes."));
}

int CVoteTeam::GetCount()
{
	int Count = 0;

	for (auto const& [Key, Item] : this->m_Data)
	{
		Count += Item.Votes;
	}

	return Count;
}

P_VOTE_TEAM_ITEM CVoteTeam::GetWinner()
{
	int Winner = 0;
	int WinnerVotes = 0;

	for (auto const& [Key, Item] : this->m_Data)
	{
		if (Item.Votes > WinnerVotes)
		{
			Winner = Key;
			WinnerVotes = Item.Votes;
		}
		else if (Item.Votes == WinnerVotes)
		{
			if (RANDOM_LONG(0, 1))
			{
				Winner = Key;
				WinnerVotes = Item.Votes;
			}
		}
	}

	return this->m_Data.at(Winner);
}

void CVoteTeam::SetMode(int GameMode)
{
	switch (GameMode)
	{
		case 0: // The Leaders will choose Players.
		{
			gCaptain.Init();
			break;
		}
		case 1: // Randomize Teams
		{
			this->TeamsRandomize();

			gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, _T("Mixing teams now."));

			gKnifeRound.Init();
			break;
		}
		case 2: // Same Teams
		{
			gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, _T("The teams will remain the same."));

			gKnifeRound.Init();
			break;
		}
		case 3: // Skill Balanced
		{
			this->TeamsOptimize();

			gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, _T("Balancing teams by skills."));

			gKnifeRound.Init();
			break;
		}
		case 4: // Swap Teams
		{
			if (g_pGameRules)
			{
				CSGameRules()->SwapAllPlayers();
			}

			gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, _T("Swaping teams now."));

			gKnifeRound.Init();
			break;
		}
		case 5: // Knife Round
		{
			gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, _T("The teams will remain the same."));
			
			gKnifeRound.Init(true);
			break;
		}
	}
}

void CVoteTeam::TeamsRandomize()
{
	// Entity Array
	CBasePlayer* Players[MAX_CLIENTS] = { NULL };

	// Get players that are joined in TR or CT
	auto Num = gPlayer.GetList(Players, true);

	// Set first random team (TR or CT)
	TeamName Team = (TeamName)RANDOM_LONG(TERRORIST, CT);

	// While have players count
	while (Num)
	{
		// Get Random index from player count minus one (Like Num)
		auto Random = RANDOM_LONG(0, Num - 1);

		// Pointer
		auto Player = Players[Random];

		// If is not null
		if (Player)
		{
			// Set Team
			Player->CSPlayer()->JoinTeam(Team);

			// Remove this entity from players count number
			Players[Random] = Players[--Num];

			// Set next team for next random player
			Team = (TeamName)(Team % CT + TERRORIST);
		}
	}

	// If has CSGameRules
	if (g_pGameRules)
	{
		// Restart Round
		CSGameRules()->RestartRound();
	}
}

void CVoteTeam::TeamsOptimize()
{
	// Entity Array
	CBasePlayer* Players[MAX_CLIENTS] = { NULL };

	// Skills Array
	std::array<float, 32> Skills = { 0 };

	// Sorted Skills Array
	std::array<float, 32> Sorted = { 0 };

	// Get players that are joined in TR or CT
	auto Num = gPlayer.GetList(Players, true);

	// Loop
	for (int i = 0; i < Num; i++)
	{
		auto Player = Players[i];

		if (Player)
		{
			// Get Frags
			int Frags = Player->edict()->v.frags;

			// Get Deaths
			int Death = Player->m_iDeaths;

			// Store Efficiency
			Sorted[i] = Skills[i] = (Frags ? (100.0f * float(Frags) / float(Frags + Death)) : 0);
		}
	}

	// Sort skill percentage descending in array
	std::sort(Sorted.begin(), Sorted.end(), std::greater<int>());

	// The first random team for most skilled player
	TeamName Team = (TeamName)RANDOM_LONG(TERRORIST, CT);

	// Loop sorted array
	for (int i = 0; i < sizeof(Sorted); i++)
	{
		// Loop players
		for (int j = 0; j < Num; j++)
		{
			// Pointer
			auto Player = Players[j];

			// If is not null
			if (Player)
			{
				// If player skill math with sorted array position
				if (Skills[j] == Sorted[i])
				{
					// Set entity team
					Player->CSPlayer()->JoinTeam(Team);

					// Get the next team oposite from current team for next skilled player
					Team = (TeamName)(Team % CT + TERRORIST);
				}
			}
		}
	}

	// If has CSGameRules
	if (g_pGameRules)
	{
		// Restart Round
		CSGameRules()->RestartRound();
	}
}