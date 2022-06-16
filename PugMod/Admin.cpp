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

bool CAdmin::Check(int EntityIndex)
{
	auto pEntity = INDEXENT(EntityIndex);

	if (!FNullEnt(pEntity))
	{
		return this->Check(pEntity);
	}

	return false;
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

bool CAdmin::Check(const char* Auth)
{
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

		gMenu[EntityIndex].Create(_T("PUG Mod Menu:"), true, (void*)this->MenuHandle);

		gMenu[EntityIndex].AddItem(0, _T("Kick Player"));

		gMenu[EntityIndex].AddItem(1, _T("Ban Player"));

		gMenu[EntityIndex].AddItem(2, _T("Slap Player"));

		gMenu[EntityIndex].AddItem(3, _T("Team Player"));

		gMenu[EntityIndex].AddItem(4, _T("Change Map"));

		gMenu[EntityIndex].AddItem(5, _T("Control Pug Mod"));

		gMenu[EntityIndex].AddItem(6, _T("Send Message"));

		gMenu[EntityIndex].AddItem(7, _T("Send Command"));

		gMenu[EntityIndex].AddItem(8, _T("Swap Teams"));

		gMenu[EntityIndex].Show(EntityIndex);
	}
}

void CAdmin::MenuHandle(int EntityIndex, P_MENU_ITEM Item)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		switch (Item.Info)
		{
			case 0:
			{
				gAdmin.MenuKick(EntityIndex);
				break;
			}
			case 1:
			{
				gAdmin.MenuBan(EntityIndex);
				break;
			}
			case 2:
			{
				gAdmin.MenuSlap(EntityIndex);
				break;
			}
			case 3:
			{
				gAdmin.MenuTeam(EntityIndex);
				break;
			}
			case 4:
			{
				gAdmin.MenuMap(EntityIndex);
				break;
			}
			case 5:
			{
				gAdmin.MenuControl(EntityIndex);
				break;
			}
			case 6:
			{
				gUtil.ClientCommand(Player->edict(), "messagemode !msg");
				break;
			}
			case 7:
			{
				gUtil.ClientCommand(Player->edict(), "messagemode !rcon");
				break;
			}
			case 8:
			{
				gAdmin.SwapTeams(Player);
				break;
			}
		}
	}
}

void CAdmin::MenuKick(int EntityIndex)
{
	if (this->Check(EntityIndex))
	{
		gMenu[EntityIndex].Create(_T("Kick Player"), true, (void*)this->MenuKickHandle);

		CBasePlayer* Players[MAX_CLIENTS] = { NULL };

		auto Num = gPlayer.GetList(Players, false);

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
}

void CAdmin::MenuKickHandle(int EntityIndex, P_MENU_ITEM Item)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		auto Target = UTIL_PlayerByIndexSafe(Item.Info);

		if (Target)
		{
			gUtil.SayText(NULL, Player->entindex(), _T("\3%s\1 Kicked \3%s\1"), STRING(Player->edict()->v.netname), STRING(Target->edict()->v.netname));

			gPlayer.DropClient(Target->entindex(), _T("Kicked by %s"), STRING(Player->edict()->v.netname));
		}
	}
}

void CAdmin::MenuBan(int EntityIndex)
{
	if (this->Check(EntityIndex))
	{
		gMenu[EntityIndex].Create(_T("Ban Player"), true, (void*)this->MenuBanHandle);

		CBasePlayer* Players[MAX_CLIENTS] = { NULL };

		auto Num = gPlayer.GetList(Players, false);

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
}

void CAdmin::MenuBanHandle(int EntityIndex, P_MENU_ITEM Item)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		auto Target = UTIL_PlayerByIndexSafe(Item.Info);

		if (Target)
		{
			char Title[128] = { 0 };

			sprintf(Title, _T("Choose a time to ban \\w%s\\y"), STRING(Target->edict()->v.netname));

			gMenu[EntityIndex].Create(Title, true, (void*)gAdmin.MenuBanHandleExtra);

			std::string BanTimes("0 5 10 15 30 45 60 120");

			if (gCvars.GetBanTimes()->string)
			{
				BanTimes = gCvars.GetBanTimes()->string;
			}

			std::stringstream BanStream(BanTimes);

			std::vector<int> BanTimeList;

			std::copy(std::istream_iterator<int>(BanStream), std::istream_iterator<int>(), std::back_inserter(BanTimeList));

			for (auto const& Time : BanTimeList)
			{
				gMenu[EntityIndex].AddItem(Target->entindex(), gTimeFormat.GetTimeLength(Time, TimeUnitType::TIMEUNIT_MINUTES), false, Time);
			}
		}
	}

	gMenu[EntityIndex].Show(EntityIndex);
}

void CAdmin::MenuBanHandleExtra(int EntityIndex, P_MENU_ITEM Item)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		auto Target = UTIL_PlayerByIndexSafe(Item.Info);

		if (Target)
		{
			gUtil.SayText(NULL, Player->entindex(), _T("\3%s\1 Banned \3%s\1: \4%s\1."), STRING(Player->edict()->v.netname), STRING(Target->edict()->v.netname), Item.Text.c_str());

			gUtil.ServerCommand("banid %d #%d kick;writeid", Item.Extra, GETPLAYERUSERID(Target->edict()));
		}
	}
}

