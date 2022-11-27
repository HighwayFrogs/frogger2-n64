
/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: layoutvisual.c
	Programmer	: Andrew Eder
	Date		: 01/03/99

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>

#include "incs.h"

int audioEnabled = 1;

unsigned long worldNum;
unsigned long levelNum;

TEXTOVERLAY *demoText = NULL;
const char *demoStr = "Demo Mode";
extern long rPlaying;


// world visual data - for texture and object banks
WORLD_VISUAL_DATA worldVisualData[MAX_WORLDS];

UBYTE darkenedLevel		= 0;
short lightIntensity	= 255;
extern unsigned long runAttractMode;
unsigned long attractTime = 60 * 10;

WORLD_VISUAL_DATA worldVisualData[MAX_WORLDS] = 
{
//------------------------------------------------ GARDEN
	{
	"garden",				// description
	WORLDID_GARDEN,			// world ID
	GARDENMASTER_OBJ_BANK,		// master object bank
	GARDEN_TEX_BANK,		// master texture bank
	WORLD_OPEN,
	2,						// number of levels
		{ 
		"froglet garden","garden1.bmp",90,		LEVELID_GARDEN1,LEVELID_ANCIENT1,WORLDID_ANCIENT,GARDENLEV1_OBJ_BANK,0,GARDENLEV1_COL,LEVEL_OPEN,WORLDID_ANCIENT,0,  0,0,0,1100,1100,800,60,
		"garden multiplayer","garden1.bmp",0,	LEVELID_GARDEN6,LEVELID_ANCIENT1,WORLDID_ANCIENT,GARDENMULTI_OBJ_BANK,0,GARDENMULTI_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,60,
		},
	},

//------------------------------------------------ ANCIENT

	{
	"ancient",			// description
	WORLDID_ANCIENT,		// world ID
	ANCIENTMASTER_OBJ_BANK,	// master object bank
	ANCIENT_TEX_BANK,	// master texture bank
	WORLD_CLOSED,
	4,						// number of levels
		{ 
		"aztec canyon","ancient1.bmp",90,		LEVELID_ANCIENT1,LEVELID_ANCIENT2,WORLDID_ANCIENT,ANCIENTLEV1_OBJ_BANK,0,ANCIENTLEV1_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,80,
		"pyramid climb","ancient2.bmp",60,		LEVELID_ANCIENT3,LEVELID_ANCIENT3,WORLDID_ANCIENT,ANCIENTLEV3_OBJ_BANK,0,ANCIENTLEV3_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,120,
		"croc crossing","ancient3.bmp",60,		LEVELID_ANCIENT4,LEVELID_CITY1,WORLDID_CITY,ANCIENTBOSS_OBJ_BANK,0,ANCIENTMASTER_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,120,
		"ancient multiplayer","garden1.bmp",0,	LEVELID_ANCIENT6,LEVELID_ANCIENT1,WORLDID_ANCIENT,ANCIENTMULTI_OBJ_BANK,0,ANCIENTMULTI_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,60,
		},
	},


//------------------------------------------------ SPACE

	{
	"space",			// description
	WORLDID_SPACE,		// world ID
	SPACEMASTER_OBJ_BANK,		// master object bank
	SPACE_TEX_BANK,		// master texture bank
	WORLD_CLOSED,
	4,						// number of levels
		{ 
		"space station","space1.bmp",80,		LEVELID_SPACE1,LEVELID_SPACE2,WORLDID_SPACE,SPACELEV1_OBJ_BANK,0,SPACELEV1_COL,LEVEL_CLOSED,-1,0, 0,0, 0,500,6500,1100,120,
		"asteroid ride","space2.bmp",80,		LEVELID_SPACE3,LEVELID_SPACE3,WORLDID_SPACE,SPACELEV3_OBJ_BANK,0,SPACELEV3_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,130,
		"spacesuckers","space3.bmp",65,			LEVELID_SPACE4,LEVELID_HALLOWEEN1,WORLDID_HALLOWEEN,SPACEBOSS_OBJ_BANK,0,SPACEMASTER_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,180,
		"space multiplayer","space1.bmp",0,		LEVELID_SPACE6,LEVELID_LABORATORY1,WORLDID_LABORATORY,SPACEMULTI_OBJ_BANK,0,SPACEMULTI_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,60,
		},
	},

//------------------------------------------------ CITY

	{
	"city",				// description
	WORLDID_CITY,		// world ID
	CITYMASTER_OBJ_BANK,		// master object bank
	CITY_TEX_BANK,		// master texture bank
	WORLD_OPEN,
	4,						// number of levels
		{ 
		"docks","city1.bmp",80,					LEVELID_CITY1,LEVELID_CITY2,WORLDID_CITY,CITYLEV1_OBJ_BANK,0,CITYLEV1_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,150,
		"warehouse","city2.bmp",80,				LEVELID_CITY2,LEVELID_CITY3,WORLDID_CITY,CITYLEV2_OBJ_BANK,0,CITYLEV2_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,160,
		"city streets","city3.bmp",90,			LEVELID_CITY3,LEVELID_SUBTERRANEAN1,WORLDID_SUBTERRANEAN,CITYLEV3_OBJ_BANK,0,CITYLEV3_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,150,
		"city multiplayer","garden1.bmp",0,		LEVELID_CITY6,LEVELID_CITY6,WORLDID_CITY,CITYMULTI_OBJ_BANK,0,CITYMULTI_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,60,
		},
	},

//------------------------------------------------ SUBTERRANEAN

	{
	"subterranean",				// description
	WORLDID_SUBTERRANEAN,		// world ID
	SUBTERRANEANMASTER_OBJ_BANK,		// master object bank
	SUBTERRANEAN_TEX_BANK,		// master texture bank
	WORLD_CLOSED,
	4,						// number of levels
		{ 
		"gold mine","sub1.bmp",80,				LEVELID_SUBTERRANEAN1,LEVELID_SUBTERRANEAN2,WORLDID_SUBTERRANEAN,SUBTERRANEANLEV1_OBJ_BANK,0,SUBTERRANEANLEV1_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,120,
		"ants nest","sub2.bmp",80,				LEVELID_SUBTERRANEAN2,LEVELID_SUBTERRANEAN3,WORLDID_SUBTERRANEAN,SUBTERRANEANLEV2_OBJ_BANK,0,SUBTERRANEANLEV2_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,130,
		"lava hell","sub3.bmp",90,				LEVELID_SUBTERRANEAN4,LEVELID_LABORATORY1,WORLDID_LABORATORY,SUBTERRANEANBOSS_OBJ_BANK,0,SUBTERRANEANMASTER_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,180,
		"sub multiplayer","garden1.bmp",		LEVELID_SUBTERRANEAN6,LEVELID_SUBTERRANEAN2,WORLDID_SUBTERRANEAN,SUBTERRANEANMULTI_OBJ_BANK,0,SUBTERRANEANMULTI_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,60,
		},
	},

//------------------------------------------------ LABORATORY

	{
	"laboratory",				// description
	WORLDID_LABORATORY,		// world ID
	LABORATORYMASTER_OBJ_BANK,		// master object bank
	LABORATORY_TEX_BANK,		// master texture bank
	WORLD_CLOSED,
	4,						// number of levels
		{ 
		"the steam room","lab1.bmp",80,			LEVELID_LABORATORY1,LEVELID_LABORATORY2,WORLDID_LABORATORY,LABORATORYLEV1_OBJ_BANK,0,LABORATORYLEV1_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,120,
		"research facility","lab2.bmp",80,		LEVELID_LABORATORY3,LEVELID_LABORATORY3,WORLDID_LABORATORY,LABORATORYLEV3_OBJ_BANK,0,LABORATORYLEV3_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,300,
		"infernal machine","lab3.bmp",65,		LEVELID_LABORATORY4,LEVELID_SPACE1,WORLDID_SPACE,LABORATORYBOSSA_OBJ_BANK,0,LABORATORYMASTERA_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,210,
		"lab multiplayer","garden1.bmp",0,		LEVELID_LABORATORY8,LEVELID_LABORATORY2,WORLDID_LABORATORY,LABORATORYMULTI_OBJ_BANK,0,LABORATORYMULTI_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,60,
		},
	},

//------------------------------------------------ HALLOWEEN

	{
	"halloween",					// description
	WORLDID_HALLOWEEN,			// world ID
	HALLOWEENMASTER_OBJ_BANK,			// master object bank
	HALLOWEEN_TEX_BANK,			// master texture bank
	WORLD_CLOSED,
	6,						// number of levels 
		{ 
		"graveyard garden","hall1.bmp",90,		LEVELID_HALLOWEEN1,LEVELID_HALLOWEEN2,WORLDID_HALLOWEEN,HALLOWEENLEV1_OBJ_BANK,0,HALLOWEENLEV1_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,150,
		"haunted house","hall2.bmp",90,			LEVELID_HALLOWEEN2,LEVELID_HALLOWEEN3,WORLDID_HALLOWEEN,HALLOWEENLEV2_OBJ_BANK,0,HALLOWEENLEV2_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,190,
		"bridge dash","",90,					LEVELID_HALLOWEEN3,LEVELID_HALLOWEEN4,WORLDID_HALLOWEEN,HALLOWEENLEV3_OBJ_BANK,0,HALLOWEENLEV3_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,190,
		"tower o traps","hall3.bmp",0,			LEVELID_HALLOWEEN4,LEVELID_HALLOWEEN5,WORLDID_HALLOWEEN,HALLOWEENBOSS_OBJ_BANK,0,HALLOWEENMASTER_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,120,
		"download","",0,						LEVELID_HALLOWEEN5,LEVELID_HALLOWEEN6,WORLDID_HALLOWEEN,HALLOWEENBOSSB_OBJ_BANK,0,HALLOWEENMASTERB_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,120,
		"swampys lair","garden1.bmp",0,			LEVELID_HALLOWEEN6,LEVELID_SUPERRETRO1,WORLDID_SUPERRETRO,HALLOWEENMULTI_OBJ_BANK,0,HALLOWEENMULTI_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,60,
		},
	},

	

//------------------------------------------------ SUPERRETRO

	{
	"super retro",			// description
	WORLDID_SUPERRETRO,		// world ID
	RETROMASTER_OBJ_BANK,	// master object bank
	SUPERRETRO_TEX_BANK,	// master texture bank
	WORLD_CLOSED,
	10,						// number of levels
		{ 
		"super 1 50","super1.bmp",50,		LEVELID_SUPERRETRO1,LEVELID_SUPERRETRO2,WORLDID_SUPERRETRO,RETROLEV1_OBJ_BANK,0,RETROLEV1_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,60,
		"super 2 50","super1.bmp",50,		LEVELID_SUPERRETRO2,LEVELID_SUPERRETRO3,WORLDID_SUPERRETRO,RETROLEV2_OBJ_BANK,0,RETROLEV2_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,60,
		"super 3 50","super1.bmp",50,		LEVELID_SUPERRETRO3,LEVELID_SUPERRETRO4,WORLDID_SUPERRETRO,RETROLEV3_OBJ_BANK,0,RETROLEV3_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,60,
		"super 4 50","super1.bmp",50,		LEVELID_SUPERRETRO4,LEVELID_SUPERRETRO5,WORLDID_SUPERRETRO,RETROLEV4_OBJ_BANK,0,RETROLEV4_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,60,
		"super 5 50","super1.bmp",50,		LEVELID_SUPERRETRO5,LEVELID_SUPERRETRO6,WORLDID_SUPERRETRO,RETROLEV5_OBJ_BANK,0,RETROLEV5_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,60,
		"super 6 50","super1.bmp",50,		LEVELID_SUPERRETRO6,LEVELID_SUPERRETRO7,WORLDID_SUPERRETRO,RETROLEV6_OBJ_BANK,0,RETROLEV6_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,60,
		"super 7 50","super1.bmp",50,		LEVELID_SUPERRETRO7,LEVELID_SUPERRETRO8,WORLDID_SUPERRETRO,RETROLEV7_OBJ_BANK,0,RETROLEV7_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,60,
		"super 8 50","super1.bmp",50,		LEVELID_SUPERRETRO8,LEVELID_SUPERRETRO9,WORLDID_SUPERRETRO,RETROLEV8_OBJ_BANK,0,RETROLEV8_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,60,
		"super 9 50","super1.bmp",50,		LEVELID_SUPERRETRO9,LEVELID_SUPERRETRO10,WORLDID_SUPERRETRO,RETROLEV9_OBJ_BANK,0,RETROLEV9_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,60,
		"super 10 50","super1.bmp",50,		LEVELID_SUPERRETRO10,LEVELID_FRONTEND1,WORLDID_FRONTEND,RETROLEV10_OBJ_BANK,0,RETROLEV10_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,60,
		},
	},

//------------------------------------------------

	{
	"frontend",			// description
	WORLDID_FRONTEND,		// world ID
	FRONTEND_OBJ_BANK,	// master object bank
	FRONTEND_TEX_BANK,	// master texture bank
	WORLD_OPEN,
	5,						// number of levels
		{ 
		"start","garden1.bmp",8,		LEVELID_FRONTEND1,LEVELID_FRONTEND1,WORLDID_FRONTEND,FRONTEND1_OBJ_BANK,0,FRONTEND1_COL,LEVEL_OPEN,-1,0, 120,120,160,500,6500,1100,60,
		"levela","garden1.bmp",16,		LEVELID_FRONTEND2,LEVELID_FRONTEND1,WORLDID_FRONTEND,FRONTEND2_OBJ_BANK,0,FRONTEND2_COL,LEVEL_OPEN,-1,0, 0,0, 0,1100,1100,800,60,
		"levelb","garden1.bmp",32,		LEVELID_FRONTEND3,LEVELID_FRONTEND1,WORLDID_FRONTEND,FRONTEND3_OBJ_BANK,0,FRONTEND3_COL,LEVEL_OPEN,-1,0, 0,0, 0,1100,1100,800,60,
		"multisel","garden1.bmp",64,		LEVELID_FRONTEND4,LEVELID_FRONTEND1,WORLDID_FRONTEND,FRONTEND4_OBJ_BANK,0,FRONTEND4_COL,LEVEL_OPEN,-1,0, 0,0, 0,1100,1100,800,60,
		"language","garden1.bmp",128,		LEVELID_FRONTEND5,LEVELID_FRONTEND1,WORLDID_FRONTEND,FRONTEND5_OBJ_BANK,0,FRONTEND5_COL,LEVEL_OPEN,-1,0, 0,0, 0,1100,1100,800,60,
		},
	}
};


