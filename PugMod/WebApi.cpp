#include "precompiled.h"

CWebApi gWebapi;

void CWebApi::ClientConnected(edict_t* pEntity)
{
	if (pEntity)
	{
		if (gCvars.GetWebApiUrl()->string)
		{
			const char* PlayerAuthId = GETPLAYERAUTHID(pEntity);

			if (PlayerAuthId)
			{
				const char* url = gUtil.VarArgs("%s?playerAuth=%s", gCvars.GetWebApiUrl()->string, PlayerAuthId);

				if (url)
				{
					gLibCurl.Get(url, this->RequestCallback, ENTINDEX(pEntity));
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
						auto PlayerData = nlohmann::json::parse(Memory);

						if (PlayerData.contains("steamid2"))
						{
							auto steamid2 = PlayerData["steamid2"].get<std::string>();

							if (steamid2.compare(GETPLAYERAUTHID(pEntity)) == 0)
							{
								return;
							}
						}
					}
				}
			}
		}

		const char* ContactAddress = CVAR_GET_STRING("sv_contact");

		if (ContactAddress)
		{
			gPlayer.DropClient(EntityIndex, "Register at %s to play in this server..", ContactAddress);
		}
		else
		{
			gPlayer.DropClient(EntityIndex, "Register to play in this server..");
		}
	}
}
