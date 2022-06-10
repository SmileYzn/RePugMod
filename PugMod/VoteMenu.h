#pragma once

#define VOTE_MENU_MAPS_FILE "cstrike/addons/pugmod/maps.txt"

class CVoteMenu
{
public:
	void Menu(CBasePlayer* Player);
	static void MenuHandle(int EntityIndex, P_MENU_ITEM Item);
};

extern CVoteMenu gVoteMenu;
