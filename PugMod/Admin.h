#pragma once

#define ADMIN_CONFIG_FILE "cstrike/addons/pugmod/users.txt"

class CAdmin
{
public:
	void Load();
	bool Check(edict_t* pEntity);
	bool Check(CBasePlayer* Player);
	bool Check(const char* Auth);

	void Menu(CBasePlayer* Player);
	static void MenuHandle(int EntityIndex, int ItemIndex, bool Disabled, const char* Option);

	void MenuKick(int EntityIndex);
	static void MenuKickHandle(int EntityIndex, int ItemIndex, bool Disabled, const char* Option);

	void MenuSlap(int EntityIndex);
	static void MenuSlapHandle(int EntityIndex, int ItemIndex, bool Disabled, const char* Option);

	void MenuMap(int EntityIndex);
	static void MenuMapHandle(int EntityIndex, int ItemIndex, bool Disabled, const char* Option);

	void MenuControl(int EntityIndex);
	static void MenuControlHandle(int EntityIndex, int ItemIndex, bool Disabled, const char* Option);

	void Chat(CBasePlayer* Player, const char* Args);
	void Rcon(CBasePlayer* Player, const char* Args);

private:
	std::map<std::string, std::string> m_Data;
};

extern CAdmin gAdmin;
