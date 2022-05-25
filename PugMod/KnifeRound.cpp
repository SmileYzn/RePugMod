#include "precompiled.h"

CKnifeRound gKnifeRound;

void CKnifeRound::Init()
{
	this->m_Running = true;

	this->m_Winner = UNASSIGNED;

	memset(this->m_Votes, 0, sizeof(this->m_Votes));

	CVAR_SET_STRING("mp_give_player_c4", "0");

	gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, "Knife Round Starting: \4Get Ready!!");
}

void CKnifeRound::Stop(bool ChangeTeams)
{
	this->m_Running = false;

	CVAR_SET_STRING("mp_give_player_c4", "1");

	if (ChangeTeams)
	{	
		auto VoteChangeTeam = (this->GetVote() > (gPlayer.GetNum(this->m_Winner) / 2));

		if (VoteChangeTeam) 
		{
			if (g_pGameRules)
			{
				CSGameRules()->SwapAllPlayers();
			}

			gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, "Changing teams automatically.");
		}
		else
		{
			gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, "Teams will remain unchanged.");
		}
	}

	gPugMod.RestarPeriod(NULL);
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
								auto Team = Player->m_iTeam == TERRORIST ? CT : TERRORIST;

								this->MenuHandle(Player->entindex(), (int)Team, false, PUG_MOD_TEAM_STR[Team]);
							}
						}
					}

					gTask.Create(PUG_TASK_VOTE, gCvars.GetVoteDelay()->value, false, (void*)this->VoteEnd);

					gTask.Create(PUG_TASK_LIST, 0.5f, true, (void*)this->List, this);
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

void CKnifeRound::List(CKnifeRound* KnifeRound)
{
	char VoteList[128] = { 0 };

	for (int Team = TERRORIST;Team <= CT;Team++)
	{
		if (KnifeRound->m_Votes[Team])
		{
			snprintf(VoteList, sizeof(VoteList), "%s%s [%d]\n", VoteList, PUG_MOD_TEAM_STR[Team], KnifeRound->m_Votes[Team]);
		}
	}

	gUtil.HudMessage(NULL, gUtil.HudParam(0, 255, 0, 0.23, 0.02, 0, 0.0, 0.53, 0.0, 0.0, 1), _T("Starting Side (%d):"), (int)gTask.Timeleft(PUG_TASK_VOTE));

	gUtil.HudMessage(NULL, gUtil.HudParam(255, 255, 225, 0.23, 0.02, 0, 0.0, 0.53, 0.0, 0.0, 2), "\n%s", strlen(VoteList) ? VoteList : _T("No votes."));
}

int CKnifeRound::SetVote(TeamName Team, int Vote)
{
	this->m_Votes[Team] += Vote;

	return this->m_Votes[Team];
}

int CKnifeRound::GetVote()
{
	return this->m_Votes[this->m_Winner];
}

int CKnifeRound::GetCount()
{
	return (this->m_Votes[TERRORIST] + this->m_Votes[CT]);
}

void CKnifeRound::MenuHandle(int EntityIndex, int ItemIndex, bool Disabled, const char* Option)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		gKnifeRound.SetVote(Player->m_iTeam, 1);

		gUtil.SayText(NULL, Player->entindex(), _T("\3%s\1 choosed \3%s\1"), STRING(Player->edict()->v.netname), Option);

		if (gKnifeRound.GetCount() >= gPlayer.GetNum(true))
		{
			gKnifeRound.VoteEnd();
		}
	}
}

void CKnifeRound::VoteEnd()
{
	MENU_RESET_ALL();

	gTask.Remove(PUG_TASK_LIST);

	gTask.Remove(PUG_TASK_VOTE);

	gKnifeRound.Stop(true);
}