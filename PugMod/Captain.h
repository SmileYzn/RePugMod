#pragma once

class CCaptain
{
public:
	void Clear();
	void Init();
	void Stop();
	void ClientDisconnected(int EntityIndex);

	void SetCaptain(CBasePlayer* Player, TeamName Team);
	TeamName GetCaptain(int EntityIndex);
	void GetPlayer(CBasePlayer* Player, CBasePlayer* Target);
	bool CheckPlayerCount();
	bool GetPicking(int EntityIndex);
	int GetCaptainPicking();

	void Menu(TeamName Team);

	static void MenuHandle(int EntityIndex, P_MENU_ITEM Item);
	static void GetRandomPlayer();
	static void List();
private:
	int      m_Captain[SPECTATOR + 1] = { 0 };
	TeamName m_Picking = UNASSIGNED;
};

extern CCaptain gCaptain;
