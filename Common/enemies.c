/*

	This file is part of Frogger2, (c) 1997 Interactive Studios Ltd.


	File		: enemies.c
	Programmer	: Jim Hubbard
	Date		: 1/12/99
	Info		: Animation:
					Different standards for groups of enemy types, which at the moment is

					Homers:
					Walk
					Idle
					Attack

					Path based:
					Walk
					Walk

					Snappers:
					Idle
					Full Attack
					Any extra idles, to be played occasionally

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>

#include "incs.h"

#define ANI_FACE		(1 << 0)
#define ANI_CENTRE		(1 << 1)
#define ANI_FIXED		(1 << 2)
#define ANI_ANTIFACE	(1 << 3)

#define ANI_REACTIVE	(1 << 31)


// lookup arrays for reactive and non-reactive deaths
REACTIVEANIM reactiveAnims[] = {
#include "x:\teamspirit\pcversion\reactive.txt"
};

DEATHANIM_FUNC deathAnims[NUM_DEATHTYPES*2];


// Basic enemy stuff
ENEMYLIST enemyList;						// the enemy linked list

#define ENEMY_RANDOMNESS (0.5 + (Random(100)/100.0))	// returns a value from 0.5 to 1.5

void NMEDamageFrog( int pl, ENEMY *nme );
void DoEnemyCollision( ENEMY *cur );
void RotateWaitingNME( ENEMY *cur );
void SlerpWaitingFlappyThing( ENEMY *cur );

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


long hurtJumpTime		= 30;
float hurtJumpHeight	= 50.0f;

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
	ENEMY *cur;

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

	if( cur->flags & ENEMY_NEW_VENT ) return;

	if (gameState.multi==SINGLEPLAYER)
	{
		if( (cur->flags & ENEMY_NEW_RADIUSBASEDCOLLISION))
		{
			if (!player[0].dead.time && (DistanceBetweenPointsSquared(&frog[0]->actor->pos,&act->actor->pos)<((frog[0]->radius+act->radius)*(frog[0]->radius+act->radius))) )
			{
				if( cur->flags & ENEMY_NEW_BABYFROG )
				{
					if( PickupBabyFrog( cur->nmeActor, cur->inTile ) )
						cur->active = 0;
				}
				else if( !player[0].safe.time ) NMEDamageFrog(0,cur);
			}
		}
		else
		{
			if( (currTile[0] == cur->inTile) && !player[0].dead.time && 
				(!(player[0].isSuperHopping) || (cur->flags & ENEMY_NEW_NOJUMPOVER)) &&
				!(player[0].frogState & FROGSTATUS_ISFLOATING) )
			{
				if( cur->flags & ENEMY_NEW_BABYFROG )
				{
					if( PickupBabyFrog( cur->nmeActor, cur->inTile ) )
						cur->active = 0;
				}
				else if( !player[0].safe.time ) NMEDamageFrog(0,cur);
			}
		}
	}
	else
	{
		int i;
		for (i=0; i<NUM_FROGS; i++)
			if( (cur->flags & ENEMY_NEW_RADIUSBASEDCOLLISION) && (DistanceBetweenPointsSquared(&frog[i]->actor->pos,&act->actor->pos)<((frog[i]->radius+act->radius)*(frog[i]->radius+act->radius))) )
			{
				if( (cur->flags & ENEMY_NEW_BABYFROG) && multiplayerMode == MULTIMODE_COLLECT )
					PickupBabyFrogMulti( cur, i );
				else if( !player[i].safe.time )
					KillMPFrog(i);
			}
			else if( (currTile[i] == cur->inTile) && !player[i].dead.time && 
					(!(player[i].frogState & FROGSTATUS_ISSUPERHOPPING) || (cur->flags & ENEMY_NEW_NOJUMPOVER)) &&
					!(player[i].frogState & FROGSTATUS_ISFLOATING))
			{
				if( (cur->flags & ENEMY_NEW_BABYFROG) && multiplayerMode == MULTIMODE_COLLECT )
					PickupBabyFrogMulti( cur, i );
				else if( !player[i].safe.time )
					KillMPFrog(i);
			}
	}
}

void NMEDamageFrog( int pl, ENEMY *nme )
{
	if( !nme || (player[pl].frogState & FROGSTATUS_ISSAFE) || (nme->flags & ENEMY_NEW_NODAMAGE) && !(nme->flags & ENEMY_NEW_MOVEONMOVE) )
		return;

	if( nme->flags & ENEMY_NEW_ONEHITKILL )
		player[pl].healthPoints = 0;
	else
		player[pl].healthPoints--;

#ifdef N64_VERSION
	StartRumble(120,1.5,5,ActiveController);
#endif
	
	if(player[pl].healthPoints != 0)
	{
		SPECFX *fx;
		VECTOR pos;

		GTInit( &player[pl].safe, 5 );

		if( (fx = CreateAndAddSpecialEffect( FXTYPE_FROGSHIELD, &frog[pl]->actor->pos, &currTile[pl]->normal, 35, 0, 0, 2 )) )
		{
			fx->follow = frog[pl]->actor;
			fx->tilt = 0.8;
			fx->spin = 0.05;
		}
		// Special hurt anim
		if( (nme->reactiveNumber != -1) && !(reactiveAnims[nme->reactiveNumber].type & ANI_REACTIVE) )
			deathAnims[reactiveAnims[nme->reactiveNumber].animFrog] (pl);
		else
			deathAnims[0] (pl); // Normal damage
	}
	else
	{
//		player[pl].healthPoints = 1;
		player[pl].frogState |= FROGSTATUS_ISDEAD;

		if (nme->reactiveNumber!=-1)
		{
			if (reactiveAnims[nme->reactiveNumber].type & ANI_FACE)
				SetQuaternion(&(frog[pl]->actor->qRot),&(nme->nmeActor->actor->qRot));
			else if( reactiveAnims[nme->reactiveNumber].type & ANI_ANTIFACE )
			{
				VECTOR fwd;
				SubVector( &fwd, &nme->nmeActor->actor->pos, &frog[pl]->actor->pos );
				MakeUnit( &fwd );
				Orientate( &frog[pl]->actor->qRot, &fwd, &currTile[pl]->normal );
			}
			
			if (reactiveAnims[nme->reactiveNumber].type & ANI_CENTRE)
				SetVector(&(frog[pl]->actor->pos),&(nme->nmeActor->actor->pos));

			if (reactiveAnims[nme->reactiveNumber].type & ANI_FIXED)
				nme->doNotMove = 1;

			// Matts reactive animations
			if( reactiveAnims[nme->reactiveNumber].type & ANI_REACTIVE )
			{
				AnimateActor(frog[pl]->actor,reactiveAnims[nme->reactiveNumber].animFrog, NO, NO, 0.25F, 0, 0);
				AnimateActor(nme->nmeActor->actor,reactiveAnims[nme->reactiveNumber].animChar, NO, NO, 0.25F, 0, 0);

				GTInit( &player[pl].dead, 3 );
			}
			else // My death animations
			{
				deathAnims[reactiveAnims[nme->reactiveNumber].animFrog+NUM_DEATHTYPES] (pl);
				if( reactiveAnims[nme->reactiveNumber].animChar != -1 )
				{
					AnimateActor( nme->nmeActor->actor, reactiveAnims[nme->reactiveNumber].animChar, NO, NO, 0.25, 0, 0 );
					AnimateActor( nme->nmeActor->actor, 0, YES, YES, 0.25, 0, 0 );
				}
			}
		}
		else deathAnims[NUM_DEATHTYPES] (pl); // DEATHBY_NORMAL
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
			angle = DotProduct( &currTile[0]->dirVector[frogFacing[0]], &moveVec );

			if (angle<-0.9)
				angle = -0.9;
			if (angle>0.9)
				angle = 0.9;

			angle = acos(angle);
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

				// if enemy is a pusher and currently waiting - should still 'push' away frog
				if(cur->flags & ENEMY_NEW_PUSHESFROG)
				{
					// perhaps not the best way to do this, though...
					if(cur->inTile == currTile[0])
					{
						SetVector(&frog[0]->actor->pos,&prevTile->centre);
						currTile[0] = prevTile;
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

	if( gameState.multi == SINGLEPLAYER )
		UpdateBabies();
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
	Orientate(&q,&fwd,&cur->currNormal);

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
	VECTOR fromPosition,toPosition, fwd, dir, cross;
	QUATERNION q;
	float length, dp;
	long progress;
	
	// first, update the enemy position
	GetPositionForPathNode(&toPosition,&cur->path->nodes[cur->path->toNode]);
	GetPositionForPathNode(&fromPosition,&cur->path->nodes[cur->path->fromNode]);
	
	SubVector(&fwd,&toPosition,&fromPosition);
	
	progress = actFrameCount - cur->path->startFrame;
	if( !progress ) progress = 1;

	length = (float)progress/(float)(cur->path->endFrame - cur->path->startFrame);
	
	ScaleVector(&fwd,length);

	if (!cur->doNotMove)
		AddVector(&cur->nmeActor->actor->pos,&fwd,&fromPosition);

	MakeUnit (&fwd);

	AddToVector(&cur->currNormal,&cur->deltaNormal);
	MakeUnit( &cur->currNormal );

	if (!cur->doNotMove)
		if( !(cur->flags & ENEMY_NEW_FACEFORWARDS) )
			Orientate(&cur->nmeActor->actor->qRot,&fwd,&cur->currNormal);
		else // Need to do this so normals still work
			Orientate( &cur->nmeActor->actor->qRot, &cur->path->nodes->worldTile->dirVector[cur->facing], &cur->currNormal );

	// check if this enemy has arrived at a path node
	if( actFrameCount >= cur->path->endFrame )
	{
		do {
			UpdateEnemyPathNodes(cur);
			cur->path->startFrame = cur->path->endFrame + cur->isWaiting * waitScale;

			if (cur->flags & ENEMY_NEW_RANDOMSPEED)
				cur->path->endFrame = cur->path->startFrame + (60*((float)Random(100)/100.0F) );
			else
				cur->path->endFrame = cur->path->startFrame + (60*cur->speed);
		}
		while (actFrameCount >= cur->path->endFrame && cur->isWaiting >= 0);

		CalcEnemyNormalInterps(cur);

	}

	/*	------------- Push blocks, big fat mofo blocks that push --------------- */
	if (cur->flags & ENEMY_NEW_PUSHESFROG)
	{
		VECTOR v;
		AddVector(&v, &cur->nmeActor->actor->pos, &fwd);
		SubFromVector(&v, &frog[0]->actor->pos);

		if( MagnitudeSquared(&v) < (50*50) )
		{
			PushFrog(&cur->nmeActor->actor->pos, &fwd, 0);
			//player[0].canJump = 0;
			AnimateActor(frog[0]->actor, FROG_ANIM_FORWARDSOMERSAULT, NO, NO, 0.6f, 0, 0);
		}
	}

	// Occasionally do the extra walk anim, then go back to normal
