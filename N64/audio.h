/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

----------------------------------------------------------------------------------------------- */

#ifndef AUDIO_H_INCLUDED
#define AUDIO_H_INCLUDED


typedef struct
{

	u8	*sfxPtr;			//pointer table for level sfx
	u8	*sfxWbk;			//wave data for each level   

	u8	*musicPtr;			//pointer table for generic sfx
	u8	*musicWbk;			//wave data for generic sfx  

	u8	*musicBin[2];		//music data for current tune
	u8	*bfx;				//pointer to sfx data

	int	musicHandle[2];		//music handle
	int currentTrack[2];	//current track number

} AUDIOCONTROL;


typedef struct
{
	u8		*bankStart;
	u8		*bankEnd;
	char	tuneName[16];

} TUNE_DATA_BANK;


typedef struct
{
	s16	triggerSecond,triggerFrame;
	s16	stopSecond,stopFrame;
	short	sfxNum;
	char	vol,pan;
	int		handle;
} SFX_SCRIPT;


typedef struct TAG_AMBIENT_SOUND
{
	struct TAG_AMBIENT_SOUND *next,*prev;
	SFX			sfx;

	VECTOR		pos;
	ACTOR		*follow;

	int			num;

	short		volume;
	short		pitch;
	long		radius;

	long		freq;
	long		randFreq;
	long		counter;

}AMBIENT_SOUND;


typedef struct
{
	AMBIENT_SOUND head;
	int numEntries;
}AMBIENT_SOUND_LIST;


#define MAX_AMBIENT_SFX	50


extern AMBIENT_SOUND_LIST	ambientSoundList;
extern AUDIOCONTROL audioCtrl;
extern TUNE_DATA_BANK gameSongs[];
extern int MAX_SFX_DIST;
extern int numberOfEnemies;
extern BOOL reverbOn;


extern unsigned long sfxVol;
extern unsigned long musicVol;

/*
extern SFX_SCRIPT	introSfxScript[];
extern SFX_SCRIPT	activeSfxScript[];
*/

enum
{
	NOTRACK,
	TEST1_TRACK,
	TEST2_TRACK,
	TEST3_TRACK,
	TEST4_TRACK,
	TEST5_TRACK,
	TEST6_TRACK,
	TEST7_TRACK,
	INTRO_TRACK,
	NUM_TRACKS,
};

#define GEN_FROG_HOP	51
#define GEN_SUPER_HOP	39
#define GEN_FROG_HURT	192
#define GEN_FROG_DEATH	193
#define GEN_FROG_TONGUE	136
#define GEN_CLOCK_TOCK	135

extern int musresult;

void UpdateContinuousSample(SFX *sfx);
void DmaRomToRam(char *src, char *dest, int len);
void ReInitMusicDriver(u8 *pointer, u8 *tune, u8 *wave);
//void SetMaxSfxDist(int dist);
void AddAmbientSfx(int num, int vol, int pan);
void ClearAmbientSfx();
void KillAmbientSfx();
void StopContinuousSample(SFX *sfx);
void PlayContinuousSample(SFX *sfx,short num, short vol, VECTOR *,short pitch);
void AddAmbientSfxAtPoint(int num, int vol,VECTOR *pos,short pitch,short freq,short randFreq,short onTime,short platTag,short tag,float radius);

void SubAmbientSound(AMBIENT_SOUND *ambientSound);
//AMBIENT_SOUND *FindTaggedAmbientSound(AMBIENT_SOUND *startAmbientSound, int tag);
int PlaySampleNot3D(int num,UBYTE vol,UBYTE pan,UBYTE pitch);
int PlaySampleRadius(short num, VECTOR *pos, short vol,short pitch,float radius);

void InitSFXScript(SFX_SCRIPT *script);
BOOL ScriptTriggerSound(short second, short frame);
void ProcessSFXScript();

void InitMusicDriver(void);

void PrepareSongForLevel(short worldID,short levelID);
void PrepareSong(char num,char slot);


void InitSampleList();
void FreeSampleList();

void InitAmbientSoundList();
void FreeAmbientSoundList();

int PlaySample(short num,VECTOR *pos,long radius,short volume,short pitch);
AMBIENT_SOUND *AddAmbientSound(int num,VECTOR *pos,long radius,int vol,short pitch,float freq,float randFreq,ACTOR *follow);
void UpdateAmbientSounds();


#endif