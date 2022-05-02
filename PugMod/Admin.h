#pragma once

#define ADMIN_CONFIG_FILE "cstrike/addons/pugmod/users.txt"

class CAdmin
{
public:
	void Load();
	bool Check(edict_t* pEntity);

	void Menu(CBasePlayer* Player);
	static void MenuHandle(int EntityIndex, int ItemIndex, const char* Option);

	void Chat(CBasePlayer* Player, const char* Args);
	void Rcon(CBasePlayer* Player, const char* Args);

	short GetType(int EntityIndex) { return this->m_Type[EntityIndex]; }
	short SetType(int EntityIndex, short Type) { this->m_Type[EntityIndex] = Type; return Type; }

private:
	std::map<std::string, std::string> m_Data;
	short m_Type[33];
};

extern CAdmin gAdmin;
