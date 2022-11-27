/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: enemies.h
	Programmer	: Andrew Eder
	Date		: 1/12/99

----------------------------------------------------------------------------------------------- */

#ifndef ENEMIES_H_INCLUDED
#define ENEMIES_H_INCLUDED


//----- [ ANIMATION BITS ] ---------------------------------------------------------------------//

#define NMEANIM_HOMER_WALK		0
#define NMEANIM_HOMER_IDLE		1
#define NMEANIM_HOMER_ATTACK	2

#define NMEANIM_PATH_WALK1		0
#define NMEANIM_PATH_WALK2		1

#define NMEANIM_SNAP_IDLE		0
#define NMEANIM_SNAP_ATTACK		1
#define NMEANIM_SNAP_EXTRA1		2
#define NMEANIM_SNAP_EXTRA2		3

#define NUM_NME_ANIMS			4


typedef void (*DEATHANIM_FUNC) (int);


//----- [ ENEMY FLAGS ] ---------------------------------------------------------------------//

#define ENEMY_NEW_NONE					0
#define ENEMY_NEW_FOLLOWPATH			(1 << 0)	// enemy follows a path (>1 node)
#define ENEMY_NEW_FORWARDS				(1 << 1)	// enemy moves forwards thru path nodes
#define ENEMY_NEW_BACKWARDS				(1 << 2)	// enemy moves backwards thru path nodes
#define ENEMY_NEW_PINGPONG				(1 << 3)	// enemy moves back and forth thru nodes
#define ENEMY_NEW_CYCLE					(1 << 4)	// enemy cycles thru nodes continuously
#define ENEMY_NEW_MOVEUP				(1 << 5)	// enemy moves up (1 node paths only)
#define ENEMY_NEW_MOVEDOWN				(1 << 6)	// enemy moves down (1 node paths only)
#define ENEMY_NEW_NOJUMPOVER			(1 << 7)	// enemy cannot be jumped over
#define ENEMY_NEW_RADIUSBASEDCOLLISION	(1 << 8)	// enemy collision based on radius
#define ENEMY_NEW_WATCHFROG				(1 << 9)	// enemy faces frogger (1 node paths only)
#define ENEMY_NEW_SNAPFROG				(1 << 10)	// enemy snaps at frogger (watching enemies only)
#define ENEMY_NEW_RANDOMSPEED			(1 << 11)	// enemy has random fluctuations in its speed
#define ENEMY_NEW_FACEFORWARDS			(1 << 12)	// enemy faces forwards
#define ENEMY_NEW_PUSHESFROG			(1 << 13)	// enemy pushes the frog, (Doesn't harm him!)
#define ENEMY_NEW_HOMING				(1 << 14)	// No path, moves blindly towards frog
#define ENEMY_NEW_ROTATEPATH_XZ			(1 << 15)	// Move in a circle around a point. Use tiles though.
#define ENEMY_NEW_ROTATEPATH_XY			(1 << 16)	// Move in a circle around a point. Use tiles though.
#define ENEMY_NEW_ROTATEPATH_ZY			(1 << 17)	// Move in a circle around a point. Use tiles though.
#define ENEMY_NEW_SNAPTILES				(1 << 18)	// Snap at a tilelist
#define ENEMY_NEW_MOVEONMOVE			(1 << 19)	// Move towards frog when it moves
#define ENEMY_NEW_VENT					(1 << 20)	// Gush particles up from a tile.
#define ENEMY_NEW_NODAMAGE				(1 << 21)	// Don't hurt the frog on collision
#define ENEMY_NEW_FLAPPYTHING			(1 << 22)	// Environmental effect - sets NODAMAGE flag. Can be used with swarm
#define ENEMY_NEW_ONEHITKILL			(1 << 23)	// Don't just damage the frog, kill it right off
#define ENEMY_NEW_SLERPPATH				(1 << 24)	// Smooth interpolation between path nodes
#define ENEMY_NEW_BABYFROG				(1 << 25)	// Babies are everyones enemies
#define ENEMY_NEW_RANDOMMOVE			(1 << 26)	// Go to a random adjacent tile
#define ENEMY_NEW_SHADOW				(1 << 27)	// Has a shadow
#define ENEMY_NEW_TILEHOMING			(1 << 28)	// Homes in using tiles


typedef struct TAGENEMY
{
	struct TAGENEMY			*next,*prev;			// ptr to next / prev enemy
	
	long					uid;					// Unique ID

	ACTOR2					*nmeActor;				// actor used as enemy
	unsigned long			flags;					// enemy flags
	unsigned long			reactiveNumber;			// What reactive animation does this NME have?
	unsigned long			doNotMove;				// What reactive animation does this NME have?


	float					startSpeed;				// enemy start speed
	float					speed;					// enemy current speed
	float					accel;					// enemy acceleration

	VECTOR					currNormal;
	VECTOR					deltaNormal;

	UBYTE					active;					// enemy active state
	short					isWaiting;				// enemy pause time at node
	long					isSnapping;				// enemy is snapping (snapping time left)
	short					isIdle;					// enemy idle state for starting animation
	unsigned char			facing;					// For face forward nmes, the tile vector they face

	GAMETILE				*inTile;				// tile enemy is currently 'in'
	PATH					*path;					// ptr to enemy path data

	void					(*Update) (struct TAGENEMY*);	// Enemy update function

} ENEMY;


typedef struct TAGENEMYLIST
{
	int				numEntries;
	ENEMY			head;

} ENEMYLIST;

typedef struct TAG_REACTIVEANIM
{
	char name[12];
	short animChar;
	short animFrog;
	long type;
} REACTIVEANIM;

extern ENEMYLIST enemyList;

// FUNCTION PROTOTYPES

void InitEnemyLinkedList();
void FreeEnemyLinkedList();
void AddEnemy(ENEMY *enemy);
void SubEnemy(ENEMY *enemy);

void UpdateEnemies();

//------------------------------------------------------------------------------------------------

ENEMY *CreateAndAddEnemy(char *eActorName, int flags, long ID, PATH *path, float animSpeed, unsigned char facing);
void AssignPathToEnemy(ENEMY *nme,PATH *path,unsigned long pathFlags);
BOOL EnemyReachedTopOrBottomPoint(ENEMY *nme);
void UpdateEnemyPathNodes(ENEMY *nme);
void CalcEnemyNormalInterps(ENEMY *nme);
ENEMY *GetEnemyFromUID (long uid);

int EnumEnemies(long id, int (*func)(ENEMY*, int), int param);
int MoveEnemyToNode(ENEMY *nme, int node);
void SetEnemyVisible(ENEMY *nme, int visible);
void SetEnemyMoving(ENEMY *nme, int moving);


//------------------------------------------------------------------------------------------------

#endif
