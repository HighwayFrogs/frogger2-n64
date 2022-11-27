/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: script.c
	Programmer	: David Swift

----------------------------------------------------------------------------------------------- */


#include "ultra64.h"
#include "incs.h"
#include "codes.h"
#include "stdio.h"
#include "script.h"
#include "memload.h"

#ifdef _DEBUG
#define DEBUG_SCRIPTING
#endif

#define SCRIPT_FEV_VERSION	3

/* --------------------------------------------------------------------------------- */

#define FS_SET_MOVE			1
#define FS_SET_STOP			2
#define FS_SET_TOGGLEMOVE	3
#define FS_SET_INVIS		4
#define FS_SET_VIS			5
#define FS_SET_TOGGLEVIS	6
#define FS_SET_DRAW			7
#define FS_SET_NODRAW		8
#define FS_SET_TOGGLEDRAW	9

#define SCRIPT_MAX_FLAGS	64
#define SCRIPT_FLAGS_BYTES	(SCRIPT_MAX_FLAGS/8)
#define SCRIPT_MAX_COUNTERS	16

unsigned char scriptFlags[SCRIPT_FLAGS_BYTES] = { 0, 0, 0, 0, 0, 0, 0, 0 };
short scriptCounters[SCRIPT_MAX_COUNTERS] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };



#ifdef DEBUG_SCRIPTING
int lineNumber = 0;

#define SCRIPT_VALIDATE_FLAG(x)		if ((x) < 0 || (x) > SCRIPT_MAX_FLAGS) { PrintScriptDebugMessage("Invalid toggle number"); return 0; }
#define SCRIPT_VALIDATE_COUNTER(x)	if ((x) < 0 || (x) > SCRIPT_MAX_COUNTERS) { PrintScriptDebugMessage("Invalid counter number"); return 0; }

#else

#define SCRIPT_VALIDATE_FLAG(x)
#define SCRIPT_VALIDATE_COUNTER(x)

#endif

typedef struct tagRUNANIMINFO
{
	int anim, loop, queue;
	float speed;
} RUNANIMINFO;

/* --------------------------------------------------------------------------------- */

TRIGGER *LoadTrigger(UBYTE**);
BOOL ExecuteCommand(UBYTE**);
void LoadTestScript(const char* filename);

void InterpretEvent( EVENT *e );
int ANDtrigger(TRIGGER *t);
int ORtrigger(TRIGGER *t);
int NOTtrigger(TRIGGER *t);
int AlwaysTrigger(TRIGGER *t);
int OnFlagSet(TRIGGER *t);
int OnCounterEquals(TRIGGER *t);

int EnumPlatforms(long id, int (*func)(PLATFORM*, int), int param);
int EnumEnemies(long id, int (*func)(ENEMY*, int), int param);

int SetEnemyFlag(ENEMY *e, int flag);
int ResetEnemyFlag(ENEMY *e, int flag);
int SetPlatformFlag(PLATFORM *p, int flag);
int ResetPlatformFlag(PLATFORM *p, int flag);
int CollapsePlatform(PLATFORM *p, int time);
int AnimateEnemy(ENEMY *e, int params);
int AnimatePlatform(PLATFORM *p, int params);


UBYTE* scriptBuffer;


typedef struct TAGSCRIPT_EFFECT_PARAMS
{
	int type;
	float size, speed, accn, lifetime;
} SCRIPT_EFFECT_PARAMS;

/* --------------------------------------------------------------------------------- */

float volTest = 0.5;

float ChangeVolume(int a, float delta)
{
	volTest += (delta*gameSpeed);
	return volTest;
}

/*	--------------------------------------------------------------------------------
    Function		: InitScripting
	Parameters		: void
	Returns			: TRUE for success (currently ALWAYS returns true)
*/

int InitScripting(void)
{
	memset(scriptFlags, 0, SCRIPT_FLAGS_BYTES);
	return TRUE;
}

/*	--------------------------------------------------------------------------------
    Function		: PrintScriptDebugMessage
	Parameters		: const char* str
	Returns			: 
*/

#ifdef DEBUG_SCRIPTING

void PrintScriptDebugMessage(const char* str)
{
	if (lineNumber)
	{
		sprintf(statusMessage, "(script) %s (%d)", str, lineNumber);
		dprintf"[Interpreter Debug] %s (line %d)\n", str, lineNumber));
	}
	else
	{
		sprintf(statusMessage, "(script) %s", str);
		dprintf"[Interpreter Debug] %s\n", str));
	}
}

