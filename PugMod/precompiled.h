#pragma once

// Disable security function warnings (MSVC)
#define _CRT_SECURE_NO_WARNINGS

// If is not MSVC build
#ifndef _WIN32
#define _GLIBCXX_USE_CXX11_ABI				0
#define _stricmp					strcasecmp
#define _strnicmp					strncasecmp
#define _strdup						strdup
#define _unlink						unlink
#define _vsnprintf					vsnprintf
#define _write						write
#define _close						close
#define _acces						access
#define _vsnwprintf					vswprintf
#else
// cURL Library windows library
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"wldap32.lib")
#pragma comment(lib,"advapi32.lib")
#pragma comment(lib,"crypt32.lib")
#pragma comment(lib,"normaliz.lib")
#endif

// System Includes
#include <string>
#include <vector>
#include <map>
#include <array>
#include <iterator>

// JSON HPP
#include "include/json.hpp"

// cURL sources
#ifndef CURL_STATICLIB
#define CURL_STATICLIB
#endif

// cURL sources
#include <curl/curl.h>

// CSSDK
#include <extdll.h>
#include <eiface.h>

// MetaMod SDK
#include <meta_api.h> 

// ReHLDS Api
#include <rehlds_api.h>
#include <rehlds_interfaces.h>

// ReGameDLL Api
#include <regamedll_api.h>

// Plugin Includes
#include "MetaMod.h"
#include "MetaDLL.h"
#include "MetaEngine.h"
#include "ReAPI.h"
#include "ReGameDLL.h"

// PugMod
#include "Util.h"
#include "LibCurl.h"
#include "Cvars.h"
#include "Translate.h"
#include "TimeFormat.h"
#include "Commands.h"
#include "AntiFlood.h"
#include "AntiRetry.h"
#include "Player.h"
#include "PlayerStats.h"
#include "Menu.h"
#include "Task.h"
#include "PugMod.h"
#include "Ready.h"
#include "Admin.h"
#include "VoteMap.h"
#include "VoteTeam.h"
#include "VoteMenu.h"
#include "VoteKick.h"
#include "VoteLevel.h"
#include "VotePause.h"
#include "VoteRestart.h"
#include "VoteStop.h"
#include "VoteOvertime.h"
#include "Captain.h"
#include "KnifeRound.h"
#include "Stats.h"
#include "StatsCmd.h"
#include "WebApi.h"