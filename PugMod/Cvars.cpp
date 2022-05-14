#include "precompiled.h"

CCvars gCvars;

void CCvars::Load()
{
	// Main
	this->m_Config[PUG_STATE_DEAD] = this->Register("pug_cfg_pugmod", "pugmod.cfg");

	// Warmup
	this->m_Config[PUG_STATE_WARMUP] = this->Register("pug_cfg_warmup", "warmup.cfg");

	// Vote Session
	this->m_Config[PUG_STATE_START] = this->Register("pug_cfg_start", "start.cfg");

	// First Half
	this->m_Config[PUG_STATE_FIRST_HALF] = this->Register("pug_cfg_1st", "esl.cfg");

	// Halftime
	this->m_Config[PUG_STATE_HALFTIME] = this->Register("pug_cfg_halftime", "halftime.cfg");

	// Second Half
	this->m_Config[PUG_STATE_SECOND_HALF] = this->Register("pug_cfg_2nd", "esl.cfg");

	// Overtime
	this->m_Config[PUG_STATE_OVERTIME] = this->Register("pug_cfg_overtime", "el-ot.cfg");

	// End
	this->m_Config[PUG_STATE_END] = this->Register("pug_cfg_end", "end.cfg");

	// Time before initialize Pug Mod after start map (0 to disable auto initialization)
	this->m_AutoStartDelay = this->Register("pug_auto_start_delay", "6.0");

	// Minimum of players to start a game
	this->m_PlayersMin = this->Register("pug_players_min", "10");

	// Maximum of players allowed in game
	this->m_PlayersMax = this->Register("pug_players_max", "10");

	// Rounds to play before start overtime
	this->m_PlayRounds = this->Register("pug_play_rounds", "30");

	// Win difference (rounds) to determine a winner in overtime
	this->m_PlayRoundsOvertime = this->Register("pug_play_overtime_rounds", "3");

	// Play Overtime (0 Sudden Death, 1 Force Overtime, 2 End Tied)
	this->m_PlayRoundsOvertimeType = this->Register("pug_play_overtime", "1");

	// Check players count before start pug and start warmup if is incorrect
	this->m_PlayCheckPlayers = this->Register("pug_play_check_players", "1");

	// Delay in vote session
	this->m_VoteDelay = this->Register("pug_vote_delay", "15.0");

	// Active vote map in current map (Not used in configuration files)
	this->m_VoteMap = this->Register("pug_vote_map", "0");

	// Active vote map in pug (0 Disable, 1 Enable, 2 Random map)
	this->m_VoteMapType = this->Register("pug_vote_map_type", "1");

	// Allow current map in map vote session
	this->m_VoteMapSelf = this->Register("pug_vote_map_self", "0");

	// The teams method for assign teams (-1 Vote, 0 Leaders, 1 Random, 2 None, 3 Skill Balanced, 4 Swap Teams, 5 Knife Round)
	this->m_VoteTeamType = this->Register("pug_vote_team_type", "-1");

	// Minimum percentage to take actions in vote systems
	this->m_VotePercentage = this->Register("pug_vote_percentage", "0.70");

	// Time in seconds to pause match while vote pause is running (0 to disable vote pause)
	this->m_VotePauseTime = this->Register("pug_vote_pause_time", "60");

	// Ready system type (1 Ready System, 0 Timer Counter)
	this->m_ReadyType = this->Register("pug_ready_type", "1");

	// Time limit to start match
	this->m_ReadyTime = this->Register("pug_ready_timer", "60.0");

	// How PUG display scores in scoreboard (0 Reset in each restart, 1 maintain team scores, 2 maintain team scores + player scores)
	this->m_ShowScoreType = this->Register("pug_show_score_type", "2");

	// Anti flood delay
	this->m_FloodTime = this->Register("pug_flood_time", "0.75");

	// Display round stats at console when round end
	this->m_StatsRoundEnd = this->Register("pug_stats_round_end", "1");

	// Anti Reconnect time in seconds (0 Disable)
	this->m_ReconnectDelay = this->Register("pug_reconnect_delay", "15.0");

	// Knife Round End Type (0 Continue match if no clear winner, 1 restart Knife Round)
	this->m_KnifeRoundEndType = this->Register("pug_knife_round_end_type", "0");
}

cvar_t* CCvars::Register(char* Name, char* Value)
{
	cvar_t* Pointer = CVAR_GET_POINTER(Name);

	if (Pointer == NULL)
	{
		cvar_t Register = { Name, Value, FCVAR_SERVER | FCVAR_SPONLY };

		CVAR_REGISTER(&Register);

		return CVAR_GET_POINTER(Name);
	}

	return Pointer;
}

cvar_t* CCvars::GetConfig(int State)
{
	return this->m_Config[State];
}

cvar_t* CCvars::GetAutoStartDelay()
{
	return this->m_AutoStartDelay;
}

cvar_t* CCvars::GetPlayersMin()
{
	return this->m_PlayersMin;
}

cvar_t* CCvars::GetPlayersMax()
{
	return this->m_PlayersMax;
}

cvar_t* CCvars::GetPlayRounds()
{
	return this->m_PlayRounds;
}

cvar_t* CCvars::GetPlayRoundsOvertime()
{
	return this->m_PlayRoundsOvertime;
}

cvar_t* CCvars::GetPlayRoundsOvertimeType()
{
	return this->m_PlayRoundsOvertimeType;
}

cvar_t* CCvars::GetPlayCheckPlayers()
{
	return this->m_PlayCheckPlayers;
}

cvar_t* CCvars::GetVoteDelay()
{
	return this->m_VoteDelay;
}

cvar_t* CCvars::GetVoteMap()
{
	return this->m_VoteMap;
}

cvar_t* CCvars::GetVoteMapType()
{
	return this->m_VoteMapType;
}

cvar_t* CCvars::GetVoteMapSelf()
{
	return this->m_VoteMapSelf;
}

cvar_t* CCvars::GetVoteTeamType()
{
	return this->m_VoteTeamType;
}

cvar_t * CCvars::GetVotePercentage()
{
	return this->m_VotePercentage;
}

cvar_t * CCvars::GetVotePauseTime()
{
	return this->m_VotePauseTime;
}

cvar_t* CCvars::GetReadyType()
{
	return this->m_ReadyType;
}

cvar_t* CCvars::GetReadyTime()
{
	return this->m_ReadyType;
}

cvar_t* CCvars::GetShowScoreType()
{
	return this->m_ShowScoreType;
}

cvar_t* CCvars::GetFloodTime()
{
	return this->m_FloodTime;
}

cvar_t* CCvars::GetStatsRoundEnd()
{
	return this->m_StatsRoundEnd;
}

cvar_t * CCvars::GetReconnectDelay()
{
	return this->m_ReconnectDelay;
}

cvar_t* CCvars::GetKnifeRoundEndType()
{
	return this->m_KnifeRoundEndType;
}
