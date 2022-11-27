/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: Jalloc.c
	Programmer	: (ta John Whigham)
	Date		: 29/04/99 11:00:38

----------------------------------------------------------------------------------------------- */

#ifndef DONTUSEJALLOC

#include <ultra64.h>

#include "incs.h"

JALLOCTYPE      jallocControl;
int jallocAllocLocked = FALSE;
int jallocFreeLocked = FALSE;

/**********************************************************************************/
/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void memclear(char *data, int size)
{
	char *temp = data;
	int x;

	for(x = 0; x < size; x++)
	{
		temp[x] = 0;
	}
}


/*********************************************************************************
	Function : Jalloc_Init
	Purpose : initialise a jalloc area structure
	Parameters : struct to initialise, ptr to base of area to use, size of area 
	Returns :
	Info :
**********************************************************************************/
void JallocInit(ULONG base, ULONG size)
{
	JALLOCTYPE *jp = &jallocControl;

	if (size%16 != 0)
    	size -= (size%16);										// round to long boundary

	

	jp->base = base;
	jp->size = size;
	JallocReset();

	dprintf"Jalloc initialised\n")); 
	
}

/*************************************************************************************/

/**********************************************************************************/
/*	Function : Jalloc_Alloc
	Purpose : try to claim a block of memory
	Parameters : jalloc struct, size of required area in bytes 
	Returns : ptr to area, or NULL for failure
	Info :
*/
/***********************************************************************************/
UBYTE *JallocAlloc(LONG size, int clear, char *name)
{		  
	UBYTE *dest;

#ifdef CHECKJALLOC
	if(jallocAllocLocked)
	{
		dprintf"ALLOC LOCKED IN ALLOC\n"));
		MusSetMasterVolume(MUSFLAG_EFFECTS,22000);
		PlaySampleNot3D(FX_THUNDER,100,128,128);
		while(!(controllerdata[ActiveController].button & CONT_A));
	}
	if(jallocFreeLocked)
	{
		dprintf"FREE LOCKED IN ALLOC\n"));
		MusSetMasterVolume(MUSFLAG_EFFECTS,22000);
		PlaySampleNot3D(FX_THUNDER,100,128,128);
		while(!(controllerdata[ActiveController].button & CONT_A));
	}
	jallocAllocLocked++;
#endif

	if(size == 0)
	{
		dprintf"ZERO MALLOC!!\n"));
#ifdef CHECKJALLOC
		jallocAllocLocked--;
#endif
		return 0;
	}

	if (size%16 != 0)
    	size += 16-(size%16);										// round to long boundary

	if((frameCount) && (dispFrameCount > 1))
		dest = JallocAllocDynamic(size,clear,name);
	else
		dest = JallocAllocStatic(size,clear,name);


#ifdef CHECKJALLOC
	CheckJalloc();

	jallocAllocLocked--;
