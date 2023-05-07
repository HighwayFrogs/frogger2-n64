/*

	This file is part of Frogger2, (c) 1997 Interactive Studios Ltd.


	File		: enemies.c
	Programmer	: Andrew Eder
	Date		: 1/12/99

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>

#include "incs.h"

REACTIVEANIM reactiveAnims[] = {
#include "x:\teamspirit\pcversion\reactive.txt"
};

ENEMYLIST enemyList;						// the enemy linked list

#define ENEMY_RANDOMNESS (0.5 + (Random(100)/100.0))	// returns a value from 0.5 to 1.5

void NMEDamageFrog( int num, ENEMY *nme );
void DoEnemyCollision( ENEMY *cur );
void RotateWaitingNME( ENEMY *cur );
void SlerpWaitingFlappyThing( ENEMY *cur );
void SetSoundEffectsForEnemy( ENEMY *nme );

void UpdatePathNME( ENEMY *cur );
void UpdateSlerpPathNME( ENEMY *cur );
void UpdateSnapper( ENEMY *cur );
void UpdateTileSnapper( ENEMY *cur );
void UpdateVent( ENEMY *cur );
void UpdateMoveVerticalNME( ENEMY *cur );
void UpdateRotatePathNME( ENEMY *cur );
void UpdateHomingNME( ENEMY *cur );
void UpdateMoveOnMoveNME( ENEMY *cur );
void UpdateFlappyThing( ENEMY *cur );
void UpdateFrogWatcher( ENEMY *cur );
void UpdateRandomMoveNME( ENEMY *cur );
void UpdateTileHomingNME( ENEMY *cur );

/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/

long GetReactiveAnimNumber(char *name)
{
	long i = 0;
	for (i=0; *reactiveAnims[i].name; i++)
		if (gstrcmp(name,reactiveAnims[i].name)==0)
			return i;
	return -1;
}

/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/

ENEMY *GetEnemyFromUID (long uid)
{
	ENEMY *cur,*next;

	for(cur = enemyList.head.next; cur != &enemyList.head; cur = cur->next)
		if (cur->uid == uid)
			return cur;

	dprintf"GetEnemyFromUID: Couldn't find enemy %d\n", uid));
	return NULL;
}

/*	--------------------------------------------------------------------------------
	Function		: DoEnemyCollision
	Purpose			: Check for collision with frog in single and multiplayer
	Parameters		: 
	Returns			: void
	Info			: 
*/
void DoEnemyCollision( ENEMY *cur )
{
	ACTOR2 *act = cur->nmeActor;

	if (NUM_FROGS==1)
	{
		if( (cur->flags & ENEMY_NEW_RADIUSBASEDCOLLISION))
		{
			if (!frog[0]->action.dead.time && !frog[0]->action.safe.time &&
				(DistanceBetweenPointsSquared(&frog[0]->actor->pos,&act->actor->pos)<((frog[0]->radius+act->radius)*(frog[0]->radius+act->radius))) )
			{
				if( cur->flags & ENEMY_NEW_BABYFROG )
				{
					if( PickupBabyFrog( cur->nmeActor ) )
						cur->active = 0;
				}
				else NMEDamageFrog(0,cur);
			}
		}
		else
		{
			if( (currTile[0] == cur->inTile) && !frog[0]->action.dead.time && !frog[0]->action.safe.time &&
				(!(player[0].frogState & FROGSTATUS_ISSUPERHOPPING) || (cur->flags & ENEMY_NEW_NOJUMPOVER)) &&
				!currPlatform[0] && !(player[0].frogState & FROGSTATUS_ISFLOATING) && !(cur->flags & ENEMY_NEW_NODAMAGE) )
			{
				if( cur->flags & ENEMY_NEW_BABYFROG )
				{
					if( PickupBabyFrog( cur->nmeActor ) )
						cur->active = 0;
				}
				else NMEDamageFrog(0,cur);
			}
		}
	}
	else
	{
		int i;
		for (i=0; i<NUM_FROGS; i++)
			if( (cur->flags & ENEMY_NEW_RADIUSBASEDCOLLISION) && !frog[i]->action.safe.time &&
				(DistanceBetweenPointsSquared(&frog[i]->actor->pos,&act->actor->pos)<((frog[i]->radius+act->radius)*(frog[i]->radius+act->radius))) )
			{
				if( cur->flags & ENEMY_NEW_BABYFROG )
				{}
//					PickupBabyFrogMulti( cur, i );
				else
					KillMPFrog(i);
			}
			else if( (currTile[i] == cur->inTile) && !frog[i]->action.safe.time && 
					(!(player[i].frogState & FROGSTATUS_ISSUPERHOPPING) || (cur->flags & ENEMY_NEW_NOJUMPOVER)) &&
					!(player[i].frogState & FROGSTATUS_ISFLOATING))
			{
				if( cur->flags & ENEMY_NEW_BABYFROG )
				{}
//					PickupBabyFrogMulti( cur, i );
				else
					KillMPFrog(i);
			}
	}
}

void NMEDamageFrog( int num, ENEMY *nme )
{
	if( !nme || (player[num].frogState & FROGSTATUS_ISSAFE))
		return;

	if( nme->flags & ENEMY_NEW_ONEHITKILL )
	{
		frog[num]->action.healthPoints = 0;
	}
	else
	{
		frog[num]->action.healthPoints--;

		#ifdef N64_VERSION
		StartRumble(120,1.5,5,ActiveController);
		#endif
	}
	
	if(frog[num]->action.healthPoints != 0)
	{
		/* 
		Check for NME flags and do different effects
		*/
		AnimateActor(frog[num]->actor, FROG_ANIM_ASSONFIRE, NO, NO, 0.5F, 0, 0);
		CreateAndAddSpecialEffect( FXTYPE_FROGSTUN, &frog[num]->actor->pos, &currTile[num]->normal, 30, 0, 0, 3.0 );
//		PlaySample(42,NULL,192,128);
		GTInit( &frog[num]->action.safe, 2 );
		PlaySample(GEN_FROG_HURT,&frog[0]->actor->pos,0,100-Random(15),60-Random(15));
	}
	else
	{
		if (nme->reactiveNumber!=-1)
		{
			if (reactiveAnims[nme->reactiveNumber].type & 0x01) //Face
				SetQuaternion(&(frog[num]->actor->qRot),&(nme->nmeActor->actor->qRot));
			
			if (reactiveAnims[nme->reactiveNumber].type & 0x02) //Center
				SetVector(&(frog[num]->actor->pos),&(nme->nmeActor->actor->pos));

			if (reactiveAnims[nme->reactiveNumber].type & 0x04) //FixedPos
				nme->doNotMove = 1;
			
			AnimateActor(frog[num]->actor,reactiveAnims[nme->reactiveNumber].animFrog, NO, NO, 0.25F, 0, 0);
			AnimateActor(nme->nmeActor->actor,reactiveAnims[nme->reactiveNumber].animChar, NO, NO, 0.25F, 0, 0);

			GTInit( &frog[num]->action.dead, 5 );
		}
		else
		{
			GTInit( &frog[num]->action.dead, 3 );
		
			AnimateActor(frog[num]->actor, FROG_ANIM_FWDSOMERSAULT, NO, NO, 0.5F, 0, 0);
		}

		frog[num]->action.healthPoints = 3;
		frog[num]->action.deathBy = DEATHBY_NORMAL;
		player[num].frogState |= FROGSTATUS_ISDEAD;

		/* 
		Check for NME flags and do different effects
//		PlaySample(110,NULL,192,128);
		AnimateActor(frog[num]->actor, FROG_ANIM_FWDSOMERSAULT, NO, NO, 0.5F, 0, 0);
		GTInit( &frog[num]->action.dead, 2 );
		frog[num]->action.healthPoints = 3;
		frog[num]->action.deathBy = DEATHBY_NORMAL;
		player[num].frogState |= FROGSTATUS_ISDEAD;
		PlaySample(GEN_FROG_DEATH,&frog[0]->actor->pos,0,100-Random(15),60-Random(15));
*/

	}
}

