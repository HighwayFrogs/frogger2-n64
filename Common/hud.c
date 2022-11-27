/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: hud.c
	Programmer	: James Healey
	Date		: 31/03/99

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>

#include "incs.h"


char scoreText[32]	= "10000000";
char livesText[8]	= "xxxx";
char timeText[32]	= "00:00      ";
char garibText[8], creditText[8];

char timeTemp[6];

char countdownTimer	= 1;
char displayFullScreenPoly = 0;

//ANIM_STRUCTURE *livesIcon;

//TEXTOVERLAY	*livesTextOver,*timeTextOver,*scoreTextOver;
TEXTOVERLAY	*babySavedText;

TEXTOVERLAY *keyCollected;

//TEXTOVERLAY *pauseTitle;

TEXTOVERLAY *continueText;
TEXTOVERLAY *restartText;
TEXTOVERLAY *quitText;
TEXTOVERLAY *garibCount, *creditCount;
TEXTOVERLAY *time;
//TEXTOVERLAY *build;
TEXTOVERLAY *posText;
TEXTOVERLAY *levelnameText;

TEXTOVERLAY *wholeKeyText = NULL;

char levelString[] = "Levelname Goes in here";
char posString[] = "-----------------------";

extern long totalFacesDrawn;
extern long numPixelsDrawn;
extern long numSprites;

///////////////////////////////////////////////////////////////////////////////////////

#define MAX_HUD_SPARKLES 20

typedef struct TAG_ARCADE_HUD
{
	SPRITEOVERLAY *sparkles[MAX_HUD_SPARKLES];
	unsigned long sX,sY,sW,sH;
	unsigned long sTime;

	SPRITEOVERLAY *backLeft;
	SPRITEOVERLAY *backLeftExtra;

	SPRITEOVERLAY *backRight;
	SPRITEOVERLAY *backRightExtra;

	SPRITEOVERLAY *backCentre;
	
	SPRITEOVERLAY *livesOver;
	TEXTOVERLAY   *livesText;
	
	SPRITEOVERLAY *coinsOver;
	TEXTOVERLAY   *coinsText;
	
	SPRITEOVERLAY *timeOver;
	TEXTOVERLAY   *timeTextMin;
	TEXTOVERLAY   *timeTextSec;

	TEXTOVERLAY   *parText;

	SPRITEOVERLAY *coins;

	TEXTOVERLAY   *timeOutText;
	unsigned long timedOut;
} ARCADE_HUD;


typedef struct TAG_MULTI_HUD
{
	SPRITEOVERLAY *backTime;
	SPRITEOVERLAY *backPenalise[MAX_FROGS];
	SPRITEOVERLAY *backChars[MAX_FROGS];
	TEXTOVERLAY   *penaliseText[MAX_FROGS];
	TEXTOVERLAY	  *winsText[MAX_FROGS];

	TEXTOVERLAY   *timeTextMin;
	TEXTOVERLAY   *timeTextSec;
	TEXTOVERLAY   *timeTextHSec;

	TEXTOVERLAY   *centreText;

} MULTI_HUD;


ARCADE_HUD arcadeHud;
MULTI_HUD multiHud;
char timeStringMin[8]	= "00";
char timeStringSec[8]	= "00";
char timeStringHSec[8]	= "00";

char coinsText[32] = "00 of 32";
char timeOutString[64] = "Out of time";
char penalString[4][8] = {"00","00","00","00"};
char winsString[4][8] = {"00","00","00","00"};

char countdownString[64] = "00";

TEXTOVERLAY   *polysText;
char polyString[256] = "";

