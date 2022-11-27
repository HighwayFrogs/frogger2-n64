/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: tongue.c
	Programmer	: Andrew Eder
	Date		: 12/15/98

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>

#include "incs.h"


TONGUE tongue[MAX_FROGS];

void StartTongue( unsigned char type, VECTOR *dest, int pl );
void CalculateTongue( int pl );


/*	--------------------------------------------------------------------------------
	Function		: InitTongues
	Purpose			: Set all tongue data to nothing
	Parameters		: 
	Returns			: 
	Info			: 
*/
void InitTongues( )
{
	long i;

	for( i=0; i<4; i++ )
	{
		tongue[i].flags = TONGUE_NONE | TONGUE_IDLE;
		tongue[i].thing = NULL;
		tongue[i].radius = TONGUE_RADIUSNORMAL;
		tongue[i].segment = NULL;
		tongue[i].tex = NULL;
		tongue[i].type = 0;
		tongue[i].canTongue = 1;
	}
}


/*	--------------------------------------------------------------------------------
	Function		: FreeTongues
	Purpose			: Delete allocated data
	Parameters		: 
	Returns			: 
	Info			: 
*/
void FreeTongues( )
{
	long i;

	for( i=0; i<4; i++ )
	{
		tongue[i].flags = TONGUE_NONE | TONGUE_IDLE;
		tongue[i].thing = NULL;
		tongue[i].radius = TONGUE_RADIUSNORMAL;

		if( tongue[i].segment ) JallocFree( (UBYTE **)&tongue[i].segment );
		tongue[i].segment = NULL;
		tongue[i].tex = NULL;
		tongue[i].type = 0;
		tongue[i].canTongue = 1;
	}
}


