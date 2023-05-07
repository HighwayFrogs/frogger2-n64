/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: map.h
	Programmer	: Matthew Cloy
	Date		: 04/12/98

----------------------------------------------------------------------------------------------- */

#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

enum
{
	RETRO1_MAP_BANK,
	RETRO2_MAP_BANK,
	RETRO3_MAP_BANK,

	VENICE1_MAP_BANK,
};

extern char *mapBank;

// global ptrs to banks for freeing allocated resources
extern char *aMapBank;
extern char *globalPtrEntityBank;

extern unsigned long numSafe;
extern unsigned long numPwrups;

extern GAMETILE **bTSafe;
extern GAMETILE **pwrupTStart;


extern SCENIC *Sc_000;

#include "levbanks\\levext.h"

void LoadMapBank(short num);
void FreeMapBank();

void LoadLevelEntities(short worldID,short levelID);
void FreeLevelEntitys();

void LoadLevelScript(int worldID,int levelID);


void LoadCollision(short cNum);
void LoadScenics(short sNum);

void MakeTeleportTile(GAMETILE *fromTile,GAMETILE *toTile,char teleportType);
BOOL IsATeleportTile(GAMETILE *tile);
void TeleportActorToTile(ACTOR2 *act,GAMETILE *tile,long pl);


#endif
