/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: game.c
	Programmer	: Matthew Cloy
	Date		: 12/11/98

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#define SHOW_ME_THE_TILE_NUMBERS

#include <ultra64.h>
#include "incs.h"


//----------------------------------------------------------------------

#ifdef SHOW_ME_THE_TILE_NUMBERS
TEXTOVERLAY *tileNum = NULL;
unsigned long currTileNum = 0;
#endif
//TEXTOVERLAY *faceNum = NULL;

//----------------------------------------------------------------------

unsigned long INPUT_POLLPAUSE	= 10;

struct gameStateStruct gameState;

unsigned short screenNum = 0;
unsigned long fadingLogos = 0;

GAMETILE *firstTile;
GAMETILE **gTStart;

long hopAmt = 10;
long idleCamera = 0;

unsigned long autoPlaying = 0;
unsigned long recordKeying = 0;
char tileString[16];
char faceString[16];

long award = 2;
short showEndLevelScreen = 1;

long bby = 0;
long babySaved = 0;

short spawnCounter = 0;
long displayingTile=0;

TIMER gameIsOver;
TIMER levelIsOver;
TIMER scoreTimer;

void UpdateCompletedLevel(unsigned long worldID,unsigned long levelID);
void DoHiscores();
extern float gCamDist;
extern TEXTURE *frogEyeOpen,*frogEyeClosed;

// Switch for irritating swingy camera
unsigned char swingCam = 1;


/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: GameProcessController

	Purpose		:
	Parameters	: (void)
	Returns		: void 