/*	--------------------------------------------------------------------------------
	Function		: StartTongue
	Purpose			: initialises Frogger's tongue for picking up, throwing, etc.
	Parameters		: char, dest, player
	Returns			: 
	Info			: contains initialisation data for shooting out the tongue
*/
void StartTongue( unsigned char type, VECTOR *dest, int pl )
{
	float dp;
	VECTOR to;
	int i, no=0;

	if( !tongue[pl].canTongue || !player[pl].canJump ) return;

	PlaySample(genSfx[GEN_FROG_TONGUE],&frog[pl]->actor->pos,0,SAMPLE_VOLUME,-1/*64*/);

	// Determine frog's forward vector
	RotateVectorByQuaternion( &tongue[pl].fwd, &inVec, &frog[pl]->actor->qRot );

	// Set source, including forward and up offsets
	tongue[pl].source.v[X] = frog[pl]->actor->pos.v[X] + (tongue[pl].fwd.v[X] * TONGUE_OFFSET_FORWARD) - (currTile[pl]->normal.v[X] * TONGUE_OFFSET_UP);
	tongue[pl].source.v[Y] = frog[pl]->actor->pos.v[Y] + (tongue[pl].fwd.v[Y] * TONGUE_OFFSET_FORWARD) - (currTile[pl]->normal.v[Y] * TONGUE_OFFSET_UP);
	tongue[pl].source.v[Z] = frog[pl]->actor->pos.v[Z] + (tongue[pl].fwd.v[Z] * TONGUE_OFFSET_FORWARD) - (currTile[pl]->normal.v[Z] * TONGUE_OFFSET_UP);

	SetVector( &tongue[pl].pos, &tongue[pl].source );

	// Calculate the vector to the collectable item and its magnitude
	SetVector( &tongue[pl].target, dest );

	SubVector( &to, &tongue[pl].target, &tongue[pl].source );
	MakeUnit( &to );

	tongue[pl].progress = 0;

	if( type == TONGUE_GET_FROG )
	{
		int i;
		for(i=0; i<MAX_FROGS; i++) if( frog[i] == (ACTOR2 *)tongue[pl].thing ) break;

		if( i!=MAX_FROGS && ((player[i].frogunder != (char)-1) || (player[i].frogon != (char)-1) || (tongue[i].flags & TONGUE_BEINGUSED)) ) no=1;
	}

	// Calculate angle through which the tongue needs to rotate
	dp = DotProduct( &tongue[pl].fwd, &to );
	if( dp<TONGUE_WRAPAROUNDTHRESHOLD || no )
	{
		tongue[pl].flags = TONGUE_NONE | TONGUE_IDLE;
		tongue[pl].thing = NULL;
		return;
	}

	tongue[pl].type = type;
	tongue[pl].tex = txtrBlank;

	if( !tongue[pl].segment )
		tongue[pl].segment = (VECTOR *)JallocAlloc( sizeof(VECTOR)*MAX_TONGUENODES, YES, "TSegments" );

	for( i=0; i<MAX_TONGUENODES; i++ )
	{
		SetVector( &tongue[pl].segment[i], &tongue[pl].pos );
	}

	// Set frog mouth open animation, and the speed
	frog[pl]->actor->animation->animTime = 0;
	AnimateActor( frog[pl]->actor, FROG_ANIM_USINGTONGUE, NO, NO, 0.5F, 0, 0 );

	tongue[pl].flags = TONGUE_NONE | TONGUE_BEINGUSED | TONGUE_OUTGOING;
	tongue[pl].canTongue = 0;

	if( (tongue[pl].type == TONGUE_GET_FROG) && (i!= MAX_FROGS) )
	{
		tongue[i].canTongue = 0;
		player[i].canJump = 0;
	}

	PlaySample(genSfx[GEN_FROG_TONGUE],&frog[pl]->actor->pos,0,100-Random(15),100-Random(15));
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateFrogTongue
	Purpose			: updates Frogger's tongue for collecting collectables
	Parameters		: none
	Returns			: 
	Info			:
*/
void UpdateFrogTongue( int pl )
{
	int i;
	// Determine tongue state and perform relevant action / update

	if( tongue[pl].flags & TONGUE_IDLE )
		return;

	// Frogs tongue is ready for action
	if( tongue[pl].flags & TONGUE_BEINGUSED )
	{
		// Check if tongue can actually be used
		if( tongue[pl].flags & TONGUE_HASITEMINMOUTH )
			return;

		// Update the forward vector
		RotateVectorByQuaternion( &tongue[pl].fwd, &inVec, &frog[pl]->actor->qRot );
		// Update source (for frog on platform)
		tongue[pl].source.v[X] = frog[pl]->actor->pos.v[X] + (tongue[pl].fwd.v[X] * TONGUE_OFFSET_FORWARD) - (currTile[pl]->normal.v[X] * TONGUE_OFFSET_UP);
		tongue[pl].source.v[Y] = frog[pl]->actor->pos.v[Y] + (tongue[pl].fwd.v[Y] * TONGUE_OFFSET_FORWARD) - (currTile[pl]->normal.v[Y] * TONGUE_OFFSET_UP);
		tongue[pl].source.v[Z] = frog[pl]->actor->pos.v[Z] + (tongue[pl].fwd.v[Z] * TONGUE_OFFSET_FORWARD) - (currTile[pl]->normal.v[Z] * TONGUE_OFFSET_UP);
		// Update the targets position (for moving babies)
		switch( tongue[pl].type )
		{
		case TONGUE_GET_BABY: SetVector( &tongue[pl].target, &((ENEMY *)tongue[pl].thing)->nmeActor->actor->pos ); break;
		case TONGUE_GET_SCENIC: SetVector( &tongue[pl].target, &((ACTOR2 *)tongue[pl].thing)->actor->pos ); break;
		case TONGUE_GET_FROG: SetVector( &tongue[pl].target, &((ACTOR2 *)tongue[pl].thing)->actor->pos ); break;
		case TONGUE_GET_GARIB: SetVector( &tongue[pl].target, &((GARIB *)tongue[pl].thing)->pos ); break;
		case TONGUE_GET_HEALTH: SetVector( &tongue[pl].target, &((GARIB *)tongue[pl].thing)->fx->act[0]->actor->pos ); break;
		}

		if( tongue[pl].flags & TONGUE_OUTGOING )
		{
			// If tongue has not got to the target
			if( DistanceBetweenPointsSquared(&tongue[pl].target,&tongue[pl].interp) > TONGUE_STICKYRADIUS*TONGUE_STICKYRADIUS )
			{
				// Extend the tongue a bit more
				tongue[pl].progress += TONGUE_FRACTION*gameSpeed*(1.5-tongue[pl].progress);
				if( tongue[pl].progress > 1 ) tongue[pl].progress = 1;
				LinearInterp( &tongue[pl].interp, &tongue[pl].source, &tongue[pl].target, tongue[pl].progress );
				CalculateTongue( pl );

				// If we have gone too far
				if( DistanceBetweenPointsSquared(&tongue[pl].source,&tongue[pl].pos) > tongue[pl].radius*tongue[pl].radius )
				{
					tongue[pl].flags = TONGUE_NONE | TONGUE_BEINGUSED | TONGUE_INCOMING;
					return;
				}
			}
			else // Got to target - return with item
			{
				tongue[pl].flags = TONGUE_NONE | TONGUE_BEINGUSED | TONGUE_INCOMING | TONGUE_HASITEMONIT;
				return;
			}
		}
		
		if(tongue[pl].flags & TONGUE_INCOMING)
		{
			if( tongue[pl].flags & TONGUE_HASITEMONIT )
			{
				if(tongue[pl].type == TONGUE_GET_GARIB)
				{
					GARIB *g = (GARIB *)tongue[pl].thing;
					SetVector( &g->pos, &tongue[pl].pos );
					g->scale *= 0.9;
				}
				else if(tongue[pl].type == TONGUE_GET_HEALTH)
				{
					GARIB *g = (GARIB *)tongue[pl].thing;
					SetVector( &g->fx->act[0]->actor->pos, &tongue[pl].pos );
				}
				else if( tongue[pl].type == TONGUE_GET_BABY )
				{
					ENEMY *e = (ENEMY *)tongue[pl].thing;
					SetVector( &e->nmeActor->actor->pos, &tongue[pl].pos );
					e->nmeActor->actor->scale.v[X] *= 0.9;
					e->nmeActor->actor->scale.v[Y] *= 0.9;
					e->nmeActor->actor->scale.v[Z] *= 0.9;
				}
				else if( tongue[pl].type == TONGUE_GET_FROG )
				{
					SetVector( &((ACTOR2 *)tongue[pl].thing)->actor->pos, &tongue[pl].pos );
				}
				else if( tongue[pl].type == TONGUE_GET_SCENIC )
				{
					ACTOR2 *s = (ACTOR2 *)tongue[pl].thing;
					s->actor->pos.v[X] += (-3 + Random(7));
					s->actor->pos.v[Y] += (-3 + Random(7));
					s->actor->pos.v[Z] += (-3 + Random(7));
				}
			}

			// Retract tongue a bit
			tongue[pl].progress -= TONGUE_FRACTION*gameSpeed*(1.5-tongue[pl].progress);
			if( tongue[pl].progress < 0 ) tongue[pl].progress = 0;
			LinearInterp( &tongue[pl].interp, &tongue[pl].source, &tongue[pl].target, tongue[pl].progress );
			CalculateTongue( pl );

			// If tongue has got back to mouth
			if( DistanceBetweenPointsSquared(&tongue[pl].source,&tongue[pl].interp) < TONGUE_STICKYRADIUS*TONGUE_STICKYRADIUS )
			{
				if( tongue[pl].flags & TONGUE_HASITEMONIT )
				{
					if( tongue[pl].type == TONGUE_GET_GARIB || tongue[pl].type == TONGUE_GET_HEALTH )
						PickupCollectable( (GARIB *)tongue[pl].thing, pl );
//					else if( tongue[pl].type == TONGUE_GET_BABY )
//						PickupBabyFrogMulti( (ENEMY *)tongue[pl].thing, pl );
					else if( tongue[pl].type == TONGUE_GET_FROG ) // Throw frog as far as he will go
					{
						int dir;
						// Find frog based on actor address
						for(i=0; i<MAX_FROGS; i++) if( frog[i] == (ACTOR2 *)tongue[pl].thing ) break;
						// Chuck the frog according to control direction
						if( controllerdata[pl].button & CONT_UP ) dir = MOVE_UP;
						else if( controllerdata[pl].button & CONT_RIGHT ) dir = MOVE_RIGHT;
						else if( controllerdata[pl].button & CONT_DOWN ) dir = MOVE_DOWN;
						else if( controllerdata[pl].button & CONT_LEFT ) dir = MOVE_LEFT;
						else
						{
							player[i].canJump = 0;
							if( !(controllerdata[pl].button & CONT_B) ) RemoveFrogTongue(pl);
							return;
						}
						
						// Throw the frog - no longer a frog on our tongue, so don't try to remove it later
						if( i != MAX_FROGS )
						{
							ThrowFrogDirection( pl, i, (dir+camFacing[pl]) &3 );
							tongue[pl].flags &= ~TONGUE_HASITEMONIT;
							tongue[i].canTongue = 1;
						}
					}
					else if( tongue[pl].type == TONGUE_GET_SCENIC )
						SetVector( &((ACTOR2 *)tongue[pl].thing)->actor->pos, &((ACTOR2 *)tongue[pl].thing)->actor->oldpos );
				}

				RemoveFrogTongue(pl);

				// Set frog idle animation
				frog[pl]->actor->animation->animTime = 0;
				AnimateActor(frog[pl]->actor,FROG_ANIM_BREATHE,YES,YES,0.5F,0,0);
			}
		}
	}

	if( tongue[pl].flags & TONGUE_SEARCHING )
	{
		// first priority - check if baby frog is in range
//		if( tongue[pl].thing = (void *)FrogIsInRange(tongue[pl].radius,pl) )
//			StartTongue( TONGUE_GET_FROG, &((ACTOR2 *)tongue[pl].thing)->actor->pos, pl );
//		else if( gameState.multi != SINGLEPLAYER && (tongue[pl].thing = (void *)BabyFrogIsInRange(tongue[pl].radius,pl)) )
//			StartTongue( TONGUE_GET_BABY, &((ENEMY *)tongue[pl].thing)->nmeActor->actor->pos, pl );
		if( tongue[pl].thing = (void *)GaribIsInRange(tongue[pl].radius,pl) )
		{
			if( ((GARIB *)tongue[pl].thing)->type == EXTRAHEALTH_GARIB )
				StartTongue( TONGUE_GET_HEALTH, &((GARIB *)tongue[pl].thing)->fx->act[0]->actor->pos, pl );
			else
				StartTongue( TONGUE_GET_GARIB, &((GARIB *)tongue[pl].thing)->pos, pl );
		}
		else if( tongue[pl].thing = (void *)ScenicIsInRange(tongue[pl].radius,pl) )
			StartTongue( TONGUE_GET_SCENIC, &((ACTOR2 *)tongue[pl].thing)->actor->pos, pl );
		else
			tongue[pl].flags = TONGUE_NONE | TONGUE_IDLE;
	}
}


/*	--------------------------------------------------------------------------------
	Function		: CalculateTongue
	Purpose			: Plot a Hermite curve based on the start and end points
	Parameters		: 
	Returns			: 
	Info			: 
*/
void CalculateTongue( int pl )
{
	VECTOR to;
	float gx[4], gy[4], gz[4], cx[4], cy[4], cz[4], t=0, t2, t3;
	int n, i, index;

	n = MAX_TONGUENODES; // Should probably be worked out by fraction of max radius covered

	SubVector( &to, &tongue[pl].target, &tongue[pl].source );

	// Set geometry components
	gx[0] = tongue[pl].source.v[X];
	gx[1] = tongue[pl].interp.v[X];
	gx[2] = tongue[pl].fwd.v[X]*200;
	gx[3] = to.v[X];
	gy[0] = tongue[pl].source.v[Y];
	gy[1] = tongue[pl].interp.v[Y];
	gy[2] = tongue[pl].fwd.v[Y]*200;
	gy[3] = to.v[Y];
	gz[0] = tongue[pl].source.v[Z];
	gz[1] = tongue[pl].interp.v[Z];
	gz[2] = tongue[pl].fwd.v[Z]*200;
	gz[3] = to.v[Z];

	index = (int)(tongue[pl].progress*(MAX_TONGUENODES-1));

	for( i=0; i<=index; i++ )
	{
		t += TONGUE_FRACTION;
		t2 = t*t;
		t3 = t2*t;

		tongue[pl].segment[i].v[X] = ((2*t3 - 3*t2 + 1)*gx[0]) + ((-2*t3 + 3*t2)*gx[1]) + ((t3 - 2*t2 + t)*gx[2]) + ((t2-t2)*gx[3]);
		tongue[pl].segment[i].v[Y] = ((2*t3 - 3*t2 + 1)*gy[0]) + ((-2*t3 + 3*t2)*gy[1]) + ((t3 - 2*t2 + t)*gy[2]) + ((t2-t2)*gy[3]);
		tongue[pl].segment[i].v[Z] = ((2*t3 - 3*t2 + 1)*gz[0]) + ((-2*t3 + 3*t2)*gz[1]) + ((t3 - 2*t2 + t)*gz[2]) + ((t2-t2)*gz[3]);
	}

	SetVector( &tongue[pl].pos, &tongue[pl].segment[index] );
}


/*	--------------------------------------------------------------------------------
	Function		: RemoveFrogTongue
	Purpose			: removes the frog tongue
	Parameters		: 
	Returns			: 
	Info			: 
*/
void RemoveFrogTongue( int pl )
{
	int i;

	dprintf"Remove TONGUE\n"));

	if( tongue[pl].flags & TONGUE_HASITEMONIT )
	{
		if( tongue[pl].type == TONGUE_GET_BABY )
		{
			ENEMY *baby = (ENEMY *)tongue[pl].thing;
			baby->nmeActor->actor->scale.v[X] = 1;
			baby->nmeActor->actor->scale.v[Y] = 1;
			baby->nmeActor->actor->scale.v[Z] = 1;
		}
		else if( tongue[pl].type == TONGUE_GET_FROG )
		{
			int i;
			for(i=0; i<MAX_FROGS; i++) if( frog[i] == (ACTOR2 *)tongue[pl].thing ) break;
			if( i != MAX_FROGS )
			{
				tongue[i].canTongue = 1;
				player[i].canJump = 1;
				TeleportActorToTile( frog[i], FindNearestJoinedTile(currTile[pl],&frog[i]->actor->pos), i );
			}
		}
	}

	tongue[pl].flags = TONGUE_NONE | TONGUE_IDLE;
	tongue[pl].thing = NULL;
	tongue[pl].radius = TONGUE_RADIUSNORMAL;
	tongue[pl].type = 0;
	tongue[pl].progress = 0;
	tongue[pl].canTongue = 1;

	player[pl].frogState &= ~FROGSTATUS_ISTONGUEING;
}


