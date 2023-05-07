/*
	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: specfx.c
	Programmer	: Andy Eder
	Date		: 14/05/99 09:31:42

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>
#include "incs.h"


SPECFXLIST specFXList;

char doScreenFade	= 0;
char fadeDir		= FADE_OUT;
short fadeOut		= 255;
short fadeStep		= 4;

char pauseMode		= 0;

#define NUM_TRAIL_ELEMENTS 32



//----- [ TEXTURES USED FOR SPECIAL FX ] -----//


TEXTURE *txtrRipple		= NULL;
TEXTURE *txtrStar		= NULL;
TEXTURE *txtrSolidRing	= NULL;
TEXTURE *txtrSmoke		= NULL;
TEXTURE *txtrRing		= NULL;
TEXTURE *txtrFly		= NULL;
TEXTURE *txtrBubble		= NULL;
TEXTURE *txtrFire		= NULL;
TEXTURE *txtrBlank		= NULL;
TEXTURE *txtrTrail		= NULL;


void UpdateFXRipple( SPECFX *fx );
void UpdateFXRing( SPECFX *fx );
void UpdateFXBolt( SPECFX *fx );
void UpdateFXSmoke( SPECFX *fx );
void UpdateFXSwarm( SPECFX *fx );
void UpdateFXExplode( SPECFX *fx );
void UpdateFXTrail( SPECFX *fx );
void UpdateFXLightning( SPECFX *fx );

void CreateBlastRing( );
void AddTrailElement( SPECFX *fx, int i );

// Used to store precalculated blast ring shape
#ifdef PC_VERSION
D3DTLVERTEX *ringVtx = NULL;
#else
Vtx *ringVtx = NULL;
#endif

/*	--------------------------------------------------------------------------------
	Function		: CreateAndAddSpecialEffect
	Purpose			: creates and initialises a special effect, either barebones or of a custom type
	Parameters		: 
	Returns			: Created effect
	Info			: 
*/
SPECFX *CreateAndAddSpecialEffect( short type, VECTOR *origin, VECTOR *normal, float size, float speed, float accn, float lifetime )
{
	SPECFX *effect = (SPECFX *)JallocAlloc( sizeof(SPECFX), YES, "FX" );
	long i,n;
	float life = lifetime*60;

	effect->type = type;
	SetVector( &effect->origin, origin );
	SetVector( &effect->normal, normal );
	effect->scale.v[X] = size;
	effect->scale.v[Y] = size;
	effect->scale.v[Z] = size;
	effect->speed = speed;
	effect->accn = accn;
	effect->lifetime = actFrameCount + life;
	effect->startLife = lifetime;
	effect->r = 255;
	effect->g = 255;
	effect->b = 255;
	effect->a = 255;

	switch( type )
	{
	case FXTYPE_WATERRIPPLE:
		effect->r = 200;
		effect->g = 200;
		effect->fade = effect->a / life;

		AddToVector(&effect->origin,&effect->normal);

		effect->tex = txtrRipple;
		effect->Update = UpdateFXRipple;
		effect->Draw = DrawFXRipple;

		break;
	case FXTYPE_GARIBCOLLECT:
		effect->b = 0;
		effect->spin = 0.15;
		effect->fade = effect->a / life;

		effect->origin.v[X] += (effect->normal.v[X] * 5);
		effect->origin.v[Y] += (effect->normal.v[Y] * 5);
		effect->origin.v[Z] += (effect->normal.v[Z] * 5);

		effect->tex = txtrStar;
		effect->Update = UpdateFXRipple;
		effect->Draw = DrawFXRipple;

		break;
	case FXTYPE_BASICRING:
		effect->a = 200;
		effect->fade = effect->a / life;

		AddToVector(&effect->origin,&effect->normal);

		effect->tex = txtrRing;
		effect->Update = UpdateFXRipple;
		effect->Draw = DrawFXRipple;

		break;
	case FXTYPE_POLYRING:
		if( !ringVtx )
			CreateBlastRing( );

		effect->fade = effect->a / life;

		AddToVector(&effect->origin,&effect->normal);

		effect->scale.v[Y] /= 8;
		effect->tilt = 0.9;

		effect->tex = txtrBlank;
		effect->Update = UpdateFXRing;
		effect->Draw = DrawFXRing;

		break;
	case FXTYPE_LASER:
		if( !ringVtx )
			CreateBlastRing( );

		effect->scale.v[X] = 2;
		effect->scale.v[Z] = 2;
		SetVector( &effect->vel, &effect->normal );
		ScaleVector( &effect->vel, effect->speed );
		effect->spin = 0.1;
		effect->tilt = 1;
		effect->a = 128;

		effect->tex = txtrBlank;
		effect->Update = UpdateFXBolt;
		effect->Draw = DrawFXRing;

		break;
	case FXTYPE_TRAIL:
	case FXTYPE_BILLBOARDTRAIL:
		effect->fade = effect->a / life;

		effect->numP = i = NUM_TRAIL_ELEMENTS;
		effect->particles = (PARTICLE *)JallocAlloc( sizeof(PARTICLE)*i, YES, "P" );
		effect->particles[0].bounce = 1;

		while( i-- )
		{
			effect->particles[i].poly = (VECTOR *)JallocAlloc( sizeof(VECTOR)*2, YES, "V" );
			effect->particles[i].a = (i+1)*(255/effect->numP);
			SetVector( &effect->particles[i].pos, &effect->origin );
			effect->particles[i].rMtrx = (float *)JallocAlloc( sizeof(float)*16, YES, "Mtx" );
		}

		effect->tex = txtrTrail;
		effect->Update = UpdateFXTrail;
		effect->Draw = DrawFXTrail;

		break;
	case FXTYPE_JUMPBLUR:
		effect->numP = 1;
		effect->sprites = (SPRITE *)JallocAlloc( sizeof(SPRITE), YES, "Sprite" );

		SetVector( &effect->sprites->pos, &effect->origin );
		effect->sprites->texture = txtrSolidRing;
		effect->sprites->scaleX = effect->scale.v[X];
		effect->sprites->scaleY = effect->scale.v[Y];
		effect->sprites->r = 100;
		effect->sprites->b = 100;
		effect->sprites->a = 200;
		effect->fade = effect->sprites->a / life;

#ifndef PC_VERSION
		effect->sprites->offsetX = -effect->sprites->texture->sx / 2;
		effect->sprites->offsetY = -effect->sprites->texture->sy / 2;
#else
		effect->sprites->offsetX = -16;
		effect->sprites->offsetY = -16;
#endif

		effect->sprites->flags = SPRITE_TRANSLUCENT;

		AddSprite( effect->sprites, NULL );

		effect->Update = UpdateFXSmoke;
		effect->Draw = NULL;
		break;
	case FXTYPE_FROGSTUN:
	case FXTYPE_FLYSWARM:
		effect->numP = 6;
		i = effect->numP;

		effect->sprites = (SPRITE *)JallocAlloc( sizeof(SPRITE)*effect->numP, YES, "Sprites" );
		effect->particles = (PARTICLE *)JallocAlloc( sizeof(PARTICLE)*effect->numP, YES, "Particles" );

		if( effect->type == FXTYPE_FLYSWARM )
			effect->tex = txtrFly;
		else
			effect->tex = txtrStar;

		while(i--)
		{
			effect->sprites[i].texture = effect->tex;
			SetVector( &effect->sprites[i].pos, &effect->origin );

			effect->sprites[i].scaleX = effect->scale.v[X];
			effect->sprites[i].scaleY = effect->scale.v[Y];
			effect->sprites[i].r = effect->r;
			effect->sprites[i].g = effect->g;
			effect->sprites[i].b = effect->b;
			effect->sprites[i].a = effect->a;

			effect->sprites[i].offsetX	= -16;
			effect->sprites[i].offsetY	= -16;
			effect->sprites[i].flags = SPRITE_TRANSLUCENT;

			AddSprite( &effect->sprites[i], NULL );

			effect->particles[i].pos.v[X] = -8 + Random(16);
			effect->particles[i].pos.v[Y] = -6 + Random(12);
			effect->particles[i].pos.v[Z] = -8 + Random(16);
		}

		effect->Update = UpdateFXSwarm;
		effect->Draw = NULL;
		break;
	case FXTYPE_BUTTERFLYSWARM:
		effect->numP = (int)lifetime; // Nasty Nasty Nasty
		i = effect->numP;

		effect->act = (ACTOR2 **)JallocAlloc( sizeof(ACTOR2 *)*effect->numP, YES, "Actor2s" );
		effect->particles = (PARTICLE *)JallocAlloc( sizeof(PARTICLE)*effect->numP, YES, "Particles" );

		while( i-- )
		{
			effect->act[i] = CreateAndAddActor( "bfly.obe", 0,0,0, INIT_ANIMATION, 0, 0 );
			if( effect->act[i]->actor->objectController )
				InitActorAnim( effect->act[i]->actor );

			AnimateActor( effect->act[i]->actor,0,YES,NO,1.0F, 0, 0);
			effect->act[i]->actor->scale.v[X] = effect->scale.v[X];
			effect->act[i]->actor->scale.v[Y] = effect->scale.v[Y];
			effect->act[i]->actor->scale.v[Z] = effect->scale.v[Z];

			effect->particles[i].pos.v[X] = -8 + Random(16);
			effect->particles[i].pos.v[Y] = -6 + Random(12);
			effect->particles[i].pos.v[Z] = -8 + Random(16);
		}

		effect->Update = UpdateFXSwarm;
		effect->Draw = NULL;

		break;
	case FXTYPE_SMOKE_STATIC:
	case FXTYPE_SMOKE_GROWS:
	case FXTYPE_BUBBLES:
		// Velocity is normal scaled by speed, plus a random offset scaled by speed
		SetVector( &effect->vel, &effect->normal );
		ScaleVector( &effect->vel, speed );
		effect->vel.v[X] += (-1 + Random(3))*speed*0.4;
		effect->vel.v[Y] += (-1 + Random(3))*speed*0.4;
		effect->vel.v[Z] += (-1 + Random(3))*speed*0.4;
		effect->fade = 180 / life;

		effect->numP = 1;
		effect->sprites = (SPRITE *)JallocAlloc( sizeof(SPRITE), YES, "Sprite" );

		if( effect->type == FXTYPE_BUBBLES )
			effect->sprites->texture = txtrBubble;
		else
			effect->sprites->texture = txtrSmoke;

		SetVector( &effect->sprites->pos, &effect->origin );
		effect->sprites->scaleX = effect->scale.v[X];
		effect->sprites->scaleY = effect->scale.v[Y];

		effect->sprites->r = effect->r;
		effect->sprites->g = effect->g;
		effect->sprites->b = effect->b;
		effect->sprites->a = effect->a;

#ifndef PC_VERSION
		effect->sprites->offsetX = -effect->sprites->texture->sx / 2;
		effect->sprites->offsetY = -effect->sprites->texture->sy / 2;
#else
		effect->sprites->offsetX = -16;
		effect->sprites->offsetY = -16;
#endif
		effect->sprites->flags = SPRITE_TRANSLUCENT;

		AddSprite( effect->sprites, NULL );
		effect->Update = UpdateFXSmoke;
		effect->Draw = NULL;
		
		break;
	case FXTYPE_SMOKEBURST:
	case FXTYPE_SPLASH:
	case FXTYPE_SPARKBURST:
	case FXTYPE_FLAMES:
	case FXTYPE_FIERYSMOKE:
		effect->numP = 5;
		i = effect->numP;

		effect->sprites = (SPRITE *)JallocAlloc( sizeof(SPRITE)*effect->numP, YES, "Sprites" );
		effect->particles = (PARTICLE *)JallocAlloc( sizeof(PARTICLE)*effect->numP, YES, "Particles" );

		effect->rebound = (PLANE2 *)JallocAlloc( sizeof(PLANE2), YES, "Rebound" );
		SetVector( &effect->rebound->point, &effect->origin );
		SetVector( &effect->rebound->normal, &effect->normal );

		if( effect->type == FXTYPE_SPLASH )
			effect->tex = txtrBubble;
		else if( effect->type == FXTYPE_SPARKBURST )
			effect->tex = txtrSolidRing;
		else if( effect->type == FXTYPE_FLAMES )
			effect->tex = txtrFire;
		else if( effect->type == FXTYPE_SMOKEBURST || effect->type == FXTYPE_FIERYSMOKE )
			effect->tex = txtrSmoke;

		effect->fade = (255/life)*2;

		while(i--)
		{
			SetVector( &effect->sprites[i].pos, &effect->origin );

			effect->sprites[i].r = effect->r;
			effect->sprites[i].g = effect->g;
			effect->sprites[i].b = effect->b;
			effect->sprites[i].a = effect->a;

			if( effect->type == FXTYPE_FIERYSMOKE )
			{
				effect->sprites[i].g = 180;
				effect->sprites[i].b = 0;
			}

			effect->sprites[i].texture = effect->tex;

			effect->sprites[i].scaleX = effect->scale.v[X];
			effect->sprites[i].scaleY = effect->scale.v[Y];

			effect->sprites[i].offsetX = -16;
			effect->sprites[i].offsetY = -16;
			effect->sprites[i].flags = SPRITE_TRANSLUCENT;
			
			AddSprite( &effect->sprites[i], NULL );

			if( effect->type == FXTYPE_SPARKBURST )
				effect->particles[i].bounce = 1;
			else
				effect->particles[i].bounce = 0;

			// Velocity is normal scaled by speed, plus a random offset scaled by speed
			SetVector( &effect->particles[i].vel, &effect->normal );
			ScaleVector( &effect->particles[i].vel, effect->speed );
			effect->particles[i].vel.v[X] += (-1 + Random(3))*effect->speed*0.4;
			effect->particles[i].vel.v[Y] += (-1 + Random(3))*effect->speed*0.4;
			effect->particles[i].vel.v[Z] += (-1 + Random(3))*effect->speed*0.4;
		}

		effect->Update = UpdateFXExplode;
		effect->Draw = NULL;
		
		break;
	case FXTYPE_LIGHTNING:
		effect->numP = 8;
		i = effect->numP;

		effect->particles = (PARTICLE *)JallocAlloc( sizeof(PARTICLE)*effect->numP, YES, "P" );

		effect->tex = txtrTrail;

		while(i--)
		{
			SetVector( &effect->particles[i].pos, &effect->origin );

			effect->particles[i].poly = (VECTOR *)JallocAlloc( sizeof(VECTOR)*2, YES, "V" );

			effect->particles[i].r = effect->r;
			effect->particles[i].g = effect->g;
			effect->particles[i].b = effect->b;
			effect->particles[i].a = effect->a;
		}

		effect->Update = UpdateFXLightning;
		effect->Draw = DrawFXLightning;
		break;
	}

	AddSpecFX( effect );

	return effect;
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateSpecialEffects
	Purpose			: Call update function for all effects in list
	Parameters		: 
	Returns			: 
	Info			: 
*/
void UpdateSpecialEffects( )
{
	SPECFX *fx1, *fx2;

	for( fx1 = specFXList.head.next; fx1 != &specFXList.head; fx1 = fx2 )
	{
		fx2 = fx1->next;

		if( fx1->Update )
			fx1->Update( fx1 );
	}
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateFXRipples
	Purpose			: Grow and fade
	Parameters		: 
	Returns			: 
	Info			: 
*/
void UpdateFXRipple( SPECFX *fx )
{
	int fo;

	if( fx->deadCount )
		if( !(--fx->deadCount) )
		{
			SubSpecFX(fx);
			return;
		}

	if( fx->follow )
		SetVector( &fx->origin, &fx->follow->pos );

	fo = fx->fade * gameSpeed;
	if( fx->a > fo ) fx->a -= fo;
	else fx->a = 0;

	fx->speed += fx->accn * gameSpeed;
	fx->scale.v[X] += fx->speed * gameSpeed;
	fx->scale.v[Z] += fx->speed * gameSpeed;
	fx->angle += fx->spin * gameSpeed;

	if( (actFrameCount > fx->lifetime) && !fx->deadCount )
		fx->deadCount = 5;
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateFXRing
	Purpose			: Grow and fade
	Parameters		: 
	Returns			: 
	Info			: 
*/
void UpdateFXRing( SPECFX *fx )
{
	int fo;
	float speed;

	if( fx->deadCount )
		if( !(--fx->deadCount) )
		{
			SubSpecFX(fx);
			return;
		}

	if( fx->follow )
		SetVector( &fx->origin, &fx->follow->pos );

	fo = fx->fade * gameSpeed;
	if( fx->a > fo ) fx->a -= fo;
	else fx->a = 0;

	fx->speed += fx->accn * gameSpeed;
	speed = fx->speed * gameSpeed;
	fx->scale.v[X] += speed;
	fx->scale.v[Y] += speed/8;
	fx->scale.v[Z] += speed;
	fx->angle += fx->spin * gameSpeed;
	
	if( (actFrameCount > fx->lifetime) && !fx->deadCount )
		fx->deadCount = 5;
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateFXBolt
	Purpose			: Just move, not grow
	Parameters		: 
	Returns			: 
	Info			: 
*/
void UpdateFXBolt( SPECFX *fx )
{
	int fo;
	float accn = fx->accn * gameSpeed;

	if( fx->deadCount )
		if( !(--fx->deadCount) )
		{
			SubSpecFX(fx);
			return;
		}

	if( fx->follow )
		SetVector( &fx->origin, &fx->follow->pos );

	fo = fx->fade * gameSpeed;
	if( fx->a > fo ) fx->a -= fo;
	else fx->a = 0;

	fx->vel.v[X] += accn;
	fx->vel.v[Y] += accn;
	fx->vel.v[Z] += accn;
	fx->origin.v[X] += fx->vel.v[X] * gameSpeed;
	fx->origin.v[Y] += fx->vel.v[Y] * gameSpeed;
	fx->origin.v[Z] += fx->vel.v[Z] * gameSpeed;
	fx->angle += fx->spin * gameSpeed;
	
	if( (actFrameCount > fx->lifetime) && !fx->deadCount )
		fx->deadCount = 5;
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateFXSmoke
	Purpose			: updates the smoke based fx
	Parameters		: 
	Returns			: void
	Info			: 
*/
void UpdateFXSmoke( SPECFX *fx )
{
	int fo, i;
	float dist, vS = 1;

	if( fx->deadCount )
		if( !(--fx->deadCount) )
		{
			SubSpecFX(fx);
			return;
		}

	if( fx->follow )
		SetVector( &fx->origin, &fx->follow->pos );

	fo = fx->fade * gameSpeed;
	if( fx->sprites->a > fo ) fx->sprites->a -= fo;
	else
	{
		if( fx->type == FXTYPE_JUMPBLUR )
			fx->sprites->a = 255;
		else
			fx->sprites->a = 0;
	}

	fx->sprites->pos.v[X] += fx->vel.v[X] * gameSpeed;
	fx->sprites->pos.v[Y] += fx->vel.v[Y] * gameSpeed;
	fx->sprites->pos.v[Z] += fx->vel.v[Z] * gameSpeed;

	// Slow down gameSpeed times
	vS = 1-(0.02*gameSpeed);
	ScaleVector( &fx->vel, vS );

	if( fx->type == FXTYPE_SMOKE_GROWS )
	{
		fx->sprites->scaleX += fx->accn*gameSpeed;
		fx->sprites->scaleY += fx->accn*gameSpeed;
	}
	else if( fx->type == FXTYPE_JUMPBLUR )
	{
		fx->sprites->scaleX--;
		fx->sprites->scaleY--;
	}
	else if( fx->type == FXTYPE_BUBBLES )
	{
		if( fx->rebound )
		{
			fx->rebound->J = -DotProduct( &fx->rebound->point, &fx->rebound->normal );
			dist = -(DotProduct(&fx->sprites->pos, &fx->rebound->normal) + fx->rebound->J);

			if(dist > 0 && dist < 10)
			{
				CreateAndAddSpecialEffect( FXTYPE_BASICRING, &fx->sprites->pos, &fx->rebound->normal, 5, 0.4, 0.05, 0.3 );
				JallocFree( (UBYTE **)&fx->rebound );
				fx->rebound = NULL;
			}
		}
	}

	if( (actFrameCount > fx->lifetime) && !fx->deadCount )
		fx->deadCount = 5;
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateFXSwarm
	Purpose			: updates the swarm based fx
	Parameters		: 
	Returns			: void
	Info			: 
*/
void UpdateFXSwarm( SPECFX *fx )
{
	VECTOR up, pos;
	int i = fx->numP;
	float dist;

	if( fx->deadCount )
		if( !(--fx->deadCount) )
		{
			SubSpecFX(fx);
			return;
		}

	if( fx->follow )
		SetVector( &fx->origin, &fx->follow->pos );

	if( fx->type == FXTYPE_FROGSTUN )
	{
		SetVector( &up, &currTile[0]->normal );
		ScaleVector( &up, 20 );
		AddVector( &fx->origin, &up, &frog[0]->actor->pos );
	}

	while(i--)
	{
		// Set world check position from either sprite or actor
		if( !fx->act )
			SetVector( &pos, &fx->sprites[i].pos );
		else
			SetVector( &pos, &fx->act[i]->actor->pos );

		// Fade out star stun
		if( fx->type == FXTYPE_FROGSTUN )
			if( fx->sprites[i].a > 7 ) fx->sprites[i].a -= 8;
			else fx->sprites[i].a = 0;

		// Update particle velocity to oscillate around the point
		if( pos.v[X] > fx->origin.v[X])
			fx->particles[i].vel.v[X] -= max(gameSpeed/3, 1);
		else
			fx->particles[i].vel.v[X] += max(gameSpeed/3, 1);
		if( pos.v[Y] > fx->origin.v[Y] )
			fx->particles[i].vel.v[Y] -= max(gameSpeed/3, 1);
		else
			fx->particles[i].vel.v[Y] += max(gameSpeed/3, 1);
		if( pos.v[Z] > fx->origin.v[Z])
			fx->particles[i].vel.v[Z] -= max(gameSpeed/3, 1);
		else
			fx->particles[i].vel.v[Z] += max(gameSpeed/3, 1);

		// Limit velocity of particles
		if( fx->particles[i].vel.v[X] > gameSpeed*2 )
			fx->particles[i].vel.v[X] = gameSpeed*2;
		else if( fx->particles[i].vel.v[X] < -gameSpeed*2 )
			fx->particles[i].vel.v[X] = -gameSpeed*2;
		if( fx->particles[i].vel.v[Y] > gameSpeed*2 )
			fx->particles[i].vel.v[Y] = gameSpeed*2;
		else if( fx->particles[i].vel.v[Y] < -gameSpeed*2 )
			fx->particles[i].vel.v[Y] = -gameSpeed*2;
		if( fx->particles[i].vel.v[Z] > gameSpeed*2 )
			fx->particles[i].vel.v[Z] = gameSpeed*2;
		else if( fx->particles[i].vel.v[Z] < -gameSpeed*2 )
			fx->particles[i].vel.v[Z] = -gameSpeed*2;

		// Add velocity to local particle position
		AddToVector( &fx->particles[i].pos, &fx->particles[i].vel );
		// Add local particle pos to swarm origin to get world coords for sprite or actor
		if( !fx->act )
		{
			AddVector( &fx->sprites[i].pos, &fx->origin, &fx->particles[i].pos );
		}
		else
		{
			AddVector( &fx->act[i]->actor->pos, &fx->origin, &fx->particles[i].pos );
		}

		if( !fx->act )
			SetVector( &pos, &fx->sprites[i].pos );
		else
			SetVector( &pos, &fx->act[i]->actor->pos );

		if( fx->rebound )
		{
			fx->rebound->J = -DotProduct( &fx->rebound->point, &fx->rebound->normal );
			dist = -(DotProduct(&pos, &fx->rebound->normal) + fx->rebound->J);

			if(dist > 0 && dist < 3)
				CreateAndAddSpecialEffect( FXTYPE_BASICRING, &pos, &fx->rebound->normal, 5, 0.5, 0.1, 0.3 );
		}
	}

	if( fx->type != FXTYPE_FLYSWARM && fx->type != FXTYPE_BUTTERFLYSWARM )
		if( (actFrameCount > fx->lifetime) && !fx->deadCount )
			fx->deadCount = 5;
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateFXSExplodeParticle
	Purpose			: updates the explode particle based fx
	Parameters		: 
	Returns			: void
	Info			: 
*/
void UpdateFXExplode( SPECFX *fx )
{
	float dist, vS;
	int i = fx->numP, j, fo, ele;
	VECTOR up;

	if( fx->deadCount )
		if( !(--fx->deadCount) )
		{
			SubSpecFX(fx);
			return;
		}

	if( fx->follow )
		SetVector( &fx->origin, &fx->follow->pos );

	// Slow down gameSpeed times
	vS = 1-(0.02*gameSpeed);

	while(i--)
	{
		if( fx->particles[i].bounce == 2 )
			continue;

		ScaleVector( &fx->particles[i].vel, vS );

		if( fx->gravity != 0 )
		{
			SetVector( &up, &fx->normal );
			ScaleVector( &up, fx->gravity*gameSpeed );
			SubFromVector( &fx->particles[i].vel, &up );
		}

		if( fx->rebound )
		{
			fx->rebound->J = -DotProduct( &fx->rebound->point, &fx->rebound->normal );
			dist = -(DotProduct(&fx->sprites[i].pos, &fx->rebound->normal) + fx->rebound->J);

			// check if particle has hit (or passed through) the plane
			if(dist > 0)
			{
				if( fx->particles[i].bounce )
					fx->particles[i].vel.v[Y] *= -0.95; // Should be relative to the normal, but it'll do for now
				else
					SetVector( &fx->particles[i].vel, &zero );

				// check if this exploding particle type triggers some other effect or event
				if( fx->type == FXTYPE_SPLASH )
				{
					fx->sprites[i].a = 1;

					CreateAndAddSpecialEffect( FXTYPE_BASICRING, &fx->sprites[i].pos, &fx->rebound->normal, 10, 1, 0.1, 0.3 );
					continue;
				}
			}
		}

		fx->sprites[i].pos.v[X] += fx->particles[i].vel.v[X] * gameSpeed;
		fx->sprites[i].pos.v[Y] += fx->particles[i].vel.v[Y] * gameSpeed;
		fx->sprites[i].pos.v[Z] += fx->particles[i].vel.v[Z] * gameSpeed;

		fo = (Random(4) + fx->fade) * gameSpeed ;

		// For fiery (of whatever colour) smoke, fade to black then fade out
		if( fx->type == FXTYPE_FIERYSMOKE && (fx->sprites[i].r || fx->sprites[i].g || fx->sprites[i].b) )
		{
			if( fx->sprites[i].r > fo/2 ) fx->sprites[i].r -= fo/2;
			else fx->sprites[i].r = 0;
			if( fx->sprites[i].g > fo ) fx->sprites[i].g -= fo;
			else fx->sprites[i].g = 0;
		}
		else
		{
			if( fx->sprites[i].a > fo ) fx->sprites[i].a -= fo;
			else fx->sprites[i].a = 0;

			if( fx->sprites[i].a < 16 )
			{
				fx->sprites[i].scaleX	= 0;
				fx->sprites[i].scaleY	= 0;
				fx->sprites[i].a		= 0;
				fx->particles[i].bounce = 2;
			}
		}
	}

	if( (actFrameCount > fx->lifetime) && !fx->deadCount )
		fx->deadCount = 5;
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateFXTrails
	Purpose			: 
	Parameters		: 
	Returns			: void
	Info			: 
*/
void UpdateFXTrail( SPECFX *fx )
{
	unsigned long i = fx->start;
	short diff;
	float fo;
	VECTOR dist;

	if( fx->deadCount )
		if( !(--fx->deadCount) )
		{
			SubSpecFX(fx);
			return;
		}

	if( fx->follow )
	{
		// Flag for first time through
		if( fx->particles[i].bounce )
		{
			fx->particles[i].bounce = 0;
			AddTrailElement( fx, i );
		}
		else 
		{
			// If space, add a new particle after end (wrapped)
			if( fx->end < fx->start ) 
				diff = fx->start - fx->end - 1;
			else
				diff = ((fx->numP-1)-fx->end)+fx->start;

			if( diff > 0 && fx->particles[(fx->end+1)%fx->numP].a < 16 )
			{
				if( ++fx->end >= fx->numP ) fx->end = 0;
				AddTrailElement( fx, fx->end );
			}
		}
	}

	// Update particles that are alive - fade, shrink by speed, move by velocity
	// Note - polys stored in object space
	do
	{
		ScaleVector( &fx->particles[i].poly[0], fx->speed );
		ScaleVector( &fx->particles[i].poly[1], fx->speed );
		AddToVector( &fx->particles[i].pos, &fx->particles[i].vel );

		if( ++i >= fx->numP ) i=0;

	} while( i != ((fx->end+1)%fx->numP) );

	// Fade out *all* particles, to get correct delay
	for( i=0; i<fx->numP; i++ )
	{
		fo = fx->fade * gameSpeed ;
		if( fx->particles[i].a > fo ) fx->particles[i].a -= fo;
		else fx->particles[i].a = 0;
	}

	// If oldest particle has died, exclude it from the list
	if( fx->particles[fx->start].a < 16 || DistanceBetweenPointsSquared(&fx->particles[fx->start].poly[0],&fx->particles[fx->start].poly[1])<2 )
	{
		fx->particles[fx->start].a = 0;
		if( ++fx->start >= fx->numP )
			fx->start = 0;
		// If no more particles in list, time to die
		if( (fx->start == fx->end-1) && !fx->follow && !fx->deadCount )
			fx->deadCount = 5;
	}
}

// Check the follow actor for quaternion and rotate the points about it. If no actor, don't add.
void AddTrailElement( SPECFX *fx, int i )
{
	float t;
	QUATERNION q, cross, d;

	fx->particles[i].r = fx->r;
	fx->particles[i].g = fx->g;
	fx->particles[i].b = fx->b;
	fx->particles[i].a = fx->a;
	fx->particles[i].poly[0].v[X] = fx->scale.v[X];
	fx->particles[i].poly[1].v[X] = -fx->scale.v[X];

	// Distance of this particle from the origin
	SubVector( &fx->particles[i].pos, &fx->follow->pos, &fx->origin );

	// Amount of drift - distance between this and the last particle scaled. Doesn't work for first one
	if( i != fx->start )
	{
		int j = i-1;
		if( j < 0 ) j = fx->numP-1;
		SubVector( &fx->particles[i].vel, &fx->particles[i].pos, &fx->particles[j].pos );
		ScaleVector( &fx->particles[i].vel, fx->accn );
	}

	if( fx->type != FXTYPE_BILLBOARDTRAIL )
		QuaternionToMatrix( &fx->follow->qRot, (MATRIX *)fx->particles[i].rMtrx );
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateFXLightning
	Purpose			: Emperors hands effect
	Parameters		: 
	Returns			: void
	Info			: 
*/
void UpdateFXLightning( SPECFX *fx )
{
	VECTOR target, aim, to;
	VECTOR ran, source, cross;
	float scale, fr;
	long i, h=fx->numP*0.25;

	if( fx->deadCount )
		if( !(--fx->deadCount) )
		{
			SubSpecFX(fx);
			return;
		}

	// Find out where the lightning is being fired at
	if( fx->follow )
	{
		SetVector( &target, &fx->follow->pos );
		SubVector( &aim, &target, &fx->origin );
	}
	else
	{
		SetVector( &aim, &fx->normal );
		ScaleVector( &aim, fx->speed );
		AddVector( &target, &aim, &fx->origin );
	}

	// Find a route through space
	for( i=0; i<fx->numP; i++ )
	{
		SetVector( &source, (!i)?(&fx->origin):(&fx->particles[i-1].pos) );
		scale = 2/(float)(fx->numP-i);
		fr = 1-((float)i/(float)fx->numP);
		// Get direction from last sprite to target
		SubVector( &to, &target, &source );
		ScaleVector( &to, scale );

		// Get a random direction, and then modify by the "unit" aim direction (general movement in direction of target)
		ran.v[X] = Random(21)-8;
		ran.v[Y] = Random(21)-12;
		ran.v[Z] = Random(21)-8;

		MakeUnit( &ran );
		ScaleVector( &ran, fx->accn*(float)min((i-0),(fx->numP-i))*Magnitude(&to) );
		AddToVector( &to, &ran );
		AddVector( &fx->particles[i].pos, &source, &to );

		// Push the polys out from the position
		MakeUnit( &to );
		CrossProduct( &cross, &to, &upVec );
		MakeUnit( &cross );
		ScaleVector( &cross, fx->scale.v[X] );
		AddVector( &fx->particles[i].poly[0], &fx->particles[i].pos, &cross );
		SubVector( &fx->particles[i].poly[1], &fx->particles[i].pos, &cross );

		// Randomly fork a new lightning strand, but not if we're near the end or we're more than 2 layers of forking deep
		if( (Random(100)>(100-fx->tilt)) && (i<fx->numP-h && i>h) && fx->fade < 4 )
		{
			VECTOR dir;
			SPECFX *effect;
			SubVector( &dir, &fx->particles[i].pos, &source );
			MakeUnit( &dir );
			effect = CreateAndAddSpecialEffect( FXTYPE_LIGHTNING, &fx->particles[i].pos,
												&dir, fx->scale.v[X]-Random(3), fx->speed * min(fr*1.5, 1),
												fx->accn, fx->startLife * min(fr*1.5, 1) );
			effect->fade = ++fx->fade;
			SetFXColour( effect, fx->r, fx->g, fx->b );
		}
	}

	if( (actFrameCount > fx->lifetime) && !fx->deadCount )
		fx->deadCount = 5;
}


/*	--------------------------------------------------------------------------------
	Function		: FreeSpecFXList
	Purpose			: frees the fx linked list
	Parameters		: 
	Returns			: void
	Info			: 
*/
void InitSpecFXList( )
{
	specFXList.head.next = specFXList.head.prev = &specFXList.head;
	specFXList.numEntries = 0;

	// get the textures used for the various special effects
	FindTexture(&txtrRipple,UpdateCRC("ai_ripple.bmp"),YES);
	FindTexture(&txtrStar,UpdateCRC("ai_star.bmp"),YES);
	FindTexture(&txtrSolidRing,UpdateCRC("ai_circle.bmp"),YES);
	FindTexture(&txtrSmoke,UpdateCRC("ai_smoke.bmp"),YES);
	FindTexture(&txtrRing,UpdateCRC("ai_ring.bmp"),YES);
	FindTexture(&txtrFly,UpdateCRC("fly1.bmp"),YES);
	FindTexture(&txtrBubble,UpdateCRC("watdrop.bmp"),YES);
	FindTexture(&txtrFire,UpdateCRC("ai_flame3.bmp"),YES);
	FindTexture(&txtrBlank,UpdateCRC("ai_fullwhite.bmp"),YES);
	FindTexture(&txtrTrail,UpdateCRC("ai_trail.bmp"),YES);
}


/*	--------------------------------------------------------------------------------
	Function		: FreeFXRippleLinkedList
	Purpose			: frees the fx linked list
	Parameters		: 
	Returns			: void
	Info			: 
*/
void FreeSpecFXList()
{
	SPECFX *cur,*next;

	if( !specFXList.numEntries )
		return;

	for(cur = specFXList.head.next; cur != &specFXList.head; cur = next)
	{
		next = cur->next;
		SubSpecFX(cur);
	}
}

/*	--------------------------------------------------------------------------------
	Function		: AddSpecFX
	Purpose			: adds a special effect to the list
	Parameters		: 
	Returns			: 
	Info			: 
*/
void AddSpecFX( SPECFX *fx )
{
	if( !fx->next )
	{
		specFXList.numEntries++;
		fx->prev = &specFXList.head;
		fx->next = specFXList.head.next;
		specFXList.head.next->prev = fx;
		specFXList.head.next = fx;
	}
}

/*	--------------------------------------------------------------------------------
	Function		: SubSpecFX
	Purpose			: removes a special effect from the list
	Parameters		: 
	Returns			: 
	Info			: 
*/
void SubSpecFX( SPECFX *fx )
{
	int i;

	if( !fx->next )
		return;

	fx->prev->next = fx->next;
	fx->next->prev = fx->prev;
	fx->next = NULL;
	specFXList.numEntries--;

	if( fx->sprites )
	{
		if( fx->numP )
			for( i=fx->numP; i; i-- )
				SubSprite( &fx->sprites[i-1] );
		else
			SubSprite( fx->sprites );

		JallocFree( (UBYTE **)&fx->sprites );
	}

	if( fx->particles )
	{
		if(fx->numP)
		{
			i = fx->numP;
			while(i--)
			{
				JallocFree( (UBYTE **)&fx->particles[i].poly );
				JallocFree( (UBYTE **)&fx->particles[i].rMtrx );
			}
		}

		JallocFree( (UBYTE **)&fx->particles );
	}

	if( fx->rebound )
		JallocFree( (UBYTE **)&fx->rebound );

	if( fx->act )
	{
		for( i=fx->numP; i; i-- )
			SubActor(fx->act[i-1]);
		JallocFree( (UBYTE **)&fx->act );
	}

	JallocFree((UBYTE **)&fx);
}


/*	--------------------------------------------------------------------------------
	Function		: SetFXColour
	Purpose			: Sets the colour of the effect and any sprites
	Parameters		: 
	Returns			: 
	Info			: 
*/
void SetFXColour( SPECFX *fx, unsigned char r, unsigned char g, unsigned char b )
{
	int i;

	if( !fx ) return;

	fx->r = r;
	fx->g = g;
	fx->b = b;

	if( fx->sprites )
		for( i=0; i<fx->numP; i++ )
		{
			fx->sprites[i].r = r;
			fx->sprites[i].g = g;
			fx->sprites[i].b = b;
		}

	if( fx->particles )
		for( i=0; i<fx->numP; i++ )
		{
			fx->particles[i].r = r;
			fx->particles[i].g = g;
			fx->particles[i].b = b;
		}
}


/*	--------------------------------------------------------------------------------
	Function		: CreateTeleportEffect
	Purpose			: Series of effect that are made when teleporting
	Parameters		: 
	Returns			: 
	Info			: 
*/
void CreateTeleportEffect( VECTOR *pos, VECTOR *normal, short r, short g, short b )
{
	VECTOR telePos;
	SPECFX *fx;

	SetVector( &telePos, pos );
	fx = CreateAndAddSpecialEffect( FXTYPE_BASICRING, &telePos, &upVec, 30, 0, 0, 1 );
	SetFXColour( fx, r, g, b );

	telePos.v[Y] += 20;
	fx = CreateAndAddSpecialEffect( FXTYPE_BASICRING, &telePos, &upVec, 25, 0, 0, 1 );
	SetFXColour( fx, r, g, b );

	telePos.v[Y] += 40;
	fx = CreateAndAddSpecialEffect( FXTYPE_BASICRING, &telePos, &upVec, 20, 0, 0, 1 );
	SetFXColour( fx, r, g, b );

	telePos.v[Y] += 60;
	fx = CreateAndAddSpecialEffect( FXTYPE_BASICRING, &telePos, &upVec, 15, 0, 0, 1 );
	SetFXColour( fx, r, g, b );
//	PlaySample(88,NULL,255,128);
}


/*	--------------------------------------------------------------------------------
	Function		: ProcessEnemyEffects
	Purpose			: Add special effects to the enemy
	Parameters		: 
	Returns			: void
	Info			: 
*/
void ProcessAttachedEffects( void *entity, int type )
{
	float r;
	VECTOR rPos, up, normal;
	SPECFX *fx = NULL;
	float fxDist;
	ACTOR2 *act;
	GAMETILE *tile;
	unsigned long flags, t;
	PATH *path;

#ifdef NO_ATTACHEDEFFECTS
	return;
#endif

	// Nasty casting - should use generic structure for enemies and platforms"
	if( type == ENTITY_ENEMY )
	{
		ENEMY *nme = (ENEMY *)entity;

		act = nme->nmeActor;
		SetVector( &normal, &nme->inTile->normal );
		tile = nme->inTile;
		flags = nme->flags;
		path = nme->path;
	}
	else if( type == ENTITY_PLATFORM )
	{
		PLATFORM *plt = (PLATFORM *)entity;

		act = plt->pltActor;
		SetVector( &normal, &plt->inTile[0]->normal );
		tile = plt->inTile[0];
		flags = plt->flags;
		path = plt->path;
	}

	fxDist = DistanceBetweenPointsSquared(&frog[0]->actor->pos,&act->actor->pos);

	if( fxDist < ACTOR_DRAWDISTANCEOUTER && actFrameCount > act->fxCount )
	{
		// Restart effect timer
		if( type == ENTITY_ENEMY && (flags & ENEMY_NEW_BABYFROG) ) r = 45;
		else if( (int)act->value1 )
		{
			if( act->effects & EF_RANDOMCREATE )
				r = 60/(Random((int)act->value1)+1);
			else
				r = 60/act->value1;
		}
		else r = 60;

		act->fxCount = actFrameCount + r;

		if( act->effects & EF_RIPPLE_RINGS )
		{
			SetVector( &rPos, &act->actor->pos );
			rPos.v[Y] = tile->centre.v[Y];
			if( act->effects & EF_FAST )
				fx = CreateAndAddSpecialEffect( FXTYPE_BASICRING, &rPos, &normal, 10, 0.3, 0.1, 0.5 );
			else if( act->effects & EF_SLOW )
				fx = CreateAndAddSpecialEffect( FXTYPE_BASICRING, &rPos, &normal, 20, 0.1, 0.05, 0.5 );
			else
				fx = CreateAndAddSpecialEffect( FXTYPE_BASICRING, &rPos, &normal, 20, 0.2, 0.1, 0.5 );

			SetAttachedFXColour( fx, act->effects );
		}
		if( act->effects & EF_SMOKE_STATIC )
		{
			if( act->effects & EF_FAST )
				fx = CreateAndAddSpecialEffect( FXTYPE_SMOKE_STATIC, &act->actor->pos, &normal, 64, 1.5, 0, 1.5 );
			else if( act->effects & EF_SLOW )
				fx = CreateAndAddSpecialEffect( FXTYPE_SMOKE_STATIC, &act->actor->pos, &normal, 64, 0.2, 0, 1.5 );
			else // EF_MEDIUM
				fx = CreateAndAddSpecialEffect( FXTYPE_SMOKE_STATIC, &act->actor->pos, &normal, 64, 0.8, 0, 1.5 );

			SetAttachedFXColour( fx, act->effects );
		}
		if( act->effects & EF_SMOKE_GROWS )
		{
			float gs = (path->nodes[path->fromNode].offset2==0)?1:path->nodes[path->fromNode].offset2;

			if( act->effects & EF_FAST )
				fx = CreateAndAddSpecialEffect( FXTYPE_SMOKE_GROWS, &act->actor->pos, &normal, 42, 1.5, gs, 1.5 );
			else if( act->effects & EF_SLOW )
				fx = CreateAndAddSpecialEffect( FXTYPE_SMOKE_GROWS, &act->actor->pos, &normal, 42, 0.2, gs, 1.5 );
			else // EF_MEDIUM
				fx = CreateAndAddSpecialEffect( FXTYPE_SMOKE_GROWS, &act->actor->pos, &normal, 42, 0.8, gs, 1.5 );

			SetAttachedFXColour( fx, act->effects );
		}
		if( act->effects & EF_SPARKBURST )
		{
			if( act->effects & EF_FAST )
				fx = CreateAndAddSpecialEffect( FXTYPE_SPARKBURST, &act->actor->pos, &normal, 7, 4, 0, 5 );
			else if( act->effects & EF_SLOW )
				fx = CreateAndAddSpecialEffect( FXTYPE_SPARKBURST, &act->actor->pos, &normal, 7, 0.5, 0, 5 );
			else // EF_MEDIUM
				fx = CreateAndAddSpecialEffect( FXTYPE_SPARKBURST, &act->actor->pos, &normal, 7, 2, 0, 5 );

			SetVector( &fx->rebound->point, &tile->centre );
			SetVector( &fx->rebound->point, &tile->normal );
			fx->gravity = act->radius;

			SetAttachedFXColour( fx, act->effects );
		}
		if( act->effects & EF_SMOKEBURST )
		{
			if( act->effects & EF_FAST )
				fx = CreateAndAddSpecialEffect( FXTYPE_SMOKEBURST, &act->actor->pos, &normal, 50, 5, 0, 1.7 );
			else if( act->effects & EF_SLOW )
				fx = CreateAndAddSpecialEffect( FXTYPE_SMOKEBURST, &act->actor->pos, &normal, 50, 0.6, 0, 1.7 );
			else // EF_MEDIUM
				fx = CreateAndAddSpecialEffect( FXTYPE_SMOKEBURST, &act->actor->pos, &normal, 50, 2, 0, 1.7 );

			SetAttachedFXColour( fx, act->effects );
		}
		if( act->effects & EF_FIERYSMOKE )
		{
			if( act->effects & EF_FAST )
				fx = CreateAndAddSpecialEffect( FXTYPE_FIERYSMOKE, &act->actor->pos, &normal, 50, 4, 0, 2.5 );
			else if( act->effects & EF_SLOW )
				fx = CreateAndAddSpecialEffect( FXTYPE_FIERYSMOKE, &act->actor->pos, &normal, 50, 1, 0, 2.5 );
			else // EF_MEDIUM
				fx = CreateAndAddSpecialEffect( FXTYPE_FIERYSMOKE, &act->actor->pos, &normal, 50, 2.5, 0, 2.5 );

//				SetAttachedFXColour( fx, act->effects );
		}
		if( act->effects & EF_FLAMES )
		{
			if( act->effects & EF_FAST )
				fx = CreateAndAddSpecialEffect( FXTYPE_FLAMES, &act->actor->pos, &normal, 50, 2, 0, 0.7 );
			else if( act->effects & EF_SLOW )
				fx = CreateAndAddSpecialEffect( FXTYPE_FLAMES, &act->actor->pos, &normal, 50, 0.2, 0, 0.7 );
			else // EF_MEDIUM
				fx = CreateAndAddSpecialEffect( FXTYPE_FLAMES, &act->actor->pos, &normal, 50, 0.9, 0, 0.7 );

			SetAttachedFXColour( fx, act->effects );
		}
		if( act->effects & EF_BUBBLES )
		{
			if( act->effects & EF_FAST )
				fx = CreateAndAddSpecialEffect( FXTYPE_BUBBLES, &act->actor->pos, &normal, 8, 1.5, 0, 0.5 );
			else if( act->effects & EF_SLOW )
				fx = CreateAndAddSpecialEffect( FXTYPE_BUBBLES, &act->actor->pos, &normal, 8, 0.3, 0, 0.5 );
			else // EF_MEDIUM
				fx = CreateAndAddSpecialEffect( FXTYPE_BUBBLES, &act->actor->pos, &normal, 8, 0.7, 0, 0.5 );

			fx->rebound = (PLANE2 *)JallocAlloc( sizeof(PLANE2), YES, "Rebound" );
			SetVector( &up, &path->nodes[0].worldTile->normal );
			SetVector( &fx->rebound->normal, &up );
			ScaleVector( &up, act->radius );
			AddVector( &fx->rebound->point, &act->actor->pos, &up );

			SetAttachedFXColour( fx, act->effects );
		}
	}

	// Persistent effects
	if( act->effects & EF_FLYSWARM )
	{
		fx = CreateAndAddSpecialEffect( FXTYPE_FLYSWARM, &act->actor->pos, &normal, 25, 0, 0, 0 );
		fx->follow = act->actor;
		if( type == 1 && (flags & ENEMY_NEW_FLAPPYTHING) )
		{
			fx->rebound = (PLANE2 *)JallocAlloc( sizeof(PLANE2), YES, "Rebound" );
			GetPositionForPathNode( &rPos, &path->nodes[0] );
			SetVector( &fx->rebound->point, &rPos );
			SetVector( &fx->rebound->normal, &path->nodes[0].worldTile->normal );
		}
		act->effects &= ~EF_FLYSWARM;
	}
	else if( act->effects & EF_BUTTERFLYSWARM )
	{
		fx = CreateAndAddSpecialEffect( FXTYPE_BUTTERFLYSWARM, &act->actor->pos, &normal, act->radius, 0, 0, act->value1 );
		fx->follow = act->actor;
		if( type == ENTITY_ENEMY && (flags & ENEMY_NEW_FLAPPYTHING) )
		{
			fx->rebound = (PLANE2 *)JallocAlloc( sizeof(PLANE2), YES, "Rebound" );
			GetPositionForPathNode( &rPos, &path->nodes[0] );
			SetVector( &fx->rebound->point, &rPos );
			SetVector( &fx->rebound->normal, &path->nodes[0].worldTile->normal );
		}
		act->effects &= ~EF_BUTTERFLYSWARM;
	}
	else if( act->effects & EF_TRAIL )
	{
		if( act->effects & EF_FAST )
			fx = CreateAndAddSpecialEffect( FXTYPE_TRAIL, &act->actor->pos, &normal, act->value1, 0.95, 0.05, 0.6 );
		else if( act->effects & EF_SLOW )
			fx = CreateAndAddSpecialEffect( FXTYPE_TRAIL, &act->actor->pos, &normal, act->value1, 0.95, 0.05, 3 );
		else
			fx = CreateAndAddSpecialEffect( FXTYPE_TRAIL, &act->actor->pos, &normal, act->value1, 0.95, 0.05, 2 );

		fx->follow = act->actor;
		SetAttachedFXColour( fx, act->effects );
		act->effects &= ~EF_TRAIL;
	}
	else if( act->effects & EF_BILLBOARDTRAIL )
	{
		if( act->effects & EF_FAST )
			fx = CreateAndAddSpecialEffect( FXTYPE_BILLBOARDTRAIL, &act->actor->pos, &normal, act->value1, 0.95, 0.05, 0.6 );
		else if( act->effects & EF_SLOW )
			fx = CreateAndAddSpecialEffect( FXTYPE_BILLBOARDTRAIL, &act->actor->pos, &normal, act->value1, 0.95, 0.05, 3 );
		else
			fx = CreateAndAddSpecialEffect( FXTYPE_BILLBOARDTRAIL, &act->actor->pos, &normal, act->value1, 0.95, 0.05, 2 );

		fx->follow = act->actor;
		SetAttachedFXColour( fx, act->effects );
		act->effects &= ~EF_BILLBOARDTRAIL;
	}
}


void SetAttachedFXColour( SPECFX *fx, int effects )
{
	unsigned char r=60, g=60, b=60;

	if( effects & EF_TINTRED )
		r = 255;
	if( effects & EF_TINTGREEN )
		g = 255;
	if( effects & EF_TINTBLUE )
		b = 255;

	SetFXColour( fx, r, g, b );
}


/*	--------------------------------------------------------------------------------
	Function		: CreateBlastRing
	Purpose			: Make a generic ring of polys
	Parameters		: 
	Returns			: void
	Info			: 
*/
#ifdef PC_VERSION
void CreateBlastRing( )
{
	float tesa, tesb, teca, tecb, pB, arcStep = PI2 / NUM_RINGSEGS;
	unsigned long i, v;

	if( !ringVtx )
		ringVtx = (D3DTLVERTEX *)JallocAlloc(sizeof(D3DTLVERTEX)*NUM_RINGSEGS*4, NO, "D3DTLVERTEX");

	for( i=0; i<NUM_RINGSEGS; i++ )
	{
		pB = i*arcStep;
		v = i*4;

		tesa = sinf(pB);
		tesb = sinf(pB+arcStep);

		teca = cosf(pB);
		tecb = cosf(pB+arcStep);

		ringVtx[v+0].sx = tesa;
		ringVtx[v+0].sy = 0.5;
		ringVtx[v+0].sz = teca;
		ringVtx[v+0].color = D3DRGBA(1,1,1,1);
		ringVtx[v+0].specular = D3DRGB(0,0,0);
		ringVtx[v+0].tu = 1;
		ringVtx[v+0].tv = 1;
		ringVtx[v+1].sx = tesb;
		ringVtx[v+1].sy = 0.5;
		ringVtx[v+1].sz = tecb;
		ringVtx[v+1].color = D3DRGBA(1,1,1,1);
		ringVtx[v+1].specular = D3DRGB(0,0,0);
		ringVtx[v+1].tu = 0;
		ringVtx[v+1].tv = 1;
		ringVtx[v+2].sx = tesb;
		ringVtx[v+2].sy = -0.5;
		ringVtx[v+2].sz = tecb;
		ringVtx[v+2].color = D3DRGBA(1,1,1,1);
		ringVtx[v+2].specular = D3DRGB(0,0,0);
		ringVtx[v+2].tu = 0;
		ringVtx[v+2].tv = 0;
		ringVtx[v+3].sx = tesa;
		ringVtx[v+3].sy = -0.5;
		ringVtx[v+3].sz = teca;
		ringVtx[v+3].color = D3DRGBA(1,1,1,1);
		ringVtx[v+3].specular = D3DRGB(0,0,0);
		ringVtx[v+3].tu = 1;
		ringVtx[v+3].tv = 0;
	}
}
#else
void CreateBlastRing()
{
	float tesa, tesb, teca, tecb, pB, arcStep = PI2 / NUM_RINGSEGS;
	unsigned long i, v;
	Vtx *vRPtr;
	
	if(!ringVtx)
		ringVtx = (Vtx *)JallocAlloc(sizeof(Vtx) * NUM_RINGSEGS * 4,NO,"Vtx");

	for(i=0; i<NUM_RINGSEGS; i++)
	{

		pB = i * arcStep;
		v = i * 4;

		tesa = sinf(pB);
		tesb = sinf(pB + arcStep);

		teca = cosf(pB);
		tecb = cosf(pB + arcStep);

		vRPtr = &ringVtx[i];

		V(vRPtr++,tesa,0.5,teca,0,1024,1024,255,255,255,255);
		V(vRPtr++,tesb,0.5,tecb,0,0,1024,255,255,255,255);
		V(vRPtr++,tesb,-0.5,tecb,0,0,0,255,255,255,255);
		V(vRPtr++,tesa,-0.5,teca,0,1024,0,255,255,255,255);
	}
}


#endif