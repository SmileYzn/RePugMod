#pragma once

class CVoteStop
{
public:
	void ClientDisconnected(int EntityIndex);
	bool Check(CBasePlayer* Player);
	int GetVoteCount(TeamName Team);
	void VoteStop(CBasePlayer* Player);

private:
	bool m_VoteStop[MAX_CLIENTS + 1][SPECTATOR + 1] = { 0 };
};

extern CVoteStop gVoteStop;