/*	--------------------------------------------------------------------------------
	Function		: BabyFrogIsInTongueRange
	Purpose			: indicates if a baby frog is in range when tongueing
	Parameters		: 
	Returns			: 
	Info			: returns ptr to the nearest baby frog (if in range)
*/
ENEMY *BabyFrogIsInRange( float radius, int pl )
{
	ACTOR2 *act,*nearest;
	ACTOR2 *inRange[4];
	ENEMY *nme;
	float dist,mags[4];
	int i = 0,numInRange = 0;

	if(numBabies)
	{
		for(i=0; i<numBabies; i++)
		{
			if( babyList[i].baby )
			{
				dist = DistanceBetweenPointsSquared(&frog[pl]->actor->pos,&babyList[i].baby->actor->pos);

				if((!babyList[i].isSaved) && (dist < (radius * radius)))
				{
					mags[numInRange]		= dist;
					inRange[numInRange++]	= babyList[i].baby;
				}
			}
		}

		if(numInRange)
		{
			// return closest baby frog
			dist	= mags[0];
			nearest	= inRange[0];
			for(i=1; i<numInRange; i++)
			{
				if(mags[i] < dist)
				{
					dist	= mags[i];
					nearest	= inRange[i];
				}
			}

			for( nme = enemyList.head.next; nme != &enemyList.head; nme = nme->next )
				if( nme->nmeActor == nearest )
					break;

			return nme;
		}
	}

	// no baby frog in range
	return NULL;
}


