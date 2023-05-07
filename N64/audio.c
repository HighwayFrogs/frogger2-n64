/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: audio.c
	Programmer	: James Healey
	Date		: 20/04/99 14:42:36

----------------------------------------------------------------------------------------------- */


#define F3DEX_GBI_2
#define NNSCHED

#include <ultra64.h>
#include "incs.h"

#define MAX_AMBIENT_SFX		50
#define DEFAULT_SFX_DIST	500


AUDIOCONTROL audioCtrl;
int MAX_SFX_DIST = 500;
BOOL reverbOn = 0;

AMBIENT_SOUND_LIST ambientSoundList;
int numContinuousSamples = 0;

TUNE_DATA_BANK gameSongs[] = 
{
	0,0,"no track",													//	NOTRACK			0 
	_mBank1SegmentRomStart,_mBank1SegmentRomEnd,"garden toon",		//	TEST1_TRACK		1
	_mBank2SegmentRomStart,_mBank2SegmentRomEnd,"hub toon",			//	TEST2_TRACK		2
	_mBank3SegmentRomStart,_mBank3SegmentRomEnd,"lab toon",			//	TEST3_TRACK		3
	_mBank4SegmentRomStart,_mBank4SegmentRomEnd,"toyshop toon",		//	TEST4_TRACK		4
	_mBank5SegmentRomStart,_mBank5SegmentRomEnd,"city toon",		//	TEST5_TRACK		5
	_mBank6SegmentRomStart,_mBank6SegmentRomEnd,"halloween toon",	//	TEST6_TRACK		6
	_mBank7SegmentRomStart,_mBank7SegmentRomEnd,"space toon",		//	TEST7_TRACK		7
};


unsigned long sfxVol	= 100;
unsigned long musicVol	= 100;

short currentScriptEntry = 0;
int musresult;


/*	--------------------------------------------------------------------------------
	Function 	: SubAmbientSound
	Purpose 	: Remove ambient sound from playlist
	Parameters 	: Ambient sound to remove
	Returns 	: 
	Info 		:
*/
void SubAmbientSound(AMBIENT_SOUND *ambientSound)
{
	ambientSound->prev->next = ambientSound->next;
	ambientSound->next->prev = ambientSound->prev;
	ambientSoundList.numEntries--;
}


