/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: Block.h
	Programmer	: Andy Eder
	Date		: 15/04/99 16:22:24

----------------------------------------------------------------------------------------------- */

#ifndef BLOCK_H_INCLUDED
#define BLOCK_H_INCLUDED


#include "define.h" 

#define ULONG_MAX	4294967295

#define MAX_LOOKATS	64
#define MAX_HILITES 64

extern char _staticSegmentStart[];
extern char __zbufferSegmentStart[];
extern char _cfbSegmentStart[];

extern char *_texBank1SegmentRomStart;		// SYSTEM TEXTURE BANK
extern char *_texBank1SegmentRomEnd;
extern char *_texBank2SegmentRomStart;		// TITLE SCREENS TEXTURE BANK
extern char *_texBank2SegmentRomEnd;
extern char *_texBank3SegmentRomStart;		// IN GAME GENERIC TEXTURE BANK
extern char *_texBank3SegmentRomEnd;

extern char *_objBank_g_0_SegmentRomStart;	// IN GAME GENERIC OBJECT BANK
extern char *_objBank_g_0_SegmentRomEnd;


//----- [ WORLD OBJECT / TEXTURE / ENTITY BANKS ] ------------------------------------------------

// TEXTURE BANKS - All worlds
extern char *_texBankFrontendSegmentRomStart;
extern char *_texBankFrontendSegmentRomEnd;
extern char *_texBankGardenSegmentRomStart;
extern char *_texBankGardenSegmentRomEnd;
extern char *_texBankAncientSegmentRomStart;
extern char *_texBankAncientSegmentRomEnd;
extern char *_texBankSpaceSegmentRomStart;
extern char *_texBankSpaceSegmentRomEnd;
extern char *_texBankCitySegmentRomStart;
extern char *_texBankCitySegmentRomEnd;
extern char *_texBankSubSegmentRomStart;
extern char *_texBankSubSegmentRomEnd;
extern char *_texBankLabSegmentRomStart;
extern char *_texBankLabSegmentRomEnd;
extern char *_texBankToySegmentRomStart;
extern char *_texBankToySegmentRomEnd;
extern char *_texBankHallowSegmentRomStart;
extern char *_texBankHallowSegmentRomEnd;
extern char *_texBankSRetroSegmentRomStart;
extern char *_texBankSRetroSegmentRomEnd;




// FRONTEND WORLD - Objects ----------------------------------------------------------------------
extern char *_objBank_0_0_SegmentRomStart;
extern char *_objBank_0_0_SegmentRomEnd;
extern char *_objBank_0_1_SegmentRomStart;
extern char *_objBank_0_1_SegmentRomEnd;
extern char *_objBank_0_2_SegmentRomStart;
extern char *_objBank_0_2_SegmentRomEnd;
extern char *_objBank_0_3_SegmentRomStart;
extern char *_objBank_0_3_SegmentRomEnd;
extern char *_objBank_0_4_SegmentRomStart;
extern char *_objBank_0_4_SegmentRomEnd;
extern char *_objBank_0_5_SegmentRomStart;
extern char *_objBank_0_5_SegmentRomEnd;
extern char *_objBank_0_6_SegmentRomStart;
extern char *_objBank_0_6_SegmentRomEnd;
// FRONTEND WORLD - Level Entities
extern char *_levData_10_1_SegmentRomStart;
extern char *_levData_10_1_SegmentRomEnd;
extern char *_levData_10_2_SegmentRomStart;
extern char *_levData_10_2_SegmentRomEnd;
extern char *_levData_10_3_SegmentRomStart;
extern char *_levData_10_3_SegmentRomEnd;
extern char *_levData_10_4_SegmentRomStart;
extern char *_levData_10_4_SegmentRomEnd;
extern char *_levData_10_5_SegmentRomStart;
extern char *_levData_10_5_SegmentRomEnd;
// FRONTEND WORLD - Level Scripts
extern char *_scrData_10_1_SegmentRomStart;
extern char *_scrData_10_1_SegmentRomEnd;
extern char *_scrData_10_2_SegmentRomStart;
extern char *_scrData_10_2_SegmentRomEnd;
extern char *_scrData_10_3_SegmentRomStart;
extern char *_scrData_10_3_SegmentRomEnd;
extern char *_scrData_10_4_SegmentRomStart;
extern char *_scrData_10_4_SegmentRomEnd;
extern char *_scrData_10_5_SegmentRomStart;
extern char *_scrData_10_5_SegmentRomEnd;

