#include "precompiled.h"

CReady gReady;

void CReady::Load()
{
	this->m_SystemTime = time(NULL);

	memset(this->m_Ready, 0, sizeof(this->m_Ready));

	this->m_Running = true;

	gTask.Create(PUG_TASK_LIST, 0.5f, true, (void*)this->List, this);

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

void CReady::List(CReady* Ready)
{
	if (gCvars.GetReadyType()->value)
	{
		char PlayerList[2][512] = { 0 };

		int  PlayerCount[2] = { 0 };

		CBasePlayer* Players[32] = { NULL };

		auto Num = gPlayer.GetList(Players);

		for (int i = 0; i < Num; i++)
		{
			auto Player = Players[i];

			if (Player)
			{
				int State = Ready->m_Ready[Player->entindex()] ? 1 : 0;

				PlayerCount[State]++;

				snprintf(PlayerList[State], sizeof(PlayerList[State]), "%s%s\n", PlayerList[State], STRING(Player->edict()->v.netname));
			}
		}

		if (PlayerCount[1] >= (int)gCvars.GetPlayersMin()->value)
		{
			Ready->Unload();

			gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, _T("All playres are ready!"));

			gPugMod.NextState(0);
		}
		else
		{
			gUtil.HudMessage(NULL, gUtil.HudParam(0, 255, 0, 0.23, 0.02, 0, 0.0, 0.53, 0.0, 0.0, 1), _T("Not Ready (%d of %d):"), PlayerCount[0], (int)gCvars.GetPlayersMin()->value);

			gUtil.HudMessage(NULL, gUtil.HudParam(0, 255, 0, 0.58, 0.02, 0, 0.0, 0.53, 0.0, 0.0, 2), _T("Ready (%d of %d):"), PlayerCount[1], (int)gCvars.GetPlayersMin()->value);

			gUtil.HudMessage(NULL, gUtil.HudParam(255, 255, 225, 0.23, 0.02, 0, 0.0, 0.53, 0.0, 0.0, 4), "\n%s", PlayerList[0]);

			gUtil.HudMessage(NULL, gUtil.HudParam(255, 255, 225, 0.58, 0.02, 0, 0.0, 0.53, 0.0, 0.0, 3), "\n%s", PlayerList[1]);
		}
	}
	else
	{
		int Needed = ((int)gCvars.GetPlayersMin()->value - gPlayer.GetNum());

		if (Needed > 0)
		{
			Ready->m_SystemTime = time(NULL);

			gUtil.HudMessage(NULL, gUtil.HudParam(0, 255, 0, -1.0, 0.2, 0, 0.53, 0.53), _T("Warmup\n%d Player(s) Left"), Needed);
		}
		else
		{
			time_t RemainTime = (time_t)((int)gCvars.GetReadyTime()->value - (time(NULL) - Ready->m_SystemTime));

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
				Ready->Unload();

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

		if (!this->m_Ready[EntityIndex])
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

		if (this->m_Ready[EntityIndex])
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
