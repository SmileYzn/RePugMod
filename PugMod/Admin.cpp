#include "precompiled.h"

CAdmin gAdmin;

void CAdmin::Load()
{
	this->m_Data.clear();

	memset(this->m_Type, 0, sizeof(this->m_Type));

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
		std::map<std::string, std::string>::iterator it = this->m_Data.find(Auth);

		return (it != this->m_Data.end());
	}

	return false;
}

void CAdmin::Menu(CBasePlayer* Player)
{
	if (this->Check(Player->edict()))
	{
		int EntityIndex = Player->entindex();

		this->SetType(EntityIndex, 0);

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
		if (gAdmin.GetType(EntityIndex) == 0)
		{
			if (ItemIndex == 0)
			{
				gAdmin.SetType(EntityIndex, 1);

				gMenu[EntityIndex].Create("Kick Player:", true, gAdmin.MenuHandle);

				CBasePlayer* Players[32] = { NULL };

				int Num = gPlayer.GetList(Players);

				for (int i = 0; i < Num; i++)
				{
					if (!gAdmin.Check(Players[i]->edict()))
					{
						gMenu[EntityIndex].AddItem(Players[i]->entindex(), STRING(Players[i]->edict()->v.netname));
					}
				}

				gMenu[EntityIndex].Show(EntityIndex);
			}
			else if (ItemIndex == 1)
			{
				gAdmin.SetType(EntityIndex, 2);

				gMenu[EntityIndex].Create("Kill Player:", true, gAdmin.MenuHandle);

				CBasePlayer* Players[32] = { NULL };

				int Num = gPlayer.GetList(Players);

				for (int i = 0; i < Num; i++)
				{
					if (!gAdmin.Check(Players[i]->edict()))
					{
						gMenu[EntityIndex].AddItem(Players[i]->entindex(), STRING(Players[i]->edict()->v.netname));
					}
				}

				gMenu[EntityIndex].Show(EntityIndex);
			}
			else if (ItemIndex == 2)
			{
				gAdmin.SetType(EntityIndex, 3);

				gMenu[EntityIndex].Create("Change Map:", true, gAdmin.MenuHandle);

				gMenu[EntityIndex].AddList(gUtil.LoadMapList(VOTE_MAP_FILE, true));

				gMenu[EntityIndex].Show(EntityIndex);
			}
			else if (ItemIndex == 3)
			{
				gAdmin.SetType(EntityIndex, 4);

				gMenu[EntityIndex].Create("Control Pug Mod:", true, gAdmin.MenuHandle);

				gMenu[EntityIndex].AddItem(0, "Run Vote Map");
				gMenu[EntityIndex].AddItem(1, "Run Vote Teams");
				gMenu[EntityIndex].AddItem(2, "Start Match");
				gMenu[EntityIndex].AddItem(3, "Stop Match");
				gMenu[EntityIndex].AddItem(4, "Restart Period");
				gMenu[EntityIndex].AddItem(5, "Toggle Ready System");

				gMenu[EntityIndex].Show(EntityIndex);
			}
			else if (ItemIndex == 4)
			{
				CLIENT_COMMAND(Player->edict(), "messagemode !msg\n");
			}
			else if (ItemIndex == 5)
			{
				CLIENT_COMMAND(Player->edict(), "messagemode !rcon\n");
			}
		}
		else if (gAdmin.GetType(EntityIndex) == 1)
		{
			auto Target = UTIL_PlayerByIndexSafe(ItemIndex);

			if (Target)
			{
				gUtil.ServerCommand("kick #%d \"Kicked by %s.\"", GETPLAYERUSERID(Target->edict()), STRING(Player->edict()->v.netname));

				gUtil.SayText(NULL, EntityIndex, "\3%s\1 Kicked \3%s\1", STRING(Player->edict()->v.netname), STRING(Target->edict()->v.netname));
			}
		}
		else if (gAdmin.GetType(EntityIndex) == 2)
		{
			auto Target = UTIL_PlayerByIndexSafe(ItemIndex);

			if (Target)
			{
				MDLL_ClientKill(Target->edict());

				gUtil.SayText(NULL, EntityIndex, "\3%s\1 Killed \3%s\1", STRING(Player->edict()->v.netname), STRING(Target->edict()->v.netname));
			}
		}
		else if (gAdmin.GetType(EntityIndex) == 3)
		{
			gTask.Create(EntityIndex, 5.0, false, SERVER_COMMAND, (void*)gUtil.VarArgs("changelevel %s\n", Option));

			gUtil.SayText(NULL, EntityIndex, "\3%s\1 changed map to \4%s\1", STRING(Player->edict()->v.netname), Option);
		}
		else if (gAdmin.GetType(EntityIndex) == 4)
		{
			int State = gPugMod.GetState();

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
