#include "precompiled.h"

CPugMod gPugMod;

void CPugMod::Load()
{
	this->SetState(PUG_STATE_DEAD); 

	this->NextState(gCvars.GetAutoStartDelay()->value);
}

void CPugMod::Unload()
{
	if (this->m_State >= PUG_STATE_FIRST_HALF && this->m_State <= PUG_STATE_OVERTIME)
	{
		this->SetState(PUG_STATE_END);
	}
}

int CPugMod::GetState()
{
	return this->m_State;
}

void CPugMod::SetState(int State)
{
	this->m_State = State;

	switch (this->m_State)
	{
		case PUG_STATE_WARMUP:
			{
				gReady.Load();

				gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, _T("%s started, get ready!"), PUG_MOD_STATES_STR[this->m_State]);
				break;
			}
		case PUG_STATE_START:
			{
				gReady.Unload();

				if (gCvars.GetVoteMap()->value && gCvars.GetVoteMapType()->value)
				{
					gCvars.GetVoteMap()->value = 0;

					if (gCvars.GetVoteMapType()->value == 1)
					{
						gVoteMap.Init();
					}
					else if (gCvars.GetVoteMapType()->value == 2)
					{
						gVoteMap.RandomMap(true);
					}
				}
				else
				{
					if (gCvars.GetVoteTeamType()->value == -1.0f)
					{
						gVoteTeam.Init();
					}
					else
					{
						gVoteTeam.SetMode((int)gCvars.GetVoteTeamType()->value);
					}
				}

				break;
			}
		case PUG_STATE_FIRST_HALF:
			{
				gReady.Unload();

				memset(this->m_Round, 0, sizeof(this->m_Round));
				memset(this->m_Score, 0, sizeof(this->m_Score));

				if (this->CheckBalanceTeams())
				{
					this->LO3(3);

					gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, _T("%s started: \3Good Luck & Have Fun!"), PUG_MOD_STATES_STR[this->m_State]);
				}
				else
				{
					gUtil.SayText(0, PRINT_TEAM_RED, _T("Failed to start match: \3Not enough players.\1"));

					this->SetState(PUG_STATE_WARMUP);
				}

				break;
			}
		case PUG_STATE_HALFTIME:
			{
				if ((int)gCvars.GetShowScoreType()->value == 2)
				{
					CBasePlayer* Players[MAX_CLIENTS] = { NULL };

					auto Num = gPlayer.GetList(Players);

					for (int i = 0; i < Num; i++)
					{
						auto Player = Players[i];

						if (Player)
						{
							this->m_Frags[Player->entindex()] = Player->edict()->v.frags;
							this->m_Death[Player->entindex()] = Player->m_iDeaths;
						}
					}
				}

				gTask.Create(PUG_TASK_EXEC, 5.0f, false, this->SwapTeams, this);

				if (gPlayer.GetNum() < (int)gCvars.GetPlayersMin()->value)
				{
					gReady.Load();
				}

				gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, _T("%s started, get ready!"), PUG_MOD_STATES_STR[this->m_State]);

				break;
			}
		case PUG_STATE_SECOND_HALF:
		case PUG_STATE_OVERTIME:
			{
				gReady.Unload();

				if (this->CheckBalanceTeams())
				{
					this->LO3(3);

					gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, _T("%s started: \3Good Luck & Have Fun!"), PUG_MOD_STATES_STR[this->m_State]);
				}
				else
				{
					gUtil.SayText(0, PRINT_TEAM_RED, _T("Failed to continue match: \3Not enough players.\1"));

					this->SetState(PUG_STATE_HALFTIME);
				}

				break;
			}
		case PUG_STATE_END:
			{
				this->ViewScores(NULL);

				gCvars.GetVoteMap()->value = 1;

				gTask.Create(PUG_TASK_EXEC, 5.0f, false, this->RunState, &gPugMod);

				break;
			}
	}

	if (gCvars.GetConfig(this->m_State))
	{
		if (gCvars.GetConfig(this->m_State)->string)
		{
			gUtil.ServerCommand("exec cstrike/addons/pugmod/cfg/%s\n", gCvars.GetConfig(this->m_State)->string);
		}
	}
}

