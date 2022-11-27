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

TEXTOVERLAY *posText;
TEXTOVERLAY *levelnameText;

SPRITEOVERLAY *atari = NULL;
SPRITEOVERLAY *konami = NULL;
SPRITEOVERLAY *islLogo[] = {NULL,NULL,NULL};
SPRITEOVERLAY *flogo[10];

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
	Function 	: RunPauseMenu
	Purpose 	: In game screen that appears when start is pressed.
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
extern unsigned long actTickCount,actTickCountModifier;
extern unsigned long idleCamera;
extern float pauseGameSpeed;
void RunPauseMenu()
{
	static unsigned long currentSelection = 0;
	static u16 button,lastbutton = 0;
	
	button = controllerdata[ActiveController].button;

	if(((button & CONT_DOWN) && !(lastbutton & CONT_DOWN)))// || ((button & CONT_DOWN) && !(lastbutton & CONT_DOWN)) )
		if (currentSelection<2)
			currentSelection++;
		
	if(((button & CONT_UP) && !(lastbutton & CONT_UP)))// || ((button & CONT_DOWN) && !(lastbutton & CONT_DOWN)) )
		if (currentSelection>0)
			currentSelection--;
	
	if((button & CONT_A) && !(lastbutton & CONT_A))
	{
		lastbutton = button;

//		DisableTextOverlay ( pauseTitle );
		DisableTextOverlay ( continueText );
		DisableTextOverlay ( restartText );
		DisableTextOverlay ( quitText );
		DisableTextOverlay ( posText );
		DisableTextOverlay ( levelnameText );

		FreeScreenTextures(screenTextureList,screenTextureList2,screenTexList);

		pauseMode = 0;
		idleCamera = 0;

		switch(currentSelection)
		{
		case 0:   // continue game
		{
			long i;
			gameState.mode	= INGAME_MODE;

			if( gameState.multi==SINGLEPLAYER && player[0].worldNum != WORLDID_FRONTEND )
			{
				EnableHUD();
//				livesTextOver->a = livesTextOver->oa;
//				scoreTextOver->a = scoreTextOver->oa;

//				for ( i = 0; i < 3; i++ )
//					sprHeart[i]->draw = 1;

//				timeTextOver->a = timeTextOver->oa;
			}

			for(i=0; i<numBabies; i++)
				babyIcons[i]->draw = 1;

			actTickCountModifier = (GetTickCount()-actTickCount);
			return;
		}
		case 1:
		{
			if( gameState.multi != SINGLEPLAYER )
				ResetMultiplayer( );

			FreeAllLists();

			gameState.mode = INGAME_MODE;
			InitLevel(player[0].worldNum,player[0].levelNum);

			frameCount = 0;
			lastbutton = 0;
			return;
		}
		case 2:   // quit game
			if( gameState.multi != SINGLEPLAYER )
				ResetMultiplayer( );

			FreeAllLists();

			if (player[0].worldNum == WORLDID_FRONTEND)
			{
				#ifdef PC_VERSION
					if (player[0].levelNum == LEVELID_FRONTEND4)
						PostQuitMessage(0);
					else
					{
						gameState.mode = INGAME_MODE;
						InitLevel(WORLDID_FRONTEND,LEVELID_FRONTEND4);						
					}
				#endif
			}
			else
			{
				NUM_FROGS = 1;
				gameState.multi = SINGLEPLAYER;
				player[0].character = FROG_FROGGER;
				gameState.mode = INGAME_MODE;
				InitLevel(WORLDID_FRONTEND,LEVELID_FRONTEND4);
			}

			frameCount = 0;
			lastbutton = 0;
			return;
		}

	}

	switch(currentSelection)
	{
		case 0:
			continueText->r = continueText->g = continueText->b = 255;

			restartText->r = restartText->b = quitText->r = quitText->b = 100;
			restartText->g = quitText->g = 200;
			break;
		
		case 1:
			restartText->r = restartText->g = restartText->b = 255;
			
			continueText->r = continueText->b = quitText->r = quitText->b = 100;
			continueText->g = quitText->g = 200;
			break;
		
		case 2:
			quitText->r = quitText->g = quitText->b = 255;
			
			continueText->r = continueText->b = restartText->r = restartText->b = 100;
			continueText->g = restartText->g = 200;
			break;

	}

	lastbutton = button;

	gameSpeed = pauseGameSpeed;
	UpdateCameraPosition();
	gameSpeed = 0;
			
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
	//FreeSpriteLinkedList();

	StartDrawing("FreeMenuItems");
}











void LevelSelProcessController(long pl);

#define MAX_LEVELSTRING 10
#define MAX_OPTIONS 5
#define MAX_SPARKLES 15

#define DEC_ALPHA(x) {if (x->a > gameSpeed * fadeSpeed)	{x->a -= gameSpeed * fadeSpeed;} else	{x->a = 0; x->draw = 0;}}
#define INC_ALPHA(x,y) {if (x->a < y-gameSpeed * fadeSpeed)	{x->a += gameSpeed * fadeSpeed;} else	{x->a = y;}}

TEXTOVERLAY *worldText, *arcadeText,*selectText,*statusText,*pcText,*bestText,*parText;
TEXTOVERLAY *levelText[MAX_LEVELSTRING];
SPRITEOVERLAY *worldBak,*titleBak,*statusBak, *infoBak, *levelPic, *multiBak;
SPRITEOVERLAY *sparkles[MAX_SPARKLES];

char arcadeStr[64];
char *selectStr = "select level to play";
char *statusStr = "Cursors select   Enter confirm   Esc cancel";

char timeStr[64] = "2 51 Matthew";
char *parStr =  "Par  Set By";

