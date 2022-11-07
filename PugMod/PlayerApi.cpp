#include "precompiled.h"

CPlayerApi gPlayerApi;

void CPlayerApi::ClientConnected(edict_t* pEntity)
{
	if (pEntity)
	{
		const char* PlayerAuthId = GETPLAYERAUTHID(pEntity);

		if (PlayerAuthId)
		{
			const char* url = gUtil.VarArgs("", PlayerAuthId);

			if (url)
			{
				gLibCurl.Get(url, this->RequestCallback, ENTINDEX(pEntity));
			}
		}
	}
}

void CPlayerApi::RequestCallback(CURL* ch, size_t Size, const char* Memory, int EntityIndex)
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

		gPlayer.DropClient(EntityIndex, "Register at PugBR.net to play in this server...");
	}
}