/*	--------------------------------------------------------------------------------
	Function 	: InitAmbientSoundList
	Purpose 	: Initialise the ambient list to empty.
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void InitAmbientSoundList()
{
	ambientSoundList.head.next = ambientSoundList.head.prev = &ambientSoundList.head;
	ambientSoundList.numEntries = 0;
}


/*	--------------------------------------------------------------------------------
	Function		: FreeAmbientSoundList
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
void FreeAmbientSoundList()
{
	AMBIENT_SOUND *cur,*next;

	// check if any elements in list
	if(!ambientSoundList.numEntries)
		return;

	// traverse enemy list and free elements
	for(cur = ambientSoundList.head.next; cur != &ambientSoundList.head; cur = next)
	{
		next = cur->next;

		SubAmbientSound(cur);
	}

	// initialise list for future use
	InitAmbientSoundList();
}


/*	--------------------------------------------------------------------------------
	Function 	: PrepareSong()
	Purpose 	: dma's down all the bits of a song and sets up the music structure
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void PrepareSong(char num,char slot)
{
	MusHandleSetReverb(audioCtrl.musicHandle[slot],0);
	MusHandleSetVolume(audioCtrl.musicHandle[slot],80);

	//stop existing audio track	
	if(num == audioCtrl.currentTrack[slot])
		return;

	audioCtrl.currentTrack[slot] = num;

	MusHandleStop(audioCtrl.musicHandle[slot], 1);
	while(MusHandleAsk(audioCtrl.musicHandle[slot]));

	if(num == NOTRACK)
		return;

	audioCtrl.musicBin[0] = audioCtrl.sfxPtr + (PBANK2_END - PBANK2_START);
	audioCtrl.musicBin[1] = audioCtrl.musicBin[0] + 20000;//audioCtrl.sfxPtr + (PBANK2_END - PBANK2_START);

	DMAMemory(audioCtrl.musicBin[slot], (u32)gameSongs[num].bankStart, (u32)gameSongs[num].bankEnd);

	audioCtrl.musicHandle[slot] = MusStartSong(audioCtrl.musicBin[slot]);
//	audioCtrl.musicHandle[slot] = MusBankStartSong(audioCtrl.musicPtr, audioCtrl.musicBin[slot]);
	while(!(MusHandleAsk(audioCtrl.musicHandle[slot])));

	MusHandleSetVolume(audioCtrl.musicHandle[slot],80);

	if((num == INTRO_TRACK) && (osTvType == 0))
	{
		MusHandleSetTempo(audioCtrl.musicHandle[slot], 0x6a);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: PlaySample
	Purpose 	: Play a sound sample at a given point with no distance attenuation
	Parameters 	: Index of sample, position, radius, volume, pitch
	Returns 	: 
	Info 		:
*/
int PlaySample(short num,VECTOR *pos,long radius,short tempVol,short pitch)
{
	int result;
	float dist,scale = 1, vol = tempVol;
	VECTOR tempVect;

	float att;
	long pan;
	VECTOR diff;

//	MusHandleStop(musresult, 0);

	if(pos)
	{
		// attenuation
		att = (radius) ? radius : DEFAULT_SFX_DIST;

		SubVector2D( &diff, pos, &frog[0]->actor->pos );
		// Volume attenuation - check also for radius != 0 and use instead of default
		dist = Magnitude( &diff );
		if( dist > att )
			return 0;
		
		vol *= (att-dist)/att;

		//work out pan
		dist = Aabs(atan2(diff.v[X], diff.v[Z]));
		pan = (255/PI) * FindShortestAngle(Aabs(frog[0]->actor->rot.v[Y]+PI/2),dist);
	}

//	vol *= 1.8;
	if(vol > 255)
		vol = 255;

	result = MusStartEffect2(num,(u8)vol,scale,0,-1);
	MusHandleSetFreqOffset(result,(float)(pitch-128)/10.0);
	MusHandleSetTempo(result,pitch);
  
	if(reverbOn)
		MusHandleSetReverb(result,30);

	return result;
}


/*	--------------------------------------------------------------------------------
	Function 	: PlaySampleRadius
	Purpose 	: Play a sample at a given point using spherical volume attenuation
	Parameters 	: Index of sample, position, volume, pitch, attenuation radius
	Returns 	: 
	Info 		:
*/

int PlaySampleRadius(short num, VECTOR *pos, short tempVol,short pitch,float radius)
{
	int result;
	float	dist, scale = 1, vol = tempVol;
	VECTOR	tempVect;

//work out volume
/*
	dist = Max(0,DistanceBetweenPoints(&currCamSource[0], pos) - radius);
	if(dist > MAX_SFX_DIST)
		return 0;

	scale = MAX_SFX_DIST - dist;
	scale = scale / MAX_SFX_DIST;

	vol *= scale;
*/
//work out pan
//	SubVector2D(&tempVect, pos, &currCamSource[0]);
//	dist = Aabs(atan2(tempVect.v[X], tempVect.v[Z]));
//	scale = FindShortestAngle(Aabs(frog[0]->actor->rot.v[Y]+PI/2),dist);

//	scale *= 255/PI;


	vol *= 1.8;
	if(vol > 255)
		vol = 255;

	result = MusStartEffect2(num, vol, scale, 0, -1);
	MusHandleSetFreqOffset(result,(float)(pitch-128)/10.0);
	MusHandleSetTempo(result,pitch);

	if(reverbOn)
		MusHandleSetReverb(result, 30);

	return result;
}


