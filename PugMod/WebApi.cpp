#include "precompiled.h"

CWebApi gWebapi;

void CWebApi::ClientConnected(edict_t* pEntity)
{
	if (pEntity)
	{
		const char* WebApiUrl = gCvars.GetWebApiUrl()->string;

		if (WebApiUrl != NULL && (WebApiUrl[0] != '\0'))
		{
			const char* PlayerAuthId = GETPLAYERAUTHID(pEntity);

			if (PlayerAuthId)
			{
				const char* url = gUtil.VarArgs("%s?playerAuth=%s", WebApiUrl, PlayerAuthId);

				if (url)
				{
					gLibCurl.Get(url, (void*)this->RequestCallback, ENTINDEX(pEntity));
				}
			}
		}
	}
}

void CWebApi::RequestCallback(CURL* ch, size_t Size, const char* Memory, int EntityIndex)
{
	auto pEntity = INDEXENT(EntityIndex);

	if (!FNullEnt(pEntity))
	{
		if (ch)
		{
			long HttpResponseCode = 0;

			if (curl_easy_getinfo(ch, CURLINFO_RESPONSE_CODE, &HttpResponseCode) == CURLE_OK)
			{
				if (HttpResponseCode == 200)
				{
					if (Memory)
					{
						auto ResultData = nlohmann::json::parse(Memory);

						if (ResultData.contains("kick") && ResultData.contains("reason"))
						{
							auto KickPlayer = ResultData["kick"].get<bool>();

							auto KickReason = ResultData["reason"].get<std::string>();

							if (KickPlayer)
							{
								if (KickReason.length())
								{
									gPlayer.DropClient(EntityIndex, "%s", KickReason.c_str());
								}
								else
								{
									gPlayer.DropClient(EntityIndex, _T("Plase, register to play in this server."));
								}
							}

							return;
						}
					}
				}
			}
		}

		gPlayer.DropClient(EntityIndex, _T("Failed to get player data, contact support team."));
	}
}

