#include "precompiled.h"

CVoteOvertime gVoteOvertime;

void CVoteOvertime::Init()
{
	this->m_Data.clear();

	this->m_Data.insert(std::make_pair(0, P_VOTE_OT_ITEM(0, 0, _T("Play overtime"))));
	this->m_Data.insert(std::make_pair(1, P_VOTE_OT_ITEM(1, 0, _T("Play sudden death round"))));
	this->m_Data.insert(std::make_pair(2, P_VOTE_OT_ITEM(2, 0, _T("End match with tied scores"))));

	CBasePlayer* Players[MAX_CLIENTS] = { NULL };

	auto Num = gPlayer.GetList(Players, true);

	for (int i = 0; i < Num; i++)
	{
		auto Player = Players[i];

		if (Player)
		{
			auto EntityIndex = Player->entindex();

			gMenu[EntityIndex].Create(_T("Match is tied, what you want to do?"), false, (void*)this->MenuHandle);

			for (auto const& Item : this->m_Data)
			{
				gMenu[EntityIndex].AddItem(Item.first, Item.second.Name);
			}

			gMenu[EntityIndex].Show(EntityIndex);
		}
	}

	if (g_pGameRules)
	{
		if (CSGameRules()->m_bRoundTerminating)
		{
			CSGameRules()->m_flRestartRoundTime = (gpGlobals->time + gCvars.GetVoteDelay()->value);
		}
	}

	gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, _T("Overtime vote started."));

	gTask.Create(PUG_TASK_LIST, 0.5f, true, (void*)this->List);

	gTask.Create(PUG_TASK_VOTE, gCvars.GetVoteDelay()->value, false, (void*)this->Stop);
}

void CVoteOvertime::AddVote(int Item, int Vote)
{
	auto it = this->m_Data.find(Item);

	if (it != this->m_Data.end())
	{
		it->second.Votes += Vote;
	}

	this->List();
}

auto CVoteOvertime::GetVote()
{
	return this->m_Data;
}

void CVoteOvertime::MenuHandle(int EntityIndex, P_MENU_ITEM Item)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		gVoteOvertime.AddVote(Item.Info, 1);

		gUtil.SayText(NULL, Player->entindex(), _T("\3%s\1 choosed \3%s\1"), STRING(Player->edict()->v.netname), Item.Text.c_str());

		if (gVoteOvertime.GetCount() >= gPlayer.GetNum(false))
		{
			gVoteOvertime.Stop();
		}
	}
}

void CVoteOvertime::Stop()
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

	auto Winner = gVoteOvertime.GetWinner();

	if (Winner.Votes > 0)
	{
		switch (Winner.Index)
		{
			case 0:
			{
				gPugMod.SetState(PUG_STATE_OVERTIME);
				break;
			}
			case 1:
			{
				gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, _T("Match will continue in: \3Sudden Death Round."));
				break;
			}
			case 2:
			{
				gPugMod.SetState(PUG_STATE_END);
				break;
			}
		}
	}
	else
	{
		gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, _T("The choice of overtime failed: \3No votes."));
	}
}

void CVoteOvertime::List()
{
	std::string VoteList;

	for (auto const& Item : gVoteOvertime.GetVote())
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

	gUtil.HudMessage(NULL, gUtil.HudParam(0, 255, 0, 0.23, 0.02, 0, 0.0, 0.53, 0.0, 0.0, 1), _T("Play Overtime (%d):"), (int)gTask.Timeleft(PUG_TASK_VOTE));

	gUtil.HudMessage(NULL, gUtil.HudParam(255, 255, 225, 0.23, 0.02, 0, 0.0, 0.53, 0.0, 0.0, 2), "\n%s", VoteList.length() ? VoteList.c_str() : _T("No Votes"));
}

int CVoteOvertime::GetCount()
{
	int Count = 0;

	for (auto const& Item : this->m_Data)
	{
		Count += Item.second.Votes;
	}

	return Count;
}

P_VOTE_OT_ITEM CVoteOvertime::GetWinner()
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