void CPugMod::NextState(float Delay)
{
	if (Delay)
	{
		gTask.Create(PUG_TASK_NEXT, Delay, false, this->RunState, this);
	}
	else
	{
		this->RunState(this);
	}
}

void CPugMod::RunState(CPugMod* PugMod)
{
	int Next = PUG_STATE_DEAD;

	switch (PugMod->m_State)
	{
		case PUG_STATE_DEAD:
			{
				Next = PUG_STATE_WARMUP;
				break;
			}
		case PUG_STATE_WARMUP:
			{
				Next = PUG_STATE_START;
				break;
			}
		case PUG_STATE_START:
			{
				Next = PUG_STATE_FIRST_HALF;
				break;
			}
		case PUG_STATE_FIRST_HALF:
			{
				Next = PUG_STATE_HALFTIME;
				break;
			}
		case PUG_STATE_HALFTIME:
			{
				if (PugMod->GetRound() < (int)gCvars.GetPlayRounds()->value)
				{
					Next = PUG_STATE_SECOND_HALF;
				}
				else
				{
					Next = PUG_STATE_OVERTIME;
				}

				break;
			}
		case PUG_STATE_SECOND_HALF:
			{
				Next = PUG_STATE_END;

				if (PugMod->GetScores(TERRORIST) == (int)gCvars.GetPlayRounds()->value / 2)
				{
					if (PugMod->GetScores(CT) == (int)gCvars.GetPlayRounds()->value / 2)
					{
						if ((int)gCvars.GetPlayRoundsOvertimeType()->value == 1)
						{
							Next = PUG_STATE_HALFTIME;
						}
					}
				}

				break;
			}
		case PUG_STATE_OVERTIME:
			{
				Next = PUG_STATE_END;

				if ((PugMod->GetRound() % (int)gCvars.GetPlayRoundsOvertime()->value) == 0)
				{
					Next = PUG_STATE_HALFTIME;
				}

				break;
			}
		case PUG_STATE_END:
			{
				Next = PUG_STATE_WARMUP;
				break;
			}
	}

	PugMod->SetState(Next);
}

bool CPugMod::CheckBalanceTeams()
{
	if (gCvars.GetPlayCheckPlayers()->value)
	{
		int MinPlayers = ((int)gCvars.GetPlayersMin()->value / 2);

		int InGame, NumTerrorist, NumCT, NumSpectator;

		gPlayer.GetNum(true, InGame, NumTerrorist, NumCT, NumSpectator);

		if (NumTerrorist < MinPlayers || NumCT < MinPlayers)
		{
			if (g_pGameRules)
			{
				CSGameRules()->BalanceTeams();

				gPlayer.GetNum(true, InGame, NumTerrorist, NumCT, NumSpectator);

				if (NumTerrorist >= MinPlayers && NumCT >= MinPlayers)
				{
					return true;
				}
			}

			return false;
		}
	}

	return true;
}

bool CPugMod::StartVoteMap(CBasePlayer* Player)
{
	if (this->m_State != PUG_STATE_DEAD && this->m_State != PUG_STATE_START && this->m_State != PUG_STATE_END)
	{
		gReady.Unload();

		gUtil.SayText(NULL, Player->entindex(), _T("\3%s\1 started Vote Map."),STRING(Player->edict()->v.netname));

		gVoteMap.Init();

		return true;
	}
	else
	{
		gUtil.SayText(Player->edict(), PRINT_TEAM_RED, _T("Cannot start an vote in \3%s\1 state."), PUG_MOD_STATES_STR[this->m_State]);
	}

	return false;
}

bool CPugMod::StartVoteTeam(CBasePlayer* Player)
{
	if (this->m_State == PUG_STATE_WARMUP)
	{
		gCvars.GetVoteMap()->value = 0.0f;

		gCvars.GetVoteTeamType()->value = -1.0f;

		gUtil.SayText(NULL, Player->entindex(), _T("\3%s\1 started Vote Team."), STRING(Player->edict()->v.netname));

		this->SetState(PUG_STATE_START);

		return true;
	}
	else
	{
		gUtil.SayText(Player->edict(), PRINT_TEAM_RED, _T("Cannot start vote team in \3%s\1 state."), PUG_MOD_STATES_STR[this->m_State]);
	}

	return false;
}