#else
#define PrintScriptDebugMessage(foo)
#endif

/*	--------------------------------------------------------------------------------
    Function		: GetTileFromNumber
	Parameters		: int
	Returns			: GAMETILE*
*/

#ifdef DEBUG_SCRIPTING
GAMETILE *GetTileFromNumber(int number)
{
	GAMETILE *tile;
	for (tile = firstTile; number && tile; number--, tile = tile->next);
	if (!tile)
	{
		char buf[40];
		sprintf(buf, "Invalid tile number: %d", number);
		PrintScriptDebugMessage(buf);
	}
	return tile;
}
#else
#define GetTileFromNumber(i) (&firstTile[i])
#endif

/*	--------------------------------------------------------------------------------
    Function		: EnumPlaceholderTiles
	Parameters		: int, function
	Returns			: 
*/

int EnumPlaceholderTiles(long id, int (*func)(PATHNODE*, int), int param)
{
	ENEMY *cur;
	PATHNODE *p;
	int n, count;

	for(cur = enemyList.head.next; cur != &enemyList.head; cur = cur->next, count++)
	{
		if (!id || cur->uid == id)
		{
			for (p = cur->path->nodes, n = cur->path->numNodes; n; p++, n--)
				if (!func(p, param)) break;
		}
	}

	return count;
}


int FrogOnPath(TRIGGER *t)
{
	PATHNODE *node;
	int n;
	int pl;
	PATH *p;
	
	pl = (int)t->data[0];
	
	// if true, we're either going to land on the tile shortly,
	// or we're double-jumping over it - skip the test
	if (player[pl].isSuperHopping) return 0;	

	p = (PATH*)t->data[1];

	for (node = p->nodes, n = p->numNodes; n; n--, node++)
		if (node->worldTile == currTile[pl]) return 1;

	return 0;
}


/*	--------------------------------------------------------------------------------
    Function		: InterpretEvent
	Parameters		: EVENT*
	Returns			: 

	Passed to the Event/Trigger code as a function pointer in an EVENT. Parameter 0
	contains a pointer to a block of code which is sent to interpret()
*/

void InterpretEvent( EVENT *e )
{
	UBYTE *buffer = (UBYTE *)e->data[0];
	if (!Interpret(buffer))
		dprintf "Error running script within trigger!\n"));
}

int ANDtrigger(TRIGGER *t)
{
	TRIGGER *a = (TRIGGER*)t->data[0], *b = (TRIGGER*)t->data[1];
	return a->func(a) && b->func(b);
}

int ORtrigger(TRIGGER *t)
{
	TRIGGER *a = (TRIGGER*)t->data[0], *b = (TRIGGER*)t->data[1];
	return a->func(a) || b->func(b);
}

int NOTtrigger(TRIGGER *t)
{
	TRIGGER *a = (TRIGGER*)t->data[0];
	return !a->func(a);
}

int AlwaysTrigger(TRIGGER *t) { return TRUE; }

int OnFlagSet(TRIGGER *t)
{
	int f = (int)t->data[0];
	return (scriptFlags[f >> 3] & (1 << (f & 7))) != 0;
}

int OnCounterEquals(TRIGGER *t)
{
	int c = (int)t->data[0], v = (int)t->data[1];
	return (scriptCounters[c] == v);
}

/*	--------------------------------------------------------------------------------
    Function		: SetEnemy
	Parameters		: ENEMY*, int
	Returns			: 
*/
int SetEnemy(ENEMY *nme, int v)
{
	switch (v)
	{
	case FS_SET_MOVE:
		SetEnemyMoving(nme, 1);
		break;

	case FS_SET_STOP:
		SetEnemyMoving(nme, 0);
		break;
	
	case FS_SET_TOGGLEMOVE:
		SetEnemyMoving(nme, !nme->isWaiting);
		break;

	case FS_SET_INVIS:
		SetEnemyVisible(nme, 0);
		break;

	case FS_SET_VIS:
		SetEnemyVisible(nme, 1);
		break;

	case FS_SET_TOGGLEVIS:
		SetEnemyVisible(nme, !nme->active);
		break;

	case FS_SET_DRAW:
		nme->nmeActor->draw = 1;
		break;

	case FS_SET_NODRAW:
		nme->nmeActor->draw = 0;
		break;

	case FS_SET_TOGGLEDRAW:
		nme->nmeActor->draw = !nme->nmeActor->draw;
		break;
	}

	return 1;
}

