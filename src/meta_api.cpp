#include "precompiled.h"

plugin_info_t Plugin_info = {
	META_INTERFACE_VERSION,
	"ReSemiclip",
	"2.3.9",
	"13/02/17",
	"s1lent & Adidasman",
	"http://www.dedicated-server.ru/",
	"ReSemiclip",
	PT_CHANGELEVEL,
	PT_ANYTIME
};

meta_globals_t *gpMetaGlobals;
mutil_funcs_t *gpMetaUtilFuncs;
DLL_FUNCTIONS gpFunctionTable;
DLL_FUNCTIONS gpFunctionTable_Post;
DLL_FUNCTIONS *g_pFunctionTable;
DLL_FUNCTIONS *g_pFunctionTable_Post;
META_FUNCTIONS gMetaFunctionTable;
NEW_DLL_FUNCTIONS *g_pNewFunctionTable;

bool g_bReHLDS = false;
bool g_bReGameDLL = false;

C_DLLEXPORT int GetEntityAPI2(DLL_FUNCTIONS *pFunctionTable, int *)
{
	memset(&gpFunctionTable, 0, sizeof(DLL_FUNCTIONS));

	gpFunctionTable.pfnSpawn = DispatchSpawn;
	gpFunctionTable.pfnPM_Move = PM_Move;
	gpFunctionTable.pfnClientDisconnect = ClientDisconnect;

	memcpy(pFunctionTable, &gpFunctionTable, sizeof(DLL_FUNCTIONS));

	g_pFunctionTable = pFunctionTable;

	return TRUE;
}

C_DLLEXPORT int GetEntityAPI2_Post(DLL_FUNCTIONS *pFunctionTable, int *)
{
	memset(&gpFunctionTable_Post, 0, sizeof(DLL_FUNCTIONS));

	gpFunctionTable_Post.pfnServerActivate = ServerActivate_Post;
	gpFunctionTable_Post.pfnServerDeactivate = ServerDeactivate_Post;

	memcpy(pFunctionTable, &gpFunctionTable_Post, sizeof(DLL_FUNCTIONS));

	g_pFunctionTable_Post = pFunctionTable;

	return TRUE;
}

C_DLLEXPORT int Meta_Query(char *, plugin_info_t **pPlugInfo, mutil_funcs_t *pMetaUtilFuncs)
{
	*pPlugInfo = &(Plugin_info);

	gpMetaUtilFuncs = pMetaUtilFuncs;

	return TRUE;
}

static NEW_DLL_FUNCTIONS gNewFunctionsTable = {
	nullptr,	//void (*pfnOnFreeEntPrivateData)(edict_t *pEnt);
	nullptr,	//void (*pfnGameShutdown)(void);
	&ShouldCollide,	//int (*pfnShouldCollide)( edict_t *pentTouched, edict_t *pentOther );
	nullptr,	//void (*pfnCvarValue)( const edict_t *pEnt, const char *value );
	nullptr		//void (*pfnCvarValue2)( const edict_t *pEnt, int requestID, const char *cvarName, const char *value );
};

C_DLLEXPORT int GetNewDLLFunctions(NEW_DLL_FUNCTIONS *pNewFunctionTable, int *interfaceVersion)
{
	if (!pNewFunctionTable) {
		LOG_ERROR(PLID, "GetNewDLLFunctions called with null pFunctionTable");
		return FALSE;
	}
	else if (*interfaceVersion != NEW_DLL_FUNCTIONS_VERSION)
	{
		LOG_ERROR(PLID, "GetNewDLLFunctions version mismatch; requested=%d ours=%d", *interfaceVersion, NEW_DLL_FUNCTIONS_VERSION);

		//! Tell metamod what version we had, so it can figure out who is out of date.
		*interfaceVersion = NEW_DLL_FUNCTIONS_VERSION;

		return FALSE;
	}

	memcpy(pNewFunctionTable, &gNewFunctionsTable, sizeof(NEW_DLL_FUNCTIONS));

	g_pNewFunctionTable = pNewFunctionTable;

	return 1;
}

C_DLLEXPORT int Meta_Attach(PLUG_LOADTIME now, META_FUNCTIONS *pFunctionTable, meta_globals_t *pMGlobals, gamedll_funcs_t *pGamedllFuncs)
{
	if (!pMGlobals) {
		LOG_ERROR(PLID, "Meta_Attach called with null pMGlobals");
		return FALSE;
	}

	if (!pFunctionTable) {
		LOG_ERROR(PLID, "Meta_Attach called with null pFunctionTable");
		return FALSE;
	}

	gpMetaGlobals = pMGlobals;

	g_bReHLDS = RehldsApi_Init();

	if (!g_bReHLDS) {
		return FALSE;
	}

	g_bReGameDLL = RegamedllApi_Init();

	if (!g_bReGameDLL) {
		return FALSE;
	}

	if(!OnMetaAttach()) {
		return FALSE;
	}

	gMetaFunctionTable.pfnGetEntityAPI2 = GetEntityAPI2;
	gMetaFunctionTable.pfnGetEntityAPI2_Post = GetEntityAPI2_Post;
	gMetaFunctionTable.pfnGetNewDLLFunctions = GetNewDLLFunctions;

	memcpy(pFunctionTable, &gMetaFunctionTable, sizeof(META_FUNCTIONS));

	return TRUE;
}

C_DLLEXPORT int Meta_Detach(PLUG_LOADTIME now, PL_UNLOAD_REASON reason)
{
	if (now > Plugin_info.unloadable && reason != PNL_CMD_FORCED)
	{
		LOG_ERROR(PLID, "Can't unload plugin right now");
		return FALSE;
	}

	if (g_bReHLDS && g_bReGameDLL)
	{
		OnMetaDetach();
	}

	return TRUE;
}