bool CPugMod::StartMatch(CBasePlayer* Player)
{
	if (this->m_State == PUG_STATE_WARMUP || this->m_State == PUG_STATE_HALFTIME)
	{
		gUtil.SayText(NULL, Player->entindex(), _T("\3%s\1 started match."), STRING(Player->edict()->v.netname));

		if (this->m_State == PUG_STATE_HALFTIME)
		{
			this->NextState(1.0);
		}
		else
		{
			this->SetState(PUG_STATE_FIRST_HALF);
		}

		return true;
	}
	else
	{
		gUtil.SayText(Player->edict(), PRINT_TEAM_RED, _T("Cannot start match in \3%s\1 state."), PUG_MOD_STATES_STR[this->m_State]);
	}

	return false;
}

bool CPugMod::StopMatch(CBasePlayer* Player)
{
	if (this->m_State >= PUG_STATE_FIRST_HALF && this->m_State <= PUG_STATE_OVERTIME)
	{
		gTask.Remove(PUG_TASK_LO3R);

		gUtil.SayText(NULL, Player->entindex(), _T("\3%s\1 stopped match."), STRING(Player->edict()->v.netname));

		this->SetState(PUG_STATE_END);

		return true;
	}
	else
	{
		gUtil.SayText(Player->edict(), PRINT_TEAM_RED, _T("Cannot stop match in \3%s\1 state."), PUG_MOD_STATES_STR[this->m_State]);
	}

	return false;
}

bool CPugMod::RestarPeriod(CBasePlayer* Player)
{
	if (this->m_State == PUG_STATE_FIRST_HALF || this->m_State == PUG_STATE_SECOND_HALF || this->m_State == PUG_STATE_OVERTIME)
	{
		if (Player)
		{
			gUtil.SayText(NULL, Player->entindex(), _T("\3%s\1 restarted \4%s\1 period, get ready!."), STRING(Player->edict()->v.netname), PUG_MOD_STATES_STR[this->m_State]);
		}

		this->m_Round[this->m_State] = 0;
		this->m_Score[this->m_State][TERRORIST] = 0;
		this->m_Score[this->m_State][CT] = 0;

		this->SetState(this->m_State);

		return true;
	}
	else
	{
		if (Player)
		{
			gUtil.SayText(Player->edict(), PRINT_TEAM_RED, _T("Cannot restart period in \3%s\1 state."), PUG_MOD_STATES_STR[this->m_State]);
		}
	}

	return false;
}

bool CPugMod::EndGame(TeamName Winner)
{
	if (Winner == TERRORIST || Winner == CT)
	{
		TeamName Losers = (Winner == TERRORIST) ? CT : TERRORIST;

		if (this->GetScores(Winner) <= this->GetScores(Losers))
		{
			memset(this->m_Round, 0, sizeof(this->m_Round));
			memset(this->m_Score, 0, sizeof(this->m_Score));

			this->m_Round[PUG_STATE_FIRST_HALF] = (gCvars.GetPlayRounds()->value / 2);
			this->m_Round[PUG_STATE_SECOND_HALF] = 1;

			this->m_Score[PUG_STATE_FIRST_HALF][Winner] = this->m_Round[PUG_STATE_FIRST_HALF];
			this->m_Score[PUG_STATE_SECOND_HALF][Winner] = this->m_Round[PUG_STATE_SECOND_HALF];
		}

		this->SetState(PUG_STATE_END);

		return true;
	}

	return false;
}

int CPugMod::GetRound()
{
	int Round = 0; 

	for (int i = PUG_STATE_DEAD; i <= PUG_STATE_END; i++)
	{
		Round += this->m_Round[i];
	}

	return Round;
}

int CPugMod::GetScores(int Team)
{
	int Score = 0;

	for (int i = PUG_STATE_DEAD; i <= PUG_STATE_END; i++)
	{
		Score += this->m_Score[i][Team];
	}

	return Score;
}