/*	--------------------------------------------------------------------------------
	Function		: LoadVisualBanksForWorld
	Purpose			: loads the relevant object and texture banks for a world
	Parameters		: unsigned long,unsigned long
	Returns			: void
	Info			: 
*/
void LoadVisualBanksForWorld(unsigned long worldID,unsigned long levelID)
{
	unsigned long i;
	long objBankToUse;
	long texBankToUse;

	objBankToUse = worldVisualData[worldID].masterObjectBank;
	texBankToUse = worldVisualData[worldID].masterTextureBank;

	// load the relevant master object / texture banks
	dprintf"LoadVisualBanksForWorld(%d,%d)...\n",worldID,levelID));

	// Contains frogger and froglet - probably quite important
	LoadObjectBank(INGAMEGENERIC_OBJ_BANK);
	// Multiplayer frogs
//	if( gameState.multi != SINGLEPLAYER )
//		LoadObjectBank(MULTIPLAYER_OBJ_BANK);

	LoadTextureBank(texBankToUse);
	LoadObjectBank(objBankToUse);

	// load the relevant child object bank(s) if required
	if(levelID == LOAD_ALL_LEVELBANKS)
	{
		for(i=0; i< MAX_LEVELS; i++)
		{
			if(worldVisualData[worldID].levelVisualData[i].collBank > -1)
			{
//				if(worldVisualData[worldID].levelVisualData[i].textureBankToUse > 0)
//					LoadTextureBank(worldVisualData[worldID].levelVisualData[i].textureBankToUse);
				if(worldVisualData[worldID].levelVisualData[i].objectBankToUse > 0)
					LoadObjectBank(worldVisualData[worldID].levelVisualData[i].objectBankToUse);
			}
		}
	}
	else
	{
		if(worldVisualData[worldID].levelVisualData[levelID].collBank > -1)
		{
//			if(worldVisualData[worldID].levelVisualData[levelID].textureBankToUse > 0)
//				LoadTextureBank(worldVisualData[worldID].levelVisualData[levelID].textureBankToUse);
			if(worldVisualData[worldID].levelVisualData[levelID].objectBankToUse > 0)
				LoadObjectBank(worldVisualData[worldID].levelVisualData[levelID].objectBankToUse);
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function		: FreeAllObjectBanks
	Purpose			: frees up ALL object banks
	Parameters		: 
	Returns			: void
	Info			: 
*/

#ifdef PC_VERSION
	void FreeObjectBank(long i);
#endif

void FreeAllObjectBanks()
{
	unsigned long i;

#ifdef PC_VERSION
	
	for(i=0; i<MAX_OBJECT_BANKS; i++)
		FreeObjectBank(i);
#else

	for(i=0; i<MAX_OBJECT_BANKS; i++)
	{
		if(objectBanks[i].freePtr)
			JallocFree((UBYTE**)&objectBanks[i].freePtr);
	}

#endif
}

void UpdateCompletedLevel(unsigned long worldID,unsigned long levelID)
{
	if (levelID<3)
		worldVisualData[worldID].levelVisualData[levelID+1].levelOpen = LEVEL_OPEN;
}

/*	--------------------------------------------------------------------------------
	Function		: InitLevelLists
	Purpose			: Initialises in-game lists
	Parameters		: 
	Returns			: void
*/
void InitGameLists()
{
	// initialise the various lists
	InitParticleList( );
	InitSpriteList( );
	InitSpriteSortArray(MAX_ARRAY_SPRITES);
	InitSpriteOverlayLinkedList();
	InitTextOverlayLinkedList();
	InitSpecFXList();
	InitBabyList(YES); // MUST BE DONE BEFORE ENEMY INITS!
	InitEnemyLinkedList();
	InitPlatformLinkedList();
	InitGaribList();
	InitTriggerList();
	Init3DTextList( );
	InitTongues( );

#ifdef PC_VERSION
	InitSampleList();
#endif
	InitAmbientSoundList();
}

/*	--------------------------------------------------------------------------------
	Function		: FreeAllGameLists
	Purpose			: Frees all in-game lists but NOT textures, objects etc.
	Parameters		: 
	Returns			: 
*/
void FreeAllGameLists()
{
	dprintf"-- freeing in-game lists <--\n"));

#ifdef N64_VERSION
	MusHandleStop(audioCtrl.musicHandle[0],0);
	audioCtrl.currentTrack[0] = 0;
#endif

#ifdef PC_VERSION
	StopSong( );
	FreeSampleList();
#endif

	KillAllTriggers();
	FreeAmbientSoundList();
	ResetBabies( );
	FreeTransCameraList();
	FreeTongues();

	// Entities and collision
	FreeGaribList();
	FreeEnemyLinkedList();
	FreePlatformLinkedList();
	FreePathList();
	FreeLevelScript();
	FreeCharacterList();

	// Graphics-related
	FreeSpecFXList( );
	Free3DTextList();
	FreeTextOverlayLinkedList();
	FreeSpriteOverlayLinkedList();

	InitTextOverlayLinkedList();
	InitSpriteOverlayLinkedList();

	dprintf"-->\n"));
}

