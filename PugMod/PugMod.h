#pragma once

enum PUG_MOD_STATES_NUM
{
	PUG_STATE_DEAD			= 0,
	PUG_STATE_WARMUP		= 1,
	PUG_STATE_START			= 2,
	PUG_STATE_FIRST_HALF	= 3,
	PUG_STATE_HALFTIME		= 4,
	PUG_STATE_SECOND_HALF	= 5,
	PUG_STATE_OVERTIME		= 6,
	PUG_STATE_END			= 7
};

const char PUG_MOD_STATES_STR[][32] =
{
	"Dead",
	"Warmup",
	"Starting",
	"First Half",
	"Half Time",
	"Second Half",
	"Overtime",
	"Finished"
};

const char PUG_MOD_TEAM_STR[][21] =
{
	"Unnasigned",
	"Terrorists",
	"Counter-Terrorists",
	"Spectators"
};

const char PUG_MOD_TEAM_STR_SHORT[][6] =
{
	"NONE",
	"TR",
	"CT",
	"SPEC"
};

class CPugMod
{
public:
	void Load();
	void Unload();

	int GetState();
	char* GetStateName();

	bool IsLive();

	void SetState(int State);
	void NextState(float Delay);
	static void RunState();

	bool CheckBalanceTeams();

	bool StartVoteMap(CBasePlayer* Player);
	bool StartVoteTeam(CBasePlayer* Player);
	bool StartMatch(CBasePlayer* Player);
	bool StopMatch(CBasePlayer* Player);
	bool RestarPeriod(CBasePlayer* Player);
	bool EndGame(TeamName Winner);
	bool PauseMatch(CBasePlayer* Player);

	int GetRound();
	int GetScores(int Team);

	void ResetScores();
	void ResetStateScores();

	TeamName GetWinner();
	bool GetOvertimeWinner();

	void Help(CBasePlayer* Player,bool AdminHelp);
	void Status(CBasePlayer* Player);
	void Scores(CBasePlayer* Player);
	void ViewScores(CBasePlayer* Player);

	static void LO3(const char* Time);
	static void SwapTeams();

	void SwapScores();

	void ClientConnected(edict_t* pEntity);
	void ClientDisconnected(int EntityIndex);
	bool ClientAddAccount(CBasePlayer* Player, int amount, RewardType type, bool bTrackChange);
	bool ClientHasRestrictItem(CBasePlayer* Player, ItemID item, ItemRestType type);
	bool ClientJoinTeam(CBasePlayer* Player, int NewTeam);

	void RoundStart();
	void RoundEnd(int winStatus, ScenarioEventEndRound event, float tmDelay);
	void RoundRestart(bool PreRestart);

	static void PauseTimer();

private:
	int m_State = PUG_STATE_DEAD;
	int m_Round[PUG_STATE_END + 1] = { 0 };
	int m_Score[PUG_STATE_END + 1][SPECTATOR + 1] = { 0 };
	int m_Pause = false;
};

extern CPugMod gPugMod;

