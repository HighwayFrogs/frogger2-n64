/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: types.h
	Programmer	: Andy Eder
	Date		: 15/04/99 14:17:22

----------------------------------------------------------------------------------------------- */

#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED


#define	SCREEN_HT	240

#define X 0
#define Y 1
#define Z 2

#define R	0
#define G	1
#define B	2
#define A	3

#define YES 1
#define NO  0
#define ON  1
#define OFF 0

#define INACTIVE	0
#define ACTIVE		1
#define DEAD		2

#define BYTE	signed char
#define UBYTE	unsigned char

#ifndef PC_VERSION
#define DWORD	unsigned long
#define USHORT	unsigned short
#define SHORT	short
#endif

#define MOD		%

#define	INIT_ANIMATION	(1 << 0)
#define	INIT_SHADOW		(1 << 1)

//object flags
#define OBJECT_FLAGS_GOURAUD_SHADED			   	(1<<0)		// 1
#define OBJECT_FLAGS_XLU					   	(1<<1)		// 2
#define OBJECT_FLAGS_TRANSPARENT			   	(1<<2)		// 4
#define OBJECT_FLAGS_PRELIT					   	(1<<3)		// 8
#define OBJECT_FLAGS_MIP_MAP				   	(1<<4)		// 16
#define OBJECT_FLAGS_MODGE					   	(1<<5)		// 32
#define OBJECT_FLAGS_TEXTURE_BLEND			   	(1<<6)		// 64
#define OBJECT_FLAGS_IA						   	(1<<7)		// 128
#define OBJECT_FLAGS_PHONG					   	(1<<8)		// 256
#define OBJECT_FLAGS_FADEOUT				   	(1<<9)		// 512
#define OBJECT_FLAGS_FACE_COLOUR			   	(1<<10)		// 1024
#define OBJECT_FLAGS_OBJECT_COLOUR			   	(1<<11)		// 2048
#define OBJECT_FLAGS_TEXTURE_BLEND_ENV		   	(1<<12)		// 4096
#define OBJECT_FLAGS_COLOUR_BLEND			   	(1<<13)		// 8192
#define OBJECT_FLAGS_COLOUR_BLEND_AFTERLIGHT   	(1<<14)		// 16384


//--------------
#define Max(p1,p2)	(((p1) > (p2)) ? (p1) : (p2))
#define Min(p1,p2)	(((p1) > (p2)) ? (p2) : (p1))

#ifndef min
#define min Min
#define max Max
#endif

#define BOOL	int

#define SKINNED_SEG (1<<24)

typedef struct
{
	float	x, y, z, w;
}QUATERNION;

// NEW DATA TYPE FOR REDUCING MEMORY REQUIREMENT OF N64 VERSION ----------------------------------

typedef struct
{
	short x,y,z,w;

} QUATERNIONSHORT;

typedef struct
{
	QUATERNIONSHORT	quat;
	int	time;

} KEYFRAMESHORT;

// -----------------------------------------------------------------------------------------------

typedef struct
{
	float	matrix[4][4];
}MATRIX;

typedef struct
{
	float	v[3];
}VECTOR;

typedef struct
{
	float	x,z;
}POINT2D;

typedef struct
{
	VECTOR	pos;
	VECTOR	size;
}BOX;

typedef struct
{
	signed char	v[4];
}BYTEVECTOR;

typedef struct
{
	UBYTE	v[4];
}UBYTEVECTOR;


typedef struct
{
	unsigned short	v[2];
}USHORT2DVECTOR;

typedef struct
{
	short	v[2];
}SHORT2DVECTOR;

typedef struct
{
	float	v[2];
}VECTOR2D;

typedef struct
{
	short	v[3];
}SHORTVECTOR;

typedef struct
{
	unsigned short	v[3];
}USHORTVECTOR;

typedef struct
{
	UBYTE r,g,b,a;
}COLOUR;


typedef struct
{
	VECTOR	normal;
	float	J;
/*
	VECTOR	vertex[3];
	VECTOR	edge[3];
	float	edgeLen[3];
#ifdef STORE_EDGE_J
	float	edgeJ[3];
#endif
	float	u1,u2;
	float	v1,v2;
	UBYTE	terrain;
	UBYTE	i[2];
	UBYTE	space;
*/
}PLANE;

