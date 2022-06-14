#pragma once

class CVoteRestart
{
public:
	void ClientDisconnected(int EntityIndex);
	bool Check(CBasePlayer* Player);
	int GetVoteCount();
	void VoteRestart(CBasePlayer* Player);

private:
	bool m_Votes[MAX_CLIENTS + 1] = { false };
	int  m_RestartCount[PUG_STATE_END + 1] = { 0 };
};

extern CVoteRestart gVoteRestart;