void InitArcadeHUD(void)
{
	int i;
	arcadeHud.livesOver = CreateAndAddSpriteOverlay(5,240-27,"hud_frog.bmp",32,32,0xff,0);
	arcadeHud.timeOver = CreateAndAddSpriteOverlay(236,240-27,"hud_time.bmp",32,32,0xff,0);
	arcadeHud.coinsOver = CreateAndAddSpriteOverlay(68,240-4-13,"hud_coin.bmp",12,12,0xff,0);
	
	arcadeHud.backLeftExtra = CreateAndAddSpriteOverlay(3,240-2-26,"prc_watrd1.bmp",62,26,0xff,XLU_ADD);
	arcadeHud.backLeft = CreateAndAddSpriteOverlay(3,240-2-26,"wback2.bmp",62,25,170,0);

	arcadeHud.backRightExtra = CreateAndAddSpriteOverlay(254-20,240-4-26,"prc_watrt1.bmp",82,26,0xff,XLU_ADD);
	arcadeHud.backRight = CreateAndAddSpriteOverlay(254-20,240-3-25,"wback2.bmp",82,25,170,0);

	arcadeHud.backCentre = CreateAndAddSpriteOverlay(65,240-3-15,"wback2.bmp",254-20-65,15,170,0);

	arcadeHud.livesText = CreateAndAddTextOverlay(5+32,240-7-16,livesText,NO,255,currFont,0,0);
	arcadeHud.coinsText = CreateAndAddTextOverlay(68+16,240-2-13,coinsText,NO,255,smallFont,0,0);
	arcadeHud.coinsText->r = 100;
	arcadeHud.coinsText->g = 200;
	arcadeHud.coinsText->b = 230;

	arcadeHud.timeTextMin = CreateAndAddTextOverlay(234+22,240-7-16,timeStringMin,NO,255,currFont,0,0);
	arcadeHud.timeTextSec = CreateAndAddTextOverlay(234+22+32,240-7-16,timeStringSec,NO,255,currFont,0,0);
	
	arcadeHud.timeOutText = CreateAndAddTextOverlay(0,120,timeOutString,YES,255,currFont,0,0);
	arcadeHud.timeOutText->r = 0xff;
	arcadeHud.timeOutText->g = 0;
	arcadeHud.timeOutText->b = 0;
	arcadeHud.timeOutText->a = 0;

	polysText = CreateAndAddTextOverlay(2,2,polyString,NO,255,smallFont,0,0);
	
	arcadeHud.timedOut = 0;

	for (i=0; i<MAX_HUD_SPARKLES; i++)
	{
		arcadeHud.sparkles[i] = CreateAndAddSpriteOverlay((rand()*320 / RAND_MAX),(rand()*240 / RAND_MAX),"flash2.bmp",10,10,0xff,XLU_ADD);
		arcadeHud.sparkles[i]->r = 200;
		arcadeHud.sparkles[i]->g = 200;
		arcadeHud.sparkles[i]->b = 0;

		arcadeHud.sparkles[i]->a = 0;
		arcadeHud.sparkles[i]->draw = 0;
		arcadeHud.sparkles[i]->num = 1;
	}
}

long xPos_multi[4] = {5,320 - 85,320 - 85,5};
long yPos_multi[4] = {5,5,240 - 50,240 - 50};

void InitMultiHUD()
{
	int i;

	switch( multiplayerMode )
	{
	case MULTIMODE_BATTLE:
	case MULTIMODE_COLLECT:
		break;
	case MULTIMODE_RACE:
		multiHud.backTime = CreateAndAddSpriteOverlay(160-40,5,"wback2.bmp",80,25,170,0);
		multiHud.timeTextMin = CreateAndAddTextOverlay(120-8,9,timeStringMin,NO,255,currFont,0,0);
		multiHud.timeTextSec = CreateAndAddTextOverlay(120+32-8,9,timeStringSec,NO,255,currFont,0,0);
		multiHud.timeTextHSec = CreateAndAddTextOverlay(120+32+32-8,9,timeStringHSec,NO,255,currFont,0,0);
		multiHud.timeTextHSec->scale = 0.75;
		break;
	}

	multiHud.centreText = CreateAndAddTextOverlay(1,100,countdownString,YES,255,currFont,0,0);

	for (i = 0; i<NUM_FROGS; i++)
	{
		multiHud.backChars[i] = CreateAndAddSpriteOverlay(xPos_multi[i]+64,yPos_multi[i]+5,frogPool[player[i].character].icon,16,16,0xff,0);
		multiHud.backPenalise[i] = CreateAndAddSpriteOverlay(xPos_multi[i],yPos_multi[i],"wback2.bmp",80,45,170,0);
		multiHud.penaliseText[i] = CreateAndAddTextOverlay(xPos_multi[i]+5,yPos_multi[i]+5,penalString[i],NO,255,currFont,0,0);
		multiHud.penaliseText[i]->scale = 2;
		multiHud.winsText[i] = CreateAndAddTextOverlay(xPos_multi[i]+64,yPos_multi[i]+24,winsString[i],NO,255,currFont,0,0);
		multiHud.winsText[i]->scale = 0.5;
	}
}

void InitHUD(void)
{
	if (gameState.multi == SINGLEPLAYER)
		InitArcadeHUD();		
	else
		InitMultiHUD();	
}

