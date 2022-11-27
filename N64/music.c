/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: music.c
	Programmer	: 
	Date		: 29/04/99 11:13:14

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>

#include "incs.h"

#define USE_NEWAUDIO 1

musConfig musicPlayer;

#ifdef USE_NEWAUDIO

static void NnSchedInstall(void);
static void NnSchedWaitFrame(void);
static void NnSchedDoTask(musTask *task);

/* music library scheduler callback structure */
static musSched nn_mus_sched = 
{ 
   NnSchedInstall, NnSchedWaitFrame, NnSchedDoTask 
};
#endif

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
  [GLOBAL FUNCTION]
  InitMusicDriver()

  [Explantion]
  Download ROM files and initialise the music player.

  [Return value]
  NONE
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

int musicMemoryUsed = 0;
void InitMusicDriver(void)
{
#ifdef USE_NEWAUDIO

	audioCtrl.bfx = audio_memory + AUDIO_HEAP_SIZE;
	DMAMemory(audioCtrl.bfx, (u32)SBANK_START, (u32)SBANK_END);

	audioCtrl.musicPtr = audioCtrl.bfx + (u32)SBANK_END - (u32)SBANK_START;
	DMAMemory(audioCtrl.musicPtr, (u32)PBANK_START, (u32)PBANK_END);

	audioCtrl.sfxPtr = audioCtrl.musicPtr + (u32)PBANK_END - (u32)PBANK_START;
	DMAMemory(audioCtrl.sfxPtr, (u32)PBANK2_START, (u32)PBANK2_END);

/*
	audioCtrl.bfk = audioCtrl.sfxPtr + (u32)SBANK_END - (u32)SBANK_START;
	DMAMemory(audioCtrl.bfk, (u32)SBANK_START, (u32)SBANK_END);
*/

	musicPlayer.control_flag	= 0;
	musicPlayer.channels		= 24;
	musicPlayer.sched			= &sched;
	musicPlayer.thread_priority	= 40;
	
	musicPlayer.heap			= audio_memory;
	musicPlayer.heap_length		= AUDIO_HEAP_SIZE;

	musicPlayer.fifo_length		= 128;
	
	musicPlayer.ptr				= audioCtrl.musicPtr;
	musicPlayer.wbk				= WBANK_START;

	musicPlayer.default_fxbank	= audioCtrl.bfx;

	musicPlayer.syn_output_rate	= 22050;
	musicPlayer.syn_updates		= 256;//512;
	musicPlayer.syn_rsp_cmds	= 2048;

	musicPlayer.syn_retraceCount = 1;
	musicPlayer.syn_num_dma_bufs = 30;
	musicPlayer.syn_dma_buf_size = 0x500;

	dprintf"Initialising sound\n"));
	MusSetScheduler(&nn_mus_sched);
	musicMemoryUsed = MusInitialize(&musicPlayer);
	dprintf"music memory used %d\n",musicMemoryUsed));

	MusSetMasterVolume(MUSFLAG_EFFECTS,32000);
  	MusSetMasterVolume(MUSFLAG_SONGS,32000);
	audioCtrl.currentTrack[0] = 0;
	MusPtrBankInitialize(audioCtrl.sfxPtr,WBANK2_START); 
	MusFxBankSetPtrBank(audioCtrl.bfx, audioCtrl.sfxPtr);

#else

	audioCtrl.musicPtr = audio_memory + AUDIO_HEAP_SIZE;
	DMAMemory(audioCtrl.musicPtr, (u32)PBANK_START, (u32)PBANK_END);
	
	audioCtrl.sfxPtr = audioCtrl.musicPtr + (u32)PBANK_END - (u32)PBANK_START;
	DMAMemory(audioCtrl.sfxPtr, (u32)PBANK2_START, (u32)PBANK2_END);


	musicPlayer.control_flag = 0;//MUSCONTROL_RAM;
	musicPlayer.channels		= 24;
	musicPlayer.sched		= &sched;
 	musicPlayer.thread_priority	= musicPriority;//116;//119;
	
	musicPlayer.heap		= audio_memory;
	musicPlayer.heap_length	= AUDIO_HEAP_SIZE;
	
	musicPlayer.ptr			= audioCtrl.sfxPtr;//_memory + AUDIO_HEAP_SIZE;//pointer_buf;
	musicPlayer.wbk			= WBANK2_START;

	musicPlayer.fxs			= libmus_sfx_list;
	musicPlayer.priority	= libmus_priority_list;

	musicPlayer.syn_output_rate	= 22050;
//	musicPlayer.syn_updates	= 512 * 2;
	musicPlayer.syn_updates	= 256;
//	musicPlayer.syn_rsp_cmds	= 4096 * 2;// * 2;
	musicPlayer.syn_rsp_cmds	= 2048;// * 2;

	musicPlayer.syn_retraceCount = 1;
	musicPlayer.syn_num_dma_bufs = 36;//40 * 2;//36;
//	musicPlayer.syn_num_dma_bufs = 40;//40 * 2;//36;
	musicPlayer.syn_dma_buf_size = 0x1000;//800;
//	musicPlayer.syn_dma_buf_size = 0x600;//800;

	musicMemoryUsed = MusInitialize(&musicPlayer);

	MusBankInitialize(audioCtrl.musicPtr, WBANK_START);
