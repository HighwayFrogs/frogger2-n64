/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: sndview.c
	Programmer	: James Healey
	Date		: 21/04/98

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

//----------------------------------------------------------------------------------------------//
//----- SYSTEM INCLUDES ------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------//
#include <ultra64.h>

//----------------------------------------------------------------------------------------------//
//----- USER INCLUDES --------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------//

#include "incs.h"

#ifdef PC_VERSION

void RunSndView() { }

#else

char *sfxNames[] =
{
	"FX NULL",
	"FX SHIRLEYSWIM FINS",
	"FX SHIRLEY LAND",
	"FX SHIRLEY LAUNCH",
	"FX MALLET WALK_LEFT",
	"FX MALLET WALK_RIGHT",
	"FX MALLET STAMPEDE",
	"FX MALLET WAIT",
	"FX GERERALWU WALK_LEFT",
	"FX GERERALWU WALK_RIGHT",
	"FX GENERALWU DIVESNAP",
	"FX GENERALWU HITGROUND",
	"FX SELWYNJOFF ATTACK",
	"FX SELWYNJOFF LAND",
	"FX SELWYNJOFF FLAP L",
	"FX SELWYNJOFF FLAP R",
	"FX SELWYNCANCER ATTACK",
	"FX SELWYNKIRK FLY",
	"FX DENNIS WALK",
	"FX DENNIS LAUNCH",
	"FX DENNIS LAND",
	"FX CHUCK CALL",
	"FX CHUCK ATTACK",
	"FX CHUCK JUGGLE",
	"FX CHUCK WALK",
	"FX BUGLE CALL",
	"FX BUGLE ATTACK",
	"FX BUGLE DEATH",
	"FX KLOSET CALL",
	"FX KLOSET WALK_LEFT",
	"FX KLOSET WALK_RIGHT",
	"FX KLOSET ATTACK",
	"FX KLOSET SPINFLOOR",
	"FX KLOSET BOXINGGLOVE",
	"FX KLOSET BOMB",
	"UNKNOWN",
	"FX CRUMPET ATTACK",
	"FX CRUMPET CALL",
	"FX TRACY DANCING",
	"FX TRACY CALL",
	"FX TRACY WALK_LEFT",
	"FX TRACY WALK_RIGHT",
	"FX TRACY LAND",
	"FX TRACY CHARGE",
	"FX TREX CALL",
	"FX TREX ATTACK",
	"FX TREX FOOTSTEP_LEFT",
	"FX TREX FOOTSTEP_RIGHT",
	"FX FLANNEL WINGFLAP",
	"FX FLANNEL CALL",
	"FX THRICE ATTACK",
	"FX THRICE IDLE",
	"FX MIKE_WALK LEFT",
	"FX MIKE_WALK RIGHT",
	"FX MIKE ATTACK",
	"FX FUMBLE CALL",
	"FX FUMBLE WALK",
	"FX ETHEL CALL",
	"FX ETHEL WIND",
	"FX ETHEL ATTACK",
	"FX FRANKIE ATTACKLOOPED",
	"FX FRANKIE ATTACK",
	"FX FRANKIE ATTACK2",
	"FX FRANKIE GENERALHUM",
	"FX FRANKIE WALK_LEFT",
	"FX FRANKIE WALK_RIGHT",
	"FX FRANKIE CALL",
	"FX FRANKIE DEATH",
	"FX REGGIE WALK",
	"FX REGGIE DEATH",
	"FX SWISH WALKLEFT",
	"FX SWISH WALKRIGHT",
	"FX SWISH_ATTACK",
	"FX KEG CALL",
	"FX KEG WALK",
	"FX CHESTER ATTACK",
	"FX CHESTER WALK",
	"FX CHESTER CALL",
	"FX SPANK CALL1",
	"FX SPANK CALL2",
	"FX SPANK SWING",
	"FX SPANK ATTACK",
	"FX SPANK DROP",
	"FX YOOFOW FLY",
	"FX YOOFOW ATTACK",
	"FX OPEC IDLE",
	"FX OPEC ATTACKLOOP",
	"FX OPEC ATTACKSINGLE",
	"FX CYMON GENERAL",
	"FX CYMON WALKLEFT",
	"FX CYMON WALKRIGHT",
	"FX SUCKER ATTACKLOOP",
	"FX SUCKER ATTACK",
	"FX SUCKER SPIT",
	"FX SUCKER CALL1",
	"FX SUCKER CALL2",
	"FX SUCKER CALL3",
	"FX SUCKER CRYSTAL",
	"FX ROBOT WALK",
	"FX ROBOT ATTACK",
	"FX ROBOT GENERAL",
	"FX BOVVA GENERAL",
	"FX BOVVA ATTACK",
	"FX CANNONBALL WALK",
	"FX SAMTEX WALK",
	"FX SAMTEX FUSE",
	"FX SAMTEX EXPLODE",
	"FX CHICKEN CLUCKNORMAL",
	"FX CHICKEN CLUCKHALFMAD",
	"FX CHICKEN CLUCKMAD",
	"FX CHICKEN FART",
	"FX CHICKEN BELCH",
	"FX SCUTTLE GENERIC1",
	"FX SCUTTLE GENERIC2",
	"FX SCUTTLE GENERIC3",
	"FX SPANK CALL3",
	"FX SPANK CALL4",
	"FX SPANK OOF",
	"FX CHESTER GROWL",
	"FX ROBOT GENERAL2",
	"FX ROBOT GENERAL3",
	"FX ROBOT GENERAL4",
	"FX ROBOT GENERAL5",
	"FX GENERALWU PULLUP",
	"FX CHUCK JUGGLE ENDING",
	"FX TRACEY JUMP",
	"FX TREX PANTING LOOP",
	"FX FUMBLE SPOTBALL",
	"FX FUMBLE KEEPBALANCE",
	"FX FUMBLE PANTING LOOP", 
	"FX KLOSET PIANOFALL",
	"FX BALLBOUNCE RUBBER",
	"FX BALLBOUNCE BOWLING",  
	"FX BALLBOUNCE POWER", 
	"FX BALLBOUNCE BEARING",   
	"FX BALLBOUNCE BEACH",
	"FX BALLBOUNCE SNOW",   
	"FX BALLBOUNCE CRYSTAL",
	"FX BALLBOUNCE CRYSTAL SMASH",
	"FX BALL BURST",
	"FX COLLECT BONUS 1",
	"FX COLLECT BONUS  ECHO",
	"FX COLLECT BONUS 1",
	"FX COLLECT POWERUP 1",
	"FX COLLECT POWERUP 2",
	"FX COLLECT POWERUP 3",
	"FX COLLECT POWERUP 4", 
	"FX COLLECT POWERUP 5",
	"FX HAND FOOTSTEP LEFT",
	"FX HAND FOOTSTEP RIGHT",
	"FX HAND SKID",
	"FX HAND SKID LOOP",
	"FX HAND JUMP SMALL",
	"FX HAND JUMP BIG",
	"FX HAND LAND",
	"FX HAND HURT 1",
	"FX HAND HURT 2",
	"FX HAND HURT 3",
	"FX HAND PUSH 1",
	"FX HAND PUSH 2",
	"FX HAND FALL",
	"FX HAND DIE 1",
	"FX HAND DIE 2",
	"FX HAND FINGERTAP 1",
	"FX HAND FINGERTAP 2",
	"FX HAND FINGERCLICK",
	"FX HAND FISTSLAM",
	"FX HAND THROW 1",
	"FX HAND THROW 2",
	"FX HAND THROW 3",
	"FX HAND THROW  LOOP",
	"FX HAND WHACKBALL",
	"FX HAND BOUNCETAP 1",
	"FX HAND BOUNCETAP 2",
	"FX CAST SPELL 1",
	"FX CAST SPELL 2",
	"FX CAST SPELL 3",
	"FX POINT DIRECTION 1",
	"FX POINT DIRECTION 2",
	"FX GENERIC BLEEP 1",
	"FX GENERIC BLEEP 2",
	"FX GENERIC BLEEP 3",
	"FX GENERIC BLEEP 4",
	"FX ROLLING SNOWBALL",

	"FX IMPACT STONE SMALL",
	"FX IMPACT STONE MEDIUM",
	"FX IMPACT STONE LARGE",
	"FX IMPACT WOOD SMALL", 
	"FX IMPACT WOOD MEDIUM",  
	"FX IMPACT WOOD LARGE",    
	"FX IMPACT METAL SMALL",   
	"FX IMPACT METAL MEDIUM",  
	"FX IMPACT METAL LARGE",   
	"FX IMPACT SAND SMALL",    
	"FX IMPACT SAND MEDIUM",   
	"FX IMPACT SAND LARGE",    
	"FX IMPACT SNOW SMALL",    
	"FX IMPACT SNOW MEDIUM",   
	"FX IMPACT SNOW LARGE",    
	"FX IMPACT MUD SMALL",     
	"FX IMPACT MUD MEDIUM",    
	"FX IMPACT MUD LARGE",     
	"FX STONE SCAPE LARGE",    
	"FX STONE SCAPE SMALL",    
	"FX METAL SCRAPE LARGE",   
	"FX METAL SCRAPE SMALL",   
	"FX WOOD SCRAPE LARGE",    
	"FX WOOD SCRAPE SMALL",    
	"FX ICE SCRAPE LARGE",     
	"FX ICE SCRAPE SMALL",     
	"FX SWITCH",              
	"FX SWITCH",              
	"FX SWITCH",              
	"FX SWITCH",              
	"FX SWITCH",              
	"FX SWITCH",              
	"FX SWITCH",              
	"FX SWITCH",              
	"FX FROG CALL",            
	"FX TROUSERS FALLDOWN",    
	"FX IMPACT WATER SMALL",   
	"FX IMPACT WATER MEDIUM",  
	"FX IMPACT WATER LARGE",   
	"FX EERIE WIND",           
	"FX BIRD TWEET",          
	"FX BIRD TWEET",         
	"FX BIRD TWEET",          
	"FX BIRD TWEET",          
	"FX BIRD TWEET",          
	"FX BIRD TWEET",          
	"FX WATERFALL",            
	"FX WATERFALL STEREO",     
	"FX LAVA",                
	"FX LAVA",                
	"FX BAT WINGFLAP",         
	"FX BAT CALL",             
	"FX BAT CALL ECHO",        
	"FX DRIPING WATER",        
	"FX FOUNTAIN",            
	"FX FOUNTAIN",            
	"FX FOUNTAIN STEREO",      
	"FX SEA WAVES",           
	"FX SEA WAVES",          
	"FX SEA OFF EDGE",         
	"FX HEAVY SWITCH",         
	"FX BADGLOVE LAUGH",      
	"FX RAIN",                 
	"FX RAIN HEAVY",           
	"FX THUNDER",              
	"FX HAND JUMP",          
	"FX HAND JUMP",          
	"FX HAND JUMP",          
	"FX HAND JUMP",          
	"FX HAND RUNJUMP",       
	"FX HAND RUNJUMP",       
	"FX HAND RUNJUMP",       
	"FX HAND RUNJUMP",       
	"FX HAND PUSH",          
	"FX HAND PUSH",          
	"FX HAND PUSH",          
	"FX HAND PUSH",          
	"FX HAND HURT",          
	"FX HAND HURT",          
	"FX HAND HURT",          
	"FX HAND HURT",          
	"FX HAND SPELL",         
	"FX HAND SPELL",         
	"FX HAND SPELL",         
	"FX HAND SPELL",         
	"FX HAND VICTORY",       
	"FX HAND VICTORY",       
	"FX HAND VICTORY",       
	"FX HAND VICTORY",       
	"FX HAND DIE",           
	"FX HAND DIE",           
	"FX HAND DIE",           
	"FX HAND FALL",           
	"FX BUBBLES",             
	"FX BUBBLES",             
	"FX BUBBLES",             
	"FX COLLECT BONUS",       
	"FX COLLECT BONUS",       
	"FX COLLECT BONUS",       
	"FX CAST SPELL",          
	"FX CAST SPELL",          
	"FX HAND WOW",            
	"FX BIGSPELL",             
	"FX INTRO BUBBLES",        
	"FX INTRO THUNDER",        
	"FX INTRO HANDOFF",        
	"FX INTRO SPLASH",         
	"FX INTRO HANDOUTWINDOW",  
	"FX INTRO FLOORBREAK",     
	"FX INTRO WIZARDFALL",     
	"FX INTRO RUMBLE",         
	"FX INTRO EXPLOSION",      
	"FX INTRO WIND",           
	"FX INTRO CHANGETOBALL",   
	"FX INTRO BALLBOUNCE",     
	"FX INTRO HANDHITGROUND",  
	"FX INTRO HANDOUCH",       
	"FX INTRO HANDGETUP",      
	"FX INTRO HANDYEAH",       
	"FX INTRO BADGLOVELAUGH",  
	"FX PORTCULLIS",          
	"FX PORTCULLIS",          
	"FX WATER LAPPING",       
	"FX WATER LAPPING",       
	"FX SPLASH SMALL",        
	"FX SPLASH SMALL",        
	"FX SPLASH SMALL",        
	"FX SPLASH SMALL",        
	"FX ROTORBLADE",           
	"FX SUCKER",               
	"FX ROCK SMASH",         
	"FX ROCK SMASH",          
	"FX ROCK SMASH",          
	"FX TELEPORT",            
	"FX TELEPORT",            
	"FX ENTRO WIND",           
	"FX ENTRO WHOOSH",         
	"FX ENTRO STONELAND",      
	"FX ENTRO FSTEP",          
	"FX ENTRO BIRD",          
	"FX ENTRO BIRD",          
	"FX ENTRO WHAA",           
	"FX ENTRO WHOA",           
	"FX ENTRO BGFALL",         
	"FX ENTRO BGLAND",         
	"FX ENTRO JUMP",           
	"FX ENTRO HANDATTACH",     
	"FX ENTRO BGRUN",          
	"FX ENTRO CASTSPELL",      
	"FX ENTRO SPELLHIT",       
	"FX ENTRO TRANSFORM",      
	"FX ENTRO CHANGE",         
	"FX ENTRO BLOW",           
	"FX ENTRO WIZWALK",        
	"FX ENGINE FAIL",          
	"FX ENGINE START",         
	"FX ENGINE RUNNING",       
	"FX ENGINE FAIL",         
	"FX JETPACK"
};