// GARDEN WORLD - Objects ------------------------------------------------------------------------
extern char *_objBank_1_0_SegmentRomStart;
extern char *_objBank_1_0_SegmentRomEnd;
extern char *_objBank_1_1_SegmentRomStart;
extern char *_objBank_1_1_SegmentRomEnd;
extern char *_objBank_1_2_SegmentRomStart;
extern char *_objBank_1_2_SegmentRomEnd;
extern char *_objBank_1_3_SegmentRomStart;
extern char *_objBank_1_3_SegmentRomEnd;
extern char *_objBank_1_4_SegmentRomStart;
extern char *_objBank_1_4_SegmentRomEnd;
extern char *_objBank_1_5_SegmentRomStart;
extern char *_objBank_1_5_SegmentRomEnd;
extern char *_objBank_1_6_SegmentRomStart;
extern char *_objBank_1_6_SegmentRomEnd;
extern char *_objBank_1_7_SegmentRomStart;
extern char *_objBank_1_7_SegmentRomEnd;
// GARDEN WORLD - Level Entities
extern char *_levData_1_1_SegmentRomStart;
extern char *_levData_1_1_SegmentRomEnd;
extern char *_levData_1_2_SegmentRomStart;
extern char *_levData_1_2_SegmentRomEnd;
extern char *_levData_1_3_SegmentRomStart;
extern char *_levData_1_3_SegmentRomEnd;
extern char *_levData_1_4_SegmentRomStart;
extern char *_levData_1_4_SegmentRomEnd;
extern char *_levData_1_5_SegmentRomStart;
extern char *_levData_1_5_SegmentRomEnd;
extern char *_levData_1_6_SegmentRomStart;
extern char *_levData_1_6_SegmentRomEnd;
extern char *_levData_1_7_SegmentRomStart;
extern char *_levData_1_7_SegmentRomEnd;
// GARDEN WORLD - Level Scripts
extern char *_scrData_1_1_SegmentRomStart;
extern char *_scrData_1_1_SegmentRomEnd;
extern char *_scrData_1_2_SegmentRomStart;
extern char *_scrData_1_2_SegmentRomEnd;
extern char *_scrData_1_3_SegmentRomStart;
extern char *_scrData_1_3_SegmentRomEnd;
extern char *_scrData_1_4_SegmentRomStart;
extern char *_scrData_1_4_SegmentRomEnd;
extern char *_scrData_1_5_SegmentRomStart;
extern char *_scrData_1_5_SegmentRomEnd;
extern char *_scrData_1_6_SegmentRomStart;
extern char *_scrData_1_6_SegmentRomEnd;
extern char *_scrData_1_7_SegmentRomStart;
extern char *_scrData_1_7_SegmentRomEnd;