int CPugMod::GetWinner()
{
	if (this->GetScores(TERRORIST) != this->GetScores(CT))
	{
		return (this->GetScores(TERRORIST) > this->GetScores(CT)) ? TERRORIST : CT;
	}

	return UNASSIGNED;
}

void CPugMod::Help(CBasePlayer * Player,bool AdminHelp)
{
	if (!AdminHelp)
	{
		gUtil.ShowMotd(Player->edict(), gCvars.GetHelpFilePlayer()->string, strlen(gCvars.GetHelpFilePlayer()->string));
	}
	else
	{
		if (gAdmin.Check(Player))
		{
			gUtil.ShowMotd(Player->edict(), gCvars.GetHelpFileAdmin()->string, strlen(gCvars.GetHelpFileAdmin()->string));
		}
	}
}

void CPugMod::Status(CBasePlayer* Player)
{
	gUtil.SayText
	(
		Player->edict(),
		PRINT_TEAM_DEFAULT,
		_T("Status: \4%s\1 (Players %d) (%d Required of %d Allowed)"),
		PUG_MOD_STATES_STR[this->m_State],
		gPlayer.GetNum(),
		(int)gCvars.GetPlayersMin()->value,
		(int)gCvars.GetPlayersMax()->value
	);

	if (this->m_State >= PUG_STATE_FIRST_HALF && this->m_State <= PUG_STATE_END)
	{
		this->ViewScores(Player);
	}
}

void CPugMod::Scores(CBasePlayer* Player)
{
	if (this->m_State >= PUG_STATE_FIRST_HALF && this->m_State <= PUG_STATE_END)
	{
		this->ViewScores(Player);
	}
	else
	{
		gUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, _T("Unable to use this command now."));
	}
}

void CPugMod::ViewScores(CBasePlayer* Player)
{
	int Winner = this->GetWinner();

	if (Winner)
	{
		gUtil.SayText
		(
			Player ? Player->edict() : NULL,
			Winner == TERRORIST ? PRINT_TEAM_RED : PRINT_TEAM_BLUE,
			(this->m_State == PUG_STATE_END) ? _T("Game Over! The \3%s\1 have won the game: %d-%d") : _T("The \3%s\1 are winning: %d-%d"),
			PUG_MOD_TEAM_STR[Winner],
			this->GetScores(Winner),
			this->GetScores(Winner == TERRORIST ? CT : TERRORIST)
		);
	}
	else
	{
		gUtil.SayText
		(
			Player ? Player->edict() : NULL,
			PRINT_TEAM_DEFAULT,
			(this->m_State == PUG_STATE_END) ? _T("Game Over! Score is tied: %d-%d") : _T("Score is tied: %d-%d"),
			this->GetScores(TERRORIST),
			this->GetScores(CT)
		);
	}
}

void CPugMod::LO3(int Delay)
{
	if (Delay >= 1 && Delay <= 3) 
	{
		if (g_pGameRules)
		{
			CSGameRules()->m_bGameStarted = true;
		}

		CVAR_SET_FLOAT("sv_restart", (float)Delay);

		gTask.Create(PUG_TASK_LO3R, (float)Delay + 1.0f, false, gPugMod.LO3, (void*)(Delay - 1));
	}
	else
	{
		hudtextparms_t Hud = { -1.0f,0.3f,0,0,0xFF,0,0xFF,0,0xFF,0,0xFF,0.0f,0.0f,10.0f,10.0f,0 };

		gUtil.HudMessage(NULL, gUtil.HudParam(0, 255, 0, -1.0, 0.2, 0, 10.0, 10.0), _T("--- MATCH IS LIVE ---"));
	}
}

void CPugMod::SwapTeams(CPugMod* PugMod)
{
	for (int iState = PUG_STATE_FIRST_HALF; iState <= PUG_STATE_OVERTIME; iState++)
	{
		int ScoreTR = PugMod->m_Score[iState][TERRORIST];

		PugMod->m_Score[iState][TERRORIST] = PugMod->m_Score[iState][CT];

		PugMod->m_Score[iState][CT] = ScoreTR;
	}

	if (g_pGameRules)
	{
		CSGameRules()->SwapAllPlayers();
	}

	gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, "Changing teams automatically.");

	if ((PugMod->m_State == PUG_STATE_HALFTIME) && (gPlayer.GetNum() >= (int)gCvars.GetPlayersMin()->value))
	{
		PugMod->NextState(2.0f);
	}
}

