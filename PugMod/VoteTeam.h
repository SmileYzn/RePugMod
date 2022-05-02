#pragma once

class CVoteTeam
{
public:
	void Load();
	void Init(int Delay);

	void AddVote(int ItemIndex, int Vote) { this->m_Vote[ItemIndex] += Vote; }

	static void MenuHandle(int EntityIndex, int ItemIndex, const char* Option);
	static void Stop();
	static void List(CVoteTeam* VoteTeam);

	int GetCount();
	int GetWinner();
	const char* GetItem(int ItemIndex);

	void SetMode(int GameMode);
	void TeamsRandomize();
	void TeamsOptimize();
private:
	std::vector<std::string> m_Data;
	std::map<int, int>       m_Vote;
	short					 m_Delay;
};

extern CVoteTeam gVoteTeam;
