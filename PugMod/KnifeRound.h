#pragma once

class CKnifeRound
{
public:
	void Init();
	void Stop();

	bool ClientHasRestrictItem(CBasePlayer* Player, ItemID item, ItemRestType type);

	void RoundStart();
	void RoundEnd(int winStatus, ScenarioEventEndRound event, float tmDelay);

private:
	bool m_Running = false;
};

extern CKnifeRound gKnifeRound;