#ifndef _INCLUDE_CONFIG_H_
#define _INCLUDE_CONFIG_H_

#define CONFIG_FILE "config.ini"

class CConfig
{

public:

	CConfig()
	{
		cfg_count = 0.0f;
		cfg_distance = 160.0f;
		cfg_time = 0.0f;
		cfg_team = 3;
		cfg_crouch = 0;
		cfg_effects = 1;
		cfg_transparency = 100;
		cfg_penetfire = 0;
		cfg_enable = 1;
	}

	bool Parse_Settings(const char *str, const char *value);

	float GetCount() const			{ return cfg_count; }
	float GetDistance() const		{ return cfg_distance; }
	float GetTime() const			{ return cfg_time; }
	int GetTeam() const				{ return cfg_team; }
	int GetCrouch() const			{ return cfg_crouch; }
	int GetEffects() const			{ return cfg_effects; }
	int GetTransparency() const		{ return cfg_transparency; }
	int GetPenetFire() const		{ return cfg_penetfire; }
	int GetEnable() const			{ return cfg_enable; }

	void SetCount(float fVal)		{ cfg_count = fVal; }
	void SetDistance(float fVal)	{ cfg_distance = fVal; }
	void SetTime(float fVal)		{ cfg_time = fVal; }
	void SetTeam(int iVal)			{ cfg_team = iVal; }
	void SetCrouch(int iVal)		{ cfg_crouch = iVal; }
	void SetEffects(int iVal)		{ cfg_effects = iVal; }
	void SetTransparency(int iVal)	{ cfg_transparency = iVal; }
	void SetPenetFire(int iVal)		{ cfg_penetfire = iVal; }
	void SetEnable(int iVal)		{ cfg_enable = iVal; }

private:

	float cfg_count;
	float cfg_distance;
	float cfg_time;
	int cfg_team;
	int cfg_crouch;
	int cfg_effects;
	int cfg_transparency;
	int cfg_penetfire;
	int cfg_enable;

};

extern CConfig g_Config;
extern int Load_Config();
extern int Load_Config_Maps();
extern void Print_Settings();

#endif //_INCLUDE_CONFIG_H_
