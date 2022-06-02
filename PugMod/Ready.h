#pragma once

class CReady
{
public:
	void Load();
	void Unload();
	void Toggle(CBasePlayer* Player);

	void ClientGetIntoGame(CBasePlayer* Player);

	static void List(CReady* Ready);
	void Ready(CBasePlayer* pPlayer);
	void NotReady(CBasePlayer* pPlayer);

private:
	bool m_Running = false;
	int m_Ready[MAX_CLIENTS + 1] = { 0 };
	time_t m_SystemTime = 0;
};

extern CReady gReady;