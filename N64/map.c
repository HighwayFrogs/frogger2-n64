/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: map.c
	Programmer	: Matthew Cloy
	Date		: 04/12/98

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>

#include "incs.h"


char *mapBank;
char *levBank;

// global ptrs used to free allocated resources
char *aMapBank				= NULL;
char *aLevBank				= NULL;
char *globalPtrEntityBank	= NULL;
char *globalPtrScriptBank	= NULL;

unsigned long numSafe = 1;
unsigned long numPwrups = 3;

GAMETILE **bTSafe;
GAMETILE **pwrupTStart;

SCENIC *Sc_000;


/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: LoadMapBank

	Purpose		:
	Parameters	: 
	Returns		: void 
*/
void LoadMapBank(short num)
{
	LoadCollision(num);
	LoadScenics(num);
}

void FreeMapBank()
{
	if(aMapBank)
		JallocFree((UBYTE**)&aMapBank);
	if(aLevBank)
		JallocFree((UBYTE**)&aLevBank);
}

/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: LoadCollision 
	Purpose		:
	Parameters	: short
	Returns		: void 
*/

void LoadCollision(short cNum)
{
	int					tnum = -1;
	char				*objectBank;
	u32					bankRomStart, bankRomEnd, bankSize;
	short				y, x;
	GAMETILE			*cgT;
	char				message[64];

	switch(cNum)
	{
		// FRONTEND ------------------------------------------------------------------------------

		case FRONTEND1_COL:
			tnum = START_COLL_BANK;
			sprintf(message,"FRONTEND 1");
			break;
		case FRONTEND2_COL:
			tnum = LEVELA_COLL_BANK;
			sprintf(message,"FRONTEND 2");
			break;
		case FRONTEND3_COL:
			tnum = -1;
			sprintf(message,"FRONTEND 3");
			break;
		case FRONTEND4_COL:
			tnum = -1;
			sprintf(message,"FRONTEND 4");
			break;
		case FRONTEND5_COL:
			tnum = LANGUAGE_COLL_BANK;
			sprintf(message,"FRONTEND 5");
			break;

		// GARDEN --------------------------------------------------------------------------------

		case GARDENMASTERA_COL:
			tnum = -1;	//GARDENBOSSA_COLL_BANK;
			sprintf(message,"GARDEN BOSS A");
			break;
		case GARDENMASTERB_COL:
			tnum = -1;	//GARDENBOSSB_COLL_BANK;
			sprintf(message,"GARDEN BOSS B");
			break;
		case GARDENLEV1_COL:
			tnum = GARDEN1_COLL_BANK;
			sprintf(message,"GARDEN 1");
			break;
		case GARDENLEV2_COL:
			tnum = GARDEN2_COLL_BANK;
			sprintf(message,"GARDEN 2");
			break;
		case GARDENLEV3_COL:
			tnum = -1;	//GARDEN3_COLL_BANK;
			sprintf(message,"GARDEN 3");
			break;
		case GARDENBONUS_COL:
			tnum = GARDENBONUS_COLL_BANK;
			sprintf(message,"GARDEN BONUS");
			break;
		case GARDENMULTI_COL:
			tnum = GARDENMULTI_COLL_BANK;
			sprintf(message,"GARDEN MULTI");
			break;

		// ANCIENTS ------------------------------------------------------------------------------

		case ANCIENTMASTERA_COL:
			tnum = -1;
			sprintf(message,"ANCIENTS BOSS A");
			break;
		case ANCIENTMASTERB_COL:
			tnum = -1;
			sprintf(message,"ANCIENTS BOSS B");
			break;
		case ANCIENTMASTERC_COL:
			tnum = -1;
			sprintf(message,"ANCIENTS BOSS C");
			break;
		case ANCIENTMASTERD_COL:
			tnum = -1;
			sprintf(message,"ANCIENTS BOSS D");
			break;
		case ANCIENTLEV1_COL:
			tnum = ANCIENTS1_COLL_BANK;
			sprintf(message,"ANCIENTS 1");
			break;
		case ANCIENTLEV2_COL:
			tnum = ANCIENTS2_COLL_BANK;
			sprintf(message,"ANCIENTS 2");
			break;
		case ANCIENTLEV3_COL:
			tnum = ANCIENTS3_COLL_BANK;
			sprintf(message,"ANCIENTS 3");
			break;
		case ANCIENTBONUS_COL:
			tnum = -1;
			sprintf(message,"ANCIENTS BONUS");
			break;
		case ANCIENTMULTI_COL:
			tnum = -1;
			sprintf(message,"ANCIENTS MULTI");
			break;

		// SPACE ---------------------------------------------------------------------------------

		case SPACEMASTERA_COL:
			tnum = SPACEBOSS_COLL_BANK;
			sprintf(message,"SPACE BOSS A");
			break;
		case SPACELEV1_COL:
			tnum = SPACE1_COLL_BANK;
			sprintf(message,"SPACE 1");
			break;
		case SPACELEV2_COL:
			tnum = SPACE2_COLL_BANK;
			sprintf(message,"SPACE 2");
			break;
		case SPACELEV3_COL:
			tnum = SPACE3_COLL_BANK;
			sprintf(message,"SPACE 3");
			break;
		case SPACEBONUS_COL:
			tnum = -1;
			sprintf(message,"SPACE BONUS");
			break;
		case SPACEMULTI_COL:
			tnum = -1;
			sprintf(message,"SPACE MULTI");
			break;

		// CITY ----------------------------------------------------------------------------------

		case CITYMASTERA_COL:
			tnum = -1;
			sprintf(message,"CITY BOSS A");
			break;
		case CITYLEV1_COL:
			tnum = CITY1_COLL_BANK;
			sprintf(message,"CITY 1");
			break;
		case CITYLEV2_COL:
			tnum = CITY2_COLL_BANK;
			sprintf(message,"CITY 2");
			break;
		case CITYLEV3_COL:
			tnum = CITY3_COLL_BANK;
			sprintf(message,"CITY 3");
			break;
		case CITYBONUS_COL:
			tnum = CITYBONUS_COLL_BANK;
			sprintf(message,"CITY BONUS");
			break;
		case CITYMULTI_COL:
			tnum = -1;
			sprintf(message,"CITY MULTI");
			break;

		// SUBTERRANEAN --------------------------------------------------------------------------

		case SUBTERRANEANMASTERA_COL:
			tnum = -1;
			sprintf(message,"SUBTERRANEAN BOSS A");
			break;
		case SUBTERRANEANLEV1_COL:
			tnum = -1;
			sprintf(message,"SUBTERRANEAN 1");
			break;
		case SUBTERRANEANLEV2_COL:
			tnum = -1;
			sprintf(message,"SUBTERRANEAN 2");
			break;
		case SUBTERRANEANLEV3_COL:
			tnum = -1;
			sprintf(message,"SUBTERRANEAN 3");
			break;
		case SUBTERRANEANBONUS_COL:
			tnum = -1;
			sprintf(message,"SUBTERRANEAN BONUS");
			break;
		case SUBTERRANEANMULTI_COL:
			tnum = -1;
			sprintf(message,"SUBTERRANEAN BOSS");
			break;

		// LABORATORY ----------------------------------------------------------------------------

		case LABORATORYMASTERA_COL:
			tnum = -1;
			sprintf(message,"LABORATORY BOSS A");
			break;
		case LABORATORYLEV1_COL:
			tnum = LAB1_COLL_BANK;
			sprintf(message,"LABORATORY 1");
			break;
		case LABORATORYLEV2_COL:
			tnum = LAB2_COLL_BANK;
			sprintf(message,"LABORATORY 2");
			break;
		case LABORATORYLEV3_COL:
			tnum = LAB3_COLL_BANK;
			sprintf(message,"LABORATORY 3");
			break;
		case LABORATORYBONUS_COL:
			tnum = -1;
			sprintf(message,"LABORATORY BONUS");
			break;
		case LABORATORYMULTI_COL:
			tnum = -1;
			sprintf(message,"LABORATORY MULTI");
			break;

		// TOYSHOP -------------------------------------------------------------------------------

		case TOYSHOPMASTERA_COL:
			tnum = -1;
			sprintf(message,"TOYSHOP BOSS A");
			break;
		case TOYSHOPLEV1_COL:
			tnum = TOY1_COLL_BANK;
			sprintf(message,"TOYSHOP 1");
			break;
		case TOYSHOPLEV2_COL:
			tnum = -1;
			sprintf(message,"TOYSHOP 2");
			break;
		case TOYSHOPLEV3_COL:
			tnum = -1;
			sprintf(message,"TOYSHOP 3");
			break;
		case TOYSHOPBONUS_COL:
			tnum = -1;
			sprintf(message,"TOYSHOP BONUS");
			break;
		case TOYSHOPMULTI_COL:
			tnum = -1;
			sprintf(message,"TOYSHOP MULTI");
			break;

		// HALLOWEEN -----------------------------------------------------------------------------

		case HALLOWEENMASTERA_COL:
			tnum = HALLOWEENBOSS_COLL_BANK;
			sprintf(message,"HALLOWEEN BOSS A");
			break;
		case HALLOWEENLEV1_COL:
			tnum = HALLOWEEN1_COLL_BANK;
			sprintf(message,"HALLOWEEN 1");
			break;
		case HALLOWEENLEV2_COL:
			tnum = -1;
			sprintf(message,"HALLOWEEN 2");
			break;
		case HALLOWEENLEV3_COL:
			tnum = -1;
			sprintf(message,"HALLOWEEN 3");
			break;
		case HALLOWEENBONUS_COL:
			tnum = -1;
			sprintf(message,"HALLOWEEN BONUS");
			break;
		case HALLOWEENMULTI_COL:
			tnum = -1;
			sprintf(message,"HALLOWEEN MULTI");
			break;

		// SUPER RETRO ---------------------------------------------------------------------------

		case RETROMASTERA_COL:
			tnum = -1;
			sprintf(message,"SUPERRETRO BOSS A");
			break;
		case RETROLEV1_COL:
			tnum = SRETRO1_COLL_BANK;
			sprintf(message,"SUPERRETRO 1");
			break;
		case RETROLEV2_COL:
			tnum = SRETRO2_COLL_BANK;
			sprintf(message,"SUPERRETRO 2");
			break;
		case RETROLEV3_COL:
			tnum = SRETRO3_COLL_BANK;
			sprintf(message,"SUPERRETRO 3");
			break;
		case RETROLEV4_COL:
			tnum = SRETRO4_COLL_BANK;
			sprintf(message,"SUPERRETRO 4");
			break;
		case RETROLEV5_COL:
			tnum = SRETRO5_COLL_BANK;
			sprintf(message,"SUPERRETRO 5");
			break;
		case RETROLEV6_COL:
			tnum = SRETRO6_COLL_BANK;
			sprintf(message,"SUPERRETRO 6");
			break;
/*
		case RETROBONUS_COL:
			tnum = -1;
			sprintf(message,"SUPERRETRO BONUS");
			break;
		case RETROMULTI_COL:
			tnum = -1;
			sprintf(message,"SUPERRETRO MULTI");
			break;
*/
		default:
			tnum = -1;
	}

	if(tnum == -1)
	{
		dprintf"NO COLLISION BANK SPECIFIED FOR %s\n",message));
		return;
	}
	
	bankRomStart	= (u32)collBanksStart[tnum];
	bankRomEnd		= (u32)collBanksEnd[tnum];			
	bankSize = bankRomEnd - bankRomStart;

	objectBank = (char *)JallocAlloc(DMAGetSize(bankRomStart, bankRomEnd), FALSE,"COLLBANK");
	aMapBank = objectBank;
	//start download from rom

	DMAMemory(objectBank, bankRomStart, bankRomEnd);

	if(objectBank)
		dprintf"Loaded collision bank %i (size %d) for %s\n",tnum,(int)bankSize,message));
	else
	{
		dprintf"Unable to load collision bank for %s\n",message));
		return;
	}

	//store a pointer to the object bank so it may be free'd at a later date	
	
	//mapBank = &objectBank[36];
	mapBank = &objectBank[0];

	x = 0;

	// relocate GAMETILE *gTStart[4] = {&gT_141,&gT_015,0,0};

	gTStart = &mapBank[0];
	
	#define LONGVAL(x) (((unsigned long *)mapBank)[x])

	for (y=0; y<4; y++,x++)
		if (LONGVAL(x))
			LONGVAL(x) = Rom2Ram((u32)LONGVAL(x), (u32)objectBank);

	numBabies = LONGVAL(x++);
	
	// relocate GAMETILE *bTStart[4] = {&gT_026,&gT_134,&gT_005};

	bTStart = &LONGVAL(x);
	for (y=0; y<numBabies; y++,x++)
		if (LONGVAL(x))
			LONGVAL(x) = Rom2Ram((u32)LONGVAL(x), (u32)objectBank);

	numSafe = LONGVAL(x++);
	
	// relocate GAMETILE *bTSafe[4] =  {&gT_141,NULL,NULL,NULL};
	bTSafe = &LONGVAL(x);
	for (y=0; y<numSafe; y++,x++)
		if (LONGVAL(x))
			LONGVAL(x) = Rom2Ram((u32)LONGVAL(x), (u32)objectBank);

	numPwrups = LONGVAL(x++);

    // relocate GAMETILE *pwrupTStart[4] = { &gT_163,&gT_039,&gT_037,&gT_047 };
	pwrupTStart = &LONGVAL(x);
	for (y=0; y<numPwrups; y++,x++)
		if (LONGVAL(x))
			LONGVAL(x) = Rom2Ram((u32)LONGVAL(x), (u32)objectBank);

	cgT = &LONGVAL(x);

	firstTile = cgT;

	while (cgT)
	{
		for (y=0; y<4; y++)
			if (cgT->tilePtrs[y])
				cgT->tilePtrs[y] = Rom2Ram((u32)cgT->tilePtrs[y], (u32)objectBank);

		if (cgT->next)
			cgT->next = Rom2Ram((u32)cgT->next, (u32)objectBank);
		cgT = cgT->next;
	}

	
	//Rom2Ram((u32)objCont->object, (u32)objectBank);	
}


