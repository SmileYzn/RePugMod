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

		if (VoteChangeTeam) // Swap Teams not working!
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

void CKnifeRound::RoundStart()
{
	if (this->m_Running)
	{
		if (this->m_Winner != UNASSIGNED)
		{
			CBasePlayer* Players[32] = { NULL };

			int Num = gPlayer.GetList(Players, this->m_Winner);

			for (int i = 0; i < Num; i++)
			{
				auto Player = Players[i];

				if (Player)
				{
					if (!Player->IsBot())
					{
						auto EntityIndex = Player->entindex();

						gMenu[EntityIndex].Create("Select Starting Side:", false, this->MenuHandle);

						gMenu[EntityIndex].AddItem(1, "Terrorists");
						gMenu[EntityIndex].AddItem(2, "Counter-Terrorists");

						gMenu[EntityIndex].Show(EntityIndex);
					}
					else
					{
						auto Team = Player->m_iTeam == TERRORIST ? CT : TERRORIST;

						this->MenuHandle(Player->entindex(), (int)Team, false, PUG_MOD_TEAM_STR[Team]);
					}
				}
			}

			gTask.Create(PUG_TASK_VOTE, gCvars.GetVoteDelay()->value, false, this->VoteEnd);
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

				gUtil.SayText(NULL, PRINT_TEAM_RED, "\3%s\1 Won: The \3%s\1 team will decide the starting side.", PUG_MOD_TEAM_STR[this->m_Winner], PUG_MOD_TEAM_STR[this->m_Winner]);
			}
			else if (winStatus == WINSTATUS_CTS && event == ROUND_CTS_WIN)
			{
				this->m_Winner = CT;

				gUtil.SayText(NULL, PRINT_TEAM_BLUE, "\3%s\1 Won: The \3%s\1 team will decide the starting side.", PUG_MOD_TEAM_STR[this->m_Winner], PUG_MOD_TEAM_STR[this->m_Winner]);
			}
			else
			{
				this->m_Winner = UNASSIGNED;

				gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, "Knife Round Failed: \3No clear winner by extermination.");

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
		gKnifeRound.SetVote((TeamName)ItemIndex, 1);

		gUtil.SayText(NULL, Player->entindex(), "\3%s\1 choosed \3%s\1.", STRING(Player->edict()->v.netname), Option);

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