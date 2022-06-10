#pragma once

class CCommands
{
public:
	bool ClientCommand(CBasePlayer* Player, const char* pcmd, const char* parg1);
};

extern CCommands gCommands;