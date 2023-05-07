
#include <ultra64.h>
#include "incs.h"
#include <stdio.h>
#include "memload.h"
#include "eventfuncs.h"
#include "script.h"

#define MEMLOAD_PROTOTYPE	// uncomment to use old file format temporarily

/*	-------------------------------------------------------------------------------- */

#ifdef MEMLOAD_PROTOTYPE
#define MEMLOAD_ENTITY_VERSION 13
#else
#define MEMLOAD_ENTITY_VERSION 12
#endif

#define MEMLOAD_SCRIPT_VERSION 2

typedef enum { CREATE_ENEMY, CREATE_PLATFORM, CREATE_GARIB, CREATE_CAMERACASE, CREATE_PLACEHOLDER } CREATETYPE;

/*	-------------------------------------------------------------------------------- */

#define MEMGETBYTE(p) (*((*p)++))

int MemLoadInt(UBYTE **p)		// get a little-endian integer
{
	unsigned int i;

	i = (unsigned int)*((*p)++);
	i += ((unsigned int)*((*p)++) << 8);
	i += ((unsigned int)*((*p)++) << 16);
	i += ((unsigned int)*((*p)++) << 24);

	return i;
}

short MemLoadWord(UBYTE **p)		// get a little-endian word
{
	unsigned short i;

	i = (unsigned int)*((*p)++);
	i += ((unsigned int)*((*p)++) << 8);

	return (short)i;
}

char *MemLoadString(UBYTE **p)
{
	char *ptr;
	int size;
	
	size = MEMGETBYTE(p);
	ptr = JallocAlloc(size + 1, NO, "str");
	memcpy(ptr, *p, size);
	ptr[size] = 0;

	(*p) += size;

	return ptr;
}

/*	--------------------------------------------------------------------------------
    Function		: MemLoadEntities
	Parameters		: void*, long
	Returns			: int

	Load enemies, platforms etc.
*/

#ifdef MEMLOAD_PROTOTYPE

int MemLoadEntities(const void* data, long size)
{
	UBYTE thing;
	UBYTE *p = (UBYTE*)data;
	PATH *pathList;
	int n, count;

	// Version check - only load files with the current version
	n = MEMGETBYTE(&p);
	
	if (n != MEMLOAD_ENTITY_VERSION ) {
		dprintf"ERROR: Attempting to load incorrect version (%03d) of level file (should be %03d)\n", n, MEMLOAD_ENTITY_VERSION));
		return 0;
	}

	dprintf"Testing prototype memload functions ---------------------\n"));

	count = MEMGETWORD(&p);

	dprintf"Loading %d paths..\n", count));

	pathList = (PATH*)JallocAlloc(count*sizeof(PATH), YES, "ml_paths");

	for (n=0; n<count; n++)
	{
		PATH *path;
		PATHNODE *node;
		VECTOR v;
		int numNodes;

		numNodes = MEMGETINT(&p);

		path = &pathList[n];
		node = (PATHNODE *)JallocAlloc(sizeof(PATHNODE) * numNodes,YES,"enode");
		path->nodes = node;
		path->numNodes = numNodes;

		while (numNodes--)
		{
			v.v[X] = MEMGETFLOAT(&p);
			v.v[Y] = MEMGETFLOAT(&p);
			v.v[Z] = MEMGETFLOAT(&p);
			node->worldTile = FindNearestTile(v);
			node->offset = MEMGETFLOAT(&p);
			node->offset2 = MEMGETFLOAT(&p);
			node->speed = MEMGETFLOAT(&p);
			node->waitTime = MEMGETINT(&p);
			
			node++;
		}

		AddPathNodesToList(path->nodes);
	}

	count = MEMGETWORD(&p);

#ifdef DEBUG_MEMLOAD
	dprintf"MemLoadEntities: %d items\n", count));
