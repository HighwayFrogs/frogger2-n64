/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: menus.c
	Programmer	: James Healey
	Date		: 27/05/99

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>
#include <limits.h>

#include "incs.h"

TEXTOVERLAY *worldNames	[ MAX_WORLDS ];
TEXTOVERLAY *levelN		[ MAX_LEVELS ];
TEXTOVERLAY *closed;

SPRITEOVERLAY *atari = NULL;
SPRITEOVERLAY *konami = NULL;
SPRITEOVERLAY *sprOver = NULL;

unsigned long USE_MENUS = 0;

/* LOGO information for Frogger 2 logo */

LOGO Frogger2Logo =
{
	"flogo", 4, 3,
	{
		1, 2, 3, 4,
		5, 6, 7, 8,
		0, 0, 9, 10
	}
};

/*	--------------------------------------------------------------------------------
	Function 	: RunTitleScreen
	Purpose 	: The first menu
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
char mpText[] = "Multiplayer 1";

void RunTitleScreen()
{
	static unsigned long currentSelection = 0;
	static u16 button;
	static u16 lastbutton;

	static TEXTOVERLAY *startText;
	static TEXTOVERLAY *selectText;
	static TEXTOVERLAY *multiText;
	static TEXTOVERLAY *devText;
	static long numPlayers = 1;

	int i = 0,xPos,j;

	if(frameCount == 1)
	{
		FreeAllLists();

		LoadTextureBank(SYSTEM_TEX_BANK);
#ifdef PC_VERSION
		LoadTextureBank(TITLES_TEX_BANK);
		currFont = bigFont;
#else
		LoadTextureBank(TITLESGENERIC_TEX_BANK);
		currFont = smallFont;
#endif

		startText = CreateAndAddTextOverlay(100,112,"start game",YES,255,currFont,TEXTOVERLAY_NORMAL,6);

		selectText = CreateAndAddTextOverlay(100,132,"level select",YES,255,currFont,TEXTOVERLAY_NORMAL,6);
		multiText = CreateAndAddTextOverlay(100,152,mpText,YES,255,currFont,TEXTOVERLAY_NORMAL,6);
		devText = CreateAndAddTextOverlay(100,172,"develop",YES,255,currFont,TEXTOVERLAY_NORMAL,6);

		konami = CreateAndAddSpriteOverlay(240,35,"konami.bmp",32,32,192,0 );
		atari = CreateAndAddSpriteOverlay(40,35,"atari.bmp",32,32,192,0 );

		CreateOverlaysFromLogo(&Frogger2Logo,98,20);

		ResetParameters();
	}

	startText->a = 100;
	selectText->a = 100;
	multiText->a = 100;
	devText->a = 100;
	sprintf(mpText,"Multiplayer %i",numPlayers);
	button = controllerdata [ ActiveController ].button;

	if((button & CONT_UP) && !(lastbutton & CONT_UP))
    {
		if( currentSelection > 0 )
		{
			currentSelection--;
//			PlaySampleNot3D(237,192,128,128);
		}
	}
	    
	if((button & CONT_DOWN) && !(lastbutton & CONT_DOWN))
    {
		if( currentSelection < 3 )
		{
			currentSelection++;
//			PlaySampleNot3D(237,192,128,128);
		}
	}
	
	if(frameCount > 15)
	{
		if(currentSelection == 2)
		{
			if((button & CONT_RIGHT) && !(lastbutton & CONT_RIGHT))
				if(numPlayers<MAX_FROGS)
					numPlayers++;

			if((button & CONT_LEFT) && !(lastbutton & CONT_LEFT))
				if(numPlayers>1)
					numPlayers--;
		}

		if(	(button & CONT_A) && !(lastbutton & CONT_A) ||
			(button & CONT_START) && !(lastbutton & CONT_START))
		{
			switch(currentSelection)
			{
				case 0:
					player[0].worldNum = WORLDID_FRONTEND;
					player[0].levelNum = LEVELID_FRONTEND1;

					FreeAllLists();
					InitLevel(player[0].worldNum,player[0].levelNum);

					gameState.mode = INGAME_MODE;
						
					if(gameState.multi != MULTIREMOTE)
						gameState.multi = SINGLEPLAYER;

					ACTOR_DRAWDISTANCEINNER = 150000;
					ACTOR_DRAWDISTANCEOUTER = 200000;

					NUM_FROGS = 1;
					frameCount = 0;
					lastbutton = 0;
					break;

				case 1:
					// level select menu
					FreeMenuItems();
					gameState.menuMode = LEVELSELECT_MODE;

					if(gameState.multi != MULTIREMOTE)
					{
						NUM_FROGS = 1;
						gameState.multi = SINGLEPLAYER;
					}

					ACTOR_DRAWDISTANCEINNER = 150000;
					ACTOR_DRAWDISTANCEOUTER = 200000;

					frameCount = 0;
					lastbutton = 0;
					break;
			
				case 2:
					// select multi player game
					FreeMenuItems();
					if(gameState.multi != MULTIREMOTE)
						gameState.multi = MULTILOCAL;
					gameState.menuMode = LEVELSELECT_MODE;
					ACTOR_DRAWDISTANCEINNER = ULONG_MAX-1;
					ACTOR_DRAWDISTANCEOUTER = ULONG_MAX;
					frameCount = 0;
					lastbutton = 0;
					NUM_FROGS = numPlayers;
					break;
			
				case 3:
					// develepoment mode
					FreeMenuItems();
					gameState.menuMode = DEVELOPMENT_MODE;
					frameCount = 0;
					lastbutton = 0;
					break;
			}

			return;		// DON'T try to do any menu stuff! Duh
		}			

		switch (currentSelection)
		{
			case 0: startText->a = 255; break;
			case 1: selectText->a = 255; break;
			case 2: multiText->a = 255; break;
			case 3: devText->a = 255; break;
		}
	}

	lastbutton = button;
}


/*	--------------------------------------------------------------------------------
	Function 	: RunLevelSelect
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void RunLevelSelect()
{
	static u16 button;
	static u16 lastbutton;

	static TEXTOVERLAY *worldSelText[MAX_WORLDS];
	static TEXTOVERLAY *levelSelText[MAX_LEVELS];

	static unsigned long lrSelect = 0;				// Select worlds or levels
	static unsigned long oldWorldSelect = 0;        // To check if selected world has changed
	static unsigned long currentWorldSelect = 0;	// Which world
	static unsigned long currentLevelSelect = 0;	// Which level

	static LNAMESTRUCT lNames[MAX_LEVELS];

	unsigned int i,j;

	if(frameCount == 1)
	{
		FreeMenuItems();

		//currFont = smallFont;

		// draw list of worlds on left
		for( i=0; i < MAX_WORLDS; i++ )
			worldSelText[i] = CreateAndAddTextOverlay( 5, (i*20)+30, worldVisualData[i].description,
														NO,100, currFont, TEXTOVERLAY_NORMAL,0 );

		// draw list of levels in current selected world on right
		for( i=0; i < MAX_LEVELS; i++ )
		{
			sprintf( lNames[i].name, worldVisualData[currentWorldSelect].levelVisualData[i].description );
			levelSelText[i] = CreateAndAddTextOverlay( 150, (i*20)+30, lNames[i].name,
														NO,100,	currFont,TEXTOVERLAY_NORMAL,0);
		}

		ResetParameters();
	}


	// Reset alpha values
	worldSelText[currentWorldSelect]->a = 100;
	if(currentLevelSelect != 255)
		levelSelText[currentLevelSelect]->a = 100;

	button = controllerdata [ ActiveController ].button;

	// move between worlds or levels
	if((button & CONT_UP) && !(lastbutton & CONT_UP))
    {
		if( lrSelect == 0 && currentWorldSelect > 0 ) // World select
		{
			currentWorldSelect--;
			for( i=0; i < MAX_LEVELS; i++ )
			{
				if( worldVisualData[currentWorldSelect].levelVisualData[i].collBank != -1 )
				{
					currentLevelSelect = i;
					break;
				}
				else currentLevelSelect = 255;
			}
		}
		else if( lrSelect == 1 && currentLevelSelect > 0 ) // Level select
		{
			for( i=currentLevelSelect-1; i >= 0; i-- )
			{
				if( worldVisualData[currentWorldSelect].levelVisualData[i].collBank != -1 )
				{
					currentLevelSelect = i;
					break;
				}
			}
		}
	}
	    
	if((button & CONT_DOWN) && !(lastbutton & CONT_DOWN))
    {
		if( lrSelect == 0 && currentWorldSelect < MAX_WORLDS-1 ) // World select
		{
			currentWorldSelect++;
			for( i=0; i < MAX_LEVELS; i++ )
			{
				if( worldVisualData[currentWorldSelect].levelVisualData[i].collBank != -1 )
				{
					currentLevelSelect = i;
					break;
				}
				else currentLevelSelect = 255;
			}
		}
		else if( lrSelect == 1 && currentLevelSelect < MAX_LEVELS )
		{
			for( i=currentLevelSelect+1; i < MAX_LEVELS; i++ )
			{
				if( worldVisualData[currentWorldSelect].levelVisualData[i].collBank != -1 )
				{
					currentLevelSelect = i;
					break;
				}
			}
		}
	}

	// move between world and level select
	if((button & CONT_LEFT) && !(lastbutton & CONT_LEFT))
		if(lrSelect == 1)
			lrSelect = 0;
	    
	if((button & CONT_RIGHT) && !(lastbutton & CONT_RIGHT))
		if(lrSelect == 0 && currentLevelSelect != 255)
			lrSelect = 1;

	//	check if world selection has changed. If so, update level list.

	if(currentWorldSelect != oldWorldSelect)
	{
		// change old level textovers
		for(i=0; i < MAX_LEVELS; i++)
			sprintf(lNames[i].name,worldVisualData[currentWorldSelect].levelVisualData[i].description);
	}


	//	don't allow menu changing for the first fifteen frames

	if(frameCount > 15)
	{
		// move back in menus
		if((button & CONT_B) && !(lastbutton & CONT_B))
		{
			FreeMenuItems();
			gameState.menuMode = TITLE_MODE;
			frameCount = 0;
			lastbutton = 0;
			return;
		}

		// run selected world and level
		if( (((button & CONT_A) && !(lastbutton & CONT_A)) ||
			((button & CONT_START) && !(lastbutton & CONT_START)))
			&& currentLevelSelect != 255)
		{
			player[0].worldNum = currentWorldSelect;
			player[0].levelNum = currentLevelSelect;

			FreeAllLists();
			InitLevel(currentWorldSelect,currentLevelSelect);

			gameState.mode = INGAME_MODE;
				
			frameCount = 0;
			lastbutton = 0;
			return;
		}			
	}

	// set selected text solid
	if(lrSelect == 0)
	{
		worldSelText[currentWorldSelect]->a = 255;
		if( currentLevelSelect != 255 ) levelSelText[currentLevelSelect]->a = 175;
	}
	else
	{
		worldSelText[currentWorldSelect]->a = 175;
		if( currentLevelSelect != 255 ) levelSelText[currentLevelSelect]->a = 255;
	}

	lastbutton = button;
	oldWorldSelect = currentWorldSelect;
}


/*	--------------------------------------------------------------------------------
	Function 	: RunPauseMenu
	Purpose 	: In game screen that appears when start is pressed.
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void RunPauseMenu()
{
	static unsigned long currentSelection = 0;
	static u16 button,lastbutton = 0;
	
	button = controllerdata[ActiveController].button;

//	pauseTitle->a -= 16;

	if((button & CONT_UP) && !(lastbutton & CONT_UP))
		if(currentSelection > 0)
			currentSelection--;

	if((button & CONT_DOWN) && !(lastbutton & CONT_DOWN))
		if(currentSelection < 1)
			currentSelection++;

	if((button & CONT_A) && !(lastbutton & CONT_A))
	{
		lastbutton = button;

//		DisableTextOverlay ( pauseTitle );
		DisableTextOverlay ( continueText );
		DisableTextOverlay ( quitText );
		pauseMode = 0;

		switch(currentSelection)
		{
			case 0:   // continue game
			{
				long i;
				gameState.mode	= INGAME_MODE;

				if( NUM_FROGS == 1 )
				{
					livesTextOver->a = livesTextOver->oa;
					scoreTextOver->a = scoreTextOver->oa;
				}
				timeTextOver->a = timeTextOver->oa;

				for ( i = 0; i < 3; i++ )
					sprHeart[i]->draw = 1;

				for(i=0; i<numBabies; i++)
					babyIcons[i]->draw = 1;

				return;
			}
			
			case 1:   // quit game
				FreeAllLists();

			if (!USE_MENUS)
			{
				if (player[0].worldNum == WORLDID_FRONTEND)
				{
					#ifdef PC_VERSION
						if (player[0].levelNum == LEVELID_FRONTEND1)
							PostQuitMessage(0);
//							DestroyWindow (winInfo.hWndMain);
						else
							gameState.mode = INGAME_MODE;
							InitLevel(WORLDID_FRONTEND,LEVELID_FRONTEND1);						
					#endif
				}
				else
				{
					gameState.mode = INGAME_MODE;
					InitLevel(WORLDID_FRONTEND,LEVELID_FRONTEND2);
				}
			}
			else
			{
				gameState.mode = MENU_MODE;
				gameState.menuMode = TITLE_MODE;
			}

				frameCount = 0;
				lastbutton = 0;
				return;
		}

	}

	switch(currentSelection)
	{
		case 0:
			continueText->a = 255;
			continueText->r = continueText->g = continueText->b = 255;

			quitText->a = 91; //255;
			quitText->r = quitText->b = 255;
			quitText->g = 255;
			break;
		
		case 1:
			quitText->a = 255;
			quitText->r = quitText->g = quitText->b = 255;

			continueText->a = 91; //255;
			continueText->r = continueText->b = 255;
			continueText->g = 255;
			break;

	}

	lastbutton = button;
}

/*	--------------------------------------------------------------------------------
	Function		: FreeMenuItems
	Purpose			: free text / sprite overlays
	Parameters		: 
	Returns			: void
	Info			: 
*/
void FreeMenuItems()
{
	StopDrawing("FreeMenuItems");

	// free text and sprite overlays
	FreeTextOverlayLinkedList();
	FreeSpriteOverlayLinkedList();
	FreeSpriteLinkedList();

	StartDrawing("FreeMenuItems");
}

