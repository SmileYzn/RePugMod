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

const char PUG_MOD_TEAM_STR[][24] =
{
	"Unnasigned",
	"Terrorists",
	"Counter-Terrorists",
	"Spectators"
};

class CPugMod
{
public:
	void Load();
	void Unload();

	int  GetState();
	void SetState(int State);
	void NextState(float Delay);
	static void RunState(CPugMod* PugMod);

	bool CheckBalanceTeams();

	bool StartVoteMap(CBasePlayer* Player);
	bool StartVoteTeam(CBasePlayer* Player);
	bool StartMatch(CBasePlayer* Player);
	bool StopMatch(CBasePlayer* Player);
	bool RestarPeriod(CBasePlayer* Player);
	bool EndGame(TeamName Winner);

	int GetRound();
	int GetScores(int Team);
	int GetWinner();

	void Help(CBasePlayer* Player,bool AdminHelp);
	void Status(CBasePlayer* Player);
	void Scores(CBasePlayer* Player);
	void ViewScores(CBasePlayer* Player);

	static void LO3(int Delay);
	static void SwapTeams(CPugMod* PugMod);

	void ClientConnected(edict_t* pEntity);
	void ClientGetIntoGame(CBasePlayer* Player);
	void ClientDisconnected(edict_t* pEntity);
	bool ClientCommand(CBasePlayer* Player, const char* pcmd, const char* parg1);
	bool ClientAddAccount(CBasePlayer* Player, int amount, RewardType type, bool bTrackChange);
	bool ClientHasRestrictItem(CBasePlayer* Player, ItemID item, ItemRestType type);
	bool ClientJoinTeam(CBasePlayer* Player, int NewTeam);

	void RoundStart();
	void RoundEnd(int winStatus, ScenarioEventEndRound event, float tmDelay);
	void RoundRestart();

private:
	int m_State = PUG_STATE_DEAD;
	int m_Round[PUG_STATE_END + 1] = { 0 };
	int m_Score[PUG_STATE_END + 1][SPECTATOR + 1] = { 0 };

	int m_Frags[33] = { 0 };
	int m_Death[33] = { 0 };
};

extern CPugMod gPugMod;