*/
void GameProcessController(long pl)
{
	static u16 button[4],lastbutton[4];
	static s16 stickX[4], stickY[4],lastStickX[4],lastStickY[4];
    
	button[pl] = controllerdata[pl].button;
	stickX[pl] = controllerdata[pl].stick_x;
	stickY[pl] = controllerdata[pl].stick_y;
		
	player[pl].hasJumped = 0;
	
	if ( autoPlaying )
	{
		LevelPlayingProcessController();
		button[pl] = 0;
		stickX[pl] = 0;
		stickY[pl] = 0;
		// find frame keys
		button[pl] = GetCurrentRecordKey();
	}

	// check if frog is using extended hop ability
	if(player[pl].isSuperHopping)
		player[pl].canJump = 0;

	if(player[pl].hasDoubleJumped)
	{
		if ((button[pl] & CONT_A) && (player[pl].jumpTime > 0.5f*player[pl].jumpMultiplier))
		{
			AnimateActor(frog[pl]->actor, FROG_ANIM_FLOATFALL, NO, NO, 0.5f, NO, NO);
			player[pl].frogState |= FROGSTATUS_ISFLOATING;
		}
		else
		{
			if (player[pl].frogState & FROGSTATUS_ISFLOATING)
				AnimateActor(frog[pl]->actor, FROG_ANIM_GETUPFROMFLOAT, NO, NO, 0.5f, NO, NO);
			player[pl].frogState &= ~FROGSTATUS_ISFLOATING;
		}
	}

	if((button[pl] & CONT_UP) && !(lastbutton[pl] & CONT_UP) && player[pl].canJump)
	{
		if(!player[pl].inputPause )
		{
			player[pl].inputPause = INPUT_POLLPAUSE;
			UpdateScore(frog[pl],hopAmt);

			player[pl].canJump = 0;
			player[pl].hasJumped = 1;
			player[pl].frogState |= FROGSTATUS_ISWANTINGU;

			// update player idleTime
			player[pl].idleTime = MAX_IDLE_TIME;
			idleCamera = 0;

			if ( recordKeying )
				AddPlayingActionToList ( MOVEMENT_UP, frameCount );
			player[pl].extendedHopDir = MOVE_UP;
		}
	}	    

	else if((button[pl] & CONT_RIGHT) && !(lastbutton[pl] & CONT_RIGHT) && player[pl].canJump)
	{
		if(!player[pl].inputPause)
		{
			player[pl].inputPause = INPUT_POLLPAUSE;
			UpdateScore(frog[pl],hopAmt);

			player[pl].canJump = 0;
			player[pl].hasJumped = 1;
			player[pl].frogState |= FROGSTATUS_ISWANTINGR;

			// update player idleTime
			player[pl].idleTime = MAX_IDLE_TIME;
			idleCamera = 0;

			if ( recordKeying )
				AddPlayingActionToList ( MOVEMENT_RIGHT, frameCount );
			player[pl].extendedHopDir = MOVE_RIGHT;
		}
	}
    
	else if((button[pl] & CONT_DOWN) && !(lastbutton[pl] & CONT_DOWN) && player[pl].canJump)
	{
		if(!player[pl].inputPause)
		{
			player[pl].inputPause = INPUT_POLLPAUSE;
			UpdateScore(frog[pl],hopAmt);
	    	
			player[pl].canJump = 0;
			player[pl].hasJumped = 1;
			player[pl].frogState |= FROGSTATUS_ISWANTINGD;

			// update player idleTime
			player[pl].idleTime = MAX_IDLE_TIME;
			idleCamera = 0;

			if ( recordKeying )
				AddPlayingActionToList ( MOVEMENT_DOWN, frameCount );
			player[pl].extendedHopDir = MOVE_DOWN;
		}
	}
    
	else if((button[pl] & CONT_LEFT) && !(lastbutton[pl] & CONT_LEFT) && (player[pl].canJump))
	{
		if(!player[pl].inputPause)
		{
			player[pl].inputPause = INPUT_POLLPAUSE;
			UpdateScore(frog[pl],hopAmt);

			player[pl].canJump = 0;
			player[pl].hasJumped = 1;
			player[pl].frogState |= FROGSTATUS_ISWANTINGL;

			// update player idleTime
			player[pl].idleTime = MAX_IDLE_TIME;
			idleCamera = 0;
			
			if ( recordKeying )
				AddPlayingActionToList ( MOVEMENT_LEFT, frameCount );
			player[pl].extendedHopDir = MOVE_LEFT;
		}
	}

	if((button[pl] & CONT_A) && !(lastbutton[pl] & CONT_A))
    {
		if ( gameState.mode == CAMEO_MODE )
			gameState.mode = INGAME_MODE;

		// update player idleTime
		player[pl].idleTime = MAX_IDLE_TIME;
		idleCamera = 0;

		if( ((player[pl].isSuperHopping) && (player[pl].heightJumped > -125.0F)) && !(player[pl].hasDoubleJumped) )
		{
			int dir = player[pl].extendedHopDir;

			if( button[pl] & CONT_UP )
				dir = MOVE_UP;
			else if( button[pl] & CONT_DOWN )
				dir = MOVE_DOWN;
			else if( button[pl] & CONT_LEFT )
				dir = MOVE_LEFT;
			else if( button[pl] & CONT_RIGHT )
				dir = MOVE_RIGHT;

			// player is superhopping - make frog double jump
			player[pl].hasDoubleJumped = 1;
			player[pl].canJump = 0;
			currTile[pl] = destTile[pl];
			frogFacing[pl] = nextFrogFacing[pl];
			camFacing = nextCamFacing;

			MoveToRequestedDestination( dir, pl );
			nextFrogFacing[pl] = frogFacing[pl] = (dir+camFacing) &3;
			player[pl].extendedHopDir = dir;

			AnimateActor(frog[pl]->actor,FROG_ANIM_FORWARDSOMERSAULT,NO,NO,0.35F,0,0);
			AnimateActor(frog[pl]->actor,FROG_ANIM_BREATHE,YES,YES,0.75F,0,0);
			// To enable endless double jumping
			//player[pl].hasDoubleJumped = 0;
#ifdef N64_VERSION
			StartRumble(100,1,3,ActiveController);
#endif
		}
		else if(!(player[pl].isSuperHopping) && !(player[pl].inputPause))
		{
			// Check if the player can jump.. if not, check if we're just hopping
			// What a horrible hack
			if (player[pl].canJump ||
			   (player[pl].frogState & (FROGSTATUS_ISJUMPINGTOTILE | FROGSTATUS_ISJUMPINGTOPLATFORM)))
			{
				// frog is wanting superhop
				player[pl].isSuperHopping = 1;
				player[pl].hasJumped = 1;
				player[pl].hasDoubleJumped = 0;

				player[pl].inputPause = INPUT_POLLPAUSE;
				UpdateScore(frog[pl],hopAmt);

				switch(player[pl].extendedHopDir)
				{
					case MOVE_UP:
						player[pl].frogState |= FROGSTATUS_ISWANTINGSUPERHOPU;
						break;
					case MOVE_LEFT:
						player[pl].frogState |= FROGSTATUS_ISWANTINGSUPERHOPL;
						break;
					case MOVE_DOWN:
						player[pl].frogState |= FROGSTATUS_ISWANTINGSUPERHOPD;
						break;
					case MOVE_RIGHT:
						player[pl].frogState |= FROGSTATUS_ISWANTINGSUPERHOPR;
						break;
				}
			}
		}
	}
	
	if((button[pl] & CONT_B) && !(lastbutton[pl] & CONT_B) && (tongue[pl].flags & TONGUE_IDLE))
    {
		// want to use tongue
		tongue[pl].flags = TONGUE_NONE | TONGUE_SEARCHING;
		player[pl].hasJumped = 1;

		// update player idleTime
		player[pl].idleTime = MAX_IDLE_TIME;
		idleCamera = 0;
	}

	if((button[0] & CONT_E) && !(lastbutton[0] & CONT_E))
    {
		StartDrawing("start");
	}

	if((button[0] & CONT_D) && !(lastbutton[0] & CONT_D))
    {
		// toggle between zoom in or out camera view
//		ChangeCameraSetting();
		StopDrawing("end");
	}

	if( !fixedPos && !fixedDir )
	{
		if((button[0] & CONT_F) && !(lastbutton[0] & CONT_F))
		{
			camFacing--;
			camFacing &= 3;
			if ( recordKeying )
				AddPlayingActionToList ( CAMERA_LEFT, frameCount );
		}

		if((button[pl] & CONT_C) && !(lastbutton[pl] & CONT_C))
		{
			camFacing++;
			camFacing &= 3;		
			if ( recordKeying )
				AddPlayingActionToList ( CAMERA_RIGHT, frameCount );
		}
	}

	// Croak and Croak Float
	if(!(player[pl].frogState & FROGSTATUS_ISDEAD) && !(frog[pl]->action.isCroaking.time))
	{
		if((button[pl] & CONT_L) && !(lastbutton[pl] & CONT_L))
		{
			GTInit( &frog[pl]->action.isCroaking, 2 );
			player[pl].frogState |= FROGSTATUS_ISCROAKING;
//			PlayActorBasedSample(218,frog[pl]->actor,255,128);

			// update player idleTime
			player[pl].idleTime = MAX_IDLE_TIME;
			idleCamera = 0;
		}
	}

	if((button[pl] & CONT_R) && !(lastbutton[pl] & CONT_R))
	{
		if(numBabies)
		{
			bby++;
			bby %= numBabies;
			if(bTStart[bby])
				SetFroggerStartPos(bTStart[bby],pl);
		}
    }

	if((button[pl] & CONT_START) && !(lastbutton[pl] & CONT_START))
	{
		long i;
		//ScreenShot();
		
		gameState.mode = PAUSE_MODE;
		pauseMode = 1;
	
//		grabData.afterEffect = NO_EFFECT;

//		EnableTextOverlay ( pauseTitle );
		EnableTextOverlay ( continueText );
		EnableTextOverlay ( quitText );

		if (NUM_FROGS == 1)
		{
			livesTextOver->oa = livesTextOver->a;
			scoreTextOver->oa = scoreTextOver->a;

			livesTextOver->a = 0;
			scoreTextOver->a = 0;
		}

		timeTextOver->oa = timeTextOver->a;
		timeTextOver->a = 0;

		if (NUM_FROGS == 1)
		{
		
			for ( i = 0; i < 3; i++ )
				sprHeart[i]->draw = 0;
		}

		for(i=0; i<numBabies; i++)
			babyIcons[i]->draw = 0;

		lastbutton[pl] = button[pl];
    }

	lastbutton[pl] = button[pl];
	lastStickX[pl] = stickX[pl];
	lastStickY[pl] = stickY[pl];
}