/*	--------------------------------------------------------------------------------
	Function		: GaribIsInRange
	Purpose			: indicates if a garib is in range when tongueing
	Parameters		: 
	Returns			: GARIB *
	Info			: returns ptr to the nearest garib (if in range)
*/
GARIB *GaribIsInRange( float radius, int pl )
{
	GARIB *garib,*nearest;
	GARIB *inRange[8];
	VECTOR pos;
	float dist,mags[8];
	int i = 0,numInRange = 0;
		
	for(garib = garibList.head.next; garib != &garibList.head; garib = garib->next)
	{
		// only check for garibs in visual range
		SetVector( &pos, ((garib->type == EXTRAHEALTH_GARIB && garib->fx)?(&garib->fx->act[0]->actor->pos):(&garib->pos)) );
		dist = DistanceBetweenPointsSquared(&frog[pl]->actor->pos,&pos);

		if( dist > (radius * radius) )
			continue;

		if( (garib->active) && (numInRange < 8))
		{
			mags[numInRange]		= dist;
			inRange[numInRange++]	= garib;
		}
	}

	if(numInRange)
	{
		// return closest item
		dist	= mags[0];
		nearest	= inRange[0];
		for(i=1; i<numInRange; i++)
		{
			if(mags[i] < dist)
			{
				dist	= mags[i];
				nearest	= inRange[i];
			}
		}

		return nearest;
	}

	return NULL;
}


