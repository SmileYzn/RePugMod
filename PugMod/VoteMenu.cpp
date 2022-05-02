#include "precompiled.h"

CVoteMenu gVoteMenu;

bool CVoteMenu::CheckMenu(CBasePlayer* Player)
{
	if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
	{
		int State = gPugMod.GetState();

		if (State != PUG_STATE_DEAD || State != PUG_STATE_START && State != PUG_STATE_END)
		{
			return true;
		}
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
	else
	{
		gUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, "Unable to use this command now.");
	}
}

void CVoteMenu::MenuHandle(int EntityIndex, int ItemIndex, const char* Option)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		if (gVoteMenu.CheckMenu(Player))
		{

		}
	}
}