#endif

	return dest;
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
UBYTE *JallocAllocStatic(LONG size, int clear, char *name)
{
	ULONG	k, oldsize;
	long	l;
	JALLOCTYPE *jp = &jallocControl;
	UBYTE	*dest;

	if(jp->lastUsedStatic == -1)											// first ever block claimed?
	{
		dest = ((UBYTE *)(jp->base+jp->size-size));
		jp->blocks[0].inuse = 1;
		jp->blocks[0].size = size;
		jp->blocks[0].address = dest;
		jp->lastUsedStatic = 0;
		l = 0;
		goto jallocSuccess;
	}

	for(l = 0; l < jp->lastUsedStatic+1; l++)
  	{
		if ((jp->blocks[l].inuse == 0) && (jp->blocks[l].size >= size))
			break;
  	}

	if(l >= jp->lastUsedDynamic)
		goto jallocFailed;

	if(l == jp->lastUsedStatic+1)									// add to the end of the list
	{
		if(jp->size - jp->staticUsed - jp->dynamicUsed < size)							// not enough space left at end of block
			goto jallocFailed;
		dest = ((UBYTE *)(jp->blocks[jp->lastUsedStatic].address - size));
		jp->lastUsedStatic++;
		jp->blocks[jp->lastUsedStatic].inuse = 1;
		jp->blocks[jp->lastUsedStatic].size = size;
		jp->blocks[jp->lastUsedStatic].address = dest;
		l = jp->lastUsedStatic;
		goto jallocSuccess;
	}

	oldsize = jp->blocks[l].size;
	jp->blocks[l].size = size;
	jp->blocks[l].inuse = 1;
	if((size < oldsize) && (jp->lastUsedStatic < jp->lastUsedDynamic-1))		// if the old block was larger and there is room to grow...
	{
		for(k = jp->lastUsedStatic; k > l; k--)
			jp->blocks[k+1] = jp->blocks[k];
		k = l+1;
		jp->lastUsedStatic++;        
		jp->blocks[k].inuse = 0;
		jp->blocks[k].size = oldsize-size;
		jp->blocks[k].address = jp->blocks[l].address; 
		jp->blocks[l].address = jp->blocks[k].address + jp->blocks[k].size; 
	}
	else if(size < oldsize)
	{
    	jp->blocks[l].size = oldsize;							// can't split it, so to avoid drop-out :-(
	}
	dest = jp->blocks[l].address;

	goto jallocSuccess;

jallocFailed:
	if(size == 0)
		dprintf"Oops! trying to malloc 0 bytes\n"));
	else
		dprintf"Cannot malloc %d - out of memory\n",size));

	dprintf"Jalloc size: %d\nStatic: %d\nDynamic: %d\n",jp->size,jp->staticUsed,jp->dynamicUsed));

	Crash("JALLOC");
	ShowJalloc();
	//while(1);
	return NULL;

jallocSuccess:
	jp->staticUsed += jp->blocks[l].size;
	
	if(strlen(name) < JALLOC_NAME_LEN - 1)
	{
		memcpy(jp->blocks[l].name,name,strlen(name));
		jp->blocks[l].name[strlen(name)] = 0;
	}

	if(clear == TRUE)
		memclear(dest, size);
	
	return dest;
}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
UBYTE *JallocAllocDynamic(LONG size, int clear, char *name)
{
	ULONG	k, oldsize;
	long	l;
	JALLOCTYPE *jp = &jallocControl;
	UBYTE	*dest;

	if(jp->lastUsedDynamic == MAXJALLOCS)											// first ever block claimed?
	{
		dest = ((UBYTE *)(jp->base));
		l = MAXJALLOCS - 1;
		jp->blocks[l].inuse = 1;
		jp->blocks[l].size = size;
		jp->blocks[l].address = dest;
		jp->lastUsedDynamic = l;
		goto jallocSuccess;
	}

	for(l = MAXJALLOCS-1; l > jp->lastUsedDynamic-1; l--)
  	{
		if ((jp->blocks[l].inuse == 0) && (jp->blocks[l].size >= size))
			break;
  	}

	if(l <= jp->lastUsedStatic)
		goto jallocFailed;

	if(l == jp->lastUsedDynamic-1)									// add to the end of the list
	{
		if(jp->size - jp->staticUsed - jp->dynamicUsed < size)							// not enough space left at end of block
			goto jallocFailed;
		dest = ((UBYTE *)(jp->blocks[jp->lastUsedDynamic].address + jp->blocks[jp->lastUsedDynamic].size));
		jp->lastUsedDynamic--;
		jp->blocks[jp->lastUsedDynamic].inuse = 1;
		jp->blocks[jp->lastUsedDynamic].size = size;
		jp->blocks[jp->lastUsedDynamic].address = dest;
		l = jp->lastUsedDynamic;
		goto jallocSuccess;
	}

	oldsize = jp->blocks[l].size;
	jp->blocks[l].size = size;
	jp->blocks[l].inuse = 1;
	if((size < oldsize) && (jp->lastUsedStatic < jp->lastUsedDynamic-1))		// if the old block was larger and there is room to grow...
	{
		for(k = jp->lastUsedDynamic; k < l; k++)
			jp->blocks[k-1] = jp->blocks[k];
		k = l-1;
		jp->lastUsedDynamic--;        
		jp->blocks[k].inuse = 0;
		jp->blocks[k].size = oldsize-size;
		jp->blocks[k].address = jp->blocks[l].address + size; 
	}
	else if(size < oldsize)
	{
    	jp->blocks[l].size = oldsize;							// can't split it, so to avoid drop-out :-(
	}
	dest = jp->blocks[l].address;

	goto jallocSuccess;

jallocFailed:
	if(size == 0)
		dprintf"Oops! trying to malloc 0 bytes\n"));
	else
		dprintf"Cannot malloc %d - out of memory\n",size));
	Crash("JALLOC");
	ShowJalloc();
	//while(1);
	return NULL;

