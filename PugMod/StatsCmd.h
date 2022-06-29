#pragma once

class CStatsCmd
{
public:
	bool HP(CBasePlayer* Player);
	bool Damage(CBasePlayer* Player);
	bool Received(CBasePlayer* Player);
	bool Summary(CBasePlayer* Player);
};

extern CStatsCmd gStatsCmd;