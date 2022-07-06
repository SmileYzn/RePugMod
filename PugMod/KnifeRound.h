#pragma once

class CKnifeRound
{
public:
	void Init(bool ForceKnifeRound = false);
	void Stop(bool ChangeTeams);

	bool ClientHasRestrictItem(CBasePlayer* Player, ItemID item, ItemRestType type);
	bool GiveC4();

	void RoundRestart();
	void RoundEnd(int winStatus, ScenarioEventEndRound event, float tmDelay);

	static void List();

	TeamName GetWinner();
	int AddVote(TeamName Team);
	int GetVote(TeamName Team);

	static void MenuHandle(int EntityIndex, P_MENU_ITEM Item);
	static void VoteEnd();

private:
	bool		m_Running = false;
	TeamName	m_Winner = UNASSIGNED;
	int			m_Votes[SPECTATOR + 1] = { 0 };
};

extern CKnifeRound gKnifeRound;