typedef struct
{
	int	handle, volume, actualVolume,origVolume;
	short pitch,sampleNum;
	VECTOR *pos;
	float radius;
}SFX;

typedef struct
{
	int volume;
	short pitch,sampleNum;
}SPOT_SFX;

//======================texture type
typedef struct
{
	int		texID;
	UBYTE	cycleStart;
	UBYTE	cycleEnd;
	short	flags;
	short	cycleSpeed;
	short	cycleCount;
	unsigned short	sx;
	unsigned short	sy;
	unsigned short	TCScaleX;
	unsigned short	TCScaleY;
	int		size;
	short	format;
	short	pixSize;
	char	*data;
	short	*palette;
}TEXTURE;

#ifdef N64_VERSION
typedef TEXTURE TEXENTRY;
#endif

typedef struct
{
	VECTOR	offset;
	float	radius;
	float	radiusAim;
	VECTOR	oldOffset;
	UBYTE	posTag;
	UBYTE	quatTag;
	short	flags;

}COLLSPHERE;


typedef struct
{
	short			numFaces;
	short			numVertices;
	VECTOR			*vertices;
	SHORTVECTOR		*faceIndex;
	BYTEVECTOR		*faceNormals;
	USHORT2DVECTOR	*faceTC;
	USHORT2DVECTOR	*originalFaceTC;
	BYTEVECTOR		*vertexNormals;
	char		 	*faceFlags;
	TEXTURE			**textureIDs;

}MESH;


typedef struct
{
	VECTOR	vect;
	int	time;
}VKEYFRAME;

typedef struct
{
	QUATERNION	quat;
	int	time;
}QKEYFRAME;

typedef struct
{
	float			radius;
	UBYTE			alpha;
	char			draw;

} ACTOR_SHADOW;


//----------------animation
typedef struct 
{
	short	   animStart, animEnd;
	float	   speed;
	int		   loop;
} animation;

#define ANIM_QUEUE_LENGTH 5

typedef struct
{
	short		numAnimations;
	short		currentAnimation;
	BOOL		reachedEndOfAnimation;
	BOOL		loopAnimation;
	float		animationSpeed;
	short		queueAnimation[ANIM_QUEUE_LENGTH];
	BOOL		queueLoopAnimation[ANIM_QUEUE_LENGTH];
	float		queueAnimationSpeed[ANIM_QUEUE_LENGTH];
	short 		numberQueued;
	animation	*anims;
	float		animTime;
	char		queueNumMorphFrames[ANIM_QUEUE_LENGTH];
	BYTE		numMorphFrames;
	char		currentMorphFrame;
	float		morphTo;
	float		morphFrom;

	char		loopFlags[5];		// NOTE: This assumes that frogger will never have looping sounds on animations
	struct _SAMPLE **sfxMapping;	// Cue these sound effects off the animations

}ACTOR_ANIMATION;
/*
typedef struct
{
	TEXTURE **texture;
	short	numFrames;
	short	overrideNumFrames;
	char	filename[16];

}FRAMELIST;

typedef struct
{
	FRAMELIST	*frameList;
	UBYTE		delay;
	UBYTE		type;
	UBYTE		startAlpha;
	UBYTE		endAlpha;
	USHORT		startScale;
	USHORT		endScale;
	USHORT		spriteFlags;
	short		pad;

}SPRITE_ANIMATION_TEMPLATE;

typedef struct
{
	FRAMELIST	*frameList;
	UBYTE		delay;
	UBYTE		type;
	UBYTE		startAlpha;
	UBYTE		endAlpha;
	USHORT		startScale;
	USHORT		endScale;
	USHORT		spriteFlags;
	USHORT		lifespan;
	USHORT		currentFrame;
	float		counter;
	float		lifetime;

}SPRITE_ANIMATION;
*/

typedef struct OBJECTSPRITE
{
	TEXTURE		*textureID;
	struct TAGSPRITE *sprite;		//actual sprite in sprite list
	short		x, y, z;
	short		sx;	//xsize - 32 is normal
	short		sy;	//ysize - 32 is normal
	short		ox;	//offset from centre
	short		oy;	//offset from centre
	short		flags;

}OBJECTSPRITE;

