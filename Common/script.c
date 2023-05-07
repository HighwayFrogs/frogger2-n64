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

#define DEBUG_SCRIPTING
#define SCRIPT_FEV_VERSION	3

/* --------------------------------------------------------------------------------- */

#define FS_SET_MOVE			1
#define FS_SET_STOP			2
#define FS_SET_TOGGLEMOVE	3
#define FS_SET_INVIS		4
#define FS_SET_VIS			5
#define FS_SET_TOGGLEVIS	6

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

/* --------------------------------------------------------------------------------- */

TRIGGER *LoadTrigger(UBYTE**);
BOOL ExecuteCommand(UBYTE**);

UBYTE* scriptBuffer;

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

void PrintScriptDebugMessage(const char* str)
{
	dprintf"[Interpreter Debug] %s", str));

	if(lineNumber) dprintf" (line %d)", lineNumber));

	dprintf"\n"));
}
/* --------------------------------------------------------------------------------- */

ACTOR2 *GetActorFromUID(int UID)
{
	ENEMY *e;
	PLATFORM *p;

	dprintf"Finding actor %d: ", UID));
	e = GetEnemyFromUID(UID);
	if (e)
	{
		dprintf"Found\n"));
		return e->nmeActor;
	}
	
	dprintf"Searching plats: "));
	p = GetPlatformFromUID(UID);
	if (p)
	{
		dprintf"Found\n"));
		return p->pltActor;
	}

	return NULL;
}


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
			params[0] = (void*)frog[MEMGETBYTE(p)];
			if (!(platform = GetPlatformFromUID(MEMGETWORD(p)))) return 0;
			params[1] = (void*)platform;
			trigger = MakeTrigger(FrogOnPlatform, params);
		}
		break;

	case TR_PLATNEARPOINT:
		{
			PLATFORM *platform; VECTOR *v;

			params = AllocArgs(3);
			if (!(platform = GetPlatformFromUID(MEMGETWORD(p)))) return 0;
			params[0] = platform->pltActor->actor;
			v = (VECTOR*)JallocAlloc(sizeof(VECTOR), NO, "vect");
			v->v[X] = MEMGETFLOAT(p);
			v->v[Y] = MEMGETFLOAT(p);
			v->v[Z] = MEMGETFLOAT(p);
			params[1] = (void*)v;
			params[2] = JallocAlloc(sizeof(float), NO, "float"); *(float*)params[1] = MEMGETFLOAT(p);
			trigger = MakeTrigger(ActorWithinRadius, params);
		}
		break;

	case TR_ENEMYNEARPOINT:
		{
			ENEMY *enemy; VECTOR *v;

			params = AllocArgs(3);
			if (!(enemy = GetEnemyFromUID(MEMGETWORD(p)))) return 0;
			params[0] = enemy->nmeActor->actor;
			v = (VECTOR*)JallocAlloc(sizeof(VECTOR), NO, "vect");
			v->v[X] = MEMGETFLOAT(p);
			v->v[Y] = MEMGETFLOAT(p);
			v->v[Z] = MEMGETFLOAT(p);
			params[1] = (void*)v;
			params[2] = JallocAlloc(sizeof(float), NO, "float"); *(float*)params[1] = MEMGETFLOAT(p);
			trigger = MakeTrigger(ActorWithinRadius, params);
		}
		break;

	case TR_ENEMYATFLAG:
		{
			ENEMY *enemy;

			params = AllocArgs(2);
			if (!(enemy = GetEnemyFromUID(MEMGETWORD(p)))) return 0;
			params[0] = enemy->path;
			params[1] = JallocAlloc(sizeof(int), NO, "int"); *(int*)params[1] = MEMGETWORD(p);
			trigger = MakeTrigger(PathAtFlag, params);
		}
		break;

	case TR_PLATATFLAG:
		{
			PLATFORM *plt;

			params = AllocArgs(2);
			if (!(plt = GetPlatformFromUID(MEMGETWORD(p)))) return 0;
			params[0] = plt->path;
			params[1] = JallocAlloc(sizeof(int), NO, "int"); *(int*)params[1] = MEMGETWORD(p);
			trigger = MakeTrigger(PathAtFlag, params);
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

	default:
#ifdef DEBUG_SCRIPTING
		dprintf"Unrecognised trigger type %02x, skipping\n", token));