char worldStr[64] = "WorldID";
char pcStr[64] = "00 percent";
char levelStr[MAX_LEVELSTRING][64] = {"","","","","","","","","",""};
unsigned long cLevel,cWorld;
float fadeSpeed = 5;
unsigned long numPlayers = 2;

char options[MAX_OPTIONS][64] = {"Music Volume","SFX Volume","Controller Config","Play Music Track 0","Play SFX 0"};

// Multiplayer select stuff
void RunMPSelect( );
void InitMPSelect( );
char charSelectMode = 0;
SPRITEOVERLAY *charPortraits[MULTI_NUM_CHARS];
SPRITEOVERLAY *charSelectIcons[MAX_FROGS];


void RunFrontendGameLoop (void)
{
	unsigned long i,j;
	GAMETILE *cur;
	
	// setup for frogger point of interest
	pOIDistance = 50000.0;
	pointOfInterest = NULL;


	if (frameCount == 1)
	{
		DisableHUD();
		strcpy(arcadeStr,"No Mode");
		for (i=0; i<MAX_SPARKLES; i++)
		{
			sparkles[i] = CreateAndAddSpriteOverlay(20,90,"flash.bmp",8,8,(rand()*0xff)/RAND_MAX,XLU_ADD);
			sparkles[i]->num = 1;
		}
		
		arcadeText = CreateAndAddTextOverlay(20,10,arcadeStr,YES,255,bigFont,0,0); 
		selectText = CreateAndAddTextOverlay(20,28,selectStr,YES,255,smallFont,0,0); 
		worldText = CreateAndAddTextOverlay(20,65,worldStr,NO,255,smallFont,0,0);
		statusText = CreateAndAddTextOverlay(20,240-18,statusStr,YES,255,smallFont,0,0);
		pcText = CreateAndAddTextOverlay(320,65+80,pcStr,NO,255,smallFont,0,0);
		
		parText = CreateAndAddTextOverlay(320,110+75,parStr,NO,255,smallFont,0,0);		
		bestText = CreateAndAddTextOverlay(320,120+75,timeStr,NO,255,smallFont,TEXTOVERLAY_WAVECHARS,0);
		bestText->waveAmplitude = 4;
		
		selectText->r = statusText->r = 0;
		selectText->g = statusText->g = 0xff;
		selectText->b = statusText->b = 10;
		for (i=0; i<MAX_LEVELSTRING; i++)
			levelText[i] = CreateAndAddTextOverlay(20,85+i*10,levelStr[i],NO,255,smallFont,TEXTOVERLAY_WAVECHARS,0);		
			
		for( i=0; i<MULTI_NUM_CHARS; i++ )
		{
			charPortraits[i] = CreateAndAddSpriteOverlay(	90 + ((i%3)*50), 50 + ((i/3)*50),
															frogPool[i].icon, 32, 32, 0, 0 );
			charPortraits[i]->draw = 0;
		}

		mpl[0].r = 30; mpl[0].g = 230; mpl[0].b = 30;
		mpl[1].r = 230; mpl[1].g = 30; mpl[1].b = 30;
		mpl[2].r = 180; mpl[2].g = 180; mpl[2].b = 230;
		mpl[3].r = 30; mpl[3].g = 30; mpl[3].b = 230;

		for( i=0; i<MAX_FROGS; i++ )
		{
			charSelectIcons[i] = CreateAndAddSpriteOverlay(	85 + ((i%3)*50), 45 + ((i/3)*50),
															"wback2.bmp", 42, 42, 0, 0 );
			charSelectIcons[i]->r = mpl[i].r;
			charSelectIcons[i]->g = mpl[i].g;
			charSelectIcons[i]->b = mpl[i].b;
			charSelectIcons[i]->draw = 0;
		}

		cLevel = 0;
		cWorld = 0;
		worldBak = CreateAndAddSpriteOverlay(10,55,"wback.bmp",130,80,190,0);
		worldBak->r = 200;
		worldBak->g = 100;
		worldBak->b = 10;

		levelPic = CreateAndAddSpriteOverlay(320-10-130+10,55+10+80,"garden1.bmp",34,30,230,0);
		
		infoBak = CreateAndAddSpriteOverlay(320-10-130,55+80,"wback.bmp",130,80,190,0);
		infoBak->r = worldBak->r;
		infoBak->g = worldBak->g;
		infoBak->b = worldBak->b;
		
		
		//titleBak = CreateAndAddSpriteOverlay(0,0,"00mwat09.bmp",32,32,190,XLU_ADD);
		titleBak = CreateAndAddSpriteOverlay(20,5,"wback.bmp",320-40,16+8+10,190,0);
		titleBak->r = worldBak->r;
		titleBak->g = worldBak->g;
		titleBak->b = worldBak->b;

		statusBak = CreateAndAddSpriteOverlay(20,240-5-18,"wback.bmp",320-40,18,190,0);
		statusBak->r = worldBak->r;
		statusBak->g = worldBak->g;
		statusBak->b = worldBak->b;

		multiBak = CreateAndAddSpriteOverlay(75,35,"wback.bmp",160,160,190,0);
		multiBak->r = worldBak->r;
		multiBak->g = worldBak->g;
		multiBak->b = worldBak->b;

		arcadeText->draw = 0;
		selectText->draw = 0;
		worldText->draw = 0;
		statusText->draw = 0;
		worldBak->draw = 0;
		infoBak->draw = 0;
		titleBak->draw = 0;
		statusBak->draw = 0;
		multiBak->draw = 0;
		levelPic->draw = 0;
		pcText->draw = 0;
		bestText->draw = 0;
		parText->draw = 0;
	}

	for (i=0; i<MAX_LEVELSTRING; i++)
		levelStr[i][0] = 0;

	//flogo[0]->num = 0;
		
	if( fadingLogos )
	{
		if (atari->xPos < 500)
		{
			atari->xPos+=2*gameSpeed;
			konami->xPos-=2*gameSpeed;
			flogo[0]->yPos+=3*gameSpeed;

			islLogo[0]->xPos+=2*gameSpeed;
			islLogo[1]->xPos+=2*gameSpeed;
			islLogo[2]->xPos+=2*gameSpeed;
		}
		else
		{
			atari->draw = 0;
			konami->draw = 0;
			flogo[0]->draw = 0;

			islLogo[0]->draw = 0;
			islLogo[1]->draw = 0;
			islLogo[2]->draw = 0;
		}
	}
	else
	{
		if (player[0].hasJumped == 1)
		{

			fadingLogos = 1;
		}
	}
	
	sprintf(pcStr,"%02i percent",worldVisualData[cWorld].levelVisualData[cLevel].percent);
	
	for (i=0; i<MAX_SPARKLES; i++)
	{
		unsigned long spkLev;

		if (sparkles[i]->a > gameSpeed * 3)
			sparkles[i]->a-= gameSpeed * 3;
		else
		{
			if (currTileNum == 5 || currTileNum == 4)
				spkLev = cLevel;
			else
				spkLev = cWorld;
			
			sparkles[i]->yPosTo = sparkles[i]->yPos = (rand()*8)/RAND_MAX + levelText[spkLev]->yPos-4;			
			sparkles[i]->a = (rand()*160)/RAND_MAX;
			sparkles[i]->xPos = levelText[spkLev]->xPos-4 + (rand() * (levelText[spkLev]->wtotal/2))/RAND_MAX;		
		}
			
		sparkles[i]->speed = 0;
		sparkles[i]->draw = levelText[cLevel]->draw;
	}
		
	// Take this out for release
	if( frameCount==1 )
		tileNum = CreateAndAddTextOverlay(0,35,tileString,YES,255,bigFont,0,0);		

	// displays the tile numbers
	cur = &firstTile[0];
	currTileNum = 0;
	while(cur)
	{
		cur = cur->next;
		if(cur == currTile[0])
		{
			currTileNum++;
			break;
		}
		currTileNum++;
	}
	
	if (tileNum)
		if (tileNum->text)
		{
			if (displayingTile)
				sprintf(tileNum->text,"%d",currTileNum);
			else
				sprintf(tileNum->text,"",currTileNum);
		}
	if (currTileNum==164)
	{
		frogFacing[0] = 3;
		Orientate( &frog[0]->actor->qRot, &currTile[0]->dirVector[frogFacing[0]], &currTile[0]->normal );	
	}

	
	if( (currTileNum!=5 && currTileNum!=3 && currTileNum!=1 && currTileNum!=4) || charSelectMode )
	{
		if( frameCount > 15 && !charSelectMode )
			GameProcessController(0);                                      
		
		DEC_ALPHA(arcadeText);
		DEC_ALPHA(pcText);
		DEC_ALPHA(bestText);
		DEC_ALPHA(parText);
		
		DEC_ALPHA(selectText);
		DEC_ALPHA(worldText);
		DEC_ALPHA(statusText);
		DEC_ALPHA(worldBak);
		DEC_ALPHA(infoBak);
		DEC_ALPHA(titleBak);
		DEC_ALPHA(statusBak);
		DEC_ALPHA(levelPic);

		for (i=0; i<MAX_LEVELSTRING; i++)
			DEC_ALPHA(levelText[i]);
		
		worldText->centred = 0;
		if (arcadeText->draw == 0)
		{
			worldBak->xPos = -100;
			infoBak->xPos = 350;
			titleBak->yPos = -100;
			statusBak->yPos = 300;
			levelPic->xPos = 400;

			worldBak->speed = 1;
			infoBak->speed = 1;
			titleBak->speed = 1;
			statusBak->speed = 1;
			levelPic->speed = 1;

			for (i=0; i<MAX_LEVELSTRING; i++)
			{
				levelText[i]->draw = 0;
				levelText[i]->centred = 0;
			}
		}
	}
	if (currTileNum==1)
	{
		worldBak->xPosTo = 95;
		worldBak->height = 80+20;
		strcpy(arcadeStr,"Options");
		sprintf(worldStr,"Some title text",numPlayers);
		
		frogFacing[0] = 3;
		Orientate( &frog[0]->actor->qRot, &currTile[0]->dirVector[frogFacing[0]], &currTile[0]->normal );

		for (i=0; i<6; i++)
		{
				strcpy (levelStr[i],options[i]);
				levelText[i]->b = 0;
				levelText[i]->g = 100;
				levelText[i]->r = 200;
				levelText[i]->waveAmplitude = 0;
		}

		levelText[cWorld]->waveAmplitude = 4;
		
		worldBak->xPos = worldBak->xPosTo;
		
				
		bestText->r = levelText[cWorld]->r = 128+(sinf(1+actFrameCount*0.09)+1)*64;
		bestText->g = levelText[cWorld]->g = 128+(sinf(2+actFrameCount*0.07)+1)*64;
		bestText->b = levelText[cWorld]->b = 10;
	
		arcadeText->yPos = titleBak->yPos+5;
		selectText->yPos = titleBak->yPos+23;
		statusText->yPos = statusBak->yPos+5;
		
		worldBak->draw = 1;
		titleBak->draw = 1;
		statusBak->draw = 1;
		statusText->draw = 1;
		arcadeText->draw = 1;
		selectText->draw = 1;
		worldText->draw = 1;
		
		worldText->centred = 1;
		
		INC_ALPHA(titleBak,0xff);
		INC_ALPHA(statusBak,0xff);
		INC_ALPHA(selectText,0xff);
		INC_ALPHA(arcadeText,0xff);
		INC_ALPHA(statusText,0xff);
		INC_ALPHA(worldBak,0xff);
		INC_ALPHA(worldText,0xff);

		DEC_ALPHA(pcText);
		DEC_ALPHA(bestText);
		DEC_ALPHA(parText);
				
		
		DEC_ALPHA(infoBak);
		DEC_ALPHA(levelPic);

		for (i=0; i<MAX_LEVELSTRING; i++)
			levelText[i]->xPos = worldText->xPos;
		
		for (i=0; i<MAX_LEVELSTRING; i++)
		{
			levelText[i]->draw = 1;
			INC_ALPHA(levelText[i],0xff);
			levelText[i]->centred = 1;
		}
		
		if (arcadeText->draw == 0)
		{
			worldBak->xPos = -100;
			infoBak->xPos = 350;
			levelPic->xPos = 400;

			worldBak->speed = 1;
			infoBak->speed = 1;
			levelPic->speed = 1;
		}

		LevelSelProcessController(0);
	}
	

	if (currTileNum==3)
	{
		// Select multiplayer characters
		if( charSelectMode )
		{
			RunMPSelect( );
			return;
		}
		else
		{
			worldBak->xPosTo = 95;
			worldBak->height = 120;
			strcpy(arcadeStr,"Multiplayer Mode");
			sprintf(worldStr,"%i player mode",numPlayers);
			
			frogFacing[0] = 3;
			Orientate( &frog[0]->actor->qRot, &currTile[0]->dirVector[frogFacing[0]], &currTile[0]->normal );

			for (i=0; i<8; i++)
			{
				strcpy (levelStr[i],worldVisualData[i].levelVisualData[multiLevelIndex[i]].description);
				levelText[i]->b = 0;
				levelText[i]->g = 100;
				levelText[i]->r = 200;
				levelText[i]->waveAmplitude = 0;
			}
			levelText[cWorld]->waveAmplitude = 4;
			strcpy (levelStr[0],worldVisualData[0].levelVisualData[1].description);

			worldBak->xPos = worldBak->xPosTo;
			
			bestText->r = levelText[cWorld]->r = 128+(sinf(1+actFrameCount*0.09)+1)*64;
			bestText->g = levelText[cWorld]->g = 128+(sinf(2+actFrameCount*0.07)+1)*64;
			bestText->b = levelText[cWorld]->b = 10;
		
			arcadeText->yPos = titleBak->yPos+5;
			selectText->yPos = titleBak->yPos+23;
			statusText->yPos = statusBak->yPos+5;
			
			worldBak->draw = 1;
			titleBak->draw = 1;
			statusBak->draw = 1;
			statusText->draw = 1;
			arcadeText->draw = 1;
			selectText->draw = 1;
			worldText->draw = 1;
			
			worldText->centred = 1;
			
			INC_ALPHA(titleBak,0xff);
			INC_ALPHA(statusBak,0xff);
			INC_ALPHA(selectText,0xff);
			INC_ALPHA(arcadeText,0xff);
			INC_ALPHA(statusText,0xff);
			INC_ALPHA(worldBak,0xff);
			INC_ALPHA(worldText,0xff);

			DEC_ALPHA(multiBak);
			DEC_ALPHA(pcText);
			DEC_ALPHA(bestText);
			DEC_ALPHA(parText);
			
			DEC_ALPHA(infoBak);
			DEC_ALPHA(levelPic);

			for( i=0; i<MULTI_NUM_CHARS; i++ )
				DEC_ALPHA( charPortraits[i] );
			for( i=0; i<numPlayers; i++ )
				DEC_ALPHA( charSelectIcons[i] );

			for (i=0; i<MAX_LEVELSTRING; i++)
				levelText[i]->xPos = worldText->xPos;
			
			for (i=0; i<MAX_LEVELSTRING; i++)
			{
				levelText[i]->draw = 1;
				INC_ALPHA(levelText[i],0xff);
				levelText[i]->centred = 1;
			}
			
			if (arcadeText->draw == 0)
			{
				worldBak->xPos = -100;
				infoBak->xPos = 350;
				levelPic->xPos = 400;

				worldBak->speed = 1;
				infoBak->speed = 1;
				levelPic->speed = 1;
			}

			LevelSelProcessController(0);
		}
	}
		
	if (currTileNum==5)
	{
		strcpy (worldStr,worldVisualData[cWorld].description);
		
		
		worldBak->height = worldVisualData[cWorld].numLevels*10 + 40;
		worldBak->xPosTo = 10;

		levelText[cLevel]->waveAmplitude = 4;
		for (i=0; i<worldVisualData[cWorld].numLevels; i++)
		{
				strcpy (levelStr[i],worldVisualData[cWorld].levelVisualData[i].description);
				levelText[i]->b = 0;
				levelText[i]->g = 100;
				levelText[i]->r = 200;
				levelText[i]->waveAmplitude = 0;
		}

		bestText->r = levelText[cLevel]->r = 128+(sinf(1+actFrameCount*0.09)+1)*64;
		bestText->g = levelText[cLevel]->g = 128+(sinf(2+actFrameCount*0.07)+1)*64;
		bestText->b = levelText[cLevel]->b = 10;
	
		levelText[cLevel]->waveAmplitude = 4;
		
		sprintf(timeStr,"%lu %02i",worldVisualData[cWorld].levelVisualData[cLevel].parTime/60,worldVisualData[cWorld].levelVisualData[cLevel].parTime%60);

		strcpy(arcadeStr,"Arcade Mode");

		frogFacing[0] = 3;
		Orientate( &frog[0]->actor->qRot, &currTile[0]->dirVector[frogFacing[0]], &currTile[0]->normal );
	
		LevelSelProcessController(0);
		arcadeText->yPos = titleBak->yPos+5;
		selectText->yPos = titleBak->yPos+23;
		pcText->xPos = infoBak->xPos+52;
		bestText->xPos = infoBak->xPos+10;
		parText->xPos = infoBak->xPos+10;
		
		statusText->yPos = statusBak->yPos+5;
		worldText->xPos = worldBak->xPos + 10;
		for (i=0; i<MAX_LEVELSTRING; i++)
			levelText[i]->xPos = worldText->xPos;

		pcText->draw = 1;
		bestText->draw = 1;
		parText->draw = 1;
		
		arcadeText->draw = 1;
		selectText->draw = 1;
		worldText->draw = 1;
		statusText->draw = 1;
		worldBak->draw = 1;
		infoBak->draw = 1;
		titleBak->draw = 1;
		statusBak->draw = 1;
		levelPic->draw = 1;
		
		INC_ALPHA(arcadeText,0xff);
		INC_ALPHA(pcText,0xff);
		
		INC_ALPHA(bestText,0xff);
		INC_ALPHA(parText,0xff);
		INC_ALPHA(selectText,0xff);
		INC_ALPHA(worldText,0xff);
		INC_ALPHA(statusText,0xff);
		INC_ALPHA(worldBak,0xff);
		INC_ALPHA(infoBak,0xff);
		INC_ALPHA(titleBak,0xff);
		INC_ALPHA(statusBak,0xff);
		INC_ALPHA(levelPic,0xff);
		
		for (i=0; i<MAX_LEVELSTRING; i++)
		{
			levelText[i]->draw = 1;
			INC_ALPHA(levelText[i],0xff);
		}
	}

	if (currTileNum==4)
	{
		sprintf (worldStr,"Chapter %i",cWorld+1);
		worldBak->height = worldVisualData[cWorld].numLevels*10 + 40;
		worldBak->xPosTo = 95;

		levelText[cLevel]->waveAmplitude = 4;
		for (i=0; i<worldVisualData[cWorld].numLevels; i++)
		{
				strcpy (levelStr[i],worldVisualData[cWorld].levelVisualData[i].description);
				levelText[i]->b = 0;
				levelText[i]->g = 100;
				levelText[i]->r = 200;
				levelText[i]->waveAmplitude = 0;
		}

		bestText->r = levelText[cLevel]->r = 128+(sinf(1+actFrameCount*0.09)+1)*64;
		bestText->g = levelText[cLevel]->g = 128+(sinf(2+actFrameCount*0.07)+1)*64;
		bestText->b = levelText[cLevel]->b = 10;
	
		levelText[cLevel]->waveAmplitude = 4;

		strcpy(arcadeStr,"Story Mode");

		frogFacing[0] = 3;
		Orientate( &frog[0]->actor->qRot, &currTile[0]->dirVector[frogFacing[0]], &currTile[0]->normal );
		
		worldBak->xPos = worldBak->xPosTo;
		
		LevelSelProcessController(0);

		if (gameState.mode == FMVPLAY_MODE)
			return;
		
		arcadeText->yPos = titleBak->yPos+5;
		selectText->yPos = titleBak->yPos+23;
		pcText->xPos = infoBak->xPos+52;
		bestText->xPos = infoBak->xPos+10;
		parText->xPos = infoBak->xPos+10;
		
		statusText->yPos = statusBak->yPos+5;
		worldText->xPos = worldBak->xPos + 10;
		for (i=0; i<MAX_LEVELSTRING; i++)
			levelText[i]->xPos = worldText->xPos;

		arcadeText->draw = 1;
		selectText->draw = 1;
		worldText->draw = 1;
		statusText->draw = 1;
		worldBak->draw = 1;
		infoBak->draw = 1;
		titleBak->draw = 1;
		statusBak->draw = 1;
		
		INC_ALPHA(arcadeText,0xff);
		INC_ALPHA(pcText,0xff);
		
		//INC_ALPHA(bestText,0xff);
		//INC_ALPHA(parText,0xff);
		INC_ALPHA(selectText,0xff);
		INC_ALPHA(worldText,0xff);
		INC_ALPHA(statusText,0xff);
		INC_ALPHA(worldBak,0xff);
		INC_ALPHA(titleBak,0xff);
		INC_ALPHA(statusBak,0xff);
		//INC_ALPHA(levelPic,0xff);
		
		for (i=0; i<MAX_LEVELSTRING; i++)
		{
			levelText[i]->draw = 1;
			INC_ALPHA(levelText[i],0xff);
			levelText[i]->centred = 1;
		}
	}

	UpdateCameraPosition();
	
	for( i=0; i<NUM_FROGS; i++ )
	{
		if( frog[i] )
		{
			UpdateFroggerPos(i);

			if( !(player[i].frogState & FROGSTATUS_ISDEAD) )
			{
				frog[i]->draw = 1;
				if (player[i].safe.time)
				{
					GTUpdate( &player[i].safe, -1 );
				}

				if( player[i].idleEnable )
				{
					player[i].idleTime-=gameSpeed;
					if(player[i].idleTime<1)
					{
						FroggerIdleAnim(i);
						player[i].idleTime = MAX_IDLE_TIME + Random(MAX_IDLE_TIME);
					}
				}
			}
		}
	}  

	UpdatePlatforms();
	UpdateEnemies();
	UpdateSpecialEffects();
	UpdateEvents();
	UpdateAmbientSounds();

	ProcessCharacters( );
	ProcessCollectables();

	UpDateOnScreenInfo();
}