void SparkleCoins(void)
{
	arcadeHud.sX = 82;
	arcadeHud.sY = 240-14;
	arcadeHud.sW = 50;
	arcadeHud.sH = 4;
	arcadeHud.sTime = actFrameCount + 30;
}

void SparkleBabies(void)
{
	arcadeHud.sX = ((234)-(numBabies*15));
	arcadeHud.sY = 240-14;
	arcadeHud.sW = (numBabies*15)-15;
	arcadeHud.sH = 4;
	arcadeHud.sTime = actFrameCount + 30;
}

void DisableHUD(void)
{
	arcadeHud.livesOver->draw = arcadeHud.timeOver->draw = arcadeHud.coinsOver->draw = arcadeHud.backLeftExtra->draw = 
	arcadeHud.backLeft->draw = arcadeHud.backRightExtra->draw = arcadeHud.backRight->draw = arcadeHud.backCentre->draw = 
	arcadeHud.livesText->draw = arcadeHud.coinsText->draw = arcadeHud.timeTextMin->draw = arcadeHud.timeTextSec->draw = 0;
	arcadeHud.timeOutText->draw = 0;
}

void EnableHUD(void)
{
	arcadeHud.livesOver->draw = arcadeHud.timeOver->draw = arcadeHud.coinsOver->draw = arcadeHud.backLeftExtra->draw = 
	arcadeHud.backLeft->draw = arcadeHud.backRightExtra->draw = arcadeHud.backRight->draw = arcadeHud.backCentre->draw = 
	arcadeHud.livesText->draw = arcadeHud.coinsText->draw = arcadeHud.timeTextMin->draw = arcadeHud.timeTextSec->draw = 1;
	arcadeHud.timeOutText->draw = 1;
}

void UpDateMultiplayerInfo( void )
{
	long timeFrames=0,i;

	multiHud.centreText->a = 0xff;
	if( multiTimer.time>1)
	{
		if( multiTimer.time<5)
		{
			sprintf(countdownString,"%lu",multiTimer.time-1);
			multiHud.centreText->scale = 2;
			switch (multiTimer.time)
			{
				case 4:
					multiHud.centreText->r = 0xff;
					multiHud.centreText->g = 0;
					multiHud.centreText->b = 0;
					break;
				case 3:
					multiHud.centreText->r = 0xff;
					multiHud.centreText->g = 180;
					multiHud.centreText->b = 30;
					break;
				case 2:
					multiHud.centreText->r = 0;
					multiHud.centreText->g = 0xff;
					multiHud.centreText->b = 0;
					break;

			}
		}
		else
		{
			sprintf(countdownString,"get ready");
			multiHud.centreText->scale = 1;
		}
		multiHud.centreText->draw = 1;
	}
	else
	{
		multiHud.centreText->r = multiHud.centreText->b = multiHud.centreText->g = 0xff;
		 			
		multiHud.centreText->scale = 1;
		if (multiTimer.time==1)
			sprintf(countdownString,"go");
		else
		{
			if (matchWinner==-1)
				multiHud.centreText->draw = 0;
			else
			{
				sprintf(countdownString,"%s won",frogPool[player[matchWinner].character].name);
				multiHud.centreText->draw = 1;
				multiHud.centreText->a = 128+(sinf(actFrameCount * 0.3) * 128);		
			}
		}
	}
	
	switch( multiplayerMode )
	{
	case MULTIMODE_RACE:
		for (i=0; i<NUM_FROGS; i++)
		{
			sprintf(winsString[i],"%02i",mpl[i].wins);
			sprintf(penalString[i],"%02i",((int)(mpl[i].penalty/60)));
			if (mpl[i].timer>timeFrames)
				timeFrames=mpl[i].timer;
		}
		sprintf(timeStringHSec,"%02i",((int)(timeFrames*100)/60)%100);
		sprintf(timeStringSec,"%02i",((int)timeFrames/60)%60);
		sprintf(timeStringMin,"%2i",((int)timeFrames/(60*60))%60);	

		break;
	case MULTIMODE_BATTLE:
	case MULTIMODE_COLLECT:
		for (i=0; i<NUM_FROGS; i++)
		{
			sprintf(winsString[i],"%02i",mpl[i].wins);
			sprintf(penalString[i],"%02i",mpl[i].score);
		}

		break;
	}
}

