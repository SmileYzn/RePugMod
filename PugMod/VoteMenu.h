#pragma once

class CVoteMenu
{
public:
	bool CheckMenu(CBasePlayer* Player);

	void Menu(CBasePlayer* Player);
	static void MenuHandle(int EntityIndex, int ItemIndex, const char* Option);

	void VoteKick(CBasePlayer* Player);
	static void VoteKickHandle(int EntityIndex, int ItemIndex, const char* Option);

	void VoteMap(CBasePlayer* Player);
	static void VoteMapHandle(int EntityIndex, int ItemIndex, const char* Option);

	void VotePause(CBasePlayer* Player);
	static void VotePauseHandle(int EntityIndex, int ItemIndex, const char* Option);

	void VoteStop(CBasePlayer* Player);
	static void VoteStopHandle(int EntityIndex, int ItemIndex, const char* Option);

private:
};

extern CVoteMenu gVoteMenu;