/*	--------------------------------------------------------------------------------
    Function		: SetPlatform
	Parameters		: PLATFORM*, int
	Returns			: 
*/
int SetPlatform(PLATFORM *plt, int v)
{
	switch (v)
	{
	case FS_SET_MOVE:
		SetPlatformMoving(plt, 1);
		break;

	case FS_SET_STOP:
		SetPlatformMoving(plt, 0);
		break;
	
	case FS_SET_TOGGLEMOVE:
		SetPlatformMoving(plt, (plt->isWaiting == -1));
		break;

	case FS_SET_INVIS:
		SetPlatformVisible(plt, 0);
		break;

	case FS_SET_VIS:
		SetPlatformVisible(plt, 1);
		break;

	case FS_SET_TOGGLEVIS:
		SetPlatformVisible(plt, !plt->active);
		break;

	case FS_SET_DRAW:
		plt->pltActor->draw = 1;
		break;

	case FS_SET_NODRAW:
		plt->pltActor->draw = 0;
		break;

	case FS_SET_TOGGLEDRAW:
		plt->pltActor->draw = !plt->pltActor->draw;
		break;
	}

	return 1;
}

int PathEffect(ENEMY *nme, int params)
{
	SCRIPT_EFFECT_PARAMS *p;
	GAMETILE *tile;
	PATHNODE *node = nme->path->nodes;
	VECTOR v;
	int c = nme->path->numNodes;
	
	(int)p = params;

	while (c--)
	{
		GetPositionForPathNode(&v, node);
		CreateAndAddSpecialEffect(p->type, &v, &node->worldTile->normal, p->size, p->speed, p->accn, p->lifetime);
		node++;
	}
	return 1;
}

int EnemyEffect(ENEMY *nme, int params)
{
	SCRIPT_EFFECT_PARAMS *p;
	(int)p = params;
	CreateAndAddSpecialEffect(p->type, &nme->nmeActor->actor->pos, &nme->inTile->normal, p->size, p->speed, p->accn, p->lifetime);
	return 1;
}

int SetTile(PATHNODE *node, int state)
{
	node->worldTile->state = state;
	return 1;
}

/*	-------------------------------------------------------------------------------- */


int SetEnemyFlag(ENEMY *e, int flag)	{ e->flags |= flag; e->Update(e); return 1; }
int ResetEnemyFlag(ENEMY *e, int flag)	{ e->flags &= ~flag; e->Update(e); return 1; }

int SetPlatformFlag(PLATFORM *p, int flag)		{ p->flags |= flag;	p->Update(p); return 1; }
int ResetPlatformFlag(PLATFORM *p, int flag)	{ p->flags &= ~flag; p->Update(p); return 1; }

int CollapsePlatform(PLATFORM *p, int time)
{
	p->countdown = time + Random(time / 4); return 1;
}

int AnimateEnemy(ENEMY *e, int params)
{
	RUNANIMINFO *i = (RUNANIMINFO*)params;
	AnimateActor(e->nmeActor->actor, i->anim, i->loop, i->queue, i->speed, NO, NO);
	return 1;
}

int AnimatePlatform(PLATFORM *p, int params)
{
	RUNANIMINFO *i = (RUNANIMINFO*)params;
	AnimateActor(p->pltActor->actor, i->anim, i->loop, i->queue, i->speed, NO, NO);	
	return 1;
}

