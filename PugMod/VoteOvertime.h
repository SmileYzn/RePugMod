#pragma once

struct P_VOTE_OT_ITEM
{
	int Index;
	int Votes;
	std::string Name;

	P_VOTE_OT_ITEM(int Index, int Votes, std::string Name) : Index(Index), Votes(Votes), Name(Name) {}
};

class CVoteOvertime
{
public:
	void Init();

	void AddVote(int Item, int Vote);
	auto GetVote();

	static void MenuHandle(int EntityIndex, P_MENU_ITEM Item);
	static void Stop();
	static void List();

	int GetCount();
	P_VOTE_OT_ITEM GetWinner();

private:
	std::map<int, P_VOTE_OT_ITEM> m_Data;
};

extern CVoteOvertime gVoteOvertime;