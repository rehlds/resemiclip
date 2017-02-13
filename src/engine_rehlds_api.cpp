#include "precompiled.h"

IRehldsApi* g_RehldsApi;
IRehldsHookchains* g_RehldsHookchains;

bool RehldsApi_TryInit(CSysModule* engineModule, char* failureReason)
{
	if (!engineModule) {
		SEM_PRINT("[%s]: ReHLDS Failed to locate engine module.", Plugin_info.logtag);
		return false;
	}

	CreateInterfaceFn ifaceFactory = Sys_GetFactory(engineModule);

	if (!ifaceFactory) {
		SEM_PRINT("[%s]: ReHLDS Failed to locate interface factory in engine module.", Plugin_info.logtag);
		return false;
	}

	int retCode = 0;

	g_RehldsApi = (IRehldsApi*)ifaceFactory(VREHLDS_HLDS_API_VERSION, &retCode);

	if (!g_RehldsApi) {
		SEM_PRINT("[%s]: ReHLDS Failed to locate retrieve rehlds api interface from engine module, return code is %d.", Plugin_info.logtag, retCode);
		return false;
	}

	int majorVersion = g_RehldsApi->GetMajorVersion();
	int minorVersion = g_RehldsApi->GetMinorVersion();

	if (majorVersion != REHLDS_API_VERSION_MAJOR)
	{
		SEM_PRINT("[%s]: ReHLDS API major version mismatch; expected %d, real %d.", Plugin_info.logtag, REHLDS_API_VERSION_MAJOR, majorVersion);

		// need to notify that it is necessary to update the ReHLDS.
		if (majorVersion < REHLDS_API_VERSION_MAJOR)
		{
			SEM_PRINT("[%s]: Please update the ReHLDS up to a major version API >= %d.", Plugin_info.logtag, REHLDS_API_VERSION_MAJOR);
		}

		// need to notify that it is necessary to update the module.
		else if (majorVersion > REHLDS_API_VERSION_MAJOR)
		{
			SEM_PRINT("[%s]: Please update the %s up to a major version API >= %d.", Plugin_info.logtag, Plugin_info.name, majorVersion);
		}

		return false;
	}

	if (minorVersion < REHLDS_API_VERSION_MINOR)
	{
		SEM_PRINT("[%s]: ReHLDS API minor version mismatch; expected at least %d, real %d.", Plugin_info.logtag, REHLDS_API_VERSION_MINOR, minorVersion);
		SEM_PRINT("[%s]: Please update the ReHLDS up to a minor version API >= %d.", Plugin_info.logtag, REHLDS_API_VERSION_MINOR);
		return false;
	}

	g_RehldsHookchains = g_RehldsApi->GetHookchains();

	return true;
}

bool RehldsApi_Init()
{
	char failReason[2048];

#ifdef WIN32
	CSysModule* engineModule = Sys_LoadModule("swds.dll");
	if (!RehldsApi_TryInit(engineModule, failReason))
	{
		engineModule = Sys_LoadModule("filesystem_stdio.dll");
		if (!RehldsApi_TryInit(engineModule, failReason))
		{
			UTIL_LogPrintf("%s", failReason);
			return false;
		}
	}
#else
	CSysModule* engineModule = Sys_LoadModule("engine_i486.so");
	if (!RehldsApi_TryInit(engineModule, failReason))
	{
		UTIL_LogPrintf("%s", failReason);
		return false;
	}
#endif

	return true;
}

void UTIL_LogPrintf(const char *fmt, ...)
{
	va_list argptr;
	char string[1024];

	va_start(argptr, fmt);
	vsprintf(string, fmt, argptr);
	va_end(argptr);

	ALERT(at_logged, "%s", string);
}