/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: LoadScenics 
	Purpose		:
	Parameters	: short
	Returns		: void 
*/

void LoadScenics(short sNum)
{
	int					tnum = -1;
	char				*objectBank;
	u32					bankRomStart, bankRomEnd, bankSize;
	short				y, x;
	SCENIC				*cSc;
	char				message[64];

	switch(sNum)
	{
		// FRONTEND ------------------------------------------------------------------------------

		case FRONTEND1_COL:
			tnum = START_COLL_BANK;
			sprintf(message,"FRONTEND 1");
			break;
		case FRONTEND2_COL:
			tnum = LEVELA_COLL_BANK;
			sprintf(message,"FRONTEND 2");
			break;
		case FRONTEND3_COL:
			tnum = -1;
			sprintf(message,"FRONTEND 3");
			break;
		case FRONTEND4_COL:
			tnum = -1;
			sprintf(message,"FRONTEND 4");
			break;
		case FRONTEND5_COL:
			tnum = LANGUAGE_COLL_BANK;
			sprintf(message,"FRONTEND 5");
			break;

		// GARDEN --------------------------------------------------------------------------------

		case GARDENMASTERA_COL:
			tnum = -1;	//GARDENBOSSA_COLL_BANK;
			sprintf(message,"GARDEN BOSS A");
			break;
		case GARDENMASTERB_COL:
			tnum = -1;	//GARDENBOSSB_COLL_BANK;
			sprintf(message,"GARDEN BOSS B");
			break;
		case GARDENLEV1_COL:
			tnum = GARDEN1_COLL_BANK;
			sprintf(message,"GARDEN 1");
			break;
		case GARDENLEV2_COL:
			tnum = GARDEN2_COLL_BANK;
			sprintf(message,"GARDEN 2");
			break;
		case GARDENLEV3_COL:
			tnum = -1;	//GARDEN3_COLL_BANK;
			sprintf(message,"GARDEN 3");
			break;
		case GARDENBONUS_COL:
			tnum = GARDENBONUS_COLL_BANK;
			sprintf(message,"GARDEN BONUS");
			break;
		case GARDENMULTI_COL:
			tnum = GARDENMULTI_COLL_BANK;
			sprintf(message,"GARDEN MULTI");
			break;

		// ANCIENTS ------------------------------------------------------------------------------

		case ANCIENTMASTERA_COL:
			tnum = -1;
			sprintf(message,"ANCIENTS BOSS A");
			break;
		case ANCIENTMASTERB_COL:
			tnum = -1;
			sprintf(message,"ANCIENTS BOSS B");
			break;
		case ANCIENTMASTERC_COL:
			tnum = -1;
			sprintf(message,"ANCIENTS BOSS C");
			break;
		case ANCIENTMASTERD_COL:
			tnum = -1;
			sprintf(message,"ANCIENTS BOSS D");
			break;
		case ANCIENTLEV1_COL:
			tnum = ANCIENTS1_COLL_BANK;
			sprintf(message,"ANCIENTS 1");
			break;
		case ANCIENTLEV2_COL:
			tnum = ANCIENTS2_COLL_BANK;
			sprintf(message,"ANCIENTS 2");
			break;
		case ANCIENTLEV3_COL:
			tnum = ANCIENTS3_COLL_BANK;
			sprintf(message,"ANCIENTS 3");
			break;
		case ANCIENTBONUS_COL:
			tnum = -1;
			sprintf(message,"ANCIENTS BONUS");
			break;
		case ANCIENTMULTI_COL:
			tnum = -1;
			sprintf(message,"ANCIENTS MULTI");
			break;

		// SPACE ---------------------------------------------------------------------------------

		case SPACEMASTERA_COL:
			tnum = SPACEBOSS_COLL_BANK;
			sprintf(message,"SPACE BOSS A");
			break;
		case SPACELEV1_COL:
			tnum = SPACE1_COLL_BANK;
			sprintf(message,"SPACE 1");
			break;
		case SPACELEV2_COL:
			tnum = SPACE2_COLL_BANK;
			sprintf(message,"SPACE 2");
			break;
		case SPACELEV3_COL:
			tnum = SPACE3_COLL_BANK;
			sprintf(message,"SPACE 3");
			break;
		case SPACEBONUS_COL:
			tnum = -1;
			sprintf(message,"SPACE BONUS");
			break;
		case SPACEMULTI_COL:
			tnum = -1;
			sprintf(message,"SPACE MULTI");
			break;

		// CITY ----------------------------------------------------------------------------------

		case CITYMASTERA_COL:
			tnum = -1;
			sprintf(message,"CITY BOSS A");
			break;
		case CITYLEV1_COL:
			tnum = CITY1_COLL_BANK;
			sprintf(message,"CITY 1");
			break;
		case CITYLEV2_COL:
			tnum = CITY2_COLL_BANK;
			sprintf(message,"CITY 2");
			break;
		case CITYLEV3_COL:
			tnum = CITY3_COLL_BANK;
			sprintf(message,"CITY 3");
			break;
		case CITYBONUS_COL:
			tnum = CITYBONUS_COLL_BANK;
			sprintf(message,"CITY BONUS");
			break;
		case CITYMULTI_COL:
			tnum = -1;
			sprintf(message,"CITY MULTI");
			break;

		// SUBTERRANEAN --------------------------------------------------------------------------

		case SUBTERRANEANMASTERA_COL:
			tnum = -1;
			sprintf(message,"SUBTERRANEAN BOSS A");
			break;
		case SUBTERRANEANLEV1_COL:
			tnum = -1;
			sprintf(message,"SUBTERRANEAN 1");
			break;
		case SUBTERRANEANLEV2_COL:
			tnum = -1;
			sprintf(message,"SUBTERRANEAN 2");
			break;
		case SUBTERRANEANLEV3_COL:
			tnum = -1;
			sprintf(message,"SUBTERRANEAN 3");
			break;
		case SUBTERRANEANBONUS_COL:
			tnum = -1;
			sprintf(message,"SUBTERRANEAN BONUS");
			break;
		case SUBTERRANEANMULTI_COL:
			tnum = -1;
			sprintf(message,"SUBTERRANEAN BOSS");
			break;

		// LABORATORY ----------------------------------------------------------------------------

		case LABORATORYMASTERA_COL:
			tnum = -1;
			sprintf(message,"LABORATORY BOSS A");
			break;
		case LABORATORYLEV1_COL:
			tnum = LAB1_COLL_BANK;
			sprintf(message,"LABORATORY 1");
			break;
		case LABORATORYLEV2_COL:
			tnum = LAB2_COLL_BANK;
			sprintf(message,"LABORATORY 2");
			break;
		case LABORATORYLEV3_COL:
			tnum = LAB3_COLL_BANK;
			sprintf(message,"LABORATORY 3");
			break;
		case LABORATORYBONUS_COL:
			tnum = -1;
			sprintf(message,"LABORATORY BONUS");
			break;
		case LABORATORYMULTI_COL:
			tnum = -1;
			sprintf(message,"LABORATORY MULTI");
			break;

		// TOYSHOP -------------------------------------------------------------------------------

		case TOYSHOPMASTERA_COL:
			tnum = -1;
			sprintf(message,"TOYSHOP BOSS A");
			break;
		case TOYSHOPLEV1_COL:
			tnum = TOY1_COLL_BANK;
			sprintf(message,"TOYSHOP 1");
			break;
		case TOYSHOPLEV2_COL:
			tnum = -1;
			sprintf(message,"TOYSHOP 2");
			break;
		case TOYSHOPLEV3_COL:
			tnum = -1;
			sprintf(message,"TOYSHOP 3");
			break;
		case TOYSHOPBONUS_COL:
			tnum = -1;
			sprintf(message,"TOYSHOP BONUS");
			break;
		case TOYSHOPMULTI_COL:
			tnum = -1;
			sprintf(message,"TOYSHOP MULTI");
			break;

		// HALLOWEEN -----------------------------------------------------------------------------

		case HALLOWEENMASTERA_COL:
			tnum = HALLOWEENBOSS_COLL_BANK;
			sprintf(message,"HALLOWEEN BOSS A");
			break;
		case HALLOWEENLEV1_COL:
			tnum = HALLOWEEN1_COLL_BANK;
			sprintf(message,"HALLOWEEN 1");
			break;
		case HALLOWEENLEV2_COL:
			tnum = -1;
			sprintf(message,"HALLOWEEN 2");
			break;
		case HALLOWEENLEV3_COL:
			tnum = -1;
			sprintf(message,"HALLOWEEN 3");
			break;
		case HALLOWEENBONUS_COL:
			tnum = -1;
			sprintf(message,"HALLOWEEN BONUS");
			break;
		case HALLOWEENMULTI_COL:
			tnum = -1;
			sprintf(message,"HALLOWEEN MULTI");
			break;

		// SUPER RETRO ---------------------------------------------------------------------------

		case RETROMASTERA_COL:
			tnum = -1;
			sprintf(message,"SUPERRETRO BOSS A");
			break;
		case RETROLEV1_COL:
			tnum = SRETRO1_COLL_BANK;
			sprintf(message,"SUPERRETRO 1");
			break;
		case RETROLEV2_COL:
			tnum = SRETRO2_COLL_BANK;
			sprintf(message,"SUPERRETRO 2");
			break;
		case RETROLEV3_COL:
			tnum = SRETRO3_COLL_BANK;
			sprintf(message,"SUPERRETRO 3");
			break;
		case RETROLEV4_COL:
			tnum = SRETRO4_COLL_BANK;
			sprintf(message,"SUPERRETRO 4");
			break;
		case RETROLEV5_COL:
			tnum = SRETRO5_COLL_BANK;
			sprintf(message,"SUPERRETRO 5");
			break;
		case RETROLEV6_COL:
			tnum = SRETRO6_COLL_BANK;
			sprintf(message,"SUPERRETRO 6");
			break;
/*
		case RETROBONUS_COL:
			tnum = -1;
			sprintf(message,"SUPERRETRO BONUS");
			break;
		case RETROMULTI_COL:
			tnum = -1;
			sprintf(message,"SUPERRETRO MULTI");
			break;
*/
		default:
			tnum = -1;
	}

	if(tnum == -1)
	{
		dprintf"NO SCENIC BANK SPECIFIED FOR %s\n",message));
		return;
	}

	bankRomStart	= (u32)scenBanksStart[tnum];
	bankRomEnd		= (u32)scenBanksEnd[tnum];			
	bankSize = bankRomEnd - bankRomStart;

	objectBank = (char *)JallocAlloc(DMAGetSize(bankRomStart, bankRomEnd), FALSE,"SCENBANK");
	aLevBank = objectBank;
	//start download from rom

	DMAMemory(objectBank, bankRomStart, bankRomEnd);

	if(objectBank)
		dprintf"Loaded scenic bank %i (size %d) for %s\n",tnum,(int)bankSize,message));
	else
	{
		dprintf"Unable to load scenic bank %s\n",message));
		return;
	}

	//store a pointer to the object bank so it may be free'd at a later date	
	
	//levBank = &objectBank[36];
	levBank = &objectBank[0];
	
	x = 0;

	Sc_000 = &levBank[0];
	
	#define LONGVAL(x) (((unsigned long *)levBank)[x])
	
	cSc = Sc_000;

	while (cSc)
	{
		if (cSc->next)
			cSc->next = Rom2Ram((u32)cSc->next, (u32)objectBank);
		if (cSc->typedata)
		{
			TYPEDATA *m;
			cSc->typedata = Rom2Ram((u32)cSc->typedata, (u32)objectBank);
			m = cSc->typedata;
			if (m->movementPath)
				m->movementPath = Rom2Ram((u32)m->movementPath, (u32)objectBank);		
		}
		cSc = cSc->next;
	}
}

