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

	// Pug Mod Language (Two characters to represent language in lang file)
	this->m_Language = this->Register("pug_language", "en");

	// Time before initialize Pug Mod after start map (0 to disable auto initialization)
	this->m_AutoStartDelay = this->Register("pug_auto_start_delay", "6.0");

	// Minimum of players to start a game
	this->m_PlayersMin = this->Register("pug_players_min", "10");

	// Maximum of players allowed in game
	this->m_PlayersMax = this->Register("pug_players_max", "10");

	// Start knife round before match to choose sides
	this->m_PlayKnifeRound = this->Register("pug_play_knife_round", "1");

	// Rounds to play before start overtime
	this->m_PlayRounds = this->Register("pug_play_rounds", "30");

	// Rounds to play while in overtime
	this->m_PlayRoundsOvertime = this->Register("pug_play_overtime_rounds", "6");

	// Play Overtime (0 Sudden death, 1 Force overtime, 2 End tied, 3 Users vote)
	this->m_PlayRoundsOvertimeType = this->Register("pug_play_overtime", "1");

	// Swap teams before overtime start or restart
	this->m_PlayRoundsOvertimeSwap = this->Register("pug_play_overtime_swap", "0");

	// Check players count before start pug and start warmup if is incorrect
	this->m_PlayCheckPlayers = this->Register("pug_play_check_players", "1");

	// Delay in vote session
	this->m_VoteDelay = this->Register("pug_vote_delay", "15.0");

	// Active vote map in current map (Not used in configuration files)
	this->m_VoteMap = this->Register("pug_vote_map", "0");

	// Type of vote map in pug (0 Disable, 1 Enable, 2 Random map)
	this->m_VoteMapType = this->Register("pug_vote_map_type", "1");

	// What will happen if vote map fail (0 Continue match, 1 Restart vote map, 2 Choose a random map)
	this->m_VoteMapFailType = this->Register("pug_vote_map_fail_type", "0");

	// Allow current map in map list vote session
	this->m_VoteMapSelf = this->Register("pug_vote_map_self", "0");

	// Run votemap after match end
	this->m_VoteMapEnd = this->Register("pug_vote_map_end", "0");

	// The teams method for assign teams (-1 Vote, 0 Leaders, 1 Random, 2 None, 3 Skill Balanced, 4 Swap Teams, 5 Knife Round)
	this->m_VoteTeamType = this->Register("pug_vote_team_type", "-1");

	// Minimum percentage to take actions in vote systems
	this->m_VotePercentage = this->Register("pug_vote_percentage", "0.70");

	// Time in seconds to pause match while vote pause is running (0 to disable vote pause)
	this->m_VotePauseTime = this->Register("pug_vote_pause_time", "60");

	// Pause count limit of team per half
	this->m_VotePauseLimit = this->Register("pug_vote_pause_limit", "2");
	
	// Allow match pause during freezetime
	this->m_VotePauseFreezetime = this->Register("pug_vote_pause_freezetime", "1");

	// Limit period restarts per half
	this->m_VoteRestartLimit = this->Register("pug_vote_restart_limit", "1");

	// Ready system type (1 Ready System, 0 Timer Counter)
	this->m_ReadyType = this->Register("pug_ready_type", "1");

	// Time limit to start match in ready timer system
	this->m_ReadyTime = this->Register("pug_ready_timer", "60.0");

	// How PUG display scores in scoreboard (0 Reset in each restart, 1 maintain team scores, 2 maintain team scores + player scores)
	this->m_ShowScoreType = this->Register("pug_show_score_type", "2");

	// Anti flood delay
	this->m_FloodTime = this->Register("pug_flood_time", "0.75");

	// Display round stats at console when round end
	this->m_StatsRoundEnd = this->Register("pug_stats_round_end", "1");

	// Anti Reconnect time in seconds (0 Disable)
	this->m_ReconnectDelay = this->Register("pug_reconnect_delay", "20.0");

	// Time to ban a player after try to drop or reconnect from a live game (0 Disable, time in minutes)
	this->m_ReconnectBanTime = this->Register("pug_reconnect_ban_time", "0");

	// Knife Round End Type (0 Continue match if no clear winner, 1 restart Knife Round)
	this->m_KnifeRoundEndType = this->Register("pug_knife_round_end_type", "0");

	// Admin help MOTD page (File path or website URL)
	this->m_HelpFileAdmin = this->Register("pug_help_admin", "cstrike/addons/pugmod/help_admin.htm");

	// Player help MOTD page (File path or website URL)
	this->m_HelpFilePlayer = this->Register("pug_help_player", "cstrike/addons/pugmod/help_player.htm");

	// Allowed ban times in minutes (0 To ban permanently)
	this->m_BanTimes = this->Register("pug_ban_times", "0 5 10 15 30 45 60 120");
}

cvar_t* CCvars::Register(const char* Name, const char* Value)
{
	// Get Pointer by variable name
	cvar_t* Pointer = CVAR_GET_POINTER(Name);

	// If not exists
	if (Pointer == NULL)
	{
		// Make structure data
		cvar_t Register = { Name, (char*)Value, FCVAR_SERVER | FCVAR_SPONLY };

		// Register the variable
		CVAR_REGISTER(&Register);

		// Return created pointer
		return CVAR_GET_POINTER(Name);
	}

	// Return existing pointer to variable
	return Pointer;
}

cvar_t* CCvars::GetConfig(int State)
{
	return this->m_Config[State];
}

cvar_t* CCvars::GetLanguage()
{
	return this->m_Language;
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

cvar_t* CCvars::GetPlayKnifeRound()
{
	return this->m_PlayKnifeRound;
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

cvar_t* CCvars::GetPlayRoundsOvertimeSwap()
{
	return this->m_PlayRoundsOvertimeSwap;
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

cvar_t* CCvars::GetVoteMapFailType()
{
	return this->m_VoteMapFailType;
}

cvar_t* CCvars::GetVoteMapSelf()
{
	return this->m_VoteMapSelf;
}

cvar_t* CCvars::GetVoteMapEnd()
{
	return this->m_VoteMapEnd;
}

cvar_t* CCvars::GetVoteTeamType()
{
	return this->m_VoteTeamType;
}

cvar_t* CCvars::GetVotePercentage()
{
	return this->m_VotePercentage;
}

cvar_t* CCvars::GetVotePauseTime()
{
	return this->m_VotePauseTime;
}

cvar_t* CCvars::GetVotePauseLimit()
{
	return this->m_VotePauseLimit;
}

cvar_t* CCvars::GetVotePauseFreezetime()
{
	return this->m_VotePauseFreezetime;
}

cvar_t* CCvars::GetVoteRestartLimit()
{
	return this->m_VoteRestartLimit;
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

cvar_t* CCvars::GetReconnectBanTime()
{
	return m_ReconnectBanTime;
}

cvar_t* CCvars::GetKnifeRoundEndType()
{
	return this->m_KnifeRoundEndType;
}

cvar_t* CCvars::GetHelpFileAdmin()
{
	return this->m_HelpFileAdmin;
}

cvar_t* CCvars::GetHelpFilePlayer()
{
	return this->m_HelpFilePlayer;
}

cvar_t* CCvars::GetBanTimes()
{
	return m_BanTimes;
}