/*	if( Random(1000) > 995 )
	{
		AnimateActor( cur->nmeActor->actor, NMEANIM_PATH_WALK2, NO, YES, cur->nmeActor->animSpeed, 0, 0 );
		AnimateActor( cur->nmeActor->actor, NMEANIM_PATH_WALK1, YES, YES, cur->nmeActor->animSpeed, 0, 0 );
	}
*/
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
	VECTOR fromPosition,toPosition, fwd;
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
	MakeUnit((VECTOR *)&q3);
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
//	RotateVectorByQuaternion( &fwd, &inVec, &act->qRot );
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

		// Random Animation
		if (Random(1000)>980)
		{
			AnimateActor( act, NMEANIM_SNAP_EXTRA1+Random(3), NO, YES, cur->nmeActor->animSpeed, 0, 0);
			AnimateActor( act, NMEANIM_SNAP_IDLE, YES, YES, cur->nmeActor->animSpeed, 0, 0);
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
			path->endFrame = path->startFrame + (cur->nmeActor->radius*60);

			cur->isSnapping = 2;
		}
		break;

	case 1: // Time to snap and hope
		if( actFrameCount < path->endFrame )
			break;

		path->startFrame = actFrameCount;
		path->endFrame = path->startFrame + (path->nodes[0].waitTime * waitScale);
		cur->isSnapping = 0;

		if( (DistanceBetweenPointsSquared(&act->rotaim,&frog[0]->actor->pos)<400) && 
			!player[0].dead.time && !player[0].safe.time ) // If frog is within hitting distance
		{
			NMEDamageFrog(0,cur);
		}
		break;

	case 2: 				// Snap animation
		if( (actFrameCount-path->startFrame) < 0.8*(path->endFrame-path->startFrame) )
			break;

		// Dirty hack for toy fair demo - fix in reactive file later
		if( !(player[0].worldNum == WORLDID_GARDEN && player[0].levelNum == LEVELID_GARDEN1) )
		{
			AnimateActor( act, NMEANIM_SNAP_ATTACK, NO, NO, cur->nmeActor->animSpeed, 0, 0);
			AnimateActor( act, NMEANIM_SNAP_IDLE, YES, YES, cur->nmeActor->animSpeed, 0, 0);
		}

		if( cur->nmeActor->effects & EF_LIGHTNING )
		{
			VECTOR source, dir;
			SubVector( &dir, &frog[0]->actor->pos, &act->pos );
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

			if( fx )
			{
				fx->tilt = cur->nmeActor->value1; // Branching factor
				SetAttachedFXColour( fx, cur->nmeActor->effects );
			}
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
	case -2:
		path->startFrame = actFrameCount;
		path->endFrame = actFrameCount + (60*cur->nmeActor->radius);

		cur->isSnapping = -1;
		break;

	case -1:
		if( actFrameCount >= path->endFrame )
			cur->isSnapping = 0;

		break;

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
		if( (path->nodes[path->fromNode].worldTile == currTile[0]) && (!player[0].dead.time) && (!player[0].safe.time) && !player[0].isSuperHopping )
			NMEDamageFrog(0,cur);
		break;

	case 2:
		if( (actFrameCount-path->startFrame) < 0.8*(path->endFrame-path->startFrame) )
		{
			VECTOR dir;
			SetVector( &cur->currNormal, &path->nodes->worldTile->normal );
			GetPositionForPathNode(&toPosition,&path->nodes[path->fromNode]);
			SubVector( &dir, &toPosition, &act->pos );
			MakeUnit( &dir );
			SetQuaternion( &q1, &act->qRot );
			Orientate( &act->qRot, &dir, &cur->currNormal ); //ActorLookAt( act, &toPosition, LOOKAT_2D );
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

				if( fx )
				{
					fx->tilt = cur->nmeActor->value1; // Branching factor
					SetAttachedFXColour( fx, cur->nmeActor->effects );
				}
			}
		}

		AnimateActor( act, NMEANIM_SNAP_ATTACK, NO, NO, cur->nmeActor->animSpeed, 0, 0 );
		AnimateActor( act, NMEANIM_SNAP_IDLE, YES, YES, cur->nmeActor->animSpeed, 0, 0 );

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
	PATH *path = cur->path;
	SPECFX *fx;
	ACTOR2 *act = cur->nmeActor;
	VECTOR pos;
	unsigned long i, t;

	switch( cur->isSnapping )
	{
	case -2:
		path->startFrame = actFrameCount;
		path->endFrame = actFrameCount + (60*act->radius);

		cur->isSnapping = -1;
		break;

	case -1:
		if( actFrameCount >= path->endFrame )
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

		// Play sound when firing
		if( path->nodes->sample )
			PlaySample( path->nodes->sample, &act->actor->pos, 0, SAMPLE_VOLUME, -1 );

		cur->isSnapping = 2;
		break;

	case 2:
		// Stop firing on timeout, and reset
		if( actFrameCount >= path->endFrame )
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
				fx = CreateAndAddSpecialEffect( FXTYPE_FIERYSMOKE, &act->actor->pos, &path->nodes->worldTile->normal, 50, act->animSpeed*path->numNodes, 0, 2.0 );
			}
			else if( cur->nmeActor->effects & EF_LASER )
			{
				fx = CreateAndAddSpecialEffect( FXTYPE_LASER, &act->actor->pos, &path->nodes->worldTile->normal, 15, act->animSpeed, 0, 0.5*path->numNodes );
				SetAttachedFXColour( fx, act->effects );
			}
			else if( cur->nmeActor->effects & EF_SMOKEBURST )
			{
				fx = CreateAndAddSpecialEffect( FXTYPE_SMOKEBURST, &act->actor->pos, &path->nodes->worldTile->normal, 50, act->animSpeed*path->numNodes, 0, 1.7 );
				SetAttachedFXColour( fx, act->effects );
			}
			else if( cur->nmeActor->effects & EF_LIGHTNING ) // Make a big lighning thing between first and last nodes
			{
				VECTOR p1, p2;

				GetPositionForPathNode( &p1, &path->nodes[0] );
				GetPositionForPathNode( &p2, &path->nodes[path->numNodes-1] );
				CreateLightningEffect( &p1, &p2, act->effects, (60*path->nodes[0].speed)/((act->value1==0)?1:act->value1) );
			}
		}

		// Check for collision with frog, and do damage
		for( i=0; i < path->numNodes; i++ )
		{
			if( ((path->nodes[i].worldTile == currTile[0]) || (path->nodes[i].worldTile == destTile[0])) && (!player[0].dead.time) && (!player[0].safe.time) )
			{
				// If radius based, do a cylindrical type collision on the vent beam
				if( cur->flags & ENEMY_NEW_RADIUSBASEDCOLLISION )
				{
					// Get position in the beam to check for
					SetVector( &pos, &path->nodes[i].worldTile->normal );
					ScaleVector( &pos, path->nodes[i].offset );
					AddToVector( &pos, &path->nodes[i].worldTile->centre );
					if( DistanceBetweenPointsSquared(&frog[0]->actor->pos, &pos) > 1600 )
						continue;
				}
				
				NMEDamageFrog(0,cur);
			}
		}

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
	if( actFrameCount >= cur->path->endFrame )
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
	else// if( cur->flags & ENEMY_NEW_ROTATEPATH_ZY )
	{
		toPosition.v[Y] = cur->path->nodes->worldTile->centre.v[Y] + (cur->nmeActor->radius * cosf(length)) + cur->path->nodes->offset;
		toPosition.v[Z] = cur->path->nodes->worldTile->centre.v[Z] + (cur->nmeActor->radius * sinf(length));
		toPosition.v[X] = cur->path->nodes->worldTile->centre.v[X];
	}

	if( !(cur->flags & ENEMY_NEW_FACEFORWARDS) ) // Look in direction of travel
		ActorLookAt( cur->nmeActor->actor, &toPosition, LOOKAT_ANYWHERE );

	SetVector( &cur->nmeActor->actor->pos, &toPosition );

	if( actFrameCount >= cur->path->endFrame )
	{
		cur->path->startFrame = cur->path->endFrame;
		cur->path->endFrame = cur->path->startFrame+(60*Fabs(cur->speed));
	}

	cur->inTile = FindNearestJoinedTile( cur->inTile, &cur->nmeActor->actor->pos );

	// Occasionally do the extra walk anim, then go back to normal
	if( Random(1000) > 995 )
	{
		AnimateActor( cur->nmeActor->actor, NMEANIM_PATH_WALK2, NO, YES, cur->nmeActor->animSpeed, 0, 0 );
		AnimateActor( cur->nmeActor->actor, NMEANIM_PATH_WALK1, YES, YES, cur->nmeActor->animSpeed, 0, 0 );
	}
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
	VECTOR fwd, at;

	// For now, screw tiles completely and just move. Make radius based if they need to collide
	SubVector( &fwd, &frog[0]->actor->pos, &cur->nmeActor->actor->pos );
	MakeUnit( &fwd );

	AddVector( &at, &fwd, &cur->nmeActor->actor->pos );

	if (!cur->doNotMove)
		if( !(cur->flags & ENEMY_NEW_FACEFORWARDS) )
			ActorLookAt( cur->nmeActor->actor, &at, LOOKAT_ANYWHERE );
			//Orientate(&cur->nmeActor->actor->qRot,&fwd,&inVec,&cur->currNormal);
		else // Need to do this so normals still work
			Orientate(&cur->nmeActor->actor->qRot,&inVec,&upVec);

	ScaleVector( &fwd, cur->speed * gameSpeed );
	AddVector( &cur->nmeActor->actor->pos, &fwd, &cur->nmeActor->actor->pos );
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
		AnimateActor( act->actor, NMEANIM_HOMER_IDLE, YES, YES, act->animSpeed, 0, 0);
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
		if( actFrameCount >= path->endFrame )
		{
			path->nodes[1].worldTile = path->nodes[2].worldTile;
			path->nodes[2].worldTile = NULL;
		}
		else if( actFrameCount > path->startFrame+(0.5*(path->endFrame-path->startFrame)) )
		{
			cur->inTile->state = TILESTATE_NORMAL;
			cur->inTile = path->nodes[2].worldTile;
			cur->inTile->state = TILESTATE_OCCUPIED;
		}
	}

	// If we need a new destination tile, find it by the direction to the frog
	if( !path->nodes[2].worldTile )
	{
		VECTOR frogVec;
		SubVector( &frogVec, &currTile[0]->centre, &act->actor->pos ); 
		path->nodes[2].worldTile = FindJoinedTileByDirection( path->nodes[1].worldTile, &frogVec );
		if( path->nodes[2].worldTile )
			if( path->nodes[2].worldTile->state == TILESTATE_OCCUPIED )
				path->nodes[2].worldTile = NULL;

		path->startFrame = actFrameCount;
		path->endFrame = path->startFrame + (60*path->nodes[0].speed);

		AnimateActor( act->actor, NMEANIM_HOMER_WALK, YES, YES, act->animSpeed, 0, 0);
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
			Orientate(&act->actor->qRot,&fwd,&path->nodes[1].worldTile->normal);

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
		AnimateActor( act->actor, NMEANIM_HOMER_IDLE, YES, YES, act->animSpeed, 0, 0);
		return;
	}
	else if( dist < 33*33 && !player[0].dead.time && !player[0].safe.time ) // Trial and error value of 33
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
		if( actFrameCount >= path->endFrame )
		{
			cur->inTile->state = TILESTATE_NORMAL;
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
		path->nodes[2].worldTile = FindJoinedTileByDirectionAndType( path->nodes[1].worldTile, &frogVec, TILESTATE_NORMAL );
		if( path->nodes[2].worldTile )
		{
			if( path->nodes[2].worldTile->state == TILESTATE_OCCUPIED )
				path->nodes[2].worldTile = NULL;
			else
				path->nodes[2].worldTile->state = TILESTATE_OCCUPIED;
		}
		path->startFrame = actFrameCount;
		path->endFrame = path->startFrame + (60*path->nodes[0].speed);
		cur->isIdle--;
		AnimateActor( act->actor, NMEANIM_HOMER_WALK, NO, NO, act->animSpeed, 0, 0);
		AnimateActor( act->actor, NMEANIM_HOMER_IDLE, YES, YES, act->animSpeed, 0, 0);
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
			Orientate(&act->actor->qRot,&fwd,&path->nodes[1].worldTile->normal);

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
		if( actFrameCount >= path->endFrame )
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
					AnimateActor( act->actor, BABY_ANIM_WHEEL, NO, NO, act->animSpeed, 0, 0 );

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
			Orientate(&act->actor->qRot,&fwd,&path->nodes[1].worldTile->normal);

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
	MakeUnit( (VECTOR *)&q3 );
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
//	RotateVectorByQuaternion( &fwd, &inVec, &act->qRot );
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


ENEMY *CreateAndAddEnemy(char *eActorName, int flags, long ID, PATH *path, float animSpeed, unsigned char facing )
{
	float shadowRadius = 0;
	int initFlags,i;

	ENEMY *newItem = (ENEMY *)JallocAlloc(sizeof(ENEMY),YES,"NME");
	AddEnemy(newItem);
	newItem->flags = flags;
	newItem->facing = facing;

	initFlags |= INIT_ANIMATION;

	if( newItem->flags & ENEMY_NEW_SHADOW )
	{
		initFlags |= INIT_SHADOW;
		shadowRadius = 20;
	}

	// create and add the nme actor
	newItem->nmeActor = CreateAndAddActor(eActorName,0,0,0,initFlags);
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
	{
		newItem->Update = UpdateSlerpPathNME;
		AnimateActor(newItem->nmeActor->actor,NMEANIM_PATH_WALK1,YES,NO,animSpeed, 0, 0);
	}
	else if( newItem->flags & ENEMY_NEW_FOLLOWPATH )
	{
		newItem->Update = UpdatePathNME;
		AnimateActor(newItem->nmeActor->actor,NMEANIM_PATH_WALK1,YES,NO,animSpeed, 0, 0);
	}
	else if( newItem->flags & ENEMY_NEW_WATCHFROG )
		newItem->Update = UpdateFrogWatcher;
	else if( newItem->flags & ENEMY_NEW_SNAPFROG )
	{
		newItem->Update = UpdateSnapper;
		AnimateActor(newItem->nmeActor->actor,NMEANIM_SNAP_IDLE,YES,NO,animSpeed, 0, 0);
	}
	else if( newItem->flags & ENEMY_NEW_SNAPTILES )
	{
		newItem->isSnapping = -2;
		newItem->Update = UpdateTileSnapper;
		AnimateActor(newItem->nmeActor->actor,NMEANIM_SNAP_IDLE,YES,NO,animSpeed, 0, 0);
	}
	else if( newItem->flags & ENEMY_NEW_VENT )
	{
		newItem->isSnapping = -2;
		newItem->Update = UpdateVent;
		Orientate( &newItem->nmeActor->actor->qRot, &newItem->path->nodes->worldTile->dirVector[newItem->facing], &newItem->path->nodes->worldTile->normal );
	}
	else if( (newItem->flags & ENEMY_NEW_MOVEUP) || (newItem->flags & ENEMY_NEW_MOVEDOWN) )
	{
		newItem->Update = UpdateMoveVerticalNME;
		Orientate( &newItem->nmeActor->actor->qRot, &newItem->path->nodes->worldTile->dirVector[newItem->facing], &newItem->path->nodes->worldTile->normal );
	}
	else if( (newItem->flags & ENEMY_NEW_ROTATEPATH_XZ ) || (newItem->flags & ENEMY_NEW_ROTATEPATH_XY) || (newItem->flags & ENEMY_NEW_ROTATEPATH_ZY) )
	{
		newItem->Update = UpdateRotatePathNME;
		AnimateActor(newItem->nmeActor->actor,NMEANIM_PATH_WALK1,YES,NO,animSpeed, 0, 0);
	}
	else if( newItem->flags & ENEMY_NEW_HOMING )
	{
		newItem->Update = UpdateHomingNME;
		AnimateActor(newItem->nmeActor->actor,NMEANIM_HOMER_IDLE,YES,NO,animSpeed, 0, 0);
	}
	else if( newItem->flags & ENEMY_NEW_MOVEONMOVE )
	{
		newItem->Update = UpdateMoveOnMoveNME;
		AnimateActor(newItem->nmeActor->actor,NMEANIM_HOMER_IDLE,YES,NO,animSpeed, 0, 0);
	}
	else if( newItem->flags & ENEMY_NEW_FLAPPYTHING )
		newItem->Update = UpdateFlappyThing;
	else if( newItem->flags & ENEMY_NEW_RANDOMMOVE )
		newItem->Update = UpdateRandomMoveNME;
	else if( newItem->flags & ENEMY_NEW_TILEHOMING )
	{
		newItem->Update = UpdateTileHomingNME;
		AnimateActor(newItem->nmeActor->actor,NMEANIM_HOMER_IDLE,YES,NO,animSpeed, 0, 0);
	}
	else // No update function - probably single flag nme
	{
		if( newItem->nmeActor->actor && newItem->path && newItem->path->nodes )
			Orientate( &newItem->nmeActor->actor->qRot, &newItem->path->nodes->worldTile->dirVector[newItem->facing], &newItem->path->nodes->worldTile->normal );
	}

	if( newItem->flags & ENEMY_NEW_BABYFROG )
	{
		for( i=0; i<numBabies; i++ )
			if( !babyList[i].baby )
			{
				babyList[i].baby = newItem->nmeActor;
				break;
			}

		newItem->nmeActor->effects |= (EF_RIPPLE_RINGS | EF_SLOW);
		if( babyList[i].fxColour[0] ) newItem->nmeActor->effects |= EF_TINTRED;
		if( babyList[i].fxColour[1] ) newItem->nmeActor->effects |= EF_TINTGREEN;
		if( babyList[i].fxColour[2] ) newItem->nmeActor->effects |= EF_TINTBLUE;
	}

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
	if( path->startNode < path->numNodes )
		path->fromNode = path->startNode;
	else
		path->fromNode = path->startNode = 0;


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

		if( path->nodes[path->fromNode].sample )
			PlaySample( path->nodes[path->fromNode].sample, &nme->nmeActor->actor->pos, 0, SAMPLE_VOLUME, -1 );
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

		if( path->nodes[path->fromNode].sample )
			PlaySample( path->nodes[path->fromNode].sample, &nme->nmeActor->actor->pos, 0, SAMPLE_VOLUME, -1 );
	}
	else if( flags & (ENEMY_NEW_MOVEDOWN | ENEMY_NEW_MOVEUP) )
	{
		if( flags & ENEMY_NEW_PINGPONG )
		{
			nme->flags	^= (ENEMY_NEW_MOVEUP | ENEMY_NEW_MOVEDOWN);
			if( path->nodes->sample )
				PlaySample( path->nodes->sample, &nme->nmeActor->actor->pos, 0, SAMPLE_VOLUME, -1 );
		}
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
	numSteps *= nme->speed;

	nme->deltaNormal.v[X] /= numSteps;
	nme->deltaNormal.v[Y] /= numSteps;
	nme->deltaNormal.v[Z] /= numSteps;
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

	// Setup damage & death animation function pointers
	deathAnims[DEATHBY_NORMAL]			= DamageNormal;
	deathAnims[DEATHBY_RUNOVER]			= DamageRunOver;
	deathAnims[DEATHBY_DROWNING]		= DamageDrowning;
	deathAnims[DEATHBY_SQUASHED]		= DamageSquashed;
	deathAnims[DEATHBY_FIRE]			= DamageFire;
	deathAnims[DEATHBY_ELECTRIC]		= DamageElectric;
	deathAnims[DEATHBY_FALLING]			= DamageFalling;
	deathAnims[DEATHBY_WHACKING]		= DamageWhacking;
	deathAnims[DEATHBY_INFLATION]		= DamageInflation;
	deathAnims[DEATHBY_POISON]			= DamagePoison;
	deathAnims[DEATHBY_SLICING]			= DamageSlicing;
	deathAnims[DEATHBY_EXPLOSION]		= DamageExplosion;
	deathAnims[DEATHBY_GIBBING]			= DamageGibbing;

	deathAnims[DEATHBY_NORMAL+NUM_DEATHTYPES]		= DeathNormal;
	deathAnims[DEATHBY_RUNOVER+NUM_DEATHTYPES]		= DeathRunOver;
	deathAnims[DEATHBY_DROWNING+NUM_DEATHTYPES]		= DeathDrowning;
	deathAnims[DEATHBY_SQUASHED+NUM_DEATHTYPES]		= DeathSquashed;
	deathAnims[DEATHBY_FIRE+NUM_DEATHTYPES]			= DeathFire;
	deathAnims[DEATHBY_ELECTRIC+NUM_DEATHTYPES]		= DeathElectric;
	deathAnims[DEATHBY_FALLING+NUM_DEATHTYPES]		= DeathFalling;
	deathAnims[DEATHBY_WHACKING+NUM_DEATHTYPES]		= DeathWhacking;
	deathAnims[DEATHBY_INFLATION+NUM_DEATHTYPES]	= DeathInflation;
	deathAnims[DEATHBY_POISON+NUM_DEATHTYPES]		= DeathPoison;
	deathAnims[DEATHBY_SLICING+NUM_DEATHTYPES]		= DeathSlicing;
	deathAnims[DEATHBY_EXPLOSION+NUM_DEATHTYPES]	= DeathExplosion;
	deathAnims[DEATHBY_GIBBING+NUM_DEATHTYPES]		= DeathGibbing;
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


/*	--------------------------------------------------------------------------------
    Function	: EnumEnemies
	Purpose		: Calls a function for every enemy with a given UID
	Parameters	: 
	Returns		: 

	func takes two params, the platform and the 'param' passed to EnumPlatforms
*/
int EnumEnemies(long id, int (*func)(ENEMY*, int), int param)
{
	ENEMY *cur;
	int count;

	for(cur = enemyList.head.next; cur != &enemyList.head; cur = cur->next, count++)
	{
		if (!id || cur->uid == id)
		{
			if (!func(cur, param)) break;
		}
	}

	return count;
}

void SetEnemyVisible(ENEMY *nme, int visible)
{
	if (visible)
	{
		nme->active = 1;
		nme->nmeActor->draw = 1;
		
		if (nme->isWaiting != -1)
		{
			nme->isWaiting = -1;
			SetEnemyMoving(nme, 1);
		}
	}
	else
	{
		nme->nmeActor->draw = 0;
		nme->active = 0;
	}
}

void SetEnemyMoving(ENEMY *nme, int moving)
{
	if (moving)
	{
		if (nme->isWaiting)
		{
			nme->isWaiting = 0;
			//nme->path->toNode = nme->path->fromNode;
			nme->path->startFrame = actFrameCount;
			nme->path->endFrame = actFrameCount + (60*nme->speed);
		}
	}
	else
	{
		nme->isWaiting = -1;
	}
}

/*	--------------------------------------------------------------------------------
    Function	: MoveEnemy
	Purpose		: Moves an enemy to a given node in its path
	Parameters	: ENEMY*, int [suitable for enumEnemies]
	Returns		: 1 for success
*/
int MoveEnemyToNode(ENEMY *nme, int node)
{
	VECTOR dest;
	// Gratuitous hack to make move-on-move enemies work
	if( nme->flags & ENEMY_NEW_MOVEONMOVE )
	{
		ENEMY *ph = GetEnemyFromUID(node);

		if( ph )
		{
			nme->path->nodes[0].worldTile = ph->path->nodes[0].worldTile;
			GetPositionForPathNode( &nme->nmeActor->actor->pos, &nme->path->nodes[0] );
			nme->isSnapping = 0;
		}
		else
			dprintf"MoveEnemyToNode(): Could not find placeholder %d\n", node));
	}
	else if (node >= 0 && node < nme->path->numNodes)
	{
		nme->path->toNode  = node;
		nme->inTile = nme->path->nodes[node].worldTile;
		GetPositionForPathNode( &nme->nmeActor->actor->pos, &nme->path->nodes[node] );
		nme->path->endFrame = actFrameCount;
		nme->Update(nme);
	}
	else
		dprintf"MoveEnemyToNode(): Flag (%d) out of range\n", node));
	return 1;
}