/*	--------------------------------------------------------------------------------
	Function		: CreateLevelObjects
	Purpose			: creates the objects for the world / level
	Parameters		: 
	Returns			: void
	Info			: 
*/
extern ACTOR2 *backGnd;


void CreateLevelObjects(unsigned long worldID,unsigned long levelID)
{
	ACTOR2 *theActor;
	SCENIC *ts = Sc_000;
	int actCount = 0;
	char tmp[5];
	long i;
	
	// go through and add items
	while (ts)
	{
		float tv;

		stringChange(ts->name);

		if(gstrcmp(ts->name,"backdrop.obe") == 0)
		{
			backGnd = CreateAndAddActor (ts->name,ts->pos.v[0],ts->pos.v[2],ts->pos.v[1],INIT_ANIMATION,0,0);
			backGnd->actor->scale.v[0] = 5;
			backGnd->actor->scale.v[1] = 5;
			backGnd->actor->scale.v[2] = 5;
			actList = actList->next;
			if (actList)
				actList->prev = NULL;
		}
		else
		{
			theActor = CreateAndAddActor (ts->name,ts->pos.v[0],ts->pos.v[2],ts->pos.v[1],INIT_ANIMATION,0,0);
			dprintf"Added actor '%s'\n",ts->name));

			if(gstrcmp(ts->name,"world.obe") == 0)
			{
				theActor->flags = ACTOR_DRAW_ALWAYS;
				globalLevelActor = theActor;
			}

			for( i=0; i<4; i++ )
				tmp[i] = ts->name[i];
			tmp[4] = '\0';
			// If a water object, draw always
			if( !gstrcmp( tmp, "wat_\0" ) )
			{
				theActor->flags = ACTOR_WATER | ACTOR_DRAW_ALWAYS;

				if (ts->name[4]=='f')
					theActor->flags |= ACTOR_SLIDYTEX;

#ifdef N64_VERSION
				AddN64WaterObjectResource(theActor->actor);
#endif
			}

			tv = ts->rot.y;
			ts->rot.y = ts->rot.z;
			ts->rot.z = tv;

			GetQuaternionFromRotation (&theActor->actor->qRot,&ts->rot);

			AnimateActor(theActor->actor,0,YES,NO,0.35,0,0);
			if(ts->name[0] == 'a')
			{
				float rMin,rMax,rNum;
				if(ts->name[2] == '_')
				{
					rMin = ts->name[1] - 30;
					if(rMin = 0) 
						rMin = 10;
					rMin /= 10.0;
					AnimateActor(theActor->actor,0,YES,NO,rMin, 0, 0);
				}
				else if(ts->name[3] == '_')
				{
					rMin = ts->name[1] - 30;
					if(rMin == 0) 
						rMin = 10;
					rMin /= 10.0;
				
					rMax = ts->name[1] - 30;
					if (rMax == 0) 
						rMax = 10;
					rMax /= 10.0;

					rNum = Random(1000);
					rNum= rMin + ((rNum * (rMax - rMin)) / 1000);
										
					AnimateActor(theActor->actor,0,YES,NO,rNum, 0, 0);
				}
			}
			actCount++;
		}

		ts = ts->next;
	}

	dprintf"\n\n** ADDED %d ACTORS **\n\n",actCount));	
}


