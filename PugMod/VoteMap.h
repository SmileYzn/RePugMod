#pragma once

#define VOTE_MAP_FILE "cstrike/addons/pugmod/maps.txt"

struct P_VOTE_MAP_ITEM
{
	int Votes;
	std::string Name;

	P_VOTE_MAP_ITEM(int Votes, std::string Name) : Votes(Votes), Name(Name) {}
};

class CVoteMap
{
public:
	void Init();

	void AddVote(int Item, int Vote);
	auto GetVote();

	static void MenuHandle(int EntityIndex, P_MENU_ITEM Item);
	static void Stop();
	static void List();

	int GetCount();
	int GetWinner();
	int RandomMap();

private:
	std::map<int, P_VOTE_MAP_ITEM> m_Data;
};

extern CVoteMap gVoteMap;