// ANCIENT WORLD - Objects -----------------------------------------------------------------------
extern char *_objBank_2_0_SegmentRomStart;
extern char *_objBank_2_0_SegmentRomEnd;
extern char *_objBank_2_1_SegmentRomStart;
extern char *_objBank_2_1_SegmentRomEnd;
extern char *_objBank_2_2_SegmentRomStart;
extern char *_objBank_2_2_SegmentRomEnd;
extern char *_objBank_2_3_SegmentRomStart;
extern char *_objBank_2_3_SegmentRomEnd;
extern char *_objBank_2_4_SegmentRomStart;
extern char *_objBank_2_4_SegmentRomEnd;
extern char *_objBank_2_5_SegmentRomStart;
extern char *_objBank_2_5_SegmentRomEnd;
extern char *_objBank_2_6_SegmentRomStart;
extern char *_objBank_2_6_SegmentRomEnd;
extern char *_objBank_2_7_SegmentRomStart;
extern char *_objBank_2_7_SegmentRomEnd;
extern char *_objBank_2_8_SegmentRomStart;
extern char *_objBank_2_8_SegmentRomEnd;
extern char *_objBank_2_9_SegmentRomStart;
extern char *_objBank_2_9_SegmentRomEnd;
// ANCIENT WORLD - Level Entities
extern char *_levData_2_1_SegmentRomStart;
extern char *_levData_2_1_SegmentRomEnd;
extern char *_levData_2_2_SegmentRomStart;
extern char *_levData_2_2_SegmentRomEnd;
extern char *_levData_2_3_SegmentRomStart;
extern char *_levData_2_3_SegmentRomEnd;
extern char *_levData_2_4_SegmentRomStart;
extern char *_levData_2_4_SegmentRomEnd;
extern char *_levData_2_5_SegmentRomStart;
extern char *_levData_2_5_SegmentRomEnd;
extern char *_levData_2_6_SegmentRomStart;
extern char *_levData_2_6_SegmentRomEnd;
extern char *_levData_2_7_SegmentRomStart;
extern char *_levData_2_7_SegmentRomEnd;
extern char *_levData_2_8_SegmentRomStart;
extern char *_levData_2_8_SegmentRomEnd;
extern char *_levData_2_9_SegmentRomStart;
extern char *_levData_2_9_SegmentRomEnd;
// ANCIENT WORLD - Level Scripts
extern char *_scrData_2_1_SegmentRomStart;
extern char *_scrData_2_1_SegmentRomEnd;
extern char *_scrData_2_2_SegmentRomStart;
extern char *_scrData_2_2_SegmentRomEnd;
extern char *_scrData_2_3_SegmentRomStart;
extern char *_scrData_2_3_SegmentRomEnd;
extern char *_scrData_2_4_SegmentRomStart;
extern char *_scrData_2_4_SegmentRomEnd;
extern char *_scrData_2_5_SegmentRomStart;
extern char *_scrData_2_5_SegmentRomEnd;
extern char *_scrData_2_6_SegmentRomStart;
extern char *_scrData_2_6_SegmentRomEnd;
extern char *_scrData_2_7_SegmentRomStart;
extern char *_scrData_2_7_SegmentRomEnd;
extern char *_scrData_2_8_SegmentRomStart;
extern char *_scrData_2_8_SegmentRomEnd;
extern char *_scrData_2_9_SegmentRomStart;
extern char *_scrData_2_9_SegmentRomEnd;

// SPACE WORLD - Objects -------------------------------------------------------------------------
extern char *_objBank_3_0_SegmentRomStart;
extern char *_objBank_3_0_SegmentRomEnd;
extern char *_objBank_3_1_SegmentRomStart;
extern char *_objBank_3_1_SegmentRomEnd;
extern char *_objBank_3_2_SegmentRomStart;
extern char *_objBank_3_2_SegmentRomEnd;
extern char *_objBank_3_3_SegmentRomStart;
extern char *_objBank_3_3_SegmentRomEnd;
extern char *_objBank_3_4_SegmentRomStart;
extern char *_objBank_3_4_SegmentRomEnd;
extern char *_objBank_3_5_SegmentRomStart;
extern char *_objBank_3_5_SegmentRomEnd;
extern char *_objBank_3_6_SegmentRomStart;
extern char *_objBank_3_6_SegmentRomEnd;
// SPACE WORLD - Level Entities
extern char *_levData_3_1_SegmentRomStart;
extern char *_levData_3_1_SegmentRomEnd;
extern char *_levData_3_2_SegmentRomStart;
extern char *_levData_3_2_SegmentRomEnd;
extern char *_levData_3_3_SegmentRomStart;
extern char *_levData_3_3_SegmentRomEnd;
extern char *_levData_3_4_SegmentRomStart;
extern char *_levData_3_4_SegmentRomEnd;
extern char *_levData_3_5_SegmentRomStart;
extern char *_levData_3_5_SegmentRomEnd;
extern char *_levData_3_6_SegmentRomStart;
extern char *_levData_3_6_SegmentRomEnd;
// SPACE WORLD - Level Scripts
extern char *_scrData_3_1_SegmentRomStart;
extern char *_scrData_3_1_SegmentRomEnd;
extern char *_scrData_3_2_SegmentRomStart;
extern char *_scrData_3_2_SegmentRomEnd;
extern char *_scrData_3_3_SegmentRomStart;
extern char *_scrData_3_3_SegmentRomEnd;
extern char *_scrData_3_4_SegmentRomStart;
extern char *_scrData_3_4_SegmentRomEnd;
extern char *_scrData_3_5_SegmentRomStart;
extern char *_scrData_3_5_SegmentRomEnd;
extern char *_scrData_3_6_SegmentRomStart;
extern char *_scrData_3_6_SegmentRomEnd;

