#pragma once

#define VOTE_MAP_FILE "cstrike/addons/pugmod/maps.txt"

class CVoteMap
{
public:
	void Init();

	void AddVote(std::string Item, int Vote);
	std::map<std::string, int> GetVote();

	static void MenuHandle(int EntityIndex, P_MENU_ITEM Item);
	static void Stop();
	static void Changelevel();
	static void List();

	int GetCount();
	std::string GetWinner();
	int RandomMap(bool Change);

private:
	std::map<std::string, int> m_Data;
};

extern CVoteMap gVoteMap;