/*	--------------------------------------------------------------------------------
    Function		: LoadTrigger
	Parameters		: UBYTE*
	Returns			: TRIGGER*
*/
TRIGGER *LoadTrigger(UBYTE **p)
{
	TRIGGER *trigger = NULL;
	void **params;
	UBYTE token = MEMGETBYTE(p);

	switch (token)
	{
	case TR_ENEMYONTILE:
		{
			ENEMY *enemy;

			params = AllocArgs(2);
			if (!(enemy = GetEnemyFromUID(MEMGETWORD(p)))) return 0;
			params[0] = (void*)enemy;
			params[1] = (void*)GetTileFromNumber(MEMGETWORD(p));
			trigger = MakeTrigger(EnemyOnTile, params);
		}
		break;

	case TR_FROGONTILE:
		{
			params = AllocArgs(2);
			params[0] = (void*)MEMGETBYTE(p);
			params[1] = (void*)GetTileFromNumber(MEMGETWORD(p));
			trigger = MakeTrigger(FrogOnTile, params);
		}
		break;

	case TR_FROGONPLATFORM:
		{
			PLATFORM *platform;

			params = AllocArgs(2);
			(int)params[0] = MEMGETBYTE(p);
			(int)params[1] = MEMGETWORD(p);
			trigger = MakeTrigger(FrogOnPlatform, params);
		}
		break;

	case TR_ENEMYATFLAG:
		{
			params = AllocArgs(2);
			(int)params[0] = MEMGETWORD(p);
			(int)params[1] = MEMGETWORD(p);
			trigger = MakeTrigger(EnemyAtFlag, params);
		}
		break;

	case TR_PLATATFLAG:
		{
			params = AllocArgs(2);
			(int)params[0] = MEMGETWORD(p);
			(int)params[1] = MEMGETWORD(p);
			trigger = MakeTrigger(PlatformAtFlag, params);
		}
		break;

	case TR_OR:
		{
			TRIGGER *a, *b;

			a = LoadTrigger(p);
			if (!a) return 0;

			b = LoadTrigger(p);
			if (!b) return 0;

			params = AllocArgs(2);
			params[0] = (void*)a; params[1] = (void*)b;
			trigger = MakeTrigger(ORtrigger, params);
		}
		break;

	case TR_AND:
		{
			TRIGGER *a, *b;

			a = LoadTrigger(p);
			if (!a) return 0;

			b = LoadTrigger(p);
			if (!b) return 0;

			params = AllocArgs(2);
			params[0] = (void*)a; params[1] = (void*)b;
			trigger = MakeTrigger(ANDtrigger, params);
		}
		break;

	case TR_NOT:
		{
			TRIGGER *a;
			a = LoadTrigger(p);
			if (!a) return 0;
			params = AllocArgs(1);
			params[0] = (void*)a;
			trigger = MakeTrigger(NOTtrigger, params);
		}
		break;

	case TR_WAIT:
		{
			float time = MEMGETFLOAT(p);

			params = AllocArgs(2);
			(int)params[0] = actFrameCount + (int)(60.0 * time);
			(int)params[1] = (int)(60.0 * time);
			trigger = MakeTrigger(OnTimeout, params);
		}
		break;

	case TR_FLAGSET:
		{
			int flag = MEMGETBYTE(p);
			params = AllocArgs(1);
			(int)params[0] = flag;
			trigger = MakeTrigger(OnFlagSet, params);
		}
		break;

	case TR_COUNTEREQUALS:
		{
			int counter = MEMGETBYTE(p), value = MEMGETWORD(p);
			params = AllocArgs(2);
			(int)params[0] = counter;
			(int)params[1] = value;
			trigger = MakeTrigger(OnCounterEquals, params);
		}
		break;
		
	case TR_FROGISDEAD:
		{
			params = AllocArgs(1);
			(int)params[0] = MEMGETBYTE(p);
			trigger = MakeTrigger(FrogIsDead, params);
		}
		break;

	case TR_LEVELISOPEN:
		{
			params = AllocArgs(2);
			(int)params[0] = MEMGETBYTE(p);
			(int)params[1] = MEMGETBYTE(p);
			trigger = MakeTrigger(LevelIsOpen, params);
		}
		break;

	case TR_ONPATH:
		{
			ENEMY *e;
			params = AllocArgs(2);
			(int)params[0] = MEMGETBYTE(p);
			
			if (!(e = GetEnemyFromUID(MEMGETWORD(p))))
				return 0;
			
			(PATH*)params[1] = e->path;
			trigger = MakeTrigger(FrogOnPath, params);
			break;
		}

	default:
#ifdef DEBUG_SCRIPTING
		dprintf"Unrecognised trigger type %02x, skipping\n", token));
#endif
		return NULL;
	}

	return trigger;
}

