#pragma once

enum UTIL_PRINT_DEST_TYPE
{
	PRINT_NOTIFY = 1,
	PRINT_CONSOLE = 2,
	PRINT_CHAT = 3,
	PRINT_CENTER = 4,
	PRINT_RADIO = 5,
};

enum UTIL_SAY_TEXT_COLOR
{
	PRINT_TEAM_DEFAULT = 0,
	PRINT_TEAM_GREY = -1,
	PRINT_TEAM_RED = -2,
	PRINT_TEAM_BLUE = -3,
};

class CUtil
{
public:
	char* VarArgs(const char* Format, ...);
	void ServerCommand(const char* Format, ...);
	void ClientCommand(edict_t* pEntity, const char* Format, ...);
	void ClientPrint(edict_t* pEntity, int msg_dest, const char* Format, ...);
	void SayText(edict_t* pEntity, int Sender, const char* Format, ...);
	hudtextparms_t HudParam(int red = 255, int green = 255, int blue = 255, float x = -1.0f, float y = 0.35f, int effects = 0, float fxtime = 6.0f, float holdtime = 12.0f, float fadeintime = 0.1f, float fadeouttime = 0.2f, int channel = 0);
	void HudMessage(edict_t* pEntity, const hudtextparms_t& textparms, const char* Format, ...);
	short FixedSigned16(float value, float scale);
	unsigned short FixedUnsigned16(float value, float scale);
	void ShowMotd(edict_t* pEntity, char* Motd, int MotdLength);
	std::vector<std::string> LoadMapList(const char* Path, bool AllowCurrentMap);
	void SetRoundTime(int Time, bool FreezePeriod);
	void ChangelevelDelay(float Delay, const char* MapName);
	static void Changelevel(const char* MapName);
};

extern CUtil gUtil;