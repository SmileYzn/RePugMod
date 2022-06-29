#include "precompiled.h"

CCommands gCommands;

enum ConsoleCmd_ : std::uint32_t
{
	ConsoleCmd_AdminMenu   = Hash::GetConst( "pug_admin_menu" ),
	ConsoleCmd_KickMenu    = Hash::GetConst( "pug_kick_menu" ),
	ConsoleCmd_BanMenu     = Hash::GetConst( "pug_ban_menu" ),
	ConsoleCmd_SlapMenu    = Hash::GetConst( "pug_slap_menu" ),
	ConsoleCmd_TeamMenu    = Hash::GetConst( "pug_team_menu" ),
	ConsoleCmd_MapMenu     = Hash::GetConst( "pug_map_menu" ),
	ConsoleCmd_ControlMenu = Hash::GetConst( "pug_control_menu" ),
	ConsoleCmd_Msg         = Hash::GetConst( "pug_msg" ),
};

enum ServerCmd_ : std::uint32_t
{
	ServerCmd_MenuSelect = Hash::GetConst( "menuselect" ),
	ServerCmd_Say        = Hash::GetConst( "say" ),
	ServerCmd_SayTeam    = Hash::GetConst( "say_team" ),

	ServerCmd_Menu       = Hash::GetConst( "!menu" ),
	ServerCmd_Kick       = Hash::GetConst( "!kick" ),
	ServerCmd_Ban        = Hash::GetConst( "!ban" ),
	ServerCmd_Slap       = Hash::GetConst( "!slap" ),
	ServerCmd_Team       = Hash::GetConst( "!team" ),
	ServerCmd_Map        = Hash::GetConst( "!map" ),
	ServerCmd_Control    = Hash::GetConst( "!control" ),
	ServerCmd_Msg        = Hash::GetConst( "!msg" ),
	ServerCmd_Rcon       = Hash::GetConst( "!rcon" ),
	ServerCmd_Help       = Hash::GetConst( "!help" )
};

enum ClientCmd_ : std::uint32_t
{
	ClientCmd_Help      = Hash::GetConst( ".help" ),
	ClientCmd_Status    = Hash::GetConst( ".status" ),
	ClientCmd_Score     = Hash::GetConst( ".score" ),
	ClientCmd_Ready     = Hash::GetConst( ".ready" ),
	ClientCmd_NotReady  = Hash::GetConst( ".notready" ),
	ClientCmd_Vote      = Hash::GetConst( ".vote" ),
	ClientCmd_VoteKick  = Hash::GetConst( ".votekick" ),
	ClientCmd_VoteMap   = Hash::GetConst( ".votemap" ),
	ClientCmd_VotePause = Hash::GetConst( ".votepause" ),
	ClientCmd_Surrender = Hash::GetConst( ".surrender" ),
	ClientCmd_Hp        = Hash::GetConst( ".hp" ),
	ClientCmd_Dmg       = Hash::GetConst( ".dmg" ),
	ClientCmd_Rdmg      = Hash::GetConst( ".rdmg" ),
	ClientCmd_Sum       = Hash::GetConst( ".sum" )
};

