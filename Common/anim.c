/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: anim.c
	Programmer	: Andy Eder
	Date		: 29/04/99 10:30:56

----------------------------------------------------------------------------------------------- */


#define F3DEX_GBI_2

#include <ultra64.h>

#include "incs.h"

float hedRotAmt = 0;

/*	--------------------------------------------------------------------------------
	Function 	  : 
	Purpose 	   : 
	Parameters 	: 
	Returns 	   : 
	Info 		     :
*/
void InitActorAnim(ACTOR *tempActor)
{
	int i;

	if( !tempActor->objectController )
		return;
	
	memcpy(tempActor->animation, tempActor->objectController->animation, sizeof(ACTOR_ANIMATION));	
	tempActor->animation->currentAnimation = -1;

	tempActor->animation->queueAnimation[0] = -1;
	tempActor->animation->queueAnimation[1] = -1;
	tempActor->animation->queueAnimation[2] = -1;
	tempActor->animation->queueAnimation[3] = -1;
	tempActor->animation->queueAnimation[4] = -1;
	tempActor->animation->loopAnimation = NO;
	tempActor->animation->numberQueued = 0;
	tempActor->animation->animTime = 0;
	tempActor->animation->reachedEndOfAnimation = FALSE;

	// Cue sounds from animations
	tempActor->animation->sfxMapping = FindSfxMapping( tempActor->objectController->objectID, tempActor );
	// Clear all loopflags
	for( i=0; i<NUM_NME_ANIMS; i++ )
		tempActor->animation->loopFlags[i] = 0;
}

/*	--------------------------------------------------------------------------------
	Function	: AnimateActor
	Purpose		: Runs an animation on an actor EXCEPT when it's already running
	Parameters	: actor, anim, loop?, queue?, speed, morph?, keep proportion?
	Returns		: void
*/
void StartAnimateActor(ACTOR *actor, int animNum, char loop, char queue, float speed, BYTE morphs,BOOL keepProportion)
{
	if (actor->animation->currentAnimation != animNum)
		AnimateActor(actor, animNum, loop, queue, speed, morphs, keepProportion);
}

