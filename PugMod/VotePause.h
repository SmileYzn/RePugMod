#pragma once

class CVotePause
{
public:
	void ClientDisconnected(int EntityIndex);
	bool Check(CBasePlayer* Player);
	int GetVoteCount(TeamName Team);
	void VotePause(CBasePlayer* Player);
	void RoundRestart();
	void RoundStart();
	static void VotePauseTimer();

private:
	bool m_Votes[MAX_CLIENTS + 1][SPECTATOR + 1] = { false };
	TeamName m_Pause = UNASSIGNED;
};

extern CVotePause gVotePause;