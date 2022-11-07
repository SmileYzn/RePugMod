#pragma once

class CWebApi
{
public:
	void ClientConnected(edict_t* pEntity);

	static void RequestCallback(CURL* ch, size_t Size, const char* Result, int EntityIndex);
};

extern CWebApi gWebapi;