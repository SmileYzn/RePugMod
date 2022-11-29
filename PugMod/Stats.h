#pragma once

class CStats
{
public:
	void RoundStart();
	void TakeDamage(CBasePlayer* Player, entvars_t* pevInflictor, entvars_t* pevAttacker, float& flDamage, int bitsDamageType);

	bool HP(CBasePlayer* Player);
	bool Damage(CBasePlayer* Player);
	bool Received(CBasePlayer* Player);
	bool Summary(CBasePlayer* Player);

private:
	int m_Hits[MAX_CLIENTS + 1][MAX_CLIENTS + 1] = { 0 };
	int m_Damage[MAX_CLIENTS + 1][MAX_CLIENTS + 1] = { 0 };
};

extern CStats gStats;
