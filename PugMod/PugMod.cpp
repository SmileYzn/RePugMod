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

char* CPugMod::GetStateName()
{
	static char Name[32] = { 0 };

	strcpy(Name, _T(PUG_MOD_STATES_STR[this->m_State]));

	return Name;
}

bool CPugMod::IsLive()
{
	return (this->m_State == PUG_STATE_FIRST_HALF || this->m_State == PUG_STATE_SECOND_HALF || this->m_State == PUG_STATE_OVERTIME);
}

void CPugMod::SetState(int State)
{
	this->m_State = State;

	switch (this->m_State)
	{
	case PUG_STATE_WARMUP:
	{
		gReady.Load();

		gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, _T("%s started, get ready!"), this->GetStateName());
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
				gVoteMap.RandomMap();
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

		gVotePause.Clear();

		gStats.Clear();

		memset(this->m_Round, 0, sizeof(this->m_Round));

		memset(this->m_Score, 0, sizeof(this->m_Score));

		if (this->CheckBalanceTeams())
		{
			this->LO3("3");

			gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, _T("%s started: \3Good Luck & Have Fun!"), this->GetStateName());
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
		gTask.Create(PUG_TASK_EXEC, 5.0f, false, (void*)this->SwapTeams);

		gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, _T("%s started, get ready!"), this->GetStateName());

		if (gPlayer.GetNum() >= (int)gCvars.GetPlayersMin()->value)
		{
			this->NextState(7.0f);
		}
		else
		{
			gReady.Load();
		}

		break;
	}
	case PUG_STATE_SECOND_HALF:
	case PUG_STATE_OVERTIME:
	{
		gReady.Unload();

		if (this->CheckBalanceTeams())
		{
			this->LO3("3");

			gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, _T("%s started: \3Good Luck & Have Fun!"), this->GetStateName());
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

		this->NextState(5.0f);

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
		gTask.Create(PUG_TASK_NEXT, Delay, false, (void*)this->RunState);
	}
	else
	{
		this->RunState();
	}
}

void CPugMod::RunState()
{
	int Next = PUG_STATE_DEAD;

	switch (gPugMod.GetState())
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
		if (gPugMod.GetRound() < (int)gCvars.GetPlayRounds()->value)
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

		int HalfRounds = (int)gCvars.GetPlayRounds()->value / 2;

		if (gPugMod.GetScores(TERRORIST) == HalfRounds && gPugMod.GetScores(CT) == HalfRounds)
		{
			if (gCvars.GetPlayRoundsOvertimeType()->value == 1.0f)
			{
				Next = PUG_STATE_HALFTIME;
			}
		}
		
		break;
	}
	case PUG_STATE_OVERTIME:
	{
		Next = PUG_STATE_HALFTIME;

		if (gPugMod.GetOvertimeWinner())
		{
			Next = PUG_STATE_END;
		}

		break;
	}
	case PUG_STATE_END:
	{
		Next = PUG_STATE_WARMUP;

		if (gCvars.GetVoteMapEnd()->value)
		{
			if (gCvars.GetVoteMapType()->value == 1.0f)
			{
				Next = PUG_STATE_START;
			}
		}

		break;
	}
	}

	gPugMod.SetState(Next);
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

		gUtil.SayText(NULL, Player->entindex(), _T("\3%s\1 started Vote Map."), STRING(Player->edict()->v.netname));

		gVoteMap.Init();

		return true;
	}
	else
	{
		gUtil.SayText(Player->edict(), PRINT_TEAM_RED, _T("Cannot start an vote in \3%s\1 state."), this->GetStateName());
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
		gUtil.SayText(Player->edict(), PRINT_TEAM_RED, _T("Cannot start vote team in \3%s\1 state."), this->GetStateName());
	}

	return false;
}

bool CPugMod::StartMatch(CBasePlayer* Player)
{
	if (this->m_State == PUG_STATE_WARMUP || this->m_State == PUG_STATE_HALFTIME)
	{
		gUtil.SayText(NULL, Player->entindex(), _T("\3%s\1 started match."), STRING(Player->edict()->v.netname));

		this->NextState(1.0);

		return true;
	}
	else
	{
		gUtil.SayText(Player->edict(), PRINT_TEAM_RED, _T("Cannot start match in \3%s\1 state."), this->GetStateName());
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
		gUtil.SayText(Player->edict(), PRINT_TEAM_RED, _T("Cannot stop match in \3%s\1 state."), this->GetStateName());
	}

	return false;
}

