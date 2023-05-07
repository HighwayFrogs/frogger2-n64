#ifndef _3DTEXT_H_INCLUDED
#define _3DTEXT_H_INCLUDED




//#define T360_TIMER		200

#define T3D_CIRCLE		1
#define T3D_HORIZONTAL	2
#define T3D_VERTICAL	3

#define T3D_MOVE_NONE		(1<<0)
#define T3D_MOVE_SPIN		(1<<1)
#define T3D_MOVE_LEFT		(1<<2)
#define T3D_MOVE_RIGHT		(1<<3)
#define T3D_MOVE_UP			(1<<4)
#define T3D_MOVE_DOWN		(1<<5)
#define T3D_MOVE_IN			(1<<6)
#define T3D_MOVE_OUT		(1<<7)
#define T3D_MOVE_SINE		(1<<8)
#define T3D_MOVE_TWIST		(1<<9)
#define T3D_MOVE_SQUISH		(1<<10)
#define T3D_ALIGN_CENTRE	(1<<11)
#define T3D_ALIGN_LEFT		(1<<12)
#define T3D_ALIGN_RIGHT		(1<<13)
#define T3D_MOVE_MODGE		(1<<14)

#define T3D_CREATED			(1<<28)
#define T3D_CALCULATED		(1<<29)
#define T3D_PATH_BOUNCE		(1<<30)
#define T3D_PATH_LOOP		(1<<31)

typedef struct _TEXT3D
{
	struct _TEXT3D *next, *prev;
	Vtx vT[100*4];			// Store polygon vertices
	char string[100];		// Text string on which the 3DText is based
	char vR, vG, vB, vA;    // Vertex rgb
	short type;				// Line, circle, etc
	unsigned long motion;	// Static, spin, move from left
	float angle, scale, radius,
		sinA, sinS, twistA; // Sine amount and speed, twist amount
	long width, xOffs, yOffs, zOffs;
	VECTOR vel;             // Velocity vector
	float rSpeed;			// Speed of rotation
	//unsigned long timer;	// For things like T3D_MOVE_360
	unsigned int tileSize;	// Scaled size of each tile

} TEXT3D;


typedef struct _TEXT3DLIST
{
	TEXT3D head;
	unsigned long numEntries;

} TEXT3DLIST;


extern TEXT3DLIST text3DList;


extern TEXT3D *CreateAndAdd3DText( char *str, unsigned long w, char r, char g, char b, char a, 
									short type, unsigned long motion, VECTOR *spd, float rSpd, float initAngle, 
									long xO, long yO, long zO, float sinA, float sinS, float twA );
extern void Print3DText( );
extern void Calculate3DText( );
extern void Init3DTextList( );
extern void Free3DTextList( );
extern void Sub3DText( TEXT3D *t );
extern void MakeTextCircle( TEXT3D *t3d );
extern void MakeTextLine( TEXT3D *t3d );
extern void UpdateT3DMotion( TEXT3D *t3d );
extern void MakeHiscoreText( );


/*

#define T3D_CIRCLE		1
#define T3D_HORIZONTAL	2
#define T3D_VERTICAL	3

#define T3D_MOVE_NONE	(1<<0)
#define T3D_MOVE_SPIN	(1<<1)
#define T3D_MOVE_LEFT	(1<<2)
#define T3D_MOVE_RIGHT	(1<<3)
#define T3D_MOVE_UP		(1<<4)
#define T3D_MOVE_DOWN	(1<<5)
#define T3D_MOVE_IN		(1<<6)
#define T3D_MOVE_OUT	(1<<7)
#define T3D_MOVE_SINE	(1<<8)
#define T3D_MOVE_TWIST	(1<<9)
#define T3D_MOVE_SQUISH	(1<<10)

#define T3D_CALCULATED	(1<<29)
#define T3D_PATH_BOUNCE	(1<<30)
#define T3D_PATH_LOOP	(1<<31)

typedef struct _TEXT3D
{
	struct _TEXT3D *next, *prev;
	Vtx *vT;				// Store polygon vertices
	char *string;			// Text string on which the 3DText is based
	char vR, vG, vB, vA;    // Vertex rgb
	short type;				// Line, circle, etc
	unsigned long motion;	// Static, spin, move from left
	float angle, scale, radius,
		sinA, sinS, twistA; // Sine amount and speed, twist amount
	long width, xOffs, yOffs, zOffs;
	VECTOR vel;             // Velocity vector
	float rSpeed;			// Speed of rotation
	unsigned int tileSize;	// Scaled size of each tile

} TEXT3D;


typedef struct _TEXT3DLIST
{
	TEXT3D head;
	unsigned long numEntries;

} TEXT3DLIST;


extern TEXT3DLIST text3DList;


extern void CreateAndAdd3DText( char *str, unsigned long w, char r, char g, char b, char a, 
							   short type, unsigned long motion, VECTOR *spd, float rSpd, 
							   long xO, long yO, long zO, float sinA, float sinS, float twA );
extern void Print3DText( );
extern void Calculate3DText( );
extern void Init3DTextList( );
extern void Free3DTextList( );
extern void Sub3DText( TEXT3D *t );
extern void MakeTextCircle( TEXT3D *t3d );
extern void MakeTextLine( TEXT3D *t3d );
extern void UpdateT3DMotion( TEXT3D *t3d );
*/
#endif