#endif

	while (count--)
	{
		thing = MEMGETBYTE(&p);

		switch (thing)
		{
		case CREATE_ENEMY:
		case CREATE_PLATFORM:
			{
				char type[20];
				int count, flags, numNodes, startNode, n, ID, effects, pathIndex;
				float scale, radius, animSpeed, value1;
				PATH *path;
				ENEMY *enemy;
				PLATFORM *platform;
				ACTOR2 *act;

				n = MEMGETBYTE(&p);
				memcpy(type, p, n); type[n] = 0; p+= n;
				flags	= MEMGETINT(&p);
				ID		= MEMGETINT(&p);
				scale	= MEMGETFLOAT(&p);
				radius	= MEMGETFLOAT(&p);
				animSpeed = MEMGETFLOAT(&p);
				value1 = MEMGETFLOAT(&p);
				effects = MEMGETINT(&p);
				startNode = MEMGETINT(&p);

				pathIndex = MEMGETWORD(&p);
				path = (PATH*)JallocAlloc(sizeof(PATH), 0, "path");
				memcpy(path, &pathList[pathIndex], sizeof(PATH));

				path->startNode = startNode;

				switch (thing)
				{
				case CREATE_ENEMY:
					enemy = CreateAndAddEnemy(type,flags,ID,path,animSpeed);
					act = enemy->nmeActor;
					break;

				case CREATE_PLACEHOLDER:
					enemy = (ENEMY *)JallocAlloc(sizeof(ENEMY), YES, "place");
					enemy->path = path;
					enemy->active = 0;
					enemy->uid = ID;
					act = NULL;
					AddEnemy(enemy);
					break;

				case CREATE_PLATFORM:
					platform = CreateAndAddPlatform(type,flags,ID,path,animSpeed);
					act = platform->pltActor;

#ifdef PLATFORM_NEW_SHAKABLESCENIC
					// check for platforms that are 'shakable' scenics - TESTING - ANDYE
					if(	gstrcmp(type,"appltree.obe") == 0 || gstrcmp(type,"appltree.ndo") == 0 ||
						gstrcmp(type,"barrel.obe") == 0 || gstrcmp(type,"barrel.ndo") == 0 ||
						gstrcmp(type,"pltbarel.obe") == 0 || gstrcmp(type,"pltbarel.ndo") == 0 ||
						gstrcmp(type,"bucket.obe") == 0 || gstrcmp(type,"bucket.ndo") == 0)
					{
						// make this 'platform' shakable
						SetVector(&platform->pltActor->actor->oldpos,&platform->pltActor->actor->pos);
						platform->flags |= PLATFORM_NEW_SHAKABLESCENIC;
					}
#endif
					break;
				}

				if(gstrcmp(type,"nothing.obe") == 0)
				{
					// null object - do not display
					act->flags = ACTOR_DRAW_NEVER;
				}

				if (act)
				{
					act->radius = radius;
					act->actor->scale.v[X] = scale;
					act->actor->scale.v[Y] = scale;
					act->actor->scale.v[Z] = scale;
					act->animSpeed = animSpeed;
					act->value1 = value1;
					act->effects = effects;
				}

				break;
			}

		case CREATE_GARIB:
			{
				VECTOR v, w;
				GAMETILE *tile;
				float offs;

				n = MEMGETBYTE(&p);
				v.v[X] = MEMGETFLOAT(&p);
				v.v[Y] = MEMGETFLOAT(&p);
				v.v[Z] = MEMGETFLOAT(&p);
				offs = MEMGETFLOAT(&p);

				tile = FindNearestTile(v);

				SetVector(&w, &tile->normal);
				ScaleVector(&w, offs);
				AddVector(&v, &tile->centre, &w);

				CreateNewGarib(v, n);
				break;
			}

		case CREATE_CAMERACASE:
			{
				TRANSCAMERA *tcam;
				VECTOR v, w;
				int flags, numNodes, speed, target;
				float FOV;
				
				flags = MEMGETINT(&p);
				v.v[X] = MEMGETFLOAT(&p);
				v.v[Y] = MEMGETFLOAT(&p);
				v.v[Z] = MEMGETFLOAT(&p);
				
				FOV = MEMGETFLOAT(&p);
				speed = MEMGETFLOAT(&p);
				target = MEMGETINT(&p);

				numNodes = MEMGETINT(&p);
				while (numNodes--)
				{
					w.v[X] = MEMGETFLOAT(&p);
					w.v[Y] = MEMGETFLOAT(&p);
					w.v[Z] = MEMGETFLOAT(&p);
					tcam = CreateAndAddTransCamera(FindNearestTile(w), flags >> 16, &v, flags & 0xFFFF);
					tcam->FOV = FOV;
					tcam->speed = speed;
					tcam->camLookAt.v[X] = (float)((char)(target & 0xFF))/0x7F;
					tcam->camLookAt.v[Y] = (float)((char)((target>>8) & 0xFF))/0x7F;
					tcam->camLookAt.v[Z] = (float)((char)((target>>16) & 0xFF))/0x7F;
				}
				break;
			}
		}
	 }

	JallocFree((UBYTE**)&pathList);

	dprintf"Success!\n----------------------------------------------------\n"));

	return 1;
}