/*	--------------------------------------------------------------------------------
	Function 	: PlayContinuousSample
	Purpose 	: Loop sound effect???
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void PlayContinuousSample(SFX *sfx,short num,short vol,VECTOR *pos,short pitch)
{
	if(num == 0)
		return;
	if(sfx->handle)
		StopContinuousSample(sfx);		
	sfx->handle = PlaySampleRadius(num, pos, vol,pitch,sfx->radius);
	vol = (float)(vol)*1.8;
	if(vol > 255)
		vol = 255;

	sfx->volume = vol;
	sfx->pos = pos;
	sfx->sampleNum = num;
	sfx->pitch = pitch;

	if(sfx->handle)
		numContinuousSamples++;
}


/*	--------------------------------------------------------------------------------
	Function 	: StopContinuousSample
	Purpose 	: Interrupt a looping sample
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void StopContinuousSample(SFX *sfx)
{
	if(sfx->handle)
	{
		MusHandleStop(sfx->handle, 0);
		numContinuousSamples--;
	}
	sfx->actualVolume = sfx->volume;
	sfx->volume = 0;
	sfx->handle = 0;
  
}


/*	--------------------------------------------------------------------------------
	Function 	: UpdateContinuousSample
	Purpose 	: Recalculates distance attenuation for a continuous sample
	Parameters 	: Sound effect structure
	Returns 	: 
	Info 		: See also PlaySampleRadius
*/
void UpdateContinuousSample(SFX *sfx)
{
	float	dist, scale;
	VECTOR	tempVect;

	//if sfx is not playing return
	if((sfx->sampleNum == 0) || ((sfx->handle == 0) && (sfx->volume == 0)))
		return;

	if(sfx->radius)
	{
		dist = Max(0,DistanceBetweenPoints(&currCamSource[0], sfx->pos) - sfx->radius);
		if(dist > MAX_SFX_DIST)
		{
			if(sfx->handle)
			{
				MusHandleStop(sfx->handle, 0);
				numContinuousSamples--;
				sfx->handle = 0;
			}
			return;
		}
	}
	else
	{
		dist = DistanceBetweenPointsSquared(&currCamSource[0], sfx->pos);
		if(dist > MAX_SFX_DIST*MAX_SFX_DIST)
		{
			if(sfx->handle)
			{
				MusHandleStop(sfx->handle, 0);
				numContinuousSamples--;
				sfx->handle = 0;
			}
			return;
		}
		dist = sqrtf(dist);
	}

	scale = MAX_SFX_DIST - dist;
	scale = scale / MAX_SFX_DIST;

	sfx->actualVolume = sfx->volume * scale;

//	if(enemy->sfx[num].actualVolume == 0)
//	{
//		MusHandleStop(enemy->sfx[num].handle, 0);
//		enemy->sfx[num].handle = 0;
//		return;
//	}


//work out pan
//	SubVector2D(&tempVect, sfx->pos, &frog[0]->actor->pos);
//	dist = Aabs(atan2(tempVect.v[X], tempVect.v[Z]));
//	scale = FindShortestAngle(Aabs(frog[0]->actor->rot.v[Y]+PI/2),dist);

//	scale *= 255/PI;

	if(sfx->handle == 0)
	{
		sfx->handle = MusStartEffect2(sfx->sampleNum, sfx->actualVolume, scale, 0, -1);
		if(reverbOn)
			MusHandleSetReverb(sfx->handle, 30);
		numContinuousSamples++;
	}
	else
		MusHandleSetVolume(sfx->handle, sfx->actualVolume);

	MusHandleSetPan(sfx->handle, scale);
	MusHandleSetFreqOffset(sfx->handle, (float)(sfx->pitch-128)/10.0);
	MusHandleSetTempo(sfx->handle, sfx->pitch);
}


