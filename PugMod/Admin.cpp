#include "precompiled.h"

CAdmin gAdmin;

void CAdmin::Load()
{
	this->m_Data.clear();

	std::ifstream File(ADMIN_CONFIG_FILE, std::ifstream::in);

	std::string Auth, Name;

	while (File >> Auth >> Name)
	{
		Auth.erase(std::remove(Auth.begin(), Auth.end(), '\"'), Auth.end());

		Name.erase(std::remove(Name.begin(), Name.end(), '\"'), Name.end());

		this->m_Data.insert(std::make_pair(Auth, Name));
	}

	File.close();
}

bool CAdmin::Check(edict_t* pEntity)
{
	const char* Auth = GETPLAYERAUTHID(pEntity);

	if (Auth)
	{
		auto it = this->m_Data.find(Auth);

		return (it != this->m_Data.end());
	}

	return false;
}

void CAdmin::Menu(CBasePlayer* Player)
{
	if (this->Check(Player->edict()))
	{
		auto EntityIndex = Player->entindex();

		gMenu[EntityIndex].Create("PUG Mod Menu:", true, this->MenuHandle);

		gMenu[EntityIndex].AddItem(0, "Kick Player");
		gMenu[EntityIndex].AddItem(1, "Slap Player"); 
		gMenu[EntityIndex].AddItem(2, "Change Map");
		gMenu[EntityIndex].AddItem(3, "Pug Mod Control");
		gMenu[EntityIndex].AddItem(4, "Send Message");
		gMenu[EntityIndex].AddItem(5, "Send Command");

		gMenu[EntityIndex].Show(EntityIndex);
	}
}

void CAdmin::MenuHandle(int EntityIndex, int ItemIndex, bool Disabled, const char* Option)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		switch (ItemIndex)
		{
			case 0:
			{
				gAdmin.MenuKick(EntityIndex);
				break;
			}
			case 1:
			{
				gAdmin.MenuSlap(EntityIndex);
				break;
			}
			case 2:
			{
				gAdmin.MenuMap(EntityIndex);
				break;
			}
			case 3:
			{
				gAdmin.MenuControl(EntityIndex);
				break;
			}
			case 4:
			{
				CLIENT_COMMAND(Player->edict(), "messagemode !msg\n");
				break;
			}
			case 5:
			{
				CLIENT_COMMAND(Player->edict(), "messagemode !rcon\n");
				break;
			}
		}
	}
}

void CAdmin::MenuKick(int EntityIndex)
{
	gMenu[EntityIndex].Create("Kick Player:", true, this->MenuKickHandle);

	CBasePlayer* Players[32] = { NULL };

	auto Num = gPlayer.GetList(Players);

	for (int i = 0; i < Num; i++)
	{
		if (!gAdmin.Check(Players[i]->edict()))
		{
			gMenu[EntityIndex].AddItem(Players[i]->entindex(), STRING(Players[i]->edict()->v.netname));
		}
	}

	gMenu[EntityIndex].Show(EntityIndex);
}

void CAdmin::MenuKickHandle(int EntityIndex, int ItemIndex, bool Disabled, const char* Option)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		auto Target = UTIL_PlayerByIndexSafe(ItemIndex);

		if (Target)
		{
			gPlayer.DropClient(Target->entindex(), "Kicked by %s.", STRING(Player->edict()->v.netname));

			gUtil.SayText(NULL, EntityIndex, "\3%s\1 Kicked \3%s\1", STRING(Player->edict()->v.netname), STRING(Target->edict()->v.netname));
		}
	}
}

void CAdmin::MenuSlap(int EntityIndex)
{
	gMenu[EntityIndex].Create("Kick Player:", true, this->MenuSlapHandle);

	CBasePlayer* Players[32] = { NULL };

	auto Num = gPlayer.GetList(Players);

	for (int i = 0; i < Num; i++)
	{
		if (!gAdmin.Check(Players[i]->edict()))
		{
			gMenu[EntityIndex].AddItem(Players[i]->entindex(), STRING(Players[i]->edict()->v.netname));
		}
	}

	gMenu[EntityIndex].Show(EntityIndex);
}

