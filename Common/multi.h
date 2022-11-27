#ifndef __MULTI_H
#define __MULTI_H


#define MULTIMODE_RACE				1
#define MULTIMODE_BATTLE			2
#define MULTIMODE_COLLECT			3


#define MULTI_BATTLE_MAXITEMS		15
#define MULTI_BATTLE_TRAILLENGTH	8

#define MULTI_NUM_CHARS				9

#define MULTI_RACE_TIMEPENALTY		180

typedef struct
{
	short wins;				// Race and battle
	short lap;

	unsigned long timer;	// Race
	unsigned long penalty;	// Race

	union
	{
		short check;		// Race
		short score;		// Battle
	};

	short trail;			// Battle

	unsigned char r, g, b;	// Both

	char ready;				// Both
	short lasttile;			// Battle

	AIPATHNODE *path;		// Battle

} MPINFO;


extern TIMER multiTimer,endTimer;
extern char matchWinner;
extern MPINFO mpl[];
extern int multiplayerMode;
extern unsigned long numMultiItems;

extern char multiLevelIndex[];

void RunMultiplayer( );
void ResetMultiplayer( );

void RaceRespawn( int pl );
void PickupBabyFrogMulti( ENEMY *baby, int pl );
void KillMPFrog(int pl);

void CalcMPCamera( VECTOR *target );

#endif