#include "precompiled.h"

CPlayer gPlayer;

void CPlayer::TeamInfo(edict_t* pEntity, int playerIndex, const char *pszTeamName)
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

int CPlayer::GetList(CBasePlayer* Players[MAX_CLIENTS])
{
	int Num = 0;

	memset(Players, 0, MAX_CLIENTS);

	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		auto Player = UTIL_PlayerByIndexSafe(i);

		if (Player)
		{
			if (!FNullEnt(Player->edict()))
			{
				if (!Player->IsDormant())
				{
					if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
					{
						Players[Num++] = Player;
					}
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
			if (!FNullEnt(Player->edict()))
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
	}

	return Num;
}

int CPlayer::GetNum(bool CountBots, int & InGame, int & NumTerrorist, int & NumCT, int & NumSpectator)
{
	InGame, NumTerrorist = NumCT = NumSpectator = 0;

	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		auto Player = UTIL_PlayerByIndexSafe(i);

		if (Player)
		{
			if (!FNullEnt(Player->edict()))
			{
				if (!Player->IsDormant())
				{
					if (CountBots == false)
					{
						if (Player->IsBot())
						{
							continue;
						}
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
		Num = this->GetList(Players);
	}

	if (Num)
	{
		return Players[RANDOM_LONG(0, Num - 1)];
	}

	return NULL;
}

bool CPlayer::DropClient(int EntityIndex, const char* Format, ...)
{
	auto Gameclient = g_RehldsSvs->GetClient(EntityIndex - 1);

	if (Gameclient)
	{
		va_list argptr;

		static char Buffer[128] = { 0 };

		va_start(argptr, Format);

		vsprintf(Buffer, Format, argptr);

		va_end(argptr);

		g_RehldsFuncs->DropClient(Gameclient, false, Buffer);

		return true;
	}

	return false;
}
