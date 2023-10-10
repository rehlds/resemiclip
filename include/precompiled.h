#pragma once

#include <extdll.h>
#include <meta_api.h>

#include "version/appversion.h"

#include "cbase.h"
#include "entity_state.h"
#include "pm_defs.h"

#include "main.h"

#include "rehlds_api.h"
#include "regamedll_api.h"

#include "gamedll_api.h"
#include "engine_rehlds_api.h"

#include "game_data.h"
#include "config.h"
#include "player.h"

#undef DLLEXPORT

#ifdef _WIN32
	#define DLLEXPORT __declspec(dllexport)
	#define NOINLINE __declspec(noinline)
#else
	#define DLLEXPORT __attribute__((visibility("default")))
	#define NOINLINE __attribute__((noinline))
	#define WINAPI /* */
#endif
