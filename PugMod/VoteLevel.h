#pragma once

class CVoteLevel
{
public:
	void		ClientDisconnected(int EntityIndex);
	bool		Check(CBasePlayer* Player);
	void		Menu(CBasePlayer* Player);
	static void MenuHandle(int EntityIndex, P_MENU_ITEM Item);
	int			GetVoteCount(int MapIndex);
	void		VoteLevel(CBasePlayer* Player, int MapIndex, bool Disabled);

private:
	bool m_Votes[MAX_CLIENTS + 1][33] = { false };
	std::vector<std::string> m_Maps;
};

extern CVoteLevel gVoteLevel;