/*	--------------------------------------------------------------------------------
	Function		: LoadLevelEntities
	Purpose			: Loads in the data for level, inc enemies, platforms etc
	Parameters		: short worldID - world number
				 	  short levelID - level number
	Returns			: void
	Info			: 
*/
void LoadLevelEntities(short worldID,short levelID)
{
	char	*entityDat;
	u32		bankRomStart,bankRomEnd,bankSize;
	char	message[16];

#ifdef NO_ENTITIES
	return;
#endif

	if(worldVisualData[worldID].levelVisualData[levelID].collBank == -1)
	{
		dprintf"NO COLLISION BANK SPECIFIED - NOT LOADING LEVEL ENTITIES\n"));
		return;
	}

	if(worldID == WORLDID_GARDEN)
	{
		switch(levelID)
		{
			case GARDENLEV1_ENT:
					bankRomStart	= (u32)&_levData_1_1_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_1_1_SegmentRomEnd;
					sprintf(message, "GAR_ENT1");				
				break;

			case GARDENLEV2_ENT:
					bankRomStart	= (u32)&_levData_1_2_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_1_2_SegmentRomEnd;
					sprintf(message, "GAR_ENT2");				
				break;

			case GARDENLEV3_ENT:
					bankRomStart	= (u32)&_levData_1_3_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_1_3_SegmentRomEnd;
					sprintf(message, "GAR_ENT3");				
				break;

			case GARDENBOSSA_ENT:
					bankRomStart	= (u32)&_levData_1_4_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_1_4_SegmentRomEnd;
					sprintf(message, "GAR_BOSA");
				break;

			case GARDENBOSSB_ENT:
					bankRomStart	= (u32)&_levData_1_5_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_1_5_SegmentRomEnd;
					sprintf(message, "GAR_BOSB");
				break;

			case GARDENBONUS_ENT:
					bankRomStart	= (u32)&_levData_1_6_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_1_6_SegmentRomEnd;
					sprintf(message, "GAR_BONS");
				break;

			case GARDENMULTI_ENT:
					bankRomStart	= (u32)&_levData_1_7_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_1_7_SegmentRomEnd;
					sprintf(message, "GAR_MULT");
				break;

			default:
				return;
		}
	}
	else if(worldID == WORLDID_ANCIENT)
	{
		switch(levelID)
		{
			case ANCIENTLEV1_ENT:
					bankRomStart	= (u32)&_levData_2_1_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_2_1_SegmentRomEnd;
					sprintf(message, "ANC_ENT1");				
				break;

			case ANCIENTLEV2_ENT:
					bankRomStart	= (u32)&_levData_2_2_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_2_2_SegmentRomEnd;
					sprintf(message, "ANC_ENT2");				
				break;

			case ANCIENTLEV3_ENT:
					bankRomStart	= (u32)&_levData_2_3_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_2_3_SegmentRomEnd;
					sprintf(message, "ANC_ENT3");				
				break;

			case ANCIENTBOSSA_ENT:
					bankRomStart	= (u32)&_levData_2_4_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_2_4_SegmentRomEnd;
					sprintf(message, "ANC_BOSA");
				break;

			case ANCIENTBOSSB_ENT:
					bankRomStart	= (u32)&_levData_2_5_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_2_5_SegmentRomEnd;
					sprintf(message, "ANC_BOSB");
				break;

			case ANCIENTBOSSC_ENT:
					bankRomStart	= (u32)&_levData_2_6_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_2_6_SegmentRomEnd;
					sprintf(message, "ANC_BOSC");
				break;

			case ANCIENTBOSSD_ENT:
					bankRomStart	= (u32)&_levData_2_7_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_2_7_SegmentRomEnd;
					sprintf(message, "ANC_BOSD");
				break;

			case ANCIENTBONUS_ENT:
					bankRomStart	= (u32)&_levData_2_8_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_2_8_SegmentRomEnd;
					sprintf(message, "ANC_BONS");
				break;

			case ANCIENTMULTI_ENT:
					bankRomStart	= (u32)&_levData_2_9_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_2_9_SegmentRomEnd;
					sprintf(message, "ANC_MULT");
				break;

			default:
				return;
		}
	}
	else if(worldID == WORLDID_SPACE)
	{
		switch(levelID)
		{
			case SPACELEV1_ENT:
					bankRomStart	= (u32)&_levData_3_1_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_3_1_SegmentRomEnd;
					sprintf(message, "SPC_ENT1");				
				break;

			case SPACELEV2_ENT:
					bankRomStart	= (u32)&_levData_3_2_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_3_2_SegmentRomEnd;
					sprintf(message, "SPC_ENT2");				
				break;

			case SPACELEV3_ENT:
					bankRomStart	= (u32)&_levData_3_3_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_3_3_SegmentRomEnd;
					sprintf(message, "SPC_ENT3");				
				break;
/*
			case SPACEBOSSA_ENT:
					bankRomStart	= (u32)&_levData_3_4_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_3_4_SegmentRomEnd;
					sprintf(message, "SPC_BOSS");				
				break;
*/
			default:
				return;
		}
	}
	else if(worldID == WORLDID_CITY)
	{
		switch(levelID)
		{
			case CITYLEV1_ENT:
					bankRomStart	= (u32)&_levData_4_1_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_4_1_SegmentRomEnd;
					sprintf(message, "CTY_ENT1");				
				break;

			case CITYLEV2_ENT:
					bankRomStart	= (u32)&_levData_4_2_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_4_2_SegmentRomEnd;
					sprintf(message, "CTY_ENT2");				
				break;

			case CITYLEV3_ENT:
					bankRomStart	= (u32)&_levData_4_3_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_4_3_SegmentRomEnd;
					sprintf(message, "CTY_ENT3");				
				break;

			default:
				return;
		}
	}
	else if(worldID == WORLDID_SUBTERRANEAN)
	{
		switch(levelID)
		{
		}
	}
	else if(worldID == WORLDID_LABORATORY)
	{
		switch(levelID)
		{
			case LABORATORYLEV1_ENT:
					bankRomStart	= (u32)&_levData_6_1_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_6_1_SegmentRomEnd;
					sprintf(message, "LAB_ENT1");				
				break;

			case LABORATORYLEV2_ENT:
					bankRomStart	= (u32)&_levData_6_2_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_6_2_SegmentRomEnd;
					sprintf(message, "LAB_ENT2");				
				break;

			case LABORATORYLEV3_ENT:
					bankRomStart	= (u32)&_levData_6_3_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_6_3_SegmentRomEnd;
					sprintf(message, "LAB_ENT3");				
				break;

			default:
				return;
		}
	}
	else if(worldID == WORLDID_TOYSHOP)
	{
		switch(levelID)
		{
			default:
				return;
		}
	}
	else if(worldID == WORLDID_HALLOWEEN)
	{
		switch(levelID)
		{
			default:
				return;
		}
	}
	else if(worldID == WORLDID_SUPERRETRO)
	{
		switch(levelID)
		{
			case SRETROLEV1_ENT:
					bankRomStart	= (u32)&_levData_9_1_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_9_1_SegmentRomEnd;
					sprintf(message, "SUP_ENT1");				
				break;
			case SRETROLEV2_ENT:
					bankRomStart	= (u32)&_levData_9_2_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_9_2_SegmentRomEnd;
					sprintf(message, "SUP_ENT1");				
				break;
			case SRETROLEV3_ENT:
					bankRomStart	= (u32)&_levData_9_3_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_9_3_SegmentRomEnd;
					sprintf(message, "SUP_ENT1");				
				break;
			case SRETROLEV4_ENT:
					bankRomStart	= (u32)&_levData_9_4_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_9_4_SegmentRomEnd;
					sprintf(message, "SUP_ENT1");				
				break;
			case SRETROLEV5_ENT:
					bankRomStart	= (u32)&_levData_9_5_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_9_5_SegmentRomEnd;
					sprintf(message, "SUP_ENT1");				
				break;
			case SRETROLEV6_ENT:
					bankRomStart	= (u32)&_levData_9_6_SegmentRomStart;
					bankRomEnd		= (u32)&_levData_9_6_SegmentRomEnd;
					sprintf(message, "SUP_ENT1");				
				break;

			default:
				return;
		}
	}
	else if(worldID == WORLDID_FRONTEND)
	{
		switch(levelID)
		{
			case FRONTEND1_ENT:
				bankRomStart	= (u32)&_levData_10_1_SegmentRomStart;
				bankRomEnd		= (u32)&_levData_10_1_SegmentRomEnd;
				sprintf(message, "FRE_ENT1");
				break;

			case FRONTEND2_ENT:
				bankRomStart	= (u32)&_levData_10_2_SegmentRomStart;
				bankRomEnd		= (u32)&_levData_10_2_SegmentRomEnd;
				sprintf(message, "FRE_ENT2");
				break;

			case FRONTEND3_ENT:
			case FRONTEND4_ENT:
				return;

			case FRONTEND5_ENT:
				bankRomStart	= (u32)&_levData_10_5_SegmentRomStart;
				bankRomEnd		= (u32)&_levData_10_5_SegmentRomEnd;
				sprintf(message, "FRE_ENT5");
				break;

			default:
				return;
		}
	}


	bankSize = bankRomEnd - bankRomStart;

	entityDat = (char *)JallocAlloc(DMAGetSize(bankRomStart,bankRomEnd),YES,"ENTBANK");
	globalPtrEntityBank = entityDat;
	
