/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: layoutvisual.c
	Programmer	: Andrew Eder
	Date		: 01/03/99

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>

#include "incs.h"

// Because CD playing pisses me off
#define USE_AUDIO

unsigned long worldNum;
unsigned long levelNum;

// world visual data - for texture and object banks
WORLD_VISUAL_DATA worldVisualData[MAX_WORLDS];

UBYTE darkenedLevel		= 0;
short lightIntensity	= 255;

//////////////////////////////////////////////////////////////////////////////////////////////////
//************************************************************************************************
// PC VERSION ONLY - PC VERSION ONLY - PC VERSION ONLY - PC VERSION ONLY - PC VERSION ONLY
//************************************************************************************************
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef PC_VERSION

WORLD_VISUAL_DATA worldVisualData[MAX_WORLDS] = 
{
//------------------------------------------------ GARDEN
	{
	"garden",				// description
	WORLDID_GARDEN,			// world ID
	GARDENMASTER_OBJ_BANK,		// master object bank
	GARDEN_TEX_BANK,		// master texture bank
	WORLD_OPEN,
	5,						// number of levels
		{ 
		"garden 1  70",	LEVELID_GARDEN1,GARDENLEV1_OBJ_BANK,0,GARDENLEV1_COL,LEVEL_OPEN,WORLDID_ANCIENT,0,  0,0,0,1100,1100,800,
		"garden 2  60",	LEVELID_GARDEN2,GARDENLEV2_OBJ_BANK,0,GARDENLEV2_COL,LEVEL_CLOSED,WORLDID_SPACE,0, 0,0,0,1100,1100,800,
		"garden 3  50",	LEVELID_GARDEN3,GARDENLEV3_OBJ_BANK,0,GARDENLEV3_COL,LEVEL_CLOSED,WORLDID_CITY,0, 0,0, 0,1100,1100,800,
		"garden boss 10",	LEVELID_GARDEN4,GARDENBOSSA_OBJ_BANK,0,GARDENMASTERA_COL,LEVEL_CLOSED,WORLDID_SUBTERRANEAN,0, 0,0, 0,1100,1100,800,
		"garden bonus 0",	LEVELID_GARDEN5,GARDENBONUS_OBJ_BANK,0,GARDENBONUS_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"garden multi 0",	LEVELID_GARDEN6,GARDENMULTI_OBJ_BANK,0,GARDENMULTI_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		},
	},

//------------------------------------------------ ANCIENT

	{
	"ancient",			// description
	WORLDID_ANCIENT,		// world ID
	ANCIENTMASTER_OBJ_BANK,	// master object bank
	ANCIENT_TEX_BANK,	// master texture bank
	WORLD_CLOSED,
	5,						// number of levels
		{ 
		"ancient 1 40",	LEVELID_ANCIENT1,ANCIENTLEV1_OBJ_BANK,0,ANCIENTLEV1_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"ancient 2 40",	LEVELID_ANCIENT2,ANCIENTLEV2_OBJ_BANK,0,ANCIENTLEV2_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"ancient 3 30",	LEVELID_ANCIENT3,ANCIENTLEV3_OBJ_BANK,0,ANCIENTLEV3_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"ancient boss 30",	LEVELID_ANCIENT4,ANCIENTBOSS_OBJ_BANK,0,ANCIENTMASTER_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"ancient bonus 0",LEVELID_ANCIENT5,ANCIENTBONUS_OBJ_BANK,0,ANCIENTBONUS_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"ancient multi 0",LEVELID_ANCIENT6,ANCIENTMULTI_OBJ_BANK,0,ANCIENTMULTI_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		},
	},


//------------------------------------------------ SPACE

	{
	"space",			// description
	WORLDID_SPACE,		// world ID
	SPACEMASTER_OBJ_BANK,		// master object bank
	SPACE_TEX_BANK,		// master texture bank
	WORLD_CLOSED,
	5,						// number of levels
		{ 
		"space 1 30",		LEVELID_SPACE1,SPACELEV1_OBJ_BANK,0,SPACELEV1_COL,LEVEL_CLOSED,-1,0, 0,0, 0,500,6500,1100,
		"space 2 30",		LEVELID_SPACE2,SPACELEV2_OBJ_BANK,0,SPACELEV2_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"space 3 20",		LEVELID_SPACE3,SPACELEV3_OBJ_BANK,0,SPACELEV3_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"space boss 10",	LEVELID_SPACE4,SPACEBOSS_OBJ_BANK,0,SPACEMASTER_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"space bonus 20",	LEVELID_SPACE5,SPACEBONUS_OBJ_BANK,0,SPACEBONUS_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"space multi 0",	LEVELID_SPACE6,SPACEMULTI_OBJ_BANK,0,SPACEMULTI_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		},
	},

//------------------------------------------------ CITY

	{
	"city",				// description
	WORLDID_CITY,		// world ID
	CITYMASTER_OBJ_BANK,		// master object bank
	CITY_TEX_BANK,		// master texture bank
	WORLD_OPEN,
	5,						// number of levels
		{ 
		"city 1 30",		LEVELID_CITYDOCKS,CITYLEV1_OBJ_BANK,0,CITYLEV1_COL,LEVEL_CLOSED,WORLDID_TOYSHOP,0, 0,0, 0,1100,1100,800,
		"city 2 30",		LEVELID_CITYWAREHOUSE,CITYLEV2_OBJ_BANK,0,CITYLEV2_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"city 3 20",		LEVELID_CITYSTREETS,CITYLEV3_OBJ_BANK,0,CITYLEV3_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"city tower 10",	LEVELID_CITYTOWER,CITYBOSS_OBJ_BANK,0,CITYMASTER_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"city bonus 20",	LEVELID_CITYBONUS,CITYBONUS_OBJ_BANK,0,CITYBONUS_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"city multi 0",		LEVELID_CITYMULTI,CITYMULTI_OBJ_BANK,0,CITYMULTI_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		},
	},

//------------------------------------------------ SUBTERRANEAN

	{
	"subterranean",				// description
	WORLDID_SUBTERRANEAN,		// world ID
	SUBTERRANEANMASTER_OBJ_BANK,		// master object bank
	SUBTERRANEAN_TEX_BANK,		// master texture bank
	WORLD_CLOSED,
	5,						// number of levels
		{ 
		"sub 1 40",		LEVELID_SUBTERRANEAN1,SUBTERRANEANLEV1_OBJ_BANK,0,SUBTERRANEANLEV1_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"sub 2 40",		LEVELID_SUBTERRANEAN2,SUBTERRANEANLEV2_OBJ_BANK,0,SUBTERRANEANLEV2_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"sub 3 20",		LEVELID_SUBTERRANEAN3,SUBTERRANEANLEV3_OBJ_BANK,0,SUBTERRANEANLEV3_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"sub boss 10",		LEVELID_SUBTERRANEAN4,SUBTERRANEANBOSS_OBJ_BANK,0,SUBTERRANEANMASTER_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"sub test xx",		LEVELID_SUBTERRANEAN4,SUBTERRANEANBOSSB_OBJ_BANK,0,SUBTERRANEANMASTERB_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"sub bonus 0",	LEVELID_SUBTERRANEAN5,SUBTERRANEANBONUS_OBJ_BANK,0,SUBTERRANEANBONUS_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"sub multi 0",	LEVELID_SUBTERRANEAN6,SUBTERRANEANMULTI_OBJ_BANK,0,SUBTERRANEANMULTI_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		},
	},

//------------------------------------------------ LABORATORY

	{
	"laboratory",				// description
	WORLDID_LABORATORY,		// world ID
	LABORATORYMASTER_OBJ_BANK,		// master object bank
	LABORATORY_TEX_BANK,		// master texture bank
	WORLD_CLOSED,
	5,						// number of levels
		{ 
		"lab 1 70",		LEVELID_LABORATORY1,LABORATORYLEV1_OBJ_BANK,0,LABORATORYLEV1_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"lab 2 60",		LEVELID_LABORATORY2,LABORATORYLEV2_OBJ_BANK,0,LABORATORYLEV2_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"lab 3 20",		LEVELID_LABORATORY3,LABORATORYLEV3_OBJ_BANK,0,LABORATORYLEV3_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"lab boss a 40",LEVELID_LABORATORY4,LABORATORYBOSSA_OBJ_BANK,0,LABORATORYMASTERA_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"lab boss b 40",LEVELID_LABORATORY5,LABORATORYBOSSB_OBJ_BANK,0,LABORATORYMASTERB_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"lab boss c 40",LEVELID_LABORATORY6,LABORATORYBOSSC_OBJ_BANK,0,LABORATORYMASTERC_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"lab bonus 0",	LEVELID_LABORATORY7,LABORATORYBONUS_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"lab multi 0",	LEVELID_LABORATORY8,LABORATORYMULTI_OBJ_BANK,0,LABORATORYMULTI_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		},
	},

//------------------------------------------------ TOYSHOP

	{
	"toy shop",				// description
	WORLDID_TOYSHOP,		// world ID
	TOYSHOPMASTER_OBJ_BANK,		// master object bank
	TOYSHOP_TEX_BANK,		// master texture bank
	WORLD_CLOSED,
	5,						// number of levels
		{ 
		"toy shop 1 0",		LEVELID_TOYSHOP1,TOYSHOPLEV1_OBJ_BANK,0,TOYSHOPLEV1_COL,LEVEL_CLOSED,WORLDID_TOYSHOP,0, 0,0, 0,1100,1100,800,
		"toy shop 2 0",		LEVELID_TOYSHOP2,TOYSHOPLEV2_OBJ_BANK,0,TOYSHOPLEV2_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"toy shop 3 0",		LEVELID_TOYSHOP3,TOYSHOPLEV3_OBJ_BANK,0,TOYSHOPLEV3_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"toy shop boss 0",	LEVELID_TOYSHOP4,TOYSHOPBOSS_OBJ_BANK,0,TOYSHOPMASTER_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"toy shop bonus 0",	LEVELID_TOYSHOP5,TOYSHOPBONUS_OBJ_BANK,0,TOYSHOPBONUS_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"toy shop multi 0",	LEVELID_TOYSHOP6,TOYSHOPMULTI_OBJ_BANK,0,TOYSHOPMULTI_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		},
	},


//------------------------------------------------ HALLOWEEN

	{
	"halloween",					// description
	WORLDID_HALLOWEEN,			// world ID
	HALLOWEENMASTER_OBJ_BANK,			// master object bank
	HALLOWEEN_TEX_BANK,			// master texture bank
	WORLD_CLOSED,
	5,						// number of levels 
		{ 
		"halloween 1 10",		LEVELID_HALLOWEEN1,	HALLOWEENLEV1_OBJ_BANK,0,HALLOWEENLEV1_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"halloween 2 0",		LEVELID_HALLOWEEN2,	HALLOWEENLEV2_OBJ_BANK,0,HALLOWEENLEV2_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"halloween 3 0",		LEVELID_HALLOWEEN3,	HALLOWEENLEV3_OBJ_BANK,0,HALLOWEENLEV3_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"halloween boss 0",	LEVELID_HALLOWEEN4,	HALLOWEENBOSS_OBJ_BANK,0,HALLOWEENMASTER_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"halloween bonus 0",	LEVELID_HALLOWEEN5,	HALLOWEENBONUS_OBJ_BANK,0,-1,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"halloween multi 0",	LEVELID_HALLOWEEN6,	HALLOWEENMULTI_OBJ_BANK,0,HALLOWEENMULTI_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		},
	},

	

//------------------------------------------------ SUPERRETRO

	{
	"super retro",			// description
	WORLDID_SPACE,		// world ID
	RETROMASTER_OBJ_BANK,	// master object bank
	SUPERRETRO_TEX_BANK,	// master texture bank
	WORLD_CLOSED,
	5,						// number of levels
		{ 
		"super 1 60",		LEVELID_SUPERRETRO1,RETROLEV1_OBJ_BANK,0,RETROLEV1_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"super 2 60",		LEVELID_SUPERRETRO2,RETROLEV2_OBJ_BANK,0,RETROLEV2_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"super 3 60",		LEVELID_SUPERRETRO3,RETROLEV3_OBJ_BANK,0,RETROLEV3_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"super 4 60",		LEVELID_SUPERRETRO4,RETROLEV4_OBJ_BANK,0,RETROLEV4_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"super 5 60",		LEVELID_SUPERRETRO5,RETROLEV5_OBJ_BANK,0,RETROLEV5_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"super 6 60",		LEVELID_SUPERRETRO6,RETROLEV6_OBJ_BANK,0,RETROLEV6_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"super 7 60",		LEVELID_SUPERRETRO7,RETROLEV7_OBJ_BANK,0,RETROLEV7_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"super 8 60",		LEVELID_SUPERRETRO8,RETROLEV8_OBJ_BANK,0,RETROLEV8_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"super 9 60",		LEVELID_SUPERRETRO9,RETROLEV9_OBJ_BANK,0,RETROLEV9_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		"super 10 60",		LEVELID_SUPERRETRO10,RETROLEV10_OBJ_BANK,0,RETROLEV10_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
//		"super boss",	LEVELID_SUPERRETRO11,RETROBOSS_OBJ_BANK,0,RETROMASTER_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
//		"super bonus",	LEVELID_SUPERRETRO12,RETROBONUS_OBJ_BANK,0,RETROBONUS_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
//		"super multi",	LEVELID_SUPERRETRO13,RETROMULTI_OBJ_BANK,0,RETROMULTI_COL,LEVEL_CLOSED,-1,0, 0,0, 0,1100,1100,800,
		},
	},

//------------------------------------------------

	{
	"frontend",			// description
	WORLDID_FRONTEND,		// world ID
	FRONTEND_OBJ_BANK,	// master object bank
	FRONTEND_TEX_BANK,	// master texture bank
	WORLD_OPEN,
	4,						// number of levels
		{ 
		"start",		LEVELID_FRONTEND1,FRONTEND1_OBJ_BANK,0,FRONTEND1_COL,LEVEL_OPEN,-1,0, 120,120,160,500,6500,1100,
		"levela",		LEVELID_FRONTEND2,FRONTEND2_OBJ_BANK,0,FRONTEND2_COL,LEVEL_OPEN,-1,0, 0,0, 0,1100,1100,800,
		"levelb",		LEVELID_FRONTEND3,FRONTEND3_OBJ_BANK,0,FRONTEND3_COL,LEVEL_OPEN,-1,0, 0,0, 0,1100,1100,800,
		"multisel",		LEVELID_FRONTEND4,FRONTEND4_OBJ_BANK,0,FRONTEND4_COL,LEVEL_OPEN,-1,0, 0,0, 0,1100,1100,800,
		"language",		LEVELID_FRONTEND5,FRONTEND5_OBJ_BANK,0,FRONTEND5_COL,LEVEL_OPEN,-1,0, 0,0, 0,1100,1100,800,
		},
	}
};





