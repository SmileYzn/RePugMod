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

bool CAdmin::Check(CBasePlayer* Player)
{
	return this->Check(Player->edict());
}

void CAdmin::Menu(CBasePlayer* Player)
{
	if (this->Check(Player->edict()))
	{
		auto EntityIndex = Player->entindex();

		gMenu[EntityIndex].Create(_T("PUG Mod Menu:"), true, (void*)this->MenuHandle);

		gMenu[EntityIndex].AddItem(0, _T("Kick Player"));
		gMenu[EntityIndex].AddItem(1, _T("Slap Player"));
		gMenu[EntityIndex].AddItem(2, _T("Change Map"));
		gMenu[EntityIndex].AddItem(3, _T("Control Pug Mod"));
		gMenu[EntityIndex].AddItem(4, _T("Send Message"));
		gMenu[EntityIndex].AddItem(5, _T("Send Command"));

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
				gUtil.ClientCommand(Player->edict(), "messagemode !msg");
				break;
			}
			case 5:
			{
				gUtil.ClientCommand(Player->edict(), "messagemode !rcon");
				break;
			}
		}
	}
}

void CAdmin::MenuKick(int EntityIndex)
{
	gMenu[EntityIndex].Create(_T("Kick Player"), true, (void*)this->MenuKickHandle);

	CBasePlayer* Players[MAX_CLIENTS] = { NULL };

	auto Num = gPlayer.GetList(Players);

	for (int i = 0; i < Num; i++)
	{
		auto Player = Players[i];

		if (Player)
		{
			if (!gAdmin.Check(Player))
			{
				gMenu[EntityIndex].AddItem(Player->entindex(), STRING(Player->edict()->v.netname));
			}
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
			gPlayer.DropClient(Target->entindex(), _T("Kicked by %s."), STRING(Player->edict()->v.netname));

			gUtil.SayText(NULL, EntityIndex, _T("\3%s\1 Kicked \3%s\1"), STRING(Player->edict()->v.netname), STRING(Target->edict()->v.netname));
		}
	}
}

void CAdmin::MenuSlap(int EntityIndex)
{
	gMenu[EntityIndex].Create(_T("Slap Player"), true, (void*)this->MenuSlapHandle);

	CBasePlayer* Players[MAX_CLIENTS] = { NULL };

	auto Num = gPlayer.GetList(Players);

	for (int i = 0; i < Num; i++)
	{
		auto Player = Players[i];

		if (Player)
		{
			if (!gAdmin.Check(Player))
			{
				gMenu[EntityIndex].AddItem(Player->entindex(), STRING(Player->edict()->v.netname));
			}
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

			gUtil.SayText(NULL, EntityIndex, _T("\3%s\1 Killed \3%s\1"), STRING(Player->edict()->v.netname), STRING(Target->edict()->v.netname));
		}
	}
}

void CAdmin::MenuMap(int EntityIndex)
{
	gMenu[EntityIndex].Create(_T("Change Map"), true, (void*)this->MenuMapHandle);

	gMenu[EntityIndex].AddList(gUtil.LoadMapList(VOTE_MAP_FILE, true));

	gMenu[EntityIndex].Show(EntityIndex);
}

void CAdmin::MenuMapHandle(int EntityIndex, int ItemIndex, bool Disabled, const char* Option)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		gTask.Create(PUG_TASK_EXEC, 5.0f, false, gUtil.ServerChangelevel, (char*)Option);

		gUtil.SayText(NULL, EntityIndex, _T("\3%s\1 changed map to \4%s\1"), STRING(Player->edict()->v.netname), Option);
	}
}

void CAdmin::MenuControl(int EntityIndex)
{
	gMenu[EntityIndex].Create(_T("Control Pug Mod"), true, (void*)gAdmin.MenuControlHandle);

	int State = gPugMod.GetState();

	gMenu[EntityIndex].AddItem(0, _T("Run Vote Map"), (State == PUG_STATE_DEAD || State == PUG_STATE_START || State == PUG_STATE_END));

	gMenu[EntityIndex].AddItem(1, _T("Run Vote Teams"), (State != PUG_STATE_WARMUP));

	gMenu[EntityIndex].AddItem(2, (State == PUG_STATE_HALFTIME) ? _T("Continue Match") : _T("Start Match"), (State != PUG_STATE_WARMUP && State != PUG_STATE_HALFTIME));
	
	gMenu[EntityIndex].AddItem(3, _T("Stop Match"), (State == PUG_STATE_DEAD || State == PUG_STATE_WARMUP || State == PUG_STATE_START || State == PUG_STATE_END));

	gMenu[EntityIndex].AddItem(4, _T("Restart Period"), (State != PUG_STATE_FIRST_HALF && State != PUG_STATE_SECOND_HALF && State != PUG_STATE_OVERTIME));

	gMenu[EntityIndex].AddItem(5, _T("Toggle Ready System"), (State != PUG_STATE_WARMUP));

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

		if (Disabled)
		{
			gAdmin.MenuControl(EntityIndex);
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

			gUtil.SayText(NULL, Player->entindex(), _T("\3(%s)\1: %s"), STRING(Player->edict()->v.netname), Message.c_str());
		}
		else
		{
			gUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, _T("Usage: !msg \3<Text Message>\1"));
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

			gUtil.SayText(Player->edict(), Player->entindex(), _T("\3Command Send\1: %s"), Command.c_str());
		}
		else
		{
			gUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, _T("Usage: !send \3<Server Command>\1"));
		}
	}
}