jallocSuccess:
	jp->dynamicUsed += jp->blocks[l].size;
	memcpy(jp->blocks[l].name,name,strlen(name));
	if(strlen(name) < JALLOC_NAME_LEN - 1)
		jp->blocks[l].name[strlen(name)] = 0;
	if(clear == TRUE)
		memclear(dest, size);

	return dest;
}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
BOOL JallocFreeStatic(UBYTE **blk)
{
	LONG	l, k;
	JALLOCTYPE *jp = &jallocControl;

	if (jp->lastUsedStatic == -1)										// nothing to free
		return NO;  

	for(l = 0; l <= jp->lastUsedStatic; l++)
	{
		if(jp->blocks[l].address == *blk)
			break;
	}
	if(l > jp->lastUsedStatic)										// could not find the block to allocate
		return NO;

	*blk = NULL;

	if(jp->blocks[l].inuse == 0)
		return YES;
			
	if(l == jp->lastUsedStatic)
	{
		jp->staticUsed -= jp->blocks[jp->lastUsedStatic].size;
		jp->lastUsedStatic--;											// last item in list
		if((jp->lastUsedStatic >= 0) && (jp->blocks[jp->lastUsedStatic].inuse == 0))	// was the previous one empty aswell?
		{
//			jp->staticUsed -= jp->blocks[jp->lastUsedStatic].size;
			jp->lastUsedStatic--;
		}
		return YES;
	}

	jp->blocks[l].inuse = 0;
	jp->staticUsed -= jp->blocks[l].size;
	if(jp->blocks[l+1].inuse == 0)								// merge with next one
	{
		jp->blocks[l].size += jp->blocks[l+1].size;
		jp->blocks[l].address = jp->blocks[l+1].address;
		for(k = l+1; k < jp->lastUsedStatic; k++)
			jp->blocks[k] = jp->blocks[k+1];
		jp->blocks[jp->lastUsedStatic--].inuse = 0;
	}
	if((l > 0) && (jp->blocks[l-1].inuse == 0))				// merge with previous one
	{
		jp->blocks[l-1].size += jp->blocks[l].size;
		jp->blocks[l-1].address = jp->blocks[l].address;
		for(k = l; k < jp->lastUsedStatic; k++)
			jp->blocks[k] = jp->blocks[k+1];
		jp->blocks[jp->lastUsedStatic--].inuse = 0;
	}
	return YES;
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
BOOL JallocFreeDynamic(UBYTE **blk)
{
	LONG	l, k;
	JALLOCTYPE *jp = &jallocControl;

	if (jp->lastUsedDynamic == MAXJALLOCS)										// nothing to free
		return NO;  

	for(l = MAXJALLOCS-1; l >= jp->lastUsedDynamic; l--)
	{
		if (jp->blocks[l].address == *blk)
			break;
	}
	if (l < jp->lastUsedDynamic)										// could not find the block to allocate
		return NO;

	*blk = NULL;
			
	if(jp->blocks[l].inuse == 0)
		return YES;

	if(l == jp->lastUsedDynamic)
	{
		jp->dynamicUsed -= jp->blocks[jp->lastUsedDynamic].size;
		jp->lastUsedDynamic++;											// last item in list
		if((jp->lastUsedDynamic <= MAXJALLOCS-1) && (jp->blocks[jp->lastUsedDynamic].inuse == 0))	// was the previous one empty aswell?
		{
//			jp->dynamicUsed -= jp->blocks[jp->lastUsedDynamic].size;
			jp->lastUsedDynamic++;
		}
		return YES;
	}

	jp->blocks[l].inuse = 0;
	jp->dynamicUsed -= jp->blocks[l].size;
	if(jp->blocks[l-1].inuse == 0)								// merge with next one
	{
		jp->blocks[l].size += jp->blocks[l-1].size;
		for(k = l-1; k > jp->lastUsedDynamic; k--)
			jp->blocks[k] = jp->blocks[k-1];
		jp->blocks[jp->lastUsedDynamic++].inuse = 0;
	}
	if((l < MAXJALLOCS-1) && (jp->blocks[l+1].inuse == 0))				// merge with previous one
	{
		jp->blocks[l+1].size += jp->blocks[l].size;
		for(k = l; k > jp->lastUsedDynamic; k--)
			jp->blocks[k] = jp->blocks[k-1];
		jp->blocks[jp->lastUsedDynamic++].inuse = 0;
	}
	return YES;
}

/*************************************************************************************/
/*	Function : Jalloc_Free
	Purpose : free a block of memory from a jalloc area
	Parameters : jalloc struct, ptr to block to free 
	Returns : 
	Info :
*/
/***********************************************************************************/
void 
JallocFree(UBYTE **blk)
{
#ifdef CHECKJALLOC
	if(jallocAllocLocked)
	{
		dprintf"ALLOC LOCKED IN FREE\n"));
		MusSetMasterVolume(MUSFLAG_EFFECTS,22000);
		PlaySampleNot3D(FX_THUNDER,100,128,128);
		while(!(controllerdata[ActiveController].button & CONT_A));
	}
	if(jallocFreeLocked)
	{
		dprintf"FREE LOCKED IN FREE\n"));
		MusSetMasterVolume(MUSFLAG_EFFECTS,22000);
		PlaySampleNot3D(FX_THUNDER,100,128,128);
		while(!(controllerdata[ActiveController].button & CONT_A));
	}
	jallocFreeLocked++;