bool CPugMod::RestarPeriod(CBasePlayer* Player)
{
	if (this->IsLive())
	{
		if (Player)
		{
			gUtil.SayText(NULL, Player->entindex(), _T("\3%s\1 restarted \4%s\1 period, get ready!."), STRING(Player->edict()->v.netname), this->GetStateName());
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
			gUtil.SayText(Player->edict(), PRINT_TEAM_RED, _T("Cannot restart period in \3%s\1 state."), this->GetStateName());
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

TeamName CPugMod::GetWinner()
{
	if (this->GetScores(TERRORIST) != this->GetScores(CT))
	{
		return (this->GetScores(TERRORIST) > this->GetScores(CT)) ? TERRORIST : CT;
	}

	return UNASSIGNED;
}

bool CPugMod::GetOvertimeWinner()
{
	if (this->GetScores(CT) != this->GetScores(TERRORIST))
	{
		int OvertimeHalfRounds = ((int)gCvars.GetPlayRoundsOvertime()->value / 2);
		int FinishedHalves = this->m_Round[PUG_STATE_OVERTIME] / OvertimeHalfRounds;

		int FinishedOvertimes = FinishedHalves / 2;

		if (FinishedHalves > 0 && FinishedHalves % 2 == 0 && this->m_Round[PUG_STATE_OVERTIME] % (OvertimeHalfRounds * 2) == 0)
		{
			return true;
		}
		else
		{
			int OvertimeScoreCT = this->GetScores(CT) - (int)gCvars.GetPlayRounds()->value / 2;
			int OvertimeScoreT = this->GetScores(TERRORIST) - (int)gCvars.GetPlayRounds()->value / 2;

			int CurrentOvertimeScoreCT = OvertimeScoreCT - (FinishedOvertimes * OvertimeHalfRounds);
			int CurrentOvertimeScoreT = OvertimeScoreT - (FinishedOvertimes * OvertimeHalfRounds);

			if (CurrentOvertimeScoreCT > OvertimeHalfRounds ||
				CurrentOvertimeScoreT > OvertimeHalfRounds) return true;
		}
	}

	return false;


}

void CPugMod::Help(CBasePlayer* Player, bool AdminHelp)
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
		this->GetStateName(),
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

void CPugMod::LO3(const char* Time)
{
	auto Delay = std::atof(Time);

	if (Delay >= 1.0f && Delay <= 3.0f)
	{
		if (g_pGameRules)
		{
			CSGameRules()->m_bGameStarted = true;
		}

		CVAR_SET_STRING("sv_restart", Time);

		char Parameter[8] = { 0 };

		snprintf(Parameter, sizeof(Parameter), "%d", (int)(Delay - 1.0f));

		gTask.Create(PUG_TASK_LO3R, (Delay + 1.0f), false, (void*)gPugMod.LO3, Parameter);
	}
	else
	{
		gUtil.HudMessage(NULL, gUtil.HudParam(0, 255, 0, -1.0, 0.2, 0, 10.0, 10.0), _T("--- MATCH IS LIVE ---"));
	}
}

void CPugMod::SwapTeams()
{
	int OvertimeRounds = gPugMod.GetRound() - (int)gCvars.GetPlayRounds()->value;

	if (OvertimeRounds >= 0 && OvertimeRounds % (int)gCvars.GetPlayRoundsOvertime()->value == 0 && !gCvars.GetPlayRoundsOvertimeSwap()->value)
	{
		return;
	}

	gPugMod.SwapScores();

	if (g_pGameRules)
	{
		CSGameRules()->SwapAllPlayers();
	}

	gUtil.SayText(NULL, PRINT_TEAM_DEFAULT, _T("Changing teams automatically."));
}

void CPugMod::SwapScores()
{
	for (int iState = PUG_STATE_FIRST_HALF; iState <= PUG_STATE_OVERTIME; iState++)
	{
		std::swap(this->m_Score[iState][TERRORIST], this->m_Score[iState][CT]);
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

void CPugMod::ClientDisconnected(int EntityIndex)
{
	if (this->m_State >= PUG_STATE_FIRST_HALF && this->m_State <= PUG_STATE_OVERTIME)
	{
		if (gPlayer.GetNum() < (gCvars.GetPlayersMin()->value / 2))
		{
			this->SetState(PUG_STATE_END);
		}
	}
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
		if (!Player->IsBot())
		{
			gPlayer.TeamInfo(Player->edict(), MAX_CLIENTS + 1 + TERRORIST, "TERRORIST");
			gPlayer.TeamInfo(Player->edict(), MAX_CLIENTS + 1 + CT, "CT");

			gUtil.SayText(Player->edict(), PRINT_TEAM_RED, _T("%s Build %s (\3%s\1)"), Plugin_info.name, Plugin_info.date, Plugin_info.author);
			gUtil.SayText(Player->edict(), PRINT_TEAM_RED, _T("Say \4.help\1 to view command list."));
		}
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
	if (this->IsLive())
	{
		this->ViewScores(NULL);
	}
}

void CPugMod::RoundEnd(int winStatus, ScenarioEventEndRound event, float tmDelay)
{
	if (this->IsLive())
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
					float OvertimeType = gCvars.GetPlayRoundsOvertimeType()->value;

					if (OvertimeType == 1.0f || OvertimeType == 2.0f)
					{
						this->NextState(tmDelay);
					}
					else if (OvertimeType == 3.0f)
					{
						gVoteOvertime.Init();
					}
				}
			}
			else if (this->m_State == PUG_STATE_OVERTIME)
			{
				if (this->GetOvertimeWinner())
				{
					this->NextState(tmDelay);
				}
				else if (this->m_Round[this->m_State] >= 0 && this->m_Round[this->m_State] % ((int)gCvars.GetPlayRoundsOvertime()->value / 2) == 0)
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
		if (this->m_State >= PUG_STATE_HALFTIME && this->m_State <= PUG_STATE_END)
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
						CBasePlayer* Players[MAX_CLIENTS] = { NULL };

						auto Num = gPlayer.GetList(Players, true);

						for (int i = 0; i < Num; i++)
						{
							auto Player = Players[i];

							if (Player)
							{
								auto Stats = gStats.GetData(Player->entindex());

								if (Stats.Frags || Stats.Deaths)
								{
									Player->m_iDeaths = Stats.Deaths;

									Player->edict()->v.frags = Stats.Frags;

									Player->AddPoints(0, TRUE);
								}
							}
						}
					}
				}
			}
		}
	}
}