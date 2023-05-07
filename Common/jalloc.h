
#ifndef __JALLOC_H__
#define __JALLOC_H__

#define UBYTE unsigned char
#define ULONG unsigned long
#define LONG  long

#define JALLOC_NAME_LEN 11
//#define CHECKJALLOC

typedef unsigned char 	uchar;
//typedef unsigned short 	ushort;
typedef unsigned long	ulong;

#ifdef DONTUSEJALLOC

#ifndef PC_VERSION
#error Must use Jalloc except in PC version!
#endif 

#include <malloc.h>

#define JallocAlloc(size, clear, name) ((UBYTE) clear? calloc(1, size): malloc(size))
#define JallocAllocDynamic JallocAlloc
#define JallocAllocStatic JallocAlloc
#define JallocFree(addrptr) free(*(addrptr))

#define ShowJalloc()
#define CheckJalloc()
#define JallocInit(base, size)
#define JallocReset()
#define ListJallocBlocks(block)

#else

#ifndef PC_VERSION
#define MAXJALLOCS	2048
#else
#define MAXJALLOCS	4096
#endif

typedef struct 
{
	UBYTE			*address;				      		// where in memory the block is
    long			size;				      			// size of this block
	char			name[JALLOC_NAME_LEN];
	char			inuse;				  	   			// is this block in use?
} jallocentType;

typedef struct 
{
	jallocentType	blocks[MAXJALLOCS];	  	   			// array of control structs
	ulong		base;					  	   			// base address for global block
	long		size;					  	   			// size of area
	int			lastUsedStatic;			  	   			// number of last used block
	int			lastUsedDynamic;		  	   			// number of last used block
	int			staticUsed;
	int			dynamicUsed;
} JALLOCTYPE;

void CheckJalloc();
void JallocInit(ULONG base,ULONG size);
UBYTE *JallocAlloc(LONG size, int clear,char *name);
UBYTE *JallocAllocDynamic(LONG size, int clear,char *name);
UBYTE *JallocAllocStatic(LONG size, int clear,char *name);
void JallocFree(UBYTE **blk);
BOOL JallocFreeDynamic(UBYTE **blk);
BOOL JallocFreeStatic(UBYTE **blk);
void JallocReset();
void ShowJalloc();
void memclear(char *data, int size);
void DMAMemory(char *, u32 ROMStart, u32 ROMEnd);
u32 DMAGetSize(u32 ROMStart, u32 ROMEnd);
void ListJallocBlocks(UBYTE **blk);

extern JALLOCTYPE	jallocControl;

#endif

#endif