/*	--------------------------------------------------------------------------------
	Function 	: ClearAmbientSfx
	Purpose 	: Stop all ambients
	Parameters 	: 
	Returns 	: 
	Info 		: Does not actually delete anything, just stops them
*/
void ClearAmbientSfx()
{
	AMBIENT_SOUND *ambientSound;

	for(ambientSound = ambientSoundList.head.next;ambientSound != &ambientSoundList.head;ambientSound = ambientSound->next)
	{
		if(ambientSound->sfx.handle)
			StopContinuousSample(&ambientSound->sfx);

		ambientSound->sfx.pos = &zero;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: KillAmbientSfx
	Purpose 	: Deletes all ambient sounds from the list, after stopping them
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void KillAmbientSfx()
{
	AMBIENT_SOUND *ambientSound, *tmp;

	for(ambientSound = ambientSoundList.head.next;ambientSound != &ambientSoundList.head; )
	{
		if( ambientSound != NULL )
		{
			if(ambientSound->sfx.handle)
				StopContinuousSample(&ambientSound->sfx);

			ambientSound->sfx.pos = &zero;
			SubAmbientSound( ambientSound );

			tmp = ambientSound->next;
			JallocFree( (UBYTE **)&ambientSound );
			ambientSound = tmp;
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: UpdateAmbientSounds
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void UpdateAmbientSounds()
{
	AMBIENT_SOUND *amb,*amb2;
	VECTOR *pos;

	// Silence ambients if paused or level over?
	if((gameState.mode == PAUSE_MODE))	// || levelIsOver )
		return;

	// Update each ambient in turn
	for( amb = ambientSoundList.head.next; amb != &ambientSoundList.head; amb = amb2 )
	{
		amb2 = amb->next;

		// Timeout, so play a new sound
		if( actFrameCount < amb->counter )
			continue;

		// If it is attached to a platform, make ambient follow that platform
		if( amb->follow )
			SetVector( &amb->pos, &amb->follow->pos );

		// If sound doesn't have a source
		if( !MagnitudeSquared(&amb->pos) )
			pos = NULL;
		else
			pos = &amb->pos;

		PlaySample( amb->num, &amb->pos, amb->radius, amb->volume, amb->pitch );

		// Freq and randFreq are cunningly pre-multiplied by 60
		amb->counter = actFrameCount + amb->freq + ((amb->randFreq)?Random(amb->randFreq):0);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: PlaySampleNot3D
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
int PlaySampleNot3D(int num,UBYTE vol,UBYTE pan,UBYTE pitch)
{
	int handle;

//	handle = MusBankStartEffect2(audioCtrl.sfxPtr, num,vol,pan, 0, -1);
	handle = MusStartEffect2(num,vol,pan, 0, -1);
	MusHandleSetFreqOffset(handle,(float)(pitch-128)/10.0);
	MusHandleSetTempo(handle,pitch);

	return handle;
}


/*	--------------------------------------------------------------------------------
	Function 	: InitSFXScript
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
/*void InitSFXScript(SFX_SCRIPT *script)
{
	currentScriptEntry = 0;
	activeSFXScript = script;
}
*/

/*	--------------------------------------------------------------------------------
	Function 	: ScriptTriggerSound
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
/*BOOL ScriptTriggerSound(short second, short frame)
{
	float temp;
	short targetFrame;

	if(activeSFXScript == &introSfxScript[0])
	{
		if(second+4 == frameCount)
			return TRUE;
	}
	else
	{
		targetFrame = second * 20;
		temp = (float)frame * 0.8;
		targetFrame += temp;

		if(frameCount == targetFrame)
			return TRUE;
	}
	return FALSE;
}
*/

/*	--------------------------------------------------------------------------------
	Function 	: ProcessSFXScript
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
/*void ProcessSFXScript()
{
	SFX_SCRIPT *script = activeSFXScript;
	char reverb;

	if(script == NULL)
		return;

	while(script->triggerSecond >= 0)
	{
 		if((script->stopSecond != -1) || levelIsOver )
		{
			if( levelIsOver || (ScriptTriggerSound(script->stopSecond, script->stopFrame)))
			{
				MusHandleStop(script->handle, 0);
			}		
		}
		script++;
	}

	if( levelIsOver ) return;

	script = activeSFXScript;

	while(script->triggerSecond >= 0)
	{
		if(ScriptTriggerSound(script->triggerSecond, script->triggerFrame))
		{
			reverb = script->handle;
			script->handle = PlaySampleNot3D(script->sfxNum, script->vol, 128,script->pan ? script->pan : 128);		
			if(script->sfxNum == FX_KLOSET_CALL)
				MusHandleSetReverb(script->handle, 60);
				
			currentScriptEntry++;
			if(script->triggerSecond == -1)
			{
				activeSFXScript = NULL;
				return;
			}
 		}
		script++;
	}	
}
*/


/*	--------------------------------------------------------------------------------
	Function		: PlayActorBasedSample
	Purpose			: play a sample based on actor rotation, rather than camera
	Parameters		: short,ACTOR *,short,short
	Returns			: int
	Info			: 
*/
int PlayActorBasedSample(short num,ACTOR *act,short tempVol,short pitch)
{
	int result;
	float	dist, scale = 1, vol = tempVol;
	VECTOR	tempVect;

//work out volume
	dist = DistanceBetweenPointsSquared(&currCamSource[0],&act->pos);
	if(dist > MAX_SFX_DIST*MAX_SFX_DIST)
		return 0;
	
	dist = sqrtf(dist);
	scale = MAX_SFX_DIST - dist;
	scale = scale / MAX_SFX_DIST;

	vol *= scale;

//work out pan
	SubVector2D(&tempVect, &act->pos, &currCamSource[0]);
	dist = Aabs(atan2(tempVect.v[X], tempVect.v[Z]));
	scale = FindShortestAngle(Aabs(act->rot.v[Y]+PI/2),dist);

	scale *= 255/PI;

	vol *= 1.8;
	if(vol > 255)
		vol = 255;

	result = MusStartEffect2(num, (u8)vol, scale, 0, -1);
	MusHandleSetFreqOffset(result,(float)(pitch-128)/10.0);
	MusHandleSetTempo(result,pitch);
  
	if(reverbOn)
		MusHandleSetReverb(result,30);
	return result;
}


/*	--------------------------------------------------------------------------------
	Function		: PrepareSongForLevel
	Purpose			: loads and starts playback of song for specified level
	Parameters		: short,short
	Returns			: void
	Info			: 
*/
void PrepareSongForLevel(short worldID,short levelID)
{
	int theToon = TEST4_TRACK;

	// just play the first track for now !!!!
	MusSetMasterVolume(MUSFLAG_SONGS,32000);

	PrepareSong(theToon,0);
}


/*	--------------------------------------------------------------------------------
	Function 	: AddAmbientSound
	Purpose 	: Create an ambient sound
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
AMBIENT_SOUND *AddAmbientSound(int num, VECTOR *pos, long radius, int vol, short pitch, float freq, float randFreq, ACTOR *follow )
{
	AMBIENT_SOUND *ptr = &ambientSoundList.head;
	AMBIENT_SOUND *ambientSound = (AMBIENT_SOUND *)JallocAlloc(sizeof(AMBIENT_SOUND),YES,"AmbSnd");
	
	if( pos ) SetVector( &ambientSound->pos, pos );
	if( follow ) ambientSound->follow = follow;

	ambientSound->volume = vol;
	ambientSound->num = num;
	ambientSound->pitch = pitch;
	ambientSound->radius = radius;

	ambientSound->freq = freq*60;
	ambientSound->randFreq = randFreq*60;

	ambientSound->prev = ptr;
	ambientSound->next = ptr->next;
	ptr->next->prev = ambientSound;
	ptr->next = ambientSound;
	ambientSoundList.numEntries++;

	return ambientSound;
}
