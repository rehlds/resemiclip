#include "precompiled.h"

static char mPluginPath[256];
static char mConfigPath[256];

static inline int IsCharSpecial(char j)
{
	return (j == ' ' || j == '"' || j == ';' || j == '\t' || j == '\r' || j == '\n');
}

void TrimSpace(char *pneedle)
{
	char *phaystack = pneedle;
	char *pbuf = pneedle;

	while(IsCharSpecial(*pbuf)) {
		++pbuf;
	}

	while(*pbuf) {
		*phaystack++ = *pbuf++;
	}

	*phaystack = '\0';

	while(phaystack > pneedle && *--phaystack && IsCharSpecial(*phaystack)) {
		*phaystack = '\0';
	}
}

static inline int clamp(int value,int _min,int _max)
{
	return value < _min ? _min : (value > _max ? _max : value);
}

static inline float clamp(double value,float _min,float _max)
{
	return value < _min ? _min : (value > _max ? _max : value);
}

bool CConfig::Parse_Settings(const char *str, const char *value)
{
	int i = atoi(value);
	double f = atof(value);

	if (!strcasecmp(str, "semiclip")) {
		cfg_enable = clamp(i, 0, 1);
	}
	else if (!strcasecmp(str, "crouch")) {
		cfg_crouch = clamp(i, 0, 1);
	}
	else if (!strcasecmp(str, "distance")) {
		cfg_distance = clamp(f, 64.0, 250.0);
	}
	else if (!strcasecmp(str, "transparency")) {
		cfg_transparency = clamp(i, 0, 255);
	}
	else if (!strcasecmp(str, "time")) {
		cfg_time = clamp(f, 0.0, 180.0);
	}
	else if (!strcasecmp(str, "effects")) {
		cfg_effects = clamp(i, 0, 1);
	}
	else if (!strcasecmp(str, "team")) {
		cfg_team = clamp(i, 0, 3);
	}
	else if (!strcasecmp(str, "penetfire")) {
		cfg_penetfire = clamp(i, 0, 1);
	}
	else
	{
		return false;
	}

	return true;
}

void Print_Settings()
{
	static const char *szConditon[] = {
		" (for all)",
		" (only Terrorists)",
		" (only Counter-Terrorists)",
		" (only teammates)"
	};

	SEM_PRINT("\n\nusage: semiclip_option\n\n [command]	[value]   [description]\n");
	SEM_PRINT(" semiclip	%d	- enable/disable semiclip", g_Config.GetEnable());
	SEM_PRINT(" team		%d	- condition for teams %s", g_Config.GetTeam(), szConditon[ g_Config.GetTeam() ]);
	SEM_PRINT(" time		%0.0f	- how many time in seconds semiclip will work from the beginning of the round", g_Config.GetTime());
	SEM_PRINT(" crouch		%d	- allows jump to crouching players when semiclip works", g_Config.GetCrouch());
	SEM_PRINT(" distance	%0.0f	- at what distance player can have transparency and semiclip", g_Config.GetDistance());
	SEM_PRINT(" effects	%d	- effect of transparency of the player. Depends from distance between players", g_Config.GetEffects());
	SEM_PRINT(" transparency	%d	- transparency of the player", g_Config.GetTransparency());
	SEM_PRINT(" penetfire	%d	- Allow fire penetration through transparent the of teammates", g_Config.GetPenetFire());
}

static int Parse_Config(const char *path)
{
	FILE *fp;

	fp = fopen(path, "rt");

	if(!fp) {
		return FALSE;
	}

	char *value;
	char buf[256];

	while(!feof(fp))
	{
		if(!fgets(buf, sizeof(buf) - 1, fp)) {
			break;
		}

		value = strchr(buf, '=');

		if(value == nullptr) {
			continue;
		}

		*(value++) = '\0';

		TrimSpace(buf);
		TrimSpace(value);

		if (*buf == '\0' || *value == '\0' || g_Config.Parse_Settings(buf, value)) {
			continue;
		}
	}

	return (fclose(fp) != EOF);
}

