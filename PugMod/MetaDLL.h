#pragma once

#pragma region DLL_PRE
#pragma endregion

#pragma region DLL_POST
void DLL_POST_ServerActivate(edict_t *pEdictList, int edictCount, int clientMax);
void DLL_POST_ServerDeactivate(void);
void DLL_POST_StartFrame(void);
#pragma endregion