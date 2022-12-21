#include "precompiled.h"

CPlayer gPlayer;

void CPlayer::TeamInfo(edict_t* pEntity, int playerIndex, const char* pszTeamName)
{
	static int iMsgTeamInfo;

	if (iMsgTeamInfo || (iMsgTeamInfo = GET_USER_MSG_ID(PLID, "TeamInfo", NULL)))
	{
		MESSAGE_BEGIN(MSG_ONE, iMsgTeamInfo, nullptr, pEntity);
		WRITE_BYTE(playerIndex);
		WRITE_STRING(pszTeamName);
		MESSAGE_END();
	}
}

int CPlayer::GetList(CBasePlayer* Players[MAX_CLIENTS], bool InGameOnly)
{
	int Num = 0;

	memset(Players, 0, MAX_CLIENTS);

	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		auto Player = UTIL_PlayerByIndexSafe(i);

		if (Player)
		{
			if (!Player->IsDormant())
			{
				if (InGameOnly)
				{
					if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
					{
						Players[Num++] = Player;
					}
				}
				else
				{
					Players[Num++] = Player;
				}
			}
		}
	}

	return Num;
}

int CPlayer::GetList(CBasePlayer* Players[MAX_CLIENTS], TeamName Team)
{
	int Num = 0;

	memset(Players, 0, MAX_CLIENTS);

	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		auto Player = UTIL_PlayerByIndexSafe(i);

		if (Player)
		{
			if (!Player->IsDormant())
			{
				if (Player->m_iTeam == Team)
				{
					Players[Num++] = Player;
				}
			}
		}
	}

	return Num;
}

int CPlayer::GetNum(bool CountBots, int& InGame, int& NumTerrorist, int& NumCT, int& NumSpectator)
{
	InGame = 0;
	NumTerrorist = 0;
	NumCT = 0;
	NumSpectator = 0;

	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		auto Player = UTIL_PlayerByIndexSafe(i);

		if (Player)
		{
			if (!Player->IsDormant())
			{
				if (!CountBots && Player->IsBot())
				{
					continue;
				}

				switch (Player->m_iTeam)
				{
					case TERRORIST:
					{
						NumTerrorist++;
						break;
					}
					case CT:
					{
						NumCT++;
						break;
					}
					case SPECTATOR:
					{
						NumSpectator++;
						break;
					}
				}
			}
		}
	}

	InGame = (NumTerrorist + NumCT);

	return (NumTerrorist + NumCT + NumSpectator);
}

int CPlayer::GetNum()
{
	int InGame, NumTerrorist, NumCT, NumSpectator;

	this->GetNum(true, InGame, NumTerrorist, NumCT, NumSpectator);

	return InGame;
}

int CPlayer::GetNum(TeamName Team)
{
	int InGame, NumTerrorist, NumCT, NumSpectator;

	this->GetNum(true, InGame, NumTerrorist, NumCT, NumSpectator);

	switch (Team)
	{
		case TERRORIST:
		{
			return NumTerrorist;
		}
		case CT:
		{
			return NumCT;
		}
		case SPECTATOR:
		{
			return NumSpectator;
		}
	}

	return InGame;
}

int CPlayer::GetNum(bool CountBots)
{
	int InGame, NumTerrorist, NumCT, NumSpectator;

	this->GetNum(CountBots, InGame, NumTerrorist, NumCT, NumSpectator);

	return InGame;
}

int CPlayer::GetNum(bool CountBots, TeamName Team)
{
	int InGame, NumTerrorist, NumCT, NumSpectator;

	this->GetNum(CountBots, InGame, NumTerrorist, NumCT, NumSpectator);

	switch (Team)
	{
		case TERRORIST:
		{
			return NumTerrorist;
		}
		case CT:
		{
			return NumCT;
		}
		case SPECTATOR:
		{
			return NumSpectator;
		}
	}

	return InGame;
}

int CPlayer::GetNum(int& NumAliveTerrorists, int& NumAliveCT)
{
	NumAliveTerrorists, NumAliveCT = 0;

	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		auto Player = UTIL_PlayerByIndexSafe(i);

		if (Player)
		{
			if (!Player->IsDormant())
			{
				if (Player->IsAlive())
				{
					if (Player->m_iTeam == TERRORIST)
					{
						NumAliveTerrorists++;
					}
					else if (Player->m_iTeam == CT)
					{
						NumAliveCT++;
					}
				}
			}
		}
	}

	return (NumAliveTerrorists + NumAliveCT);
}

CBasePlayer* CPlayer::GetRandom(TeamName Team)
{
	int Num = 0;

	CBasePlayer* Players[MAX_CLIENTS] = { NULL };

	if (Team != UNASSIGNED)
	{
		Num = this->GetList(Players, Team);
	}
	else
	{
		Num = this->GetList(Players, true);
	}

	if (Num)
	{
		return Players[RANDOM_LONG(0, Num - 1)];
	}

	return NULL;
}

void CPlayer::DropClient(int EntityIndex, const char* Format, ...)
{
	auto Gameclient = g_RehldsSvs->GetClient(EntityIndex - 1);

	if (Gameclient)
	{
		va_list argList;

		va_start(argList, Format);

		char Buffer[255] = { 0 };

		vsnprintf(Buffer, sizeof(Buffer), Format, argList);

		va_end(argList);

		if (g_RehldsFuncs)
		{
			g_RehldsFuncs->DropClient(Gameclient, false, "%s", Buffer);
		}
		else
		{
			auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

			if (Player)
			{
				int UserIndex = GETPLAYERUSERID(Player->edict());

				if (!FNullEnt(Player->edict()) && UserIndex > 0)
				{
					if (strlen(Buffer) > 0)
					{
						gUtil.ServerCommand("kick #%d %s", UserIndex, Buffer);
					}
					else
					{
						gUtil.ServerCommand("kick #%d", UserIndex);
					}
				}

			}
		}
	}
}

void CPlayer::BanClient(int EntityIndex, int Time, bool Kick)
{
	auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

	if (Player)
	{
		int UserIndex = GETPLAYERUSERID(Player->edict());

		if (!FNullEnt(Player->edict()) && UserIndex > 0)
		{
			if (Kick)
			{
				gUtil.ServerCommand("banid %d #%d kick", Time, UserIndex);
			}
			else
			{
				gUtil.ServerCommand("banid %d #%d", Time, UserIndex);
			}

			gUtil.ServerCommand("writeid;writeip");
		}
	}
}