/*	--------------------------------------------------------------------------------
	Function	: AnimateActor
	Purpose		: Runs an animation on a specific ACTOR
	Parameters	: actor, anim, loop?, queue?, speed, morph?, keep proportion?
	Returns		: void
*/
void AnimateActor(ACTOR *actor, int animNum, char loop, char queue, float speed, BYTE morphs,BOOL keepProportion)
{
	ACTOR_ANIMATION *actorAnim = actor->animation;
	animation *anim;
	float	actualSpeed,proportion;

	if(actorAnim == NULL)
		return;
	if(actorAnim->anims == NULL)
		return;

//if((actorAnim->currentAnimation == animNum) && (queue == NO) && (actorAnim->reachedEndOfAnimation == FALSE) && (((speed == 0) && (actorAnim->animationSpeed == actorAnim->anims[animNum].speed)) || (speed == actorAnim->animationSpeed)))
//	{
//		actorAnim->loopAnimation = loop;
//		return;
//	}

	if(animNum > actorAnim->numAnimations)
//	{
		return;
//		dprintf"Trying to access illegal animation\n"));
//		animNum = 0;
//	}

	if(speed == 0)
		actualSpeed = actorAnim->anims[animNum].speed;
	else
		actualSpeed = speed;

	if(keepProportion)
	{
		anim = actorAnim->anims + actorAnim->currentAnimation;
		proportion = (float)(actorAnim->animTime - anim->animStart)/((float)anim->animEnd - anim->animStart);
	}

	if(queue == NO)
	{
		if(morphs)
		{
			actorAnim->currentAnimation = animNum;
			anim = actorAnim->anims + actorAnim->currentAnimation;
			actorAnim->loopAnimation = loop;
			actorAnim->animationSpeed = actualSpeed;
			actorAnim->reachedEndOfAnimation = FALSE;
			actorAnim->numMorphFrames = morphs;
			actorAnim->currentMorphFrame = 0;
			actorAnim->morphTo = anim->animStart;
			actorAnim->morphFrom = actorAnim->animTime;
			if(actualSpeed < 0)
				actorAnim->animTime = anim->animEnd;
			else
				actorAnim->animTime = anim->animStart;
			FlushQueue(actor);
		}
		else
		{
			actorAnim->currentAnimation = animNum;
			anim = actorAnim->anims + actorAnim->currentAnimation;
			actorAnim->loopAnimation = loop;
			actorAnim->animationSpeed = actualSpeed;
			actorAnim->reachedEndOfAnimation = FALSE;
			actorAnim->numMorphFrames = morphs;
			if(actualSpeed < 0)
				actorAnim->animTime = anim->animEnd;
			else
				actorAnim->animTime = anim->animStart;
			FlushQueue(actor);
		}
		if(keepProportion)
			actorAnim->morphTo = actorAnim->animTime = anim->animStart + proportion*(float)(anim->animEnd-anim->animStart);

		if( actorAnim->sfxMapping && actorAnim->sfxMapping[actorAnim->currentAnimation] )
			PlaySfxMappedSample( actor, 500, SAMPLE_VOLUME, -1/*128*/ );
	}
	else
	{
		if(actorAnim->currentAnimation == -1)
		{
			if (morphs)
			{
				actorAnim->currentAnimation = animNum;
				actorAnim->loopAnimation = loop;
				actorAnim->animationSpeed = actualSpeed;
				actorAnim->reachedEndOfAnimation = FALSE;
				actorAnim->numMorphFrames = morphs;
				actorAnim->currentMorphFrame = 0;
				actorAnim->morphTo = anim->animStart;
				actorAnim->morphFrom = actorAnim->animTime;
			}
			else
			{
				actorAnim->currentAnimation = animNum;
				actorAnim->loopAnimation = loop;
				actorAnim->animationSpeed = actualSpeed;
				actorAnim->numMorphFrames = morphs;
			}
		}
		else
		{
			if(actorAnim->numberQueued == ANIM_QUEUE_LENGTH)
				return;
			actorAnim->queueAnimation[actorAnim->numberQueued] = animNum;
			actorAnim->queueLoopAnimation[actorAnim->numberQueued] = loop;
			actorAnim->queueAnimationSpeed[actorAnim->numberQueued] = actualSpeed;
			actorAnim->queueNumMorphFrames[actorAnim->numberQueued] = morphs;
			actorAnim->numberQueued++;
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: SetAnimationSpeed	
	Purpose 	: Sets the speed of an actor
	Parameters 	: actor, speed
	Returns 	: none
	Info 		:
*/
void SetAnimationSpeed(ACTOR *myActor, float speed)
{
	myActor->animation->animationSpeed = speed;
}

/*	--------------------------------------------------------------------------------
	Function 	: FlushQueue()
	Purpose 	: Flush the animation queue of the given actor
	Parameters 	: actor pointer
	Returns 	: none
	Info 		:
*/
void FlushQueue(ACTOR *myActor)
{
	int i;

	myActor->animation->numberQueued = 0;
	for(i = 0;i < ANIM_QUEUE_LENGTH;i++)
		myActor->animation->queueAnimation[i] = -1;
}

/*	--------------------------------------------------------------------------------
	Function 	: AnimateActor(ACTOR *actor)
	Purpose 	: handle animation for all charcters
	Parameters 	: ACTOR *
	Returns 	: none
	Info 		:
*/
void UpdateAnimations(ACTOR *actor)
{
	ACTOR_ANIMATION *actorAnim = actor->animation;
	animation *anim;
	int i;

	if(actorAnim == NULL)
		return;

	if(actorAnim->anims == NULL)
		return;
	anim = actorAnim->anims + actorAnim->currentAnimation;

	if(actorAnim->currentAnimation < 0)
		return;

	if(actorAnim->numMorphFrames > 0)
	{
		if (actorAnim->currentMorphFrame < actorAnim->numMorphFrames)
			actorAnim->currentMorphFrame++;
		else
			actorAnim->numMorphFrames = 0;
		return;
	}

	actorAnim->reachedEndOfAnimation = FALSE;
	actorAnim->animTime += (actorAnim->animationSpeed) * gameSpeed;	// * GAME_SPEED);

	if (((actorAnim->animTime > anim->animEnd) || (actorAnim->animTime < anim->animStart)) &&
		(actorAnim->loopAnimation == NO || actorAnim->numberQueued))
	{
		actorAnim->animTime = Bound(actorAnim->animTime,anim->animStart,anim->animEnd);
		actorAnim->reachedEndOfAnimation = actorAnim->currentAnimation+1;

		if(actorAnim->numberQueued)
		{
			actorAnim->currentAnimation = actorAnim->queueAnimation[0];//actorAnim;
			actorAnim->loopAnimation = actorAnim->queueLoopAnimation[0];//loop;
			actorAnim->animationSpeed = actorAnim->queueAnimationSpeed[0];//speed;
			actorAnim->numMorphFrames = actorAnim->queueNumMorphFrames[0];
			if(actorAnim->numMorphFrames != 0)
			{
				actorAnim->morphTo = actorAnim->anims[actorAnim->currentAnimation].animStart;
				actorAnim->morphFrom = actorAnim->animTime;
				actorAnim->currentMorphFrame = 0;
			}
			if(actorAnim->animationSpeed < 0)
				actorAnim->animTime = actorAnim->anims[actorAnim->currentAnimation].animEnd;
			else
				actorAnim->animTime = actorAnim->anims[actorAnim->currentAnimation].animStart;


			if (--actorAnim->numberQueued)
			{
				for(i = 0;i < ANIM_QUEUE_LENGTH - 1;i++)
				{
					actorAnim->queueAnimation[i] = actorAnim->queueAnimation[i + 1];
					actorAnim->queueLoopAnimation[i] = actorAnim->queueLoopAnimation[i + 1];
					actorAnim->queueAnimationSpeed[i] = actorAnim->queueAnimationSpeed[i + 1];
					actorAnim->queueNumMorphFrames[i] = actorAnim->queueNumMorphFrames[i + 1];
				}

				actorAnim->queueAnimation[ANIM_QUEUE_LENGTH - 1] = -1;
				actorAnim->queueLoopAnimation[ANIM_QUEUE_LENGTH - 1] = -1;
				actorAnim->queueAnimationSpeed[ANIM_QUEUE_LENGTH - 1] = -1;
				actorAnim->queueNumMorphFrames[ANIM_QUEUE_LENGTH - 1] = 0;
			}
			else
			{
				*actorAnim->queueAnimation = -1;
				*actorAnim->queueLoopAnimation = -1;
				*actorAnim->queueAnimationSpeed = -1;
				*actorAnim->queueNumMorphFrames = 0;
			}
			if( actorAnim->sfxMapping )
				PlaySfxMappedSample( actor, 500, SAMPLE_VOLUME, -1/*128*/ );
		}
	}
	else
	{
		
		if(anim->animEnd == anim->animStart)
			actorAnim->animTime = anim->animEnd;			
		else if(actorAnim->animTime > anim->animEnd)
		{
			actorAnim->animTime -= (anim->animEnd - anim->animStart);

			if( actorAnim->sfxMapping )
				PlaySfxMappedSample( actor, 500, SAMPLE_VOLUME, -1/*128*/ );

		}
		else if(actorAnim->animTime < anim->animStart)
			actorAnim->animTime += (anim->animEnd - anim->animStart);			
		
	}
	if(actorAnim->numMorphFrames)
	{
		if(actorAnim->currentMorphFrame < -actorAnim->numMorphFrames)
			actorAnim->currentMorphFrame++;
		else
			actorAnim->numMorphFrames = 0;
		actorAnim->morphTo = actorAnim->animTime;
	}
}




/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
BOOL QueryAnimTime(ACTOR *actor, float time)
{
	if((actor->animation->animTime >= time) && (actor->animation->animTime < actor->animation->animationSpeed + time))
		return YES;

	return NO;
}


/*	--------------------------------------------------------------------------------
	Function 	: Damage functions
	Purpose 	: Frog is hurt but not dead
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void DamageNormal( int pl )
{
	SPECFX *fx;

	AnimateActor(frog[pl]->actor, FROG_ANIM_ASSONFIRE, NO, NO, 0.5F, 0, 0);
	CreateAndAddSpecialEffect( FXTYPE_FROGSTUN, &frog[pl]->actor->pos, &currTile[pl]->normal, 30, 0, 0, 3.0 );

	if( (fx = CreateAndAddSpecialEffect( FXTYPE_SPARKBURST, &frog[pl]->actor->pos, &currTile[pl]->normal, 30, 4, 0, 5)) )
	{
		SetFXColour( fx, 255, 255, 0 );
		fx->gravity = 0.1;
	}
	if( (fx = CreateAndAddSpecialEffect( FXTYPE_SPARKBURST, &frog[pl]->actor->pos, &currTile[pl]->normal, 30, 3, 0, 5 )) )
	{
		SetFXColour( fx, 255, 255, 130 );
		fx->gravity = 0.1;
	}
	if( (fx = CreateAndAddSpecialEffect( FXTYPE_SPARKLYTRAIL, &frog[pl]->actor->pos, &currTile[pl]->normal, 30, 2, 0, 5 )) )
	{
		SetFXColour( fx, 255, 255, 130 );
		fx->gravity = 0.1;
	}
	if( (fx = CreateAndAddSpecialEffect( FXTYPE_SPARKLYTRAIL, &frog[pl]->actor->pos, &currTile[pl]->normal, 30, 2.5, 0, 5 )) )
	{
		SetFXColour( fx, 255, 255, 255 );
		fx->gravity = 0.1;
	}
	if( (fx = CreateAndAddSpecialEffect( FXTYPE_SPARKBURST, &frog[pl]->actor->pos, &currTile[pl]->normal, 30, 3, 0, 5 )) )
	{
		SetFXColour( fx, 200, 255, 200 );
	}
	if( (fx = CreateAndAddSpecialEffect( FXTYPE_SPARKLYTRAIL, &frog[pl]->actor->pos, &currTile[pl]->normal, 30, 2.5, 0, 5 )) )
	{
		SetFXColour( fx, 255, 255, 255 );
		fx->gravity = 0.1;
	}
	
	// jump in air thing
	if (!currPlatform[pl])
	{
		if (destTile[pl])
			CalculateFrogJump(
				&frog[pl]->actor->pos, &destTile[pl]->centre, &currTile[pl]->normal,
				100, 50, pl);
		else
		{
			CalculateFrogJump(
				&frog[pl]->actor->pos, &currTile[pl]->centre, &currTile[pl]->normal,
				100, 50, pl);
			destTile[pl] = currTile[pl];
		}
		player[pl].canJump = 0;
	}
}

void DamageRunOver( int pl )
{
	DamageNormal(pl);
}

void DamageDrowning( int pl )
{
	DamageNormal(pl);
}

void DamageSquashed( int pl )
{
	DamageNormal(pl);
}

void DamageFire( int pl )
{
	CreateAndAddSpecialEffect( FXTYPE_FIERYSMOKE, &frog[pl]->actor->pos, &currTile[pl]->normal, 50, 0.5, 0, 2 );
	AnimateActor(frog[pl]->actor, FROG_ANIM_ASSONFIRE, NO, NO, 0.5F, 0, 0);
}

void DamageElectric( int pl )
{
	DamageNormal(pl);
}

void DamageFalling( int pl )
{
	DamageNormal(pl);
}

void DamageWhacking( int pl )
{
	DamageNormal(pl);
}

void DamageInflation( int pl )
{
	DamageNormal(pl);
}

void DamagePoison( int pl )
{
	DamageNormal(pl);
}

void DamageSlicing( int pl )
{
	DamageNormal(pl);
}

void DamageExplosion( int pl )
{
	DamageNormal(pl);
}

void DamageGibbing( int pl )
{
	DamageNormal(pl);
}

/*	--------------------------------------------------------------------------------
	Function 	: Death functions
	Purpose 	: Frog is dead
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void DeathNormal( int pl )
{
	player[pl].deathBy = DEATHBY_NORMAL;
	AnimateActor(frog[pl]->actor, FROG_ANIM_FWDSOMERSAULT, NO, NO, 0.5F, 0, 0);
	GTInit( &player[pl].dead, 3 );
}

void DeathRunOver( int pl )
{
	DeathNormal(pl);
}

void DeathDrowning( int pl )
{
	DeathNormal(pl);
}

void DeathSquashed( int pl )
{
	player[pl].deathBy = DEATHBY_SQUASHED;
	player[pl].idleEnable = 0;

	// Drop in replacement model and animate gib explosion
	frog[pl]->actor->LODObjectController = frog[pl]->actor->objectController;
	FindObject( &frog[pl]->actor->objectController, UpdateCRC("dth-flat.obe"), "dth-flat.obe", NO );
	InitActorAnim( frog[pl]->actor );

	AnimateActor( frog[pl]->actor, 0, NO, NO, 0.2, 0, 0 );
	AnimateActor( frog[pl]->actor, 1, YES, YES, 0.2, 0, 0 );

	GTInit( &player[pl].dead, 3 );
}

void DeathFire( int pl )
{
	player[pl].deathBy = DEATHBY_FIRE;
	AnimateActor(frog[pl]->actor, FROG_ANIM_ASSONFIRE, NO, NO, 0.5F, 0, 0);

	BounceFrog( pl, 100, 50 );

	GTInit( &player[pl].dead, 3 );
}

void DeathElectric( int pl )
{
	player[pl].deathBy = DEATHBY_ELECTRIC;
	CreateAndAddSpecialEffect( FXTYPE_LIGHTNING, &frog[pl]->actor->pos, &currTile[pl]->normal, 5, 40, 0.25, 0.5 );
	AnimateActor(frog[pl]->actor, FROG_ANIM_ELECTROCUTE, NO, NO, 0.5F, 0, 0);
	GTInit( &player[pl].dead, 3 );
}

void DeathFalling( int pl )
{
	DeathNormal(pl);
}

void DeathWhacking( int pl )
{
	if( frog[pl]->actor->shadow ) frog[pl]->actor->shadow->draw = 0;

	player[pl].deathBy = DEATHBY_WHACKING;
	ThrowFrogAtScreen( pl );
	GTInit( &player[pl].dead, 5 );
}

void DeathInflation( int pl )
{
	player[pl].deathBy = DEATHBY_INFLATION;
	player[pl].idleEnable = 0;

	// Drop in replacement model and animate gib explosion
	frog[pl]->actor->LODObjectController = frog[pl]->actor->objectController;
	FindObject( &frog[pl]->actor->objectController, UpdateCRC("dth-ball.obe"), "dth-ball.obe", NO );
	InitActorAnim( frog[pl]->actor );

	// Randomly play different type of inflation anim
	if( Random(2) )
	{
		AnimateActor( frog[pl]->actor, 0, NO, NO, 0.2, 0, 0 );
		AnimateActor( frog[pl]->actor, 2, YES, YES, 0.2, 0, 0 );
	}
	else
	{
		AnimateActor( frog[pl]->actor, 1, NO, NO, 0.2, 0, 0 );
		AnimateActor( frog[pl]->actor, 3, YES, YES, 0.2, 0, 0 );
	}

	GTInit( &player[pl].dead, 5 );
}

void DeathPoison( int pl )
{
	player[pl].deathBy = DEATHBY_POISON;
	AnimateActor(frog[pl]->actor, FROG_ANIM_LANDONHEAD, NO, NO, 0.5F, 0, 0);
	GTInit( &player[pl].dead, 3 );
}

void DeathSlicing( int pl )
{
	SPECFX *fx;

	player[pl].deathBy = DEATHBY_SLICING;

	if( frog[pl]->actor->shadow ) frog[pl]->actor->shadow->draw = 0;

	// Drop in replacement model and animate halves falling apart
	frog[pl]->actor->LODObjectController = frog[pl]->actor->objectController;
	FindObject( &frog[pl]->actor->objectController, UpdateCRC("dth-half.obe"), "dth-half.obe", NO );
	InitActorAnim( frog[pl]->actor );

	AnimateActor( frog[pl]->actor, 0, NO, NO, 0.25, 0, 0 );
	AnimateActor( frog[pl]->actor, 1, YES, YES, 0.25, 0, 0 );

	if( (fx = CreateAndAddSpecialEffect( FXTYPE_SPARKBURST, &frog[pl]->actor->pos, &currTile[pl]->normal, 20, 4, 0, 5 )) )
	{
		SetFXColour( fx, 255, 255, 0 );
		fx->gravity = 0.1;
	}
	if( (fx = CreateAndAddSpecialEffect( FXTYPE_SPARKBURST, &frog[pl]->actor->pos, &currTile[pl]->normal, 20, 3, 0, 5 )) )
	{
		SetFXColour( fx, 255, 255, 130 );
		fx->gravity = 0.1;
	}
	if( (fx = CreateAndAddSpecialEffect( FXTYPE_DECAL, &frog[pl]->actor->pos, &currTile[pl]->normal, 40, 0, 0, 5 )) )
	{
		SetFXColour( fx, 50, 200, 50 );
		fx->tex = txtrSolidRing;
	}
	GTInit( &player[pl].dead, 3 );
}

void DeathExplosion( int pl )
{
	SPECFX *fx;

	if( frog[pl]->actor->shadow ) frog[pl]->actor->shadow->draw = 0;

	player[pl].deathBy = DEATHBY_EXPLOSION;
	player[pl].idleEnable = 0;

	// Drop in replacement model and animate gib explosion
	frog[pl]->actor->LODObjectController = frog[pl]->actor->objectController;
	FindObject( &frog[pl]->actor->objectController, UpdateCRC("dth-gib.obe"), "dth-gib.obe", NO );
	InitActorAnim( frog[pl]->actor );

	AnimateActor( frog[pl]->actor, 0, NO, NO, 0.2, 0, 0 );
	AnimateActor( frog[pl]->actor, 2, YES, YES, 0.2, 0, 0 );

	if( (fx = CreateAndAddSpecialEffect( FXTYPE_SPARKBURST, &frog[pl]->actor->pos, &currTile[pl]->normal, 20, 4, 0, 5 )) )
	{
		SetFXColour( fx, 255, 255, 0 );
		fx->gravity = 0.1;
	}
	if( (fx = CreateAndAddSpecialEffect( FXTYPE_SPARKBURST, &frog[pl]->actor->pos, &currTile[pl]->normal, 20, 3, 0, 5 )) )
	{
		SetFXColour( fx, 255, 255, 130 );
		fx->gravity = 0.1;
	}
	if( (fx = CreateAndAddSpecialEffect( FXTYPE_DECAL, &frog[pl]->actor->pos, &currTile[pl]->normal, 40, 0, 0, 5 )) )
	{
		SetFXColour( fx, 50, 200, 50 );
		fx->tex = txtrSolidRing;
	}
	GTInit( &player[pl].dead, 3 );
}

void DeathGibbing( int pl )
{
	SPECFX *fx;

	if( frog[pl]->actor->shadow ) frog[pl]->actor->shadow->draw = 0;

	player[pl].deathBy = DEATHBY_GIBBING;
	player[pl].idleEnable = 0;

	// Drop in replacement model and animate gib explosion
	frog[pl]->actor->LODObjectController = frog[pl]->actor->objectController;
	FindObject( &frog[pl]->actor->objectController, UpdateCRC("dth-gib.obe"), "dth-gib.obe", NO );
	InitActorAnim( frog[pl]->actor );

	AnimateActor( frog[pl]->actor, 1, NO, NO, 0.2, 0, 0 );
	AnimateActor( frog[pl]->actor, 3, YES, YES, 0.2, 0, 0 );

	if( (fx = CreateAndAddSpecialEffect( FXTYPE_SPARKBURST, &frog[pl]->actor->pos, &currTile[pl]->normal, 20, 4, 0, 5 )) )
	{
		SetFXColour( fx, 255, 255, 0 );
		fx->gravity = 0.1;
	}
	if( (fx = CreateAndAddSpecialEffect( FXTYPE_SPARKBURST, &frog[pl]->actor->pos, &currTile[pl]->normal, 20, 3, 0, 5 )) )
	{
		SetFXColour( fx, 255, 255, 130 );
		fx->gravity = 0.1;
	}
	if( (fx = CreateAndAddSpecialEffect( FXTYPE_DECAL, &frog[pl]->actor->pos, &currTile[pl]->normal, 40, 0, 0, 5 )) )
	{
		SetFXColour( fx, 50, 200, 50 );
		fx->tex = txtrSolidRing;
	}
	GTInit( &player[pl].dead, 3 );
}