//	MusBankInitialize(audioCtrl.sfxPtr, WBANK2_START);
#endif
}


/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
  [INTERNAL FUNCTION]
  DmaRomToRam(src, dest, len)

  [Explantion]
  Download an area of ROM to RAM. Note this function limits the size of any DMA
  generated to 16k so as to cause clashes with audio DMAs.

  [Parameters]
  src		address of ROM source
  dest		address of RAM destination 
  len		number of bytes to transfer

  [Return value]
  NONE
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

// avoid large DMAs these can cause problems with the music DMAs...
#define DMA_LEN 16384

void DmaRomToRam(char *src, char *dest, int len)
{
    OSIoMesg dmaIoMesgBuf;
    OSMesg dummyMesg;
    int length;
    
    while (len)
    {
      if (len>DMA_LEN)
        length = DMA_LEN;
      else
        length = len;

      osInvalDCache(dest, length);
      osPiStartDma(&dmaIoMesgBuf, OS_MESG_PRI_NORMAL, OS_READ,
                   (u32)src, dest, length, &dmaMessageQ);
      (void) osRecvMesg(&dmaMessageQ, &dummyMesg, OS_MESG_BLOCK);
      src+=length;
      dest+=length;
      len-=length;
    }
}





#ifdef USE_NEWAUDIO
/* scheduler callback functions workspace */
#define QUEUE_SIZE   4

static NNScClient		nnclient;
static OSMesgQueue		nnframe_queue;
static OSMesgQueue		nntask_queue;
static OSMesg			nnframe_messages[QUEUE_SIZE];
static OSMesg			nntask_messages[QUEUE_SIZE];

static void NnSchedInstall(void)
{
	/* create message queues for WaitFrame and DoTask functions */
	osCreateMesgQueue(&nnframe_queue, &nnframe_messages[0], QUEUE_SIZE);
	osCreateMesgQueue(&nntask_queue, &nntask_messages[0], QUEUE_SIZE);
	/* add waitframe client */
	nnScAddClient(&sched, &nnclient, &nnframe_queue);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
  [MUSIC LIBRARY CALLBACK FUNCTION]
  NnSchedWaitFrame()

  [Explantion]
  Wait for a retrace message for the music library. This function is called by the
  audio threads inifinite loop to wait for a retrace message. The 'syn_retraceCount'
  parameter of the musConfig structure must contain the number of retraces per
  message received.
  
  [Return value]
  NONE
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

static void NnSchedWaitFrame(void)
{
   NNScMsg *message;

	do
	{
      osRecvMesg(&nnframe_queue, (OSMesg *)&message, OS_MESG_BLOCK);
		osRecvMesg(&nnframe_queue, NULL, OS_MESG_NOBLOCK);	/* bin any missed syncs! */
	} while (*message!=NN_SC_RETRACE_MSG);
//	} while (*message!=NN_SC_FRAME_MSG);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
  [MUSIC LIBRARY CALLBACK FUNCTION]
  NnSchedDoTask()

  [Parameters]
  task      address of the music library task descriptor structure

  [Explantion]
  Process a audio task for the music library. This function is called by the audio
  threads inifinite loop to generate an audio task and wait for its completion.

  The structure passed is defined in "libmus.h" and is defined as:

         typedef struct
         {
            u64   *data;
            int   data_size;
            u64   *ucode;
            u64   *ucode_data;
         } musTask;

  The 'data' and 'data_size' components of the structure relate to the command list
  to be processed. 'data' is the address of the command list and 'data_size' is the
  number of commands it contains.

  The 'ucode' and 'ucode_data' components of the structure relate to the microcode
  to be used. 'ucode' is the address of the microcode Text section, 'ucode_data' is
  the address of the microcode Data section.
  
  [Return value]
  NONE
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

static void NnSchedDoTask(musTask *task)
{
	NNScTask t;
   NNScMsg message;

   /* copy settings from musTask structure */
   t.list.t.data_ptr    = task->data;
   t.list.t.data_size   = task->data_size;
   t.list.t.ucode = (u64 *) task->ucode;
   t.list.t.ucode_data = (u64 *) task->ucode_data;

   /* fill in the rest of task data */
   t.next      = 0;
   t.msgQ      = &nntask_queue;
//   t.msg       = 0;    
   t.msg       = &message;    
   t.list.t.type  = M_AUDTASK;
   t.list.t.ucode_boot = (u64 *)rspbootTextStart;
   t.list.t.ucode_boot_size = ((int) rspbootTextEnd - (int) rspbootTextStart);
   t.list.t.flags  = 0;
   t.list.t.ucode_size = 4096;
   t.list.t.ucode_data_size = SP_UCODE_DATA_SIZE;
   t.list.t.dram_stack = (u64 *) NULL;
   t.list.t.dram_stack_size = 0;
   t.list.t.output_buff = (u64 *) NULL;
   t.list.t.output_buff_size = 0;
   t.list.t.yield_data_ptr = NULL;
   t.list.t.yield_data_size = 0;

   /* start task */
   osSendMesg(nnScGetAudioMQ(&sched), (OSMesg) &t, OS_MESG_BLOCK);    
   /* wait for task to finish */
	osRecvMesg(&nntask_queue, NULL, OS_MESG_BLOCK);
}

#endif




/* end of file */