// CITY WORLD - Objects --------------------------------------------------------------------------
extern char *_objBank_4_0_SegmentRomStart;
extern char *_objBank_4_0_SegmentRomEnd;
extern char *_objBank_4_1_SegmentRomStart;
extern char *_objBank_4_1_SegmentRomEnd;
extern char *_objBank_4_2_SegmentRomStart;
extern char *_objBank_4_2_SegmentRomEnd;
extern char *_objBank_4_3_SegmentRomStart;
extern char *_objBank_4_3_SegmentRomEnd;
extern char *_objBank_4_4_SegmentRomStart;
extern char *_objBank_4_4_SegmentRomEnd;
extern char *_objBank_4_5_SegmentRomStart;
extern char *_objBank_4_5_SegmentRomEnd;
extern char *_objBank_4_6_SegmentRomStart;
extern char *_objBank_4_6_SegmentRomEnd;
// CITY WORLD - Level Entities
extern char *_levData_4_1_SegmentRomStart;
extern char *_levData_4_1_SegmentRomEnd;
extern char *_levData_4_2_SegmentRomStart;
extern char *_levData_4_2_SegmentRomEnd;
extern char *_levData_4_3_SegmentRomStart;
extern char *_levData_4_3_SegmentRomEnd;
extern char *_levData_4_4_SegmentRomStart;
extern char *_levData_4_4_SegmentRomEnd;
extern char *_levData_4_5_SegmentRomStart;
extern char *_levData_4_5_SegmentRomEnd;
extern char *_levData_4_6_SegmentRomStart;
extern char *_levData_4_6_SegmentRomEnd;
// CITY WORLD - Level Scripts
extern char *_scrData_4_1_SegmentRomStart;
extern char *_scrData_4_1_SegmentRomEnd;
extern char *_scrData_4_2_SegmentRomStart;
extern char *_scrData_4_2_SegmentRomEnd;
extern char *_scrData_4_3_SegmentRomStart;
extern char *_scrData_4_3_SegmentRomEnd;
extern char *_scrData_4_4_SegmentRomStart;
extern char *_scrData_4_4_SegmentRomEnd;
extern char *_scrData_4_5_SegmentRomStart;
extern char *_scrData_4_5_SegmentRomEnd;
extern char *_scrData_4_6_SegmentRomStart;
extern char *_scrData_4_6_SegmentRomEnd;

// SUBTERRANEAN WORLD - Objects ------------------------------------------------------------------
extern char *_objBank_5_0_SegmentRomStart;
extern char *_objBank_5_0_SegmentRomEnd;
extern char *_objBank_5_1_SegmentRomStart;
extern char *_objBank_5_1_SegmentRomEnd;
extern char *_objBank_5_2_SegmentRomStart;
extern char *_objBank_5_2_SegmentRomEnd;
extern char *_objBank_5_3_SegmentRomStart;
extern char *_objBank_5_3_SegmentRomEnd;
extern char *_objBank_5_4_SegmentRomStart;
extern char *_objBank_5_4_SegmentRomEnd;
extern char *_objBank_5_5_SegmentRomStart;
extern char *_objBank_5_5_SegmentRomEnd;
extern char *_objBank_5_6_SegmentRomStart;
extern char *_objBank_5_6_SegmentRomEnd;
// SUBTERRANEAN WORLD - Level Entities
extern char *_levData_5_1_SegmentRomStart;
extern char *_levData_5_1_SegmentRomEnd;
extern char *_levData_5_2_SegmentRomStart;
extern char *_levData_5_2_SegmentRomEnd;
extern char *_levData_5_3_SegmentRomStart;
extern char *_levData_5_3_SegmentRomEnd;
extern char *_levData_5_4_SegmentRomStart;
extern char *_levData_5_4_SegmentRomEnd;
extern char *_levData_5_5_SegmentRomStart;
extern char *_levData_5_5_SegmentRomEnd;
extern char *_levData_5_6_SegmentRomStart;
extern char *_levData_5_6_SegmentRomEnd;
// SUBTERRANEAN WORLD - Level Scripts
extern char *_scrData_5_1_SegmentRomStart;
extern char *_scrData_5_1_SegmentRomEnd;
extern char *_scrData_5_2_SegmentRomStart;
extern char *_scrData_5_2_SegmentRomEnd;
extern char *_scrData_5_3_SegmentRomStart;
extern char *_scrData_5_3_SegmentRomEnd;
extern char *_scrData_5_4_SegmentRomStart;
extern char *_scrData_5_4_SegmentRomEnd;
extern char *_scrData_5_5_SegmentRomStart;
extern char *_scrData_5_5_SegmentRomEnd;
extern char *_scrData_5_6_SegmentRomStart;
extern char *_scrData_5_6_SegmentRomEnd;

