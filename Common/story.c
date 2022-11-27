/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: sprite.c
	Programmer	: Jim Hubbard
	Date		: 25/02/00

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>
#include "incs.h"
#include "gelf.h"

#define NUM_CHAPTERS 7
#define IMAGETIME_SECONDS 10

char chapterNumImages[NUM_CHAPTERS] = 
{	5,3,3,4,2,6,3,	};

unsigned long levelToPlay[NUM_CHAPTERS][2] = 
{
	{WORLDID_GARDEN,LEVELID_GARDEN1},	
	{WORLDID_GARDEN,LEVELID_GARDEN1},
	{WORLDID_GARDEN,LEVELID_GARDEN1},
	{WORLDID_GARDEN,LEVELID_GARDEN1},
	{WORLDID_GARDEN,LEVELID_GARDEN1},
	{WORLDID_GARDEN,LEVELID_GARDEN1},
	{WORLDID_GARDEN,LEVELID_GARDEN1},	
};

unsigned long currentChapter = 1;
unsigned long currentSubChapter = 1;
unsigned long chaptersOpen = 1;

unsigned long finishTime = 0;

short *screenInfo;


void LoadCurrentFMVImage(void)
{
	char filename[MAX_PATH];
	//if (screenInfo)
	//	gelfDefaultFree(screenInfo);

	sprintf(filename,"%stextures\\story\\SBscene%lu.%lu.bmp",baseDirectory,currentChapter,currentSubChapter);
	
	screenInfo = GetGelfBmpDataAsShortPtr(filename,NULL,NULL);
	finishTime = actFrameCount+60*IMAGETIME_SECONDS;	
}


long InitChapterFMV(void)
{
	screenInfo = NULL;
	currentSubChapter = 1;
	
	LoadCurrentFMVImage();
	return 1;
}



long ShowChapterFMV(void)
{
	if (actFrameCount>finishTime)
	{
		if (chapterNumImages[currentChapter-1]>currentSubChapter)
		{
			currentSubChapter++;
			LoadCurrentFMVImage();			
		}
		else
			return 0;
	}

	if (screenInfo)
		CopyShortDataToScreen(screenInfo);

	return 1;
}

long DoneChapterFMV(void)
{
	screenInfo = 0;
	finishTime = 0;

	player[0].worldNum = levelToPlay[currentChapter-1][0];
	player[0].levelNum = levelToPlay[currentChapter-1][1];			
	gameState.single = STORY_MODE;
					
	lastActFrameCount = actFrameCount;
	gameState.mode = LEVELCOMPLETE_MODE;
	GTInit( &modeTimer, 1 );
	showEndLevelScreen = 0;		

	return 1;
}

long CheckForFMVMode(void)
{
	switch (player[0].worldNum)
	{
		case WORLDID_GARDEN:
			currentChapter = 2;
			break;
		case WORLDID_ANCIENT:
			if (player[0].levelNum != LEVELID_ANCIENT3)
				return 0;
			currentChapter = 3;			
			break;
		case WORLDID_SPACE:
			if (player[0].levelNum != LEVELID_SPACE3)
				return 0;
			currentChapter = 4;			
			break;
		case WORLDID_CITY:
			if (player[0].levelNum != LEVELID_CITY3)
				return 0;
			currentChapter = 3;			
			break;
		case WORLDID_SUBTERRANEAN:	
	//		if ((player[0].levelNum != LEVELID_SUB2) && (player[0].levelNum != LEVELID_SUB4))
	//			return 0;
	//		currentChapter = 5;				
			break;
		case WORLDID_LABORATORY:
			if (player[0].levelNum != LEVELID_LABORATORY3)
				return 0;
			currentChapter = 4;						
			break;
		case WORLDID_HALLOWEEN:
			//bugger
			break;
	}

	lastActFrameCount = actFrameCount;

	FreeAllLists();
	gameState.mode = FMVPLAY_MODE;
	showEndLevelScreen = 0;		
	return 1;
}

void RunFMVMode(void)
{
	if (!screenInfo)
		InitChapterFMV();
	
	if (!ShowChapterFMV())
		DoneChapterFMV();
}	