void CWebApi::SaveMatchData()
{
	const char* WebApiUrl = gCvars.GetWebApiUrl()->string;

	if (WebApiUrl != NULL && (WebApiUrl[0] != '\0'))
	{
		//
		// Match Data
		nlohmann::json MatchData =
		{
			CVAR_GET_STRING("hostname"),
			CVAR_GET_STRING("net_address"),
			static_cast<long long>(std::time(NULL)),
			gPugMod.GetScores(TERRORIST),
			gPugMod.GetScores(CT)
		};
		//
		// Save all player in stats container
		for (int i = 1; i <= gpGlobals->maxClients; ++i)
		{
			auto Temp = UTIL_PlayerByIndexSafe(i);

			if (Temp)
			{
				gStats.Disconnected(Temp->edict());
			}
		}
		//
		// Json data for players
		nlohmann::json PlayerData, RoundData, BombData, HitBoxData, HitBoxDamageData, WeaponStatsData, KillStreakData, VersusData, MoneyData, HackStatsData;
		//
		// Log Player Stats
		for (auto const& Player : gStats.GetStats())
		{
			PlayerData.push_back
			({
				Player.first.c_str(),
				Player.second.Frags,
				Player.second.Assists,
				Player.second.Deaths,
				Player.second.Headshot,
				Player.second.Shots,
				Player.second.Hits,
				Player.second.Damage,
				Player.second.DamageReceive,
				Player.second.JoinTime,
				Player.second.GameTime,
				Player.second.RoundWinShare
			});
			//
			// Log Round Stats
			RoundData.push_back
			({
				Player.first.c_str(),
				Player.second.Rounds[ROUND_PLAY],
				Player.second.Rounds[ROUND_WIN_TR],
				Player.second.Rounds[ROUND_LOSE_TR],
				Player.second.Rounds[ROUND_WIN_CT],
				Player.second.Rounds[ROUND_LOSE_CT]
			});
			//
			// Log Bomb Stats
			BombData.push_back
			({
				Player.first.c_str(),
				Player.second.BombStats[BOMB_PLANTING],
				Player.second.BombStats[BOMB_PLANTED],
				Player.second.BombStats[BOMB_EXPLODED],
				Player.second.BombStats[BOMB_DEFUSING],
				Player.second.BombStats[BOMB_DEFUSED]
			});
			//
			// Log HitBox Stats
			HitBoxData.push_back
			({
				Player.first.c_str(),
				Player.second.HitBox[HITGROUP_GENERIC],
				Player.second.HitBox[HITGROUP_HEAD],
				Player.second.HitBox[HITGROUP_CHEST],
				Player.second.HitBox[HITGROUP_STOMACH],
				Player.second.HitBox[HITGROUP_LEFTARM],
				Player.second.HitBox[HITGROUP_RIGHTARM],
				Player.second.HitBox[HITGROUP_LEFTLEG],
				Player.second.HitBox[HITGROUP_RIGHTLEG],
				Player.second.HitBox[HITGROUP_SHIELD]
			});
			//
			// Log HitBox Damage
			HitBoxDamageData.push_back
			({
				Player.first.c_str(),
				Player.second.HitBoxDamage[HITGROUP_GENERIC],
				Player.second.HitBoxDamage[HITGROUP_HEAD],
				Player.second.HitBoxDamage[HITGROUP_CHEST],
				Player.second.HitBoxDamage[HITGROUP_STOMACH],
				Player.second.HitBoxDamage[HITGROUP_LEFTARM],
				Player.second.HitBoxDamage[HITGROUP_RIGHTARM],
				Player.second.HitBoxDamage[HITGROUP_LEFTLEG],
				Player.second.HitBoxDamage[HITGROUP_RIGHTLEG],
				Player.second.HitBoxDamage[HITGROUP_SHIELD]
			});
			//
			// Log Weapon Stats
			for (int WeaponType = 0; WeaponType < MAX_WEAPONS; WeaponType++)
			{
				if (Player.second.WeaponStats[WeaponType][WEAPON_SHOT] > 0)
				{
					WeaponStatsData.push_back
					({
						Player.first.c_str(),
						WeaponType,
						Player.second.WeaponStats[WeaponType][WEAPON_KILL],
						Player.second.WeaponStats[WeaponType][WEAPON_DEATH],
						Player.second.WeaponStats[WeaponType][WEAPON_HEADSHOT],
						Player.second.WeaponStats[WeaponType][WEAPON_SHOT],
						Player.second.WeaponStats[WeaponType][WEAPON_HIT],
						Player.second.WeaponStats[WeaponType][WEAPON_DAMAGE]
					});
				}
			}
			//
			// Log KillStreak
			KillStreakData.push_back
			({
				Player.first.c_str(),
				Player.second.KillStreak[1],
				Player.second.KillStreak[2],
				Player.second.KillStreak[3],
				Player.second.KillStreak[4],
				Player.second.KillStreak[5]
			});
			//
			// Log Versus
			VersusData.push_back
			({
				Player.first.c_str(),
				Player.second.Versus[1],
				Player.second.Versus[2],
				Player.second.Versus[3],
				Player.second.Versus[4],
				Player.second.Versus[5]
			});
			//
			// Log Money
			MoneyData.push_back
			({
				Player.first.c_str(),
				Player.second.Money[RT_NONE],
				Player.second.Money[RT_ROUND_BONUS],
				Player.second.Money[RT_PLAYER_RESET],
				Player.second.Money[RT_PLAYER_JOIN],
				Player.second.Money[RT_PLAYER_SPEC_JOIN],
				Player.second.Money[RT_PLAYER_BOUGHT_SOMETHING],
				Player.second.Money[RT_HOSTAGE_TOOK],
				Player.second.Money[RT_HOSTAGE_RESCUED],
				Player.second.Money[RT_HOSTAGE_DAMAGED],
				Player.second.Money[RT_HOSTAGE_KILLED],
				Player.second.Money[RT_TEAMMATES_KILLED],
				Player.second.Money[RT_ENEMY_KILLED],
				Player.second.Money[RT_INTO_GAME],
				Player.second.Money[RT_VIP_KILLED],
				Player.second.Money[RT_VIP_RESCUED_MYSELF]
			});
			//
			// Log HackStats
			HackStatsData.push_back
			({
				Player.first.c_str(),
				Player.second.HackStats[HACK_VISION],
				Player.second.HackStats[HACK_ONEHIT],
				Player.second.HackStats[HACK_NOSCOP]
			});
		}
	}
}