void CAdmin::MenuSlap(int EntityIndex)
{
	if (this->Check(EntityIndex))
	{
		gMenu[EntityIndex].Create(_T("Slap Player"), true, (void*)this->MenuSlapHandle);

		CBasePlayer* Players[MAX_CLIENTS] = { NULL };

		auto Num = gPlayer.GetList(Players, false);

		for (int i = 0; i < Num; i++)
		{
			auto Player = Players[i];

			if (Player)
			{
				if (!gAdmin.Check(Player))
				{
					gMenu[EntityIndex].AddItem(Player->entindex(), STRING(Player->edict()->v.netname), !Player->IsAlive());
				}
			}
		}

		gMenu[EntityIndex].Show(EntityIndex);
	}
}

void CAdmin::MenuSlapHandle(int EntityIndex, P_MENU_ITEM Item)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		auto Target = UTIL_PlayerByIndexSafe(Item.Info);

		if (Target)
		{
			gUtil.SayText(NULL, EntityIndex, _T("\3%s\1 Killed \3%s\1"), STRING(Player->edict()->v.netname), STRING(Target->edict()->v.netname));

			MDLL_ClientKill(Target->edict());
		}
	}
}

void CAdmin::MenuTeam(int EntityIndex)
{
	if (this->Check(EntityIndex))
	{
		gMenu[EntityIndex].Create(_T("Team Player"), true, (void*)this->MenuTeamHandle);

		CBasePlayer* Players[MAX_CLIENTS] = { NULL };

		auto Num = gPlayer.GetList(Players, false);

		for (int i = 0; i < Num; i++)
		{
			auto Player = Players[i];

			if (Player)
			{
				if (Player->m_iTeam != UNASSIGNED)
				{
					if (!gAdmin.Check(Player))
					{
						gMenu[EntityIndex].AddItem(Player->entindex(), STRING(Player->edict()->v.netname));
					}
				}
			}
		}

		gMenu[EntityIndex].Show(EntityIndex);
	}
}

void CAdmin::MenuTeamHandle(int EntityIndex, P_MENU_ITEM Item)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		auto Target = UTIL_PlayerByIndexSafe(Item.Info);

		if (Target)
		{
			char Title[128] = { 0 };

			sprintf(Title, _T("Set \\w%s\\y Team"), STRING(Target->edict()->v.netname));

			gMenu[EntityIndex].Create(Title, true, (void*)gAdmin.MenuTeamHandleExtra);

			gMenu[EntityIndex].AddItem(Target->entindex(), _T("Terrorist"), Target->m_iTeam == TERRORIST, TERRORIST);

			gMenu[EntityIndex].AddItem(Target->entindex(), _T("Counter-Terrorist"), Target->m_iTeam == CT, CT);

			gMenu[EntityIndex].AddItem(Target->entindex(), _T("Spectator"), Target->m_iTeam == SPECTATOR, SPECTATOR);
		}
	}

	gMenu[EntityIndex].Show(EntityIndex);
}

void CAdmin::MenuTeamHandleExtra(int EntityIndex, P_MENU_ITEM Item)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		auto Target = UTIL_PlayerByIndexSafe(Item.Info);

		if (Target)
		{
			Target->CSPlayer()->JoinTeam((TeamName)Item.Extra);

			if (Item.Extra == SPECTATOR)
			{
				if (Target->IsAlive())
				{
					MDLL_ClientKill(Target->edict());
				}
			}

			gUtil.SayText(NULL, Target->entindex(), _T("\4%s\1 moved \3%s\1 to \3%s\1"), STRING(Player->edict()->v.netname), STRING(Target->edict()->v.netname), Item.Text.c_str());
		}
	}
}

void CAdmin::MenuMap(int EntityIndex)
{
	if (this->Check(EntityIndex))
	{
		gMenu[EntityIndex].Create(_T("Change Map"), true, (void*)this->MenuMapHandle);

		gMenu[EntityIndex].AddList(gUtil.LoadMapList(VOTE_MAP_FILE, true));

		gMenu[EntityIndex].Show(EntityIndex);
	}
}

void CAdmin::MenuMapHandle(int EntityIndex, P_MENU_ITEM Item)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		gTask.Create(PUG_TASK_EXEC, 5.0f, false, (void*)SERVER_COMMAND, gUtil.VarArgs("changelevel %s\n", Item.Text.c_str()));

		gUtil.SayText(NULL, EntityIndex, _T("\3%s\1 changed map to \4%s\1"), STRING(Player->edict()->v.netname), Item.Text.c_str());
	}
}

void CAdmin::MenuControl(int EntityIndex)
{
	if (this->Check(EntityIndex))
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
}

void CAdmin::MenuControlHandle(int EntityIndex, P_MENU_ITEM Item)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		switch (Item.Info)
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

void CAdmin::SwapTeams(CBasePlayer* Player)
{
	if (this->Check(Player->edict()))
	{
		if (gPugMod.GetState() == PUG_STATE_WARMUP)
		{
			if (g_pGameRules)
			{
				CSGameRules()->SwapAllPlayers();

				CSGameRules()->RestartRound();
			}

			gUtil.SayText(NULL, Player->entindex(), _T("\3%s\1 change team sides manually."), STRING(Player->edict()->v.netname));
		}
		else
		{
			gUtil.SayText(Player->edict(), Player->entindex(), _T("Can only swap teams during \4%s\1 period."), PUG_MOD_STATES_STR[PUG_STATE_WARMUP]);
		}
	}
}
