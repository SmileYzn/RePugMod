#include "precompiled.h"

CAntiFlood gAntiFlood;

bool CAntiFlood::ClientCommand(CBasePlayer* Player, const char* pcmd, const char* parg1)
{
	if (_stricmp(pcmd, "say") == 0 || _stricmp(pcmd, "say_team") == 0)
	{
		float MaxChat = gCvars.GetFloodTime()->value;

		if (MaxChat)
		{
			float NexTime = gpGlobals->time;

			int EntityIndex = Player->entindex();

			if (this->m_Flooding[EntityIndex] > NexTime)
			{
				if (this->m_Flood[EntityIndex] >= MAX_FLOOD_REPEAT)
				{
					this->m_Flooding[EntityIndex] = (NexTime + MaxChat + MIN_FLOOD_NEXT_TIME);

					return true;
				}

				this->m_Flood[EntityIndex]++;
			}
			else if (this->m_Flood[EntityIndex])
			{
				this->m_Flood[EntityIndex]--;
			}

			this->m_Flooding[EntityIndex] = NexTime + MaxChat;
		}
	}

	return false;
}
