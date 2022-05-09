#pragma once

#define PUG_STATS_HIT 0
#define PUG_STATS_DMG 1

class CStats
{
public:
	void ClientGetIntoGame(CBasePlayer* Player);
	void RoundStart();
	void RoundEnd(int winStatus, ScenarioEventEndRound event, float tmDelay);
	void CBasePlayer_TakeDamage(CBasePlayer *pthis, entvars_t *pevInflictor, entvars_t *pevAttacker, float& flDamage, int bitsDamageType);

	bool HP(CBasePlayer* Player);
	bool Damage(CBasePlayer* Player);
	bool Received(CBasePlayer* Player);
	bool Summary(CBasePlayer* Player);

private:
	int m_Stats[33][33][2] = { 0 };
};

extern CStats gStats;