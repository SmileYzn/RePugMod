#pragma once

#define MAX_FLOOD_REPEAT		3
#define MIN_FLOOD_NEXT_TIME		3.0

class CAntiFlood
{
public:
	bool ClientCommand(CBasePlayer* Player, const char* pcmd, const char* parg1);

private:
	int   m_Flood[33];
	float m_Flooding[33];
};

extern CAntiFlood gAntiFlood;