void CPugMod::ClientConnected(edict_t* pEntity)
{
	if (gPlayer.GetNum() >= (int)gCvars.GetPlayersMax()->value)
	{
		if (!CVAR_GET_FLOAT("allow_spectators"))
		{
			gPlayer.DropClient(ENTINDEX(pEntity), _T("Server is full and spectators are not allowed."));
		}
	}
}

void CPugMod::ClientGetIntoGame(CBasePlayer* Player)
{
	this->m_Frags[Player->entindex()] = 0;
	this->m_Death[Player->entindex()] = 0;
}

void CPugMod::ClientDisconnected(edict_t* pEntity)
{
	if (this->m_State >= PUG_STATE_FIRST_HALF && this->m_State <= PUG_STATE_OVERTIME)
	{
		if (gPlayer.GetNum() < (gCvars.GetPlayersMin()->value / 2))
		{
			this->SetState(PUG_STATE_END);
		}
	}
}

bool CPugMod::ClientCommand(CBasePlayer * Player, const char * pcmd, const char * parg1)
{
	if (_stricmp(pcmd, "say") == 0 || _stricmp(pcmd, "say_team") == 0)
	{
		if (parg1[0] == '.' || parg1[0] == '!')
		{
			if (CMD_ARGS())
			{
				gUtil.ClientCommand(Player->edict(), "%s", CMD_ARGS());
				return true;
			}
		}
	}
	else if (_stricmp(pcmd, "jointeam") == 0)
	{
		if (this->ClientJoinTeam(Player, atoi(parg1)))
		{
			return true;
		}
	}
	else if (_stricmp(pcmd, "menuselect") == 0)
	{
		int Key = atoi(parg1);

		if (Player->m_iMenu == Menu_OFF)
		{
			if (gMenu[Player->entindex()].Handle(Player->entindex(), Key))
			{
				return true;
			}
		}
		else if (Player->m_iMenu == Menu_ChooseTeam || Player->m_iMenu == Menu_IGChooseTeam)
		{
			if (this->ClientJoinTeam(Player, Key))
			{
				return true;
			}
		}
	}
	else if (_stricmp(pcmd, _T(".help")) == 0)
	{
		this->Help(Player,false);
		return true;
	}
	else if (_stricmp(pcmd, _T("!help")) == 0)
	{
		this->Help(Player, true);
		return true;
	}
	else if (_stricmp(pcmd, _T(".status")) == 0)
	{
		this->Status(Player);
		return true;
	}
	else if (_stricmp(pcmd, _T(".score")) == 0)
	{
		this->Scores(Player);
		return true;
	}
	else if (_stricmp(pcmd, _T(".ready")) == 0)
	{
		gReady.Ready(Player);
		return true;
	}
	else if (_stricmp(pcmd, _T(".notready")) == 0)
	{
		gReady.NotReady(Player);
		return true;
	}
	else if (_stricmp(pcmd, _T(".vote")) == 0)
	{
		gVoteMenu.Menu(Player);
		return true;
	}
	else if (_stricmp(pcmd, _T("!menu")) == 0)
	{
		gAdmin.Menu(Player);
		return true;
	}
	else if (_stricmp(pcmd, _T("!msg")) == 0)
	{
		gAdmin.Chat(Player, CMD_ARGS());
		return true;
	}
	else if (_stricmp(pcmd, _T("!rcon")) == 0)
	{
		gAdmin.Rcon(Player, CMD_ARGS());
		return true;
	}
	else if (_stricmp(pcmd, _T(".hp")) == 0)
	{
		gStats.HP(Player);
		return true;
	}
	else if (_stricmp(pcmd, _T(".dmg")) == 0)
	{
		gStats.Damage(Player);
		return true;
	}
	else if (_stricmp(pcmd, _T(".rdmg")) == 0)
	{
		gStats.Received(Player);
		return true;
	}
	else if (_stricmp(pcmd, _T(".sum")) == 0)
	{
		gStats.Summary(Player);
		return true;
	}
	return false;
}

