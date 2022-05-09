#pragma once

#define VOTE_MENU_MAPS_FILE "cstrike/addons/pugmod/maps.txt"

class CVoteMenu
{
public:
	void Load(); 

	void ClientDisconnected(edict_t* pEntity);

	bool CheckMenu(CBasePlayer* Player);

	void Menu(CBasePlayer* Player);
	static void MenuHandle(int EntityIndex, int ItemIndex, bool Disabled, const char* Option);

	void		VoteKick(CBasePlayer* Player);
	static void VoteKickHandle(int EntityIndex, int ItemIndex, bool Disabled, const char* Option);
	void		VoteKickPlayer(CBasePlayer* Player, CBasePlayer* Target, bool Disabled);

	void		VoteMap(CBasePlayer* Player);
	static void VoteMapHandle(int EntityIndex, int ItemIndex, bool Disabled, const char* Option);
	void		VoteMapPickup(CBasePlayer* Player, int MapIndex, bool Disabled);

	void VotePause(CBasePlayer* Player);
	void RoundRestart();

	void VoteStop(CBasePlayer* Player);

private:
	bool m_VoteKick[33][33] = { 0 };
	bool m_VotedMap[33][33] = { 0 };
	bool m_VotedPause[33][SPECTATOR + 1] = { 0 };
	bool m_VotedStop[33][SPECTATOR + 1] = { 0 };

	std::vector<std::string> m_MapList;

	TeamName m_PausedTeam = UNASSIGNED;
	int m_PausedTime = 0;
};

extern CVoteMenu gVoteMenu;