/*	--------------------------------------------------------------------------------
    Function		: ExecuteCommand
	Parameters		: UBYTE*
	Returns			: TRUE if successful, FALSE otherwise

	NOTE! (*p) MUST now be incremented to the start of the next command
*/
BOOL ExecuteCommand(UBYTE **p)
{
	UBYTE command = MEMGETBYTE(p);

	switch (command)
	{
#ifdef DEBUG_SCRIPTING
	case EV_DEBUG:
		{
			char string[80];
			int len;

			len = MEMGETBYTE(p);
			memcpy(string, (*p), len);
			string[len] = 0;
			(*p) += len;

			PrintScriptDebugMessage(string);
			break;
		}
#endif
		
	case EV_SETTILE:
		{
			int tile, state;
			
			tile = MEMGETWORD(p);
			state = MEMGETBYTE(p);

			firstTile[tile].state = (unsigned char)state;
			break;
		}

	case EV_SETENEMY:
		{
			long id = (long)MEMGETWORD(p);
			int foo = (int)MEMGETBYTE(p);
			if (EnumEnemies(id, SetEnemy, foo) == 0) return 0;

			// if (EnumEnemies((long)MEMGETWORD(p), SetEnemy, (int)MEMGETBYTE(p)) == 0) return 0;
			break;
		}

	case EV_SETPLATFORM:
		{
			long id = (long)MEMGETWORD(p);
			int foo = (int)MEMGETBYTE(p);
			if (EnumPlatforms(id, SetPlatform, foo) == 0) return 0;
			break;
		}

	case EV_SETENEMYFLAG:
		{
			long id = (long)MEMGETWORD(p);
			unsigned flags = (unsigned)MEMGETINT(p);
			if (EnumEnemies(id, SetEnemyFlag, flags) == 0) return 0;
			break;
		}
		
	case EV_RESETENEMYFLAG:
		{
			long id = (long)MEMGETWORD(p);
			unsigned flags = (unsigned)MEMGETINT(p);
			if (EnumEnemies(id, ResetEnemyFlag, flags) == 0) return 0;
			break;
		}

	case EV_SETPLATFLAG:
		{
			long id = (long)MEMGETWORD(p);
			unsigned flags = (unsigned)MEMGETINT(p);
			if (EnumPlatforms(id, SetPlatformFlag, flags) == 0) return 0;
			break;
		}

	case EV_RESETPLATFLAG:
		{
			long id = (long)MEMGETWORD(p);
			unsigned flags = (unsigned)MEMGETINT(p);
			if (EnumPlatforms(id, ResetPlatformFlag, flags) == 0) return 0;
			break;
		}
		
	case EV_ANIMATEACTOR:
		{
			RUNANIMINFO i;
			int id, flags;

			id = MEMGETWORD(p);

			i.anim = MEMGETBYTE(p);
			flags = MEMGETBYTE(p);
			i.loop = (flags & 1);
			i.queue = (flags & 2);
			i.speed = MEMGETFLOAT(p);

			EnumEnemies(id, AnimateEnemy, (int)&i);
			EnumPlatforms(id, AnimatePlatform, (int)&i);
			break;
		}

	case EV_ON:
		{
			TRIGGER *t;
			EVENT *e;
			int flags, size;
			void **param = AllocArgs(1);

			if (t = LoadTrigger(p))
			{
				flags = MEMGETBYTE(p);
				
				size = MEMGETINT(p); 
				param[0] = *p;

				e = MakeEvent( InterpretEvent, param );
				AttachEvent(t, e, (short)flags, 0);

				(*p) += size;	// skip block of code!
			}
			else
				return 0;
			break;
		}

	case EV_IF:
		{
			TRIGGER *t;

			if (t = LoadTrigger(p))
			{
				UBYTE *q;
				int size = MEMGETINT(p);

				if (t->func(t))
					Interpret(*p);

				SubTrigger(t);
				
				(*p) += size;
			}
			else return 0;
			break;
		}

	case EV_TELEPORT:
		{
			TRIGGER *t;
			EVENT *e;
			int fNum = MEMGETBYTE(p),
				tNum = MEMGETWORD(p),
				time = (MEMGETFLOAT(p) * 60) + actFrameCount;
			VECTOR telePos;
			void **param;

			if( player[fNum].frogState & FROGSTATUS_ISDEAD ) break;

			player[fNum].frogState = FROGSTATUS_ISTELEPORTING;	// clear ALL other flags
			player[fNum].canJump = 0;
			FrogLeavePlatform(fNum);	// bah

			CreateTeleportEffect( &frog[fNum]->actor->pos, &currTile[fNum]->normal, 255, 255, 255 );

			param = AllocArgs(1);
			param[0] = (void *)time;
			t = MakeTrigger( OnTimeout, param );

			param = AllocArgs(2);
			param[0] = (void *)fNum;
			param[1] = (void *)(firstTile + tNum);
			e = MakeEvent( TeleportFrog, param );

			AttachEvent( t, e, TRIGGER_ONCE, 0 );

			break;
		}

	case EV_SPRING:
		{
			GAMETILE *tile;	int pl;	float height, time;

			pl = MEMGETBYTE(p);
			tile = GetTileFromNumber(MEMGETWORD(p));
			height = MEMGETFLOAT(p);
			time = MEMGETFLOAT(p);

			SpringFrogToTile(tile, height, time, pl);

			break;
		}

	case EV_MOVEENEMY:
		{
			long id = MEMGETWORD(p);
			int flag = MEMGETWORD(p);
			EnumEnemies(id, MoveEnemyToNode, flag);
			break;
		}

	case EV_MOVEPLAT:
		{
			long id = MEMGETWORD(p);
			int flag = MEMGETWORD(p);
			EnumPlatforms(id, MovePlatformToNode, flag);
			break;
		}

	case EV_DROPGARIB:
		{
			GARIB *garib;
			GAMETILE *tile;
			int g, t;

			g = MEMGETWORD(p);
			t = MEMGETWORD(p);

			if (g < 0 || g > garibList.count || t < 0) return 0;

			for (garib = garibList.head.next; g; garib = garib->next, g--);
			tile = GetTileFromNumber(t);

			garib->gameTile = tile;
			garib->dropSpeed = 0.0;
			break;
		}

	case EV_CHANGEVOLUME:
		{
			float delta, amount;
			int n, platnum;
			PLATFORM *plt;
			VECTOR u, v;

			n = MEMGETBYTE(p);
			delta = MEMGETFLOAT(p);
			platnum = MEMGETWORD(p);
			amount = ChangeVolume(n, delta);

			plt = GetPlatformFromUID(platnum);
			if (!plt || (plt->path->numNodes != 2)) return 0;

			GetPositionForPathNode(&u, &plt->path->nodes[1]);
			GetPositionForPathNode(&v, &plt->path->nodes[0]);

			ScaleVector(&u, amount);
			ScaleVector(&v, 1.0-amount);
			AddVector(&plt->pltActor->actor->pos, &u, &v);
			plt->isWaiting = -1;
			break;
		}

	case EV_CHANGELEVEL:
		{
			int world = MEMGETBYTE(p);
			int level = MEMGETBYTE(p);

			player[0].worldNum = world;
			player[0].levelNum = level;

			gameState.mode = LEVELCOMPLETE_MODE;
			gameState.multi = SINGLEPLAYER;

			GTInit( &modeTimer, 1 );
			showEndLevelScreen = 0;
			break;
		}

	case EV_SETSTARTTILE:	gTStart[0] = GetTileFromNumber(MEMGETWORD(p)); break;

	case EV_FOG:
		{
			fog.r = MEMGETBYTE(p);
			fog.g = MEMGETBYTE(p);
			fog.b = MEMGETBYTE(p);
			fog.min = MEMGETWORD(p);
			fog.max = MEMGETWORD(p);
			fog.mode = 1;
			break;
		}

	case EV_NO_FOG:	fog.mode = 0; break;

	case EV_CAMEOMODE_ON: cameoMode = 1; break;

	case EV_CAMEOMODE_OFF:
		{
			int pl;
			for (pl=0; pl<4; pl++)
				player[pl].canJump = 1;
			cameoMode = 0; break;
		}

	case EV_HOP:
		{
			int pl = MEMGETBYTE(p);
			GAMETILE *tile = GetTileFromNumber(MEMGETWORD(p));
			HopFrogToTile(tile, pl);
			break;
		}

	case EV_SETFLAG:
		{
			int flag = MEMGETBYTE(p);
			scriptFlags[flag >> 3] |= (1 << (flag & 7));
			break;
		}

	case EV_RESETFLAG:
		{
			int flag = MEMGETBYTE(p);
			scriptFlags[flag >> 3] &= ~(1 << (flag & 7));
			break;
		}

	case EV_SFX_TILE:
		{
			short type;
			float a, b, c, d;
			GAMETILE *tile;

			tile = GetTileFromNumber(MEMGETWORD(p));
			if (!tile) return 0;

			type = MEMGETBYTE(p);
			a = MEMGETFLOAT(p);
			b = MEMGETFLOAT(p);
			c = MEMGETFLOAT(p);
			d = MEMGETFLOAT(p);

			CreateAndAddSpecialEffect(type, &tile->centre, &tile->normal, a, b, c, d);
			break;
		}

	case EV_SFX_ENEMY:
		{
			int id;
			SCRIPT_EFFECT_PARAMS params;
			//float a, b, c, d;
			GAMETILE *tile;

			id = MEMGETWORD(p);

			params.type = MEMGETBYTE(p);
			params.size = MEMGETFLOAT(p);
			params.speed = MEMGETFLOAT(p);
			params.accn = MEMGETFLOAT(p);
			params.lifetime = MEMGETFLOAT(p);

			EnumEnemies(id, EnemyEffect, (int)&params);
			break;
		}

	case EV_SFX_PLACEHOLDER:
		{
			int id;
			SCRIPT_EFFECT_PARAMS params;
			//float a, b, c, d;
			GAMETILE *tile;

			id = MEMGETWORD(p);

			params.type = MEMGETBYTE(p);
			params.size = MEMGETFLOAT(p);
			params.speed = MEMGETFLOAT(p);
			params.accn = MEMGETFLOAT(p);
			params.lifetime = MEMGETFLOAT(p);

			EnumEnemies(id, PathEffect, (int)&params);
			break;
		}

	case EV_SCALENMESPEED:
		{
			ENEMY *nme;
			float speed;

			nme = GetEnemyFromUID(MEMGETWORD(p));
			if (!nme) return 0;

			speed = MEMGETFLOAT(p);

			ScalePathSpeed(nme->path, speed);
			break;
		}
			
	case EV_SCALEPLATSPEED:
		{
			PLATFORM *plat;
			float speed;

			plat = GetPlatformFromUID(MEMGETWORD(p));
			if (!plat) return 0;

			speed = MEMGETFLOAT(p);

			ScalePathSpeed(plat->path, speed);
			break;
		}

	case EV_INC_COUNTER:
		{
			unsigned char counter = MEMGETBYTE(p);
			SCRIPT_VALIDATE_COUNTER(counter);
			scriptCounters[counter]++;
			break;
		}

	case EV_DEC_COUNTER:
		{
			unsigned char counter = MEMGETBYTE(p);
			SCRIPT_VALIDATE_COUNTER(counter);
			scriptCounters[counter]--;
			break;
		}

	case EV_SET_COUNTER:
		{
			unsigned char counter = MEMGETBYTE(p);
			SCRIPT_VALIDATE_COUNTER(counter);
			scriptCounters[counter] = MEMGETWORD(p);
			break;
		}

	case EV_COLLAPSE_PLAT:
		{
			int id = MEMGETWORD(p);
			int time = MEMGETFLOAT(p) * 60;
			EnumPlatforms(id, CollapsePlatform, time);
			break;
		}

	case EV_CHANGEFROG:
		{
			char model[20];
			int pl, l;

			pl = MEMGETBYTE(p);
			l = MEMGETBYTE(p);
			if (l > 19) {
				PrintScriptDebugMessage("Model names must be < 20 chars");
				return 0;
			}
			memcpy(model, *p, l);
			model[l] = 0;
			(*p) += l;

			SwapActorObject(frog[pl], model);
			AnimateActor(frog[pl]->actor, FROG_ANIM_BREATHE, YES, NO, 0.35f, NO, NO);
			break;
		}

	case EV_SHAKECAMERA:
		{
			cam_shakiness = MEMGETFLOAT(p);
			cam_shake_falloff = MEMGETFLOAT(p);
			break;
		}

	case EV_SETTILE_P:
		{
			int id = MEMGETWORD(p), state = MEMGETBYTE(p);
			EnumPlaceholderTiles(id, SetTile, state);
			break;
		}

	case EV_SETSTARTTILE_P:
		{
			ENEMY *e;
			if (!(e = GetEnemyFromUID(MEMGETWORD(p)))) return 0;
			gTStart[0] = e->path->nodes->worldTile;
			startCamFacing = camFacing[0];
			startFrogFacing = frogFacing[0];
			break;
		}

	case EV_TELEPORT_P:
		{
			TRIGGER *t;
			EVENT *e;
			int fNum, time;
			GAMETILE *tile; 
			VECTOR telePos;
			void **param;

			fNum = MEMGETBYTE(p);
			tile = GetEnemyFromUID(MEMGETWORD(p))->path->nodes->worldTile;
			time = (MEMGETFLOAT(p) * 60) + actFrameCount;

			if( player[fNum].frogState & FROGSTATUS_ISDEAD ) break;

			player[fNum].frogState = FROGSTATUS_ISTELEPORTING;	// clear ALL other flags
			player[fNum].canJump = 0;
			FrogLeavePlatform(fNum);	// bah

			CreateTeleportEffect( &frog[fNum]->actor->pos, &currTile[fNum]->normal, 255, 255, 255 );

			param = AllocArgs(1);
			param[0] = (void *)time;
			t = MakeTrigger( OnTimeout, param );

			param = AllocArgs(2);
			param[0] = (void *)fNum;
			param[1] = (void *)tile;
			e = MakeEvent( TeleportFrog, param );

			AttachEvent( t, e, TRIGGER_ONCE, 0 );

			break;
		}

	case EV_SPRING_P:
		{
			GAMETILE *tile;	int pl;	float height, time;

			pl = MEMGETBYTE(p);
			tile = GetEnemyFromUID(MEMGETWORD(p))->path->nodes->worldTile;
			height = MEMGETFLOAT(p);
			time = MEMGETFLOAT(p);

			SpringFrogToTile(tile, height, time, pl);

			break;
		}

	case EV_PLAYSOUND:
		{
			char name[32];
			short length;
			ENEMY *nme;

			length = MEMGETBYTE(p);
			if( length > 31 )
			{
				dprintf"Sample name too long! Max 32 characters\n"));
				return 0;
			}
			memcpy( name, *p, length );
			name[length] = '\0';
			(*p) += length;

			nme = GetEnemyFromUID(MEMGETWORD(p));
			if( !nme ) return 0;

			PlaySample( FindSample(UpdateCRC(name)), &nme->path->nodes->worldTile->centre, 0, SAMPLE_VOLUME, -1 );
			break;
		}

