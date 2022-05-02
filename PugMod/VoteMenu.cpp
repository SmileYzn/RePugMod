#include "precompiled.h"

CVoteMenu gVoteMenu;

void CVoteMenu::ClientGetIntoGame(CBasePlayer* Player)
{
	int EntityIndex = Player->entindex();

	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		this->m_VoteKick[i][EntityIndex] = false;
		this->m_VoteKick[EntityIndex][i] = false;
	}
}

bool CVoteMenu::CheckMenu(CBasePlayer* Player)
{
	if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
	{
		int State = gPugMod.GetState();

		if (State != PUG_STATE_DEAD || State != PUG_STATE_START && State != PUG_STATE_END)
		{
			return true;
		}

		gUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, "Unable to use this command now.");
	}

	return false;
}

void CVoteMenu::Menu(CBasePlayer* Player)
{
	if (this->CheckMenu(Player))
	{
		int EntityIndex = Player->entindex();

		gMenu[EntityIndex].Create("Vote Menu:", true, this->MenuHandle);

		gMenu[EntityIndex].AddItem(0, "Vote Kick");
		gMenu[EntityIndex].AddItem(1, "Vote Map");

		if (gPugMod.GetState() >= PUG_STATE_FIRST_HALF && gPugMod.GetState() <= PUG_STATE_OVERTIME)
		{
			gMenu[EntityIndex].AddItem(2, "Vote Pause");
			gMenu[EntityIndex].AddItem(3, "Vote Stop");
		}

		gMenu[EntityIndex].Show(EntityIndex);
	}
}

void CVoteMenu::MenuHandle(int EntityIndex, int ItemIndex, const char* Option)
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
		CBasePlayer* Players[32] = { NULL };

		int Num = gPlayer.GetList(Players,Player->m_iTeam);

		int NeedPlayers = (gCvars.GetPlayersMin()->value / 2);

		int EntityIndex = Player->entindex();

		if (Num >= NeedPlayers)
		{
			gMenu[EntityIndex].Create("Vote Kick:", true, this->MenuHandle);

			for (int i = 0;i < Num;i++)
			{
				if (EntityIndex != Players[i]->entindex())
				{
					if (!gAdmin.Check(Player->edict()))
					{
						gMenu[EntityIndex].AddItem(Players[i]->entindex(), STRING(Players[i]->edict()->v.netname));
					}
				}
			}

			gMenu[EntityIndex].Show(EntityIndex);

			gUtil.SayText(Player->edict(), EntityIndex, "Choose a player to vote."); 
		}
		else
		{
			gUtil.SayText(Player->edict(), EntityIndex, "Need \3%d\1 players to use vote kick.", NeedPlayers);
		}
	}
}

void CVoteMenu::VoteKick(CBasePlayer* Player, CBasePlayer* Target)
{
	this->m_VoteKick[Player->entindex()][Target->entindex()] = true;

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
		gUtil.ServerCommand("kick #%d \"Kicked by Vote Kick.\"", GETPLAYERUSERID(Target->edict()));

		gUtil.SayText(NULL, Target->entindex(), "\3%s\1 Kicked: \4%d\1 votes reached.", STRING(Target->edict()->v.netname), VotesNeed);
	}
	else
	{
		gUtil.SayText(NULL, Player->entindex(), "\3%s\1 voted to kick %s: Need more \4%d\1 votes to kick.", STRING(Target->edict()->v.netname), VotesNeed);
	}
}

void CVoteMenu::VoteKickHandle(int EntityIndex, int ItemIndex, const char * Option)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		auto Target = UTIL_PlayerByIndexSafe(ItemIndex);

		if (Target)
		{
			gVoteMenu.VoteKick(Player, Target);
		}
	}
}

void CVoteMenu::VoteMap(CBasePlayer* Player)
{
	if (this->CheckMenu(Player))
	{
		// TODO: Vote  Map Menu
	}
}

void CVoteMenu::VoteMapHandle(int EntityIndex, int ItemIndex, const char * Option)
{
	// TODO: Vote  Map Menu Handle
}

void CVoteMenu::VotePause(CBasePlayer* Player)
{
	if (this->CheckMenu(Player))
	{
		//
	}
}

void CVoteMenu::VotePauseHandle(int EntityIndex, int ItemIndex, const char * Option)
{
}

void CVoteMenu::VoteStop(CBasePlayer* Player)
{
	if (this->CheckMenu(Player))
	{
		//
	}
}

void CVoteMenu::VoteStopHandle(int EntityIndex, int ItemIndex, const char * Option)
{
}
