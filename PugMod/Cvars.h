#pragma once

class CCvars
{
public:
	void Load();

	cvar_t* Register(const char* Name, const char* Value);

	cvar_t* GetConfig(int State);
	cvar_t* GetLanguage();
	cvar_t* GetAutoStartDelay();
	cvar_t* GetPlayersMin();
	cvar_t* GetPlayersMax();
	cvar_t* GetPlayRounds();
	cvar_t* GetPlayRoundsOvertime();
	cvar_t* GetPlayRoundsOvertimeType();
	cvar_t* GetPlayCheckPlayers();
	cvar_t* GetVoteDelay();
	cvar_t* GetVoteMap();
	cvar_t* GetVoteMapType();
	cvar_t* GetVoteMapSelf();
	cvar_t* GetVoteTeamType();
	cvar_t* GetVotePercentage();
	cvar_t* GetVotePauseTime();
	cvar_t* GetReadyType();
	cvar_t* GetReadyTime();
	cvar_t* GetShowScoreType();
	cvar_t* GetFloodTime();
	cvar_t* GetStatsRoundEnd();
	cvar_t* GetReconnectDelay();
	cvar_t* GetKnifeRoundEndType();

private:
	cvar_t* m_Config[8];
	cvar_t* m_Language;
	cvar_t* m_AutoStartDelay;
	cvar_t* m_PlayersMin;
	cvar_t* m_PlayersMax;
	cvar_t* m_PlayRounds;
	cvar_t* m_PlayRoundsOvertime;
	cvar_t* m_PlayRoundsOvertimeType;
	cvar_t* m_PlayCheckPlayers;
	cvar_t* m_VoteDelay;
	cvar_t* m_VoteMap;
	cvar_t* m_VoteMapType;
	cvar_t* m_VoteMapSelf;
	cvar_t* m_VoteTeamType;
	cvar_t* m_VotePercentage;
	cvar_t* m_VotePauseTime;
	cvar_t* m_ReadyType;
	cvar_t* m_ReadyTime;
	cvar_t* m_ShowScoreType;
	cvar_t* m_FloodTime;
	cvar_t* m_StatsRoundEnd;
	cvar_t* m_ReconnectDelay;
	cvar_t* m_KnifeRoundEndType;
};

extern CCvars gCvars;