void UpDateOnScreenInfo( void )
{
	long i;
	long xPos,yPos;
	long timeFrames = worldVisualData[player[0].worldNum].levelVisualData[player[0].levelNum].parTime * 60;
	long frameCheck = 0;
	long r,g,b,a,h;

	sprintf(polyString,"%lu tri  %lu spr  %lu kpixels  %lu pc ovdrw",totalFacesDrawn,numSprites,numPixelsDrawn/1000,(numPixelsDrawn*100/307200));
	
	
	if (totalFacesDrawn>2000)
		if (totalFacesDrawn>2500)
			if (totalFacesDrawn>2800)
				frameCheck = 3;
			else
				frameCheck = 2;
		else
			frameCheck = 1;
	
	switch(frameCheck)
	{
		case 0:
			r=g=b=0xff;
			a = 100;
			break;
		case 1:
			r=255;
			g=180;
			b=0;
			a = 200;
			break;
		case 2:
			r=255;
			g=0;
			b=0;
			a = 255;
			break;
		case 3:
			r=255;
			g=0;
			b=0;
			a = 128+ (64 * (1+sinf(actFrameCount / 5.0)));			
			break;
	}

	polysText->r = r;
	polysText->g = g;
	polysText->b = b;
	polysText->a = a;
	
	timeFrames -=actFrameCount;
	if (timeFrames<0)
	{
		timeFrames = 0;
		if (!arcadeHud.timedOut)
			arcadeHud.timeOutText->a = 0xff;
	
		if (arcadeHud.timeOutText->a > gameSpeed)
			arcadeHud.timeOutText->a-=gameSpeed;

		arcadeHud.timedOut = 1;
		arcadeHud.timeTextMin->r = arcadeHud.timeTextSec->r = 255;
		arcadeHud.timeTextMin->g = arcadeHud.timeTextSec->g = 0;
		arcadeHud.timeTextMin->b = arcadeHud.timeTextSec->b = 0;

	}

	sprintf(livesText,"%lu",player[0].lives);	
	sprintf(timeStringSec,"%02i",((int)timeFrames/60)%60);
	sprintf(timeStringMin,"%2i",((int)timeFrames/(60*60))%60);	
	sprintf(coinsText,"%02i of %02i",garibList.total-garibList.count,garibList.total);

	if (actFrameCount<arcadeHud.sTime)
	{
				
		for (i=0; i<MAX_HUD_SPARKLES; i++)
		{
			xPos = arcadeHud.sX + ((1+sinf(i*0.3+actFrameCount *0.05))*arcadeHud.sW)/2;
			arcadeHud.sparkles[i]->xPosTo = arcadeHud.sparkles[i]->xPos = xPos;
		

			if (arcadeHud.sparkles[i]->a>gameSpeed*3)
				arcadeHud.sparkles[i]->a -= gameSpeed*3;
			else
			{
				SPRITEOVERLAY *me;
				yPos = arcadeHud.sY + (rand()*arcadeHud.sH)/RAND_MAX;
				me = arcadeHud.sparkles[i];
				me->a = (rand()*255)/RAND_MAX;
				me->width = 4+(rand() * 6) / RAND_MAX;
				me->height = me->width;
				me->yPosTo = me->yPos = yPos + 5 - me->height;			
				
				me->draw = 1;
			}		
		}
	}
	else
		for (i=0; i<MAX_HUD_SPARKLES; i++)
		{
			xPos = arcadeHud.sX + ((1+sinf(i*0.3+actFrameCount *0.05))*arcadeHud.sW)/2;
			arcadeHud.sparkles[i]->xPosTo = arcadeHud.sparkles[i]->xPos = xPos;
		
			if (arcadeHud.sparkles[i]->a>gameSpeed*3)
				arcadeHud.sparkles[i]->a -= gameSpeed*3;
			else
				arcadeHud.sparkles[i]->draw = 0;
		}

}