/*	--------------------------------------------------------------------------------
	Function		: ScenicIsInTongueRange
	Purpose			: indicates if a scenic is in range when tongueing
	Parameters		: 
	Returns			: ACTOR2 *
	Info			: returns ptr to the nearest scenic (if in range)
*/
ACTOR2 *ScenicIsInRange( float radius, int pl )
{
	PLATFORM *cur,*next,*nearest;
	PLATFORM *inRange[4];
	float dist,mags[4];
	int i = 0,numInRange = 0;
		
	for(cur = platformList.head.next; cur != &platformList.head; cur = next)
	{
		next = cur->next;

		// only check for scenics in visual range
		dist = DistanceBetweenPointsSquared(&frog[pl]->actor->pos,&cur->pltActor->actor->pos);
		if(dist > (radius * radius))
			continue;

		if((cur->flags & PLATFORM_NEW_SHAKABLESCENIC) && (numInRange < 4))
		{
			mags[numInRange]		= dist;
			inRange[numInRange++]	= cur;
		}
	}

	if(numInRange)
	{
		// return closest item
		dist	= mags[0];
		nearest	= inRange[0];
		for(i=1; i<numInRange; i++)
		{
			if(mags[i] < dist)
			{
				dist	= mags[i];
				nearest	= inRange[i];
			}
		}

		// we now have the nearest 'scenic' - return relevant actor
		return nearest->pltActor;
	}

	return NULL;
}


