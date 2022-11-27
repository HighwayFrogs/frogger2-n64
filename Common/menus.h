/*

	This file is part of Frogger2, (c) 1997 Interactive Studios Ltd.


	File		: menus.h
	Programmer	: James (Random) Healey
	Date		: 27/05/99

----------------------------------------------------------------------------------------------- */

#ifndef MENUS_H_INCLUDED
#define MENUS_H_INCLUDED


extern SPRITEOVERLAY *atari,*konami,*flogo[10], *islLogo[3];

void RunPauseMenu();
void FreeMenuItems();

void Modify3DText(TEXT3D *t3d, char *str,unsigned char alpha);
void RunCredits();
void DeactivateCredits();
void ActivateCredits();
void InitCredits();


#endif