//start download from rom

	DMAMemory ( entityDat, bankRomStart, bankRomEnd );
	if ( entityDat )
	{
		dprintf"Loaded entity data %s (size %d)\n",message,(int)bankSize));
	}
	else
	{
		dprintf"Unable to load entity data %s\n",message));
		return;
	}
	// ENDIF - entityDat

	MemLoadEntities(entityDat,bankSize);
}

/*	--------------------------------------------------------------------------------
	Function		: FreeLevelEntitys
	Purpose			: frees the entity bank associated with the level
	Parameters		: 
	Returns			: void
	Info			: 
*/
void FreeLevelEntitys()
{
	if(globalPtrEntityBank)
		JallocFree((UBYTE**)&globalPtrEntityBank);
}


/*	--------------------------------------------------------------------------------
	Function		: TeleportActorToTile
	Purpose			: teleports the actor to the specified tile
	Parameters		: ACTOR2 *,GAMETILE *
	Returns			: void
	Info			: 
*/
void TeleportActorToTile(ACTOR2 *act,GAMETILE *tile,long pl)
{
	// make the teleport 'to' tile the current tile
	currTile[pl] = tile;
	SetVector(&act->actor->pos,&tile->centre);
	player[pl].frogState |= FROGSTATUS_ISSTANDING;
	player[pl].frogState &= ~FROGSTATUS_ISTELEPORTING;
	player[pl].canJump = 1;
	player[pl].isSuperHopping = 0;
}