/*	--------------------------------------------------------------------------------
	Function		: FrogIsInRange
	Purpose			: Find nearest frog 
	Parameters		: 
	Returns			: ACTOR2 *
	Info			: Multiplayer use only
*/
ACTOR2 *FrogIsInRange(float radius, int pl)
{
	ACTOR2 *f;
	int i, closest=-1;
	float dist, best=1000000;

	for( i=(pl+1)%NUM_FROGS; i!=pl; i=(i+1)%NUM_FROGS )
	{
		dist = DistanceBetweenPointsSquared(&frog[i]->actor->pos, &frog[pl]->actor->pos);
		if( (dist < (radius*radius)) && (dist < best) )
		{
			closest = i;
			best = dist;
		}
	}

	if( closest == -1 ) return NULL;

	return frog[closest];
}


/*	--------------------------------------------------------------------------------
	Function		: ThrowFrogDirection
	Purpose			: Hurl a frog along a tile pointer direction
	Parameters		: 
	Returns			: 
	Info			: Multiplayer use only
*/
void ThrowFrogDirection( int thrower, int throwee, int dir )
{
	GAMETILE *tile = NULL, *tile2 = currTile[thrower], *tile3 = NULL;
	float count=0;

	// Find all tiles in the direction as far as the eye can see
	while( (tile = FindJoinedTileByDirectionConstrained(tile2,&currTile[thrower]->dirVector[dir],4)) )
	{
		if( tile3 == tile ) { tile = tile2; break; } // Oscillating between 2 tiles
		tile3 = tile2;
		tile2 = tile;
		count++;
	}

	if( !tile ) tile = tile2;

	AnimateActor( frog[throwee]->actor, FROG_ANIM_FWDSOMERSAULT, NO, NO, 0.5, 0, 0 );
	AnimateActor( frog[throwee]->actor, FROG_ANIM_TRYTOFLY, NO, YES, 0.5, 0, 0 );
	// Spring throwee to target tile
	SpringFrogToTile(tile, 50+(count*10), 0.1+(count*0.08), throwee);
}