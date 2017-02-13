#ifndef _INCLUDE_GAME_DATA_H_
#define _INCLUDE_GAME_DATA_H_

class CGameData
{

public:

	void Init();
	void SetMaxClients(int num)				{ MaxClients = num; }
	void SetMaxClientEdict(edict_t *pMax)	{ m_pMaxClientEdict = pMax; }
	void SetStartEdict(edict_t *pStart)		{ m_pStartEdict = pStart; }
	void SetStartEnt(edict_t *pEnt)			{ m_pStartEnt = pEnt; }

	int GetMaxClients() const				{ return MaxClients; }
	edict_t *GetMaxClientEdict() const		{ return m_pMaxClientEdict; }
	edict_t *GetStartEnt() const			{ return m_pStartEnt; }
	edict_t *GetStartEdict() const			{ return m_pStartEdict; }

private:

	edict_t *m_pStartEdict;
	edict_t *m_pStartEnt;
	edict_t *m_pMaxClientEdict;
	int MaxClients;

};

extern CGameData g_GameData;

#endif //_INCLUDE_GAME_DATA_H_
