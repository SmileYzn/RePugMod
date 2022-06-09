#pragma once

class CVotePause
{
public:
	void ClientDisconnected(edict_t* pEntity);
	bool Check(CBasePlayer* Player);
	void VotePause(CBasePlayer* Player);
	void RoundRestart();
	void RoundStart();
	static void VotePauseTimer();

private:
	bool m_VotedPause[MAX_CLIENTS + 1][SPECTATOR + 1] = { false };
	TeamName m_PausedTeam = UNASSIGNED;
};

extern CVotePause gVotePause;