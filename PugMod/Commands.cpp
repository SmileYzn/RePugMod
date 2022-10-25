#include "precompiled.h"

CCommands gCommands;

bool CCommands::ClientCommand(CBasePlayer* Player, const char* pcmd, const char* parg1)
{
	if (!pcmd || pcmd[0u] == '\0')
	{
		return false;
	}

	if (Q_stricmp(pcmd, "say") == 0 || Q_stricmp(pcmd, "say_team") == 0)
	{
		if (parg1)
		{
			if (parg1[0u] == '.' || parg1[0u] == '!')
			{
				if (auto pCmdArgs{ CMD_ARGS() })
				{
					gUtil.ClientCommand(Player->edict(), "%s", pCmdArgs);

					return true;
				}
			}
		}
	}
	else if (Q_stricmp(pcmd, "menuselect") == 0)
	{
		if (parg1)
		{
			if (Player->m_iMenu == Menu_OFF)
			{
				if (gMenu[Player->entindex()].Handle(Player->entindex(), atoi(parg1)))
				{
					return true;
				}
			}
		}

		return false;
	}
	else if (Q_stricmp(pcmd, "!menu") == 0 || Q_stricmp(pcmd, "pug_admin_menu") == 0)
	{
		gAdmin.Menu(Player);
		return true;
	}
	else if (Q_stricmp(pcmd, "!kick") == 0 || Q_stricmp(pcmd, "pug_kick_menu") == 0)
	{
		gAdmin.MenuKick(Player->entindex());
		return true;
	}
	else if (Q_stricmp(pcmd, "!ban") == 0 || Q_stricmp(pcmd, "pug_ban_menu") == 0)
	{
		gAdmin.MenuBan(Player->entindex());
		return true;
	}
	else if (Q_stricmp(pcmd, "!slap") == 0 || Q_stricmp(pcmd, "pug_slap_menu") == 0)
	{
		gAdmin.MenuSlap(Player->entindex());
		return true;
	}
	else if (Q_stricmp(pcmd, "!team") == 0 || Q_stricmp(pcmd, "pug_team_menu") == 0)
	{
		gAdmin.MenuTeam(Player->entindex());
		return true;
	}
	else if (Q_stricmp(pcmd, "!map") == 0 || Q_stricmp(pcmd, "pug_map_menu") == 0)
	{
		gAdmin.MenuMap(Player->entindex());
		return true;
	}
	else if (Q_stricmp(pcmd, "!control") == 0 || Q_stricmp(pcmd, "pug_control_menu") == 0)
	{
		gAdmin.MenuControl(Player->entindex());
		return true;
	}
	else if (Q_stricmp(pcmd, "!msg") == 0 || Q_stricmp(pcmd, "pug_msg") == 0)
	{
		gAdmin.Chat(Player, CMD_ARGS());
		return true;
	}
	else if (Q_stricmp(pcmd, "!rcon") == 0 || Q_stricmp(pcmd, "pug_rcon") == 0)
	{
		gAdmin.Rcon(Player, CMD_ARGS());
		return true;
	}
	else if (Q_stricmp(pcmd, "!help") == 0 || Q_stricmp(pcmd, "pug_help") == 0)
	{
		gPugMod.Help(Player, true);
		return true;
	}
	else if (Q_stricmp(pcmd, ".help") == 0)
	{
		gPugMod.Help(Player, false);
		return true;
	}
	else if (Q_stricmp(pcmd, ".status") == 0)
	{
		gPugMod.Status(Player);
		return true;
	}
	else if (Q_stricmp(pcmd, ".score") == 0)
	{
		gPugMod.Scores(Player);
		return true;
	}
	else if (Q_stricmp(pcmd, ".ready") == 0)
	{
		gReady.Ready(Player);
		return true;
	}
	else if (Q_stricmp(pcmd, ".notready") == 0)
	{
		gReady.NotReady(Player);
		return true;
	}
	else if (Q_stricmp(pcmd, ".vote") == 0)
	{
		gVoteMenu.Menu(Player);
		return true;
	}
	else if (Q_stricmp(pcmd, ".votekick") == 0)
	{
		gVoteKick.Menu(Player);
		return true;
	}
	else if (Q_stricmp(pcmd, ".votemap") == 0)
	{
		gVoteLevel.Menu(Player);
		return true;
	}
	else if (Q_stricmp(pcmd, ".votepause") == 0)
	{
		gVotePause.VotePause(Player);
		return true;
	}
	else if (Q_stricmp(pcmd, ".surrender") == 0)
	{
		gVoteStop.VoteStop(Player);
		return true;
	}
	else if (Q_stricmp(pcmd, ".hp") == 0)
	{
		gStatsCmd.HP(Player);
		return true;
	}
	else if (Q_stricmp(pcmd, ".dmg") == 0)
	{
		gStatsCmd.Damage(Player);
		return true;
	}
	else if (Q_stricmp(pcmd, ".rdmg") == 0)
	{
		gStatsCmd.Received(Player);
		return true;
	}
	else if (Q_stricmp(pcmd, ".sum") == 0)
	{
	gStatsCmd.Summary(Player);
		return true;
	}

	return false;
}