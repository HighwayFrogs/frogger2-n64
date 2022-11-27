/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: textoverlays.h
	Programmer	: Andy Eder
	Date		: 16/04/99 09:31:49

----------------------------------------------------------------------------------------------- */

#ifndef TEXTOVERLAYS_H_INCLUDED
#define TEXTOVERLAYS_H_INCLUDED


//----- [ DEFINES / FLAGS ] --------------------------------------------------------------------//

#define TEXTOVERLAY_NORMAL			0
#define TEXTOVERLAY_WAVEALL			(1 << 0)
#define TEXTOVERLAY_WAVECHARS		(1 << 1)
#define TEXTOVERLAY_WAVEDECAY		(1 << 2)
#define TEXTOVERLAY_LIFE			(1 << 3)


//----- [ GLOBALS ] ----------------------------------------------------------------------------//

typedef struct TAGTEXTOVERLAYLIST
{
	int				numEntries;
	TEXTOVERLAY		head;

} TEXTOVERLAYLIST;

extern TEXTOVERLAYLIST textOverlayList;


//----- [ FUNCTION PROTOTYPES ] ----------------------------------------------------------------//

extern TEXTOVERLAY *CreateAndAddTextOverlay(float x,float y,char *text,char centred,char a,FONT *fontPtr,char flags,float waveAmplitude);
extern void PrintTextOverlays();
extern void EnableTextOverlay(TEXTOVERLAY *tover);
extern void DisableTextOverlay(TEXTOVERLAY *tover);

extern void InitTextOverlayLinkedList();
extern void FreeTextOverlayLinkedList();
extern void AddTextOverlay(TEXTOVERLAY *tOver);
extern void SubTextOverlay(TEXTOVERLAY *tOver);


#endif
