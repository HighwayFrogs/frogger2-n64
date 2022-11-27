/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: savegame.h
	Programmer	: James Healey
	Date		: 08/06/99

----------------------------------------------------------------------------------------------- */

#ifndef SAVEGAME_H_INCLUDED
#define SAVEGAME_H_INCLUDED


//***********************************
// Defines

#define NUM_SAVE_SLOTS		6

#define EEPROM_IDLE				0
#define EEPROM_SAVELEVELSCORES	1
#define EEPROM_LOADLEVELSCORES	2
#define EEPROM_SAVEGAME			3
#define EEPROM_LOADGAME			4
#define EEPROM_VALID			8
#define EEPROM_SAVEID			9

#define MAX_EEPROM_MESSAGES	6

/*#define NUM_LEVEL_TIMES		24
#define NUM_HISCORE_ENTRIES	6

#define EEPROM_LOADHISCORES		2
#define EEPROM_SAVETIMES		4
#define EEPROM_LOADTIMES		5
#define EEPROM_SAVESLOT			6
#define EEPROM_LOADSLOT			7
#define EEPROM_LOADCRC			10
#define EEPROM_SAVECRC			11


	  */
typedef struct 
{
	char name[3];
	int score;
} LEVEL_HISCORE;

/*
typedef struct
{
	// Player
	char	name[3];
	long	score;
	char	livesnhealth; // 6-2 l-h 

	// State
//	char	worldState[MAX_WORLDS]; // 2-2-2-2 l-l-l-B   0 = Closed; 1 = gold; 2= silv 3  = bronz... The first "closed" world is in fact open but uncompleted
//	short 	bonuses; // Each bit is whether bonus is open


	short	currentWorld;
	short	currentLevel;

} SAVE_SLOT;
*/


/*typedef struct
{
	short	time;
	char	name[3];
}LEVEL_TIME;

typedef struct
{
	char	name[4];
	int		score;
} HISCORE_ENTRY;



/*typedef struct
{
	unsigned int		slotCRC[NUM_SAVE_SLOTS];
	unsigned int		hiscoreCRC;
	unsigned int		bestTimesCRC;
}EEPROM_CRCS;

//void InitEeprom();
//void LoadHiscoreTable();
//void LoadBestTimes();
//void SaveHiscoreTable();
//void SaveBestTimes();
//void EepromSaveHiscoreTable();
//void EepromLoadHiscoreTable();
//void EepromSaveBestTimes();
//void EepromLoadBestTimes();
//void LoadSlots();
//void SaveSlots();
//void SaveID();
//void EepromSaveSlots();
//void EepromLoadSlots();
//void EepromValid();
//void EepromSaveID();
//void LoadCRC();
//void SaveCRC();
//void CheckCRC();





/*extern HISCORE_ENTRY	hiscoreTable[NUM_HISCORE_ENTRIES];
extern HISCORE_ENTRY	hiscoreTableCopy[NUM_HISCORE_ENTRIES];
extern short		currentSaveSlot;

extern char		currentName[];

extern short		eepromPresent;
//extern OSMesgQueue	eepromMsgQ;
//extern OSMesg		eepromMsgBuf;



*/

//***********************************
// Global Externs

extern char	validEeprom;

extern char	eepromMessageQueue[];
extern char	eepromMessageNum;

extern short eepromPresent;

extern LEVEL_HISCORE	levelTable [ MAX_WORLDS*3 ];
//extern SAVE_SLOT		saveSlot [ NUM_SAVE_SLOTS ];

//***********************************
// Function Prototypes

extern void Wait ( short time );

extern void InitEepromMessageQueue	( void );
extern void InitEeprom				( void );
extern void GetEepromMessage		( void );
extern void PostEepromMessage		( short message );

extern void SaveID					( void );
extern void EepromSaveID			( void );
extern void EepromValid				( void );

extern void SaveLevelScores			( void );
extern void EepromSaveLevelScores	( void );

extern void LoadLevelScores			( void );
extern void EepromLoadLevelScores	( void );

extern void SaveGame();
extern void EepromSaveGame();

extern void LoadGame();
extern void EepromLoadGame();

extern void StoreSaveSlot( int p, int s );
extern void ReadSaveSlot( int p, int s );


/*
void GetEepromMessage();
void PostEepromMessage(short message);
void UpdateSaveSlot(char conquered);

	 */

#endif