/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: rumblepak.cpp
	Programmer	: Andy Eder
	Date		: 23/11/99

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>
#include "incs.h"

#define RUMBLE_MIN_FREQ  10
#define RUMBLE_MAX_FREQ  110

char anyRumblePresent = NO;

RUMBLE rumble[4];
RUMBLE delayRumble[4];

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
    Returns 	: 
	Info 		:
*/

void InitRumble()
{
	int i;
	for(i=0; i<4; i++)
	{
		rumble[i].stopCount= 0;
		rumble[i].freq = -1.0;
		rumble[i].rumbleCount = 0;
		if(rumblePresent[i])
		{
			anyRumblePresent = YES;
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
    Returns 	: 
	Info 		:  called in controller thread
*/
void UpdateRumble()
{
	int i, res;

	for(i=0; i<NUM_FROGS; i++)
	{
		if(rumblePresent[i])
		{
			if(delayRumble[i].lifetime && delayRumble[i].delay-- == 0)
			{
				StartRumble(delayRumble[i].freq, delayRumble[i].inertia, delayRumble[i].lifetime, i);
				delayRumble[i].lifetime=0;
			}
			if(rumble[i].lifetime > 0)
			{
			
				rumble[i].lifetime--;
				rumble[i].rumbleCount += rumble[i].freq;
				rumble[i].freq *= rumble[i].inertia;

				if(rumble[i].rumbleCount >= 100.0)
				{
				 	// start rumble
					if(!rumble[i].rumbleOn) // probably remove this if osMotorStart is cool if motor is already on
					{
						// try at most twice to start motor
  						if(res=osMotorStart(&pfsRumble[i]))
  						{
  							res=osMotorStart(&pfsRumble[i]);
  						}
  
  						rumble[i].rumbleOn = (res == NULL);
  					}
  				 	rumble[i].rumbleCount -= 100.0;
				}
				else
				{
					// stop rumble
				  	if(rumble[i].rumbleOn)
				  	{
				 //		if(res = osMotorStop(&pfsRumble[i]))
							res = osMotorStop(&pfsRumble[i]);
///							dprintf"stopRumble1\n"));
				  	}							
					rumble[i].rumbleOn = (res != NULL);
				}
				
			}
			else if(rumble[i].stopCount > 0)
			{
				// must stop at least 3 times on different frames to make sure.
				// stop rumble
				res = osMotorStop(&pfsRumble[i]);
				rumble[i].rumbleOn = (res != NULL);
///				dprintf"stopRumble2\n"));


				if(rumble[i].rumbleCount > 100.0)
					rumble[i].rumbleCount -= 100.0;

				rumble[i].stopCount--;
				if(rumble[i].stopCount == 0)
				{
					rumble[i].freq = -1.0;
					rumble[i].rumbleCount = 0.0;
				}
			}
		}				
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	:  freq 10->100  inertia 0.0->2.0  life is lifetime  num is pad to rumble
    Returns 	: 
	Info 		:
*/
//#define DEBUG_RUMBLE
void StartRumble(float freq, float inertia, short life, short num)
{

	if(!rumblePresent[num] || rumble[num].lifetime>0)
		return;

	#ifdef DEBUG_RUMBLE
	dprintf"rumble %s\n", string));
	#endif

	rumble[num].stopCount = 3;
	rumble[num].lifetime = life;
	rumble[num].inertia = Bound(0, inertia, 2);
	rumble[num].freq = freq; 
		
//	dprintf"rumbleCount=%f, lifetime=%i, freq=%f\n",rumble[num].rumbleCount	, rumble[num].lifetime, rumble[num].freq));
}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
    Returns 	: 
	Info 		:
*/
void StopAllRumbles()
{
	int i;
	for(i=0; i<NUM_FROGS; i++)
	{
		if(rumblePresent[i])
		{
			if(rumble[i].lifetime > 0)
				rumble[i].lifetime = 0;	
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	:  freq 10->100  inertia 0.0->2.0  life is lifetime  num is pad to rumble
    Returns 	: 
	Info 		:
*/
void StartDelayedRumble(float freq, float inertia, short life, short num, char* string, short delay)
{

	if(!rumblePresent[num] || rumble[num].lifetime>0)
		return;

	#ifdef DEBUG_RUMBLE
	dprintf"rumble %s\n", string));
	#endif

	delayRumble[num].stopCount = 3;
	delayRumble[num].lifetime = life;
	delayRumble[num].inertia = Bound(0, inertia, 2);
	delayRumble[num].freq = freq; 
	delayRumble[num].delay = delay;
//	dprintf"rumbleCount=%f, lifetime=%i, freq=%f\n",rumble[num].rumbleCount	, rumble[num].lifetime, rumble[num].freq));
}