/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: RunGameLoop 

	Purpose		:
	Parameters	: (void)
	Returns		: void 
*/

extern float camSideOfs;
long playMode = NORMAL_PMODE;
long carryOnBabies = 1;
long clearTiles = 0;

SPRITEOVERLAY *atari,*konami,*flogo[10];


//////////////////////////////////////////
void Modify3DText(TEXT3D *t3d, char *str,unsigned char alpha);

TEXT3D *cText[4];
TEXT3D *rText[4];

unsigned char *credits[] = 
{
	"","Programmers",		"Matthew Cloy",		"",
	"","\x1",				"Andy Eder",		"",
	"","\x1",				"James Hubbard",	"",
	"","\x1",				"David Swift",		"",
	"","\x1",				"James Healey",		"",


	"","Artists",			"Alex Rigby","",
	"","\x1",				"Joff Scarcliffe","", 
	"","\x1",				"Bruce Millar","",
	"","\x1",				"Lauren Grindrod","",
	"","\x1",				"Simon Little","",
	"","\x1",				"Sandro Da Cruz","",
	"","\x1",				"Richard Whale","",
	"","\x1",				"Mark Turner","",
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

///////////////////////////////////////////////////////

void RunGameLoop (void)
{
	unsigned long i,j;
	GAMETILE *cur;
	
	// setup for frogger point of interest
	pOIDistance = 50000.0;
	pointOfInterest = NULL;
		
	if(frameCount == 1)
	{	
		if ( worldVisualData [ player[0].worldNum ].levelVisualData [ player[0].levelNum ].multiPartLevel == NO_MULTI_LEV )
		{
			player[0].lives				= 3;
			player[0].score				= 0;
			player[0].spawnTimer		= 0;
			player[0].spawnScoreLevel	= 1;
			GTInit( &scoreTimer, 90 );
		}

		GTInit( &gameIsOver, 0 );
		GTInit( &levelIsOver, 0 );
		
		if (player[0].worldNum == WORLDID_FRONTEND)
		{
			if (player[0].levelNum == LEVELID_FRONTEND1)
			{
				InitCredits();			
				creditsActive = 3;
				DeactivateCredits();
			
				DisableTextOverlay(scoreTextOver);
#ifdef PC_VERSION
				frogFacing[0] = 3;
				atari = CreateAndAddSpriteOverlay(270,195,"atari.bmp",32,32,255,0);
				konami = CreateAndAddSpriteOverlay(18,195,"konami.bmp",32,32,255,0);
				i = 0;
				flogo[i++] = CreateAndAddSpriteOverlay(98,136,"flogo01.bmp",32,32,255,0);
				flogo[i++] = CreateAndAddSpriteOverlay(130,136,"flogo02.bmp",32,32,255,0);
				flogo[i++] = CreateAndAddSpriteOverlay(162,136,"flogo03.bmp",32,32,255,0);
				flogo[i++] = CreateAndAddSpriteOverlay(194,136,"flogo04.bmp",32,32,255,0);
				flogo[i++] = CreateAndAddSpriteOverlay(98,168,"flogo05.bmp",32,32,255,0);
				flogo[i++] = CreateAndAddSpriteOverlay(130,168,"flogo06.bmp",32,32,255,0);
				flogo[i++] = CreateAndAddSpriteOverlay(162,168,"flogo07.bmp",32,32,255,0);
				flogo[i++] = CreateAndAddSpriteOverlay(194,168,"flogo08.bmp",32,32,255,0);
				flogo[i++] = CreateAndAddSpriteOverlay(162,200,"flogo09.bmp",32,32,255,0);
				flogo[i++] = CreateAndAddSpriteOverlay(194,200,"flogo10.bmp",32,32,255,0);
#endif
				fadingLogos = 0;
			}
		}

		bronzeCup[0] = CreateAndAddSpriteOverlay(230,20,"bronz001.bmp",32,32,255,0);
		bronzeCup[1] = CreateAndAddSpriteOverlay(262,20,"bronz002.bmp",32,32,255,0);
		bronzeCup[2] = CreateAndAddSpriteOverlay(230,52,"bronz003.bmp",32,32,255,0);
		bronzeCup[3] = CreateAndAddSpriteOverlay(262,52,"bronz004.bmp",32,32,255,0);
		bronzeCup[4] = CreateAndAddSpriteOverlay(20,20,"bronz001.bmp",32,32,255,0);
		bronzeCup[5] = CreateAndAddSpriteOverlay(52,20,"bronz002.bmp",32,32,255,0);
		bronzeCup[6] = CreateAndAddSpriteOverlay(20,52,"bronz003.bmp",32,32,255,0);
		bronzeCup[7] = CreateAndAddSpriteOverlay(52,52,"bronz004.bmp",32,32,255,0);
		for(i=0; i<8; i++)
			DisableSpriteOverlay(bronzeCup[i]);
		
		silverCup[0] = CreateAndAddSpriteOverlay(230,20,"silv001.bmp",32,32,255,0);
		silverCup[1] = CreateAndAddSpriteOverlay(262,20,"silv002.bmp",32,32,255,0);
		silverCup[2] = CreateAndAddSpriteOverlay(230,52,"silv003.bmp",32,32,255,0);
		silverCup[3] = CreateAndAddSpriteOverlay(262,52,"silv004.bmp",32,32,255,0);
		silverCup[4] = CreateAndAddSpriteOverlay(20,20,"silv001.bmp",32,32,255,0);
		silverCup[5] = CreateAndAddSpriteOverlay(52,20,"silv002.bmp",32,32,255,0);
		silverCup[6] = CreateAndAddSpriteOverlay(20,52,"silv003.bmp",32,32,255,0);
		silverCup[7] = CreateAndAddSpriteOverlay(52,52,"silv004.bmp",32,32,255,0);
		for(i=0; i<8; i++)
			DisableSpriteOverlay(silverCup[i]);

		goldCup[0] = CreateAndAddSpriteOverlay(230,20,"gold001.bmp",32,32,255,0);
		goldCup[1] = CreateAndAddSpriteOverlay(262,20,"gold002.bmp",32,32,255,0);
		goldCup[2] = CreateAndAddSpriteOverlay(230,52,"gold003.bmp",32,32,255,0);
		goldCup[3] = CreateAndAddSpriteOverlay(262,52,"gold004.bmp",32,32,255,0);
		goldCup[4] = CreateAndAddSpriteOverlay(20,20,"gold001.bmp",32,32,255,0);
		goldCup[5] = CreateAndAddSpriteOverlay(52,20,"gold002.bmp",32,32,255,0);
		goldCup[6] = CreateAndAddSpriteOverlay(20,52,"gold003.bmp",32,32,255,0);
		goldCup[7] = CreateAndAddSpriteOverlay(52,52,"gold004.bmp",32,32,255,0);
		for(i=0; i<8; i++)
			DisableSpriteOverlay(goldCup[i]);

		if (player[0].worldNum==9)
		{
		//	CreateOverlays();
			timeTextOver->draw = 0;
			livesTextOver->draw = 0;
			countdownTimer = 0;
			i = 3;
			while(i--)
				sprHeart[i]->draw = 0;			
		}

#ifdef SHOW_ME_THE_TILE_NUMBERS
		tileNum = CreateAndAddTextOverlay(0,35,tileString,YES,255,bigFont,0,0);
#endif
//		faceNum = CreateAndAddTextOverlay(0,35,faceString,YES,255,smallFont,0,0);

		CheckForDynamicCameraChange(currTile[0]); // TEMPORARY FIX!!

		lastActFrameCount = 0;
	}
	// FINISH FIRST FRAME STUFF
	if (player[0].worldNum == WORLDID_FRONTEND)
			if (player[0].levelNum == LEVELID_FRONTEND1)
			{
				if ((currTile[0] == &(firstTile[22])))
				{
					if (creditsActive==0)
						ActivateCredits();
				}
				else
					DeactivateCredits();
				
				RunCredits();
			}
	
	if	((player[0].worldNum == WORLDID_FRONTEND) &&
	     (player[0].levelNum == LEVELID_FRONTEND1) )
	{
		if( fadingLogos )
		{
#ifdef PC_VERSION
			if (atari->xPos < 500)
			{
				atari->xPos+=2*gameSpeed;
				konami->xPos-=2*gameSpeed;
				for (i=0; i<10; i++)
					flogo[i]->yPos+=3*gameSpeed;
			}
#endif
		}
		else
		{
			if (player[0].hasJumped == 1)
				fadingLogos = 1;
		}
	}

	if(gameIsOver.time)
	{
		if (NUM_FROGS == 1)
		{
			DisableTextOverlay(livesTextOver);
			DisableTextOverlay(scoreTextOver);
		}
//				livesIcon->active = 0;
		DisableTextOverlay(timeTextOver);

		RunGameOverSequence();
		GTUpdate( &gameIsOver, -1 );
		if(!gameIsOver.time)
		{
			StopDrawing("game over");
			FreeAllLists();

			player[0].levelNum = LEVELID_FRONTEND1;
			player[0].worldNum = WORLDID_FRONTEND;
			player[0].frogState &= ~FROGSTATUS_ISDEAD;
			InitLevel(player[0].worldNum,player[0].levelNum);

			frameCount = 0;
			StartDrawing("game over");
			return;
		}
	}
	else if ( levelIsOver.time )
	{
		if( showEndLevelScreen )
		{
			RunLevelCompleteSequence();

			if(!levelComplete1->draw)
			{
				darkenedLevel = 0;
				pauseMode = 1;
				EnableTextOverlay(levelComplete1);
				EnableTextOverlay(levelComplete2);
				scoreTextOver->a = 255;
				EnableTextOverlay(scoreTextOver);
			}

			levelComplete1->a -= (levelComplete1->a - 255) / 20.0F;
			levelComplete2->a -= (levelComplete2->a - 255) / 20.0F;			
		}

		GTUpdate( &levelIsOver, -1 );

		if(!levelIsOver.time)
		{
			DoHiscores( );

			// Only go to next level if in normal level progression.
			if( showEndLevelScreen )
			{
				//			gameState.mode		= FRONTEND_MODE;
				//frontEndState.mode	= DEMO_MODE;

				player[0].worldNum = WORLDID_FRONTEND;
				player[0].levelNum = LEVELID_FRONTEND2;
				//player[0].levelNum++;

#ifndef PC_VERSION
				StoreSaveSlot(0, 0); // Write data for Player 0 into Slot 0
				SaveGame(); // Write save games into eeprom
#else
//				SaveGameData();
#endif
			}

			FreeAllLists();
			frameCount = 0;

			player[0].numSpawn	= 0;
			spawnCounter = 0;

			worldVisualData[player[0].worldNum].levelVisualData[player[0].levelNum].levelOpen |= LEVEL_OPEN;
			InitLevel(player[0].worldNum,player[0].levelNum);

			showEndLevelScreen = 1; // Normal level progression is default
			return;
		}
	}
	else
	{
		if(keyFound)
			RunLevelKeyFound();
//		if( babySaved )
//			RunBabySavedSequence(lastBabySaved);

		if ( babiesSaved==numBabies && numBabies && (worldVisualData[player[0].worldNum].levelVisualData[player[0].levelNum ].multiPartLevel == NO_MULTI_LEV) )
		{
			camDist.v[X]	= 0;
			camDist.v[Y]	= 680;
			camDist.v[Z]	= 192;
			UpdateCompletedLevel(player[0].worldNum,player[0].levelNum);
			GTInit( &levelIsOver, 15 );
		}
		else
		{
			if( frameCount > 15 )
			{
				for (i=0; i<NUM_FROGS; i++)
					if( !frog[i]->action.dead.time && !(player[i].frogState & FROGSTATUS_ISDEAD) )
						if ( frog[i]->action.stun.time)
						{
							GTUpdate( &frog[i]->action.stun, -1 );
						}
						else
						{
							GameProcessController(i);                                      
						}
			}

			CheckForDynamicCameraChange(currTile[0]); // TEMPORARY FIX!!

			CameraLookAtFrog();
			UpdateCameraPosition(0);

			for (i=0; i<NUM_FROGS; i++)
				if (frog[i]) SitAndFace(frog[i],currTile[i],frogFacing[i]);
		}
	}

	UpdatePlatforms();
	UpdateEnemies();
	UpdateSpecialEffects();
	UpdateEvents();
	UpdateAmbientSounds();

	ProcessCollectables();

	for( i=0; i<NUM_FROGS; i++ )
	{
		if( frog[i] )
		{
			UpdateFroggerPos(i);

			if( !(player[i].frogState & FROGSTATUS_ISDEAD) )
			{
				frog[i]->draw = 1;
				if (frog[i]->action.safe.time)
				{
					frog[i]->actor->xluOverride = 50;
					
					GTUpdate( &frog[i]->action.safe, -1 );
					if( !frog[i]->action.safe.time )
						frog[i]->actor->xluOverride = 100;
				}
			}

			if( player[i].idleEnable )
			{
				player[i].idleTime-=gameSpeed;
				if(player[i].idleTime<1)
				{
					unsigned long iAnim = Random(4);
					switch (iAnim)
					{
						case 0:
							AnimateActor(frog[i]->actor,FROG_ANIM_SCRATCHHEAD,NO,NO,0.4F,0,0);
							if (Random(10)>6)
								AnimateActor(frog[i]->actor,FROG_ANIM_SCRATCHHEAD,NO,YES,0.4F,0,0);
							AnimateActor(frog[i]->actor,FROG_ANIM_BREATHE,YES,YES,0.4F,0,0);
							break;
						case 1:
							AnimateActor(frog[i]->actor,FROG_ANIM_DANCE1,YES,NO,0.3F,0,0);
							break;
						case 2:
							AnimateActor(frog[i]->actor,FROG_ANIM_DANCE2,YES,NO,0.3F,0,0);
							break;
						case 3:
							AnimateActor(frog[i]->actor,FROG_ANIM_DANCE3,NO,NO,0.3F,0,0);
							if (Random(10)>6)
								AnimateActor(frog[i]->actor,FROG_ANIM_DANCE1,YES,YES,0.3F,0,0);
							else
								AnimateActor(frog[i]->actor,FROG_ANIM_BREATHE,YES,YES,0.4F,0,0);
							break;
						case 4:
							AnimateActor(frog[i]->actor,FROG_ANIM_BREATHE,YES,YES,0.4F,0,0);
							break;
					}

					player[i].idleTime = 400 + Random(300);
				}
			}

		}
	}  

	if( gameState.multi == SINGLEPLAYER )
		UpDateOnScreenInfo();
	else
	{
		int res;
		switch( player[0].worldNum )
		{
		case WORLDID_GARDEN:
			res = UpdateRace( );
			break;
		case WORLDID_SUBTERRANEAN:
		case WORLDID_HALLOWEEN:
			res = UpdateCTF( );
			break;
		}

		if( !res ) return;
	}

	// Send network update packet
#ifdef PC_VERSION
	if( gameState.multi == MULTIREMOTE )
		if( controllerdata[0].button == 0 || (controllerdata[0].button != controllerdata[0].lastbutton) )
			SendUpdateMessage( );
#endif

#ifdef SHOW_ME_THE_TILE_NUMBERS
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
	
//	if (faceNum)
//		if (faceNum->text)
//			sprintf(faceNum->text,"%d",camFacing);

	if (tileNum)
		if (tileNum->text)
		{
			if (displayingTile)
				sprintf(tileNum->text,"%d",currTileNum);
			else
				sprintf(tileNum->text,"",currTileNum);
		}
#endif
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RunLevelCompleteSequence()
{
	long i;

	DisableTextOverlay(livesTextOver);
	DisableTextOverlay(timeTextOver);
	//DisableTextOverlay(scoreTextOver);
	DisableTextOverlay(babySavedText);

	if(carryOnBabies)
	{					
		if(numHops_TOTAL < 110)
				award = 1;
		if(numHops_TOTAL < 105)
			award = 0;
	}

	clock->draw = 1;
	spawn->draw = 1;
	
	spawnCollected->draw = 1;
	time->draw = 1;

	nextLev1->draw = 1;
	nextLev2->draw = 1;

	if (NUM_FROGS == 1)
	{
		for ( i = 0; i < 3; i++ )
			sprHeart[i]->draw = 0;
	}

	scoreTextOver->xPos -= ((float)scoreTextOver->xPos - (100.0F)) / 15.0F;
	scoreTextOver->yPos -= ((float)scoreTextOver->yPos - (110.0F)) / 16.0F;
	
	if ( spawnCounter != player[0].numSpawn )
	{
		spawnCounter++;
	}

	sprintf ( spawnCollected->text, "%d / 150", spawnCounter );

	sprintf ( timeTemp, "%i secs", 90-scoreTimer.time );

	i = numBabies;
	while(i--)
	{
		babyIcons[i]->xPos -= ((float)babyIcons[i]->xPos - ((20.0F*i)+115.0F)) / 16.0F;
		babyIcons[i]->yPos -= ((float)babyIcons[i]->yPos - (65.0F)) / 16.0F;
		babyIcons[i]->animSpeed = 1.5F;
	}
							
	switch(award)
	{
		case 0:
			for(i=0; i<8; i++)
				EnableSpriteOverlay(goldCup[i]);
			break;
		case 1:
			for(i=0; i<8; i++)
				EnableSpriteOverlay(silverCup[i]);
			break;
		case 2:
			for(i=0; i<8; i++)
				EnableSpriteOverlay(bronzeCup[i]);
			break;
	}
}

void DoHiscores( )
{
/*	HISCORE *hs = &worldHiScoreData[player[0].worldNum][player[0].levelNum];
	long i, place = MAX_HISCORE_SLOTS;
	
	// TEST
	player[0].score = 9999999;

	// Level hiscores
	if( player[0].score > hs->score )
	{
		hs->score = player[0].score;
		hs->name[0] = player[0].name[0];
		hs->name[1] = player[0].name[1];
		hs->name[2] = player[0].name[2];
		hs->time = scoreTimer.time;
		hs->cup = award;
	}

	// Overall hiscores
	for( i=MAX_HISCORE_SLOTS-1; i >= 0; i-- )
	{
		if( hiScoreData[i].score < player[0].score )
			place = i;
	}

	if( place != MAX_HISCORE_SLOTS )
	{
		// Sort hiscore table to make room for new entry
		for( i=MAX_HISCORE_SLOTS-1; i>place; i-- )
			cmemcpy( (char *)&hiScoreData[i], (char *)&hiScoreData[i-1], sizeof(HISCORE) );

		hiScoreData[place].score = player[0].score;
		hiScoreData[place].name[0] = player[0].name[0];
		hiScoreData[place].name[1] = player[0].name[1];
		hiScoreData[place].name[2] = player[0].name[2];
		hiScoreData[place].time = scoreTimer.time;
		hiScoreData[place].cup = award;
	}*/

}
