#include <ultra64.h>
#include "incs.h"

#define BABY_UID	255

int multiplayerMode;
long started = 0;


/*	--------------------------------------------------------------------------------
	Function		: UpdateCTF
	Purpose			: Do game mechanics for Capture the Frog multiplayer mode
	Parameters		: 
	Returns			: 
	Info			:
*/
int UpdateCTF( )
{
	unsigned long i;
	static TIMER endTimer, multiTimer;
	multiplayerMode = MULTIMODE_CTF;

	if( !started )
	{
		GTInit( &multiTimer, 90 );
		GTInit( &endTimer, 0 );
		started = 1;
	}

	if( endTimer.time )
	{
		GTUpdate( &endTimer, -1 );

		if( !endTimer.time )
		{
			StopDrawing("game over");
			FreeAllLists();

			InitLevel(player[0].worldNum,player[0].levelNum);
			gameState.mode = INGAME_MODE;

			started = frameCount = 0;
			fixedPos = fixedDir = 0;
			StartDrawing("game over");

			return FALSE;
		}
		return TRUE;
	}
	else // Is anyone still alive?
	{
		for( i=0; i<NUM_FROGS; i++ )
			if( frog[i]->action.healthPoints ) break;

		if( i==NUM_FROGS )
		{
			GTInit( &endTimer, 10 );
			fixedPos = fixedDir = 1;
		}
	}

//	GTUpdate( &multiTimer, -1 );

	if( !multiTimer.time )	// Check win conditions
	{
		GAMETILE *t;
		ENEMY *nme;
		short babyCount[4] = {0,0,0,0}, winner=-1, best=0;

		// If on a froggers area tile, check for babies on the tile
		for (t = firstTile; t != NULL; t = t->next)
			if( (t->state == TILESTATE_FROGGER1AREA) || (t->state == TILESTATE_FROGGER2AREA) || (t->state == TILESTATE_FROGGER3AREA) || (t->state == TILESTATE_FROGGER4AREA) )
				for( nme = enemyList.head.next; nme != &enemyList.head; nme = nme->next )
					if( (nme->flags & ENEMY_NEW_BABYFROG) && (t == nme->inTile) )
						babyCount[t->state-TILESTATE_FROGGER1AREA]++;

		for( i=0; i<NUM_FROGS; i++ )
			if( babyCount[i] > best )
			{
				best = babyCount[i];
				winner = i;
			}

		if( !best ) sprintf( timeTextOver->text, "No winner" );
		else sprintf( timeTextOver->text, "P%i won", winner );

		GTInit( &endTimer, 10 );
		return TRUE;
	}

	sprintf(timeTextOver->text,"%d",multiTimer.time);
	return TRUE;
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateRace
	Purpose			: Do game mechanics for mulitplayer race mode
	Parameters		: 
	Returns			: 
	Info			:
*/
int UpdateRace( )
{
	int i, j;

	multiplayerMode = MULTIMODE_RACE;

	for( i=0; i<NUM_FROGS; i++ )
	{
		if( !(frog[i]->action.safe.time) && (frameCount > 20))
		if(!IsPointVisible(&frog[i]->actor->pos))
		{
			KillMPFrog(i);

			for( j=0; j<NUM_FROGS; j++ )
				if (IsPointVisible(&frog[j]->actor->pos))
				{
					TeleportActorToTile(frog[i],currTile[j],i);
					destTile[i] = currTile[j];
				}
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function		: PickupBabyFrogMulti
	Purpose			: Player has collected baby frog in a multiplayer game
	Parameters		: ACTOR2
	Returns			: void
	Info			:
*/
void PickupBabyFrogMulti( ENEMY *baby, int pl )
{
	int i;
	ENEMY *nme;
	PATH *path = baby->path, *path2;

	for( i=0; i<numBabies; i++ ) if( babyList[i].baby == baby->nmeActor ) break;

	if( i==numBabies ) return;

	lastBabySaved = i;

	// For different multiplayer modes do different things:
		// CTF - transport baby back to frogger start point - assumes babies are all randommove nmes

	if( multiplayerMode == MULTIMODE_CTF )
	{
		// Find placeholder enemy that holds the baby respawn path for this frog
		for(nme = enemyList.head.next; nme != &enemyList.head; nme = nme->next )
			if( nme->uid-BABY_UID == pl ) break;

		if( nme == &enemyList.head ) return;

		path2 = nme->path;

		if( baby->flags & ENEMY_NEW_RANDOMMOVE )
		{
			CreateTeleportEffect( &baby->nmeActor->actor->pos, &baby->currNormal, babyList[i].fxColour[R], babyList[i].fxColour[G], babyList[i].fxColour[B] );
			// Baby teleports to froggers base
			path->nodes[0].worldTile = path2->nodes[0].worldTile;
			path->nodes[1].worldTile = path2->nodes[1].worldTile;
			path->nodes[2].worldTile = path2->nodes[2].worldTile;
			baby->isSnapping = 0;

			// Teleport frog back to base too
			CreateTeleportEffect( &frog[pl]->actor->pos, &currTile[pl]->normal, 255, 255, 255 );
			TeleportActorToTile( frog[pl], gTStart[pl], pl );
			destTile[pl] = gTStart[pl];
		}
	}
}

void KillMPFrog(int num)
{
	int i=0;
	GTInit( &frog[num]->action.stun, 2 );
	GTInit( &frog[num]->action.safe, 3 );
	
	if (frog[num]->action.healthPoints > 0)
	{
		i=num*3+(--frog[num]->action.healthPoints);
		if( sprHeart[i] ) sprHeart[i]->draw = 0;
	}
	else
	{
		frog[num]->action.deathBy = DEATHBY_NORMAL;
		player[num].frogState |= FROGSTATUS_ISDEAD;

		if (frog[num]->actor)
		{
			if (frog[num]->action.healthPoints > 0)
				frog[num]->draw = 1;
			else
				frog[num]->draw = 0;

		} 
	}
}