void LevelSelProcessController(long pl)
{
	unsigned long changedLevel = 0;
	u16 button[4],lastbutton[4];

	button[pl] = controllerdata[pl].button;
	lastbutton[pl] = controllerdata[pl].lastbutton;
	
	
	if((button[pl] & CONT_UP) && !(lastbutton[pl] & CONT_UP) && player[pl].canJump)
	{
		if (currTileNum==5 || currTileNum==4)
		{		
			if (cLevel>0)
				cLevel--;
			changedLevel = 1;
		}

		if (currTileNum==3  || currTileNum==1)
		{
			if (cWorld>0)
				cWorld--;
			changedLevel = 1;		
		}
	}	    
	else if((button[pl] & CONT_RIGHT) && !(lastbutton[pl] & CONT_RIGHT) && player[pl].canJump)
	{
		if (currTileNum==5 || currTileNum==4)
		{				
			if (cWorld<8)
				cWorld++;
			changedLevel = 1;
		}

		if (currTileNum==3)
		{
			if (numPlayers<4)
				numPlayers++;
		}
	}
    else if((button[pl] & CONT_DOWN) && !(lastbutton[pl] & CONT_DOWN) && player[pl].canJump)
	{
		if (currTileNum==5 || currTileNum==4)
		{				
			if (cLevel<worldVisualData[cWorld].numLevels-1)
				cLevel++;
			changedLevel = 1;
		}

		if (currTileNum==3 || currTileNum==1)
		{		
			if (cWorld<8)
				cWorld++;
			changedLevel = 1;
		}
	}
    else if((button[pl] & CONT_LEFT) && !(lastbutton[pl] & CONT_LEFT) && (player[pl].canJump))
	{
		if (currTileNum==5 || currTileNum==4)
		{		
		
			if (cWorld>0)
				cWorld--;
			changedLevel = 1;
		}

		if (currTileNum==3)
		{
			if (numPlayers>2)
				numPlayers--;
		}
	}

	if (changedLevel)
		FindTexture(&levelPic->frames[0],UpdateCRC(worldVisualData[cWorld].levelVisualData[cLevel].texCRC),YES);

	if((button[pl] & CONT_A) && !(lastbutton[pl] & CONT_A))
    {
		if (currTileNum==4)
		{
			if(gameState.multi != MULTIREMOTE)
			{
				NUM_FROGS = 1;
				gameState.multi = SINGLEPLAYER;
				gameState.single = ARCADE_MODE;
			}
			player[0].worldNum = cWorld;
			player[0].levelNum = cLevel;			
			
			lastActFrameCount = actFrameCount;

			FreeAllLists();
			gameState.mode = FMVPLAY_MODE;
			showEndLevelScreen = 0;		
		}
		else
		{
			if (currTileNum==5)
			{
				if(gameState.multi != MULTIREMOTE)
				{
					NUM_FROGS = 1;
					gameState.multi = SINGLEPLAYER;
					gameState.single = ARCADE_MODE;
				}
				player[0].worldNum = cWorld;
				player[0].levelNum = cLevel;			
			}
						
			if (currTileNum==3)
			{
				// Store in player 1 so the frontend still works :)
				player[1].worldNum = cWorld;
				InitMPSelect( );
				return;
			}

			lastActFrameCount = actFrameCount;
			gameState.mode = LEVELCOMPLETE_MODE;
			GTInit( &modeTimer, 1 );
			showEndLevelScreen = 0;		
		}
	}
	
	if((button[pl] & CONT_B) && !(lastbutton[pl] & CONT_B) && (tongue[pl].flags & TONGUE_IDLE))
    {
	}

	if((button[pl] & CONT_E) && !(lastbutton[pl] & CONT_E))
    {
	}

	if((button[pl] & CONT_D) && !(lastbutton[pl] & CONT_D))
    {
	}

	if((button[pl] & CONT_START) && !(lastbutton[pl] & CONT_START))
	{
		
		player[pl].inputPause = INPUT_POLLPAUSE;
	    
		player[pl].canJump = 0;
		player[pl].frogState |= FROGSTATUS_ISWANTINGD;

		// update player idleTime
		player[pl].idleTime = MAX_IDLE_TIME;
		idleCamera = 0;

		if ( recordKeying )
			AddPlayingActionToList ( MOVEMENT_DOWN, frameCount );
		player[pl].extendedHopDir = MOVE_DOWN;
	}
}


