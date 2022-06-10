#pragma once

class CVotePause
{
public:
	void ClientDisconnected(int EntityIndex);
	bool Check(CBasePlayer* Player);
	int GetVoteCount(TeamName Team);
	void VotePause(CBasePlayer* Player);
	TeamName GetPauseTeam();
	void RoundRestart();
	void RoundStart();
	static void VotePauseTimer();

private:
	bool m_Votes[MAX_CLIENTS + 1][SPECTATOR + 1] = { false };
	TeamName m_Pause = UNASSIGNED;
	int m_Count[SPECTATOR + 1][PUG_STATE_END+1] = { 0 };
};

extern CVotePause gVotePause;