/*	--------------------------------------------------------------------------------
	Function		: LoadLevelScript
	Purpose			: loads script for specified level
	Parameters		: int,int
	Returns			: void
	Info			: 
*/
void LoadLevelScript(int worldID,int levelID)
{
	char	*scriptDat;
	u32		bankRomStart,bankRomEnd,bankSize;
	char	message[16];

#ifdef NO_SCRIPTS
	return;
#endif

	if(worldVisualData[worldID].levelVisualData[levelID].collBank == -1)
	{
		dprintf"NO COLLISION BANK SPECIFIED - NOT LOADING SCRIPTING DATA\n"));
		return;
	}

	if(worldID == WORLDID_GARDEN)
	{
		switch(levelID)
		{
/*
			case GARDENLEV1_ENT:
					bankRomStart	= (u32)&_scrData_1_1_SegmentRomStart;
					bankRomEnd		= (u32)&_scrData_1_1_SegmentRomEnd;
					sprintf(message, "GAR_SCR1");				
				break;
*/
			default:
				return;
		}
	}
	else if(worldID == WORLDID_ANCIENT)
	{
		switch(levelID)
		{
/*
			case ANCIENTLEV1_ENT:
					bankRomStart	= (u32)&_scrData_2_1_SegmentRomStart;
					bankRomEnd		= (u32)&_scrData_2_1_SegmentRomEnd;
					sprintf(message, "ANC_SCR1");				
				break;
*/
			default:
				return;
		}
	}
	else if(worldID == WORLDID_SPACE)
	{
		switch(levelID)
		{
			case SPACELEV1_ENT:
					bankRomStart	= (u32)&_scrData_3_1_SegmentRomStart;
					bankRomEnd		= (u32)&_scrData_3_1_SegmentRomEnd;
					sprintf(message, "SPC_SCR1");				
				break;

			case SPACELEV2_ENT:
					bankRomStart	= (u32)&_scrData_3_2_SegmentRomStart;
					bankRomEnd		= (u32)&_scrData_3_2_SegmentRomEnd;
					sprintf(message, "SPC_SCR2");				
				break;
/*
			case SPACELEV3_ENT:
					bankRomStart	= (u32)&_scrData_3_3_SegmentRomStart;
					bankRomEnd		= (u32)&_scrData_3_3_SegmentRomEnd;
					sprintf(message, "SPC_SCR3");				
				break;

			case SPACEBOSSA_ENT:
					bankRomStart	= (u32)&_scrData_3_4_SegmentRomStart;
					bankRomEnd		= (u32)&_scrData_3_4_SegmentRomEnd;
					sprintf(message, "SPC_SCR4");				
				break;
*/
			default:
				return;
		}
	}
	else if(worldID == WORLDID_CITY)
	{
		switch(levelID)
		{
/*
			case CITYLEV1_ENT:
					bankRomStart	= (u32)&_scrData_4_1_SegmentRomStart;
					bankRomEnd		= (u32)&_scrData_4_1_SegmentRomEnd;
					sprintf(message, "CTY_SCR1");				
				break;
*/
			default:
				return;
		}
	}
	else if(worldID == WORLDID_SUBTERRANEAN)
	{
		switch(levelID)
		{
			default:
				return;
		}
	}
	else if(worldID == WORLDID_LABORATORY)
	{
		switch(levelID)
		{
/*
			case LABORATORYLEV1_ENT:
					bankRomStart	= (u32)&_scrData_6_1_SegmentRomStart;
					bankRomEnd		= (u32)&_scrData_6_1_SegmentRomEnd;
					sprintf(message, "LAB_SCR1");				
				break;
*/
			default:
				return;
		}
	}
	else if(worldID == WORLDID_TOYSHOP)
	{
		switch(levelID)
		{
			default:
				return;
		}
	}
	else if(worldID == WORLDID_HALLOWEEN)
	{
		switch(levelID)
		{
			default:
				return;
		}
	}
	else if(worldID == WORLDID_SUPERRETRO)
	{
		switch(levelID)
		{
/*
			case SRETROLEV1_ENT:
					bankRomStart	= (u32)&_scrData_9_1_SegmentRomStart;
					bankRomEnd		= (u32)&_scrData_9_1_SegmentRomEnd;
					sprintf(message, "SUP_SCR1");				
				break;
*/
			default:
				return;
		}
	}
	else if(worldID == WORLDID_FRONTEND)
	{
		switch(levelID)
		{
/*
			case FRONTEND1_ENT:
				bankRomStart	= (u32)&_scrData_10_1_SegmentRomStart;
				bankRomEnd		= (u32)&_scrData_10_1_SegmentRomEnd;
				sprintf(message, "FRE_SCR1");
				break;
*/
			default:
				return;
		}
	}


	bankSize = bankRomEnd - bankRomStart;

	scriptDat = (char *)JallocAlloc(DMAGetSize(bankRomStart,bankRomEnd),YES,"SCRBANK");
	
//start download from rom

	DMAMemory(scriptDat,bankRomStart,bankRomEnd);
	if(scriptDat)
	{
		dprintf"Loaded script data %s (size %d)\n",message,(int)bankSize));
	}
	else
	{
		dprintf"Unable to load script data %s\n",message));
		return;
	}

	InitLevelScript(scriptDat);
}
