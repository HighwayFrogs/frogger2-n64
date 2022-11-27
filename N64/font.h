/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: font.h
	Programmer	: Andrew Eder
	Date		: 10/28/98

----------------------------------------------------------------------------------------------- */

#ifndef FONT_H_INCLUDED
#define FONT_H_INCLUDED


#define MAX_FONT_CHARS	48							// Number of characters in our font - letters,numbers,etc.

//----- [ DATA STRUCTURES ] --------------------------------------------------------------------//

typedef struct TAGFONT
{
	unsigned char *data;							// Ptr to our 'incbin' data (see link file)
	unsigned long offset[MAX_FONT_CHARS];			// Offset of font chars in data chunk
	unsigned long palOffset;						// Offset of palette in data chunk
		
	unsigned long width;							// Width of our font characters (e.g. 16x16, 32x32, etc.)
	unsigned long height;							// Height of our font characters (e.g. 16x16, 32x32, etc.)

	unsigned long xSpacing[MAX_FONT_CHARS];			// X spacing for our individual font characters
	unsigned long ySpacing[MAX_FONT_CHARS];			// Y spacing for our individual font characters

} FONT;


typedef struct _TEXTLIST2
{
	const char *text;
	unsigned long x,y;
	unsigned long sinAmp,sinSpd,sinStart,sinEnd;
	float flashSpd;
	unsigned long kill;
	struct _TEXTLIST2 *next;
} TEXTLIST2;

extern TEXTLIST2 *textList;

extern FONT *bigFont;
extern FONT *smallFont;
extern FONT *oldeFont;
extern FONT	*currFont;

// ----- [ FONT PRINTING DATA ] ----- //

extern unsigned char characterMap[256];

extern unsigned long letters16_offset[];
extern unsigned long letters8_offset[];

// ----- [ FUNCTION PROTOTYPES ] ----- //

void InitFont();


#endif
