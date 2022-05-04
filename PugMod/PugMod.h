#pragma once

#define HELP_FILE_PLAYER "cstrike/addons/pugmod/help_player.htm"
#define HELP_FILE_ADMIN "cstrike/addons/pugmod/help_admin.htm"

enum PUG_MOD_STATES_NUM
{
	PUG_STATE_DEAD,
	PUG_STATE_WARMUP,
	PUG_STATE_START,
	PUG_STATE_FIRST_HALF,
	PUG_STATE_HALFTIME,
	PUG_STATE_SECOND_HALF,
	PUG_STATE_OVERTIME,
	PUG_STATE_END
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

	void StartVoteMap(CBasePlayer* Player);
	void StartVoteTeam(CBasePlayer* Player);
	void StartMatch(CBasePlayer* Player);
	void StopMatch(CBasePlayer* Player);
	void RestarPeriod(CBasePlayer* Player);
	void EndGame(TeamName Winner);

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
	int m_State;
	int m_Round[PUG_STATE_END + 1];
	int m_Score[PUG_STATE_END + 1][SPECTATOR + 1];

	int m_Frags[33];
	int m_Death[33];
};

extern CPugMod gPugMod;

