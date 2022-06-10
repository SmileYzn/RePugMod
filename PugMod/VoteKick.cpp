#include "precompiled.h"

CVoteKick gVoteKick;

void CVoteKick::ClientDisconnected(int EntityIndex)
{
	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		this->m_VoteKick[i][EntityIndex] = false;
		this->m_VoteKick[EntityIndex][i] = false;
	}
}

bool CVoteKick::Check(CBasePlayer* Player)
{
	if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
	{
		if (gPugMod.GetState() > PUG_STATE_DEAD)
		{
			if (!gTask.Exists(PUG_TASK_EXEC) && !gTask.Exists(PUG_TASK_VOTE))
			{
				if (gPlayer.GetNum() > (gCvars.GetPlayersMin()->value / 2))
				{
					return true;
				}
			}
		}
	}

	return false;
}

void CVoteKick::Menu(CBasePlayer* Player)
{
	if (this->Check(Player))
	{
		CBasePlayer* Players[MAX_CLIENTS] = { NULL };

		auto Num = gPlayer.GetList(Players, Player->m_iTeam);

		int NeedPlayers = (gCvars.GetPlayersMin()->value / 2);

		auto PlayerIndex = Player->entindex();

		if (Num >= NeedPlayers)
		{
			gMenu[PlayerIndex].Create(_T("Vote Kick"), true, (void*)this->MenuHandle);

			char Text[128] = { 0 };

			int VotesNeed = (gPlayer.GetNum(Player->m_iTeam) - 1);

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
							snprintf(Text, sizeof(Text), "%s \\y\\R%2.0f%%", STRING(Target->edict()->v.netname),(float)((this->GetVoteCount(TargetIndex)*100)/VotesNeed));

							gMenu[PlayerIndex].AddItem(TargetIndex, Text, this->m_VoteKick[PlayerIndex][TargetIndex]);
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
	else
	{
		gUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, _T("Unable to use this command now."));
	}
}

void CVoteKick::MenuHandle(int EntityIndex, P_MENU_ITEM Item)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		auto Target = UTIL_PlayerByIndexSafe(Item.Info);

		if (Target)
		{
			gVoteKick.VoteKick(Player, Target);
		}
	}
}

int CVoteKick::GetVoteCount(int EntityIndex)
{
	int VoteCount = 0;

	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		if (this->m_VoteKick[i][EntityIndex])
		{
			VoteCount++;
		}
	}

	return VoteCount;
}

void CVoteKick::VoteKick(CBasePlayer* Player, CBasePlayer* Target)
{
	if (Player)
	{
		if (Target)
		{
			auto TargetIndex = Target->entindex();
			auto PlayerIndex = Player->entindex();

			if (!this->m_VoteKick[PlayerIndex][TargetIndex])
			{
				this->m_VoteKick[PlayerIndex][TargetIndex] = true;

				int VoteCount = this->GetVoteCount(TargetIndex);
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
				gUtil.SayText(Player->edict(), TargetIndex, _T("Already voted to kick: \3%s\1..."), STRING(Target->edict()->v.netname));
			}
		}
	}
}