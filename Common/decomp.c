/*******************************************************************************

   ISL Flatpacker II decompression                             R.Hackett 1997

   (c) Copyright Interactive Studios Ltd.

*******************************************************************************/

#include <ultra64.h>

#include "incs.h"
#include "decomp.h"



#define	HISTORY_SIZE		4096
#define MASK_HISTORY		(HISTORY_SIZE-1)
#define MASK_UPPER			(0xF0)
#define MASK_LOWER			(0x0F)
#define SHIFT_UPPER			16
#define LSR_UPPER			4
#define MAX_COMP_LEN		17


static unsigned char	*outputBufPtr = NULL;
static int				outputBufLen = 0;



static UBYTE	LZhistory[HISTORY_SIZE];
static WORD		LZhistoryOff;



#ifndef PC_VERSION

void memset(char *source, char val, int size)
{
	int x;

	for(x = 0; x < size; x++)
		source[x] = val;

}

#endif


static void DecompressOutputByte(UBYTE data)
{
	*outputBufPtr++ = data;
	outputBufLen++;
	LZhistory[LZhistoryOff] = data;
	LZhistoryOff = (LZhistoryOff+1) & MASK_HISTORY;
}


/*	--------------------------------------------------------------------------------
	FUNCTION:	DecompressBuffer
	PURPOSE:	Decompress data from input buffer into output buffer
	PARAMETERS:	Input buffer start, Output buffer start
	RETURNS:	Length of uncompressed data
	INFO:
*/

int DecompressBuffer(UBYTE *inBuf, UBYTE *outBuf)
{
	WORD	tag = 0, count = 0, offset = 0, loop = 0;

	outputBufPtr = outBuf;								// Initialise output
	outputBufLen = 0;
	
	LZhistoryOff = 0;									// Clear history
	memset(LZhistory, 0, HISTORY_SIZE);

	while(1)
	{
		tag = *inBuf++;
		for(loop=0; loop!=8; loop++)
		{
			if (tag & 0x80)
			{
				if ((count=*inBuf++) == 0)
				{
					return outputBufLen;				// Finished now
				}
				else
				{										// Copy from history
					offset = HISTORY_SIZE-(((MASK_UPPER & count)*SHIFT_UPPER)+(*inBuf++));
					count &= MASK_LOWER;
					count += 2;
					while (count!=0)
					{
						DecompressOutputByte(LZhistory[(LZhistoryOff+offset) & MASK_HISTORY]);
						count--;
					}
				}
			}
			else
			{
				DecompressOutputByte(*inBuf++);			// Copy data byte
			}
			tag += tag;
		}
	}
	return outputBufLen;
}