#endif


	if(*blk == NULL)
	{
#ifdef CHECKJALLOC
		jallocFreeLocked--;
#endif
		return;
	}
	if(JallocFreeDynamic(blk))
	{
#ifdef CHECKJALLOC
		CheckJalloc();
		jallocFreeLocked--;
#endif
		return;
	}

	JallocFreeStatic(blk);
#ifdef CHECKJALLOC
	CheckJalloc();
	jallocFreeLocked--;
#endif
}

/***********************************************************************************/
/*	Function : Jalloc_Reset
	Purpose : reset a jalloc block to initial status
	Parameters : jalloc struct 
	Returns : 
	Info :
*/
/***********************************************************************************/

void JallocReset()
{
	ULONG	l;
	JALLOCTYPE *jp = &jallocControl;

	for(l = 0; l < MAXJALLOCS; l++)								// initialise the block array
	{
		jp->blocks[l].inuse = 0;
		jp->blocks[l].address = 0;
		jp->blocks[l].size = 0;
	}

	jp->lastUsedStatic = -1;
	jp->lastUsedDynamic = MAXJALLOCS;
	jp->dynamicUsed = 0;
	jp->staticUsed = 0;
}


/**********************************************************************************/
void 
ShowJalloc()
{
	int n = 0,m = 0;
	int sizeUsed = 0,sizeEmpty = 0;
	ULONG      l;
	JALLOCTYPE *jp = &jallocControl;

	dprintf"\nJalloc block list...\n"));
	dprintf"-------------------------\n"));
    dprintf"%d blocks:\n", jp->lastUsedStatic+1));

	if (jp->lastUsedStatic == -1)
	return;

	for(l = 0; l <= jp->lastUsedStatic; l++)
	{
		if(jp->blocks[l].inuse)
		{
			n++;
			sizeUsed += jp->blocks[l].size;
		}
		else
		{
			m++;
			sizeEmpty += jp->blocks[l].size;
		}
		dprintf"    block #%d (%s): at (&%x), size %d status %d\n", (int)l, jp->blocks[l].name,(int)jp->blocks[l].address, (int)jp->blocks[l].size, (int)jp->blocks[l].inuse));
	}
	for(l = MAXJALLOCS-1; l >= jp->lastUsedDynamic; l--)
	{
		if(jp->blocks[l].inuse)
		{
			n++;
			sizeUsed += jp->blocks[l].size;
		}
		else
		{
			m++;
			sizeEmpty += jp->blocks[l].size;
		}
		dprintf"    block #%d (%s): at (&%x), size %d status %d\n", (int)l, jp->blocks[l].name,(int)jp->blocks[l].address, (int)jp->blocks[l].size, (int)jp->blocks[l].inuse));
	}
    dprintf"\n"));
	dprintf"%d blocks used; %d blocks empty\n",n,m));
	dprintf"%d bytes used; %d bytes wasted\n",sizeUsed,sizeEmpty));
}