/*	--------------------------------------------------------------------------------
	Function		: UpdateEnemies
	Purpose			: updates all enemies and their states / movements, etc.
	Parameters		: 
	Returns			: void
	Info			: 
*/
void UpdateEnemies()
{
	ENEMY *cur,*next;
	long i;

	if(enemyList.numEntries == 0)
		return;

	for(cur = enemyList.head.next; cur != &enemyList.head; cur = next)
	{
		ACTOR2 *act = cur->nmeActor;
		next = cur->next;

		// check if enemy is active
		if(!cur->active)
			continue;

		// Set point of interest for frog
		if( act->distanceFromFrog < pOIDistance )
		{
			VECTOR moveVec;
			float angle;
			SubVector( &moveVec, &act->actor->pos, &frog[0]->actor->pos );
			MakeUnit( &moveVec );
			angle = acos(DotProduct( &currTile[0]->dirVector[frogFacing[0]], &moveVec ));
			if( angle < 0.9 )
			{
				pOIDistance = act->distanceFromFrog;
				pointOfInterest = &act->actor->pos;
			}
		}

		if( frameCount > 10 )
			DoEnemyCollision( cur );

		// check if this enemy is currently 'waiting' at a node
		if(cur->isWaiting)
		{
			if(cur->isWaiting == -1)
				continue;

			if(actFrameCount > cur->path->startFrame)
				cur->isWaiting = 0;
			else
			{
				if( !(cur->flags & ENEMY_NEW_FACEFORWARDS) )
				{
					// if enemy is following a path, do a slerp so it'll rotate nicely
					// (except the way I do it is a bit poo - Dave)
					if( (cur->flags & ENEMY_NEW_FOLLOWPATH) || (cur->flags & ENEMY_NEW_RANDOMMOVE) )
					{
						RotateWaitingNME( cur );
					}
					else if( cur->flags & ENEMY_NEW_FLAPPYTHING )
					{
						SlerpWaitingFlappyThing( cur );
					}
				}
				continue;
			}
		}

		// Do update functions for individual enemy types
		if (cur->Update)
			cur->Update(cur);

		// Do Special Effects attached to enemies
		if( act->effects && !(cur->flags & ENEMY_NEW_VENT) )
			ProcessAttachedEffects( (void *)cur, ENTITY_ENEMY );
	}
}

void RotateWaitingNME( ENEMY *cur )
{
	QUATERNION q, res;
	float t;
	VECTOR fromPosition, toPosition, fwd;
	long start_t, end_t, time;
	
	end_t = cur->path->startFrame;
	time = cur->isWaiting*waitScale;
	start_t = end_t - time;

	t = 1.0 - (float)(actFrameCount-start_t)/(float)(time);

	GetPositionForPathNode(&toPosition,&cur->path->nodes[cur->path->toNode]);
	GetPositionForPathNode(&fromPosition,&cur->path->nodes[cur->path->fromNode]);
	
	SubVector(&fwd,&toPosition,&fromPosition);
	MakeUnit( &fwd );
	Orientate(&q,&fwd,&inVec,&cur->currNormal);

	QuatSlerp(&q, &cur->nmeActor->actor->qRot, t, &res);

	SetQuaternion(&cur->nmeActor->actor->qRot, &res);
}

/*	--------------------------------------------------------------------------------
	Function		: UpdatePathNME
	Purpose			: update enemies that move along a path
	Parameters		: ENEMY
	Returns			: void
	Info			: One day, some enemies will be done this way...
*/
void UpdatePathNME( ENEMY *cur )
{
	VECTOR fromPosition,toPosition, fwd, moveVec;
	float length;
	
	// first, update the enemy position
	GetPositionForPathNode(&toPosition,&cur->path->nodes[cur->path->toNode]);
	GetPositionForPathNode(&fromPosition,&cur->path->nodes[cur->path->fromNode]);
	
	SubVector(&fwd,&toPosition,&fromPosition);
	
	length = (float)(actFrameCount - cur->path->startFrame)/(float)(cur->path->endFrame - cur->path->startFrame);
	
	ScaleVector(&fwd,length);

	if (!cur->doNotMove)
		AddVector(&cur->nmeActor->actor->pos,&fwd,&fromPosition);

	MakeUnit (&fwd);

	AddToVector(&cur->currNormal,&cur->deltaNormal);

	if (!cur->doNotMove)
		if( !(cur->flags & ENEMY_NEW_FACEFORWARDS) )
			Orientate(&cur->nmeActor->actor->qRot,&fwd,&inVec,&cur->currNormal);
		else // Need to do this so normals still work
			Orientate(&cur->nmeActor->actor->qRot,&inVec,&inVec,&cur->currNormal);

	// check if this enemy has arrived at a path node
	if( actFrameCount > cur->path->endFrame )
	{
		UpdateEnemyPathNodes(cur);
		CalcEnemyNormalInterps(cur);

		cur->path->startFrame = cur->path->endFrame + cur->isWaiting * waitScale;

		if (cur->flags & ENEMY_NEW_RANDOMSPEED)
			cur->path->endFrame = cur->path->startFrame + (60*((float)Random(100)/100.0F) );
		else
			cur->path->endFrame = cur->path->startFrame + (60*cur->speed);
	}

	/*	------------- Push blocks, lovely blocks that push --------------- */
	if (cur->flags & ENEMY_NEW_PUSHESFROG)
	{
		VECTOR v;
		AddVector(&v, &cur->nmeActor->actor->pos, &fwd);
		SubFromVector(&v, &frog[0]->actor->pos);

		if ((MagnitudeSquared(&v) < 1000) && player[0].canJump)
		{
			PushFrog(&cur->nmeActor->actor->pos, &fwd, 0);
			player[0].canJump = 0;
			AnimateActor(frog[0]->actor, FROG_ANIM_FORWARDSOMERSAULT, NO, NO, 0.6f, 0, 0);
		}
	}

	if( actFrameCount < cur->path->startFrame+(0.5*(cur->path->endFrame-cur->path->startFrame)) )
		cur->inTile = cur->path->nodes[cur->path->fromNode].worldTile;
	else
		cur->inTile = cur->path->nodes[cur->path->toNode].worldTile;
}


