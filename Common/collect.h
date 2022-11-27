/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: collect.h
	Programmer	: Andrew Eder
	Date		: 11/27/98

----------------------------------------------------------------------------------------------- */

#ifndef COLLECT_H_INCLUDED
#define COLLECT_H_INCLUDED

// ----- [ DEFINES ] --------------- //

#define PICKUP_RADIUS			25.0F
#define PICKUP_RADIUS_SQUARED	625.0F

enum
{
	SPAWN_GARIB,
	EXTRAHEALTH_GARIB,
	EXTRALIFE_GARIB,
	AUTOHOP_GARIB,
	LONGTONGUE_GARIB,
	QUICKHOP_GARIB,
	INVULNERABILITY_GARIB,

	NUM_GARIB_TYPES
};

typedef struct TAGGARIB
{
	struct TAGGARIB		*prev,*next;
	BYTE				active;
	UBYTE				type;
	float				scale;
	float				scaleAim;
	SPRITE				*sprite;
	SPECFX				*fx;
	VECTOR				pos;
	unsigned long		flags;
	GAMETILE			*gameTile;
	float				dropSpeed;

} GARIB;

typedef struct TAGGARIBLIST
{
	GARIB				head;
	short				count, total;

} GARIBLIST;


extern GARIBLIST garibList;


// ----- [ FUNCTION PROTOTYPES ] --------------- //

void CheckTileForCollectable(GAMETILE *tile,long pl);

void ProcessCollectables();
void PickupCollectable(GARIB *garib, int pl);

void InitGaribList();
void FreeGaribList();
void AddGarib(GARIB *garib);
void SubGarib(GARIB *garib);

void InitGaribSprite(GARIB *garib);
GARIB *CreateNewGarib(VECTOR pos, int type);
void UpdateGaribs();

void DropGaribToTile(GARIB *garib, GAMETILE *tile, float dropSpeed);

#endif
