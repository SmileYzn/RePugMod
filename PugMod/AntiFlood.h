#pragma once

#define MAX_FLOOD_REPEAT		3
#define MIN_FLOOD_NEXT_TIME		3.0

class CAntiFlood
{
public:
	bool ClientCommand(CBasePlayer* Player, const char* pcmd, const char* parg1);

private:
	int   m_Flood[MAX_CLIENTS+1] = { 0 };
	float m_Flooding[MAX_CLIENTS+1] = { 0 };
};

extern CAntiFlood gAntiFlood;