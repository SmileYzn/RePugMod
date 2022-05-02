#pragma once

class CReady
{
public:
	void Load(int PlayersMin, int ReadyType, float ReadyTime);
	void Unload();
	void Toggle(CBasePlayer* Player);
	static void List(CReady* Ready);
	void Ready(CBasePlayer* pPlayer);
	void NotReady(CBasePlayer* pPlayer);

private:
	bool m_Running;
	int m_PlayersMin;
	int m_Ready[33];
	int m_ReadyType;
	time_t m_ReadyTime;
	time_t m_SystemTime;
};

extern CReady gReady;