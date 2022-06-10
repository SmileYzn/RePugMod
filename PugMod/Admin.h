#pragma once

#define ADMIN_CONFIG_FILE "cstrike/addons/pugmod/users.txt"

class CAdmin
{
public:
	void Load();
	bool Check(int EntityIndex);
	bool Check(edict_t* pEntity);
	bool Check(CBasePlayer* Player);
	bool Check(const char* Auth);

	void Menu(CBasePlayer* Player);
	static void MenuHandle(int EntityIndex, P_MENU_ITEM Item);

	void MenuKick(int EntityIndex);
	static void MenuKickHandle(int EntityIndex, P_MENU_ITEM Item);

	void MenuBan(int EntityIndex);
	static void MenuBanHandle(int EntityIndex, P_MENU_ITEM Item);
	static void MenuBanHandleExtra(int EntityIndex, P_MENU_ITEM Item);

	void MenuSlap(int EntityIndex);
	static void MenuSlapHandle(int EntityIndex, P_MENU_ITEM Item);

	void MenuTeam(int EntityIndex);
	static void MenuTeamHandle(int EntityIndex, P_MENU_ITEM Item);
	static void MenuTeamHandleExtra(int EntityIndex, P_MENU_ITEM Item);

	void MenuMap(int EntityIndex);
	static void MenuMapHandle(int EntityIndex, P_MENU_ITEM Item);

	void MenuControl(int EntityIndex);
	static void MenuControlHandle(int EntityIndex, P_MENU_ITEM Item);

	void Chat(CBasePlayer* Player, const char* Args);
	void Rcon(CBasePlayer* Player, const char* Args);

private:
	std::map<std::string, std::string> m_Data;
};

extern CAdmin gAdmin;
