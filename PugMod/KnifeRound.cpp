#include "precompiled.h"

CKnifeRound gKnifeRound;

void CKnifeRound::Init(bool ForceKnifeRound)
{
	if (gCvars.GetPlayKnifeRound()->value == 1 || ForceKnifeRound) 
	{
		this->m_Running = true;

		this->m_Winner = UNASSIGNED;

		memset(this->m_Votes, 0, sizeof(this->m_Votes));

		CVAR_SET_STRING("sv_restart", "3");

		gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, _T("Knife Round Starting: \4Get Ready!!"));
	}
	else
	{
		gPugMod.NextState(3.0f);
	}
}

void CKnifeRound::Stop(bool ChangeTeams)
{
	this->m_Running = false;

	if (ChangeTeams)
	{
		if ((this->GetVote(this->GetWinner()) < (gPlayer.GetNum(this->GetWinner()) / 2)))
		{
			if (g_pGameRules)
			{
				CSGameRules()->SwapAllPlayers();
			}

			gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, _T("Changing teams automatically."));
		}
		else
		{
			gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, _T("Teams will remain unchanged."));
		}
	}

	gPugMod.RestarPeriod(NULL);

	gPugMod.NextState(3.0f);
}

bool CKnifeRound::ClientHasRestrictItem(CBasePlayer* Player, ItemID item, ItemRestType type)
{
	if (this->m_Running)
	{
		if (item != ITEM_KEVLAR && item != ITEM_KNIFE)
		{
			if (type == ITEM_TYPE_BUYING)
			{
				gUtil.ClientPrint(Player->edict(), PRINT_CENTER, "#Cstrike_TitlesTXT_Weapon_Not_Available");
			}

			return true;
		}
	}

	return false;
}

bool CKnifeRound::GiveC4()
{
	return this->m_Running;
}

void CKnifeRound::RoundRestart()
{
	if (this->m_Running)
	{
		if (g_pGameRules)
		{
			if (CSGameRules()->m_bCompleteReset == false)
			{
				if (this->m_Winner != UNASSIGNED)
				{
					CBasePlayer* Players[MAX_CLIENTS] = { NULL };

					int Num = gPlayer.GetList(Players, this->m_Winner);

					for (int i = 0; i < Num; i++)
					{
						auto Player = Players[i];

						if (Player)
						{
							if (!Player->IsBot())
							{
								auto EntityIndex = Player->entindex();

								gMenu[EntityIndex].Create(_T("Select Starting Side:"), false, (void*)this->MenuHandle);

								gMenu[EntityIndex].AddItem(1, _T("Terrorists"));
								gMenu[EntityIndex].AddItem(2, _T("Counter-Terrorists"));

								gMenu[EntityIndex].Show(EntityIndex);
							}
							else
							{
								this->AddVote((Player->m_iTeam == TERRORIST ? CT : TERRORIST));
							}
						}
					}

					if ((gKnifeRound.GetVote(TERRORIST) + gKnifeRound.GetVote(CT)) >= gPlayer.GetNum(gKnifeRound.GetWinner()))
					{
						gKnifeRound.VoteEnd();
					}
					else
					{
						gTask.Create(PUG_TASK_VOTE, gCvars.GetVoteDelay()->value, false, (void*)this->VoteEnd);

						gTask.Create(PUG_TASK_LIST, 0.5f, true, (void*)this->List);
					}
				}
			}
		}
	}
}

void CKnifeRound::RoundEnd(int winStatus, ScenarioEventEndRound event, float tmDelay)
{
	if (this->m_Running)
	{
		if (winStatus != WINSTATUS_NONE)
		{
			if (winStatus == WINSTATUS_TERRORISTS && event == ROUND_TERRORISTS_WIN)
			{
				this->m_Winner = TERRORIST;

				gUtil.SayText(NULL, PRINT_TEAM_RED, _T("\3%s\1 Won: The \3%s\1 team will decide the starting side."), PUG_MOD_TEAM_STR[this->m_Winner], PUG_MOD_TEAM_STR[this->m_Winner]);
			}
			else if (winStatus == WINSTATUS_CTS && event == ROUND_CTS_WIN)
			{
				this->m_Winner = CT;

				gUtil.SayText(NULL, PRINT_TEAM_BLUE, _T("\3%s\1 Won: The \3%s\1 team will decide the starting side."), PUG_MOD_TEAM_STR[this->m_Winner], PUG_MOD_TEAM_STR[this->m_Winner]);
			}
			else
			{
				this->m_Winner = UNASSIGNED;

				gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, _T("Knife Round Failed: \3No clear winner by extermination."));

				if (!gCvars.GetKnifeRoundEndType()->value)
				{
					this->Stop(false);
				}
				else
				{
					this->Init();

					gPugMod.RestarPeriod(NULL);
				}
			}
		}
	}
}

void CKnifeRound::List()
{
	std::string VoteList;

	if (gKnifeRound.GetVote(TERRORIST))
	{
		VoteList += PUG_MOD_TEAM_STR[TERRORIST];
		VoteList += " [";
		VoteList += std::to_string(gKnifeRound.GetVote(TERRORIST));
		VoteList += "]\n";
	}

	if (gKnifeRound.GetVote(CT))
	{
		VoteList += PUG_MOD_TEAM_STR[CT];
		VoteList += " [";
		VoteList += std::to_string(gKnifeRound.GetVote(CT));
		VoteList += "]\n";
	}

	gUtil.HudMessage(NULL, gUtil.HudParam(0, 255, 0, 0.23, 0.02, 0, 0.0, 0.53, 0.0, 0.0, 1), _T("Starting Side (%d):"), (int)gTask.Timeleft(PUG_TASK_VOTE));

	gUtil.HudMessage(NULL, gUtil.HudParam(255, 255, 225, 0.23, 0.02, 0, 0.0, 0.53, 0.0, 0.0, 2), "\n%s", VoteList.length() ? VoteList.c_str() : _T("No votes."));
}

TeamName CKnifeRound::GetWinner()
{
	return this->m_Winner;
}

int CKnifeRound::AddVote(TeamName Team)
{
	this->m_Votes[Team]++;

	return this->m_Votes[Team];
}

int CKnifeRound::GetVote(TeamName Team)
{
	return this->m_Votes[Team];
}

void CKnifeRound::MenuHandle(int EntityIndex, P_MENU_ITEM Item)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		gKnifeRound.AddVote((TeamName)Item.Info);

		gUtil.SayText(NULL, Player->entindex(), _T("\3%s\1 choosed \3%s\1"), STRING(Player->edict()->v.netname), Item.Text.c_str());

		if ((gKnifeRound.GetVote(TERRORIST) + gKnifeRound.GetVote(CT)) >= gPlayer.GetNum(gKnifeRound.GetWinner()))
		{
			gKnifeRound.VoteEnd();
		}
	}
}

void CKnifeRound::VoteEnd()
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

	gKnifeRound.Stop(true);
}