// LABORATORY WORLD - Objects --------------------------------------------------------------------
extern char *_objBank_6_0_SegmentRomStart;
extern char *_objBank_6_0_SegmentRomEnd;
extern char *_objBank_6_1_SegmentRomStart;
extern char *_objBank_6_1_SegmentRomEnd;
extern char *_objBank_6_2_SegmentRomStart;
extern char *_objBank_6_2_SegmentRomEnd;
extern char *_objBank_6_3_SegmentRomStart;
extern char *_objBank_6_3_SegmentRomEnd;
extern char *_objBank_6_4_SegmentRomStart;
extern char *_objBank_6_4_SegmentRomEnd;
extern char *_objBank_6_5_SegmentRomStart;
extern char *_objBank_6_5_SegmentRomEnd;
extern char *_objBank_6_6_SegmentRomStart;
extern char *_objBank_6_6_SegmentRomEnd;
// LABORATORY WORLD - Level Entities
extern char *_levData_6_1_SegmentRomStart;
extern char *_levData_6_1_SegmentRomEnd;
extern char *_levData_6_2_SegmentRomStart;
extern char *_levData_6_2_SegmentRomEnd;
extern char *_levData_6_3_SegmentRomStart;
extern char *_levData_6_3_SegmentRomEnd;
extern char *_levData_6_4_SegmentRomStart;
extern char *_levData_6_4_SegmentRomEnd;
extern char *_levData_6_5_SegmentRomStart;
extern char *_levData_6_5_SegmentRomEnd;
extern char *_levData_6_6_SegmentRomStart;
extern char *_levData_6_6_SegmentRomEnd;
// LABORATORY WORLD - Level Scripts
extern char *_scrData_6_1_SegmentRomStart;
extern char *_scrData_6_1_SegmentRomEnd;
extern char *_scrData_6_2_SegmentRomStart;
extern char *_scrData_6_2_SegmentRomEnd;
extern char *_scrData_6_3_SegmentRomStart;
extern char *_scrData_6_3_SegmentRomEnd;
extern char *_scrData_6_4_SegmentRomStart;
extern char *_scrData_6_4_SegmentRomEnd;
extern char *_scrData_6_5_SegmentRomStart;
extern char *_scrData_6_5_SegmentRomEnd;
extern char *_scrData_6_6_SegmentRomStart;
extern char *_scrData_6_6_SegmentRomEnd;

// TOYSHOP WORLD - Objects -----------------------------------------------------------------------
extern char *_objBank_7_0_SegmentRomStart;
extern char *_objBank_7_0_SegmentRomEnd;
extern char *_objBank_7_1_SegmentRomStart;
extern char *_objBank_7_1_SegmentRomEnd;
extern char *_objBank_7_2_SegmentRomStart;
extern char *_objBank_7_2_SegmentRomEnd;
extern char *_objBank_7_3_SegmentRomStart;
extern char *_objBank_7_3_SegmentRomEnd;
extern char *_objBank_7_4_SegmentRomStart;
extern char *_objBank_7_4_SegmentRomEnd;
extern char *_objBank_7_5_SegmentRomStart;
extern char *_objBank_7_5_SegmentRomEnd;
extern char *_objBank_7_6_SegmentRomStart;
extern char *_objBank_7_6_SegmentRomEnd;
// TOYSHOP WORLD - Level Entities
extern char *_levData_7_1_SegmentRomStart;
extern char *_levData_7_1_SegmentRomEnd;
extern char *_levData_7_2_SegmentRomStart;
extern char *_levData_7_2_SegmentRomEnd;
extern char *_levData_7_3_SegmentRomStart;
extern char *_levData_7_3_SegmentRomEnd;
extern char *_levData_7_4_SegmentRomStart;
extern char *_levData_7_4_SegmentRomEnd;
extern char *_levData_7_5_SegmentRomStart;
extern char *_levData_7_5_SegmentRomEnd;
extern char *_levData_7_6_SegmentRomStart;
extern char *_levData_7_6_SegmentRomEnd;
// TOYSHOP WORLD - Level Scripts
extern char *_scrData_7_1_SegmentRomStart;
extern char *_scrData_7_1_SegmentRomEnd;
extern char *_scrData_7_2_SegmentRomStart;
extern char *_scrData_7_2_SegmentRomEnd;
extern char *_scrData_7_3_SegmentRomStart;
extern char *_scrData_7_3_SegmentRomEnd;
extern char *_scrData_7_4_SegmentRomStart;
extern char *_scrData_7_4_SegmentRomEnd;
extern char *_scrData_7_5_SegmentRomStart;
extern char *_scrData_7_5_SegmentRomEnd;
extern char *_scrData_7_6_SegmentRomStart;
extern char *_scrData_7_6_SegmentRomEnd;