//////////////////////////////////////////////////////////////////////////////////////////////////
//************************************************************************************************
// N64 VERSION ONLY - N64 VERSION ONLY - N64 VERSION ONLY - N64 VERSION ONLY - N64 VERSION ONLY
//************************************************************************************************
//////////////////////////////////////////////////////////////////////////////////////////////////
#else

WORLD_VISUAL_DATA worldVisualData[MAX_WORLDS] = 
{
//------------------------------------------------ GARDEN
	{
	"garden",					// description
	WORLDID_GARDEN,				// world ID
	GENERIC_GARDEN_OBJ_BANK,		// master object bank
	GARDEN_TEX_BANK,	// master texture bank
	WORLD_OPEN,
	7,							// number of levels
		{ 
		"the spawn lawn",		LEVELID_GARDENLEV1,LEVEL1_GARDEN_OBJ_BANK,GARDEN_TEX_BANK,GARDENLEV1_COL,LEVEL_OPEN,WORLDID_ANCIENT,0,			0,0,0,1100,1100,800,
		"a-maze-ing garden",	LEVELID_GARDENLEV2,LEVEL2_GARDEN_OBJ_BANK,GARDEN_TEX_BANK,GARDENLEV2_COL,LEVEL_OPEN,WORLDID_SPACE,0,				0,0,0,1100,1100,800,
		"vegging out",			LEVELID_GARDENLEV3,LEVEL3_GARDEN_OBJ_BANK,GARDEN_TEX_BANK,GARDENLEV3_COL,LEVEL_OPEN,WORLDID_CITY,0,				0,0,0,1100,1100,800,
		"garden boss",			LEVELID_GARDENBOSSA,BOSSA_GARDEN_OBJ_BANK,GARDEN_TEX_BANK,GARDENMASTERA_COL,LEVEL_OPEN,WORLDID_SUBTERRANEAN,0,	0,0,0,1100,1100,800,
		"tree tops b", 			LEVELID_GARDENBOSSB,BOSSB_GARDEN_OBJ_BANK,GARDEN_TEX_BANK,GARDENMASTERB_COL,LEVEL_OPEN,0,0,						0,0,0,1100,1100,800,
		"garden bonus",			LEVELID_GARDENBONUS,BONUS_GARDEN_OBJ_BANK,GARDEN_TEX_BANK,GARDENBONUS_COL,LEVEL_OPEN,-1,0,						0,0,0,1100,1100,800,
		"garden multi",			LEVELID_GARDENMULTI,MULTI_GARDEN_OBJ_BANK,GARDEN_TEX_BANK,GARDENMULTI_COL,LEVEL_OPEN,-1,0,						0,0,0,1100,1100,800,
		},
	},

//------------------------------------------------ ANCIENT

	{
	"ancient",			// description
	WORLDID_ANCIENT,		// world ID
	GENERIC_ANCIENT_OBJ_BANK,	// master object bank
	ANCIENT_TEX_BANK,	// master texture bank
	WORLD_CLOSED,
	9,						// number of levels
		{ 
		"ancient 1",		LEVELID_ANCIENTLEV1,LEVEL1_ANCIENT_OBJ_BANK,ANCIENT_TEX_BANK,ANCIENTLEV1_COL,LEVEL_OPEN,-1,0,				0,0,0,1100,1100,800,
		"ancient 2",		LEVELID_ANCIENTLEV2,LEVEL2_ANCIENT_OBJ_BANK,ANCIENT_TEX_BANK,ANCIENTLEV2_COL,LEVEL_OPEN,-1,0,				0,0,0,1100,1100,800,
		"ancient 3",		LEVELID_ANCIENTLEV3,LEVEL3_ANCIENT_OBJ_BANK,ANCIENT_TEX_BANK,ANCIENTLEV3_COL,LEVEL_OPEN,-1,0,				0,0,0,1100,1100,800,
		"ancient boss",		LEVELID_ANCIENTBOSSA,BOSSA_ANCIENT_OBJ_BANK,ANCIENT_TEX_BANK,ANCIENTMASTERA_COL,LEVEL_OPEN,-1,0,				0,0,0,1100,1100,800,
		"ancient boss a",	LEVELID_ANCIENTBOSSB,BOSSA_ANCIENT_OBJ_BANK,ANCIENT_TEX_BANK,ANCIENTMASTERB_COL,LEVEL_OPEN,-1,0,				0,0,0,1100,1100,800,
		"ancient boss b",	LEVELID_ANCIENTBOSSC,BOSSA_ANCIENT_OBJ_BANK,ANCIENT_TEX_BANK,ANCIENTMASTERC_COL,LEVEL_OPEN,-1,0,				0,0,0,1100,1100,800,
		"ancient boss c",	LEVELID_ANCIENTBOSSD,BOSSA_ANCIENT_OBJ_BANK,ANCIENT_TEX_BANK,ANCIENTMASTERD_COL,LEVEL_OPEN,-1,0,				0,0,0,1100,1100,800,
		"ancient bonus",	LEVELID_ANCIENTBONUS,BONUS_ANCIENT_OBJ_BANK,ANCIENT_TEX_BANK,ANCIENTBONUS_COL,LEVEL_OPEN,-1,0,				0,0,0,1100,1100,800,
		"ancient multi",	LEVELID_ANCIENTMULTI,MULTI_ANCIENT_OBJ_BANK,ANCIENT_TEX_BANK,ANCIENTMULTI_COL,LEVEL_OPEN,-1,0,				0,0,0,1100,1100,800,
		},
	},


//------------------------------------------------ SPACE

	{
	"space",			// description
	WORLDID_SPACE,		// world ID
	GENERIC_SPACE_OBJ_BANK,		// master object bank
	SPACE_TEX_BANK,		// master texture bank
	WORLD_CLOSED,
	6,						// number of levels
		{ 
		"space 1",		LEVELID_SPACELEV1,LEVEL1_SPACE_OBJ_BANK,SPACE_TEX_BANK,SPACELEV1_COL,LEVEL_OPEN,-1,0,			0,0,0,1100,1100,800,
		"space 2",		LEVELID_SPACELEV2,LEVEL2_SPACE_OBJ_BANK,SPACE_TEX_BANK,SPACELEV2_COL,LEVEL_OPEN,-1,0,			0,0,0,1100,1100,800,
		"space 3",		LEVELID_SPACELEV3,LEVEL3_SPACE_OBJ_BANK,SPACE_TEX_BANK,SPACELEV3_COL,LEVEL_OPEN,-1,0,			0,0,0,1100,1100,800,
		"space boss",	LEVELID_SPACEBOSSA,BOSSA_SPACE_OBJ_BANK,SPACE_TEX_BANK,SPACEMASTERA_COL,LEVEL_OPEN,-1,0,			0,0,0,1100,1100,800,
		"space bonus",	LEVELID_SPACEBONUS,BONUS_SPACE_OBJ_BANK,SPACE_TEX_BANK,SPACEBONUS_COL,LEVEL_OPEN,-1,0,			0,0,0,1100,1100,800,
		"space multi",	LEVELID_SPACEMULTI,MULTI_SPACE_OBJ_BANK,SPACE_TEX_BANK,SPACEMULTI_COL,LEVEL_OPEN,-1,0,			0,0,0,1100,1100,800,
		},
	},

//------------------------------------------------ CITY

	{
	"city",				// description
	WORLDID_CITY,		// world ID
	GENERIC_CITY_OBJ_BANK,		// master object bank
	CITY_TEX_BANK,		// master texture bank
	WORLD_OPEN,
	6,						// number of levels
		{ 
		"city 1",		LEVELID_CITYLEV1,LEVEL1_CITY_OBJ_BANK,CITY_TEX_BANK,CITYLEV1_COL,LEVEL_OPEN,WORLDID_TOYSHOP,0,	0,0,0,1100,1100,800,
		"city 2",		LEVELID_CITYLEV2,LEVEL2_CITY_OBJ_BANK,CITY_TEX_BANK,CITYLEV2_COL,LEVEL_OPEN,-1,0,				0,0,0,1100,1100,800,
		"city 3",		LEVELID_CITYLEV3,LEVEL3_CITY_OBJ_BANK,CITY_TEX_BANK,CITYLEV3_COL,LEVEL_OPEN,-1,0,				0,0,0,1100,1100,800,
		"city boss",	LEVELID_CITYBOSSA,BOSSA_CITY_OBJ_BANK,CITY_TEX_BANK,CITYMASTERA_COL,LEVEL_OPEN,-1,0,				0,0,0,1100,1100,800,
		"city bonus",	LEVELID_CITYBONUS,BONUS_CITY_OBJ_BANK,CITY_TEX_BANK,CITYBONUS_COL,LEVEL_OPEN,-1,0,				0,0,0,1100,1100,800,
		"city multi",	LEVELID_CITYMULTI,MULTI_CITY_OBJ_BANK,CITY_TEX_BANK,CITYMULTI_COL,LEVEL_OPEN,-1,0,				0,0,0,1100,1100,800,
		},
	},

//------------------------------------------------ SUBTERRANEAN

	{
	"subterranean",				// description
	WORLDID_SUBTERRANEAN,		// world ID
	GENERIC_SUBTERRANEAN_OBJ_BANK,		// master object bank
	SUBTERRANEAN_TEX_BANK,		// master texture bank
	WORLD_CLOSED,
	6,						// number of levels
		{ 
		"sub 1",		LEVELID_SUBTERRANEANLEV1,LEVEL1_SUBTERRANEAN_OBJ_BANK,SUBTERRANEAN_TEX_BANK,SUBTERRANEANLEV1_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
		"sub 2",		LEVELID_SUBTERRANEANLEV2,LEVEL2_SUBTERRANEAN_OBJ_BANK,SUBTERRANEAN_TEX_BANK,SUBTERRANEANLEV2_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
		"sub 3",		LEVELID_SUBTERRANEANLEV3,LEVEL3_SUBTERRANEAN_OBJ_BANK,SUBTERRANEAN_TEX_BANK,SUBTERRANEANLEV3_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
		"sub boss",		LEVELID_SUBTERRANEANBOSSA,BOSSA_SUBTERRANEAN_OBJ_BANK,SUBTERRANEAN_TEX_BANK,SUBTERRANEANMASTERA_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
		"sub test",		LEVELID_SUBTERRANEANBOSSB,BOSSB_SUBTERRANEAN_OBJ_BANK,SUBTERRANEAN_TEX_BANK,SUBTERRANEANMASTERB_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
		"sub bonus",	LEVELID_SUBTERRANEANBONUS,BONUS_SUBTERRANEAN_OBJ_BANK,SUBTERRANEAN_TEX_BANK,SUBTERRANEANBONUS_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
		"sub multi",	LEVELID_SUBTERRANEANMULTI,MULTI_SUBTERRANEAN_OBJ_BANK,SUBTERRANEAN_TEX_BANK,SUBTERRANEANMULTI_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
		},
	},

//------------------------------------------------ LABORATORY

	{
	"laboratory",				// description
	WORLDID_LABORATORY,		// world ID
	GENERIC_LABORATORY_OBJ_BANK,		// master object bank
	LABORATORY_TEX_BANK,		// master texture bank
	WORLD_CLOSED,
	6,						// number of levels
		{ 
		"lab 1",		LEVELID_LABORATORYLEV1,LEVEL1_LABORATORY_OBJ_BANK,LABORATORY_TEX_BANK,LABORATORYLEV1_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
		"lab 2",		LEVELID_LABORATORYLEV2,LEVEL2_LABORATORY_OBJ_BANK,LABORATORY_TEX_BANK,LABORATORYLEV2_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
		"lab 3",		LEVELID_LABORATORYLEV3,LEVEL3_LABORATORY_OBJ_BANK,LABORATORY_TEX_BANK,LABORATORYLEV3_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
		"lab boss",		LEVELID_LABORATORYBOSSA,BOSSA_LABORATORY_OBJ_BANK,LABORATORY_TEX_BANK,LABORATORYMASTERA_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
		"lab bonus",	LEVELID_LABORATORYBONUS,BONUS_LABORATORY_OBJ_BANK,LABORATORY_TEX_BANK,LABORATORYBONUS_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
		"lab multi",	LEVELID_LABORATORYMULTI,MULTI_LABORATORY_OBJ_BANK,LABORATORY_TEX_BANK,LABORATORYMULTI_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
		},
	},

//------------------------------------------------ TOYSHOP

	{
	"toy shop",				// description
	WORLDID_TOYSHOP,		// world ID
	GENERIC_TOYSHOP_OBJ_BANK,		// master object bank
	TOYSHOP_TEX_BANK,		// master texture bank
	WORLD_CLOSED,
	6,						// number of levels
		{ 
		"toy shop 1",		LEVELID_TOYSHOPLEV1,LEVEL1_TOYSHOP_OBJ_BANK,TOYSHOP_TEX_BANK,TOYSHOPLEV1_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
		"toy shop 2",		LEVELID_TOYSHOPLEV2,LEVEL2_TOYSHOP_OBJ_BANK,TOYSHOP_TEX_BANK,TOYSHOPLEV2_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
		"toy shop 3",		LEVELID_TOYSHOPLEV3,LEVEL3_TOYSHOP_OBJ_BANK,TOYSHOP_TEX_BANK,TOYSHOPLEV3_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
		"toy shop boss",	LEVELID_TOYSHOPBOSSA,BOSSA_TOYSHOP_OBJ_BANK,TOYSHOP_TEX_BANK,TOYSHOPMASTERA_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
		"toy shop bonus",	LEVELID_TOYSHOPBONUS,BONUS_TOYSHOP_OBJ_BANK,TOYSHOP_TEX_BANK,TOYSHOPBONUS_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
		"toy shop multi",	LEVELID_TOYSHOPMULTI,MULTI_TOYSHOP_OBJ_BANK,TOYSHOP_TEX_BANK,TOYSHOPMULTI_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
		},
	},


//------------------------------------------------ HALLOWEEN

	{
	"halloween",					// description
	WORLDID_HALLOWEEN,			// world ID
	GENERIC_HALLOWEEN_OBJ_BANK,	// master object bank
	HALLOWEEN_TEX_BANK,			// master texture bank
	WORLD_CLOSED,
	6,						// number of levels 
		{ 
		"halloween 1",		LEVELID_HALLOWEENLEV1, LEVEL1_HALLOWEEN_OBJ_BANK,HALLOWEEN_TEX_BANK,HALLOWEENLEV1_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
		"halloween 2",		LEVELID_HALLOWEENLEV2, LEVEL2_HALLOWEEN_OBJ_BANK,HALLOWEEN_TEX_BANK,HALLOWEENLEV2_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
		"halloween 3",		LEVELID_HALLOWEENLEV3, LEVEL3_HALLOWEEN_OBJ_BANK,HALLOWEEN_TEX_BANK,HALLOWEENLEV3_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
		"halloween boss",	LEVELID_HALLOWEENBOSSA,	BOSSA_HALLOWEEN_OBJ_BANK,HALLOWEEN_TEX_BANK,HALLOWEENMASTERA_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
		"halloween bonus",	LEVELID_HALLOWEENBONUS,	BONUS_HALLOWEEN_OBJ_BANK,HALLOWEEN_TEX_BANK,HALLOWEENBONUS_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
		"halloween multi",	LEVELID_HALLOWEENMULTI,	MULTI_HALLOWEEN_OBJ_BANK,HALLOWEEN_TEX_BANK,HALLOWEENMULTI_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
		},
	},

	

//------------------------------------------------ SUPERRETRO

	{
	"super retro",			// description
	WORLDID_SUPERRETRO,		// world ID
	GENERIC_RETRO_OBJ_BANK,	// master object bank
	SUPERRETRO_TEX_BANK,	// master texture bank
	WORLD_OPEN,
	6,						// number of levels
		{ 
		"super 1",		LEVELID_SUPERRETROLEV1, LEVEL1_RETRO_OBJ_BANK, SUPERRETRO_TEX_BANK, RETROLEV1_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
		"super 2",		LEVELID_SUPERRETROLEV2, LEVEL2_RETRO_OBJ_BANK, SUPERRETRO_TEX_BANK, RETROLEV2_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
		"super 3",		LEVELID_SUPERRETROLEV3, LEVEL3_RETRO_OBJ_BANK, SUPERRETRO_TEX_BANK, RETROLEV3_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
		"super 4",		LEVELID_SUPERRETROLEV4, LEVEL4_RETRO_OBJ_BANK, SUPERRETRO_TEX_BANK, RETROLEV4_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
		"super 5",		LEVELID_SUPERRETROLEV5, LEVEL5_RETRO_OBJ_BANK, SUPERRETRO_TEX_BANK, RETROLEV5_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
		"super 6",		LEVELID_SUPERRETROLEV6, LEVEL6_RETRO_OBJ_BANK, SUPERRETRO_TEX_BANK, RETROLEV6_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
		"super 7",		LEVELID_SUPERRETROLEV7, LEVEL7_RETRO_OBJ_BANK, SUPERRETRO_TEX_BANK, RETROLEV7_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
		"super 8",		LEVELID_SUPERRETROLEV8, LEVEL8_RETRO_OBJ_BANK, SUPERRETRO_TEX_BANK, RETROLEV8_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
		"super 9",		LEVELID_SUPERRETROLEV9, LEVEL9_RETRO_OBJ_BANK, SUPERRETRO_TEX_BANK, RETROLEV9_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
		"super 10",		LEVELID_SUPERRETROLEV10,LEVEL10_RETRO_OBJ_BANK,SUPERRETRO_TEX_BANK,RETROLEV10_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
//		"super boss",	LEVELID_SUPERRETROBOSSA,BOSSA_RETRO_OBJ_BANK,BOSSA_RETRO_TEX_BANK,RETROMASTERA_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
//		"super bonus",	LEVELID_SUPERRETROBONUS,BONUS_RETRO_OBJ_BANK,BONUS_RETRO_TEX_BANK,RETROBONUS_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
//		"super multi",	LEVELID_SUPERRETROMULTI,MULTI_RETRO_OBJ_BANK,MULTI_RETRO_TEX_BANK,RETROMULTI_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
		},
	},

//------------------------------------------------

	{
	"frontend",				// description
	WORLDID_FRONTEND,		// world ID
	FRONTEND_OBJ_BANK,		// master object bank
	FRONTEND_TEX_BANK,		// master texture bank
	WORLD_OPEN,
	5,						// number of levels
		{ 
		"start",		LEVELID_FRONTEND1,FRONTEND1_OBJ_BANK,FRONTEND_TEX_BANK,FRONTEND1_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
		"levela",		LEVELID_FRONTEND2,FRONTEND2_OBJ_BANK,FRONTEND_TEX_BANK,FRONTEND2_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
		"levelb",		LEVELID_FRONTEND3,FRONTEND3_OBJ_BANK,FRONTEND_TEX_BANK,FRONTEND3_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
		"multisel",		LEVELID_FRONTEND4,FRONTEND4_OBJ_BANK,FRONTEND_TEX_BANK,FRONTEND4_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
		"language",		LEVELID_FRONTEND5,FRONTEND5_OBJ_BANK,FRONTEND_TEX_BANK,FRONTEND5_COL,LEVEL_OPEN,-1,0,	0,0,0,1100,1100,800,
		},
	}
};

