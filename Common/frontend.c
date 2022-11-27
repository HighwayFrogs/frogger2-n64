/*

	This file is part of Frogger2, (c) 1997 Interactive Studios Ltd.


	File		: frontend.c
	Programmer	: Jim & Dave
	Date		: 30/03/99

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>
#include "incs.h"


//------ [ GLOBALS ] ---------------------------------------------------------------------------//

int frameCount			= 0;
int	frameCount2			= 0;
int dispFrameCount		= 0;

char ActiveController	= 0;
extern unsigned long levelTime;

struct INTRO
{
	long timer;
	int stage;
	TEXTOVERLAY *text[4];
	GAMETILE *tiles;
};

struct INTRO* intro = NULL;

/*	--------------------------------------------------------------------------------
	Function		: GameLoop
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
void GameLoop(void)
{
	int i;

	switch (gameState.mode)
	{
	case INGAME_MODE:
	case FRONTEND_MODE:
		if(frameCount == 15)
			StartDrawing("gameloop");

#ifdef PC_VERSION
		UseAAMode = 2;
		UseZMode = 1;
#endif

		if (player[0].worldNum == WORLDID_FRONTEND)
			RunFrontendGameLoop();
		else
			RunGameLoop();

		frameCount++;
		break;

	case LEVELCOMPLETE_MODE:
		RunLevelComplete( );
		frameCount++;
		break;

	case WORLDCOMPLETE_MODE:
		RunWorldComplete( );
		frameCount++;
		break;

	case GAMECOMPLETE_MODE:
		RunGameComplete( );
		frameCount++;
		break;

	case GAMEOVER_MODE:
		RunGameOver( );
		frameCount++;
		break;
		
	case INTRO_MODE:
		RunGameIntro( );
		//RunGameLoop();
		frameCount++;
		break;

	case CAMEO_MODE:
	case PAUSE_MODE:
		if(frameCount == 15)
			StartDrawing("gameloop");

		RunPauseMenu();
		frameCount++;
		break;

	case FMVPLAY_MODE:
		if(frameCount == 15)
			StartDrawing("gameloop");

		RunFMVMode();
		frameCount++;
		break;
	
	}

	i = NUM_FROGS;
	while(i--)
		if(player[i].inputPause > 0)
			player[i].inputPause = 0;
}



/*	--------------------------------------------------------------------------------
	Function		: CreateOverlaysFromLogo
	Purpose			: Creates a series of sprite overlays from a LOGO, at screen resolution
	Parameters		: LOGO*, left, top
	Returns			: 
	Info			: 
*/
void CreateOverlaysFromLogo(const LOGO* logo, int x, int y)
{
	int i, j;
	char texture[32];
	unsigned short *tex, t;

	tex = (short*)logo->tiles;

	for (i = 0; i < logo->height; i++)
		for (j = 0; j < logo->width; j++)
		{
			if (t = *(tex++))
			{
				sprintf(texture, "%s%02d.bmp", logo->texname, t);
				CreateAndAddSpriteOverlay(x + j*32, y + i*32, texture, 32, 32, 255, 0);
			}
		}
}


/*	--------------------------------------------------------------------------------
	Function		: RunLevelComplete
	Purpose			: What happens at the end of a level
	Parameters		: 
	Returns			: 
	Info			: 
*/
void RunLevelComplete( )
{
	/*
	if( showEndLevelScreen )
	{
		RunLevelCompleteSequence();
	}
	*/
	GTUpdate( &modeTimer, -1 );
	if(!modeTimer.time)
	{
		DoHiscores( );

		// Only go to next level if in normal level progression.
		if( showEndLevelScreen )
		{
			short wld = player[0].worldNum, lvl = player[0].levelNum;

			if ((levelTime/60) < worldVisualData[wld].levelVisualData[lvl].parTime)
				worldVisualData[wld].levelVisualData[lvl].parTime = (levelTime/60);

			player[0].worldNum = worldVisualData[wld].levelVisualData[lvl].nextWorldID;
			player[0].levelNum = worldVisualData[wld].levelVisualData[lvl].nextLevelID;
			
#ifndef PC_VERSION
			StoreSaveSlot(0, 0); // Write data for Player 0 into Slot 0
			SaveGame(); // Write save games into eeprom
#else
//				SaveGameData();
#endif
		}

		if (gameState.single == STORY_MODE)
			if (CheckForFMVMode())
				return;


		FreeAllLists();
		frameCount = 0;
		gameState.mode = INGAME_MODE;

		player[0].numSpawn	= 0;
//		spawnCounter = 0;

		worldVisualData[player[0].worldNum].levelVisualData[player[0].levelNum].levelOpen |= LEVEL_OPEN;
		InitLevel(player[0].worldNum,player[0].levelNum);

		showEndLevelScreen = 1; // Normal level progression is default
	}
}


void StartLevelComplete()
{
//	FreeAllGameLists();

	CreateAndAddTextOverlay(0, 120, "Level complete", YES, 255, currFont, 0, 0);
}

/*	--------------------------------------------------------------------------------
	Function		: RunWorldComplete
	Purpose			: What happens at the end of a world
	Parameters		: 
	Returns			: 
	Info			: 
*/
void RunWorldComplete( )
{

}


/*	--------------------------------------------------------------------------------
	Function		: RunGameComplete
	Purpose			: The final complete screen
	Parameters		: 
	Returns			: 
	Info			: 
*/
void RunGameComplete( )
{

}


