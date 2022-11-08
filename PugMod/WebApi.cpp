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