/*
void UpDateOnScreenInfo ( void )
{
	int i;
	static char tickTock = 0;
	static long lastCount = 0;

//	if( player[0].worldNum == WORLDID_SUPERRETRO ) // credits are lives in super retro
//	{
//		sprintf(livesText,"*%lu",player[0].numCredits);	
//		sprintf(scoreText,"%lu",player[0].score);
//	}
//	else
//	{
		sprintf(livesText,"%lu",player[0].lives);	
		sprintf(scoreText,"%lu",player[0].score);
		sprintf(garibText,"%lu",player[0].numSpawn);	
		sprintf(creditText,"%lu",player[0].numCredits);
//	}

	if(scoreTextOver->a && !(gameState.mode==LEVELCOMPLETE_MODE && modeTimer.time) )
	{
		scoreTextOver->a -= 8;
		if(scoreTextOver->a < 8)
			scoreTextOver->a = 0;
	}

	if(countdownTimer)
	{
		GTUpdate( &scoreTimer, -1 );

		if( !scoreTimer.time )
		{
			countdownTimer = 0;
//			PlaySample(GEN_TIME_OUT,0,0,0);
			sprintf(timeText,"NO BONUS !");
			if (gameState.multi==SINGLEPLAYER)
				timeTextOver->draw = 50;
		}

		if( (scoreTimer.time < 11) && !(scoreTimer.time%2) )
		{
			if( tickTock )
			{
//				PlaySample(GEN_CLOCK_TOCK,0,0,0);
				tickTock = 0;
			}
			else
			{
//				PlaySample(GEN_CLOCK_TICK,0,0,0);
				tickTock = 1;
			}
		}
								   
		if( scoreTimer.time ) sprintf(timeText,"%02lu",scoreTimer.time);
	}
	else
	{
		if(timeTextOver->draw)
			timeTextOver->draw-=gameSpeed;
		timeTextOver->a -= 16*gameSpeed;
		timeTextOver->a &= 255;
	}

	i = 3;
	while(i--)
	{
		if ( frog[0] )
		{
			if(player[0].healthPoints <= i)
				sprHeart[i]->a = 64;
			else
				sprHeart[i]->a = 255;
		}
	}
}

*/
/*	--------------------------------------------------------------------------------
	Function		: UpdateScore
	Purpose			: updates the current score
	Parameters		: ACTOR2 *,long
	Returns			: void
	Info			: 
*/
void UpdateScore(ACTOR2 *act,long scoreUpdate)
{
	if (gameState.multi == SINGLEPLAYER)
	{
		player[0].score += scoreUpdate;
//		scoreTextOver->a = 255;
	}
}


/*	--------------------------------------------------------------------------------
	Function		: InitInGameTextOverlays
	Purpose			: creates and initialises in game text overlay messages
	Parameters		: unsigned long,unsigned long
	Returns			: void
	Info			: 
*/
char levelCompleteText[32];
char gameOverText[32];

extern char levelString[];
extern char posString[];


void InitInGameTextOverlays(unsigned long worldID,unsigned long levelID)
{
//	pauseTitle		= CreateAndAddTextOverlay ( 50, 70, "pause", YES, NO, 255, 255, 255, 255, currFont, 0, 0, 0 );
	continueText	= CreateAndAddTextOverlay ( 50, 90, "continue", YES, 255, currFont, 0,0 );
	restartText		= CreateAndAddTextOverlay ( 50, 110, "restart level", YES, 255, currFont, 0,0 );
	quitText		= CreateAndAddTextOverlay ( 50, 130, "quit", YES, 255, currFont, 0,0 );

	posText			= CreateAndAddTextOverlay ( 50, 40, posString, YES, 255, smallFont, 0, 0);
	levelnameText	= CreateAndAddTextOverlay ( 50, 50, levelString, YES, 255, smallFont, 0, 0);

	//	DisableTextOverlay ( pauseTitle );
	DisableTextOverlay ( continueText );
	DisableTextOverlay ( restartText );
	DisableTextOverlay ( quitText );
	DisableTextOverlay ( posText );
	DisableTextOverlay ( levelnameText );

	/* Lives and score */

	InitHUD();
//	livesTextOver	= CreateAndAddTextOverlay(50,205,livesText,NO,255,currFont,0,0);
//	scoreTextOver	= CreateAndAddTextOverlay(230,20,scoreText,NO,255,currFont,0,0);
//	timeTextOver	= CreateAndAddTextOverlay(25,20,timeText,NO,255,currFont,0,0);

#ifdef TOYFAIR_BUILD
	build = CreateAndAddTextOverlay(40,230,"Toy Fair Demo Build      In Development",NO,255,smallFont,0,0);
	build->r			= 55;
	build->g			= 235;
	build->b			= 55;
	build->a			= 210;
#endif

	/* Garib and Credit counters */

	garibCount = CreateAndAddTextOverlay ( 100, 205, garibText, NO, 255, currFont, 0, 0 );
	creditCount = CreateAndAddTextOverlay ( 150, 205, creditText, NO, 255, currFont, 0, 0 );

	/* Baby display */

	babySavedText = CreateAndAddTextOverlay(0,110,"BABY SAVED",YES,255,currFont,TEXTOVERLAY_WAVECHARS,4);
	DisableTextOverlay(babySavedText);
}