/*
	case EV_HURTFROG:
		{
			int player, anim_hurt, anim_die;
			
			player = MEMGETBYTE(p);
			anim_hurt = MEMGETBYTE(p);
			anim_die = MEMGETBYTE(p);

			if ((--player[player].healthPoints) == 0)
				AnimateActor(frog[player], anim_hurt, NO, NO, 
		}
*/

#ifdef DEBUG_SCRIPTING
	case C_DEBUG:
		MEMGETWORD(p);	// blank
		lineNumber = MEMGETWORD(p);
		break;
#endif

	default:
#ifdef DEBUG_SCRIPTING
		PrintScriptDebugMessage("Unrecognised command");
#endif
		return 0;
	}

	return 1;
}			

/*	--------------------------------------------------------------------------------
    Function		: Interpret
	Parameters		: UBYTE*
	Returns			: TRUE if success, FALSE otherwise
*/
int Interpret(const UBYTE *buffer)
{
	UBYTE *p, *q;
	int events;

	p = (UBYTE*)buffer;

	events = MEMGETWORD(&p);

	while (events--)
	{
		if (!ExecuteCommand(&p))
		{
#ifdef DEBUG_SCRIPTING
			if (lineNumber)
				dprintf"Possibly an error on line %d?\n", lineNumber));
#endif
			return 0;
		}
	}

	return 1;
}