/* --------------------------------------------------------------------------------
	Function	: RunMPSelect
	Purpose		: Select characters for multiplayer
	Parameters	: 
	Returns		: 
*/
void RunMPSelect( )
{
	int i, num;
	static char ready[4] = {0,0,0,0};

	for( i=0; i<MULTI_NUM_CHARS; i++ )
	{
		if( frogPool[i].active )
		{
			INC_ALPHA( charPortraits[i], 0xff );
		}
		else
		{
			INC_ALPHA( charPortraits[i], 0x33 );
		}
	}

	INC_ALPHA( multiBak, 0xff );

	for( i=0; i<numPlayers; i++ )
	{
		short inc = gameSpeed*fadeSpeed;
		INC_ALPHA( charSelectIcons[i], 190 );

		if( ready[i] )
		{
			if( charSelectIcons[i]->r < 0xff-inc ) charSelectIcons[i]->r += inc; else charSelectIcons[i]->r = 0xff;
			if( charSelectIcons[i]->g < 0xff-inc ) charSelectIcons[i]->g += inc; else charSelectIcons[i]->g = 0xff;
			if( charSelectIcons[i]->b < 0xff-inc ) charSelectIcons[i]->b += inc; else charSelectIcons[i]->b = 0xff;
		}
		else
		{
			if( charSelectIcons[i]->r > mpl[i].r+inc ) charSelectIcons[i]->r -= inc; else charSelectIcons[i]->r = mpl[i].r;
			if( charSelectIcons[i]->g > mpl[i].g+inc ) charSelectIcons[i]->g -= inc; else charSelectIcons[i]->g = mpl[i].g;
			if( charSelectIcons[i]->b > mpl[i].b+inc ) charSelectIcons[i]->b -= inc; else charSelectIcons[i]->b = mpl[i].b;

			if( (controllerdata[i].button & CONT_UP) && !(controllerdata[i].lastbutton & CONT_UP) && (charSelectIcons[i]->yPos > 80) )
				charSelectIcons[i]->yPos -= 50;

			if( (controllerdata[i].button & CONT_DOWN) && !(controllerdata[i].lastbutton & CONT_DOWN) && (charSelectIcons[i]->yPos < 130) )
				charSelectIcons[i]->yPos += 50;

			if( (controllerdata[i].button & CONT_LEFT) && !(controllerdata[i].lastbutton & CONT_LEFT) && (charSelectIcons[i]->xPos > 100) )
				charSelectIcons[i]->xPos -= 50;

			if( (controllerdata[i].button & CONT_RIGHT) && !(controllerdata[i].lastbutton & CONT_RIGHT) && (charSelectIcons[i]->xPos < 180) )
				charSelectIcons[i]->xPos += 50;

			if( (controllerdata[i].button & CONT_A) && !(controllerdata[i].lastbutton & CONT_A) )
			{
				// Set character number in player struct
				num = ((charSelectIcons[i]->xPos-85) / 50) + (3 * ((charSelectIcons[i]->yPos-45) / 50));

				if( frogPool[num].active )
				{
					player[i].character = num;
					ready[i] = 1;
				}
			}
		}

		if( (controllerdata[i].button & CONT_B) && !(controllerdata[i].lastbutton & CONT_B) )
		{
			ready[i] = 0;
		}

		if( (controllerdata[i].button & CONT_START) && !(controllerdata[i].lastbutton & CONT_START) )
		{
			player[i].inputPause = INPUT_POLLPAUSE;
			charSelectMode = 0;
		}
	}

	for( i=0; i<numPlayers; i++ ) if( !ready[i] ) break;

	if( i == numPlayers )
	{
		charSelectMode = 0;
		ready[0] = ready[1] = ready[2] = ready[3] = 0;

		NUM_FROGS = numPlayers;
		player[0].worldNum = player[1].worldNum;

		gameState.single = INVALID_MODE;
		if(gameState.multi != MULTIREMOTE)
			gameState.multi = MULTILOCAL;

		lastActFrameCount = actFrameCount;
		gameState.mode = LEVELCOMPLETE_MODE;
		GTInit( &modeTimer, 1 );
		showEndLevelScreen = 0;

		switch( player[0].worldNum )
		{
		case WORLDID_ANCIENT: 
			player[0].levelNum = 3;
			multiplayerMode = MULTIMODE_RACE; 
			break;
		case WORLDID_GARDEN:
			player[0].levelNum = 1;
			multiplayerMode = MULTIMODE_RACE; 
			break;
		case WORLDID_LABORATORY: 
			player[0].levelNum = 3;
			multiplayerMode = MULTIMODE_BATTLE; 
			break;
		case WORLDID_SUPERRETRO:
			player[0].levelNum = 0;
			multiplayerMode = MULTIMODE_COLLECT;
			break;
		}

		return;
	}

	UpdateCameraPosition();
	
	for( i=0; i<NUM_FROGS; i++ )
	{
		if( frog[i] )
		{
//			UpdateFroggerPos(i);

			if( !(player[i].frogState & FROGSTATUS_ISDEAD) )
			{
				frog[i]->draw = 1;
				if (player[i].safe.time)
				{
					GTUpdate( &player[i].safe, -1 );
				}

				if( player[i].idleEnable )
				{
					player[i].idleTime-=gameSpeed;
					if(player[i].idleTime<1)
					{
						FroggerIdleAnim(i);
						player[i].idleTime = MAX_IDLE_TIME + Random(MAX_IDLE_TIME);
					}
				}
			}
		}
	}  

	UpdatePlatforms();
	UpdateEnemies();
	UpdateSpecialEffects();
	UpdateEvents();
	UpdateAmbientSounds();

	ProcessCharacters( );
	ProcessCollectables();

	UpDateOnScreenInfo();

}

