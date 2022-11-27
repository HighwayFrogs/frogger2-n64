/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: graphic.c
	Programmer	: Andy Eder
	Date		: 06/08/99 12:37:57

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>
#include "incs.h"

TIMECOUNT timers[10];
TIMECOUNT printTimer[10];
unsigned int frameStart;
unsigned int printFrameStart[10];
char timerMode=0;
char message[256];

TEXTOVERLAY *timerTxt[10];

/*	--------------------------------------------------------------------------------
	Function 	: TIMER_InitTimers
	Purpose 	: initialises the timers
	Parameters 	: 
	Returns 	: void
	Info 		:
*/
void TIMER_InitTimers()
{
	int i,j;

	for (i=0;i<10;i++)
	{
		timers[i].status = 0;

		for (j=0;j<MAX_SLICES;j++)
		{
			timers[i].start[j] = 0;
			timers[i].end[j] = 0;
			timers[i].total[j] = 0;
		}

		timers[i].name[0] = NULL;
		timers[i].slice = 0;
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: TIMER_ClearTimers
	Purpose 	: clears the timers
	Parameters 	: 
	Returns 	: void
	Info 		:
*/
void TIMER_ClearTimers()
{
	int i,j;
	char overrun=0;

	for (i=0;i<10;i++)
	{
		if (timers[i].status==0) printTimer[i] = timers[i];
		else overrun=1;
	}
	if (overrun==1) return;
	for (i=0;i<10;i++)
	{
		//printTimer[i] = timers[i];
		timers[i].slice=0;
		timers[i].name[0] = NULL;
		timers[i].frameStart = frameStart;

		for (j=0;j<MAX_SLICES;j++)
		{
			timers[i].start[j] = 0;
			timers[i].end[j] = 0;
			timers[i].total[j] = 0;
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: TIMER_StartTimer
	Purpose 	: starts a timer
	Parameters 	: int,char *
	Returns 	: void
	Info 		:
*/
void TIMER_StartTimer(int number,char *name)
{
	int slice;

	slice = timers[number].slice;

	if (slice>(MAX_SLICES-1)) return;

	timers[number].start[slice] = osGetCount();
	timers[number].status = 1;
	//timers[number].frameStart = frameStart;
//	strcpy(timers[number].name, name);
	sprintf(timers[number].name, name);
}

/*	--------------------------------------------------------------------------------
	Function 	: TIMER_EndTimer
	Purpose 	: ends a timer
	Parameters 	: int
	Returns 	: void
	Info 		:
*/
void TIMER_EndTimer(int number)
{
	unsigned int maxCount = -1;
	int slice;

	slice = timers[number].slice;

	if (slice>(MAX_SLICES-1)) return;

	timers[number].end[slice] = osGetCount();
	if (timers[number].end[slice] > timers[number].start[slice])
	{
		timers[number].total[slice] += timers[number].end[slice] - timers[number].start[slice];
	}
	else
	{
		timers[number].total[slice] += maxCount-timers[number].start[slice]+timers[number].end[slice];
	}
	timers[number].status = 0;

	if (slice<MAX_SLICES) timers[number].slice++;
}

/*	--------------------------------------------------------------------------------
	Function 	: TIMER_PrintTimers
	Purpose 	: prints the timers
	Parameters 	: 
	Returns 	: void
	Info 		:
*/
void TIMER_PrintTimers()
{
	int i,j;
	unsigned int ulx,uly,lrx,lry,size,start;
	unsigned int maxCount = -1;

	if(!timerMode)
	{
		if(timerTxt[0])
		{
			i = 10;
			while(i--)
				timerTxt[i]->draw = 0;
		}

		return;
	}

	if(timerMode == 2)
	{
		i = 10;
		while(i--)
		{
			if(!timerTxt[i])
				timerTxt[i] = CreateAndAddTextOverlay(30,20 + (i * 20),timers[i].name,NO,255,oldeFont,0,0);
		}
	}

	gDPPipeSync(glistp++);
	gDPSetRenderMode(glistp++,G_RM_OPA_SURF , G_RM_OPA_SURF2);      
	gDPPipeSync(glistp++);
	gDPSetCycleType(glistp++, G_CYC_FILL);

	gDPSetFillColor(glistp++, GPACK_RGBA5551(255,255,255,1) << 16 | 
		GPACK_RGBA5551(255,255,255,1));

	gDPFillRectangle(glistp++,100,30,101,195);
	gDPFillRectangle(glistp++,100+((TicksPerFrame)/10000),30,101+((TicksPerFrame)/10000),195);
	gDPFillRectangle(glistp++,100+((TicksPerFrame)/20000),30,101+((TicksPerFrame)/20000),195);
	gDPFillRectangle(glistp++,100+((TicksPerFrame)/1),30,101+((TicksPerFrame)/1),195);

	ulx = 100;
	uly = 30;
	lry = 35;
	for (i=0;i<10;i++)
	{
		if(timerTxt[i])
			timerTxt[i]->draw = 0;

		if((timerMode == 2) && (printTimer[i].slice > 0))
		{
			if(timerTxt[i])
			{
				timerTxt[i]->draw = 1;
				timerTxt[i]->yPos = uly;
			}
		}

		for (j=0;j<printTimer[i].slice;j++)
		{
			if (printTimer[i].start[j]!=0)
			{
				gDPPipeSync(glistp++);
				switch(j&3)
				{
				case 0:
					gDPSetFillColor(glistp++, GPACK_RGBA5551(240,0,0,1) << 16 | 
						GPACK_RGBA5551(240,0,0,1));
					break;
				case 1:
					gDPSetFillColor(glistp++, GPACK_RGBA5551(0,240,0,1) << 16 | 
						GPACK_RGBA5551(0,240,0,1));
					break;
				case 2:
					gDPSetFillColor(glistp++, GPACK_RGBA5551(0,0,240,1) << 16 | 
						GPACK_RGBA5551(0,0,240,1));
					break;
				case 3:
					gDPSetFillColor(glistp++, GPACK_RGBA5551(240,240,10,1) << 16 | 
						GPACK_RGBA5551(240,240,10,1));
					break;
				}
				

				size = printTimer[i].total[j]/20000;
				if (printTimer[i].start[j] > printTimer[i].frameStart)
				{
					start = (printTimer[i].start[j]-printTimer[i].frameStart)/20000;
				}
				else
				{
					start = (maxCount-printTimer[i].frameStart+printTimer[i].start[j])/20000;
				}
				lrx = 100+size;
				if(timerMode)
					gDPFillRectangle(glistp++,ulx+start,uly,lrx+start,lry);
			}
		}
		uly += 16;
		lry += 16;
	}

	j = 0;

	gDPPipeSync(glistp++);
	gDPSetCycleType(glistp++, G_CYC_1CYCLE);      
	gDPPipeSync(glistp++);
	gDPSetRenderMode(glistp++,G_RM_AA_ZB_OPA_SURF , G_RM_AA_ZB_OPA_SURF2);
}






//----- [ GLOBALS ] ----------------------------------------------------------------------------//

BACKDROPLIST backdropList;


//----- [ BACKDROP HANDLING ] ------------------------------------------------------------------//


/*	--------------------------------------------------------------------------------
	Function 	: InitBackdropLinkedList
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void InitBackdropLinkedList()
{
	backdropList.head.next = backdropList.head.prev = &backdropList.head;
	backdropList.numEntries = 0;
}

/*	--------------------------------------------------------------------------------
	Function 	: AddBackdrop()
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void AddBackdrop(BACKDROP *backdrop)
{
	BACKDROP *ptr;	 

	if(backdrop->next == NULL)
	{
		for(ptr = backdropList.head.next;ptr != &backdropList.head;ptr = ptr->next)
		{
			if((ptr->zPos < backdrop->zPos) || ((ptr->texture == backdrop->texture) && (ptr->texture->format == backdrop->texture->format) && (ptr->zPos == backdrop->zPos)))
			{
				break;
			}
		}
		backdrop->next = ptr;
		backdrop->prev = ptr->prev;
		ptr->prev->next = backdrop;
		ptr->prev = backdrop;
		backdropList.numEntries++;
		backdrop->draw = 1;
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: SubBackdrop
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void SubBackdrop(BACKDROP *backdrop)
{
	if(backdrop->next == NULL)
		return;

	backdrop->prev->next = backdrop->next;
	backdrop->next->prev = backdrop->prev;

	backdrop->next = NULL;
	backdropList.numEntries--;
}

/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
void FreeBackdropLinkedList()
{
	BACKDROP *cur,*next;	 

	if(backdropList.numEntries == 0)
		return;

	dprintf"Freeing linked list : BACKDROP : (%d elements)\n",backdropList.numEntries));
	for(cur = backdropList.head.next; cur != &backdropList.head; cur = next)
	{
		next = cur->next;

		SubBackdrop(cur);
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: SetupBackdrop
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
BACKDROP *SetupBackdrop(BACKDROP *backdrop,int texID,int sourceX,int sourceY,int z,int destX,int destY,int destWidth,int destHeight,int scalex,int scaley,int flags)
{
	if(backdrop == NULL)
		backdrop = (BACKDROP *)JallocAlloc(sizeof(BACKDROP),YES,"backdrop");
	backdrop->scaleX = scalex;
	backdrop->scaleY = scaley;
	backdrop->xPos = sourceX;
	backdrop->yPos = sourceY;
	backdrop->zPos = z;
	backdrop->draw = 1;
	backdrop->flags = flags;
	backdrop->r = backdrop->g = backdrop->b = backdrop->a = 255;
	FindTexture(&backdrop->texture,texID,YES);
	backdrop->background.s.imageX = sourceX<<5;
	backdrop->background.s.imageY = sourceY<<5;
	backdrop->background.s.imageW = (backdrop->texture->sx+1)<<2;
	backdrop->background.s.imageH = backdrop->texture->sy<<2;
	backdrop->background.s.frameX = destX<<2;
	backdrop->background.s.frameY = destY<<2;
	backdrop->background.s.frameW = destWidth<<2;
	backdrop->background.s.frameH = destHeight<<2;
	backdrop->background.s.imagePtr = (u64*)backdrop->texture->data;
	backdrop->background.s.imageLoad = G_BGLT_LOADTILE;
	backdrop->background.s.imageFmt = backdrop->texture->format;
	backdrop->background.s.imageSiz = backdrop->texture->pixSize;
	backdrop->background.s.imagePal = 0;
	backdrop->background.s.imageFlip = 0;
	backdrop->background.s.scaleW = (1024*1024)/scalex;
	backdrop->background.s.scaleH = (1024*1024)/scaley;
	backdrop->background.s.imageYorig = 0<<5;

	osWritebackDCache(&backdrop->background, sizeof(uObjBg));

	AddBackdrop(backdrop);

	return backdrop;
}
