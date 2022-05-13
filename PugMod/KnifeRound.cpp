#include "precompiled.h"

CKnifeRound gKnifeRound;

void CKnifeRound::Init()
{
	this->m_Running = true;

	CVAR_SET_STRING("mp_give_player_c4", "0");
	CVAR_SET_STRING("mp_round_infinite", "abcdeghijk");

	gUtil.SayText(NULL, PRINT_TEAM_RED, "Knife Round will start while match is live.");
}

void CKnifeRound::Stop()
{
	this->m_Running = false;

	CVAR_SET_STRING("mp_give_player_c4", "0");
	CVAR_SET_STRING("mp_round_infinite", "0");
	
	gPugMod.RestarPeriod(NULL);
}

bool CKnifeRound::ClientHasRestrictItem(CBasePlayer* Player, ItemID item, ItemRestType type)
{
	if (item == ITEM_C4)
	{
		gUtil.ServerPrint("%s has C4!", STRING(Player->edict()->v.netname));
	}

	if (this->m_Running)
	{
		if (item != ITEM_KEVLAR && item != ITEM_KNIFE)
		{
			if (type == ITEM_TYPE_BUYING)
			{
				gUtil.ClientPrint(Player->edict(), PRINT_CENTER, "#Cstrike_TitlesTXT_Weapon_Not_Available");
			}

			return true;
		}
	}

	return false;
}

void CKnifeRound::RoundStart()
{
	if (this->m_Running)
	{
		// Some message?
	}
}

void CKnifeRound::RoundEnd(int winStatus, ScenarioEventEndRound event, float tmDelay)
{
	if (this->m_Running)
	{
		if (winStatus != WINSTATUS_NONE)
		{
			if (winStatus == WINSTATUS_TERRORISTS)
			{
				// TRs won, start vote
			}
			else if (winStatus == WINSTATUS_CTS)
			{
				// CTs won, start vote
			}
			else if (winStatus == WINSTATUS_DRAW)
			{
				// None, continue match
			}

			this->Stop();
		}
	}
}