void InitMPSelect( )
{
	int i;

	multiBak->draw = 1;
	charSelectMode = 1;

	for( i=0; i<MULTI_NUM_CHARS; i++ )
		charPortraits[i]->draw = 1;

	for( i=0; i<numPlayers; i++ )
		charSelectIcons[i]->draw = 1;
}










/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: Credits on frontend

	Purpose		:
	Parameters	: (void)
	Returns		: void 
*/

extern float camSideOfs;
long playMode = NORMAL_PMODE;
long clearTiles = 0;


//////////////////////////////////////////

TEXT3D *cText[4];
TEXT3D *rText[4];

unsigned char *credits[] = 
{
	"","Programming",	"Matthew Cloy",		"",
	"","\x1",			"Andy Eder",		"",
	"","\x1",			"James Hubbard",	"",
	"","\x1",			"David Swift",		"",
	"","\x1",			"James Healey",		"",


	"","Art",			"Alex Rigby","",
	"","\x1",			"Joff Scarcliffe","", 
	"","\x1",			"Bruce Millar","",
	"","\x1",			"Lauren Grindrod","",
	"","\x1",			"Simon Little","",
	"","\x1",			"Sandro Da Cruz","",
	"","\x1",			"Richard Whale","",
	"","\x1",			"Mark Turner","",
	0,0,0,0,0,0,0,0
};

