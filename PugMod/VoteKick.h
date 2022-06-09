#pragma once

class CVoteKick
{
public:
	void		ClientDisconnected(edict_t* pEntity);
	bool		CheckMenu(CBasePlayer* Player);
	void		VoteKick(CBasePlayer* Player);
	static void VoteKickHandle(int EntityIndex, P_MENU_ITEM Item);
	void		VoteKickPlayer(CBasePlayer* Player, CBasePlayer* Target, bool Disabled);

private:
	bool m_VoteKick[MAX_CLIENTS + 1][MAX_CLIENTS + 1] = { false };
};

extern CVoteKick gVoteKick;