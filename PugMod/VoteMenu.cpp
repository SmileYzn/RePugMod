#include "precompiled.h"

CVoteMenu gVoteMenu;

void CVoteMenu::Load()
{
	memset(this->m_VoteKick, false, sizeof(this->m_VoteKick));

	memset(this->m_VotedMap, false, sizeof(this->m_VotedMap));

	memset(this->m_VotedPause, false, sizeof(this->m_VotedPause));
	
	this->m_PausedTeam = UNASSIGNED;
	this->m_PausedTime = 0;

	this->m_MapList.clear();

	this->m_MapList = gUtil.LoadMapList(VOTE_MENU_MAPS_FILE, gCvars.GetVoteMapSelf()->value ? true : false);
}

void CVoteMenu::ClientDisconnected(edict_t * pEntity)
{
	if (pEntity)
	{
		auto EntityIndex = ENTINDEX(pEntity);

		for (int i = 1; i <= gpGlobals->maxClients; ++i)
		{
			this->m_VoteKick[i][EntityIndex] = false;
		}

		memset(this->m_VoteKick[EntityIndex], false, sizeof(this->m_VoteKick[EntityIndex]));
		memset(this->m_VotedMap[EntityIndex], false, sizeof(this->m_VotedMap[EntityIndex]));
		memset(this->m_VotedPause[EntityIndex], false, sizeof(this->m_VotedPause[EntityIndex]));
		memset(this->m_VotedStop[EntityIndex], false, sizeof(this->m_VotedStop[EntityIndex]));
	}
}

bool CVoteMenu::CheckMenu(CBasePlayer* Player)
{
	if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
	{
		int State = gPugMod.GetState();

		if (State != PUG_STATE_DEAD || State != PUG_STATE_START && State != PUG_STATE_END)
		{
			if (!gTask.Exists(PUG_TASK_EXEC))
			{
				return true;
			}
		}

		gUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, _T("Unable to use this command now."));
	}

	return false;
}

void CVoteMenu::Menu(CBasePlayer* Player)
{
	if (this->CheckMenu(Player))
	{
		auto EntityIndex = Player->entindex();

		gMenu[EntityIndex].Create(_T("Vote Menu:"), true, (void*)this->MenuHandle);

		gMenu[EntityIndex].AddItem(0, _T("Vote Kick"));
		gMenu[EntityIndex].AddItem(1, _T("Vote Map"), !this->m_MapList.size());

		bool Disabled = (gPugMod.GetState() != PUG_STATE_FIRST_HALF && gPugMod.GetState() != PUG_STATE_SECOND_HALF && gPugMod.GetState() != PUG_STATE_OVERTIME);

		gMenu[EntityIndex].AddItem(2, _T("Vote Pause"), Disabled || !gCvars.GetVotePauseTime()->value);
		gMenu[EntityIndex].AddItem(3, _T("Vote Stop"), Disabled);

		gMenu[EntityIndex].Show(EntityIndex);
	}
}

void CVoteMenu::MenuHandle(int EntityIndex, int ItemIndex, bool Disabled, const char* Option)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		switch (ItemIndex)
		{
			case 0:
			{
				gVoteMenu.VoteKick(Player);
				break;
			}
			case 1:
			{
				gVoteMenu.VoteMap(Player);
				break;
			}
			case 2:
			{
				gVoteMenu.VotePause(Player);
				break;
			}
			case 3:
			{
				gVoteMenu.VoteStop(Player);
				break;
			}
		}
	}
}

void CVoteMenu::VoteKick(CBasePlayer* Player)
{
	if (this->CheckMenu(Player))
	{
		CBasePlayer* Players[MAX_CLIENTS] = { NULL };

		auto Num = gPlayer.GetList(Players,Player->m_iTeam);

		int NeedPlayers = (gCvars.GetPlayersMin()->value / 2);

		auto PlayerIndex = Player->entindex();

		if (Num >= NeedPlayers)
		{
			gMenu[PlayerIndex].Create(_T("Vote Kick"), true, (void*)this->VoteKickHandle);

			for (int i = 0; i < Num; i++)
			{
				auto Target = Players[i];

				if (Target)
				{
					auto TargetIndex = Target->entindex();

					if (PlayerIndex != TargetIndex)
					{
						if (!gAdmin.Check(Target))
						{
							gMenu[PlayerIndex].AddItem(TargetIndex, STRING(Target->edict()->v.netname), this->m_VoteKick[PlayerIndex][TargetIndex]);
						}
					}
				}
			}

			gMenu[PlayerIndex].Show(PlayerIndex);
		}
		else
		{
			gUtil.SayText(Player->edict(), PlayerIndex, _T("Need \3%d\1 players to use vote kick."), NeedPlayers);
		}
	}
}

void CVoteMenu::VoteKickHandle(int EntityIndex, int ItemIndex, bool Disabled, const char * Option)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		auto Target = UTIL_PlayerByIndexSafe(ItemIndex);

		if (Target)
		{
			gVoteMenu.VoteKickPlayer(Player, Target, Disabled);
		}
	}
}