#else

int MemLoadEntities(const void* data, long size)
{
	int count, flags, numNodes, startNode, n, ID, effects;
	float scale, radius, animSpeed, value1;
	UBYTE thing;
	char type[20];
	PATH *path;
	PATHNODE *node;
	VECTOR v,w;
	ENEMY *enemy;
	PLATFORM *platform;
	UBYTE *p = (UBYTE*)data;
	ACTOR2 *act;
	TRANSCAMERA *tcam;

	// Version check - only load files with the current version
	n = MEMGETBYTE(&p);
	
	if (n != MEMLOAD_ENTITY_VERSION ) {
		dprintf"ERROR: Attempting to load incorrect version (%03d) of level file (should be %03d)\n", n, MEMLOAD_ENTITY_VERSION));
		return 0;
	}

	count = MEMGETINT(&p);

#ifdef DEBUG_MEMLOAD
	dprintf"MemLoadEntities: %d items\n", count));
#endif

	while (count--)
	{
		thing = MEMGETBYTE(&p);

		switch (thing)
		{
		case CREATE_ENEMY:
		case CREATE_PLATFORM:
			n = MEMGETBYTE(&p);
			memcpy(type, p, n); type[n] = 0; p+= n;
			flags	= MEMGETINT(&p);
			ID		= MEMGETINT(&p);
			scale	= MEMGETFLOAT(&p);
			radius	= MEMGETFLOAT(&p);
			animSpeed = MEMGETFLOAT(&p);
			value1 = MEMGETFLOAT(&p);
			effects = MEMGETINT(&p);
			startNode = MEMGETINT(&p);
		
			numNodes = MEMGETINT(&p);

#ifdef DEBUG_MEMLOAD
			dprintf"'%s' %08x with %d path nodes\n", type, flags, numNodes));
