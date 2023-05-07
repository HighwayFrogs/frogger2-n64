/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: anim.h
	Programmer	: Andy Eder
	Date		: 29/04/99 10:32:50

----------------------------------------------------------------------------------------------- */

#ifndef ANIM_H_INCLUDED
#define ANIM_H_INCLUDED

#define ANIM_RANDOM_START	(1 << 0)

// Needs using on N64
extern float hedRotAmt;

extern void UpdateAnimations(ACTOR *actor);
extern void AnimateActor(ACTOR *actor, int animNum, char loop, char queue, float speed, BYTE morphs,BOOL keepProportion);
extern void StartAnimateActor(ACTOR *actor, int animNum, char loop, char queue, float speed, BYTE morphs,BOOL keepProportion);
extern void FlushQueue(ACTOR *myActor);
extern void SetAnimationSpeed(ACTOR *myActor, float speed);
extern void InitActorAnim(ACTOR *tempActor);
extern BOOL QueryAnimTime(ACTOR *actor, float time);

#endif
