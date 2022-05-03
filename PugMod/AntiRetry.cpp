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
					gUtil.ServerCommand("kick #%d \"Wait %d seconds before retry.\"", GETPLAYERUSERID(pEntity), Seconds);
				}
			}
		}
	}
}

void CAntiRetry::ClientDisconnected(edict_t* pEntity, bool crash, const char* Reason)
{
	if (pEntity)
	{
		if (gCvars.GetReconnectDelay()->value > 0)
		{
			if (FStrEq(Reason, "Client sent 'drop'") || FStrEq(Reason, "Timed out"))
			{
				const char* Auth = GETPLAYERAUTHID(pEntity);

				if (Auth)
				{
					this->m_Data.erase(Auth);

					this->m_Data.insert(std::make_pair(Auth, time(NULL) + (int)gCvars.GetReconnectDelay()->value));
				}
			}
		}
	}
}
