#pragma once

class CVoteKick
{
public:
	void		ClientDisconnected(int EntityIndex);
	bool		Check(CBasePlayer* Player);
	void		Menu(CBasePlayer* Player);
	static void MenuHandle(int EntityIndex, P_MENU_ITEM Item);
	int			GetVoteCount(int EntityIndex);
	void		VoteKick(CBasePlayer* Player, CBasePlayer* Target);

private:
	bool m_VoteKick[MAX_CLIENTS + 1][MAX_CLIENTS + 1] = { false };
};

extern CVoteKick gVoteKick;