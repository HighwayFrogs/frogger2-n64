/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: platform.h
	Programmer	: Andrew Eder
	Date		: 1/21/99

----------------------------------------------------------------------------------------------- */

#ifndef PLATFORM_H_INCLUDED
#define PLATFORM_H_INCLUDED

#define NODE_RANGETHRESHOLD				10.0F

//----- [ PLATFORM FLAGS ] ---------------------------------------------------------------------//

#define PLATFORM_NEW_NONE				0
#define PLATFORM_NEW_FOLLOWPATH			(1 << 0)	// platform follows a path (>1 node)
#define PLATFORM_NEW_FORWARDS			(1 << 1)	// platform moves forwards thru path nodes
#define PLATFORM_NEW_BACKWARDS			(1 << 2)	// platform moves backwards thru path nodes
#define PLATFORM_NEW_PINGPONG			(1 << 3)	// platform moves back and forth thru nodes
#define PLATFORM_NEW_CYCLE				(1 << 4)	// platform cycles thru nodes continuously
#define PLATFORM_NEW_MOVEUP				(1 << 5)	// platform moves up (1 node paths only)
#define PLATFORM_NEW_MOVEDOWN			(1 << 6)	// platform moves down (1 node paths only)
#define PLATFORM_NEW_STEPONACTIVATED	(1 << 7)	// platform is activated when frog is on it
#define PLATFORM_NEW_SINKWITHFROG		(1 << 8)	// platform sinks when frog is on it
#define PLATFORM_NEW_RISEWITHFROG		(1 << 9)	// platform sinks when frog is on it
#define PLATFORM_NEW_CARRYINGFROG		(1 << 10)	// platform is carrying a frog
#define PLATFORM_NEW_DISAPPEARWITHFROG	(1 << 11)	// platform disappears when frog is on it (after period of time)
#define PLATFORM_NEW_CRUMBLES			(1 << 12)	// platform crumbles when frog is on it (after period of time)
#define PLATFORM_NEW_NONMOVING			(1 << 13)	// platform does not move
#define PLATFORM_NEW_REGENERATES		(1 << 14)	// platform regenerates after specified time
#define PLATFORM_NEW_NOWALKUNDER		(1 << 15)	// platform cannot be walked under
#define PLATFORM_NEW_KILLSFROG			(1 << 16)	// platform kills when it moves over frog
#define PLATFORM_NEW_FACEFORWARDS		(1 << 17)	// face same direction
#define PLATFORM_NEW_SHAKABLESCENIC		(1 << 18)	// platform is actually a shakable scenic
#define PLATFORM_NEW_SHADOW				(1 << 19)	// Has a shadow
#define PLATFORM_NEW_SLERPPATH			(1 << 20)	// Use slerp-based algorithm to smoothly follow the path

typedef struct TAGPLATFORM
{
	struct TAGPLATFORM		*next,*prev;			// ptr to next / prev platform
	struct TAGPLATFORM		*pltPtrs[4];			// ptrs to adjacent platforms (if any)

	long					uid;					// Unique ID

	VECTOR					dirVector[4];			// 4 possible direction vectors
	VECTOR					initDirVector[4];		// initial 4 direction vectors
		
	ACTOR2					*pltActor;				// actor used as platform

	UBYTE					active;					// 1 = active ; 0 = non-active

	unsigned long			flags;					// platform flags

	float					currSpeed;				// platform current speed
	short					isWaiting;				// platform node pause time

	QUATERNION				srcOrientation;			// orientations to turn between while following a path?
	QUATERNION				destOrientation;		
													
/*
	VECTOR					currNormal;				// platform current normal
	VECTOR					deltaNormal;			// platform delta normal (for linear interp)
*/
	float					countdown;				// countdown for disappear/regenerate
	unsigned char			facing;					// For face forward platforms, which tilevector it is aligned to

	GAMETILE				*inTile[2];				// tile(s) platform is(are) currently 'in'
	PATH					*path;					// ptr to platform path data
	ACTOR2					*carrying;				// current actor platform is carrying

	void					(*Update)();			// ptr to platform update function

} PLATFORM;


typedef struct TAGPLATFORMLIST
{
	int				numEntries;
	PLATFORM		head;

} PLATFORMLIST;


extern PLATFORMLIST platformList;


extern PLATFORM *destPlatform[MAX_FROGS];		// platform that frog is about to attempt to jump to
extern PLATFORM *currPlatform[MAX_FROGS];		// platform that frog is currently on
extern PLATFORM *nearestPlatform[MAX_FROGS];	// Nearest platforms to frogs ...
extern float     nearestPlatDist[MAX_FROGS];	// .. and their distance from the frogs (^2)

extern float waitScale;

void InitPlatformsForLevel(unsigned long worldID, unsigned long levelID);

void InitPlatformLinkedList();
void FreePlatformLinkedList();
void AddPlatform(PLATFORM *plat);
void SubPlatform(PLATFORM *plat);

void UpdatePlatforms();
void RecalculatePlatform(PLATFORM *);
void GetNextLocalPlatform(unsigned long direction);
PLATFORM *GetPlatformForTile(GAMETILE *tile);

BOOL PlatformTooHigh(PLATFORM *plat,long pl);
BOOL PlatformTooLow(PLATFORM *plat,long pl);

PLATFORM *GetPlatformFrogIsOn(long pl);

void FrogLeavePlatform(long pl);


//------------------------------------------------------------------------------------------------

PLATFORM *CreateAndAddPlatform(char *pActorName,int flags,long ID,PATH *path,float animSpeed,unsigned char facing);
void AssignPathToPlatform(PLATFORM *pform,PATH *path,unsigned long pathFlags);

BOOL PlatformHasArrivedAtNode(PLATFORM *pform);
BOOL PlatformReachedTopOrBottomPoint(PLATFORM *pform);
void UpdatePlatformPathNodes(PLATFORM *pform);
void CalcPlatformNormalInterps(PLATFORM *pform);

void SetPlatformVisibleTime(PLATFORM *pform,short time);
void SetPlatformRegenerateTime(PLATFORM *pform,short time);

PLATFORM *GetNearestPlatformBelowFrog(GAMETILE *tile,long pl);
PLATFORM *GetPlatformFromUID(long uid);

PLATFORM *CheckDestForPlatform(GAMETILE *dest,long pl);

int EnumPlatforms(long id, int (*func)(PLATFORM*, int), int param);
int MovePlatformToNode(PLATFORM *, int node);
void SetPlatformVisible(PLATFORM *plt, int visible);
void SetPlatformMoving(PLATFORM *plt, int moving);

//------------------------------------------------------------------------------------------------


#endif