/**********************************************************************************
	Function:	ListJallocBlocks
	Programmer:	David Swift
		
	ListJallocBlocks() displays a list of blocks that the given address lies in
	It should obviously only return one, except in the case of nasty memory overrides!
*/

void ListJallocBlocks(UBYTE **blk)
{
	int count;
	LONG	l, k;
	JALLOCTYPE *jp = &jallocControl;
	jallocentType *block;

	count = 0;

	dprintf"--- Checking jalloc lists for address 0x%08x ---\n", *blk));

	for(l = 0, block = jp->blocks; l <= jp->lastUsedStatic; l++, block++)
	{
		if((*blk >= block->address) && (*blk < (block->address + block->size)))
		{
			dprintf"Static: %08x (%+d) '%s'\n", block->address, *blk - block->address, block->name));
			count++;
		}
	}
	
	for(l = MAXJALLOCS-1, block = &jp->blocks[l]; l >= jp->lastUsedDynamic; l--, block--)
	{
		if((*blk >= block->address) && (*blk < (block->address + block->size)))
		{
			dprintf"Dynamic: %08x (%+d) '%s'\n", block->address, *blk - block->address, block->name));
			count++;
		}
	}

	dprintf"--- Found %d block%s of memory ---\n", count, (count==1)?"":"s"));
}


#ifndef PC_VERSION

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
u32 DMAGetSize(u32 ROMStart, u32 ROMEnd)
{
	u32 bankSize = ROMEnd - ROMStart;
	u32 x;
	u32 *temp;	
	u32 swapChar;
	u32 header[2];


	osInvalDCache(header, 8);
	osWritebackDCache(header, 8);


	osPiStartDma(&dmaIOMessageBuf, OS_MESG_PRI_NORMAL, OS_READ, ROMStart, header, 8, &dmaMessageQ);
	(void) osRecvMesg(&dmaMessageQ, NULL, OS_MESG_BLOCK);


	if(header[0] == 0x464C4132)	//FLA2 identifier
	{
//		dprintf"Compressed file found"));

		temp++;
		swapChar = header[1];	//temp holds the uncompressed file size (backwards!)
		
		x = (swapChar >> 24) & 0xff;
		x += ((swapChar >> 16) & 0xff) << 8;
		x += ((swapChar >> 8) & 0xff) << 16;
		x += ((swapChar) & 0xff) << 24;

	}
	else	//just download normaly
	{
		x = bankSize;
	}

//	dprintf"DMAGetSize:%d\n", x));
	return x;
}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void DMAMemory(char *ramPtr, u32 ROMStart, u32 ROMEnd)
{
	u32 bankSize = ROMEnd - ROMStart;
	u32 x;
	u32 *temp;	
	char *decompBuffer;
	u32 swapChar;
//	char *RAMAddress = *ramPtr;
	u32 header[2];

	osInvalDCache(header, 8);
	osWritebackDCache(header, 8);

	//dma down the first eight bytes of bank to determine if it is compressed
	osPiStartDma(&dmaIOMessageBuf, OS_MESG_PRI_NORMAL, OS_READ, ROMStart, header, 8, &dmaMessageQ);
	(void)osRecvMesg(&dmaMessageQ, NULL, OS_MESG_BLOCK);


	if(header[0] == 0x464C4132)	//FLA2 identifier
	{
//		dprintf"Compressed file found"));

		temp++;
		swapChar = header[1];	//temp holds the uncompressed file size (backwards!)
		
		x = (swapChar >> 24) & 0xff;
		x += ((swapChar >> 16) & 0xff) << 8;
		x += ((swapChar >> 8) & 0xff) << 16;
		x += ((swapChar) & 0xff) << 24;

		decompBuffer = (char *)JallocAlloc(bankSize, YES, "tempdecomp");
		osInvalDCache(decompBuffer, bankSize);
		osWritebackDCache(decompBuffer, bankSize);

		osPiStartDma(&dmaIOMessageBuf, OS_MESG_PRI_NORMAL, OS_READ,
				 ROMStart, decompBuffer,
				 bankSize,
				 &dmaMessageQ);

	//wait for completion
		(void) osRecvMesg(&dmaMessageQ, NULL, OS_MESG_BLOCK);


		osInvalDCache(ramPtr, x);
		osWritebackDCache(ramPtr, x);


		DecompressBuffer(decompBuffer+8, ramPtr);
		JallocFree((UBYTE**)&decompBuffer);

//		dprintf"DMAMemory:%d\n", x));

	}
	else	//just download normaly
	{

	//start download from rom
//		*ramPtr = (char *)JallocAlloc(bankSize, YES, "DMAalloc");

		osInvalDCache(ramPtr, bankSize);
		osWritebackDCache(ramPtr, bankSize);

		osPiStartDma(&dmaIOMessageBuf, OS_MESG_PRI_NORMAL, OS_READ,
				 ROMStart, ramPtr, bankSize, &dmaMessageQ);

	//wait for completion
		(void) osRecvMesg(&dmaMessageQ, NULL, OS_MESG_BLOCK);
//		dprintf"DMAMemory:%d\n", bankSize));
	}
}