int Load_Config_Maps()
{
	char *tempMap;
	char path[256];
	char mapName[32];

	Parse_Config(mConfigPath);

	strncpy(mapName, STRING(gpGlobals->mapname), sizeof(mapName) - 1);
	tempMap = strchr(mapName, '_');

	if(tempMap != nullptr)
	{
		*tempMap = '\0';

		snprintf(path, sizeof(path) - 1, "%smaps/prefix_%s.ini", mPluginPath, mapName);

		Parse_Config(path);
	}

	snprintf(path, sizeof(path) - 1, "%smaps/%s.ini", mPluginPath, STRING(gpGlobals->mapname));

	Parse_Config(path);

	g_RehldsHookchains->SV_CreatePacketEntities()->unregisterHook(&SV_CreatePacketEntities);

	g_ReGameHookchains->CSGameRules_OnRoundFreezeEnd()->unregisterHook(&CSGameRules_OnRoundFreezeEnd);
	g_ReGameHookchains->CBasePlayer_Spawn()->unregisterHook(&CBasePlayer_Spawn);
	g_ReGameHookchains->CBasePlayer_TraceAttack()->unregisterHook(&CBasePlayer_TraceAttack);
	g_ReGameHookchains->RadiusFlash_TraceLine()->unregisterHook(&RadiusFlash_TraceLine);

	g_pFunctionTable->pfnPM_Move = nullptr;
	g_pNewFunctionTable->pfnShouldCollide = nullptr;

	if (g_Config.GetEnable())
	{
		if (g_Config.GetTime()) {
			g_ReGameHookchains->CSGameRules_OnRoundFreezeEnd()->registerHook(&CSGameRules_OnRoundFreezeEnd, HC_PRIORITY_DEFAULT + 1);
		}
		else
		{
			g_RehldsHookchains->SV_CreatePacketEntities()->registerHook(&SV_CreatePacketEntities, HC_PRIORITY_HIGH);

			g_ReGameHookchains->CBasePlayer_Spawn()->registerHook(&CBasePlayer_Spawn, HC_PRIORITY_DEFAULT + 1);

			g_pFunctionTable->pfnPM_Move = PM_Move;
			g_pNewFunctionTable->pfnShouldCollide = ShouldCollide;
		}

		if (g_Config.GetPenetFire()) {
			g_ReGameHookchains->CBasePlayer_TraceAttack()->registerHook(&CBasePlayer_TraceAttack, HC_PRIORITY_DEFAULT + 1);
		}

		g_ReGameHookchains->RadiusFlash_TraceLine()->registerHook(&RadiusFlash_TraceLine, HC_PRIORITY_HIGH);
	}

	return TRUE;
}

int Load_Config()
{
	char *pos;
	strcpy(mConfigPath, GET_PLUGIN_PATH(PLID));
	pos = strrchr(mConfigPath, '/');

	if(pos == nullptr || *pos == '\0') {
		return FALSE;
	}

	*(pos + 1) = '\0';

	strncpy(mPluginPath, mConfigPath, sizeof(mPluginPath) - 1);
	strcat(mConfigPath, CONFIG_FILE);

	g_Config.SetEnable(1);
	g_Config.SetTime(0.0f);
	g_Config.SetTeam(3);
	g_Config.SetCrouch(0);
	g_Config.SetDistance(160.0f);
	g_Config.SetTransparency(100);
	g_Config.SetPenetFire(0);

	return TRUE;
}

void SEM_PRINT(const char *fmt, ...)
{
	va_list ap;
	uint32 len;
	char buf[1048];

	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);

	len = strlen(buf);

	if (len < sizeof(buf) - 2) {
		strcat(buf, "\n");
	} else {
		buf[len - 1] = '\n';
	}

	SERVER_PRINT(buf);
}