bool CCommands::ClientCommand( CBasePlayer* Player, const char* pcmd, const char* parg1 )
{
	//
	// skip invalid command
	//
	if ( !pcmd )
	{
		return false;
	}

	//
	// skip empty command
	//
	if ( pcmd[ 0u ] == '\0' )
	{
		return false;
	}

	//
	// get command hash
	//
	const auto CommandHash{ Hash::Get( pcmd ) };

	//
	// handle menu select
	//
	if ( CommandHash == ServerCmd_MenuSelect )
	{
		if ( parg1 )
		{
			if ( Player->m_iMenu == Menu_OFF )
			{
				if ( gMenu[ Player->entindex( ) ].Handle( Player->entindex( ), atoi( parg1 ) ) )
				{
					return true;
				}
			}
		}

		return false;
	}

	//
	// handle 'say' or 'say_team'
	//
	if ( CommandHash == ServerCmd_Say ||
		 CommandHash == ServerCmd_SayTeam )
	{
		if ( parg1 )
		{
			const bool HasServerCommand { parg1[ 0u ] == '!' };
			const bool HasClientCommand { parg1[ 0u ] == '.' };

			if ( HasServerCommand || HasClientCommand )
			{
				if ( auto pCmdArgs{ CMD_ARGS( ) } )
				{
					//
					// handle the command recursively
					//
					gUtil.ClientCommand( Player->edict( ), "%s", pCmdArgs );

					return true;
				}
			}
		}
	}

	//
	// handle a possible server, client or console command
	// peek at the first character for a short optimization
	//
	else
	{
		//
		// is server command
		//
		if ( pcmd[ 0u ] == '!' )
		{
			switch ( CommandHash )
			{
				case ServerCmd_Menu:    gAdmin.Menu( Player ); break;
				case ServerCmd_Kick:    gAdmin.MenuKick( Player->entindex( ) ); break;
				case ServerCmd_Ban:     gAdmin.MenuBan( Player->entindex( ) ); break;
				case ServerCmd_Slap:    gAdmin.MenuSlap( Player->entindex( ) ); break;
				case ServerCmd_Team:    gAdmin.MenuTeam( Player->entindex( ) ); break;
				case ServerCmd_Map:     gAdmin.MenuMap( Player->entindex( ) ); break;
				case ServerCmd_Control: gAdmin.MenuControl( Player->entindex( ) ); break;
				case ServerCmd_Msg:     gAdmin.Chat( Player, CMD_ARGS( ) ); break;
				case ServerCmd_Rcon:    gAdmin.Rcon( Player, CMD_ARGS( ) ); break;

				case ServerCmd_Help:    gPugMod.Help( Player, true ); break;

				default: return false;
			}

			return true;
		}

		//
		// is client command
		//
		if ( pcmd[ 0u ] == '.' )
		{
			switch ( CommandHash )
			{
				case ClientCmd_Help:      gPugMod.Help( Player, false ); break;
				case ClientCmd_Status:    gPugMod.Status( Player ); break;
				case ClientCmd_Score:     gPugMod.Scores( Player ); break;

				case ClientCmd_Ready:     gReady.Ready( Player ); break;
				case ClientCmd_NotReady:  gReady.NotReady( Player ); break;

				case ClientCmd_Vote:      gVoteMenu.Menu( Player ); break;
				case ClientCmd_VoteKick:  gVoteKick.Menu( Player ); break;
				case ClientCmd_VoteMap:   gVoteLevel.Menu( Player ); break;
				case ClientCmd_VotePause: gVotePause.VotePause( Player ); break;
				case ClientCmd_Surrender: gVoteStop.VoteStop( Player ); break;

				case ClientCmd_Hp:        gStatsCmd.HP( Player ); break;
				case ClientCmd_Dmg:       gStatsCmd.Damage( Player ); break;
				case ClientCmd_Rdmg:      gStatsCmd.Received( Player ); break;
				case ClientCmd_Sum:       gStatsCmd.Summary( Player ); break;

				default: return false;
			}

			return true;
		}

		//
		// is console command
		//
		if ( pcmd[ 0u ] == 'p' )
		{
			switch ( CommandHash )
			{
				case ConsoleCmd_AdminMenu:   gAdmin.Menu( Player ); break;
				case ConsoleCmd_KickMenu:    gAdmin.MenuKick( Player->entindex( ) ); break;
				case ConsoleCmd_BanMenu:     gAdmin.MenuBan( Player->entindex( ) ); break;
				case ConsoleCmd_SlapMenu:    gAdmin.MenuSlap( Player->entindex( ) ); break;
				case ConsoleCmd_TeamMenu:    gAdmin.MenuTeam( Player->entindex( ) ); break;
				case ConsoleCmd_MapMenu:     gAdmin.MenuMap( Player->entindex( ) ); break;
				case ConsoleCmd_ControlMenu: gAdmin.MenuControl( Player->entindex( ) ); break;
				case ConsoleCmd_Msg:         gAdmin.Chat( Player, CMD_ARGS( ) ); break;

				default: return false;
			}

			return true;
		}
	}

	return false;
}