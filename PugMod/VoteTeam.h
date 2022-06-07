#pragma once

struct P_VOTE_TEAM_ITEM
{
	int Votes;
	std::string Name;

	P_VOTE_TEAM_ITEM(int Votes, std::string Name) : Votes(Votes), Name(Name) {}
};

class CVoteTeam
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

	void SetMode(int GameMode);
	void TeamsRandomize();
	void TeamsOptimize();

private:
	std::map<int, P_VOTE_TEAM_ITEM> m_Data;
};

extern CVoteTeam gVoteTeam;