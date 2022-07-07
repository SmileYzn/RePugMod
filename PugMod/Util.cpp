#include "precompiled.h"

CUtil gUtil;

char* CUtil::VarArgs(const char* Format, ...)
{
	va_list argList;

	static char VarArgs[256];

	va_start(argList, Format);

	vsnprintf(VarArgs, sizeof(VarArgs), Format, argList);

	va_end(argList);

	return VarArgs;
}

void CUtil::ServerCommand(const char* Format, ...)
{
	va_list argList;

	va_start(argList, Format);

	char Buffer[255] = { 0 };

	int Length = vsnprintf(Buffer, sizeof(Buffer), Format, argList);

	va_end(argList);

	if (Length > 254)
	{
		Length = 254;
	}

	Buffer[Length++] = '\n';
	Buffer[Length] = 0;

	SERVER_COMMAND(Buffer);
	// SERVER_EXECUTE();
}

void CUtil::ClientCommand(edict_t* pEntity, const char* Format, ...)
{
	if (pEntity)
	{
		va_list argList;

		va_start(argList, Format);

		char Buffer[255] = { 0 };

		int Length = vsnprintf(Buffer, sizeof(Buffer), Format, argList);

		va_end(argList);

		if (Length > 254)
		{
			Length = 254;
		}

		Buffer[Length++] = '\n';
		Buffer[Length] = 0;

		CLIENT_COMMAND(pEntity, Buffer);
	}
}

void CUtil::ClientPrint(edict_t* pEntity, int msg_dest, const char* Format, ...)
{
	va_list argList;

	va_start(argList, Format);

	char Buffer[188] = { 0 };

	int Length = vsnprintf(Buffer, sizeof(Buffer), Format, argList);

	va_end(argList);

	if (msg_dest == PRINT_CONSOLE)
	{
		if (Length > 125)
		{
			Length = 125;
		}

		Buffer[Length++] = '\n';
		Buffer[Length++] = '\n';
		Buffer[Length]   = 0;
	}

	static int iMsgTextMsg;

	if (iMsgTextMsg || (iMsgTextMsg = GET_USER_MSG_ID(PLID, "TextMsg", NULL)))
	{
		if (pEntity)
		{
			MESSAGE_BEGIN(MSG_ONE, iMsgTextMsg, NULL, pEntity);
		}
		else
		{
			MESSAGE_BEGIN(MSG_BROADCAST, iMsgTextMsg);
		}

		WRITE_BYTE(msg_dest);
		WRITE_STRING("%s");
		WRITE_STRING(Buffer);
		MESSAGE_END();
	}
}

void CUtil::SayText(edict_t* pEntity, int Sender, const char* Format, ...)
{
	static int iMsgSayText;

	if (iMsgSayText || (iMsgSayText = GET_USER_MSG_ID(PLID, "SayText", NULL)))
	{
		va_list argList;

		va_start(argList, Format);

		char Buffer[191] = { 0 };

		vsnprintf(Buffer, sizeof(Buffer), Format, argList);

		va_end(argList);

		char SayText[191] = { 0 };

		snprintf(SayText, sizeof(SayText), "\4[%s]\1 %s", Plugin_info.logtag, Buffer);

		if (Sender < PRINT_TEAM_BLUE || Sender > gpGlobals->maxClients)
		{
			Sender = PRINT_TEAM_DEFAULT;
		}
		else if (Sender < PRINT_TEAM_DEFAULT)
		{
			Sender = abs(Sender) + 32;
		}

		if (pEntity && !FNullEnt(pEntity))
		{
			if (!(pEntity->v.flags & FL_FAKECLIENT))
			{
				MESSAGE_BEGIN(MSG_ONE, iMsgSayText, nullptr, pEntity);
				WRITE_BYTE(Sender ? Sender : ENTINDEX(pEntity));
				WRITE_STRING("%s");
				WRITE_STRING(SayText);
				MESSAGE_END();
			}
		}
		else
		{
			for (int i = 1; i <= gpGlobals->maxClients; ++i)
			{
				edict_t *pTempEntity = INDEXENT(i);

				if (!FNullEnt(pTempEntity))
				{
					if (!(pTempEntity->v.flags & FL_FAKECLIENT))
					{
						MESSAGE_BEGIN(MSG_ONE, iMsgSayText, nullptr, pTempEntity);
						WRITE_BYTE(Sender ? Sender : i);
						WRITE_STRING("%s");
						WRITE_STRING(SayText);
						MESSAGE_END();
					}
				}
			}
		}
	}
}

hudtextparms_t CUtil::HudParam(int red, int green, int blue, float x, float y, int effects, float fxtime, float holdtime, float fadeintime, float fadeouttime, int channel)
{
	hudtextparms_t hud = { 0 };

	hud.r1 = red;
	hud.g1 = green;
	hud.b1 = blue;
	hud.x = x;
	hud.y = y;
	hud.effect = effects;
	hud.fxTime = fxtime;
	hud.holdTime = holdtime;
	hud.fadeinTime = fadeintime;
	hud.fadeoutTime = fadeouttime;
	hud.channel = channel;

	return hud;
}

