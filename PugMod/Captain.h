#pragma once

class CCaptain
{
public:
	void Clear();
	void Init();
	void Stop();
	void ClientDisconnected(edict_t* pEntity);

	void SetCaptain(CBasePlayer* Player, TeamName Team);
	TeamName GetCaptain(int EntityIndex);
	void GetPlayer(CBasePlayer* Player, CBasePlayer* Target);
	bool CheckPlayerCount();
	bool GetPicking(int EntityIndex);

	void Menu(TeamName Team);

	static void MenuHandle(int EntityIndex, int ItemIndex, bool Disabled, const char* Option);
	static void GetRandomPlayer(int EntityIndex);
	static void List();
private:
	int      m_Captain[SPECTATOR + 1];
	TeamName m_Picking;
};

extern CCaptain gCaptain;
