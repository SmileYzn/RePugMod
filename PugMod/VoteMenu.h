#pragma once

class CVoteMenu
{
public:
	bool CheckMenu(CBasePlayer* Player);

	void Menu(CBasePlayer* Player);
	static void MenuHandle(int EntityIndex, int ItemIndex, const char* Option);
};

extern CVoteMenu gVoteMenu;