bool CPugMod::ClientAddAccount(CBasePlayer* Player, int amount, RewardType type, bool bTrackChange)
{
	if (type == RT_PLAYER_BOUGHT_SOMETHING)
	{
		if (this->m_State == PUG_STATE_WARMUP || this->m_State == PUG_STATE_START || this->m_State == PUG_STATE_HALFTIME)
		{
			return true;
		}
	}

	return false;
}

bool CPugMod::ClientHasRestrictItem(CBasePlayer* Player, ItemID item, ItemRestType type)
{
	if (this->m_State == PUG_STATE_WARMUP || this->m_State == PUG_STATE_START || this->m_State == PUG_STATE_HALFTIME)
	{
		if (type == ITEM_TYPE_BUYING)
		{
			if (item == ITEM_SHIELDGUN || item == ITEM_HEGRENADE || item == ITEM_FLASHBANG || item == ITEM_SMOKEGRENADE)
			{
				gUtil.ClientPrint(Player->edict(), PRINT_CENTER, "#Cstrike_TitlesTXT_Weapon_Not_Available");

				return true;
			}
		}
	}

	return false;
}

bool CPugMod::ClientJoinTeam(CBasePlayer* Player, int NewTeam)
{
	if (Player->m_iTeam == UNASSIGNED)
	{
		gPlayer.TeamInfo(Player->edict(), 33 + 1, "TERRORIST");
		gPlayer.TeamInfo(Player->edict(), 33 + 2, "CT");

		gUtil.SayText(Player->edict(), PRINT_TEAM_RED, _T("%s Build %s (\3%s\1)"), Plugin_info.name, Plugin_info.date, Plugin_info.author);
		gUtil.SayText(Player->edict(), PRINT_TEAM_RED, _T("Say \4.help\1 to view command list."));
	}

	if (NewTeam == 5)
	{
		gUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, _T("Auto Team Select is not allowed."));
		return true;
	}

	if (Player->m_iTeam == NewTeam)
	{
		gUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, _T("You are already on the \3%s\1 team."), PUG_MOD_TEAM_STR[Player->m_iTeam]);
		return true;
	}

	if (NewTeam == TERRORIST || NewTeam == CT)
	{
		if (gPlayer.GetNum((TeamName)NewTeam) >= ((int)gCvars.GetPlayersMax()->value / 2))
		{
			gUtil.SayText(0, (NewTeam == TERRORIST) ? PRINT_TEAM_RED : PRINT_TEAM_BLUE, _T("The \3%s\1 team is complete."), PUG_MOD_TEAM_STR[NewTeam]);
			return true;
		}
	}

	if (Player->m_iTeam == UNASSIGNED || Player->m_iTeam == SPECTATOR)
	{
		if (this->m_State == PUG_STATE_START) 
		{
			if (NewTeam != 6)
			{
				gUtil.SayText(Player->edict(), PRINT_TEAM_GREY, _T("Please, join in \3Spectator\1 while vote session is running."));
				return true;
			}
		}
	}

	if (Player->m_iTeam == TERRORIST || Player->m_iTeam == CT)
	{
		if (this->m_State >= PUG_STATE_START && this->m_State <= PUG_STATE_OVERTIME)
		{
			gUtil.SayText(Player->edict(), PRINT_TEAM_DEFAULT, _T("Cannot switch when the game is started."));
			return true;
		}
	}

	if (NewTeam == 6)
	{
		if (!CVAR_GET_FLOAT("allow_spectators")) 
		{
			if (!gAdmin.Check(Player))
			{
				gUtil.SayText(Player->edict(), PRINT_TEAM_GREY, _T("\3Spectators\1 are not allowed."));
				return true;
			}
		}
	}

	return false;
}

