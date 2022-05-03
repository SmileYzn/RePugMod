#include "precompiled.h"

CVoteTeam gVoteTeam;

void CVoteTeam::Load()
{
	this->m_Data.clear();
	this->m_Vote.clear();

	this->m_Data.push_back("Leaders Sorted");
	this->m_Data.push_back("Random");
	this->m_Data.push_back("Not Sorted");
	this->m_Data.push_back("Skill Sorted");
	this->m_Data.push_back("Swap Teams");
	this->m_Data.push_back("Knife Round");
}

void CVoteTeam::Init()
{
	this->m_Vote.clear();

	CBasePlayer* Players[32] = { NULL };

	int Num = gPlayer.GetList(Players);

	for (int i = 0; i < Num; i++)
	{
		auto EntityIndex = Players[i]->entindex();

		gMenu[EntityIndex].Create("Game Mode:", false, this->MenuHandle);

		gMenu[EntityIndex].AddList(this->m_Data);

		gMenu[EntityIndex].Show(EntityIndex);
	}

	gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, "Select teams started.");

	gTask.Create(PUG_TASK_VOTE, gCvars.GetVoteDelay()->value, false, this->Stop);

	gTask.Create(PUG_TASK_LIST, 0.5f, true, this->List, this);
}

void CVoteTeam::MenuHandle(int EntityIndex, int ItemIndex, bool Disabled, const char* Option)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		gVoteTeam.AddVote(ItemIndex, 1);

		gUtil.SayText(NULL, Player->entindex(), "\3%s\1 choosed \3%s\1", STRING(Player->edict()->v.netname), Option);

		if (gVoteTeam.GetCount() >= gPlayer.GetNum(false))
		{
			gVoteTeam.Stop();
		}
	}
}


void CVoteTeam::Stop()
{
	MENU_RESET_ALL();

	gTask.Remove(PUG_TASK_LIST);

	gTask.Remove(PUG_TASK_VOTE);

	int Winner = gVoteTeam.GetWinner();

	if (Winner == -1)
	{
		gPugMod.NextState(3.0);

		gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, "The choice of the teams failed: \3No votes.");
	}
	else
	{
		gVoteTeam.SetMode(Winner);
	}
}

void CVoteTeam::List(CVoteTeam* VoteTeam)
{
	char VoteList[511] = { 0 };

	for (std::size_t MapIndex = 0; MapIndex < VoteTeam->m_Data.size(); MapIndex++)
	{
		if (VoteTeam->m_Vote[MapIndex])
		{
			snprintf(VoteList, sizeof(VoteList), "%s%s [%d]\n", VoteList, VoteTeam->m_Data[MapIndex].c_str(), VoteTeam->m_Vote[MapIndex]);
		}
	}

	gUtil.HudMessage(NULL, gUtil.HudParam(0, 255, 0, 0.23, 0.02, 0, 0.0, 0.53, 0.0, 0.0, 1), "Game Mode (%d):",(int)gTask.Timeleft(PUG_TASK_VOTE));

	gUtil.HudMessage(NULL, gUtil.HudParam(255, 255, 225, 0.23, 0.02, 0, 0.0, 0.53, 0.0, 0.0, 2), "\n%s", strlen(VoteList) ? VoteList : "No votes.");
}

int CVoteTeam::GetCount()
{
	int Count = 0;

	for (std::size_t i = 0; i < this->m_Data.size(); i++)
	{
		Count += this->m_Vote[i];
	}

	return Count;
}

int CVoteTeam::GetWinner()
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

const char* CVoteTeam::GetItem(int ItemIndex)
{
	return this->m_Data[ItemIndex].c_str();
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

			gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, "Mixing teams now.");

			gPugMod.NextState(3.0f);
			break;
		}
		case 2: // Same Teams
		{
			gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, "The teams will remain the same.");

			gPugMod.NextState(3.0f);
			break;
		}
		case 3: // Skill Balanced
		{
			this->TeamsOptimize();

			gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, "Balancing teams by skills.");

			gPugMod.NextState(3.0f);
			break;
		}
		case 4: // Swap Teams
		{
			if (g_pGameRules)
			{
				CSGameRules()->SwapAllPlayers();
			}

			gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, "Swaping teams now.");

			gPugMod.NextState(3.0f);
			break;
		}
		case 5: // Knife Round
		{
			gUtil.SayText(NULL, PRINT_TEAM_RED, "\3Knife Round not implemented yet.");

			this->SetMode(1);
			break;
		}
	}
}


void CVoteTeam::TeamsRandomize()
{
	// Entity Array
	CBasePlayer* Players[32] = { NULL };

	// Get players that are joined in TR or CT
	int Num = gPlayer.GetList(Players);

	// Set first random team (TR or CT)
	TeamName Team = (TeamName)RANDOM_LONG(TERRORIST, CT);

	// While have players count
	while (Num)
	{
		// Get Random index from player count minus one (Like Num)
		int Rand = RANDOM_LONG(0, (Num - 1));

		// Set Team
		Players[Rand]->CSPlayer()->JoinTeam(Team);

		// Remove this entity from players count number
		Players[Rand] = Players[--Num];

		// Set next team for next random player
		Team = (Team == TERRORIST) ? CT : TERRORIST;
	}

	// If has CSGameRules
	if (g_pGameRules)
	{
		// Balance Teams to prevent incorrect count
		CSGameRules()->BalanceTeams();

		// Restart Round
		CSGameRules()->RestartRound();
	}
}

void CVoteTeam::TeamsOptimize()
{
	// Entity Array
	CBasePlayer* Players[32] = { NULL };

	// Skills Array
	std::array<float, 32> Skills = { 0 };

	// Sorted Skills Array
	std::array<float, 32> Sorted = { 0 };

	// Get players that are joined in TR or CT
	int Num = gPlayer.GetList(Players);

	// Loop
	for (int i = 0; i < Num; i++)
	{
		// Get Frags
		int Frags = Players[i]->edict()->v.frags;

		// Get Deaths
		int Death = Players[i]->m_iDeaths;

		// Store Efficiency
		Sorted[i] = Skills[i] = (Frags ? (100.0f * float(Frags) / float(Frags + Death)) : 0);
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
			// If player skill math with sorted array position
			if (Skills[j] == Sorted[i])
			{
				// Set entity team
				Players[j]->CSPlayer()->JoinTeam(Team);

				// Get the next team oposite from current team for next skilled player
				Team = (Team == TERRORIST) ? CT : TERRORIST;
			}
		}
	}

	// If has CSGameRules
	if (g_pGameRules)
	{
		// Balance Teams to prevent incorrect count
		CSGameRules()->BalanceTeams();

		// Restart Round
		CSGameRules()->RestartRound();
	}
}