#endif

			path = (PATH *)JallocAlloc(sizeof(PATH), YES, "epath");
			node = (PATHNODE *)JallocAlloc(sizeof(PATHNODE) * numNodes,YES,"enode");
			path->nodes = node;
			path->startNode = startNode;
			path->numNodes = numNodes;

			while (numNodes--)
			{
				v.v[X] = MEMGETFLOAT(&p);
				v.v[Y] = MEMGETFLOAT(&p);
				v.v[Z] = MEMGETFLOAT(&p);
				node->worldTile = FindNearestTile(v);
				node->offset = MEMGETFLOAT(&p);
				node->offset2 = MEMGETFLOAT(&p);
				node->speed = MEMGETFLOAT(&p);
				node->waitTime = MEMGETINT(&p);
				
				node++;
			}

			switch (thing)
			{
			case CREATE_ENEMY:
				enemy = CreateAndAddEnemy(type,flags,ID,path,animSpeed);
				act = enemy->nmeActor;
				break;

			case CREATE_PLATFORM:
				platform = CreateAndAddPlatform(type,flags,ID,path,animSpeed);
				act = platform->pltActor;

#ifdef PLATFORM_NEW_SHAKABLESCENIC
				// check for platforms that are 'shakable' scenics - TESTING - ANDYE
				if(	gstrcmp(type,"appltree.obe") == 0 || gstrcmp(type,"appltree.ndo") == 0 ||
					gstrcmp(type,"barrel.obe") == 0 || gstrcmp(type,"barrel.ndo") == 0 ||
					gstrcmp(type,"pltbarel.obe") == 0 || gstrcmp(type,"pltbarel.ndo") == 0 ||
					gstrcmp(type,"bucket.obe") == 0 || gstrcmp(type,"bucket.ndo") == 0)
				{
					// make this 'platform' shakable
					SetVector(&platform->pltActor->actor->oldpos,&platform->pltActor->actor->pos);
					platform->flags |= PLATFORM_NEW_SHAKABLESCENIC;
				}
#endif
				break;
			}

			if(gstrcmp(type,"nothing.obe") == 0)
			{
				// null object - do not display
				act->flags = ACTOR_DRAW_NEVER;
			}

			act->radius = radius;
			act->actor->scale.v[X] = scale;
			act->actor->scale.v[Y] = scale;
			act->actor->scale.v[Z] = scale;
			act->animSpeed = animSpeed;
			act->value1 = value1;
			act->effects = effects;

			break;

		case CREATE_GARIB:
			n = MEMGETBYTE(&p);
			v.v[X] = MEMGETFLOAT(&p);
			v.v[Y] = MEMGETFLOAT(&p);
			v.v[Z] = MEMGETFLOAT(&p);
//			CreateNewGarib(v, n, &firstTile[1], 0.0f);
			CreateNewGarib(v, n);
			break;

		case CREATE_CAMERACASE:
			int target;
			flags = MEMGETINT(&p);
			v.v[X] = MEMGETFLOAT(&p);
			v.v[Y] = MEMGETFLOAT(&p);
			v.v[Z] = MEMGETFLOAT(&p);
			
			scale = MEMGETFLOAT(&p);
			animSpeed = MEMGETFLOAT(&p);
			target= MEMGETINT(&p);

			numNodes = MEMGETINT(&p);
			while (numNodes--)
			{
				w.v[X] = MEMGETFLOAT(&p);
				w.v[Y] = MEMGETFLOAT(&p);
				w.v[Z] = MEMGETFLOAT(&p);
				tcam = CreateAndAddTransCamera(FindNearestTile(w), flags >> 16, &v, flags & 0xFFFF);
				tcam->FOV = scale;
				tcam->speed = animSpeed;
				tcam->camLookAt.v[X] = (float)((char)(target & 0xFF))/0x7F;
				tcam->camLookAt.v[Y] = (float)((char)((target>>8) & 0xFF))/0x7F;
				tcam->camLookAt.v[Z] = (float)((char)((target>>16) & 0xFF))/0x7F;
			}
			break;
		}
	 }

	return 1;
}

#endif	// MEMLOAD_PROTOTYPE

/*	--------------------------------------------------------------------------------
    Function		: MemLoadEvents
	Parameters		: void*, long
	Returns			: int

	Load events and triggers from an ETC output file
*/

int MemLoadEvents(const void* data, long size)
{
	int ver, s;
	UBYTE *p = (UBYTE*)data;

	if (*p != MEMLOAD_SCRIPT_VERSION) {
		ver = *p;
		dprintf"ERROR: Attempting to load incorrect version (%03d) of script (should be %03d)\n",
			ver, MEMLOAD_SCRIPT_VERSION));
		return 0;
	}

	return (Interpret(p + 1));
}

