#pragma once

class CAuth
{
public:
	void ClientConnected(edict_t* pEdict);

	static void RequestCallback(CURL* ch, size_t Size, const char* Memory, int EntityIndex);
};

extern CAuth gAuth;