#endif



/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void CheckJalloc()
{
	JALLOCTYPE *jp = &jallocControl;
	ULONG l,staticUsed = 0;

	for(l = 0; l <= jp->lastUsedStatic; l++)
	{
		if(jp->blocks[l].inuse)
			staticUsed += jp->blocks[l].size;

		if((l > 0) && (jp->blocks[l].address + jp->blocks[l].size != jp->blocks[l-1].address))
		{
			dprintf"Uh-oh - memory crobbed on frame %d\n",frameCount));
//			MusSetMasterVolume(MUSFLAG_EFFECTS,22000);
//			PlaySampleNot3D(FX_THUNDER,100,128,128);
			while(!(controllerdata[ActiveController].button & CONT_A));
		}
	}
	if(staticUsed != jp->staticUsed)
	{
		dprintf"Uh-oh - memory clodged on frame %d\n",frameCount));
//		MusSetMasterVolume(MUSFLAG_EFFECTS,22000);
//		PlaySampleNot3D(FX_THUNDER,100,128,128);
		while(!(controllerdata[ActiveController].button & CONT_A));
	}

	staticUsed = 0;
	for(l = MAXJALLOCS - 1;l >= jp->lastUsedDynamic; l--)
	{
		if(jp->blocks[l].inuse)
			staticUsed += jp->blocks[l].size;

		if((l < MAXJALLOCS-1) && (jp->blocks[l+1].address + jp->blocks[l+1].size != jp->blocks[l].address))
		{
			dprintf"Uh-oh - memory crobbed on frame %d\n",frameCount));
//			MusSetMasterVolume(MUSFLAG_EFFECTS,22000);
//			PlaySampleNot3D(FX_THUNDER,100,128,128);
			while(!(controllerdata[ActiveController].button & CONT_A));
		}
	}
	if(staticUsed != jp->dynamicUsed)
	{
		dprintf"Uh-oh - memory clodged on frame %d\n",frameCount));
//		MusSetMasterVolume(MUSFLAG_EFFECTS,22000);
//		PlaySampleNot3D(FX_THUNDER,100,128,128);
		while(!(controllerdata[ActiveController].button & CONT_A));
	}
}


#endif
/* end of #ifndef DONTUSEJALLOC block */