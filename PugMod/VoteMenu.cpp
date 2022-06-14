#include "precompiled.h"

CVoteMenu gVoteMenu;

void CVoteMenu::Menu(CBasePlayer* Player)
{
	if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
	{
		auto EntityIndex = Player->entindex();

		gMenu[EntityIndex].Create(_T("Vote Menu:"), true, (void*)this->MenuHandle);

		gMenu[EntityIndex].AddItem(0, _T("Vote Kick"), !gVoteKick.Check(Player));

		gMenu[EntityIndex].AddItem(1, _T("Vote Map"), !gVoteLevel.Check(Player));

		gMenu[EntityIndex].AddItem(2, _T("Vote Restart"), !gVoteRestart.Check(Player));

		gMenu[EntityIndex].AddItem(3, _T("Vote Pause"), !gVotePause.Check(Player));

		gMenu[EntityIndex].AddItem(4, _T("Vote Stop"), !gVoteStop.Check(Player));

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
				gVoteLevel.Menu(Player);
				break;
			}
			case 2:
			{
				gVoteRestart.VoteRestart(Player);
				break;
			}
			case 3:
			{
				gVotePause.VotePause(Player);
				break;
			}
			case 4:
			{
				gVoteStop.VoteStop(Player);
				break;
			}
		}
	}
}