void CVoteMenu::VoteKickPlayer(CBasePlayer* Player, CBasePlayer* Target, bool Disabled)
{
	if (Player)
	{
		if (Target)
		{
			auto TargetIndex = Target->entindex();

			if (!Disabled)
			{
				auto PlayerIndex = Player->entindex();

				this->m_VoteKick[PlayerIndex][TargetIndex] = true;

				int VoteCount = 0;

				for (int i = 1; i <= gpGlobals->maxClients; ++i)
				{
					if (this->m_VoteKick[i][Target->entindex()])
					{
						VoteCount++;
					}
				}

				int VotesNeed = (gPlayer.GetNum(Player->m_iTeam) - 1);
				int VotesLack = (VotesNeed - VoteCount);

				if (!VotesLack)
				{
					gPlayer.DropClient(Target->entindex(), _T("Kicked by Vote Kick."));

					gUtil.SayText(NULL, TargetIndex, _T("\3%s\1 Kicked: \4%d\1 votes reached."), STRING(Target->edict()->v.netname), VotesNeed);
				}
				else
				{
					gUtil.SayText(NULL, PlayerIndex, _T("\3%s\1 voted to kick \3%s\1: \4%d\1 of \4%d\1 votes to kick."), STRING(Player->edict()->v.netname), STRING(Target->edict()->v.netname), VoteCount, VotesNeed);
					gUtil.SayText(NULL, PlayerIndex, _T("Say \3.vote\1 to open vote kick."));
				}
			}
			else
			{
				gVoteMenu.VoteKick(Player);

				gUtil.SayText(Player->edict(), TargetIndex, _T("Already voted to kick: \3%s\1..."), STRING(Target->edict()->v.netname));
			}
		}
	}
}

void CVoteMenu::VoteMap(CBasePlayer* Player)
{
	if (this->CheckMenu(Player))
	{
		if (this->m_MapList.size())
		{
			auto PlayerIndex = Player->entindex();

			gMenu[PlayerIndex].Create(_T("Vote Map"), true, (void*)this->VoteMapHandle);

			for (std::size_t MapIndex = 0; MapIndex < this->m_MapList.size(); MapIndex++)
			{
				gMenu[PlayerIndex].AddItem(MapIndex, this->m_MapList[MapIndex], this->m_VotedMap[PlayerIndex][MapIndex]);
			}

			gMenu[PlayerIndex].Show(PlayerIndex);
		}
	}
}

void CVoteMenu::VoteMapHandle(int EntityIndex, int ItemIndex, bool Disabled, const char * Option)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		gVoteMenu.VoteMapPickup(Player, ItemIndex, Disabled);
	}
}

void CVoteMenu::VoteMapPickup(CBasePlayer* Player, int MapIndex, bool Disabled)
{
	if (!Disabled)
	{
		auto PlayerIndex = Player->entindex();

		this->m_VotedMap[PlayerIndex][MapIndex] = true;

		int VoteCount = 0;

		for (int i = 1; i <= gpGlobals->maxClients; ++i)
		{
			if (this->m_VotedMap[i][MapIndex])
			{
				VoteCount++;
			}
		}

		int VotesNeed = (int)(gCvars.GetPlayersMin()->value * gCvars.GetVotePercentage()->value);
		int VotesLack = (VotesNeed - VoteCount);

		if (VotesLack)
		{
			gUtil.SayText(NULL, PlayerIndex, _T("\3%s\1 nomitated \4%s\1: \4%d\1 of \4%d\1 votes to change map."), STRING(Player->edict()->v.netname), this->m_MapList[MapIndex].c_str(), VoteCount, VotesLack);
			gUtil.SayText(NULL, PlayerIndex, _T("Say \3.vote\1 to nominate a map."));
		}
		else
		{
			gTask.Create(PUG_TASK_EXEC, 5.0f, false, SERVER_COMMAND, gUtil.VarArgs("changelevel %s\n", this->m_MapList[MapIndex].c_str()));

			gUtil.SayText(NULL, PlayerIndex, _T("Changing map to \4%s\1..."), this->m_MapList[MapIndex].c_str());
		}
	}
	else
	{
		this->VoteMap(Player);

		gUtil.SayText(Player->edict(), Player->entindex(), _T("Already nominated \3%s\1..."), this->m_MapList[MapIndex].c_str());
	}
}

void CVoteMenu::Changelevel(char* MapName)
{
	if (MapName)
	{
		SERVER_COMMAND(gUtil.VarArgs("changelevel %s\n", MapName));
	}
}