void CUtil::HudMessage(edict_t* pEntity, const hudtextparms_t &textparms, const char *Format, ...)
{
	va_list argList;

	va_start(argList, Format);

	char Buffer[511] = { 0 };

	vsnprintf(Buffer, sizeof(Buffer), Format, argList);

	va_end(argList);

	if (pEntity)
	{
		MESSAGE_BEGIN(MSG_ONE_UNRELIABLE, SVC_TEMPENTITY, NULL, pEntity);
	}
	else
	{
		MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
	}

	WRITE_BYTE(TE_TEXTMESSAGE);
	WRITE_BYTE(textparms.channel & 0xFF);

	WRITE_SHORT(this->FixedSigned16(textparms.x, (1 << 13)));
	WRITE_SHORT(this->FixedSigned16(textparms.y, (1 << 13)));

	WRITE_BYTE(textparms.effect);

	WRITE_BYTE(textparms.r1);
	WRITE_BYTE(textparms.g1);
	WRITE_BYTE(textparms.b1);
	WRITE_BYTE(textparms.a1);

	WRITE_BYTE(textparms.r2);
	WRITE_BYTE(textparms.g2);
	WRITE_BYTE(textparms.b2);
	WRITE_BYTE(textparms.a2);

	WRITE_SHORT(this->FixedUnsigned16(textparms.fadeinTime, (1 << 8)));
	WRITE_SHORT(this->FixedUnsigned16(textparms.fadeoutTime, (1 << 8)));
	WRITE_SHORT(this->FixedUnsigned16(textparms.holdTime, (1 << 8)));

	if (textparms.effect == 2)
	{
		WRITE_SHORT(this->FixedUnsigned16(textparms.fxTime, (1 << 8)));
	}

	WRITE_STRING(Buffer);
	MESSAGE_END();
}

short CUtil::FixedSigned16(float value, float scale)
{
	int output;

	output = (int)(value * scale);

	if (output > 32767)
	{
		output = 32767;
	}

	if (output < -32768)
	{
		output = -32768;
	}

	return (short)output;
}

unsigned short CUtil::FixedUnsigned16(float value, float scale)
{
	int output;

	output = (int)(value * scale);

	if (output < 0)
	{
		output = 0;
	}
	if (output > 0xFFFF)
	{
		output = 0xFFFF;
	}

	return (unsigned short)output;
}

void CUtil::ShowMotd(edict_t* pEntity, char *Motd, int MotdLength)
{
	static int iMsgMOTD;

	if (iMsgMOTD || (iMsgMOTD = GET_USER_MSG_ID(PLID, "MOTD", NULL)))
	{
		if (MotdLength < 128)
		{
			int FileLength = 0;

			char* FileContent = reinterpret_cast<char*>(LOAD_FILE_FOR_ME(Motd, &FileLength));

			if (FileLength)
			{
				this->ShowMotd(pEntity, FileContent, FileLength);

				FREE_FILE(FileContent);

				return;
			}
		}

		char *Buffer = Motd;

		char Character = 0;

		int Size = 0;

		while (*Buffer)
		{
			Size = MotdLength;

			if (Size > 175)
			{
				Size = 175;
			}

			MotdLength -= Size;

			Character = *(Buffer += Size);

			*Buffer = 0;

			MESSAGE_BEGIN(MSG_ONE, iMsgMOTD, NULL, pEntity);
			WRITE_BYTE(Character ? FALSE : TRUE);
			WRITE_STRING(Motd);
			MESSAGE_END();

			*Buffer = Character;

			Motd = Buffer;
		}
	}
}

std::vector<std::string> CUtil::LoadMapList(const char * Path, bool AllowCurrentMap)
{
	std::ifstream File(Path, std::ifstream::in);

	std::vector<std::string> List;

	std::string Map;

	while (File >> Map)
	{
		if (IS_MAP_VALID((char*)Map.c_str()))
		{
			if (std::find(List.begin(), List.end(), Map) == List.end())
			{
				List.push_back(Map);
			}
		}
	}

	if (AllowCurrentMap == false)
	{
		auto it = std::find(List.begin(), List.end(), STRING(gpGlobals->mapname));

		if (it != List.end())
		{
			List.erase(it);
		}
	}

	File.close();

	return List;
}

void CUtil::SetRoundTime(int Time, bool FreezePeriod)
{
	if (g_pGameRules)
	{
		if (!CSGameRules()->m_bCompleteReset)
		{
			CSGameRules()->m_bFreezePeriod = true;
			CSGameRules()->m_iRoundTimeSecs = Time;
			CSGameRules()->m_iIntroRoundTime = Time;
			CSGameRules()->m_fRoundStartTime = gpGlobals->time;
		}
	}

	static int iMsgRoundTime;

	if (iMsgRoundTime || (iMsgRoundTime = GET_USER_MSG_ID(PLID, "RoundTime", NULL)))
	{
		CBasePlayer* Players[MAX_CLIENTS] = { NULL };

		auto Num = gPlayer.GetList(Players, false);

		for (int i = 0; i < Num; i++)
		{
			auto Player = Players[i];

			if (Player)
			{
				if (Player->pev)
				{

				}
				if (!Player->IsDormant())
				{
					if (!Player->IsBot())
					{
						MESSAGE_BEGIN(MSG_ONE, iMsgRoundTime, nullptr, Player->pev);
						WRITE_SHORT(Time);
						MESSAGE_END();
					}
				}
			}
		}
	}
}

void CUtil::ChangelevelDelay(float Delay,const char* MapName)
{
	if (MapName)
	{
		char Map[64] = { 0 };

		strncpy(Map, MapName, sizeof(Map));

		if (IS_MAP_VALID(Map))
		{
			if (Delay > 0.0f)
			{
				if (!gTask.Exists(PUG_TASK_CMAP))
				{
					gTask.Create(PUG_TASK_CMAP, Delay, false, (void*)this->Changelevel, Map);
				}
			}
			else
			{
				this->Changelevel(Map);
			}
		}
	}
}

void CUtil::Changelevel(const char* MapName)
{
	if (MapName)
	{
		char Map[64] = { 0 };

		strncpy(Map, MapName, sizeof(Map));

		if (IS_MAP_VALID(Map))
		{
			gUtil.ServerCommand("changelevel %s", Map);
		}
	}
}