/*
typedef struct
{
	FRAMELIST	*frameList;
	UBYTE		delay;
	UBYTE		flags;
	UBYTE		currentFrame;
	BYTE		counter;

}TEXTURE_ANIMATION;
*/
typedef struct
{
	s16		*verts;
//	Vtx		**verts;
	char	numVerts;

}SKINVTX;

typedef struct
{
	float	v[3];
	s8		n[3];

}DUELVECTOR; 


typedef struct OBJECT
{
	u32			objID;
	u8			name[16];
	short		pixelOutAlpha;
	u8			xlu;

	u8			numSprites;
	OBJECTSPRITE	*sprites;

	//for non skinned objects
	MESH		*mesh;
	Gfx			*drawList;

	long textureAnim; // NOT USED - just for padding

	//for skinning
	SKINVTX		*effectedVerts;
//	Vtx			**effectedVerts;
	DUELVECTOR	*originalVerts;

	short		numVerts;


	VKEYFRAME	*scaleKeys;
	VKEYFRAME	*moveKeys;

#ifdef PC_VERSION
	QKEYFRAME	*rotateKeys;
#else
	KEYFRAMESHORT *rotateKeys;
#endif

	short		numScaleKeys;
	short		numMoveKeys;
	short		numRotateKeys;

	COLOUR		colour;
#ifndef PCVERSION
	void		*renderData;
#else
	D3DTLVERTEX *renderData;
#endif
	short		flags;

	short		numChildren;
	struct OBJECT	*children;
	struct OBJECT	*next;

	COLLSPHERE	*collSphere;
#ifndef PC_VERSION
	Mtx			objMatrix;
#else
	MATRIX		objMatrix;
#endif

}OBJECT;

typedef struct
{
	int		objectID;

	Mtx		*matrix;
	short	objectSize;
	OBJECT	*object;
	float	radius;
	PLANE	*planes;
	ACTOR_ANIMATION	*animation;
	Gfx		*drawList;
	Vtx		*vtx[2];
	MESH	*LOCMesh;
	short	vtxBuf;
	short	numVtx;

}OBJECT_CONTROLLER;


typedef struct
{
	int		objectID;
 	OBJECT_CONTROLLER	*objCont;
}OBJECT_DESCRIPTOR;


typedef struct
{
	char	*data;
	char	*freePtr;
	int		numObjects;
	OBJECT_DESCRIPTOR	*objList;
}OBJECT_BANK;

typedef struct TAGACTOR
{
	struct TAGACTOR				*next,*prev;
	float						distanceFromCamera;

	Mtx							*matrix;
	BYTE						status;
	short						xluOverride;
	int							flags;

	float						maxRadius;

	VECTOR						pos;
	VECTOR						oldpos;
	VECTOR						vel;
	VECTOR						rot;
	VECTOR						rotaim;
								
	QUATERNION					qRot;
												
	VECTOR	  					scale;

	OBJECT_CONTROLLER			*objectController;
	OBJECT_CONTROLLER			*LODObjectController;
	ACTOR_SHADOW				*shadow;
	ACTOR_ANIMATION				*animation;
	unsigned char				visible;

} ACTOR;

/*
typedef struct TAGTEXT
{
	struct TAGTEXT *next,*prev;
	char message[64];
	int x,y;
	float scaleX,scaleY;
	UBYTE r,g,b,a;
	int byebye;
}TEXT;

typedef struct
{
	TEXT head;
	int numEntries;
}TEXTLIST;
*/

typedef struct TAGPLANE2
{
	float	distance;
	float	J;
	VECTOR	point;
	VECTOR	normal;

} PLANE2;


typedef struct TAGPOLY
{
	struct TAGPOLY *next;

	PLANE2 plane;
	unsigned char r, g, b, a;
	float u, v, u1, v1, angle;
	TEXTURE *tex;
	VECTOR *vT;

} POLYGON;


#define dprintf debugPrintf(sprintf(outputMessageBuffer, 

#define bprintf sprintf(statusMessage,

extern char outputMessageBuffer[];

extern Gfx *glistp;


#endif