#define NUM_SNDVIEW_SFX		343	//181

TEXTOVERLAY *sfxName,*musName;

unsigned long sfxNum	= 0;
unsigned long musNum	= 0;
unsigned long sfx		= 1;
unsigned long mus		= 0;
int sfxRes				= 0;


/*	--------------------------------------------------------------------------------
	Function		: RunSndView
	Purpose			: runs the sound viewer
	Parameters		: 
	Returns			: void
	Info			: 
*/
void RunSndView()
{
	static TEXTOVERLAY *title,*titleShadow,*titleSfx,*titleMus;
	static u16 button,lastbutton;
	static s16 stickX,stickY;
	static SPRITEOVERLAY *curPane;
	SPRITEOVERLAY *sprPane;
	char sfxNameTxt[256];
	char musNameTxt[256];
	int j;
	
	if(frameCount == 1)
	{
		float x,y,z;
		int i;

		FreeMenuItems();
		LoadTextureBank(SYSTEM_TEX_BANK);
//		LoadTextureBank(INGAMEGENERIC_TEX_BANK);

		title = CreateAndAddTextOverlay(30,24,"sound player",NO,255,smallFont,0,0);
		titleShadow = CreateAndAddTextOverlay(32,26,"sound player",NO,255,smallFont,0,0);
		titleShadow->r = 0;	titleShadow->g = 0;	titleShadow->b = 0;

		// bottom pane --------------------------------------------------------------------------
		CreateAndAddTextOverlay(75,125,"select first or last item",NO,255,oldeFont,0,0);
		title = CreateAndAddTextOverlay(77,127,"select first or last item",NO,255,oldeFont,0,0);
		title->r = 0;	title->g = 0;	title->b = 0;

		CreateAndAddTextOverlay(75,140,"play selected item",NO,255,oldeFont,0,0);
		title = CreateAndAddTextOverlay(77,142,"play selected item",NO,255,oldeFont,0,0);
		title->r = 0;	title->g = 0;	title->b = 0;

		CreateAndAddTextOverlay(75,155,"stop playback",NO,255,oldeFont,0,0);
		title = CreateAndAddTextOverlay(77,157,"stop playback",NO,255,oldeFont,0,0);
		title->r = 0;	title->g = 0;	title->b = 0;
		// bottom pane --------------------------------------------------------------------------

		// middle pane --------------------------------------------------------------------------
		titleSfx = CreateAndAddTextOverlay(40,55,"SNDFX NAMES",NO,255,oldeFont,0,0);
		titleSfx->g = 255;
		sfxName = CreateAndAddTextOverlay(45,65,"",NO,255,oldeFont,0,0);
		sprintf(sfxNameTxt,"%s %d",sfxNames[sfxNum],sfxNum);
		sfxName->text = sfxNameTxt;

		titleMus = CreateAndAddTextOverlay(40,85,"MUSIC TRACK",NO,95,oldeFont,0,0);
		titleMus->g = 255;
		musName = CreateAndAddTextOverlay(45,95,"",NO,95,oldeFont,0,0);
		sprintf(musNameTxt,"%s %d",gameSongs[musNum].tuneName,musNum);
		musName->text = musNameTxt;
		// middle pane --------------------------------------------------------------------------

		// bottom pane --------------------------------------------------------------------------
		CreateAndAddSpriteOverlay(30,120,"shldr_l.bmp",16,16,255,0);
		CreateAndAddSpriteOverlay(47,120,"shldr_r.bmp",16,16,255,0);

		CreateAndAddSpriteOverlay(34,135,"button_a.bmp",16,16,255,0);
		CreateAndAddSpriteOverlay(34,150,"button_b.bmp",16,16,255,0);
		// bottom pane --------------------------------------------------------------------------

		curPane = CreateAndAddSpriteOverlay(43,63,"tippane.bmp",200,12,191,0);

		// top pane
//		sprPane = CreateAndAddSpriteOverlay(25,20,"tippane.bmp",270,25,191,0);
		sprPane = CreateAndAddSpriteOverlay(25,20,"prc_watrd.bmp",270,25,191,0);
		// middle pane
//		sprPane = CreateAndAddSpriteOverlay(25,46,"tippane.bmp",270,68,95,0);
		sprPane = CreateAndAddSpriteOverlay(25,46,"prc_watrd.bmp",270,68,95,0);
		// bottom pane
//		sprPane = CreateAndAddSpriteOverlay(25,115,"tippane.bmp",270,105,191,0);
		sprPane = CreateAndAddSpriteOverlay(25,115,"prc_watrd.bmp",270,105,191,0);
		
		CreateAndAddProceduralTexture(sprPane->frames[0],"prc_watrd");

		sfxNum	= 0;
		musNum	= 0;
		sfxRes	= 0;
	}

	// read controller
	button = controllerdata[ActiveController].button;
	stickX = controllerdata[ActiveController].stick_x;
	stickY = controllerdata[ActiveController].stick_y;

	if((button & CONT_UP) && !(lastbutton & CONT_UP))
	{
		sfx = 1;		// select sound effects
		mus = 0;

		MusHandleStop(sfxRes,0);
		MusHandleStop(audioCtrl.musicHandle[0],0);
		audioCtrl.currentTrack[0] = 0;

		curPane->xPos = 43;	curPane->yPos = 63;
	}
	if((button & CONT_DOWN) && !(lastbutton & CONT_DOWN))
	{
		sfx = 0;
		mus = 1;		// select music tracks

		MusHandleStop(sfxRes,0);
		MusHandleStop(audioCtrl.musicHandle[0],0);
		audioCtrl.currentTrack[0] = 0;

		curPane->xPos = 43;	curPane->yPos = 93;
	}

	if((button & CONT_LEFT) && !(lastbutton & CONT_LEFT))
	{
		// select required sound effect
		if(sfx)
		{
			if(sfxNum > 0)
			{
				MusHandleStop(sfxRes,0);
				sfxNum--;
			}

			sprintf(sfxNameTxt,"%s %d",sfxNames[sfxNum],sfxNum);
			sfxName->text = sfxNameTxt;
		}
		// select required music track
		else
		{
			if(musNum > 0)
			{
				MusHandleStop(sfxRes,0);
				audioCtrl.currentTrack[0] = 0;
				musNum--;
			}

			sprintf(musNameTxt,"%s %d",gameSongs[musNum].tuneName,musNum);
			musName->text = musNameTxt;
		}
	}

	if((button & CONT_RIGHT) && !(lastbutton & CONT_RIGHT))
	{
		// select required sound effect
		if(sfx)
		{
			if(sfxNum < (NUM_SNDVIEW_SFX - 1))
			{
				MusHandleStop(sfxRes,0);
				sfxNum++;
			}

			sprintf(sfxNameTxt,"%s %d",sfxNames[sfxNum],sfxNum);
			sfxName->text = sfxNameTxt;
		}
		// select required music track
		else
		{
			if(musNum < (NUM_TRACKS - 1))
			{
				MusHandleStop(sfxRes,0);
				audioCtrl.currentTrack[0] = 0;
				musNum++;
			}

			sprintf(musNameTxt,"%s %d",gameSongs[musNum].tuneName,musNum);
			musName->text = gameSongs[musNum].tuneName;
		}
	}

	if((button & CONT_A) && !(lastbutton & CONT_A))
	{
		// play currently selected sample
		if(sfx)
		{
			MusHandleStop(audioCtrl.musicHandle[0],0);
			sfxRes = PlaySample(sfxNum,NULL,128,255,128);
		}
		// play currently selected music track
		else
		{
			MusHandleStop(sfxRes,0);
			PrepareSong(musNum,0);
		}
	}

	if((button & CONT_B) && !(lastbutton & CONT_B))
	{
		// stop current sound effect
		if(sfx)
		{
			MusHandleStop(sfxRes,0);
		}
		// stop current music track
		else
		{
			MusHandleStop(audioCtrl.musicHandle[0],0);
			audioCtrl.currentTrack[0] = 0;
		}
	}

	if((button & CONT_L) && !(lastbutton & CONT_L))
	{
		// set to start of sound effect list
		if(sfx)
		{
			MusHandleStop(sfxRes,0);
			sfxNum = 0;
		}
		// set to start of music track list
		else
		{
			MusHandleStop(audioCtrl.musicHandle[0],0);
			audioCtrl.currentTrack[0] = 0;
			musNum = 0;
		}

		sprintf(musNameTxt,"%s %d",gameSongs[musNum].tuneName,musNum);
		musName->text = musNameTxt;
		sprintf(sfxNameTxt,"%s %d",sfxNames[sfxNum],sfxNum);
		sfxName->text = sfxNameTxt;
	}

	if((button & CONT_R) && !(lastbutton & CONT_R))
	{
		// set to end of sound effect list
		if(sfx)
		{
			MusHandleStop(sfxRes,0);
			sfxNum = (NUM_SNDVIEW_SFX - 1);
		}
		// set to start of music track list
		else
		{
			MusHandleStop(audioCtrl.musicHandle[0],0);
			audioCtrl.currentTrack[0] = 0;
			musNum = (NUM_TRACKS - 1);
		}

		sprintf(musNameTxt,"%s %d",gameSongs[musNum].tuneName,musNum);
		musName->text = musNameTxt;
		sprintf(sfxNameTxt,"%s %d",sfxNames[sfxNum],sfxNum);
		sfxName->text = sfxNameTxt;
	}

	if((button & CONT_G) && !(lastbutton & CONT_G))
	{
		MusHandleStop(sfxRes,0);
		MusHandleStop(audioCtrl.musicHandle[0],0);
		audioCtrl.currentTrack[0] = 0;

		FreeAllLists();
		developmentMode = 0;
		gameState.mode = MENU_MODE;
		gameState.menuMode = TITLE_MODE;

		frameCount = 0;
		lastbutton = 0;
		return;
	}

	lastbutton = button;

	if(sfx)
	{
		sfxName->a	= 255;
		titleSfx->a = 255;
		musName->a	= 95;
		titleMus->a = 95;
	}
	else if(mus)
	{
		sfxName->a	= 95;
		titleSfx->a = 95;
		musName->a	= 255;
		titleMus->a = 255;
	}

	curPane->a = 255 * Fabs(sinf(frameCount/12.5));
}

#endif