unsigned long cNumber = 0;
unsigned long cFrame = 0;
unsigned long creditsActive = 0;
GAMETILE *credTile;

#define CRED_SPEED		(60*3)
#define CRED_BORDERIN	(60*0.8)
#define CRED_BORDEROUT	(60*0.8)


void RunCredits()
{
	unsigned long cn;

	if (creditsActive==0)
		return;

	if (actFrameCount>cFrame)
	{
		if (creditsActive==2)
		{
			creditsActive=3;
			return;
		}

		cFrame = actFrameCount + CRED_SPEED;
		cNumber+= 4;

		if (credits[cNumber] == 0)
			cNumber = 0;
	
		Modify3DText(cText[0], credits[cNumber],220);
		Modify3DText(cText[1], credits[cNumber+1],220);
		Modify3DText(cText[2], credits[cNumber+2],220);
		Modify3DText(cText[3], credits[cNumber+3],220);

		Modify3DText(rText[1], credits[cNumber+1],220);
		Modify3DText(rText[2], credits[cNumber+2],220);

	}

	for (cn=0; cn<4; cn++)
		cText[cn]->angle = 90+360*2;

	
	if (actFrameCount<(cFrame-CRED_SPEED+CRED_BORDERIN))
	{
		unsigned long amt;
		amt = 0xff - ((((cFrame-CRED_SPEED+CRED_BORDERIN)-actFrameCount) * 255) / CRED_BORDEROUT);
		for (cn=0; cn<4; cn++)
			if ((credits[cNumber+cn]) && (credits[cNumber+cn][0]!=1))
			{
				cText[cn]->vA = amt; 
				//cText[cn]->angle = 90+ ((amt * 360 * 2) / 0xff);
				cText[cn]->yScale = 3-(amt/128.0);
				cText[cn]->sinA = 6-(amt/(256.0/6));
			}
		
	}

	if (actFrameCount>(cFrame-CRED_BORDEROUT))
	{
		unsigned long amt;
		amt = 0xff - (((actFrameCount-(cFrame-CRED_BORDEROUT)) * 255) / CRED_BORDEROUT);
		for (cn=0; cn<4; cn++)
			if ((creditsActive == 2) || ((credits[cNumber+cn+4]) && (credits[cNumber+cn+4][0]!=1)))
			{
				cText[cn]->vA = amt; 
				//cText[cn]->angle = 90+ ((amt * 360 * 2) / 0xff);
				cText[cn]->yScale = 3-(amt/128.0);
				cText[cn]->sinA = 6-(amt/(256.0/6));
			}
	}

	for (cn=1; cn<3; cn++)
	{
		rText[cn]->angle = (360*8) - (cText[cn]->angle);
		rText[cn]->vA = ((unsigned char) cText[cn]->vA) / (6-(cn*2));
		rText[cn]->yScale = cText[cn]->yScale;
	}
}

