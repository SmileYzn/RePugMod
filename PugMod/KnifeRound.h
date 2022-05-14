#pragma once

class CKnifeRound
{
public:
	void Init();
	void Stop(bool ChangeTeams);

	bool ClientHasRestrictItem(CBasePlayer* Player, ItemID item, ItemRestType type);

	void RoundStart();
	void RoundEnd(int winStatus, ScenarioEventEndRound event, float tmDelay);

	int SetVote(TeamName Team, int Vote);
	int GetVote();
	int GetCount();

	static void MenuHandle(int EntityIndex, int ItemIndex, bool Disabled, const char* Option);
	static void VoteEnd();

private:
	bool		m_Running    = false;
	TeamName	m_Winner	 = UNASSIGNED;
	int			m_Votes[SPECTATOR + 1] = {0};
};

extern CKnifeRound gKnifeRound;