// HALLOWEEN WORLD - Objects ---------------------------------------------------------------------
extern char *_objBank_8_0_SegmentRomStart;
extern char *_objBank_8_0_SegmentRomEnd;
extern char *_objBank_8_1_SegmentRomStart;
extern char *_objBank_8_1_SegmentRomEnd;
extern char *_objBank_8_2_SegmentRomStart;
extern char *_objBank_8_2_SegmentRomEnd;
extern char *_objBank_8_3_SegmentRomStart;
extern char *_objBank_8_3_SegmentRomEnd;
extern char *_objBank_8_4_SegmentRomStart;
extern char *_objBank_8_4_SegmentRomEnd;
extern char *_objBank_8_5_SegmentRomStart;
extern char *_objBank_8_5_SegmentRomEnd;
extern char *_objBank_8_6_SegmentRomStart;
extern char *_objBank_8_6_SegmentRomEnd;
// HALLOWEEN WORLD - Level Entities
extern char *_levData_8_1_SegmentRomStart;
extern char *_levData_8_1_SegmentRomEnd;
extern char *_levData_8_2_SegmentRomStart;
extern char *_levData_8_2_SegmentRomEnd;
extern char *_levData_8_3_SegmentRomStart;
extern char *_levData_8_3_SegmentRomEnd;
extern char *_levData_8_4_SegmentRomStart;
extern char *_levData_8_4_SegmentRomEnd;
extern char *_levData_8_5_SegmentRomStart;
extern char *_levData_8_5_SegmentRomEnd;
extern char *_levData_8_6_SegmentRomStart;
extern char *_levData_8_6_SegmentRomEnd;
// HALLOWEEN WORLD - Level Scripts
extern char *_scrData_8_1_SegmentRomStart;
extern char *_scrData_8_1_SegmentRomEnd;
extern char *_scrData_8_2_SegmentRomStart;
extern char *_scrData_8_2_SegmentRomEnd;
extern char *_scrData_8_3_SegmentRomStart;
extern char *_scrData_8_3_SegmentRomEnd;
extern char *_scrData_8_4_SegmentRomStart;
extern char *_scrData_8_4_SegmentRomEnd;
extern char *_scrData_8_5_SegmentRomStart;
extern char *_scrData_8_5_SegmentRomEnd;
extern char *_scrData_8_6_SegmentRomStart;
extern char *_scrData_8_6_SegmentRomEnd;

