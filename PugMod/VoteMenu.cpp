#include "precompiled.h"

CVoteMenu gVoteMenu;

void CVoteMenu::Load()
{
	memset(this->m_VotedMap, false, sizeof(this->m_VotedMap));

	this->m_MapList.clear();

	this->m_MapList = gUtil.LoadMapList(VOTE_MENU_MAPS_FILE, gCvars.GetVoteMapSelf()->value ? true : false);
}

void CVoteMenu::ClientDisconnected(edict_t * pEntity)
{
	auto EntityIndex = ENTINDEX(pEntity);

	memset(this->m_VotedMap[EntityIndex], false, sizeof(this->m_VotedMap[EntityIndex]));
	memset(this->m_VotedStop[EntityIndex], false, sizeof(this->m_VotedStop[EntityIndex]));
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

		gMenu[EntityIndex].AddItem(0, _T("Vote Kick"), (gPlayer.GetNum() <= 1));

		gMenu[EntityIndex].AddItem(1, _T("Vote Map"), (this->m_MapList.size() < 1));

		gMenu[EntityIndex].AddItem(2, _T("Vote Pause"), !(gPugMod.IsLive() && gCvars.GetVotePauseTime()->value));

		gMenu[EntityIndex].AddItem(3, _T("Vote Stop"), !gPugMod.IsLive());

		gMenu[EntityIndex].Show(EntityIndex);
	}
}

void CVoteMenu::MenuHandle(int EntityIndex, P_MENU_ITEM Item)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		switch (Item.Info)
		{
			case 0:
			{
				gVoteKick.Menu(Player);
				break;
			}
			case 1:
			{
				gVoteMenu.VoteMap(Player);
				break;
			}
			case 2:
			{
				gVotePause.VotePause(Player);
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

void CVoteMenu::VoteMapHandle(int EntityIndex, P_MENU_ITEM Item)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		gVoteMenu.VoteMapPickup(Player, Item.Info, Item.Disabled);
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
			gTask.Create(PUG_TASK_EXEC, 5.0f, false, (void*)SERVER_COMMAND, gUtil.VarArgs("changelevel %s\n", this->m_MapList[MapIndex].c_str()));

			gUtil.SayText(NULL, PlayerIndex, _T("Changing map to \4%s\1..."), this->m_MapList[MapIndex].c_str());
		}
	}
	else
	{
		this->VoteMap(Player);

		gUtil.SayText(Player->edict(), Player->entindex(), _T("Already nominated \3%s\1..."), this->m_MapList[MapIndex].c_str());
	}
}

void CVoteMenu::VoteStop(CBasePlayer* Player)
{
	if (this->CheckMenu(Player))
	{
		if (gPugMod.IsLive())
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
		else
		{
			gUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, _T("Unable to use this command now."));
		}
	}
}
