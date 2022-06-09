#include "precompiled.h"

CVoteKick gVoteKick;

void CVoteKick::ClientDisconnected(edict_t* pEntity)
{
	if (!FNullEnt(pEntity))
	{
		auto EntityIndex = ENTINDEX(pEntity);

		for (int i = 1; i <= gpGlobals->maxClients; ++i)
		{
			this->m_VoteKick[i][EntityIndex] = false;
			this->m_VoteKick[EntityIndex][i] = false;
		}
	}
}

bool CVoteKick::CheckMenu(CBasePlayer* Player)
{
	if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
	{
		int State = gPugMod.GetState();

		if (State >= PUG_STATE_WARMUP && State <= PUG_STATE_END)
		{
			if (State != PUG_STATE_START)
			{
				return !gTask.Exists(PUG_TASK_EXEC);
			}
		}

		gUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, _T("Unable to use this command now."));
	}

	return false;
}

void CVoteKick::VoteKick(CBasePlayer* Player)
{
	if (this->CheckMenu(Player))
	{
		CBasePlayer* Players[MAX_CLIENTS] = { NULL };

		auto Num = gPlayer.GetList(Players, Player->m_iTeam);

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

void CVoteKick::VoteKickHandle(int EntityIndex, P_MENU_ITEM Item)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		auto Target = UTIL_PlayerByIndexSafe(Item.Info);

		if (Target)
		{
			gVoteKick.VoteKickPlayer(Player, Target, Item.Disabled);
		}
	}
}

void CVoteKick::VoteKickPlayer(CBasePlayer* Player, CBasePlayer* Target, bool Disabled)
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
				gVoteKick.VoteKick(Player);

				gUtil.SayText(Player->edict(), TargetIndex, _T("Already voted to kick: \3%s\1..."), STRING(Target->edict()->v.netname));
			}
		}
	}
}