void DeactivateCredits(void)
{
	unsigned long cn;
	if (creditsActive)
	{
		if (creditsActive == 1)
		{
			creditsActive = 2;
			cFrame = actFrameCount + CRED_BORDEROUT;
		}

		if (creditsActive == 3)
		{
			for (cn=0; cn<4; cn++)
				cText[cn]->motion &= ~T3D_CREATED;
			for (cn=1; cn<3; cn++)
				rText[cn]->motion &= ~T3D_CREATED;
			creditsActive = 0;
			
		}
	}
}

void ActivateCredits(void)
{
	unsigned long cn;

	if (creditsActive == 0)
	{
		for (cn=0; cn<4; cn++)
			cText[cn]->motion |= T3D_CREATED;
		for (cn=1; cn<3; cn++)
			rText[cn]->motion |= T3D_CREATED;
		creditsActive = 1;

		cFrame = actFrameCount + CRED_SPEED;
		cNumber = 0;

		Modify3DText(cText[0], credits[cNumber],220);
		Modify3DText(cText[1], credits[cNumber+1],220);
		Modify3DText(cText[2], credits[cNumber+2],220);
		Modify3DText(cText[3], credits[cNumber+3],220);

		Modify3DText(rText[1], credits[cNumber+1],220);
		Modify3DText(rText[2], credits[cNumber+2],220);

	}
}

