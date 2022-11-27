/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: water.h
	Programmer	: Andy Eder
	Date		: 20/10/99 16:46:53

----------------------------------------------------------------------------------------------- */

#ifndef WATER_H_INCLUDED
#define WATER_H_INCLUDED


#ifdef N64_VERSION

#define MAX_N64_WATEROBJECTS	8
#define MAX_N64_MODGYTEXOBJECTS	8

extern ACTOR2 *watActor;

extern short numN64WaterObjects;
extern short currN64WaterObject;
extern SHORT2DVECTOR *wTC[MAX_N64_WATEROBJECTS];

extern short numN64ModgyTexObjects;
extern short currN64ModgyTexObject;
extern SHORT2DVECTOR *mTC[MAX_N64_MODGYTEXOBJECTS];



void RunWaterDemo();

void UpdateWaterN64(ACTOR2 *wAct);
void SetWaterModifiersN64(short worldID,short levelID);
void AddN64WaterObjectResource(ACTOR *wAct);
void FreeN64WaterResources();

void UpdateModgyTexN64(ACTOR2 *mAct);
void AddN64ModgyTexObjectResource(ACTOR *mAct);
void FreeN64ModgyTexResources();

#endif


#endif
