#ifndef _INCLUDE_PLAYER_H_
#define _INCLUDE_PLAYER_H_

class CGamePlayer
{

public:

	void Player_Clear();
	void Init(edict_t *pEdictList, int index);

	int GetIndex() const					{ return m_iIndex; }
	float GetDiff(int object_index) const	{ return m_fDiff[ object_index ]; }
	float *GetDiff(CGamePlayer *pObject)	{ return &m_fDiff[ pObject->m_iIndex ]; }
	bool GetSolid(int object_index) const	{ return m_bSolid[ object_index ]; }
	bool *GetSolid(CGamePlayer *pObject)	{ return &m_bSolid[ pObject->m_iIndex ]; }
	bool GetCrouch(int object_index) const	{ return m_bCrouch[ object_index ]; }
	bool *GetCrouch(CGamePlayer *pObject)	{ return &m_bCrouch[ pObject->m_iIndex ]; }
	bool GetDont() const					{ return m_bDont; }

	void SetDont(bool val)					{ m_bDont = val; }

private:

	int m_iIndex;
	bool m_bDont;
	bool m_bSolid[MAX_CLIENTS + 1];
	bool m_bCrouch[MAX_CLIENTS + 1];
	float m_fDiff[MAX_CLIENTS + 1];

};

extern CGamePlayer g_Players[MAX_CLIENTS];

inline edict_t *EDICT(int index)
{
	return (edict_t *)(g_GameData.GetStartEdict() + index);
}

inline int EDICT_NUM(const edict_t *pEntity)
{
	return (int)(pEntity - g_GameData.GetStartEdict());
}

inline CGamePlayer *PLAYER_FOR_EDICT(const edict_t *pEntity)
{
	return &g_Players[ EDICT_NUM(pEntity) - 1 ];
}

inline CGamePlayer *PLAYER_FOR_NUM(int index)
{
	return &g_Players[ index ];
}

#endif //_INCLUDE_PLAYER_H_
