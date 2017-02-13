#ifndef _INCLUDE_MAIN_H_
#define _INCLUDE_MAIN_H_

#define MIN_AMOUNT				100.0f
#define FLOAT_CROUCH			49.9f
#define MAX_CLIENTS				32
#define GET_DISTANCE(a, b)		((a - b).Length2D())
#define GET_COLLIDE(a, b)		(abs(a.z - b.z) < 96 && (a - b).Length2D() < 96)

extern int DispatchSpawn(edict_t *pEntity);
extern int ShouldCollide(edict_t *pentTouched, edict_t *pentOther);
extern int OnMetaAttach();
extern void OnMetaDetach();
extern void SVR_SemiclipOption();
extern void ClientDisconnect(edict_t *pEnt);
extern void PM_Move(playermove_t *pmove,int);
extern void ServerActivate_Post(edict_t *pEdictList,int edictCount,int clientMax);
extern void ServerDeactivate_Post();
extern void SEM_PRINT(const char *fmt, ...);
extern void UTIL_LogPrintf(const char *fmt, ...);
extern DLL_FUNCTIONS *g_pFunctionTable;
extern NEW_DLL_FUNCTIONS *g_pNewFunctionTable;

#endif //_INCLUDE_MAIN_H_