// SUPERRETRO WORLD - Objects --------------------------------------------------------------------
extern char *_objBank_9_0_SegmentRomStart;
extern char *_objBank_9_0_SegmentRomEnd;
extern char *_objBank_9_1_SegmentRomStart;
extern char *_objBank_9_1_SegmentRomEnd;
extern char *_objBank_9_2_SegmentRomStart;
extern char *_objBank_9_2_SegmentRomEnd;
extern char *_objBank_9_3_SegmentRomStart;
extern char *_objBank_9_3_SegmentRomEnd;
extern char *_objBank_9_4_SegmentRomStart;
extern char *_objBank_9_4_SegmentRomEnd;
extern char *_objBank_9_5_SegmentRomStart;
extern char *_objBank_9_5_SegmentRomEnd;
extern char *_objBank_9_6_SegmentRomStart;
extern char *_objBank_9_6_SegmentRomEnd;
extern char *_objBank_9_7_SegmentRomStart;
extern char *_objBank_9_7_SegmentRomEnd;
extern char *_objBank_9_8_SegmentRomStart;
extern char *_objBank_9_8_SegmentRomEnd;
extern char *_objBank_9_9_SegmentRomStart;
extern char *_objBank_9_9_SegmentRomEnd;
extern char *_objBank_9_10_SegmentRomStart;
extern char *_objBank_9_10_SegmentRomEnd;
// SUPERRETRO WORLD - Level Entities
extern char *_levData_9_1_SegmentRomStart;
extern char *_levData_9_1_SegmentRomEnd;
extern char *_levData_9_2_SegmentRomStart;
extern char *_levData_9_2_SegmentRomEnd;
extern char *_levData_9_3_SegmentRomStart;
extern char *_levData_9_3_SegmentRomEnd;
extern char *_levData_9_4_SegmentRomStart;
extern char *_levData_9_4_SegmentRomEnd;
extern char *_levData_9_5_SegmentRomStart;
extern char *_levData_9_5_SegmentRomEnd;
extern char *_levData_9_6_SegmentRomStart;
extern char *_levData_9_6_SegmentRomEnd;
extern char *_levData_9_7_SegmentRomStart;
extern char *_levData_9_7_SegmentRomEnd;
extern char *_levData_9_8_SegmentRomStart;
extern char *_levData_9_8_SegmentRomEnd;
extern char *_levData_9_9_SegmentRomStart;
extern char *_levData_9_9_SegmentRomEnd;
extern char *_levData_9_10_SegmentRomStart;
extern char *_levData_9_10_SegmentRomEnd;
// SUPERRETRO WORLD - Level Scripts
extern char *_scrData_9_1_SegmentRomStart;
extern char *_scrData_9_1_SegmentRomEnd;
extern char *_scrData_9_2_SegmentRomStart;
extern char *_scrData_9_2_SegmentRomEnd;
extern char *_scrData_9_3_SegmentRomStart;
extern char *_scrData_9_3_SegmentRomEnd;
extern char *_scrData_9_4_SegmentRomStart;
extern char *_scrData_9_4_SegmentRomEnd;
extern char *_scrData_9_5_SegmentRomStart;
extern char *_scrData_9_5_SegmentRomEnd;
extern char *_scrData_9_6_SegmentRomStart;
extern char *_scrData_9_6_SegmentRomEnd;
extern char *_scrData_9_7_SegmentRomStart;
extern char *_scrData_9_7_SegmentRomEnd;
extern char *_scrData_9_8_SegmentRomStart;
extern char *_scrData_9_8_SegmentRomEnd;
extern char *_scrData_9_9_SegmentRomStart;
extern char *_scrData_9_9_SegmentRomEnd;
extern char *_scrData_9_10_SegmentRomStart;
extern char *_scrData_9_10_SegmentRomEnd;



//----- [ DEFINES ] -----------------------------------------------------------//


#define	STACKSIZE		0x4000
#define	SMALLSTACKSIZE	0x400
#define MAXTASKS		20

// microcode types
enum
{
	UCODE_POLY,
	UCODE_LINE,
	UCODE_SPRITE,
};


#define V(p, x, y, z, f, s, t, r, g, b, a) {                            \
        (p)->v.ob[0] = (x);                                             \
        (p)->v.ob[1] = (y);                                             \
        (p)->v.ob[2] = (z);                                             \
        (p)->v.flag  = (f);                                             \
        (p)->v.tc[0] = (s);                                             \
        (p)->v.tc[1] = (t);                                             \
        (p)->v.cn[0] = (r);                                             \
        (p)->v.cn[1] = (g);                                             \
        (p)->v.cn[2] = (b);                                             \
        (p)->v.cn[3] = (a);                                             \
}

#define	SCREEN_HT			240
#define SCREEN_WD_320		320
#define SCREEN_WD_MIN		320
#define SCREEN_WD_MAX		640
#define SCREEN_WD			SCREEN_WD_320

#define XSCALE_MAX			0x400
#define MAXCONTROLLERMODE	4

#define XMIN				-5
#define XMAX				315
#define YMIN				40
#define YMAX				230
#define ZMIN				0
#define ZMAX				600

#define EYEZPOS				475

#define MAXCLIPRATIO		6

#define RDP_OUTPUT_BUF		0x8004b400		// End of Z buffer
#define COLOR_BUFFER		0x8016a000

#define RDP_OUTPUT_BUF_LEN	(4096)

#define RDP_OUTBUF_SIZE_MIN 0x100			// Min fifo buffer size

