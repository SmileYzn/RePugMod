#pragma once

#define VOTE_MENU_MAPS_FILE "cstrike/addons/pugmod/maps.txt"

class CVoteMenu
{
public:
	void Load(); 

	void ClientDisconnected(edict_t* pEntity);

	bool CheckMenu(CBasePlayer* Player);

	void Menu(CBasePlayer* Player);
	static void MenuHandle(int EntityIndex, P_MENU_ITEM Item);

	void		VoteMap(CBasePlayer* Player);
	static void VoteMapHandle(int EntityIndex, P_MENU_ITEM Item);
	void		VoteMapPickup(CBasePlayer* Player, int MapIndex, bool Disabled);

	void VoteStop(CBasePlayer* Player);

private:
	bool m_VotedMap[MAX_CLIENTS + 1][MAX_CLIENTS + 1] = { 0 };
	bool m_VotedStop[MAX_CLIENTS + 1][SPECTATOR + 1] = { 0 };

	std::vector<std::string> m_MapList;
};

extern CVoteMenu gVoteMenu;
