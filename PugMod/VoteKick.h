#pragma once

class CVoteKick
{
public:
	void		ClientDisconnected(edict_t* pEntity);
	bool		Check(CBasePlayer* Player);
	void		Menu(CBasePlayer* Player);
	static void MenuHandle(int EntityIndex, P_MENU_ITEM Item);
	void		VoteKick(CBasePlayer* Player, CBasePlayer* Target);

private:
	bool m_VoteKick[MAX_CLIENTS + 1][MAX_CLIENTS + 1] = { false };
};

extern CVoteKick gVoteKick;