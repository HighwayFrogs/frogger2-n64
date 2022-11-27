/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: anim.h
	Programmer	: Andy Eder
	Date		: 29/04/99 10:32:50

----------------------------------------------------------------------------------------------- */

#ifndef ANIM_H_INCLUDED
#define ANIM_H_INCLUDED

#define ANIM_RANDOM_START	(1 << 0)

// Frogger death and damage functions
void DamageNormal( int pl );
void DamageRunOver( int pl );
void DamageDrowning( int pl );
void DamageSquashed( int pl );
void DamageFire( int pl );
void DamageElectric( int pl );
void DamageFalling( int pl );
void DamageWhacking( int pl );
void DamageInflation( int pl );
void DamagePoison( int pl );
void DamageSlicing( int pl );
void DamageExplosion( int pl );
void DamageGibbing( int pl );

void DeathNormal( int pl );
void DeathRunOver( int pl );
void DeathDrowning( int pl );
void DeathSquashed( int pl );
void DeathFire( int pl );
void DeathElectric( int pl );
void DeathFalling( int pl );
void DeathWhacking( int pl );
void DeathInflation( int pl );
void DeathPoison( int pl );
void DeathSlicing( int pl );
void DeathExplosion( int pl );
void DeathGibbing( int pl );


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