void CPugMod::RoundStart()
{
	if (this->m_State == PUG_STATE_FIRST_HALF || this->m_State == PUG_STATE_SECOND_HALF || this->m_State == PUG_STATE_OVERTIME)
	{
		this->ViewScores(NULL);
	}
}

void CPugMod::RoundEnd(int winStatus, ScenarioEventEndRound event, float tmDelay)
{
	if (this->m_State == PUG_STATE_FIRST_HALF || this->m_State == PUG_STATE_SECOND_HALF || this->m_State == PUG_STATE_OVERTIME)
	{
		if (winStatus != WINSTATUS_NONE)
		{
			if (winStatus == WINSTATUS_TERRORISTS)
			{
				this->m_Round[this->m_State]++;

				this->m_Score[this->m_State][TERRORIST]++;

				gUtil.ClientPrint(NULL, PRINT_CONSOLE, _T("* Round %d won by: %s"), this->GetRound(), PUG_MOD_TEAM_STR[TERRORIST]);
			}
			else if (winStatus == WINSTATUS_CTS)
			{
				this->m_Round[this->m_State]++;

				this->m_Score[this->m_State][CT]++;

				gUtil.ClientPrint(NULL, PRINT_CONSOLE, _T("* Round %d won by: %s"), this->GetRound(), PUG_MOD_TEAM_STR[CT]);
			}
			else if (winStatus == WINSTATUS_DRAW)
			{
				gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, _T("Round %d draw: No clear winner."), this->GetRound());
			}

			if (this->m_State == PUG_STATE_FIRST_HALF)
			{
				if (this->GetRound() >= ((int)gCvars.GetPlayRounds()->value / 2))
				{
					this->NextState(tmDelay);
				}
			}
			else if (this->m_State == PUG_STATE_SECOND_HALF)
			{
				int Half = (int)(gCvars.GetPlayRounds()->value / 2);

				if ((this->GetScores(TERRORIST) > Half) || (this->GetScores(CT) > Half))
				{
					this->NextState(tmDelay);
				}
				else if ((this->GetScores(TERRORIST) == Half) && (this->GetScores(CT) == Half))
				{
					if (gCvars.GetPlayRoundsOvertimeType()->value == 1)
					{
						this->NextState(tmDelay);
					}
				}
			}
			else if (this->m_State == PUG_STATE_OVERTIME)
			{
				if ((this->GetRound() % (int)gCvars.GetPlayRoundsOvertime()->value) == 0)
				{
					this->NextState(tmDelay);
				}
				else if ((this->m_Score[this->m_State][TERRORIST] - this->m_Score[this->m_State][CT]) > gCvars.GetPlayRoundsOvertime()->value)
				{
					this->NextState(tmDelay);
				}
				else if ((this->m_Score[this->m_State][CT] - this->m_Score[this->m_State][TERRORIST]) > gCvars.GetPlayRoundsOvertime()->value)
				{
					this->NextState(tmDelay);
				}
			}
		}
	}
}

void CPugMod::RoundRestart()
{
	if (g_pGameRules)
	{
		if (this->m_State >= PUG_STATE_HALFTIME && this->m_State <= PUG_STATE_OVERTIME)
		{
			if (gCvars.GetShowScoreType()->value > 0)
			{
				if (!CSGameRules()->m_bCompleteReset)
				{
					CSGameRules()->m_iNumCTWins = this->GetScores(CT);
					CSGameRules()->m_iNumTerroristWins = this->GetScores(TERRORIST);

					CSGameRules()->UpdateTeamScores();

					if (gCvars.GetShowScoreType()->value == 2)
					{
						CBasePlayer* Players[MAX_CLIENTS] = { nullptr };

						auto Num = gPlayer.GetList(Players);

						for (int i = 0; i < Num; i++)
						{
							auto Player = Players[i];

							if (Player)
							{
								auto EntityIndex = Player->entindex();

								if (this->m_Frags[EntityIndex] || this->m_Death[EntityIndex])
								{
									Player->m_iDeaths = this->m_Death[EntityIndex];

									Player->edict()->v.frags = 0.0f;

									Player->AddPoints(this->m_Frags[EntityIndex], TRUE);
								}
							}
						}
					}
				}
			}
		}
	}
}