#define RDP_BUFFER_LEN		43000

#define PRIMCOLOR			0
#define GOURAUD				1
#define LIGHTING			2

#define	GLIST_LEN			20000			// Large enough to handle enough polys

#define TXTR_NONE			0
#define TXTR_ON				1
#define TXTR_ON_MM			2

#define MAX_MATRICES		64*8


//-----------------------------------------------------------------------------//

/*
 * ifdef needed because this file is included by "spec"
 */
#ifdef _LANGUAGE_C

// Layout of dynamic data.
/*
  This structure holds the things which change per frame. It is advantageous
  to keep dynamic data together so that we may selectively write back dirty
  data cache lines to DRAM prior to processing by the RCP.
*/

typedef struct
{
	Mtx	projection[4];

	Mtx TranslateOut;

	Mtx modelling;
	Mtx modeling4[MAX_MATRICES];
    
	Mtx viewing[4];
	Mtx noViewing;
	Mtx noProjection;
	Mtx identity;
	Gfx glist[GLIST_LEN];

	LookAt lookat[4];
	Hilite hilite[4];

	Vp vp[4];

} Dynamic;

typedef union
{
	struct { short type; } gen;
    struct { short type; } done;
    
	NNScMsg		app;

} GFXMsg;

extern Dynamic				dynamic[];
extern Dynamic				*dynamicp;
extern int					actualGXFListLength[];

extern unsigned short		cfb_16_a[];
extern unsigned short		cfb_16_b[];
extern void					*cfb_ptrs[2];
extern unsigned short		zbuffer[];

extern char					musicPriority;

extern char					draw_buffer;
extern char					codeDrawingRequest;
extern char					gfxIsDrawing;
extern char					disableGraphics;
extern char					onlyDrawGraphics;
extern char					onlyDrawBackdrops;
extern char					displayOverlays;

extern char					numtasks;
extern char					currentTask;
extern NNSched				sched;

extern unsigned long		actFrameCount,lastActFrameCount,currentFrameTime;

extern OSThread				mainThread;
extern OSThread				drawGraphicsThread;
extern OSThread				ControllerThread;

extern u64					rdp_output[];
extern u64					rdp_output_len;

extern float				theta;

extern char					ActiveController;
extern char					ShadingMode;
extern int					UseGlobalTransforms;
extern int					ChangeVideoModes;

extern int					ScreenWidth;
extern int					ScreenWidthDelta;
extern int					UseUCode;
extern int					OutLen;

extern char					mirrorTextures;
extern char					runningIntro;

extern short				farPlaneDist;
extern short				nearPlaneDist;

extern						Vtx font_vtx[];

extern OSMesgQueue			dmaMessageQ, rspMessageQ, rdpMessageQ, retraceMessageQ;
extern OSMesg				dmaMessageBuf, rspMessageBuf, rdpMessageBuf, retraceMessageBuf;
extern OSMesg				dummyMessage;
extern OSIoMesg				dmaIOMessageBuf;
extern OSMesg				dummyMesg;
extern OSIoMesg				dmaIOMessageBuf;

extern OSMesgQueue			main_msgQ;

extern unsigned int			TicksPerFrame;

extern int gfxTasks;

extern void					ClearViewing();
extern void					DrawGraphics(void *arg);
extern void					SetRenderMode();
extern void					SetupViewing();

extern int dontClearScreen;

extern short dontDoAnims;

extern char codeRunning;

void ControllerProc(void *arg);
void InitDisplayLists(void);
void ComputeClockSpeed();

int strncmp(const char *a,char *b,int len);


// MEMORY CHECKING & DEBUGGING ROUTINES - ANDYE - CURRENTLY SPECIFIC TO N64 ----------------------

enum
{
	MEM_SHOW_INGAMEINFO = 1,
};

float ConvertKb(unsigned long op1);

int GetNumStaticBlocksUsed();
int GetNumDynamicBlocksUsed();
unsigned long GetBaseAddrMemoryArea();
long GetSizeOfMemoryArea();
unsigned long GetBytesUsedForStatic();
unsigned long GetBytesUsedForDynamic();
unsigned long GetBytesUsedForUniques();
unsigned long GetBytesUsed(char *search);

void ShowMemorySituation(char info);


#endif	/* _LANGUAGE_C */

#endif	/* __BLOCK_H__ */
