/*

	This file is part of Frogger2, (c) 1997 Interactive Studios Ltd.


	File		: l10n.h
	Programmer	: David Swift (from $/Tools/PSXLibraries/ISLLocal)
	Date		: 23/03/99

----------------------------------------------------------------------------------------------- */

#ifndef _L10N_H_INCLUDED
#define _L10N_H_INCLUDED

typedef enum {
	LANG_ENGLISH,
	LANG_FRENCH,
	LANG_GERMAN,
	LANG_SPANISH,
	LANG_ITALIAN,
	LANG_SWEDIAN,
	NUM_LANGUAGES
} LANGUAGE;


// use this macro to access the string you want
#define TEXTSTR(n) (gameTextStr[n])

extern char	**gameTextStr;

void gameTextInit(char *fName, int numStrings, int numLang, int currLang);
void gameTextDestroy();

#endif