#endif
		return NULL;
	}

	return trigger;
}

/*	--------------------------------------------------------------------------------
    Function		: SetEnemy
	Parameters		: ENEMY*, int
	Returns			: 
*/
void SetEnemy(ENEMY *nme, int v)
{
	switch (v)
	{
Move:
	case FS_SET_MOVE:
		if (nme->isWaiting)
		{
			nme->isWaiting = 0;
			nme->path->startFrame = actFrameCount;
			nme->path->endFrame = nme->path->startFrame + (int)(60.0*nme->speed);
		}
		break;
Stop:			
	case FS_SET_STOP:
		nme->isWaiting = -1; break;
	
	case FS_SET_TOGGLEMOVE:
		if (nme->isWaiting) goto Move; else goto Stop;
Invis:
	case FS_SET_INVIS:
		nme->active = 0;
		nme->nmeActor->draw = 0;
		break;
Vis:
	case FS_SET_VIS:
		nme->active = 1;
		nme->nmeActor->draw = 1;
		break;

	case FS_SET_TOGGLEVIS:
		if (nme->active) goto Invis; else goto Vis;
	}
}

/*	--------------------------------------------------------------------------------
    Function		: SetPlatform
	Parameters		: PLATFORM*, int
	Returns			: 
*/
void SetPlatform(PLATFORM *plt, int v)
{
	switch (v)
	{
Move:
	case FS_SET_MOVE:
		if (plt->isWaiting)
		{
			plt->isWaiting = 0;
			plt->path->startFrame = actFrameCount;
			plt->path->endFrame = plt->path->startFrame + (int)(60.0*plt->currSpeed);
		}
		break;
Stop:			
	case FS_SET_STOP:
		plt->isWaiting = -1; break;
	
	case FS_SET_TOGGLEMOVE:
		if (plt->isWaiting) goto Move; else goto Stop;
Invis:
	case FS_SET_INVIS:
		plt->active = 0;
		plt->pltActor->draw = 0;
		break;
Vis:
	case FS_SET_VIS:
		plt->active = 1;
		plt->pltActor->draw = 1;
		break;

	case FS_SET_TOGGLEVIS:
		if (plt->active) goto Invis; else goto Vis;
	}
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

	case EV_SETENEMYFLAG:
		{
			ENEMY *nme;
			int flag;
			if (!(nme = GetEnemyFromUID(MEMGETWORD(p)))) return 0;
			flag = MEMGETINT(p);
			nme->flags |= flag;

			break;
		}
		
	case EV_RESETENEMYFLAG:
		{
			ENEMY *nme;
			int flag;
			if (!(nme = GetEnemyFromUID(MEMGETWORD(p)))) return 0;
			flag = MEMGETINT(p);
			nme->flags &= ~flag;

			break;
		}


	case EV_SETPLATFLAG:
		{
			PLATFORM *plt;
			int flag;
			if (!(plt = GetPlatformFromUID(MEMGETWORD(p)))) return 0;
			flag = MEMGETINT(p);
			plt->flags |= flag;
			RecalculatePlatform(plt);
			break;
		}

	case EV_RESETPLATFLAG:
		{
			PLATFORM *plt;
			int flag;
			if (!(plt = GetPlatformFromUID(MEMGETWORD(p)))) return 0;
			flag = MEMGETINT(p);
			plt->flags &= ~flag;
			RecalculatePlatform(plt);
			break;
		}
		
	case EV_ANIMATEACTOR:
		{
			ACTOR2 *actor;
			char anim, flags;
			float speed;

			actor = GetActorFromUID(MEMGETWORD(p));
			if (!actor) return 0;

			anim = MEMGETBYTE(p);
			flags = MEMGETBYTE(p);
			speed = MEMGETFLOAT(p);

			AnimateActor(actor->actor,anim,(char)(flags & 1),(char)(flags & 2),speed,0,0);
			break;
		}

	case EV_SETENEMY:
		{
			ENEMY *nme;
			nme = GetEnemyFromUID(MEMGETWORD(p));
			if (!nme) return 0;

			SetEnemy(nme, MEMGETBYTE(p));
			break;
		}

	case EV_SETPLATFORM:
		{
			PLATFORM *plt;
			plt = GetPlatformFromUID(MEMGETWORD(p));
			if (!plt) return 0;

			SetPlatform(plt, MEMGETBYTE(p));
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

			player[fNum].frogState = FROGSTATUS_ISTELEPORTING;	// clear ALL other flags
			player[fNum].canJump = 0;
			FrogLeavePlatform(fNum);	// bah

			CreateTeleportEffect( &frog[fNum]->actor->pos, &upVec, 255, 255, 255 );

			param = AllocArgs(1);
			param[0] = (void *)time;
			t = MakeTrigger( OnTimeout, param );

			param = AllocArgs(2);
			param[0] = (void *)fNum;
			param[1] = (void *)tNum;
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
			ENEMY *nme;
			int flag;

			nme = GetEnemyFromUID(MEMGETWORD(p));
			if (!nme) return 0;

			flag = MEMGETWORD(p);
			if (flag > 0 && flag < nme->path->numNodes)
			{
				// Cunning cheat (that probably flies apart)6
				nme->path->toNode = flag;
				nme->path->endFrame = actFrameCount;
				UpdateEnemyPathNodes(nme);
			}
			break;
		}

	case EV_MOVEPLAT:
		{
			PLATFORM *plt;
			int flag;

			plt = GetPlatformFromUID(MEMGETWORD(p));
			if (!plt) return 0;

			flag = MEMGETWORD(p);
			if (flag > 0 && flag < plt->path->numNodes)
			{
				plt->path->fromNode = flag;
				plt->inTile[0] = plt->path->nodes[flag].worldTile;
				plt->isWaiting = 1;
				RecalculatePlatform(plt);
			}
			break;
		}

	case EV_DROPGARIB:
		{
			GARIB *garib;
			GAMETILE *tile;
			int g, t;

			g = MEMGETWORD(p);
			t = MEMGETWORD(p);

			if (g < 0 || g > garibCollectableList.numEntries || t < 0) return 0;

			for (garib = garibCollectableList.head.next; g; garib = garib->next, g--);
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

			GTInit( &levelIsOver, 1 );
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
			int type;
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

	case EV_SFX_PLATFORM:
	case EV_SFX_ENEMY:
		(*p) += 2 + 1 + 4*4;
		PrintScriptDebugMessage("Enemy/Platform sfx not quite available");
		break;

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
/*
	case EV_HURTFROG:
		{
			int player, anim_hurt, anim_die;
			
			player = MEMGETBYTE(p);
			anim_hurt = MEMGETBYTE(p);
			anim_die = MEMGETBYTE(p);

			if ((--frog[player]->action.healthPoints) == 0)
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
#ifdef PC_VERSION
void LoadTestScript(const char* filename)
{
	HANDLE h;
	long size, read;
	UBYTE* buffer;
	
	dprintf"Testing script %s\n", filename));

	h = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);

	if (h == INVALID_HANDLE_VALUE)
	{
		sprintf(statusMessage, "Couldn't load script file %s", filename);
		dprintf "%s\n", statusMessage)); return;
	}

	size = GetFileSize(h, NULL);
	buffer = JallocAlloc(size, NO, "entLoad");

	ReadFile(h, buffer, size, &read, NULL);
	CloseHandle(h);

	if (!InitLevelScript(buffer))
	{
		sprintf(statusMessage, "Failed initialising script %s", filename);
		dprintf "%s\n", statusMessage)); return;
	}

	sprintf(statusMessage, "Loaded script %s", filename);
}
#endif

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