/*	--------------------------------------------------------------------------------
	Function		: StartGameOver
	Purpose			: Die, froggy! Wuaahahahahahahaha
	Parameters		: 
	Returns			: 
*/
void StartGameOver()
{
	gameState.mode = GAMEOVER_MODE;
	GTInit( &modeTimer, 10 );

	FreeAllGameLists();

	CreateAndAddTextOverlay(0, 120, "Game over", YES, 255, currFont, 0, 0);
}

/*	--------------------------------------------------------------------------------
	Function		: RunGameOver
	Purpose			: Runs the game-over screen
	Parameters		: 
	Returns			: 
*/
void RunGameOver( )
{
/*	if (NUM_FROGS == 1)
	{
		DisableTextOverlay(livesTextOver);
		DisableTextOverlay(scoreTextOver);
	}

	DisableTextOverlay(timeTextOver);
*/

	GTUpdate( &modeTimer, -1 );
	if(!modeTimer.time)
	{
		StopDrawing("game over");
		FreeAllLists();
		gameState.mode = INGAME_MODE;

		player[0].levelNum = LEVELID_FRONTEND4;
		player[0].worldNum = WORLDID_FRONTEND;
		player[0].frogState &= ~FROGSTATUS_ISDEAD;
		InitLevel(player[0].worldNum,player[0].levelNum);

		frameCount = 0;
		StartDrawing("game over");
	}
}

/*	--------------------------------------------------------------------------------
	Function		: StartGameIntro
	Purpose			: Runs the intro sequence, ISL logos and so on
	Parameters		: 
	Returns			: 

	NOTE! This is a very crude placeholder intro sequence. Replace it!
*/
void StartGameIntro()
{
	int i, j;
	GAMETILE *tile, *ptile = NULL;

	const static GAMETILE ex_tile = {
		{ NULL, NULL, NULL, NULL }, NULL, { NULL },
		TILESTATE_NORMAL,
		{ 0, 0, 0 },
		{ 0, 1, 0 },
		{ { 1, 0, 0 }, { 0, 0, -1 }, { -1, 0, 0 }, { 0, 0, 1 } }
	};		

	gameState.mode = INTRO_MODE;

	// load the generic banks
	LoadTextureBank(SYSTEM_TEX_BANK);
//	LoadTextureBank(INGAMEGENERIC_TEX_BANK);
	LoadObjectBank(INGAMEGENERIC_OBJ_BANK);

	InitGameLists();
	
	// Create intro stuff
	
	intro = (struct INTRO*)JallocAlloc(sizeof(struct INTRO), YES, "intro");

	// Generate a crap collision mesh

	firstTile = intro->tiles = (GAMETILE*)JallocAlloc(sizeof(GAMETILE) * 11, YES, "introtiles");

	for (i=0, tile = intro->tiles; i<=10; i++, tile++)
	{
		//memcpy(tile, &ex_tile, sizeof(GAMETILE));

		*tile = ex_tile;

		tile->centre.v[0] = ((i-5)*50.0f);
		
		if (ptile)
		{
			tile->tilePtrs[0] = ptile;
			ptile->tilePtrs[2] = ptile->next = tile;
		}

		ptile = tile;
	}

	controlCamera = 1;

	// set up player

	destTile[0] = currTile[0] = &intro->tiles[5];
	player[0].jumpTime = 1;

	// Create overlays
	intro->text[0] = CreateAndAddTextOverlay(0, 120, "Atari presents", YES, 255, currFont, 0, 0);

	intro->text[1] = CreateAndAddTextOverlay(0, 100, "An", YES, 255, currFont, 0, 0);
	intro->text[2] = CreateAndAddTextOverlay(0, 120, "Interactive Studios", YES, 255, currFont, 0, 0);
	intro->text[3] = CreateAndAddTextOverlay(0, 140, "Game", YES, 255, currFont, 0, 0);
	
	for (i=0;i<=3;i++)
		intro->text[i]->draw = 0;
	
	fog.r = fog.g = fog.b = 0;
	fog.mode = FOG_OFF;

	intro->timer = 0;
	intro->stage = 0;

	lastActFrameCount = 0;
}


/*	--------------------------------------------------------------------------------
	Function		: RunGameIntro
	Purpose			: Title screens and what have you
	Parameters		: 
	Returns			: 

	NOTE: This is a placeholder! DO NOT MODIFY THIS CODE! Delete it and start again.
*/
void RunGameIntro( )
{
	long t = actFrameCount - intro->timer;
	int alpha, i;

	switch (intro->stage)
	{
	case 0:
		intro->timer = actFrameCount - 200;
		intro->stage = 2;
		break;

	case 1:
		{
			if (t < 120) break;
			t -= 120;

			alpha = 255 - (t * 255)/120;
			
			if (alpha > 0)
				intro->text[0]->a = alpha;
			else
			{
				intro->stage = 2;
				intro->timer = actFrameCount;
				
				intro->text[0]->draw = 0;

				for (i=1;i<=3;i++)
				{
					intro->text[i]->draw = 1;
					intro->text[i]->a = 255;
				}
			}

			break;
		}

	case 2:
		{
			t -= 120;
			if (t < 0) break;

			alpha = 255 - (t * 255)/120;

			if (alpha > 0)
			{
				for (i=1;i<=3;i++)
					intro->text[i]->a = alpha;
			}
			else
			{
				JallocFree((UBYTE**)&intro);
				controlCamera = 0;

				player[0].worldNum = WORLDID_FRONTEND;
				player[0].levelNum = LEVELID_FRONTEND4;
				
				gameState.mode = LEVELCOMPLETE_MODE;
				gameState.multi = SINGLEPLAYER;
				GTInit( &modeTimer, 1 );
				showEndLevelScreen = 0;
			}
			break;
		}
	}
}

