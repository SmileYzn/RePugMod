#pragma once

#define VOTE_MAP_FILE "cstrike/addons/pugmod/maps.txt"

class CVoteMap
{
public:
	void Load();
	void Init();

	void AddVote(int ItemIndex,int Vote) { this->m_Vote[ItemIndex] += Vote; }

	static void MenuHandle(int EntityIndex, P_MENU_ITEM Item);
	static void Stop();
	static void Changelevel(char* MapName);
	static void List(CVoteMap* VoteMap);

	int GetCount();
	int GetWinner();
	const char* GetItem(int ItemIndex);
	int RandomMap(bool Change);

private:
	std::vector<std::string> m_Data;
	std::map<int, int>       m_Vote;
};

extern CVoteMap gVoteMap;
