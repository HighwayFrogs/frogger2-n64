#ifndef __MULTI_H
#define __MULTI_H


#define MULTIMODE_RACE	1
#define MULTIMODE_CTF	2


extern int multiplayerMode;


int UpdateCTF( );
int UpdateRace( );

void PickupBabyFrogMulti( ENEMY *baby, int pl );
void KillMPFrog(int num);


#endif