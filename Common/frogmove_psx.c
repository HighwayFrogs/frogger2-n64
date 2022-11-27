
/*	--------------------------------------------------------------------------------
	Function		: UpdateFroggerPos()
	Purpose			: updates Frogger's position on game world
	Parameters		: none
	Returns			: none
	Info			:
*/

void UpdateFroggerPos(long pl)
{
	float x,y,z;
	FX_RIPPLE *rip;
	VECTOR effectPos;
	PLANE2 ground;
	float dist;
	
	if(player[pl].frogState & FROGSTATUS_ISDEAD)
	{
		KillFrog(frog[pl],pl);
		return;
	}

	if(player[pl].frogState & FROGSTATUS_ISTELEPORTING)
	{
		// frog is in state of teleportation
		frog[pl]->action.isTeleporting--;
		if(!frog[pl]->action.isTeleporting)
		{
			TeleportActorToTile(frog[pl],currTile[pl]->teleportTo,pl);
			fadeDir		= FADE_IN;
			fadeOut		= 255;
			fadeStep	= 8;
			doScreenFade = 63;
		}
		
		return;
	}

	
	// update frog tongue
	UpdateFrogTongue();

	if(player[pl].frogState & FROGSTATUS_ISFALLINGTOGROUND)
	{
		// the frog is falling to the ground
		SetVector(&ground.point,&currTile[pl]->centre);
		SetVector(&ground.normal,&currTile[pl]->normal);

		// update frog position in relation to ground
//		frog[pl]->actor->vel.v[X] *= 0.95F;
		frog[pl]->actor->vel.v[Y] += -1.1F;
//		frog[pl]->actor->vel.v[Z] *= 0.95F;

		AddToVector(&frog[pl]->actor->pos,&frog[pl]->actor->vel);
		ground.J = -DotProduct(&ground.point,&ground.normal);
		dist = -(DotProduct(&frog[pl]->actor->pos,&ground.normal) + ground.J);						

		// check if frog has hit (or passed through) the ground plane
		if(dist > 0)
		{
			SetVector(&frog[pl]->actor->pos,&ground.point);
			CalcBounce(&frog[pl]->actor->vel,&ground.normal);
//			frog[pl]->actor->vel.v[X] *= 0.75F;
			frog[pl]->actor->vel.v[Y] *= 0.75F;
//			frog[pl]->actor->vel.v[Z] *= 0.75F;

			CreateAndAddFXSmoke(SMOKE_TYPE_NORMAL,&ground.point,128,0,0.5,15);

			if(MagnitudeSquared(&frog[pl]->actor->vel) < 5.0F)
			{
				// stop the frog from bouncing - set to standing
				SetVector(&frog[pl]->actor->pos,&ground.point);
				player[pl].frogState &= ~FROGSTATUS_ISFALLINGTOGROUND;
				player[pl].frogState &= ~FROGSTATUS_ISWANTINGU;
				player[pl].frogState &= ~FROGSTATUS_ISWANTINGD;
				player[pl].frogState &= ~FROGSTATUS_ISWANTINGL;
				player[pl].frogState &= ~FROGSTATUS_ISWANTINGR;
				player[pl].frogState &= ~FROGSTATUS_ISWANTINGSUPERHOPU;
				player[pl].frogState &= ~FROGSTATUS_ISWANTINGSUPERHOPD;
				player[pl].frogState &= ~FROGSTATUS_ISWANTINGSUPERHOPL;
				player[pl].frogState &= ~FROGSTATUS_ISWANTINGSUPERHOPR;
			}
		}

		return;
	}
	else if(player[pl].frogState & FROGSTATUS_ISFALLINGTOPLATFORM)
	{
		// the frog is falling to a platform below
		SetVector(&ground.point,&destPlatform[pl]->pltActor->actor->pos);
		SetVector(&ground.normal,&destPlatform[pl]->inTile->normal);

		// update frog position in relation to ground
//		frog[pl]->actor->vel.v[X] *= 0.95F;
		frog[pl]->actor->vel.v[Y] += -1.1F;
//		frog[pl]->actor->vel.v[Z] *= 0.95F;

		AddToVector(&frog[pl]->actor->pos,&frog[pl]->actor->vel);
		ground.J = -DotProduct(&ground.point,&ground.normal);
		dist = -(DotProduct(&frog[pl]->actor->pos,&ground.normal) + ground.J);						

		// check if frog has hit (or passed through) the platform
		if(dist > 0)
		{
			SetVector(&frog[pl]->actor->pos,&ground.point);
			CalcBounce(&frog[pl]->actor->vel,&ground.normal);
//			frog[pl]->actor->vel.v[X] *= 0.75F;
			frog[pl]->actor->vel.v[Y] *= 0.75F;
//			frog[pl]->actor->vel.v[Z] *= 0.75F;

			CreateAndAddFXSmoke(SMOKE_TYPE_NORMAL,&ground.point,128,0,0.5,15);

			if(MagnitudeSquared(&frog[pl]->actor->vel) < 5.0F)
			{
				// stop the frog from bouncing - set to standing on the platform
				currPlatform[pl] = destPlatform[pl];
				currPlatform[pl]->flags |= PLATFORM_NEW_CARRYINGFROG;
				currPlatform[pl]->carrying = frog[pl];

				SetVector(&frog[pl]->actor->pos,&ground.point);
				player[pl].frogState &= ~FROGSTATUS_ISFALLINGTOPLATFORM;
				player[pl].frogState &= ~FROGSTATUS_ISWANTINGU;
				player[pl].frogState &= ~FROGSTATUS_ISWANTINGD;
				player[pl].frogState &= ~FROGSTATUS_ISWANTINGL;
				player[pl].frogState &= ~FROGSTATUS_ISWANTINGR;
				player[pl].frogState &= ~FROGSTATUS_ISWANTINGSUPERHOPU;
				player[pl].frogState &= ~FROGSTATUS_ISWANTINGSUPERHOPD;
				player[pl].frogState &= ~FROGSTATUS_ISWANTINGSUPERHOPL;
				player[pl].frogState &= ~FROGSTATUS_ISWANTINGSUPERHOPR;
			}
		}

		return;
	}

	if(player[pl].frogState & FROGSTATUS_ISSTANDING)
	{
	}
	if(player[pl].frogState & FROGSTATUS_ISONMOVINGPLATFORM)
	{
	}

	if(player[pl].frogState & FROGSTATUS_ISFLOATING)
	{
		if ( ( destPlatform[pl] ) && ( destPlatform[pl]->inTile != currTile[pl] ) )
		{
			player[pl].frogState |= FROGSTATUS_ISJUMPINGTOTILE;
			player[pl].frogState &= ~FROGSTATUS_ISJUMPINGTOPLATFORM;
		}
		// ENDIF
	}
	// ENDIF

	// frog is croaking
	if(player[pl].frogState & FROGSTATUS_ISCROAKING)
	{
		if((frog[pl]->action.isCroaking & 3) == 0)
		{
			SetVector(&effectPos,&frog[pl]->actor->pos);
			effectPos.v[Y] += 15;
			rip = CreateAndAddFXRipple(RIPPLE_TYPE_CROAK,&effectPos,&upVec,15,2,1,15);
			rip->r = 191;
			rip->g = 255;
			rip->b = 0;
		}

		frog[pl]->action.isCroaking--;
		if(!frog[pl]->action.isCroaking)
		{
			player[pl].frogState &= ~FROGSTATUS_ISCROAKING;

			// check for nearest baby frog - do radius check ????
			if(nearestBaby = GetNearestBabyFrog())
			{
				rip = CreateAndAddFXRipple(RIPPLE_TYPE_CROAK,&nearestBaby->actor->pos,&upVec,15,2,3,20);
				rip->r = 191;
				rip->g = 255;
				rip->b = 0;
			}
		}
	}

	if(player[pl].frogState & FROGSTATUS_ISWANTINGU)
	{
		if (player[pl].frogState & FROGSTATUS_ISFLOATING )
			prevTile = currTile[pl];
		// requests up
		if(!MoveToRequestedDestination(MOVE_UP,pl))
		{
			if ( !( player[pl].frogState & FROGSTATUS_ISFLOATING ) )
			{
				frog[pl]->actor->animation->animTime = 0;
				AnimateActor(frog[pl]->actor,0,NO,NO,frogAnimSpeed);
				AnimateActor(frog[pl]->actor,3,YES,YES,1.0F);

				if ( currPlatform[pl] )
					player[pl].frogState |= FROGSTATUS_ISONMOVINGPLATFORM;
				else
					player[pl].frogState |= FROGSTATUS_ISSTANDING;			
			}
			else
			{
				destTile[pl] = prevTile;
			}
			// ENDELSEIF
			
			player[pl].frogState &= ~FROGSTATUS_ISWANTINGU;
		}
		move = moveVal;
		nextFrogFacing[pl] = nextCamFacing;
		frogFacing[pl] = camFacing;
		PlaySample ( 24,NULL,255,128);
	}
	else if(player[pl].frogState & FROGSTATUS_ISWANTINGD)
	{
		if (player[pl].frogState & FROGSTATUS_ISFLOATING )
			prevTile = currTile[pl];
		// requests up
		if(!MoveToRequestedDestination(MOVE_DOWN,pl))
		{
			if ( !(player[pl].frogState & FROGSTATUS_ISFLOATING )	)
			{
				frog[pl]->actor->animation->animTime = 0;
				AnimateActor(frog[pl]->actor,0,NO,NO,frogAnimSpeed);
				AnimateActor(frog[pl]->actor,3,YES,YES,1.0F);

				if ( currPlatform[pl] )
					player[pl].frogState |= FROGSTATUS_ISONMOVINGPLATFORM;
				else
					player[pl].frogState |= FROGSTATUS_ISSTANDING;			
			}
			else
			{
				destTile[pl] = prevTile;
			}
			// ENDELSEIF
			
			player[pl].frogState &= ~FROGSTATUS_ISWANTINGD;
		}
		move = moveVal;
		PlaySample ( 24,NULL,255,128);
		nextFrogFacing[pl] = (nextCamFacing + 2) & 3;
		frogFacing[pl] = (camFacing + 2) & 3;

	}
	else if(player[pl].frogState & FROGSTATUS_ISWANTINGL)
	{
		if (player[pl].frogState & FROGSTATUS_ISFLOATING )
			prevTile = currTile[pl];
		// requests up
		if(!MoveToRequestedDestination(MOVE_LEFT,pl))
		{
			if ( !(player[pl].frogState & FROGSTATUS_ISFLOATING )	)
			{
				frog[pl]->actor->animation->animTime = 0;
				AnimateActor(frog[pl]->actor,0,NO,NO,frogAnimSpeed);
				AnimateActor(frog[pl]->actor,3,YES,YES,1.0F);

				if ( currPlatform[pl] )
					player[pl].frogState |= FROGSTATUS_ISONMOVINGPLATFORM;
				else
					player[pl].frogState |= FROGSTATUS_ISSTANDING;			
			}
			else
			{
				destTile[pl] = prevTile;
			}
			// ENDELSEIF
			
			player[pl].frogState &= ~FROGSTATUS_ISWANTINGL;
		}
		move = moveVal;
		nextFrogFacing[pl] = (nextCamFacing + 1) & 3;		
		frogFacing[pl] = (camFacing + 1) & 3;		

		PlaySample ( 24,NULL,255,128);
	}
	else if(player[pl].frogState & FROGSTATUS_ISWANTINGR)
	{
		if (player[pl].frogState & FROGSTATUS_ISFLOATING )
			prevTile = currTile[pl];
		// requests up
		if(!MoveToRequestedDestination(MOVE_RIGHT,pl))
		{
			if ( !(player[pl].frogState & FROGSTATUS_ISFLOATING )	)
			{
				frog[pl]->actor->animation->animTime = 0;
				AnimateActor(frog[pl]->actor,0,NO,NO,frogAnimSpeed);
				AnimateActor(frog[pl]->actor,3,YES,YES,1.0F);

				if ( currPlatform[pl] )
					player[pl].frogState |= FROGSTATUS_ISONMOVINGPLATFORM;
				else
					player[pl].frogState |= FROGSTATUS_ISSTANDING;			
			}
			else
			{
				destTile[pl] = prevTile;
			}
			// ENDELSEIF
			
			player[pl].frogState &= ~FROGSTATUS_ISWANTINGR;
		}
		move = moveVal;
		nextFrogFacing[pl] = (nextCamFacing + 3) & 3;		
		frogFacing[pl] = (camFacing + 3) & 3;		
		PlaySample ( 24,NULL,255,128);
	}

	if(player[pl].frogState & FROGSTATUS_ISWANTINGLONGHOPU)
	{
		nextFrogFacing[pl] = nextCamFacing;
		player[pl].frogState |= FROGSTATUS_ISLONGHOPPING;
		// requests long hop up
		if(!MoveToRequestedDestination(MOVE_UP,pl))
		{
			frog[pl]->actor->animation->animTime = 0;
			AnimateActor(frog[pl]->actor,0,NO,NO,frogAnimSpeed);
			AnimateActor(frog[pl]->actor,3,YES,YES,1.0F);
			
			player[pl].frogState &= ~FROGSTATUS_ISWANTINGLONGHOPU;

			if ( currPlatform[pl] )
				player[pl].frogState |= FROGSTATUS_ISONMOVINGPLATFORM;
			else
				player[pl].frogState |= FROGSTATUS_ISSTANDING;			

		}
		move = moveVal;
		//frogFacing = camFacing;
		PlaySample ( 24,NULL,255,128);

		CreateAndAddFXRipple(RIPPLE_TYPE_CROAK,&frog[pl]->actor->pos,&upVec,15,1,0.3,15);
	}
	else if(player[pl].frogState & FROGSTATUS_ISWANTINGLONGHOPD)
	{
		nextFrogFacing[pl] = (nextCamFacing + 2) & 3;
		player[pl].frogState |= FROGSTATUS_ISLONGHOPPING;
		// requests long hop up
		if(!MoveToRequestedDestination(MOVE_DOWN,pl))
		{
			frog[pl]->actor->animation->animTime = 0;
			AnimateActor(frog[pl]->actor,0,NO,NO,frogAnimSpeed);
			AnimateActor(frog[pl]->actor,3,YES,YES,1.0F);
			
			player[pl].frogState &= ~FROGSTATUS_ISWANTINGLONGHOPD;

			if ( currPlatform[pl] )
				player[pl].frogState |= FROGSTATUS_ISONMOVINGPLATFORM;
			else
				player[pl].frogState |= FROGSTATUS_ISSTANDING;			

		}
		move = moveVal;
		//frogFacing = camFacing;
		PlaySample ( 24,NULL,255,128);

		CreateAndAddFXRipple(RIPPLE_TYPE_CROAK,&frog[pl]->actor->pos,&upVec,15,1,0.3,15);
	}
	else if(player[pl].frogState & FROGSTATUS_ISWANTINGLONGHOPL)
	{
		nextFrogFacing[pl] = (nextCamFacing + 1) & 3;
		player[pl].frogState |= FROGSTATUS_ISLONGHOPPING;
		// requests long hop up
		if(!MoveToRequestedDestination(MOVE_LEFT,pl))
		{
			frog[pl]->actor->animation->animTime = 0;
			AnimateActor(frog[pl]->actor,0,NO,NO,frogAnimSpeed);
			AnimateActor(frog[pl]->actor,3,YES,YES,1.0F);
			
			player[pl].frogState &= ~FROGSTATUS_ISWANTINGLONGHOPL;

			if ( currPlatform[pl] )
				player[pl].frogState |= FROGSTATUS_ISONMOVINGPLATFORM;
			else
				player[pl].frogState |= FROGSTATUS_ISSTANDING;			

		}
		move = moveVal;
		//frogFacing = (camFacing + 1) & 3;		
		PlaySample ( 24,NULL,255,128);

		CreateAndAddFXRipple(RIPPLE_TYPE_CROAK,&frog[pl]->actor->pos,&upVec,15,1,0.3,15);
	}
	else if(player[pl].frogState & FROGSTATUS_ISWANTINGLONGHOPR)
	{
		nextFrogFacing[pl] = (nextCamFacing + 3) & 3;
		player[pl].frogState |= FROGSTATUS_ISLONGHOPPING;
		// requests long hop up
		if(!MoveToRequestedDestination(MOVE_RIGHT,pl))
		{
			frog[pl]->actor->animation->animTime = 0;
			AnimateActor(frog[pl]->actor,0,NO,NO,frogAnimSpeed);
			AnimateActor(frog[pl]->actor,3,YES,YES,1.0F);
			
			player[pl].frogState &= ~FROGSTATUS_ISWANTINGLONGHOPR;

			if ( currPlatform[pl] )
				player[pl].frogState |= FROGSTATUS_ISONMOVINGPLATFORM;
			else
				player[pl].frogState |= FROGSTATUS_ISSTANDING;			

		}
		move = moveVal;
		//frogFacing = camFacing;
		PlaySample ( 24,NULL,255,128);

		CreateAndAddFXRipple(RIPPLE_TYPE_CROAK,&frog[pl]->actor->pos,&upVec,15,1,0.3,15);
	}
	
	
	if(player[pl].frogState & FROGSTATUS_ISWANTINGSUPERHOPU)
	{
		// requests superhop up
		nextFrogFacing[pl] = nextCamFacing;
		player[pl].frogState |= FROGSTATUS_ISSUPERHOPPING;
		if(!MoveToRequestedDestination(MOVE_UP,pl))
		{
			isJump[pl] = 0;
			superHop = 0;
			player[pl].frogState &= ~FROGSTATUS_ISWANTINGSUPERHOPU;
			player[pl].frogState &= ~FROGSTATUS_ISSUPERHOPPING;
			player[pl].frogState |= FROGSTATUS_ISSTANDING;
		}
		move = moveVal;
		PlaySample ( 104,NULL,255,128);

		CreateAndAddFXRipple(RIPPLE_TYPE_DUST,&frog[pl]->actor->pos,&upVec,5,1,0.15,15);
	}
	else if(player[pl].frogState & FROGSTATUS_ISWANTINGSUPERHOPD)
	{
		// requests superhop down
		nextFrogFacing[pl] = (nextCamFacing + 2) & 3;
		player[pl].frogState |= FROGSTATUS_ISSUPERHOPPING;
		if(!MoveToRequestedDestination(MOVE_DOWN,pl))
		{
			isJump[pl] = 0;
			superHop = 0;
			player[pl].frogState &= ~FROGSTATUS_ISWANTINGSUPERHOPD;
			player[pl].frogState &= ~FROGSTATUS_ISSUPERHOPPING;
			player[pl].frogState |= FROGSTATUS_ISSTANDING;
		}
		move = moveVal;
		PlaySample ( 104,NULL,255,128);

		CreateAndAddFXRipple(RIPPLE_TYPE_DUST,&frog[pl]->actor->pos,&upVec,5,1,0.15,15);
	}
	else if(player[pl].frogState & FROGSTATUS_ISWANTINGSUPERHOPL)
	{
		// requests superhop left
		nextFrogFacing[pl] = (nextCamFacing + 1) & 3;
		player[pl].frogState |= FROGSTATUS_ISSUPERHOPPING;
		if(!MoveToRequestedDestination(MOVE_LEFT,pl))
		{
			isJump[pl] = 0;
			superHop = 0;
			player[pl].frogState &= ~FROGSTATUS_ISWANTINGSUPERHOPL;
			player[pl].frogState &= ~FROGSTATUS_ISSUPERHOPPING;
			player[pl].frogState |= FROGSTATUS_ISSTANDING;
		}
		move = moveVal;
		PlaySample ( 104,NULL,255,128);

		CreateAndAddFXRipple(RIPPLE_TYPE_DUST,&frog[pl]->actor->pos,&upVec,5,1,0.15,15);
	}
	else if(player[pl].frogState & FROGSTATUS_ISWANTINGSUPERHOPR)
	{
		// requests superhop right
		nextFrogFacing[pl] = (nextCamFacing + 3) & 3;
		player[pl].frogState |= FROGSTATUS_ISSUPERHOPPING;
		if(!MoveToRequestedDestination(MOVE_RIGHT,pl))
		{
			isJump[pl] = 0;
			superHop = 0;
			player[pl].frogState &= ~FROGSTATUS_ISWANTINGSUPERHOPR;
			player[pl].frogState &= ~FROGSTATUS_ISSUPERHOPPING;
			player[pl].frogState |= FROGSTATUS_ISSTANDING;
		}
		move = moveVal;
		PlaySample ( 104,NULL,255,128);

		CreateAndAddFXRipple(RIPPLE_TYPE_DUST,&frog[pl]->actor->pos,&upVec,5,1,0.15,15);
	}

	// process frog's jump
	if ( (player[pl].frogState & FROGSTATUS_ISJUMPINGTOTILE ) && ( destTile[pl] ) )
	{
		SlurpFroggerPosition(JUMPING_TOTILE,pl);
		CheckForFroggerLanding(JUMPING_TOTILE,pl);
	}
	else if(player[pl].frogState & FROGSTATUS_ISJUMPINGTOPLATFORM)
	{
		// frog is jumping between a game tile and a platform
		SlurpFroggerPosition(JUMPING_TOPLATFORM,pl);
		CheckForFroggerLanding(JUMPING_TOPLATFORM,pl);
	}

	// check if frog is on fire, etc.
	if(frog[pl]->action.isOnFire)
	{
		frog[pl]->action.isOnFire--;
		if((frog[pl]->action.isOnFire & 3) == 0)
		{
			SetVector(&effectPos,&frog[pl]->actor->pos);
			effectPos.v[Y] += 25;
			CreateAndAddFXSmoke(SMOKE_TYPE_NORMAL,&effectPos,128,1,0.4,30);
		}
	}
}
