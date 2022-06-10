#include "precompiled.h"

CAntiRetry gAntiRetry;

void CAntiRetry::ClientConnected(edict_t * pEntity)
{
	if (pEntity)
	{
		const char* Auth = GETPLAYERAUTHID(pEntity);

		if (Auth)
		{
			auto it = this->m_Data.find(Auth);

			if (it != this->m_Data.end())
			{
				int Seconds = (it->second - time(NULL));

				if (Seconds > 0)
				{
					gPlayer.DropClient(ENTINDEX(pEntity), _T("Wait %d seconds before retry."), Seconds);
				}
			}
		}
	}
}

void CAntiRetry::ClientDisconnected(edict_t* pEntity, const char* Reason)
{
	if (FStrEq(Reason, "Client sent 'drop'") || FStrEq(Reason, "Timed out"))
	{
		if (gCvars.GetReconnectDelay()->value > 0)
		{
			const char* Auth = GETPLAYERAUTHID(pEntity);

			if (!gAdmin.Check(Auth))
			{
				auto Player = UTIL_PlayerByIndexSafe(ENTINDEX(pEntity));

				if (Player)
				{
					if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
					{
						if (gCvars.GetReconnectBanTime()->value > 0)
						{
							if (gPugMod.GetState() >= PUG_STATE_FIRST_HALF && gPugMod.GetState() <= PUG_STATE_OVERTIME)
							{
								gPlayer.BanClient(Player->entindex(), (int)gCvars.GetReconnectBanTime()->value, false);
							}
						}
						else
						{
							this->m_Data.erase(Auth);

							this->m_Data.insert(std::make_pair(Auth, time(NULL) + (int)gCvars.GetReconnectDelay()->value));
						}
					}
				}
			}
		}
	}
}
