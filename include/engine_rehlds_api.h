#ifndef _INCLUDE_REHLDS_API_H_
#define _INCLUDE_REHLDS_API_H_

#pragma once

extern IRehldsApi* g_RehldsApi;
extern IRehldsHookchains* g_RehldsHookchains;
extern bool RehldsApi_Init();

typedef struct packet_entities_s {
	int num_entities;
	unsigned char flags[32];
	entity_state_t *entities;
} packet_entities_t;

extern int SV_CreatePacketEntities(IRehldsHook_SV_CreatePacketEntities *chain, sv_delta_t type, IGameClient *client, packet_entities_t *to, sizebuf_t *msg);

#endif //_INCLUDE_REHLDS_API_H_
