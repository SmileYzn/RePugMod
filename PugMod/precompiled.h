#pragma once

// Disable security function warnings (MSVC)
#define _CRT_SECURE_NO_WARNINGS

// System Includes
#include <string>
#include <algorithm>
#include <vector>
#include <map>
#include <array>

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
#include "AntiFlood.h"
#include "Util.h"
#include "Player.h"
#include "Menu.h"
#include "Task.h"
#include "PugMod.h"
#include "Ready.h"
#include "Admin.h"
#include "VoteMap.h"
#include "VoteTeam.h"
#include "VoteMenu.h"
#include "Captain.h"
#include "Stats.h"