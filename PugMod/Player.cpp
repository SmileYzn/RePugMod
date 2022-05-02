#include "precompiled.h"

CPlayer gPlayer;

void CPlayer::TeamInfo(edict_t *pEntity, int playerIndex, const char *pszTeamName)
{
	if (this->m_iMsgTeamInfo == NULL)
	{
		this->m_iMsgTeamInfo = GET_USER_MSG_ID(PLID, "TeamInfo", NULL);
	}

	MESSAGE_BEGIN(MSG_ONE, this->m_iMsgTeamInfo, nullptr, pEntity);
	WRITE_BYTE(playerIndex);
	WRITE_STRING(pszTeamName);
	MESSAGE_END();
}

int CPlayer::GetList(CBasePlayer* Players[32])
{
	int Num = 0;

	memset(Players, NULL, sizeof(Players));

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

int CPlayer::GetList(CBasePlayer* Players[32], TeamName Team)
{
	int Num = 0;

	memset(Players, NULL, sizeof(Players));

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

	if (g_pGameRules)
	{
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
	}

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

	CBasePlayer* Players[32] = { NULL };

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