void InitCredits(void)
{
	cText[0] = CreateAndAdd3DText( credits[cNumber], 180,
		255,30,0,220,
		T3D_HORIZONTAL,
		T3D_MOVE_MODGE | T3D_MOVE_SPIN | T3D_ALIGN_CENTRE,
		&zero,
		0,0,
		-30,120,-440,
		0, 0.2, 0.0 );
	
	
	cText[1] = CreateAndAdd3DText( credits[cNumber+1], 180,
		255,30,0,220,
		T3D_HORIZONTAL,
		T3D_MOVE_MODGE | T3D_MOVE_SPIN | T3D_ALIGN_CENTRE,
		&zero,
		0,0,
		-30,90,-440,
		0, 0.3, 0.0 );

	cText[2] = CreateAndAdd3DText(credits[cNumber+2], 250,
		255,220,30,220,
		T3D_HORIZONTAL,
		T3D_MOVE_MODGE | T3D_MOVE_SPIN | T3D_ALIGN_CENTRE,
		&zero,
		0,0,
		-30,60,-440,
		0, 0.25, 0.0 );

	cText[3] = CreateAndAdd3DText( credits[cNumber+3], 250,
		255,220,30,220,
		T3D_HORIZONTAL,
		T3D_MOVE_MODGE | T3D_MOVE_SPIN | T3D_ALIGN_CENTRE,
		&zero,
		0,0,
		-30,30,-440,
		0, 0.28, 0.0 );
			
				
	rText[1] = CreateAndAdd3DText( credits[cNumber+1], 180,
		255,30,130,220,
		T3D_HORIZONTAL,
		T3D_MOVE_MODGE | T3D_MOVE_SPIN | T3D_ALIGN_CENTRE,
		&zero,
		0,0,
		-30,-90-30,-440,
		3, 0.3, 0.0 );

	rText[2] = CreateAndAdd3DText(credits[cNumber+2], 250,
		255,220,130,220,
		T3D_HORIZONTAL,
		T3D_MOVE_MODGE | T3D_MOVE_SPIN | T3D_ALIGN_CENTRE,
		&zero,
		0,0,
		-30,-60-30,-440,
		3, 0.25, 0.0 );
}