/*	--------------------------------------------------------------------------------
    Function		: LoadTestScript
	Parameters		: const char*
	Returns			: 
*/
void LoadTestScript(const char* filename)
{
	UBYTE* buffer;
	
	dprintf"Testing script %s\n", filename));

	if (!(buffer = (UBYTE*)fileLoad(filename, 0)))
	{
		sprintf(statusMessage, "Couldn't load script file %s", filename);
		dprintf "%s\n", statusMessage)); return;
	}

	if (!InitLevelScript(buffer))
	{
		sprintf(statusMessage, "Failed initialising script %s", filename);
		dprintf "%s\n", statusMessage)); return;
	}

	sprintf(statusMessage, "Loaded script %s", filename);
}

int InitLevelScript(void *buffer)
{
	int err = 0;

	KillAllTriggers();
	FreeLevelScript();

	scriptBuffer = buffer;
	
	if (*scriptBuffer != SCRIPT_FEV_VERSION)
	{
#ifdef DEBUG_SCRIPTING
		dprintf"Script failed version check\n"));
#endif
		err = 1;
	}
	else if (!Interpret(scriptBuffer + 1 + 4))	// block starts after header + outer block size
		err = 1;

	if (err)
	{
		KillAllTriggers();
		JallocFree((UBYTE**)&scriptBuffer);
		scriptBuffer = NULL;
		return 0;
	}

	return 1;
}

int FreeLevelScript(void)
{
	if(scriptBuffer)
	{
		JallocFree((UBYTE**)&scriptBuffer);
		scriptBuffer = NULL;
	}

	return 1;
}