void CAdmin::MenuSlapHandle(int EntityIndex, int ItemIndex, bool Disabled, const char* Option)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		auto Target = UTIL_PlayerByIndexSafe(ItemIndex);

		if (Target)
		{
			MDLL_ClientKill(Target->edict());

			gUtil.SayText(NULL, EntityIndex, "\3%s\1 Killed \3%s\1", STRING(Player->edict()->v.netname), STRING(Target->edict()->v.netname));
		}
	}
}

void CAdmin::MenuMap(int EntityIndex)
{
	gMenu[EntityIndex].Create("Change Map:", true, this->MenuMapHandle);

	gMenu[EntityIndex].AddList(gUtil.LoadMapList(VOTE_MAP_FILE, true));

	gMenu[EntityIndex].Show(EntityIndex);
}

void CAdmin::MenuMapHandle(int EntityIndex, int ItemIndex, bool Disabled, const char* Option)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		gTask.Create(EntityIndex, 5.0, false, SERVER_COMMAND, (void*)gUtil.VarArgs("changelevel %s\n", Option));

		gUtil.SayText(NULL, EntityIndex, "\3%s\1 changed map to \4%s\1", STRING(Player->edict()->v.netname), Option);
	}
}

void CAdmin::MenuControl(int EntityIndex)
{
	gMenu[EntityIndex].Create("Control Pug Mod:", true, gAdmin.MenuHandle);

	gMenu[EntityIndex].AddItem(0, "Run Vote Map");
	gMenu[EntityIndex].AddItem(1, "Run Vote Teams");
	gMenu[EntityIndex].AddItem(2, "Start Match");
	gMenu[EntityIndex].AddItem(3, "Stop Match");
	gMenu[EntityIndex].AddItem(4, "Restart Period");
	gMenu[EntityIndex].AddItem(5, "Toggle Ready System");

	gMenu[EntityIndex].Show(EntityIndex);
}

void CAdmin::MenuControlHandle(int EntityIndex, int ItemIndex, bool Disabled, const char* Option)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		switch (ItemIndex)
		{
			case 0: // Start Vote Map
			{
				gPugMod.StartVoteMap(Player);
				break;
			}
			case 1: // Start Vote Teams
			{
				gPugMod.StartVoteTeam(Player);

				break;
			}
			case 2: // Start Match
			{
				gPugMod.StartMatch(Player);
				break;
			}
			case 3: // Stop Match
			{
				gPugMod.StopMatch(Player);
				break;
			}
			case 4: // Restart Period
			{
				gPugMod.RestarPeriod(Player);
				break;
			}
			case 5: // Toggle Ready System 
			{
				gReady.Toggle(Player);
				break; 
			}
		}
	}
}

void CAdmin::Chat(CBasePlayer* Player, const char* Args)
{
	if (this->Check(Player->edict()))
	{
		if (Args)
		{
			std::string Message = Args;

			Message.erase(std::remove(Message.begin(), Message.end(), '\"'), Message.end());

			gUtil.SayText(NULL, Player->entindex(), "\3(%s)\1: %s", STRING(Player->edict()->v.netname), Message.c_str());
		}
		else
		{
			gUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, "Usage: !msg \3<Text Message>\1");
		}
	}
}

void CAdmin::Rcon(CBasePlayer* Player, const char* Args)
{
	if (this->Check(Player->edict()))
	{
		if (Args)
		{
			std::string Command = Args;

			Command.erase(std::remove(Command.begin(), Command.end(), '\"'), Command.end());

			gUtil.ServerCommand(Command.c_str());

			gUtil.SayText(Player->edict(), Player->entindex(), "\3Command Send\1: %s", Command.c_str());
		}
		else
		{
			gUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, "Usage: !send \3<Server Command>\1");
		}
	}
}
