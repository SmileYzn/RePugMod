#include "precompiled.h"

CAuth gAuth;

void CAuth::ClientConnected(edict_t* pEdict)
{
	if (gCvars.GetApiCheckConnect()->value)
	{
		if (gCvars.GetApiUrl()->string[0] != '\0')
		{
			if (!FNullEnt(pEdict))
			{
				if (!(pEdict->v.flags & FL_FAKECLIENT) && !(pEdict->v.flags & FL_PROXY))
				{
					const char* Auth = GETPLAYERAUTHID(pEdict);

					if (Auth && Auth[0] != '\0')
					{
						char Link[512] = { 0 };

						Q_snprintf(Link, sizeof(Link), "%s?auth=%s", gCvars.GetApiUrl()->string, Auth);

						if (Link[0] && Link[0] != '\0')
						{
							gLibCurl.Get(Link, (void*)this->RequestCallback, ENTINDEX(pEdict));
						}
					}
				}
			}
		}
	}
}

void CAuth::RequestCallback(CURL* ch, size_t Size, const char* Memory, int EntityIndex)
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
						auto Result = nlohmann::json::parse(Memory);

						if (Result.contains("kick") && Result.contains("reason"))
						{
							auto KickPlayer = Result["kick"].get<bool>();

							auto KickReason = Result["reason"].get<std::string>();

							if (KickPlayer)
							{
								if (KickReason.length())
								{
									gUtil.DropClient(EntityIndex, "%s", KickReason.c_str());
								}
								else
								{
									gUtil.DropClient(EntityIndex, "Failed to connect, contact: %s", CVAR_GET_STRING("sv_contact"));
								}
							}

							return;
						}
					}
				}
			}
		}
	}

	gUtil.DropClient(EntityIndex, "Failed to connect, contact: %s", CVAR_GET_STRING("sv_contact"));
}