void CVoteMenu::VotePause(CBasePlayer* Player)
{
	if (this->CheckMenu(Player))
	{
		if (gPugMod.GetState() == PUG_STATE_FIRST_HALF || gPugMod.GetState() == PUG_STATE_SECOND_HALF || gPugMod.GetState() == PUG_STATE_OVERTIME)
		{
			if (gCvars.GetVotePauseTime()->value)
			{
				if (this->m_PausedTeam == UNASSIGNED)
				{
					auto PlayerIndex = Player->entindex();

					if (!this->m_VotedPause[PlayerIndex][Player->m_iTeam])
					{
						this->m_VotedPause[PlayerIndex][Player->m_iTeam] = true;

						int VoteCount = 0;

						for (int i = 1; i <= gpGlobals->maxClients; ++i)
						{
							if (this->m_VotedPause[i][Player->m_iTeam])
							{
								VoteCount++;
							}
						}

						int VotesNeed = (int)(gPlayer.GetNum(Player->m_iTeam) * gCvars.GetVotePercentage()->value);
						int VotesLack = (VotesNeed - VoteCount);

						if (VotesLack)
						{
							gUtil.SayText(NULL, PlayerIndex, _T("\3%s\1 from voted for a timeout: \4%d\1 of \4%d\1 vote(s) to run timeout."), STRING(Player->edict()->v.netname), VoteCount, VotesNeed);
							gUtil.SayText(NULL, PlayerIndex, _T("Say \3.vote\1 for a timeout."));
						}
						else
						{
							this->m_PausedTeam = Player->m_iTeam;
							this->m_PausedTime = (int)gCvars.GetVotePauseTime()->value;

							gUtil.SayText(NULL, PlayerIndex, _T("Match will pause for \4%d\1 seconds on next round."), this->m_PausedTime);
						}
					}
					else
					{
						gUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, _T("You already voted for a timeout."));
					}
				}
				else
				{
					gUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, _T("The \3%s\1 team already paused the game."), PUG_MOD_TEAM_STR[this->m_PausedTeam]);
				}
			}
			else
			{
				gUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, _T("Unable to vote to pause the match."));
			}
		}
		else
		{
			gUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, _T("Unable to use this command now."));
		}
	}
}

void CVoteMenu::RoundRestart()
{
	if (gPugMod.GetState() == PUG_STATE_FIRST_HALF || gPugMod.GetState() == PUG_STATE_SECOND_HALF || gPugMod.GetState() == PUG_STATE_OVERTIME)
	{
		if (this->m_PausedTime > 0)
		{
			if (this->m_PausedTeam != UNASSIGNED)
			{
				if (g_pGameRules)
				{
					if (!CSGameRules()->m_bCompleteReset)
					{
						CSGameRules()->m_bFreezePeriod   = true;
						CSGameRules()->m_iIntroRoundTime += this->m_PausedTime;
						CSGameRules()->m_iRoundTimeSecs  += this->m_PausedTime;
						CSGameRules()->m_fRoundStartTime = gpGlobals->time;

						static int iMsgRoundTime;

						if (iMsgRoundTime || (iMsgRoundTime = GET_USER_MSG_ID(PLID, "RoundTime", NULL)))
						{
							MESSAGE_BEGIN(MSG_ALL, iMsgRoundTime);
							WRITE_SHORT(CSGameRules()->GetRoundRemainingTimeReal());
							MESSAGE_END();
						}
					}
				}
			}

			this->m_PausedTeam = UNASSIGNED;
			this->m_PausedTime = 0; 
		}
	}
}

void CVoteMenu::VoteStop(CBasePlayer* Player)
{
	if (this->CheckMenu(Player))
	{
		auto PlayerIndex = Player->entindex();

		if (!this->m_VotedStop[PlayerIndex][Player->m_iTeam])
		{
			this->m_VotedStop[PlayerIndex][Player->m_iTeam] = true;

			int VoteCount = 0;

			for (int i = 1; i <= gpGlobals->maxClients; ++i)
			{
				if (this->m_VotedStop[i][Player->m_iTeam])
				{
					VoteCount++;
				}
			}

			int VotesNeed = (int)(gPlayer.GetNum(Player->m_iTeam) * gCvars.GetVotePercentage()->value);
			int VotesLack = (VotesNeed - VoteCount);
			
			if (VotesLack)
			{
				gUtil.SayText(NULL, PlayerIndex, _T("\3%s\1 voted for surrender: \4%d\1 of \4%d\1 vote(s) to stop the match."), STRING(Player->edict()->v.netname), VoteCount, VotesNeed);
				gUtil.SayText(NULL, PlayerIndex, _T("Say \3.vote\1 to vote for stop the match."));
			}
			else
			{
				gUtil.SayText(NULL, PlayerIndex, _T("Game Over! The \3%s\1 Surrendered!"), PUG_MOD_TEAM_STR[Player->m_iTeam]);

				gPugMod.EndGame(Player->m_iTeam == TERRORIST ? CT : TERRORIST); 
			}
		}
		else
		{
			gUtil.SayText(Player->edict(), PlayerIndex, _T("You already voted to stop the match."));
		}
	}
}
