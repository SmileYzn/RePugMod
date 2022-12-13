#pragma once

class CPlayer
{
public:
	void TeamInfo(edict_t* pEntity, int playerIndex, const char *pszTeamName);

	int GetList(CBasePlayer* pPlayers[MAX_CLIENTS], bool InGameOnly);
	int GetList(CBasePlayer* pPlayers[MAX_CLIENTS], TeamName Team);

	int GetNum(bool CountBots, int &InGame, int &NumTerrorist, int &NumCT, int& NumSpectator);
	int GetNum();
	int GetNum(TeamName Team);
	int GetNum(bool CountBots);
	int GetNum(bool CountBots,TeamName Team);
	int GetNum(int& NumAliveTerrorists, int& NumAliveCT);

	CBasePlayer* GetRandom(TeamName Team);

	void DropClient(int EntityIndex, const char* Format, ...);
	void BanClient(int EntityIndex, int Time, bool Kick);
};

extern CPlayer gPlayer;