/*	--------------------------------------------------------------------------------
	Function		: UpdatePathSlerpNME
	Purpose			: Enemies that move in a smooth curve along a path
	Parameters		: ENEMY
	Returns			: void
	Info			: Use offset2 for slerp speed.
*/
void UpdateSlerpPathNME( ENEMY *cur )
{
	QUATERNION q1, q2, q3;
	VECTOR fromPosition,toPosition, fwd, moveVec;
	PATH *path = cur->path;
	ACTOR *act = cur->nmeActor->actor;
	float speed, t;
	
	// Find the position of the current 2 nodes
	GetPositionForPathNode(&toPosition,&path->nodes[path->toNode]);
	
	// Set direction to desired point
	SubVector(&fwd,&toPosition,&act->pos);
	MakeUnit(&fwd);

	SetQuaternion( &q1, &act->qRot );

	// Skewer a line to rotate around, and make a rotation
	CrossProduct((VECTOR *)&q3,&inVec,&fwd);
	t = DotProduct(&inVec,&fwd);
	if (t<-0.999)
		t=-0.999;
	if (t>0.999)
		t = 0.999;
	q3.w=acos(t);
	// Duh.
	GetQuaternionFromRotation(&q2,&q3);

	// Slerp between current and desired orientation
	speed = path->nodes[path->fromNode].offset2 * gameSpeed;
	if( speed > 0.999 ) speed = 0.999;
	QuatSlerp( &q1, &q2, speed, &act->qRot );

	// Move forwards a bit in direction of facing
	RotateVectorByQuaternion( &fwd, &inVec, &act->qRot );
	if( cur->flags & ENEMY_NEW_RANDOMSPEED )
	{
		ScaleVector( &fwd, cur->speed*gameSpeed*((float)Random(100)/100.0F) );
	}
	else
	{
		ScaleVector( &fwd, cur->speed*gameSpeed );
	}

	AddToVector( &act->pos, &fwd );

//	AddToVector(&cur->currNormal,&cur->deltaNormal);

	// Update path nodes
	if( DistanceBetweenPointsSquared(&act->pos,&toPosition) < 100 )
	{
		UpdateEnemyPathNodes(cur);

		cur->path->startFrame = actFrameCount + cur->isWaiting * waitScale;
	}

	// Set inTile
	GetPositionForPathNode(&fromPosition,&path->nodes[path->fromNode]);
	GetPositionForPathNode(&toPosition,&path->nodes[path->toNode]);

	t = DistanceBetweenPoints(&fromPosition,&toPosition) / 2.0F;

	if(DistanceBetweenPointsSquared(&fromPosition,&act->pos) < (t*t))
		cur->inTile = path->nodes[path->fromNode].worldTile;
	else
		cur->inTile = path->nodes[path->toNode].worldTile;
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateSnapper
	Purpose			: Snap at the frog
	Parameters		: ENEMY
	Returns			: void
	Info			: Path speed -> Slerp speed; waitTime -> Snap delay; 
*/
void UpdateSnapper( ENEMY *cur )
{
	QUATERNION q1, q2;
	ACTOR *act = cur->nmeActor->actor;
	PATH *path = cur->path;
	SPECFX *fx;
	float speed;

	// Idle animations
	switch( cur->isSnapping )
	{
	case 0:
		if( actFrameCount < path->endFrame )
			break;

		if (Random(1000)>980)
			AnimateActor(act,2,NO,NO,cur->nmeActor->animSpeed, 0, 0);
	
		if (Random(1000)>950)
		{
			if (Random(1000)>950)
				AnimateActor(act,3,NO,YES,cur->nmeActor->animSpeed, 0, 0);
			else
				AnimateActor(act,0,NO,YES,cur->nmeActor->animSpeed, 0, 0);
		}

		// Slerp orientation towards frog
		SetVector( &cur->currNormal, &path->nodes[0].worldTile->normal );
		SetQuaternion( &q1, &act->qRot );
		ActorLookAt( act, &frog[0]->actor->pos, LOOKAT_2D );
		SetQuaternion( &q2, &act->qRot );
		speed = path->nodes[0].speed * gameSpeed;
		if( speed > 0.999 ) speed = 0.999;
		QuatSlerp( &q1, &q2, speed, &act->qRot );

		// If the snapper has just spotted the frog, set snap time
		if( cur->nmeActor->distanceFromFrog < 5625 ) // 75*75
		{
			SetVector( &act->rotaim, &currTile[0]->centre );

			SetQuaternion( &q1, &act->qRot );
			ActorLookAt( act, &act->rotaim, LOOKAT_2D );
			SetQuaternion( &q2, &act->qRot );
			QuatSlerp( &q1, &q2, speed, &act->qRot );

			path->startFrame = actFrameCount;
			path->endFrame = path->startFrame + (path->nodes[0].waitTime * waitScale);

			cur->isSnapping = 2;
		}
		break;

	case 1: // Time to snap and hope
		if( actFrameCount < path->endFrame )
			break;

		path->startFrame = actFrameCount;
		path->endFrame = path->startFrame + (path->nodes[0].waitTime * waitScale);
		cur->isSnapping = 0;

		if( (DistanceBetweenPointsSquared(&act->rotaim,&frog[0]->actor->pos)<900) && 
			!frog[0]->action.dead.time && !frog[0]->action.safe.time ) // If frog is within hitting distance
		{
			NMEDamageFrog(0,cur);
		}
		break;

	case 2: 				// Snap animation
		if( (actFrameCount-path->startFrame) < 0.8*(path->endFrame-path->startFrame) )
			break;

		AnimateActor(act,1,NO,NO,cur->nmeActor->animSpeed, 0, 0);

		if( cur->nmeActor->effects & EF_LIGHTNING )
		{
			VECTOR source, dir;
			SubVector( &dir, &frog[0]->actor->pos, &act->pos );
			MakeUnit( &dir );
			SetVector( &source, &cur->currNormal );
			ScaleVector( &source, path->nodes[0].offset2 );
			AddToVector( &source, &act->pos );
			fx = CreateAndAddSpecialEffect( FXTYPE_LIGHTNING, &source, &dir, 15, 40, 0.2, cur->nmeActor->value1 );
			SetAttachedFXColour( fx, cur->nmeActor->effects );
		}

		cur->isSnapping = 1;
		break;
	}
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateTileSnapper
	Purpose			: Snap at a sequence of tiles
	Parameters		: ENEMY
	Returns			: void
	Info			: waitTime -> snap delay; path speed -> slerp speed
*/
void UpdateTileSnapper( ENEMY *cur )
{
	QUATERNION q1, q2;
	VECTOR toPosition;
	SPECFX *fx;
	ACTOR *act = cur->nmeActor->actor;
	PATH *path = cur->path;
	float speed;

	switch( cur->isSnapping )
	{
	case 0:
		if( actFrameCount < path->endFrame )
			break;

		if( path->toNode >= GET_PATHLASTNODE(path) )	// reached end of path nodes
		{
			path->fromNode	= GET_PATHLASTNODE(path);
			path->toNode	= 1; // Don't snap at the tile we're sitting on
		}
		else
		{
			path->fromNode = path->toNode;
			path->toNode++;
		}

		path->startFrame = actFrameCount;
		path->endFrame = path->startFrame + (path->nodes[path->fromNode].waitTime * waitScale);
		cur->isSnapping = 2;
		break;

	case 1:
		if( actFrameCount < path->endFrame )
			break;

		path->startFrame = actFrameCount;
		path->endFrame = path->startFrame + (path->nodes[path->fromNode].waitTime * waitScale);
		cur->isSnapping = 0;

		// If the frog is on our current target tile
		if( (path->nodes[path->fromNode].worldTile == currTile[0]) && (!frog[0]->action.dead.time) && (!frog[0]->action.safe.time) )
			NMEDamageFrog(0,cur);
		break;

	case 2:
		if( (actFrameCount-path->startFrame) < 0.8*(path->endFrame-path->startFrame) )
		{
			SetVector( &cur->currNormal, &path->nodes[0].worldTile->normal );
			GetPositionForPathNode(&toPosition,&path->nodes[path->fromNode]);
			SetQuaternion( &q1, &act->qRot );
			ActorLookAt( act, &toPosition, LOOKAT_2D );
			SetQuaternion( &q2, &act->qRot );
			speed = path->nodes[0].speed * gameSpeed;
			if( speed > 0.999 ) speed = 0.999;
			QuatSlerp( &q1, &q2, speed, &act->qRot );
			break;
		}

		if( cur->nmeActor->effects & EF_LIGHTNING )
		{
			VECTOR dir, source;
			GetPositionForPathNode(&toPosition,&path->nodes[path->fromNode]);
			SubVector( &dir, &toPosition, &act->pos );
			if( MagnitudeSquared(&dir) )
			{
				MakeUnit( &dir );
				SetVector( &source, &cur->currNormal );
				ScaleVector( &source, path->nodes[0].offset2 );
				AddToVector( &source, &act->pos );
				
				if( cur->nmeActor->effects & EF_FAST )
					fx = CreateAndAddSpecialEffect( FXTYPE_LIGHTNING, &source, &dir, 5, 40, 0.25, 0.2 );
				if( cur->nmeActor->effects & EF_SLOW )
					fx = CreateAndAddSpecialEffect( FXTYPE_LIGHTNING, &source, &dir, 5, 40, 0.25, 0.8 );
				else
					fx = CreateAndAddSpecialEffect( FXTYPE_LIGHTNING, &source, &dir, 5, 40, 0.25, 0.5 );

				fx->tilt = cur->nmeActor->value1; // Branching factor
				SetAttachedFXColour( fx, cur->nmeActor->effects );
			}
		}

		AnimateActor( act, 1, NO, NO, cur->nmeActor->animSpeed, 0, 0 );
		cur->isSnapping = 1;
		break;
	}
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateVent
	Purpose			: Blow hurt at the frog
	Parameters		: ENEMY
	Returns			: void
	Info			: radius -> delay before first fire; waitTime -> time between bursts; speed -> burst time;
					animSpeed -> particle speed; value1 -> particle lifetime
*/
void UpdateVent( ENEMY *cur )
{
	PLANE2 rebound;
	PATH *path = cur->path;
	SPECFX *fx;
	ACTOR2 *act = cur->nmeActor;
	unsigned long i, t;

	switch( cur->isSnapping )
	{
	case -2:
		path->startFrame = actFrameCount;
		path->endFrame = actFrameCount + (60*act->radius);

		cur->isSnapping = -1;
		break;

	case -1:
		if( actFrameCount > path->endFrame )
			cur->isSnapping = 0;

		break;

	case 0: // Start timer
		// Delay until fire
		path->startFrame = actFrameCount;
		path->endFrame = actFrameCount + (60*path->nodes[0].waitTime);

		cur->isSnapping = 1;
		break;

	case 1:
		if( actFrameCount < path->endFrame )
			break;

		// Delay until stop
		path->startFrame = actFrameCount;
		path->endFrame = actFrameCount + (60*path->nodes[0].speed);
		act->fxCount = 0;

		cur->isSnapping = 2;
		break;

	case 2:
		// Stop firing on timeout, and reset
		if( actFrameCount > path->endFrame )
		{
			cur->isSnapping = 0;
			break;
		}

		if(act->value1)
			t = (path->endFrame-path->startFrame)/act->value1;
		else
			t = 1;

		if( (actFrameCount-path->startFrame) > (act->fxCount*t) )
		{
			act->fxCount++;

			if( cur->nmeActor->effects & EF_FIERYSMOKE )
			{
				fx = CreateAndAddSpecialEffect( FXTYPE_FIERYSMOKE, &cur->nmeActor->actor->pos, &cur->path->nodes->worldTile->normal, 50, cur->nmeActor->animSpeed*path->numNodes, 0, 2.0 );
			}
			else if( cur->nmeActor->effects & EF_LASER )
			{
				fx = CreateAndAddSpecialEffect( FXTYPE_LASER, &cur->nmeActor->actor->pos, &cur->path->nodes->worldTile->normal, 15, cur->nmeActor->animSpeed, 0, 0.5*path->numNodes );
				SetAttachedFXColour( fx, cur->nmeActor->effects );
			}
			else if( cur->nmeActor->effects & EF_SMOKEBURST )
			{
				fx = CreateAndAddSpecialEffect( FXTYPE_SMOKEBURST, &cur->nmeActor->actor->pos, &cur->path->nodes->worldTile->normal, 50, cur->nmeActor->animSpeed*path->numNodes, 0, 1.7 );
				SetAttachedFXColour( fx, cur->nmeActor->effects );
			}
		}

		// Check for collision with frog, and do damage
		for( i=0; i < path->numNodes; i++ )
			if( (path->nodes[i].worldTile == currTile[0]) && (!frog[0]->action.dead.time) && (!frog[0]->action.safe.time) )
				NMEDamageFrog(0,cur);

		break;
	}
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateMoveVerticalNME
	Purpose			: Enemy moves up and down
	Parameters		: ENEMY
	Returns			: void
	Info			: 
*/
void UpdateMoveVerticalNME( ENEMY *cur )
{
	PATH *path = cur->path;
	ACTOR *act = cur->nmeActor->actor;
	VECTOR moveVec;
	float start_offset, end_offset, t;

	// check if this platform has arrived at a path node
	if( actFrameCount > cur->path->endFrame )
	{
		UpdateEnemyPathNodes(cur);
		path->startFrame = path->endFrame + cur->isWaiting * waitScale;
		path->endFrame = path->startFrame + (60*cur->speed);

		if (cur->isWaiting) return;
	}

	if (cur->flags & ENEMY_NEW_MOVEUP)
	{
		start_offset = path->nodes->offset;
		end_offset = path->nodes->offset2;
	}
	else
	{
		start_offset = path->nodes->offset2;
		end_offset = path->nodes->offset;
	}

	// get up vector for this platform
	SetVector(&moveVec,&path->nodes[0].worldTile->normal);
	
	t = (float)(actFrameCount- path->startFrame)/(float)(path->endFrame - path->startFrame);
	ScaleVector(&moveVec, t * end_offset + (1-t) * start_offset);
	AddVector(&act->pos, &path->nodes->worldTile->centre, &moveVec);
}

/*	--------------------------------------------------------------------------------
	Function		: UpdateRotatePathNME
	Purpose			: Enemy that flies around a point
	Parameters		: ENEMY
	Returns			: void
	Info			: 
*/
void UpdateRotatePathNME( ENEMY *cur )
{
	float length;
	VECTOR toPosition;

	length = (float)(actFrameCount - cur->path->startFrame)/(float)(cur->path->endFrame - cur->path->startFrame);
	length *= PI2;
	length += cur->path->nodes->offset2 * PI2; // Fraction of circumference it starts at

	if( cur->speed < 0 )
		length = PI2 - length;

	if( cur->flags & ENEMY_NEW_ROTATEPATH_XZ )
	{
		toPosition.v[X] = cur->path->nodes->worldTile->centre.v[X] + (cur->nmeActor->radius * sinf(length));
		toPosition.v[Z] = cur->path->nodes->worldTile->centre.v[Z] + (cur->nmeActor->radius * cosf(length));
		toPosition.v[Y] = cur->path->nodes->worldTile->centre.v[Y] + cur->path->nodes->offset;
	}
	else if( cur->flags & ENEMY_NEW_ROTATEPATH_XY )
	{
		toPosition.v[X] = cur->path->nodes->worldTile->centre.v[X] + (cur->nmeActor->radius * sinf(length));
		toPosition.v[Y] = cur->path->nodes->worldTile->centre.v[Y] + (cur->nmeActor->radius * cosf(length)) + cur->path->nodes->offset;
		toPosition.v[Z] = cur->path->nodes->worldTile->centre.v[Z];
	}
	else if( cur->flags & ENEMY_NEW_ROTATEPATH_ZY )
	{
		toPosition.v[Y] = cur->path->nodes->worldTile->centre.v[Y] + (cur->nmeActor->radius * cosf(length)) + cur->path->nodes->offset;
		toPosition.v[Z] = cur->path->nodes->worldTile->centre.v[Z] + (cur->nmeActor->radius * sinf(length));
		toPosition.v[X] = cur->path->nodes->worldTile->centre.v[X];
	}

	if( !(cur->flags & ENEMY_NEW_FACEFORWARDS) ) // Look in direction of travel
		ActorLookAt( cur->nmeActor->actor, &toPosition, LOOKAT_ANYWHERE );

	SetVector( &cur->nmeActor->actor->pos, &toPosition );

	if( actFrameCount > cur->path->endFrame )
	{
		cur->path->startFrame = cur->path->endFrame;
		cur->path->endFrame = cur->path->startFrame+(60*Fabs(cur->speed));
	}

	cur->inTile = FindNearestJoinedTile( cur->inTile, &cur->nmeActor->actor->pos );
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateHomingNME
	Purpose			: Enemy moves towards frog
	Parameters		: ENEMY
	Returns			: void
	Info			: 
*/
void UpdateHomingNME( ENEMY *cur )
{
	GAMETILE *chTile;
	VECTOR nmeup, tVec, moveVec;
	float distance=10000, best=-2;
	short bFlag = 0;
	unsigned long i;

	SubVector( &moveVec, &frog[0]->actor->pos, &cur->nmeActor->actor->pos );
	MakeUnit( &moveVec );
	chTile = FindJoinedTileByDirection( cur->inTile, &moveVec );

	// Do check for close direction vector from tile. If none match closely, do not move.
	for( i=0; i<4; i++ )
		if( chTile->tilePtrs[i] )
		{
			// Direction to tile
			SubVector( &tVec, &chTile->tilePtrs[i]->centre, &chTile->centre );
			MakeUnit( &tVec );
			// Cosine of angle between vectors
			distance = DotProduct(&tVec,&moveVec);
			if( distance > best )
				best = distance;
		}
		else
			bFlag = 1; // There is some invalid tile

	// If the best direction match is close enough we can carry on (approx 45 degrees)
	// Also check that we're over a tile.
	if( best > 0.7 )
	{
		ScaleVector( &moveVec, cur->speed * gameSpeed );
		AddVector( &tVec, &moveVec, &cur->nmeActor->actor->pos );
		chTile = FindNearestTile( tVec );
		if( bFlag )
		{
			SubVector( &moveVec, &chTile->centre, &tVec );
			distance = abs(Magnitude( &moveVec ));
		}
		if( distance < 35 || !bFlag )
			cur->nmeActor->actor->pos = tVec;
	}
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateTileHomingNME
	Purpose			: Home in on the frog using tiles
	Parameters		: ENEMY
	Returns			: void
	Info			: waitTime -> radius of activation; 
*/
void UpdateTileHomingNME( ENEMY *cur )
{
	VECTOR up, fwd;
	PATH *path = cur->path;
	ACTOR2 *act = cur->nmeActor;
	float length;

	if( path->numNodes < 3 )
		return;

	if( DistanceBetweenPointsSquared(&act->actor->pos, &frog[0]->actor->pos) > (act->radius*act->radius) )
	{
		cur->isIdle = 0;
		path->nodes[2].worldTile = NULL;
		return;
	}

	if( !cur->isSnapping )
	{
		path->nodes[1].worldTile = NULL;
		path->nodes[2].worldTile = path->nodes[0].worldTile;
		cur->isSnapping = 1; // This is a completely random misuse of isSnapping. And I don't care.
	}

	// If enemy is on the next path node, set startnode worldtile and the next to zero
	if( path->nodes[2].worldTile )
	{
		if( actFrameCount > path->endFrame )
		{
			path->nodes[1].worldTile = path->nodes[2].worldTile;
			path->nodes[2].worldTile = NULL;
		}
		else if( actFrameCount > path->startFrame+(0.5*(path->endFrame-path->startFrame)) )
		{
			cur->inTile = path->nodes[2].worldTile;
		}
	}

	// If we need a new destination tile, find it by the direction to the frog
	if( !path->nodes[2].worldTile )
	{
		VECTOR frogVec;
		SubVector( &frogVec, &currTile[0]->centre, &act->actor->pos ); 
		path->nodes[2].worldTile = FindJoinedTileByDirection( path->nodes[1].worldTile, &frogVec );

		path->startFrame = actFrameCount;
		path->endFrame = path->startFrame + (60*path->nodes[0].speed);
	}

	// Move towards next node - third condition is so fwd is not scaled to zero
	if( path->nodes[1].worldTile && path->nodes[2].worldTile && (actFrameCount > path->startFrame) )
	{
		// Move towards frog
		SubVector(&fwd,&path->nodes[2].worldTile->centre,&path->nodes[1].worldTile->centre);
		length = (float)(actFrameCount - path->startFrame)/(float)(path->endFrame - path->startFrame);
		ScaleVector(&fwd,length);
		AddVector(&act->actor->pos,&fwd,&path->nodes[1].worldTile->centre);

		// Orientate to direction of travel
		MakeUnit( &fwd );
		if (!(cur->flags & ENEMY_NEW_FACEFORWARDS))
			Orientate(&act->actor->qRot,&fwd,&inVec,&path->nodes[1].worldTile->normal);

		// Elevate above gametile
		SetVector( &up, &cur->inTile->normal );
		ScaleVector( &up, path->nodes[0].offset );
		AddToVector( &act->actor->pos, &up );
	}
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateMoveOnMoveNME
	Purpose			: Move when frog has moved
	Parameters		: ENEMY
	Returns			: void
	Info			: radius -> radius of activation; 
*/
void UpdateMoveOnMoveNME( ENEMY *cur )
{
	VECTOR up, fwd;
	PATH *path = cur->path;
	ACTOR2 *act = cur->nmeActor;
	float length, dist;

	if( path->numNodes < 3 )
		return;

	dist = DistanceBetweenPointsSquared(&act->actor->pos, &frog[0]->actor->pos);
	if( dist > (act->radius*act->radius) )
	{
		cur->isIdle = 0;
		path->nodes[2].worldTile = NULL;
		return;
	}
	else if( dist < 33*33 && !frog[0]->action.dead.time && !frog[0]->action.safe.time ) // Trial and error value of 33
	{
		NMEDamageFrog(0,cur);
	}

	if( !cur->isSnapping )
	{
		path->nodes[1].worldTile = NULL;
		path->nodes[2].worldTile = path->nodes[0].worldTile;
		cur->isSnapping = 1; // This is a completely random misuse of isSnapping. And I don't care.
		cur->flags |= ENEMY_NEW_NODAMAGE;
	}

	// If frog has moved
	cur->isIdle += player[0].hasJumped;

	// If enemy is on the next path node, set startnode worldtile and the next to zero
	if( path->nodes[2].worldTile )
	{
		if( actFrameCount > path->endFrame )
		{
			cur->inTile = path->nodes[2].worldTile;
			path->nodes[1].worldTile = path->nodes[2].worldTile;
			path->nodes[2].worldTile = NULL;
		}
	}

	// If we need a new destination tile, find it by the direction to the frog
	if( cur->isIdle && !path->nodes[2].worldTile )
	{
		VECTOR frogVec;
		SubVector( &frogVec, &currTile[0]->centre, &act->actor->pos ); 
		path->nodes[2].worldTile = FindJoinedTileByDirection( path->nodes[1].worldTile, &frogVec );

		path->startFrame = actFrameCount;
		path->endFrame = path->startFrame + (60*path->nodes[0].speed);
		cur->isIdle--;
	}

	// Move towards next node - third condition is so fwd is not scaled to zero
	if( path->nodes[1].worldTile && path->nodes[2].worldTile && (actFrameCount > path->startFrame) )
	{
		// Move towards frog
		SubVector(&fwd,&path->nodes[2].worldTile->centre,&path->nodes[1].worldTile->centre);
		length = (float)(actFrameCount - path->startFrame)/(float)(path->endFrame - path->startFrame);
		ScaleVector(&fwd,length);
		AddVector(&act->actor->pos,&fwd,&path->nodes[1].worldTile->centre);

		// Orientate to direction of travel
		MakeUnit( &fwd );
		if (!(cur->flags & ENEMY_NEW_FACEFORWARDS))
			Orientate(&act->actor->qRot,&fwd,&inVec,&path->nodes[1].worldTile->normal);

		// Elevate above gametile
		SetVector( &up, &cur->inTile->normal );
		ScaleVector( &up, path->nodes[0].offset );
		AddToVector( &act->actor->pos, &up );
	}
}


void UpdateRandomMoveNME( ENEMY *cur )
{
	VECTOR up, fwd;
	PATH *path = cur->path;
	ACTOR2 *act = cur->nmeActor;
	float length;

	if( path->numNodes < 3 )
		return;

	if( !cur->isSnapping )
	{
		path->nodes[2].worldTile = path->nodes[1].worldTile;
		path->nodes[1].worldTile = path->nodes[0].worldTile;
		cur->isSnapping = 1; // This is a completely random misuse of isSnapping. And I don't care.
		cur->path->fromNode = 1;
		cur->path->toNode = 2;
	}

	// If enemy is on the next path node, set startnode worldtile and the next to zero
	if( path->nodes[2].worldTile )
	{
		if( actFrameCount > path->endFrame )
		{
			VECTOR rVec = {0,0,0};
			int r;

			SubVector( &fwd, &path->nodes[2].worldTile->centre, &path->nodes[1].worldTile->centre );
			path->nodes[1].worldTile = path->nodes[2].worldTile;

			r = Random(4);
			if(r==0) rVec.v[X] = 1;
			else if(r==1) rVec.v[X] = -1;
			else if(r==2) rVec.v[Z] = 1;
			else rVec.v[Z] = -1;
			path->nodes[2].worldTile = FindJoinedTileByDirection( path->nodes[1].worldTile, &rVec );
			if( !path->nodes[2].worldTile )
				path->nodes[2].worldTile = path->nodes[1].worldTile;

			if( !(Random((int)act->value1+1)) )
				cur->isWaiting = Random((int)path->nodes[0].waitTime);

			if( cur->flags & ENEMY_NEW_BABYFROG )
			{
				SubVector( &rVec, &path->nodes[2].worldTile->centre, &path->nodes[1].worldTile->centre );
				if( DotProduct(&fwd, &rVec) < 0.99 ) // Only do cartwheel if baby needs to turn
					AnimateActor( act->actor, BABY_ANIM_CARTWHEEL, NO, NO, act->animSpeed, 0, 0 );

				AnimateActor( act->actor, BABY_ANIM_WAVE, NO, YES, act->animSpeed, 0, 0 );
			}

			path->startFrame = actFrameCount + ((Random(cur->isWaiting*0.5)+cur->isWaiting*0.5) * waitScale);
			path->endFrame = path->startFrame + (60*path->nodes[0].speed);
			return;
		}
		else if( actFrameCount > path->startFrame+(0.5*(path->endFrame-path->startFrame)) )
		{
			cur->inTile = path->nodes[2].worldTile;
		}
	}

	// Move towards next node - third condition is so fwd is not scaled to zero
	if( path->nodes[1].worldTile && path->nodes[2].worldTile && (actFrameCount > path->startFrame) )
	{
		if( cur->flags & ENEMY_NEW_BABYFROG )
		{	// Start baby hop the first time we move
			if( DistanceBetweenPointsSquared(&path->nodes[1].worldTile->centre, &act->actor->pos) < 5 )
				AnimateActor( act->actor, BABY_ANIM_HOP, NO, NO, act->animSpeed, 0, 0 );
		}
		SubVector(&fwd,&path->nodes[2].worldTile->centre,&path->nodes[1].worldTile->centre);
		length = (float)(actFrameCount - path->startFrame)/(float)(path->endFrame - path->startFrame);
		ScaleVector(&fwd,length);
		AddVector(&act->actor->pos,&fwd,&path->nodes[1].worldTile->centre);

		// Orientate to direction of travel
		MakeUnit( &fwd );
		if (!(cur->flags & ENEMY_NEW_FACEFORWARDS))
			Orientate(&act->actor->qRot,&fwd,&inVec,&path->nodes[1].worldTile->normal);

		// Elevate above gametile
		SetVector( &up, &cur->inTile->normal );
		ScaleVector( &up, path->nodes[0].offset );
		AddToVector( &act->actor->pos, &up );
	}
}


void UpdateFrogWatcher( ENEMY *cur )
{
	QUATERNION q1, q2;
	ACTOR *act = cur->nmeActor->actor;
	float speed;

	SetQuaternion( &q1, &act->qRot );
	ActorLookAt( act, &frog[0]->actor->pos, LOOKAT_ANYWHERE );
	SetQuaternion( &q2, &act->qRot );
	speed = cur->path->nodes[0].speed * gameSpeed;
	if( speed > 0.999 ) speed = 0.999;
	QuatSlerp( &q1, &q2, speed, &act->qRot );
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateFlappyThing
	Purpose			: Make the enemy fly around in a box
	Parameters		: ENEMY
	Returns			: void
	Info			: waitTime -> how long to wait on a point of Interest; node[0].speed -> slerp speed;
					node[1].speed -> move speed
*/
void UpdateFlappyThing( ENEMY *nme )
{
	PATH *path = nme->path;
	ACTOR *act = nme->nmeActor->actor;
	VECTOR fwd;
	QUATERNION q1, q2, q3;
	float t, best = 10000000, speed;
	unsigned long i;

	// Use default values if the user can't be arsed
	if( path->nodes[0].offset == path->nodes[1].offset )
	{
		path->nodes[0].offset = 0;
		path->nodes[1].offset = 10;
		path->nodes[0].speed = 0.07;
		path->nodes[1].speed = 1;
	}

	// If nme is very close to destination (stored in nmeactor->actor->rotaim) then choose a new destination.
	if( (DistanceBetweenPointsSquared(&act->pos,&act->rotaim) < 300) || !Magnitude(&act->rotaim) ) // 10 units apart
	{
		VECTOR up;

		// If just got to the special node we're aiming for, wait at it
		if( path->startNode )
		{
			path->startFrame = actFrameCount + (path->nodes[path->startNode].waitTime*60);
			nme->isWaiting = 1;
			path->startNode = 0;
			return;
		}

		// On a random chance, aim at the closest special node (DOESN'T INCLUDE OFFSET!)
		if( Random(100)>80 )
			for( i=2; i<path->numNodes; i++ )
			{
				t = DistanceBetweenPointsSquared( &act->pos, &path->nodes[i].worldTile->centre );
				if( t < best )
				{
					best = t;
					path->startNode = i;
				}
			}

		// If we just found a node to aim at, go there
		if( path->startNode )
		{
			SetVector( &up, &path->nodes[path->startNode].worldTile->normal );
			ScaleVector( &up, path->nodes[path->startNode].offset );
			AddVector( &act->rotaim, &up, &path->nodes[path->startNode].worldTile->centre );
		}
		else // Else pick a random point in space
		{
			VECTOR p1, p2;
			int dX, dY, dZ;

			SetVector( &up, &path->nodes[0].worldTile->normal );
			ScaleVector( &up, path->nodes[0].offset );
			AddVector( &p1, &path->nodes[0].worldTile->centre, &up );

			SetVector( &up, &path->nodes[1].worldTile->normal );
			ScaleVector( &up, path->nodes[1].offset );
			AddVector( &p2, &path->nodes[1].worldTile->centre, &up );

			// Get dimensions of the box we can move in.
			dX = Fabs(p2.v[0] - p1.v[0]);
			dY = Fabs(p2.v[1] - p1.v[1]);
			dZ = Fabs(p2.v[2] - p1.v[2]);

			p1.v[0] = min(p2.v[0], p1.v[0]);
			p1.v[1] = min(p2.v[1], p1.v[1]);
			p1.v[2] = min(p2.v[2], p1.v[2]);

			// Random destination within the valid box
			act->rotaim.v[0] = p1.v[0]+Random(dX);
			act->rotaim.v[1] = p1.v[1]+Random(dY);
			act->rotaim.v[2] = p1.v[2]+Random(dZ);
		}
	}

	// Store current orientation
	SetQuaternion( &q1, &act->qRot );
	
	// Set direction to desired point
	SubVector(&fwd,&act->rotaim,&act->pos);
	MakeUnit(&fwd);

	// Skewer a line to rotate around, and make a rotation
	CrossProduct((VECTOR *)&q3,&inVec,&fwd);
	t = DotProduct(&inVec,&fwd);
	if (t<-0.999)
		t=-0.999;
	if (t>0.999)
		t = 0.999;
	if(t<0.001 && t>-0.001) // If its trying to move backwards, make it turn
		t = 0.1;
	q3.w=acos(t);
	// Duh.
	GetQuaternionFromRotation(&q2,&q3);

	// Slerp between current and desired orientation
	speed = path->nodes[0].speed * gameSpeed;
	if( speed > 0.999 ) speed = 0.999;
	QuatSlerp( &q1, &q2, speed, &act->qRot );

	// Move forwards a bit in direction of facing
	RotateVectorByQuaternion( &fwd, &inVec, &act->qRot );
	ScaleVector( &fwd, path->nodes[1].speed*gameSpeed );
	AddVector( &act->pos, &fwd, &act->pos );

	nme->flags |= ENEMY_NEW_NODAMAGE;
}


/*	--------------------------------------------------------------------------------
	Function		: InitEnemyLinkedList
	Purpose			: initialises the enemy linked list
	Parameters		: 
	Returns			: void
	Info			: 
*/
void SlerpWaitingFlappyThing( ENEMY *cur )
{
	QUATERNION q1, q2, q3;
	float speed;

	SetQuaternion( &q1, &cur->nmeActor->actor->qRot );
	SetVector( (VECTOR *)&q3, &cur->path->nodes[cur->path->startNode].worldTile->normal );
	q3.w = 0;
	GetQuaternionFromRotation( &q2, &q3 );
	speed = cur->path->nodes[0].speed * gameSpeed;
	if( speed > 0.999 ) speed = 0.999;
	QuatSlerp( &q1, &q2, speed, &cur->nmeActor->actor->qRot );
}


ENEMY *CreateAndAddEnemy(char *eActorName, int flags, long ID, PATH *path, float animSpeed )
{
	int enemyType = 0;
	float shadowRadius = 0;
	int initFlags,i;

	ENEMY *newItem = (ENEMY *)JallocAlloc(sizeof(ENEMY),YES,"NME");
	AddEnemy(newItem);
	newItem->flags = flags;

	initFlags |= INIT_ANIMATION;

	if( newItem->flags & ENEMY_NEW_SHADOW )
	{
		initFlags |= INIT_SHADOW;
		shadowRadius = 20;
	}

	// create and add the nme actor
	newItem->nmeActor = CreateAndAddActor(eActorName,0,0,0,initFlags,0,0);
	if(newItem->nmeActor->actor->objectController)
		InitActorAnim(newItem->nmeActor->actor);
	
	newItem->reactiveNumber = GetReactiveAnimNumber(eActorName);

	// set shadow radius (if applicable)
	if(shadowRadius)
		if (newItem->nmeActor->actor->shadow)
			newItem->nmeActor->actor->shadow->radius = shadowRadius;

	AnimateActor(newItem->nmeActor->actor,0,YES,NO,animSpeed, 0, 0);
	newItem->nmeActor->animSpeed = animSpeed;
	newItem->nmeActor->actor->scale.v[X] = 1.5F;
	newItem->nmeActor->actor->scale.v[Y] = 1.5F;
	newItem->nmeActor->actor->scale.v[Z] = 1.5F;

	newItem->active			= 1;
	
	newItem->path			= NULL;
	newItem->inTile			= NULL;
	
	newItem->speed			= 1.0F;
	newItem->startSpeed		= 1.0F;
	newItem->accel			= 0.0F;
	newItem->uid = ID;
	newItem->isSnapping = 0;
	newItem->doNotMove = 0;

	AssignPathToEnemy(newItem,path,0);

	if( newItem->flags & ENEMY_NEW_SLERPPATH )
		newItem->Update = UpdateSlerpPathNME;
	else if( newItem->flags & ENEMY_NEW_FOLLOWPATH )
		newItem->Update = UpdatePathNME;
	else if( newItem->flags & ENEMY_NEW_WATCHFROG )
		newItem->Update = UpdateFrogWatcher;
	else if( newItem->flags & ENEMY_NEW_SNAPFROG )
		newItem->Update = UpdateSnapper;
	else if( newItem->flags & ENEMY_NEW_SNAPTILES )
		newItem->Update = UpdateTileSnapper;
	else if( newItem->flags & ENEMY_NEW_VENT )
	{
		newItem->isSnapping = -2;
		newItem->Update = UpdateVent;
	}
	else if( (newItem->flags & ENEMY_NEW_MOVEUP) || (newItem->flags & ENEMY_NEW_MOVEDOWN) )
		newItem->Update = UpdateMoveVerticalNME;
	else if( (newItem->flags & ENEMY_NEW_ROTATEPATH_XZ ) || (newItem->flags & ENEMY_NEW_ROTATEPATH_XY) || (newItem->flags & ENEMY_NEW_ROTATEPATH_ZY) )
		newItem->Update = UpdateRotatePathNME;
	else if( newItem->flags & ENEMY_NEW_HOMING )
		newItem->Update = UpdateHomingNME;
	else if( newItem->flags & ENEMY_NEW_MOVEONMOVE )
		newItem->Update = UpdateMoveOnMoveNME;
	else if( newItem->flags & ENEMY_NEW_FLAPPYTHING )
		newItem->Update = UpdateFlappyThing;
	else if( newItem->flags & ENEMY_NEW_RANDOMMOVE )
		newItem->Update = UpdateRandomMoveNME;
	else if( newItem->flags & ENEMY_NEW_TILEHOMING )
		newItem->Update = UpdateTileHomingNME;

	if( newItem->flags & ENEMY_NEW_BABYFROG )
	{
		for( i=0; i<numBabies; i++ )
			if( !babyList[i].baby )
			{
				babyList[i].baby = newItem->nmeActor;
				break;
			}
	}

	SetSoundEffectsForEnemy( newItem );

	return newItem;
}

void AssignPathToEnemy(ENEMY *nme,PATH *path,unsigned long pathFlags)
{
	int i;
	VECTOR enemyStartPos;

	nme->path = path;

	// check if pathnode indices need converting to game tile pointers
	if(pathFlags & PATH_MAKENODETILEPTRS)
		for(i=0; i<path->numNodes; i++)
			nme->path->nodes[i].worldTile = &firstTile[(unsigned long)path->nodes[i].worldTile];

	// set the start position for the enemy
	nme->path->fromNode	= nme->path->startNode;

	if(nme->flags & ENEMY_NEW_FORWARDS)
	{
		// this enemy moves forward thru path nodes
		nme->flags |= ENEMY_NEW_FOLLOWPATH;
		nme->path->toNode = nme->path->fromNode + 1;

		if(nme->path->toNode > GET_PATHLASTNODE(path))
			nme->path->toNode = 0;
	}
	else if(nme->flags & ENEMY_NEW_BACKWARDS)
	{
		// this enemy moves backward thru path nodes
		nme->flags |= ENEMY_NEW_FOLLOWPATH;
		nme->path->toNode = nme->path->fromNode - 1;

		if(nme->path->toNode < 0)
			nme->path->toNode = GET_PATHLASTNODE(path);
	}
	else if((nme->flags & ENEMY_NEW_MOVEUP) ||
			(nme->flags & ENEMY_NEW_MOVEDOWN))
	{
		// this enemy moves up or down
		nme->path->fromNode = nme->path->toNode = 0;
	}

	// set enemy position to relevant point on path
	GetPositionForPathNode(&enemyStartPos,&path->nodes[nme->path->fromNode]);
	SetVector(&nme->nmeActor->actor->pos,&enemyStartPos);
	NormalToQuaternion(&nme->nmeActor->actor->qRot,&path->nodes[nme->path->fromNode].worldTile->normal);

	// set enemy current 'in' tile and speeds and pause times
	nme->inTile		= path->nodes[nme->path->fromNode].worldTile;
	nme->speed		= path->nodes[nme->path->fromNode].speed;
	nme->startSpeed	= path->nodes[nme->path->fromNode].speed;
	nme->isWaiting	= path->nodes[nme->path->fromNode].waitTime;

	nme->path->startFrame = actFrameCount;
	nme->path->endFrame = (actFrameCount+(60*nme->speed));

	CalcEnemyNormalInterps(nme);
}


/*	--------------------------------------------------------------------------------
	Function		: EnemyReachedTopOrBottomPoint
	Purpose			: checks if an enemy has arrived at top or bottom point (1 node based)
	Parameters		: ENEMY *
	Returns			: BOOL - TRUE if node reached, else FALSE
	Info			: 
*/
BOOL EnemyReachedTopOrBottomPoint(ENEMY *nme)
{
	VECTOR toPos;
	PATH *path = nme->path;

	// check if path extreme point is reached
	if(nme->flags & ENEMY_NEW_MOVEUP)
	{
		// moving up
		GetPositionForPathNodeOffset2(&toPos,&path->nodes[0]);

		if(DistanceBetweenPointsSquared(&nme->nmeActor->actor->pos,&toPos) < (100 * gameSpeed))
			return TRUE;
	}
	else if(nme->flags & ENEMY_NEW_MOVEDOWN)
	{
		// moving down
		GetPositionForPathNode(&toPos,&path->nodes[0]);

		if(DistanceBetweenPointsSquared(&nme->nmeActor->actor->pos,&toPos) < (100 * gameSpeed))
			return TRUE;
	}

	return FALSE;
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateEnemyPathNodes
	Purpose			: updates enemy path move status
	Parameters		: ENEMY *
	Returns			: void
	Info			: 
*/
void UpdateEnemyPathNodes(ENEMY *nme)
{
	VECTOR nmePos;
	int wait;
	PATH *path = nme->path;
	unsigned long flags = nme->flags;
	
	if(flags & ENEMY_NEW_FORWARDS)	// enemy moves forward through path nodes
	{
		if(path->toNode >= GET_PATHLASTNODE(path))	// reached end of path nodes
		{
			if(flags & ENEMY_NEW_PINGPONG)		// this enemy has ping-pong movement
			{
				nme->flags		^= (ENEMY_NEW_FORWARDS | ENEMY_NEW_BACKWARDS);	// reverse enemy path movement
				path->fromNode	= GET_PATHLASTNODE(path);
				path->toNode	= GET_PATHLASTNODE(path) - 1;
			}
			else if(flags & ENEMY_NEW_CYCLE) 	// enemy has cyclic movement
			{
				path->fromNode	= GET_PATHLASTNODE(path);
				path->toNode	= 0;
			}
			else
			{
				path->fromNode	= 0;
				path->toNode	= 1;
				GetPositionForPathNode(&nmePos,&path->nodes[0]);
				SetVector(&nme->nmeActor->actor->pos,&nmePos);
			}
		}
		else
		{
			path->fromNode = path->toNode;
			path->toNode++;
		}
	}
	else if(flags & ENEMY_NEW_BACKWARDS) // enemy moves backwards through path nodes
	{
		if(path->toNode <= 0)
		{
			// reached beginning of path nodes
			// check if this enemy has ping-pong movement
			if(flags & ENEMY_NEW_PINGPONG)
			{
				// reverse enenmy path movement
				nme->flags		^= (ENEMY_NEW_FORWARDS | ENEMY_NEW_BACKWARDS);	// reverse enemy path movement
				path->fromNode	= 0;
				path->toNode	= 1;
			}
			else if(flags & ENEMY_NEW_CYCLE)
			{
				// enenmy has cyclic movement
				path->fromNode	= 0;
				path->toNode	= GET_PATHLASTNODE(path);
			}
			else
			{
				path->fromNode	= GET_PATHLASTNODE(path);
				path->toNode	= GET_PATHLASTNODE(path) - 1;
				GetPositionForPathNode(&nmePos,&path->nodes[GET_PATHLASTNODE(path)]);
				SetVector(&nme->nmeActor->actor->pos,&nmePos);
			}
		}
		else
		{
			path->fromNode = path->toNode;
			path->toNode--;
		}

	}
	else if( flags & (ENEMY_NEW_MOVEDOWN | ENEMY_NEW_MOVEUP) )
	{
		if( flags & ENEMY_NEW_PINGPONG )
			nme->flags	^= (ENEMY_NEW_MOVEUP | ENEMY_NEW_MOVEDOWN);
	}

	nme->speed		= path->nodes[path->fromNode].speed;
	nme->isWaiting	= path->nodes[path->fromNode].waitTime;

	// Stop overshoot when waiting on a path node
	if (nme->isWaiting)
	{
		if( nme->flags & ENEMY_NEW_MOVEDOWN)
			GetPositionForPathNodeOffset2(&nmePos, &path->nodes[path->fromNode]);
		else
			GetPositionForPathNode(&nmePos, &path->nodes[path->fromNode]);

		SetVector(&nme->nmeActor->actor->pos, &nmePos);
	}

	if (nme->flags & ENEMY_NEW_RANDOMSPEED) nme->speed *= ENEMY_RANDOMNESS;
}


/*	--------------------------------------------------------------------------------
	Function		: CalcEnemyNormalInterps
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
void CalcEnemyNormalInterps(ENEMY *nme)
{
	PATH *path;
	PATHNODE *fromNode,*toNode;
	float numSteps;
	VECTOR destNormal,fromPos,toPos;

	path = nme->path;
	if(path->numNodes < 2)
		return;

	fromNode	= &path->nodes[path->fromNode];
	toNode		= &path->nodes[path->toNode];

	// set the current enemy normal to that of the 'from' node and get the dest normal
	SetVector(&nme->currNormal,&fromNode->worldTile->normal);
	SetVector(&destNormal,&toNode->worldTile->normal);

	// calculate deltas for linear interpolation of enemy normal during movement
	SubVector(&nme->deltaNormal,&destNormal,&nme->currNormal);

	// determine number of 'steps' over which to interpolate
	GetPositionForPathNode(&fromPos,fromNode);
	GetPositionForPathNode(&toPos,toNode);
	
	numSteps = DistanceBetweenPoints(&fromPos,&toPos);
	if(numSteps == 0)
		numSteps = 1;
	numSteps /= nme->speed;

	nme->deltaNormal.v[X] /= numSteps;
	nme->deltaNormal.v[Y] /= numSteps;
	nme->deltaNormal.v[Z] /= numSteps;
}


/*	--------------------------------------------------------------------------------
	Function		: SetSoundEffectsForEnemy
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: Add sounds for different enemy types (e.g. motor for moas
*/
void SetSoundEffectsForEnemy( ENEMY *nme )
{
	ACTOR *act;
	
	if (!nme->nmeActor) return;
		
	act = nme->nmeActor->actor;

	if (!act->objectController) return;

	if( !gstrcmp("lomoahrt", act->objectController->object->name) )
		AddAmbientSound( GEN_CLOCK_TOCK, &act->pos, 1000, 100, 100, 3, 0, act );
}


/*	--------------------------------------------------------------------------------
	Function		: InitEnemyLinkedList
	Purpose			: initialises the enemy linked list
	Parameters		: 
	Returns			: void
	Info			: 
*/
void InitEnemyLinkedList()
{
	enemyList.numEntries = 0;
	enemyList.head.next = enemyList.head.prev = &enemyList.head;
}

/*	--------------------------------------------------------------------------------
	Function		: AddEnemy
	Purpose			: adds an enemy to the linked list
	Parameters		: ENEMY *
	Returns			: void
	Info			: 
*/
void AddEnemy(ENEMY *enemy)
{
	if(enemy->next == NULL)
	{
		enemyList.numEntries++;
		enemy->prev = &enemyList.head;
		enemy->next = enemyList.head.next;
		enemyList.head.next->prev = enemy;
		enemyList.head.next = enemy;
	}
}

/*	--------------------------------------------------------------------------------
	Function		: SubEnemy
	Purpose			: removes an enemy from the linked list
	Parameters		: ENEMY *
	Returns			: void
	Info			: 
*/
void SubEnemy(ENEMY *enemy)
{
	if(enemy->next == NULL)
		return;

	if(enemy->path)
		JallocFree((UBYTE**)&enemy->path);

	enemy->prev->next = enemy->next;
	enemy->next->prev = enemy->prev;
	enemy->next = NULL;
	enemyList.numEntries--;

	JallocFree((UBYTE **)&enemy);
}


/*	--------------------------------------------------------------------------------
	Function		: FreeEnemyLinkedList
	Purpose			: frees the enemy linked list
	Parameters		: 
	Returns			: void
	Info			: 
*/
void FreeEnemyLinkedList()
{
	ENEMY *cur,*next;

	// check if any elements in list
	if(enemyList.numEntries == 0)
		return;

	dprintf"Freeing linked list : ENEMY : (%d elements)\n",enemyList.numEntries));

	// traverse enemy list and free elements
	for(cur = enemyList.head.next; cur != &enemyList.head; cur = next)
	{
		next = cur->next;

		SubEnemy(cur);
	}

	// initialise list for future use
	InitEnemyLinkedList();
}



/*
void UpdateCurvePathNME( ENEMY *cur )
{
	VECTOR from, to, future, tan1, tan2, last, face;
	float length;
	PATH *path = cur->path;
	ACTOR *act = cur->nmeActor->actor;
	float gx[4], gy[4], gz[4], cx[4], cy[4], cz[4], t=0, t2, t3, temp;
	int n, i, index;
	
	// first, update the enemy position
	GetPositionForPathNode(&to,&path->nodes[cur->path->toNode]);
	GetPositionForPathNode(&from,&path->nodes[cur->path->fromNode]);

	// Find the node after next
	if(cur->flags & ENEMY_NEW_FORWARDS)
	{
		if(path->toNode >= GET_PATHLASTNODE(path))
		{
			if(cur->flags & ENEMY_NEW_PINGPONG) GetPositionForPathNode( &future, &path->nodes[GET_PATHLASTNODE(path)-1] );
			else if(cur->flags & ENEMY_NEW_CYCLE) GetPositionForPathNode( &future, &path->nodes[0] );
			else GetPositionForPathNode( &future, &path->nodes[0] );
		}
		else GetPositionForPathNode( &future, &path->nodes[path->toNode+1] );
	}
	else if(cur->flags & ENEMY_NEW_BACKWARDS)
	{
		if(path->toNode <= 0)
		{
			if(cur->flags & ENEMY_NEW_PINGPONG) GetPositionForPathNode( &future, &path->nodes[1] );
			else if(cur->flags & ENEMY_NEW_CYCLE) GetPositionForPathNode( &future, &path->nodes[GET_PATHLASTNODE(path)] );
			else GetPositionForPathNode( &future, &path->nodes[GET_PATHLASTNODE(path)-1] );
		}
		else GetPositionForPathNode( &future, &path->nodes[path->toNode-1] );
	}

	// Find tangents (kind of) at last and next path nodes
	SubVector( &tan1, &to, &from );
	SubVector( &tan2, &future, &to );
	
	length = (float)(actFrameCount - cur->path->startFrame)/(float)(cur->path->endFrame - cur->path->startFrame);

	length = (length*length*length);

	// Set geometry components
	gx[0] = from.v[X];
	gx[1] = to.v[X];
	gx[2] = tan2.v[X]*-1;
	gx[3] = tan1.v[X];
//	gy[0] = from.v[Y];
//	gy[1] = to.v[Y];
//	gy[2] = tan2.v[Y]*-1;
//	gy[3] = tan1.v[Y];
	gz[0] = from.v[Z];
	gz[1] = to.v[Z];
	gz[2] = tan2.v[Z]*-1;
	gz[3] = tan1.v[Z];

	t = length;
	t2 = t*t;
	t3 = t2*t;

	act->pos.v[X] = ((2*t3 - 3*t2 + 1)*gx[0]) + ((-2*t3 + 3*t2)*gx[1]) + ((t3 - 2*t2 + t)*gx[2]) + ((t2-t2)*gx[3]);
	act->pos.v[Y] = 0;//((2*t3 - 3*t2 + 1)*gy[0]) + ((-2*t3 + 3*t2)*gy[1]) + ((t3 - 2*t2 + t)*gy[2]) + ((t2-t2)*gy[3]);
	act->pos.v[Z] = ((2*t3 - 3*t2 + 1)*gz[0]) + ((-2*t3 + 3*t2)*gz[1]) + ((t3 - 2*t2 + t)*gz[2]) + ((t2-t2)*gz[3]);

	t = length*0.9;
	t2 = t*t;
	t3 = t2*t;

	last.v[X] = ((2*t3 - 3*t2 + 1)*gx[0]) + ((-2*t3 + 3*t2)*gx[1]) + ((t3 - 2*t2 + t)*gx[2]) + ((t2-t2)*gx[3]);
	last.v[Y] = 0;//((2*t3 - 3*t2 + 1)*gy[0]) + ((-2*t3 + 3*t2)*gy[1]) + ((t3 - 2*t2 + t)*gy[2]) + ((t2-t2)*gy[3]);
	last.v[Z] = ((2*t3 - 3*t2 + 1)*gz[0]) + ((-2*t3 + 3*t2)*gz[1]) + ((t3 - 2*t2 + t)*gz[2]) + ((t2-t2)*gz[3]);
	
	SubVector( &face, &act->pos, &last );
	MakeUnit( &face );

	// And back to normal path stuff
	AddToVector(&cur->currNormal,&cur->deltaNormal);

	if( !(cur->flags & ENEMY_NEW_FACEFORWARDS) )
		Orientate(&cur->nmeActor->actor->qRot,&face,&inVec,&cur->currNormal);
	else // Need to do this so normals still work
		Orientate(&cur->nmeActor->actor->qRot,&inVec,&inVec,&cur->currNormal);

	// check if this enemy has arrived at a path node
	if( actFrameCount > cur->path->endFrame )
	{
		UpdateEnemyPathNodes(cur);
		CalcEnemyNormalInterps(cur);

		cur->path->startFrame = cur->path->endFrame + cur->isWaiting * waitScale;

		if (cur->flags & ENEMY_NEW_RANDOMSPEED)
			cur->path->endFrame = cur->path->startFrame + (60*((float)Random(100)/100.0F) );
		else
			cur->path->endFrame = cur->path->startFrame + (60*cur->speed);
	}

	if( actFrameCount < cur->path->startFrame+(0.5*(cur->path->endFrame-cur->path->startFrame)) )
		cur->inTile = cur->path->nodes[cur->path->fromNode].worldTile;
	else
		cur->inTile = cur->path->nodes[cur->path->toNode].worldTile;
}
*/