#endif


//////////////////////////////////////////////////////////////////////////////////////////////////
//************************************************************************************************
// BOTH VERSIONS (PC AND N64)
//************************************************************************************************
//////////////////////////////////////////////////////////////////////////////////////////////////


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
/* --------------------------------------------------------------------------------
	Programmer	: Matthew Cloy
	Function	: InitLevel
	Purpose		:
	Parameters	: unsigned long , unsigned long
	Returns		: void 
*/
long initialCamera = 0;
void InitLevel(unsigned long worldID,unsigned long levelID)
{
	int i;

	// prepare screen for fade in
	fadeOut		= 255;
	fadeStep	= 8;
	fadeDir		= FADE_IN;
	doScreenFade = 63;

	// load the system and in-game generic texture banks
	LoadTextureBank(SYSTEM_TEX_BANK);
	LoadTextureBank(INGAMEGENERIC_TEX_BANK);

	// load relevant collison, texture and object banks
	LoadMapBank(worldVisualData[worldID].levelVisualData[levelID].collBank);
	LoadVisualBanksForWorld(worldID,levelID);

#ifdef PC_VERSION
	InitSampleList();
#endif
	InitAmbientSoundList();

#ifdef USE_AUDIO
#ifdef PC_VERSION
	LoadSfx(worldID);
#endif
#endif

	// initialise the various lists
	InitSpriteFrameLists();
	InitSpriteOverlayLinkedList();
	InitTextOverlayLinkedList();
	InitSpecFXList();
	InitBabyList( TRUE ); // MUST BE DONE BEFORE ENEMY INITS!
	InitEnemyLinkedList();
	InitPlatformLinkedList();
	InitGaribLinkedList();
	InitTriggerList();
	Init3DTextList( );
	InitTongues( );

	player[0].worldNum = worldID;
	player[0].levelNum = levelID;

	pOIDistance = 50000.0;
	pointOfInterest = NULL;

	// create objects for the level
	CreateLevelObjects(worldID,levelID);

	LoadLevelEntities(worldID,levelID);
	LoadLevelScript(worldID,levelID);

	i=0;
	initialCamera = 1;
	SetVector(&(currCamSource[i]),&outVec);
	SetVector(&(currCamTarget[i]),&inVec);
		
	CreateFrogger(1,1);

	// prepare the text overlays for the current level
	InitInGameTextOverlays(worldID,levelID);

#ifndef PC_VERSION
	MusSetMasterVolume(MUSFLAG_SONGS,32000);
#endif

#ifdef USE_AUDIO
	PrepareSongForLevel((short)worldID,(short)levelID);
#endif

#ifdef PC_VERSION // TEMPORARY
	fog.mode = 1;
	fog.r = worldVisualData[worldID].levelVisualData[levelID].fogR;
	fog.g = worldVisualData[worldID].levelVisualData[levelID].fogG;
	fog.b = worldVisualData[worldID].levelVisualData[levelID].fogB;
	farClip = 	worldVisualData[worldID].levelVisualData[levelID].farDist;
	fStart = worldVisualData[worldID].levelVisualData[levelID].fogNearDist * 0.0005;
	fEnd = 	(worldVisualData[worldID].levelVisualData[levelID].fogFarDist * 0.0005);
#endif

	frameCount = 0;
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

#ifdef N64_VERSION
	MusHandleStop(audioCtrl.musicHandle[0],0);
	audioCtrl.currentTrack[0] = 0;
#endif

#ifdef PC_VERSION
	StopSong( );
	FreeSampleList();
#endif

	FreeAmbientSoundList();

	KillAllTriggers();

	ResetBabies( );
	FreeSpecFXList( );
	FreeSpriteFrameLists();
	FreeGaribLinkedList();
	FreeTextOverlayLinkedList();
	FreeSpriteOverlayLinkedList();
	FreeSpriteLinkedList();
	FreeEnemyLinkedList();
	FreePlatformLinkedList();
	FreePathList();
	Free3DTextList();
	FreeAnimationList();
	FreeTextureList();
	FreeAllObjectBanks();
	FreeAllTextureBanks();
	FreeTransCameraList();
#ifndef PC_VERSION
	FreeLevelEntitys();
	FreeN64WaterResources();
	FreeN64ModgyTexResources();
#endif
	FreeLevelScript();
	FreeMapBank();
	FreeTongues();

	FreeActorList();

#ifndef PC_VERSION
	// global ptrs
	aMapBank = NULL;
	globalPtrEntityBank = NULL;
#endif

	pOIDistance = 50000.0;
	pointOfInterest = NULL;

	InitTextOverlayLinkedList();
	InitSpriteOverlayLinkedList();
	InitSpriteLinkedList();

	//fog.r = fog.g = fog.b = 0;
	fog.mode = 0;
	pauseMode		= 0;
	darkenedLevel	= 0;

	dprintf"-----------------------------\n"));
}
