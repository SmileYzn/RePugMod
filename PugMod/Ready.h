#pragma once

class CReady
{
public:
	void Load();
	void Unload();
	void Toggle(CBasePlayer* Player);
	static void List(CReady* Ready);
	void Ready(CBasePlayer* pPlayer);
	void NotReady(CBasePlayer* pPlayer);

private:
	bool m_Running;
	int m_Ready[33];
	time_t m_SystemTime;
};

extern CReady gReady;