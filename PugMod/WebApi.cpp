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
		nlohmann::json MatchData;
		//
		MatchData["server"] = 
		{
			{"hostname", CVAR_GET_STRING("hostname")},
			{"address", CVAR_GET_STRING("net_address")},
			{"time", static_cast<long long>(std::time(NULL))},
			{"map", STRING(gpGlobals->mapname)},
			{"score_tr", gPugMod.GetScores(TERRORIST)},
			{"score_ct", gPugMod.GetScores(CT)},
		};
		//
		for (auto const& Player : gStats.GetStats())
		{
			// Auth Id
			const char* AuthId = Player.first.c_str();

			// Player Stats
			CPlayerStats Stats = Player.second;

			// Stats
			MatchData[AuthId]["stats"] = 
			{
				{"frags",Stats.Frags},
				{"assists",Stats.Assists},
				{"deaths",Stats.Deaths},
				{"hs",Stats.Headshot},
				{"shots",Stats.Shots},
				{"hits",Stats.Hits},
				{"damage",Stats.Damage},
				{"damage_recv",Stats.DamageReceive},
				{"join_time",Stats.JoinTime},
				{"rws",Stats.RoundWinShare},
			};
			//
			MatchData[AuthId]["round"] =
			{
				{"play",Stats.Rounds[ROUND_PLAY]},
				{"win_tr",Stats.Rounds[ROUND_WIN_TR]},
				{"lose_tr",Stats.Rounds[ROUND_LOSE_TR]},
				{"win_ct",Stats.Rounds[ROUND_WIN_CT]},
				{"lose_ct",Stats.Rounds[ROUND_LOSE_CT]},
			};
			//
			MatchData[AuthId]["bomb"] =
			{
				{"planting", Stats.BombStats[BOMB_PLANTING]},
				{"planted",Stats.BombStats[BOMB_PLANTED]},
				{"explode",Stats.BombStats[BOMB_EXPLODED]},
				{"defusing",Stats.BombStats[BOMB_DEFUSING]},
				{"defused",Stats.BombStats[BOMB_DEFUSED]},
			};
			//
			MatchData[AuthId]["hitbox"] =
			{
				{"generic",Stats.HitBox[HITGROUP_GENERIC]},
				{"generic_dmg",Stats.HitBoxDamage[HITGROUP_GENERIC]},
				{"head",Stats.HitBox[HITGROUP_HEAD]},
				{"head_dmg",Stats.HitBoxDamage[HITGROUP_HEAD]},
				{"chest",Stats.HitBox[HITGROUP_CHEST]},
				{"chest_dmg",Stats.HitBoxDamage[HITGROUP_CHEST]},
				{"stomach",Stats.HitBox[HITGROUP_STOMACH]},
				{"stomach_dmg",Stats.HitBoxDamage[HITGROUP_STOMACH]},
				{"leftarm",Stats.HitBox[HITGROUP_LEFTARM]},
				{"leftarm_dmg",Stats.HitBoxDamage[HITGROUP_LEFTARM]},
				{"rightarm",Stats.HitBox[HITGROUP_RIGHTARM]},
				{"rightarm_dmg",Stats.HitBoxDamage[HITGROUP_RIGHTARM]},
				{"leftleg",Stats.HitBox[HITGROUP_LEFTLEG]},
				{"leftleg_dmg",Stats.HitBoxDamage[HITGROUP_LEFTLEG]},
				{"rightleg",Stats.HitBox[HITGROUP_RIGHTLEG]},
				{"rightleg_dmg",Stats.HitBoxDamage[HITGROUP_RIGHTLEG]},
				{"shield",Stats.HitBox[HITGROUP_SHIELD]},
				{"shield_dmg",Stats.HitBoxDamage[HITGROUP_SHIELD]},
			};
			//
			MatchData[AuthId]["killstreak"] =
			{
				{"k1",Stats.KillStreak[1]},
				{"k2",Stats.KillStreak[2]},
				{"k3",Stats.KillStreak[3]},
				{"k4",Stats.KillStreak[4]},
				{"k5",Stats.KillStreak[5]},
			};
			//
			MatchData[AuthId]["versus"] =
			{
				{"v1",Stats.Versus[1]},
				{"v2",Stats.Versus[2]},
				{"v3",Stats.Versus[3]},
				{"v4",Stats.Versus[4]},
				{"v5",Stats.Versus[5]},
			};
			//
			MatchData[AuthId]["money"] = 
			{
				{"none",Stats.Money[RT_NONE]},
				{"round_bonus",Stats.Money[RT_ROUND_BONUS]},
				{"player_reset",Stats.Money[RT_PLAYER_RESET]},
				{"player_join",Stats.Money[RT_PLAYER_JOIN]},
				{"player_spec_join",Stats.Money[RT_PLAYER_SPEC_JOIN]},
				{"player_bought_something",Stats.Money[RT_PLAYER_BOUGHT_SOMETHING]},
				{"hostage_took",Stats.Money[RT_HOSTAGE_TOOK]},
				{"hostage_rescued",Stats.Money[RT_HOSTAGE_RESCUED]},
				{"hostage_damaged",Stats.Money[RT_HOSTAGE_DAMAGED]},
				{"hostage_killed",Stats.Money[RT_HOSTAGE_KILLED]},
				{"teammates_killed",Stats.Money[RT_TEAMMATES_KILLED]},
				{"enemy_killed",Stats.Money[RT_ENEMY_KILLED]},
				{"into_game",Stats.Money[RT_INTO_GAME]},
				{"vip_killed",Stats.Money[RT_VIP_KILLED]},
				{"vip_rescued_myself",Stats.Money[RT_VIP_RESCUED_MYSELF]},
			};
			//
			MatchData[AuthId]["hack"] = 
			{
				{"vision",Stats.HackStats[HACK_VISION]},
				{"onehit",Stats.HackStats[HACK_ONEHIT] },
				{"noscope",Stats.HackStats[HACK_NOSCOP] },
			};
			//
			for (int WeaponId = 1; WeaponId < MAX_WEAPONS; WeaponId++)
			{
				if (Stats.WeaponStats[WeaponId][WEAPON_SHOT] > 0)
				{
					MatchData[AuthId]["weapon"].push_back
					({
						{"weapon",WeaponId},
						{"kills",Stats.WeaponStats[WeaponId][WEAPON_KILL]},
						{"deaths",Stats.WeaponStats[WeaponId][WEAPON_DEATH]},
						{"hs",Stats.WeaponStats[WeaponId][WEAPON_HEADSHOT]},
						{"shots",Stats.WeaponStats[WeaponId][WEAPON_SHOT]},
						{"hits",Stats.WeaponStats[WeaponId][WEAPON_HIT]},
						{"damage",Stats.WeaponStats[WeaponId][WEAPON_DAMAGE]},
					});
				}
			}
		}
		//
		if (MatchData.size())
		{
			const char* url = gUtil.VarArgs("%s?matchData=%s", WebApiUrl, CVAR_GET_STRING("net_address"));
			//
			if (url)
			{
				gLibCurl.PostJSON(url, MatchData.dump(), NULL, 0);
			}
		}
	}
}