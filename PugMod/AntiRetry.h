#pragma once

class CAntiRetry
{
public:
	void ClientConnected(edict_t* pEntity);
	void ClientDisconnected(edict_t* pEntity, bool crash, const char* Reason);

private:
	std::map<std::string, time_t> m_Data;
};

extern CAntiRetry gAntiRetry;

