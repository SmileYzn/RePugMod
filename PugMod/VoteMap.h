#pragma once

#define VOTE_MAP_FILE "cstrike/addons/pugmod/maps.txt"

struct P_VOTE_MAP_ITEM
{
	int Index;
	int Votes;
	std::string Name;

	P_VOTE_MAP_ITEM(int Index, int Votes, std::string Name) : Index(Index), Votes(Votes), Name(Name) {}
};

class CVoteMap
{
public:
	void Load();
	void Init();

	void AddVote(int Item, int Vote);
	auto GetVote();

	static void MenuHandle(int EntityIndex, P_MENU_ITEM Item);
	static void Stop();
	static void Fail();
	static void List();

	int GetCount();
	P_VOTE_MAP_ITEM GetWinner();
	void RandomMap();

private:
	std::vector<std::string> m_Maps;
	std::map<int, P_VOTE_MAP_ITEM> m_Data;
};

extern CVoteMap gVoteMap;