/* --------------------------------------------------------------------------------
	Function	: InitLevel
	Purpose		:
	Parameters	: unsigned long , unsigned long
	Returns		: void 
*/
void ShowLoadScreen(void);

void InitLevel(unsigned long worldID,unsigned long levelID)
{
	int i;

#ifdef PC_VERSION
	actFrameCount = 0;
	gameSpeed = 1;
#endif 

	// Print the "loading..." screen
	ShowLoadScreen();

	// load the system and in-game generic texture banks
	LoadTextureBank(SYSTEM_TEX_BANK);

	// load relevant collison, texture and object banks
	LoadMapBank(worldVisualData[worldID].levelVisualData[levelID].collBank);
	LoadVisualBanksForWorld(worldID,levelID);

	InitGameLists();

#ifdef PC_VERSION
	if (audioEnabled)
	{
		LoadSfx(worldID);
		LoadSfxMapping(worldID,levelID);
	}
#endif

	player[0].worldNum = worldID;
	player[0].levelNum = levelID;

	pOIDistance = 50000.0;
	pointOfInterest = NULL;

	// create objects for the level
	CreateLevelObjects(worldID,levelID);
	CreateFrogger(1,1);
	
	LoadLevelEntities(
		worldVisualData[worldID].worldID,
		worldVisualData[worldID].levelVisualData[levelID].levelID);

	ResetCamera( );
	InitCamera();
	
	// prepare the text overlays for the current level
	InitInGameTextOverlays(worldID,levelID);

	if( gameState.multi != SINGLEPLAYER )
		ReinitialiseMultiplayer( );

#ifndef PC_VERSION
	MusSetMasterVolume(MUSFLAG_SONGS,32000);
#endif

	if (audioEnabled)
		PrepareSongForLevel((short)worldID,(short)levelID);

#ifdef PC_VERSION // TEMPORARY
	fog.mode = 1;
	fog.r = worldVisualData[worldID].levelVisualData[levelID].fogR;
	fog.g = worldVisualData[worldID].levelVisualData[levelID].fogG;
	fog.b = worldVisualData[worldID].levelVisualData[levelID].fogB;
	farClip = 	4000; //worldVisualData[worldID].levelVisualData[levelID].farDist;
	fStart = worldVisualData[worldID].levelVisualData[levelID].fogNearDist * 0.0005;
	fEnd = 	(worldVisualData[worldID].levelVisualData[levelID].fogFarDist * 0.0005);
#endif

	frameCount = 0;

	// script is loaded last when we know everything's set up
	LoadLevelScript(
		worldVisualData[worldID].worldID,
		worldVisualData[worldID].levelVisualData[levelID].levelID);

	// Stuff that used to be in RunGameLoop
	if ( worldVisualData [ player[0].worldNum ].levelVisualData [ player[0].levelNum ].multiPartLevel == NO_MULTI_LEV )
	{
		player[0].lives				= 10;
		player[0].score				= 0;
		player[0].spawnTimer		= 0;
		player[0].spawnScoreLevel	= 1;
		GTInit( &scoreTimer, 90 );
	}

	if (player[0].worldNum == WORLDID_FRONTEND)
	{
		if (player[0].levelNum == LEVELID_FRONTEND4)
		{
			runAttractMode = attractTime;
			NUM_FROGS = 1;
			gameState.multi = SINGLEPLAYER;

#ifdef PC_VERSION
			InitCredits();			
			creditsActive = 3;
			DeactivateCredits();
		
//			DisableTextOverlay(scoreTextOver);

			frogFacing[0] = 3;
			atari = CreateAndAddSpriteOverlay(270,195,"atari.bmp",32,32,255,0);
			konami = CreateAndAddSpriteOverlay(18,195,"konami.bmp",32,32,255,0);
			flogo[0] = CreateAndAddSpriteOverlay(98,136,"frogger2.bmp",128,128,255,0) ;
			
			islLogo[0] = CreateAndAddSpriteOverlay(250,10,"isl1.bmp",20,20,192,0 );
			islLogo[1] = CreateAndAddSpriteOverlay(270,10,"isl2.bmp",20,20,192,0 );
			islLogo[2] = CreateAndAddSpriteOverlay(290,10,"isl3.bmp",20,20,192,0 );
#endif
			fadingLogos = 0;
		}
	}

	if (player[0].worldNum==8 || gameState.multi != SINGLEPLAYER)
	{
	//	CreateOverlays();
//		DisableHUD();
//		livesTextOver->draw = 0;
		garibCount->draw = 0;
		creditCount->draw = 0;
		countdownTimer = 0;

		if( player[0].worldNum == 8 )
		{
//			timeTextOver->draw = 0;

//			i = 3;
//			while(i--)
//				sprHeart[i]->draw = 0;

//			DisableTextOverlay( scoreTextOver );
		}

		i = numBabies;
		while(i--)
			babyIcons[i]->draw = 0;
	}
	
	demoText = NULL;

	if (rPlaying)
	{
		demoText = CreateAndAddTextOverlay(160,5,demoStr,YES,128,currFont,0,0);
		DisableHUD();
//		DisableTextOverlay(livesTextOver);
//		DisableTextOverlay(timeTextOver);
		DisableTextOverlay(babySavedText);
		DisableTextOverlay(garibCount);
		DisableTextOverlay(creditCount);
//		DisableTextOverlay(scoreTextOver);
		
//		i = 3;
//		while(i--)
//			sprHeart[i]->draw = 0;			

		i = numBabies;
		while(i--)
			babyIcons[i]->draw = 0;
	}

	lastActFrameCount = 0;

	for (i = 3; i>=0; i--)
		controllerdata[i].button = controllerdata[i].lastbutton = 0;
}


/*	--------------------------------------------------------------------------------
	Function		: FreeAllLists
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
void FreeAllLists()
{
	dprintf"----- FREEING ALL LISTS -----\n"));

	StopDrawing("FREELIST");

//	ResetMultiplayer( );

	FreeAllGameLists();
	
	FreeMapBank();
	FreeProcTextures( );
#ifdef PC_VERSION
	FreeRandomPolyList( );
#endif
	FreeAllObjectBanks();
	FreeAllTextureBanks();
#ifndef PC_VERSION
	FreeN64WaterResources();
	FreeN64ModgyTexResources();
#endif

	FreeActorList();
	FreeSpriteSortArray();
	FreeSpriteList();
	FreeParticleList();

#ifndef PC_VERSION
	// global ptrs
	aMapBank = NULL;
	globalPtrEntityBank = NULL;
#endif

	pOIDistance = 50000.0;
	pointOfInterest = NULL;

	//fog.r = fog.g = fog.b = 0;
	fog.mode = 0;
	pauseMode		= 0;
	darkenedLevel	= 0;

	dprintf"-----------------------------\n"));
}
