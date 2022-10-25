#include "precompiled.h"

CReady gReady;

void CReady::Load()
{
	this->m_SystemTime = time(NULL);

	memset(this->m_Ready, 0, sizeof(this->m_Ready));

	this->m_Running = true;

	gTask.Create(PUG_TASK_LIST, 0.5f, true, (void*)this->List);

	if (gCvars.GetReadyType()->value)
	{
		gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, _T("Say \4.ready\1 to continue."));
	}
	else
	{
		gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, _T("Match will start when all players join in game."));
	}
}

void CReady::Unload()
{
	this->m_Running = false;

	gTask.Remove(PUG_TASK_LIST);
}

void CReady::Toggle(CBasePlayer* Player)
{
	if (gPugMod.GetState() == PUG_STATE_WARMUP)
	{
		if (!this->m_Running)
		{
			gUtil.SayText(Player->edict(), Player->entindex(), _T("\3%s\1 enabled Ready System."),STRING(Player->edict()->v.netname));

			this->Load();
		}
		else
		{
			gUtil.SayText(Player->edict(), Player->entindex(), _T("\3%s\1 disabled Ready System."), STRING(Player->edict()->v.netname));

			this->Unload();
		}
	}
	else
	{
		gUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, _T("Can't change Ready System while Pug Mod is running."));
	}
}

void CReady::ClientGetIntoGame(CBasePlayer* Player)
{
	if (this->m_Running)
	{
		if (Player)
		{
			this->m_Ready[Player->entindex()] = 0;
		}
	}
}

void CReady::ClientDisconnected(int EntityIndex)
{
	this->m_Ready[EntityIndex] = 0;
}

void CReady::List()
{
	if (gCvars.GetReadyType()->value)
	{
		std::string PlayerList[2];

		int PlayerCount[2] = { 0 };

		CBasePlayer* Players[MAX_CLIENTS] = { NULL };

		auto Num = gPlayer.GetList(Players, true);

		for (int i = 0; i < Num; i++)
		{
			auto Player = Players[i];

			if (Player)
			{
				int State = gReady.GetReady(Player->entindex()) ? 1 : 0;

				PlayerCount[State]++;

				PlayerList[State].append(STRING(Player->edict()->v.netname));

				PlayerList[State].append("\n");
			}
		}

		if (PlayerCount[1] >= (int)gCvars.GetPlayersMin()->value)
		{
			gReady.Unload();

			gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, _T("All playres are ready!"));

			gPugMod.NextState(0);
		}
		else
		{
			gUtil.HudMessage(NULL, gUtil.HudParam(0, 255, 0, 0.23, 0.02, 0, 0.0, 0.53, 0.0, 0.0, 1), _T("Not Ready (%d of %d):"), PlayerCount[0], (int)gCvars.GetPlayersMin()->value);

			gUtil.HudMessage(NULL, gUtil.HudParam(0, 255, 0, 0.58, 0.02, 0, 0.0, 0.53, 0.0, 0.0, 2), _T("Ready (%d of %d):"), PlayerCount[1], (int)gCvars.GetPlayersMin()->value);

			gUtil.HudMessage(NULL, gUtil.HudParam(255, 255, 225, 0.23, 0.02, 0, 0.0, 0.53, 0.0, 0.0, 4), "\n%s", PlayerList[0].c_str());

			gUtil.HudMessage(NULL, gUtil.HudParam(255, 255, 225, 0.58, 0.02, 0, 0.0, 0.53, 0.0, 0.0, 3), "\n%s", PlayerList[1].c_str());
		}
	}
	else
	{
		int Needed = ((int)gCvars.GetPlayersMin()->value - gPlayer.GetNum());

		time_t CurrentTime = time(NULL);

		if (Needed)
		{
			gReady.SetSystemTime(CurrentTime);

			gUtil.HudMessage(NULL, gUtil.HudParam(0, 255, 0, -1.0, 0.2, 0, 0.53, 0.53), "%s\n%d %s", _T("Warmup"), Needed, _T("Player(s) Left"));
		}
		else
		{
			time_t RemainTime = (time_t)((time_t)gCvars.GetReadyTime()->value - (CurrentTime - gReady.GetSystemTime()));

			if (RemainTime > 0)
			{
				struct tm* tm_info = localtime(&RemainTime);

				if (tm_info)
				{
					char Time[32] = { 0 };

					strftime(Time, sizeof(Time), _T("Starting Match\n%M:%S"), tm_info);

					gUtil.HudMessage(NULL, gUtil.HudParam(0, 255, 0, -1.0, 0.2, 0, 0.53, 0.53), Time);
				}
			}
			else
			{
				gReady.Unload();

				gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, _T("All playres are in teams and ready!"));

				gPugMod.NextState(0);
			}
		}
	}
}

void CReady::Ready(CBasePlayer* Player)
{
	if (this->m_Running)
	{
		auto EntityIndex = Player->entindex();

		if (!this->m_Ready[EntityIndex] && gCvars.GetReadyType()->value)
		{
			if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
			{
				this->m_Ready[EntityIndex] = 1;

				gUtil.SayText(NULL, EntityIndex, _T("\3%s\1 is ready."), STRING(Player->edict()->v.netname));
			}
		}
		else
		{
			gUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, _T("Unable to use this command now."));
		}
	}
}

void CReady::NotReady(CBasePlayer* Player)
{
	if (this->m_Running)
	{
		auto EntityIndex = Player->entindex();

		if (this->m_Ready[EntityIndex] && gCvars.GetReadyType()->value)
		{
			if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
			{
				this->m_Ready[EntityIndex] = 0;

				gUtil.SayText(NULL, EntityIndex, _T("\3%s\1 is not ready."), STRING(Player->edict()->v.netname));
			}
		}
		else
		{
			gUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, _T("Unable to use this command now."));
		}
	}
}

bool CReady::GetReady(int EntityIndex)
{
	return this->m_Ready[EntityIndex] ? true : false;
}

time_t CReady::GetSystemTime()
{
	return this->m_SystemTime;
}

void CReady::SetSystemTime(time_t Time)
{
	this->m_SystemTime = Time;
}
