/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: objects.c
	Programmer	: 
	Date		: 29/04/99 11:14:23

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>

#include "incs.h"


OBJECT_BANK	objectBanks[MAX_OBJECT_BANKS];
OBJECT *tempObject;

int offsetVtx = TRUE;

extern OBJECT sec02;
/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
u32 Rom2Ram(u32 x, u32 addressOffset)
{
	u32	temp;
	temp = (u32)x & 0x00ffffff;
	temp += addressOffset;
	return temp;
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void RestoreDrawList(Gfx *dl, u32 offset)
{
	u8	*temp;
	u32	address;
	u32 *addrP;
	TEXTURE	*tempTex;

	temp = (u8 *)dl; 

	while(*temp != 0xDF)
	{
		switch(*temp)
		{
			//loadvertices
			case 0x01:

				if(offsetVtx)
				{
					addrP = (u32 *)(temp + 4);
					address = *addrP;
					address = Rom2Ram(address, offset);
					*addrP = address;
				}
				temp += 8;
				break;

			//load texture block
			case 0xFD:		
				//load texture and load texture palette share the same first byte
				//this appears to be the code for a texture load
				addrP = (u32 *)(temp + 4);
				address = *addrP;
				FindTexture(&tempTex,address,YES);

				if(tempTex)
				{
					if(*(temp + 8) == 0xf5)
						*addrP = (u32)tempTex->data;
					else
						*addrP = (u32)tempTex->palette;
				}
				else
				{
					FindTexture(&tempTex,UpdateCRC("fg.bmp"),YES);
				}
	
				temp += 8;
				break;


			default:		
				temp += 8;
				break;
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void SwapVtxReferencesInDrawlist(OBJECT_CONTROLLER *objectC)
{
	Gfx *dl = objectC->drawList;
//	u32	address;
//	u32 *addrP;
	s32 offset;


	if(objectC->vtxBuf)
		offset = objectC->vtx[0] - objectC->vtx[1];
	else
		offset = objectC->vtx[1] - objectC->vtx[0];

	offset *= sizeof(Vtx);

	AddOffsetToVertexLoads(offset, dl);

	objectC->vtxBuf = 1 - objectC->vtxBuf;
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void AddOffsetToVertexLoads(int offset, Gfx *dl)
{
	u8	*temp = (u8 *)dl; 
	u32	address;
	u32 *addrP;

	while(*temp != 0xDF)
	{
		switch(*temp)
		{
			//loadvertices
			case 0x01:
				addrP = (u32 *)(temp + 4);
				address = *addrP;
				*addrP += (u32)offset;
				temp += 8;
				break;

			default:
				temp += 8;
				break;
		}
	}
}



/*	--------------------------------------------------------------------------------
	Function 	: DoubleBufferSkinVtx
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void DoubleBufferSkinVtx()
{
	ACTOR2 *cur;

	while(gfxTasks);	//must wait for the graphics tasks to be finished so that we don't disturb the drawlists

	TIMER_StartTimer(6,"DBLBFSVTX");

	cur = actList;
	while ( cur )
	{
		if(cur->actor->objectController)
			if(cur->actor->objectController->drawList)
				cur->actor->objectController->vtxBuf = 1 - cur->actor->objectController->vtxBuf;

		cur = cur->next;
	}

	TIMER_EndTimer(6);
}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
int numoftc = 0;
void RestoreObjectPointers(OBJECT *obj, u32 memoryOffset)
{
	u32	tempInt;
	int x, y;
	char mess[16];

////////////initialise mesh
	tempInt = (u32)obj->mesh;
	if(tempInt)
	{
		obj->mesh = (MESH *)Rom2Ram((u32)obj->mesh, memoryOffset);

		obj->mesh->vertices = (VECTOR *)Rom2Ram((u32)obj->mesh->vertices, memoryOffset);
		
		if(obj->mesh->numFaces)
		{
			obj->mesh->faceIndex = (SHORTVECTOR *)Rom2Ram((u32)obj->mesh->faceIndex, memoryOffset);

			if(obj->mesh->faceNormals)
				obj->mesh->faceNormals = (BYTEVECTOR *)Rom2Ram((u32)obj->mesh->faceNormals, memoryOffset);
			if(obj->mesh->faceTC)
			{
				obj->mesh->faceTC = (USHORT2DVECTOR *)Rom2Ram((u32)obj->mesh->faceTC, memoryOffset);
				sprintf(mess,"OFTC%d",numoftc++);
				obj->mesh->originalFaceTC = (USHORT2DVECTOR *)JallocAlloc(sizeof(USHORT2DVECTOR) * 3 * obj->mesh->numFaces, YES,mess);
				memcpy(obj->mesh->originalFaceTC, obj->mesh->faceTC, sizeof(USHORT2DVECTOR) * 3 * obj->mesh->numFaces);
			}

			if(obj->mesh->faceFlags)
				obj->mesh->faceFlags = (char *)Rom2Ram((u32)obj->mesh->faceFlags, memoryOffset);
			if(obj->mesh->textureIDs)
			{
				obj->mesh->textureIDs = (TEXTURE **)Rom2Ram((u32)obj->mesh->textureIDs, memoryOffset);

				for(x = 0; x < obj->mesh->numFaces; x++)
					FindTexture(&(obj->mesh->textureIDs[x]), (u32)obj->mesh->textureIDs[x],YES);
			}
		}
		if(obj->mesh->vertexNormals)
			obj->mesh->vertexNormals = (BYTEVECTOR *)Rom2Ram((u32)obj->mesh->vertexNormals, memoryOffset);
	}

//////////initialise all sprites within object
	tempInt = (u32)obj->sprites;
	if(tempInt)
	{
		obj->sprites = (OBJECTSPRITE *)Rom2Ram((u32)obj->sprites, memoryOffset);
		for(x = 0; x < obj->numSprites; x++)
		{
			FindTexture(&(obj->sprites[x].textureID), (u32)obj->sprites[x].textureID,YES);
		}
	}

	tempInt = (u32)obj->effectedVerts;
	if(tempInt)
	{
		obj->effectedVerts = (SKINVTX *)Rom2Ram((u32)obj->effectedVerts, memoryOffset);
		for(x = 0; x < obj->numVerts; x++)
		{
			obj->effectedVerts[x].verts = (s16*)Rom2Ram((u32)obj->effectedVerts[x].verts, memoryOffset);
		}
	}

	obj->originalVerts = (DUELVECTOR *)Rom2Ram((u32)obj->originalVerts, memoryOffset);


	tempInt = (u32)obj->drawList;
	if(tempInt)
		obj->drawList = (Gfx *)Rom2Ram((u32)obj->drawList, memoryOffset);


	tempInt = (u32)obj->scaleKeys;
	if(tempInt)
		obj->scaleKeys = (VKEYFRAME *)Rom2Ram((u32)obj->scaleKeys, memoryOffset);

	tempInt = (u32)obj->moveKeys;
	if(tempInt)
		obj->moveKeys = (VKEYFRAME *)Rom2Ram((u32)obj->moveKeys, memoryOffset);

	tempInt = (u32)obj->rotateKeys;
	if(tempInt)
		obj->rotateKeys = (QKEYFRAME *)Rom2Ram((u32)obj->rotateKeys, memoryOffset);

	tempInt = (u32)obj->children;
	if(tempInt)
		obj->children = (OBJECT *)Rom2Ram((u32)obj->children, memoryOffset);

	tempInt = (u32)obj->next;
	if(tempInt)
		obj->next = (OBJECT *)Rom2Ram((u32)obj->next, memoryOffset);


	if(obj->drawList)
		RestoreDrawList(obj->drawList, memoryOffset);


	if(obj->children)
		RestoreObjectPointers(obj->children, memoryOffset);
	if(obj->next)
		RestoreObjectPointers(obj->next, memoryOffset);
/*	u32	tempInt;
	int x;
	char mess[16];

////////////initialise mesh
	tempInt = (u32)obj->mesh;
	if(tempInt)
	{
		obj->mesh = (MESH *)Rom2Ram((u32)obj->mesh, memoryOffset);

		obj->mesh->vertices = (VECTOR *)Rom2Ram((u32)obj->mesh->vertices, memoryOffset);
		
		if(obj->mesh->numFaces)
		{

			obj->mesh->faceIndex = (SHORTVECTOR *)Rom2Ram((u32)obj->mesh->faceIndex, memoryOffset);

			if(obj->mesh->faceNormals)
				obj->mesh->faceNormals = (BYTEVECTOR *)Rom2Ram((u32)obj->mesh->faceNormals, memoryOffset);
			if(obj->mesh->faceTC)
			{
				obj->mesh->faceTC = (SHORT2DVECTOR *)Rom2Ram((u32)obj->mesh->faceTC, memoryOffset);
				sprintf(mess,"OFTC%d",numoftc++);
				obj->mesh->originalFaceTC = (SHORT2DVECTOR *)JallocAlloc(sizeof(SHORT2DVECTOR) * 3 * obj->mesh->numFaces, YES,mess);
				memcpy(obj->mesh->originalFaceTC, obj->mesh->faceTC, sizeof(SHORT2DVECTOR) * 3 * obj->mesh->numFaces);
			}

			if(obj->mesh->faceFlags)
				obj->mesh->faceFlags = (char *)Rom2Ram((u32)obj->mesh->faceFlags, memoryOffset);
			if(obj->mesh->textureIDs)
			{
				obj->mesh->textureIDs = (TEXTURE **)Rom2Ram((u32)obj->mesh->textureIDs, memoryOffset);

				for(x = 0; x < obj->mesh->numFaces; x++)
					FindTexture(&(obj->mesh->textureIDs[x]), (u32)obj->mesh->textureIDs[x],YES);
			}

		}
		if(obj->mesh->vertexNormals)
			obj->mesh->vertexNormals = (BYTEVECTOR *)Rom2Ram((u32)obj->mesh->vertexNormals, memoryOffset);
	}

//////////initialise all sprites within object
	tempInt = (u32)obj->sprites;
	if(tempInt)
	{
		obj->sprites = (OBJECTSPRITE *)Rom2Ram((u32)obj->sprites, memoryOffset);
		for(x = 0; x < obj->numSprites; x++)
		{
			FindTexture(&(obj->sprites[x].textureID), (u32)obj->sprites[x].textureID,YES);
		}
	}

	tempInt = (u32)obj->drawList;
	if(tempInt)
		obj->drawList = (Gfx *)Rom2Ram((u32)obj->drawList, memoryOffset);


	tempInt = (u32)obj->scaleKeys;
	if(tempInt)
		obj->scaleKeys = (KEYFRAME *)Rom2Ram((u32)obj->scaleKeys, memoryOffset);

	tempInt = (u32)obj->moveKeys;
	if(tempInt)
		obj->moveKeys = (KEYFRAME *)Rom2Ram((u32)obj->moveKeys, memoryOffset);

	tempInt = (u32)obj->rotateKeys;
	if(tempInt)
		obj->rotateKeys = (KEYFRAME *)Rom2Ram((u32)obj->rotateKeys, memoryOffset);

	tempInt = (u32)obj->children;
	if(tempInt)
		obj->children = (OBJECT *)Rom2Ram((u32)obj->children, memoryOffset);

	tempInt = (u32)obj->next;
	if(tempInt)
		obj->next = (OBJECT *)Rom2Ram((u32)obj->next, memoryOffset);


	if(obj->drawList)
		RestoreDrawList(obj->drawList, memoryOffset,obj);


	if(obj->children)
		RestoreObjectPointers(obj->children, memoryOffset);
	if(obj->next)
		RestoreObjectPointers(obj->next, memoryOffset);*/
}

/*	--------------------------------------------------------------------------------
	Function 	: LoadObjectBank
	Purpose 	: dma's an object bank from rom into ram.
	Parameters 	: char *objectbank
	Returns 	: none
	Info 		:
*/
void LoadObjectBank(int num)
{
	char				*objectBank;
	u32					bankRomStart, bankRomEnd, bankSize;
	short				x = -1, y = 0;
	OBJECT				*obj;
	OBJECT_CONTROLLER	*objCont;

	switch(num)
	{
		// GAMEGENERIC OBJECTS -------------------------------------------------------------------
		case INGAMEGENERIC_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_g_0_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_g_0_SegmentRomEnd;
			dprintf"GENERIC GAME OBJECT BANK - "));
			break;

		// FRONTEND OBJECTS ----------------------------------------------------------------------
		case FRONTEND_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_0_0_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_0_0_SegmentRomEnd;
			dprintf"GENERIC FRONTEND OBJECT BANK - "));
			break;
		case FRONTEND1_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_0_1_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_0_1_SegmentRomEnd;
			dprintf"FRONTEND 1 OBJECT BANK - "));
			break;
		case FRONTEND2_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_0_2_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_0_2_SegmentRomEnd;
			dprintf"FRONTEND 2 OBJECT BANK - "));
			break;
		case FRONTEND3_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_0_3_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_0_3_SegmentRomEnd;
			dprintf"FRONTEND 3 OBJECT BANK - "));
			break;
		case FRONTEND4_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_0_4_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_0_4_SegmentRomEnd;
			dprintf"FRONTEND 4 OBJECT BANK - "));
			break;
		case FRONTEND5_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_0_5_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_0_5_SegmentRomEnd;
			dprintf"FRONTEND 5 OBJECT BANK - "));
			break;

		// GARDEN OBJECTS ------------------------------------------------------------------------
		case GENERIC_GARDEN_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_1_0_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_1_0_SegmentRomEnd;
			dprintf"GENERIC GARDEN OBJECT BANK - "));
			break;
		case LEVEL1_GARDEN_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_1_1_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_1_1_SegmentRomEnd;
			dprintf"GARDEN LEVEL 1 OBJECT BANK - "));
			break;
		case LEVEL2_GARDEN_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_1_2_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_1_2_SegmentRomEnd;
			dprintf"GARDEN LEVEL 2 OBJECT BANK - "));
			break;
		case LEVEL3_GARDEN_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_1_3_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_1_3_SegmentRomEnd;
			dprintf"GARDEN LEVEL 3 OBJECT BANK - "));
			break;
		case BOSSA_GARDEN_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_1_4_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_1_4_SegmentRomEnd;
			dprintf"GARDEN BOSS A OBJECT BANK - "));
			break;
		case BOSSB_GARDEN_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_1_5_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_1_5_SegmentRomEnd;
			dprintf"GARDEN BOSS B OBJECT BANK - "));
			break;
		case BONUS_GARDEN_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_1_6_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_1_6_SegmentRomEnd;
			dprintf"GARDEN BONUS OBJECT BANK - "));
			break;
		case MULTI_GARDEN_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_1_7_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_1_7_SegmentRomEnd;
			dprintf"GARDEN MULTIPLAYER OBJECT BANK - "));
			break;

		// ANCIENT OBJECTS ------------------------------------------------------------------------
		case GENERIC_ANCIENT_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_2_0_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_2_0_SegmentRomEnd;
			dprintf"GENERIC ANCIENT OBJECT BANK - "));
			break;
		case LEVEL1_ANCIENT_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_2_1_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_2_1_SegmentRomEnd;
			dprintf"ANCIENT LEVEL 1 OBJECT BANK - "));
			break;
		case LEVEL2_ANCIENT_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_2_2_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_2_2_SegmentRomEnd;
			dprintf"ANCIENT LEVEL 2 OBJECT BANK - "));
			break;
		case LEVEL3_ANCIENT_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_2_3_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_2_3_SegmentRomEnd;
			dprintf"ANCIENT LEVEL 3 OBJECT BANK - "));
			break;
		case BOSS_ANCIENT_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_2_4_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_2_4_SegmentRomEnd;
			dprintf"ANCIENT BOSS OBJECT BANK - "));
			break;
		case BOSSA_ANCIENT_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_2_5_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_2_5_SegmentRomEnd;
			dprintf"ANCIENT BOSS A OBJECT BANK - "));
			break;
		case BOSSB_ANCIENT_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_2_6_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_2_6_SegmentRomEnd;
			dprintf"ANCIENT BOSS B OBJECT BANK - "));
			break;
		case BOSSC_ANCIENT_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_2_7_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_2_7_SegmentRomEnd;
			dprintf"ANCIENT BOSS C OBJECT BANK - "));
			break;
		case BONUS_ANCIENT_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_2_8_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_2_8_SegmentRomEnd;
			dprintf"ANCIENT BONUS OBJECT BANK - "));
			break;
		case MULTI_ANCIENT_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_2_9_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_2_9_SegmentRomEnd;
			dprintf"ANCIENT MULTIPLAYER OBJECT BANK - "));
			break;

		// SPACE OBJECTS -------------------------------------------------------------------------
		case GENERIC_SPACE_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_3_0_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_3_0_SegmentRomEnd;
			dprintf"GENERIC SPACE OBJECT BANK - "));
			break;
		case LEVEL1_SPACE_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_3_1_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_3_1_SegmentRomEnd;
			dprintf"SPACE LEVEL 1 OBJECT BANK - "));
			break;
		case LEVEL2_SPACE_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_3_2_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_3_2_SegmentRomEnd;
			dprintf"SPACE LEVEL 2 OBJECT BANK - "));
			break;
		case LEVEL3_SPACE_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_3_3_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_3_3_SegmentRomEnd;
			dprintf"SPACE LEVEL 3 OBJECT BANK - "));
			break;
		case BOSSA_SPACE_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_3_4_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_3_4_SegmentRomEnd;
			dprintf"SPACE BOSS A OBJECT BANK - "));
			break;
		case BONUS_SPACE_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_3_5_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_3_5_SegmentRomEnd;
			dprintf"SPACE BONUS OBJECT BANK - "));
			break;
		case MULTI_SPACE_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_3_6_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_3_6_SegmentRomEnd;
			dprintf"SPACE MULTIPLAYER OBJECT BANK - "));
			break;

		// CITY OBJECTS -------------------------------------------------------------------------
		case GENERIC_CITY_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_4_0_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_4_0_SegmentRomEnd;
			dprintf"GENERIC CITY OBJECT BANK - "));
			break;
		case LEVEL1_CITY_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_4_1_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_4_1_SegmentRomEnd;
			dprintf"CITY LEVEL 1 OBJECT BANK - "));
			break;
		case LEVEL2_CITY_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_4_2_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_4_2_SegmentRomEnd;
			dprintf"CITY LEVEL 2 OBJECT BANK - "));
			break;
		case LEVEL3_CITY_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_4_3_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_4_3_SegmentRomEnd;
			dprintf"CITY LEVEL 3 OBJECT BANK - "));
			break;
		case BOSSA_CITY_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_4_4_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_4_4_SegmentRomEnd;
			dprintf"CITY BOSS A OBJECT BANK - "));
			break;
		case BONUS_CITY_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_4_5_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_4_5_SegmentRomEnd;
			dprintf"CITY BONUS OBJECT BANK - "));
			break;
		case MULTI_CITY_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_4_6_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_4_6_SegmentRomEnd;
			dprintf"CITY MULTIPLAYER OBJECT BANK - "));
			break;

		// SUBTERRANEAN OBJECTS ------------------------------------------------------------------
		case GENERIC_SUBTERRANEAN_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_5_0_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_5_0_SegmentRomEnd;
			dprintf"GENERIC SUBTERRANEAN OBJECT BANK - "));
			break;
		case LEVEL1_SUBTERRANEAN_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_5_1_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_5_1_SegmentRomEnd;
			dprintf"SUBTERRANEAN LEVEL 1 OBJECT BANK - "));
			break;
		case LEVEL2_SUBTERRANEAN_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_5_2_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_5_2_SegmentRomEnd;
			dprintf"SUBTERRANEAN LEVEL 2 OBJECT BANK - "));
			break;
		case LEVEL3_SUBTERRANEAN_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_5_3_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_5_3_SegmentRomEnd;
			dprintf"SUBTERRANEAN LEVEL 3 OBJECT BANK - "));
			break;
		case BOSSA_SUBTERRANEAN_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_5_4_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_5_4_SegmentRomEnd;
			dprintf"SUBTERRANEAN BOSS A OBJECT BANK - "));
			break;
		case BONUS_SUBTERRANEAN_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_5_5_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_5_5_SegmentRomEnd;
			dprintf"SUBTERRANEAN BONUS OBJECT BANK - "));
			break;
		case MULTI_SUBTERRANEAN_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_5_6_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_5_6_SegmentRomEnd;
			dprintf"SUBTERRANEAN MULTIPLAYER OBJECT BANK - "));
			break;

		// LABORATORY OBJECTS --------------------------------------------------------------------
		case GENERIC_LABORATORY_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_6_0_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_6_0_SegmentRomEnd;
			dprintf"GENERIC LABORATORY OBJECT BANK - "));
			break;
		case LEVEL1_LABORATORY_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_6_1_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_6_1_SegmentRomEnd;
			dprintf"LABORATORY LEVEL 1 OBJECT BANK - "));
			break;
		case LEVEL2_LABORATORY_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_6_2_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_6_2_SegmentRomEnd;
			dprintf"LABORATORY LEVEL 2 OBJECT BANK - "));
			break;
		case LEVEL3_LABORATORY_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_6_3_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_6_3_SegmentRomEnd;
			dprintf"LABORATORY LEVEL 3 OBJECT BANK - "));
			break;
		case BOSSA_LABORATORY_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_6_4_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_6_4_SegmentRomEnd;
			dprintf"LABORATORY BOSS A OBJECT BANK - "));
			break;
		case BONUS_LABORATORY_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_6_5_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_6_5_SegmentRomEnd;
			dprintf"LABORATORY BONUS OBJECT BANK - "));
			break;
		case MULTI_LABORATORY_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_6_6_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_6_6_SegmentRomEnd;
			dprintf"LABORATORY MULTIPLAYER OBJECT BANK - "));
			break;

		// TOYSHOP OBJECTS -----------------------------------------------------------------------
		case GENERIC_TOYSHOP_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_7_0_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_7_0_SegmentRomEnd;
			dprintf"GENERIC TOYSHOP OBJECT BANK - "));
			break;
		case LEVEL1_TOYSHOP_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_7_1_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_7_1_SegmentRomEnd;
			dprintf"TOYSHOP LEVEL 1 OBJECT BANK - "));
			break;
		case LEVEL2_TOYSHOP_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_7_2_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_7_2_SegmentRomEnd;
			dprintf"TOYSHOP LEVEL 2 OBJECT BANK - "));
			break;
		case LEVEL3_TOYSHOP_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_7_3_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_7_3_SegmentRomEnd;
			dprintf"TOYSHOP LEVEL 3 OBJECT BANK - "));
			break;
		case BOSSA_TOYSHOP_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_7_4_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_7_4_SegmentRomEnd;
			dprintf"TOYSHOP BOSS A OBJECT BANK - "));
			break;
		case BONUS_TOYSHOP_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_7_5_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_7_5_SegmentRomEnd;
			dprintf"TOYSHOP BONUS OBJECT BANK - "));
			break;
		case MULTI_TOYSHOP_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_7_6_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_7_6_SegmentRomEnd;
			dprintf"TOYSHOP MULTIPLAYER OBJECT BANK - "));
			break;

		// HALLOWEEN OBJECTS ---------------------------------------------------------------------
		case GENERIC_HALLOWEEN_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_8_0_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_8_0_SegmentRomEnd;
			dprintf"GENERIC HALLOWEEN OBJECT BANK - "));
			break;
		case LEVEL1_HALLOWEEN_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_8_1_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_8_1_SegmentRomEnd;
			dprintf"HALLOWEEN LEVEL 1 OBJECT BANK - "));
			break;
		case LEVEL2_HALLOWEEN_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_8_2_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_8_2_SegmentRomEnd;
			dprintf"HALLOWEEN LEVEL 2 OBJECT BANK - "));
			break;
		case LEVEL3_HALLOWEEN_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_8_3_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_8_3_SegmentRomEnd;
			dprintf"HALLOWEEN LEVEL 3 OBJECT BANK - "));
			break;
		case BOSSA_HALLOWEEN_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_8_4_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_8_4_SegmentRomEnd;
			dprintf"HALLOWEEN BOSS A OBJECT BANK - "));
			break;
		case BONUS_HALLOWEEN_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_8_5_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_8_5_SegmentRomEnd;
			dprintf"HALLOWEEN BONUS OBJECT BANK - "));
			break;
		case MULTI_HALLOWEEN_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_8_6_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_8_6_SegmentRomEnd;
			dprintf"HALLOWEEN MULTIPLAYER OBJECT BANK - "));
			break;

		// RETRO OBJECTS -------------------------------------------------------------------------
		case GENERIC_RETRO_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_9_0_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_9_0_SegmentRomEnd;
			dprintf"GENERIC RETRO OBJECT BANK - "));
			break;
		case LEVEL1_RETRO_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_9_1_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_9_1_SegmentRomEnd;
			dprintf"RETRO LEVEL 1 OBJECT BANK - "));
			break;
		case LEVEL2_RETRO_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_9_2_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_9_2_SegmentRomEnd;
			dprintf"RETRO LEVEL 2 OBJECT BANK - "));
			break;
		case LEVEL3_RETRO_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_9_3_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_9_3_SegmentRomEnd;
			dprintf"RETRO LEVEL 3 OBJECT BANK - "));
			break;
		case LEVEL4_RETRO_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_9_4_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_9_4_SegmentRomEnd;
			dprintf"RETRO LEVEL 4 OBJECT BANK - "));
			break;
		case LEVEL5_RETRO_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_9_5_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_9_5_SegmentRomEnd;
			dprintf"RETRO LEVEL 5 OBJECT BANK - "));
			break;
		case LEVEL6_RETRO_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_9_6_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_9_6_SegmentRomEnd;
			dprintf"RETRO LEVEL 6 OBJECT BANK - "));
			break;
/*
		case BOSSA_RETRO_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_9_4_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_9_4_SegmentRomEnd;
			dprintf"RETRO BOSS A OBJECT BANK - "));
			break;
		case BONUS_RETRO_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_9_5_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_9_5_SegmentRomEnd;
			dprintf"RETRO BONUS OBJECT BANK - "));
			break;
		case MULTI_RETRO_OBJ_BANK:
			bankRomStart	= (u32)&_objBank_9_6_SegmentRomStart;
			bankRomEnd		= (u32)&_objBank_9_6_SegmentRomEnd;
			dprintf"RETRO MULTIPLAYER OBJECT BANK - "));
			break;
*/

		default:
			dprintf"ERROR: no object bank specified....\n"));
			for(;;);
			break;
	}

	bankSize = bankRomEnd - bankRomStart;

	objectBank = (char *)JallocAlloc(DMAGetSize(bankRomStart,bankRomEnd),YES,"OBJBANK");

//start download from rom

	DMAMemory(objectBank, bankRomStart, bankRomEnd);
	if(objectBank)
		dprintf"LOADED OK (size %d)\n",(int)bankSize));
	else
	{
		dprintf"ERROR: UNABLE TO LOAD OBJECT BANK\n"));
		return;
	}

//store a pointer to the object bank so it may be free'd at a later date
	while(objectBanks[++x].freePtr != 0);
	if(x >= MAX_OBJECT_BANKS)
	{
		dprintf"ERROR:too many object banks\n"));
		return;
	}

	objectBanks[x].freePtr = objectBank;
	objectBanks[x].objList = (OBJECT_DESCRIPTOR *)objectBank;
	y = 0;
	/*while(objectBanks[x].objList[y].objCont)
	{
		obj = (OBJECT *)objectBanks[x].objList[y].objCont;
		objectBanks[x].objList[y].objCont = (OBJECT_CONTROLLER *)Rom2Ram((u32)obj, (u32)objectBank);
		objCont = objectBanks[x].objList[y].objCont;
				
		objCont->object = (OBJECT *)Rom2Ram((u32)objCont->object, (u32)objectBank);
		
		//restore animation pointers
		if(objCont->animation)
		{
			objCont->animation = (ACTOR_ANIMATION *)Rom2Ram((u32)objCont->animation, (u32)objectBank);
			if(objCont->animation->anims)
				objCont->animation->anims = (animation *)Rom2Ram((u32)objCont->animation->anims, (u32)objectBank);
		}
		RestoreObjectPointers(objCont->object, (u32)objectBank);
//		if((objCont->object->numScaleKeys == 1) && (objCont->object->numMoveKeys == 1) && (objCont->object->numRotateKeys == 1))
//			objCont->radius = CalcRadius(objCont->object->mesh);

		y++;
	}*/
	while(objectBanks[x].objList[y].objCont)
	{
		obj = (OBJECT *)objectBanks[x].objList[y].objCont;
		objectBanks[x].objList[y].objCont = (OBJECT_CONTROLLER *)Rom2Ram((u32)obj, (u32)objectBank);
		objCont = objectBanks[x].objList[y].objCont;
				
		objCont->object = (OBJECT *)Rom2Ram((u32)objCont->object, (u32)objectBank);
		
		//restore animation pointers
		if(objCont->animation)
		{
			objCont->animation = (ACTOR_ANIMATION *)Rom2Ram((u32)objCont->animation, (u32)objectBank);
			if(objCont->animation->anims)
				objCont->animation->anims = (animation *)Rom2Ram((u32)objCont->animation->anims, (u32)objectBank);
		}
		RestoreObjectPointers(objCont->object, (u32)objectBank);
		if(objCont->drawList)
		{
			objCont->drawList = (Gfx *)Rom2Ram((u32)objCont->drawList, (u32)objectBank);
			if(objCont->drawList)
				offsetVtx = FALSE;
			else
				offsetVtx = TRUE;		
			RestoreDrawList(objCont->drawList, (u32)objectBank);
			offsetVtx = TRUE;
  			
			//objCont->drawList = (Gfx *)Rom2Ram((u32)objCont->drawList, (u32)objectBank);
			//RestoreDrawList(objCont->drawList, (u32)objectBank);
		}
		if(objCont->vtx[0])
			objCont->vtx[0] = (Vtx *)Rom2Ram((u32)objCont->vtx[0], (u32)objectBank);
		if(objCont->vtx[1])
			objCont->vtx[1] = (Vtx *)Rom2Ram((u32)objCont->vtx[1], (u32)objectBank);

		if(objCont->LOCMesh)
		{
			objCont->LOCMesh = (MESH *)Rom2Ram((u32)objCont->LOCMesh,(u32)objectBank);
		}

		y++;
	}
	objectBanks[x].numObjects = y;	

//	if(num == SYSTEM_OBJ_BANK)
//		InitFadeOverlay();
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
float CalcRadius(MESH *mesh)
{
	int i;
	float radius = 0,temp;
	VECTOR *vertex;

	for(i = 0;i < mesh->numVertices;i++)
	{
		vertex = mesh->vertices+i;
		if((temp = Magnitude(vertex)) > radius)
			radius = temp;
	}
	return radius;
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void FindObject(OBJECT_CONTROLLER **objCPtr, int objID, char *name,BOOL vital)
{
	int x, y;
	char *temp;
			
	for(x = 0; x < MAX_OBJECT_BANKS; x++)
	{
		if(objectBanks[x].freePtr)
		{
			temp = objectBanks[x].data;
			for(y = 0; y < objectBanks[x].numObjects; y++)
			{
				if(objectBanks[x].objList[y].objectID == objID)
				{
					(*objCPtr) = objectBanks[x].objList[y].objCont;
//					if(name)
//						dprintf"Found object %s\n", name));
					return;
				}

			}
		}
	}
	*objCPtr = NULL;
	if(name)
	{
		if(vital)
		{
			dprintf"Object '%s' not found\n", name));
//			sprintf(gameInfo.error,"LEVEL UNPLAYABLE - %s MISSING",name);
		}
//		Crash();
	}
	else
	{
		dprintf"Whoops, object '%X' not found!\n", objID));
//		if(vital)
//			sprintf(gameInfo.error,"LEVEL UNPLAYABLE - OBJECT MISSING");
 //		Crash();
	}
	if(vital)
	{
//		StopDrawing("fondobj");
//help		disableGraphics = TRUE;
//		gameInfo.state = ERROR_SCREEN_MODE;
	}

	return;
}

/*	--------------------------------------------------------------------------------
	Function 	: FindSubObjectFromObject
	Purpose 	: called from FindSubObject - recursive search of an object
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
OBJECT *FindSubObjectFromObject(OBJECT *obj, int objID)
{
	OBJECT	*tempObj = NULL;

	tempObj = obj;

	if(tempObj->objID == objID)
		return tempObj;	

	if(obj->children)
	{
		tempObj = FindSubObjectFromObject(obj->children, objID);
		if(tempObj)
		{
			if(tempObj->objID == objID)
				return tempObj;	
		}
	}

	if(obj->next)
	{
		tempObj = FindSubObjectFromObject(obj->next, objID);
		if(tempObj)
		{
			if(tempObj->objID == objID)
				return tempObj;	
		}
	}

	return NULL;
}

/*	--------------------------------------------------------------------------------
	Function 	: FindSubObject
	Purpose 	: searches an object and all its children for a specified object
	Parameters 	: OBJECT **, required id
	Returns 	: none
	Info 		:
*/
void FindSubObject(OBJECT **objPtr, int objID)
{
	int x, y;
	OBJECT	*tempObj, *foundObj;

	for(x = 0; x < MAX_OBJECT_BANKS; x++)
	{
		if(objectBanks[x].freePtr)
		{
			for(y = 0; y < objectBanks[x].numObjects; y++)
			{
				tempObj = objectBanks[x].objList[y].objCont->object;
				foundObj = FindSubObjectFromObject(tempObj, objID);				   		
				if(foundObj)
				{
					(*objPtr) = foundObj;
					return;
				}
					
			}
		}
	}

	*objPtr = NULL;
	dprintf"Whoops, object '%X' not found!\n", objID));
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void WriteObjectDisplayListFlat(OBJECT *obj)
{
	int x, y, face = 0,j;
	MESH	*mesh = obj->mesh;
	USHORT2DVECTOR	*tcp = mesh->faceTC;
	VECTOR		*baseVertices = mesh->vertices;
	VECTOR		*vertex;
	SHORTVECTOR	*baseFaceIndex = mesh->faceIndex;
	SHORTVECTOR *faceIndex;
	Vtx			*batchPtr;
	int			fC = mesh->numFaces / 10;
	int			mod = mesh->numFaces % 10, temp;
	TEXTURE		*texture = NULL;
	BYTEVECTOR	*normalPtr;
	BYTEVECTOR	*baseNormalPtr = mesh->faceNormals;

    gDPPipeSync(glistp++);

	for(x = 0; x < fC; x++)
	{
		batchPtr = vtxPtr;

		for(y = 0; y < 10; y++)
		{		
			faceIndex = baseFaceIndex + face;
			normalPtr = baseNormalPtr + face;

			vertex = baseVertices + faceIndex->v[X];
  			V((vtxPtr), 
				(int)vertex->v[0],
				(int)vertex->v[1],
				(int)vertex->v[2],0,
				tcp->v[0], (tcp++)->v[1],
				normalPtr->v[X], normalPtr->v[Y], normalPtr->v[Z], xluFact);

			vertex = baseVertices + faceIndex->v[Y];
			V((vtxPtr+1), 
				(int)vertex->v[0],
				(int)vertex->v[1],
				(int)vertex->v[2],0,
				tcp->v[0], (tcp++)->v[1],
				normalPtr->v[X], normalPtr->v[Y], normalPtr->v[Z], xluFact);

			vertex = baseVertices + faceIndex->v[Z];
			V((vtxPtr+2), 
				(int)vertex->v[0],
				(int)vertex->v[1],
				(int)vertex->v[2],0,
				tcp->v[0], (tcp++)->v[1],
				normalPtr->v[X], normalPtr->v[Y], normalPtr->v[Z], xluFact);



  
			face++;			
			vtxPtr+=3;
//			if(face == mesh->numFaces)
//				break;

		}

		gSPVertex(glistp++, batchPtr , 30, 0);

		for(j = 0;j < 10;j++)
		{
			if(texture != obj->mesh->textureIDs[face-10+j]	)
			{
				texture = obj->mesh->textureIDs[face-10+j];
			    gDPPipeSync(glistp++);
				LoadTexture(texture);
			}
			gSP1Triangle(glistp++, j*3, j*3+1, j*3+2, 2);
		}
	}

//handle remaining faces
	batchPtr = vtxPtr;

	for(y = 0; y < mod; y++)
	{		
		faceIndex = baseFaceIndex + face;
		normalPtr = baseNormalPtr + face;

		vertex = baseVertices + faceIndex->v[0];
		V((vtxPtr), 
			(int)vertex->v[0],
			(int)vertex->v[1],
			(int)vertex->v[2],0,
			tcp->v[0], (tcp++)->v[1],
			normalPtr->v[X], normalPtr->v[Y], normalPtr->v[Z], xluFact);

		vertex = baseVertices + faceIndex->v[1];
		V((vtxPtr+1), 
			(int)vertex->v[0],
			(int)vertex->v[1],
			(int)vertex->v[2],0,
			tcp->v[0], (tcp++)->v[1],
  			normalPtr->v[X], normalPtr->v[Y], normalPtr->v[Z], xluFact);

		vertex = baseVertices + faceIndex->v[2];
  		V((vtxPtr+2), 
			(int)vertex->v[0],
			(int)vertex->v[1],
			(int)vertex->v[2],0,
			tcp->v[0], (tcp++)->v[1],
			normalPtr->v[X], normalPtr->v[Y], normalPtr->v[Z], xluFact);


		face++;			
		vtxPtr+=3;

	}

	if(mod)
	{
		face -= mod;
		gSPVertex(glistp++, batchPtr , mod * 3, 0);
		temp = 0;
		for(x = 0; x < mod; x++)
		{
			if(texture != obj->mesh->textureIDs[face])
			{
				texture = obj->mesh->textureIDs[face];		
				gDPPipeSync(glistp++);
				LoadTexture(texture);
			}
			gSP1Triangle(glistp++, temp++, temp++, temp++, 2);
			face++;
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void WriteObjectDisplayListGouraud(OBJECT *obj)
{
	int x, y, face = 0,j;
	MESH	*mesh = obj->mesh;
	USHORT2DVECTOR	*tcp = mesh->faceTC;
	VECTOR		*baseVertices = mesh->vertices;
	VECTOR		*vertex;
	SHORTVECTOR	*baseFaceIndex = mesh->faceIndex;
	SHORTVECTOR *faceIndex;
	Vtx			*batchPtr;
	int			fC = mesh->numFaces / 10;
	int			mod = mesh->numFaces % 10, temp;
	TEXTURE		*texture = NULL;
	BYTEVECTOR	*vNormalPtr;
	BYTEVECTOR	*baseVNormalPtr = mesh->vertexNormals;

    gDPPipeSync(glistp++);

	for(x = 0; x < fC; x++)
	{
		batchPtr = vtxPtr;

		for(y = 0; y < 10; y++)
		{
			faceIndex = baseFaceIndex + face;
			tcp = mesh->faceTC + face;

			vertex = baseVertices + faceIndex->v[X];
			vNormalPtr = baseVNormalPtr + faceIndex->v[X];
  			V((vtxPtr), 
				(int)vertex->v[0],
				(int)vertex->v[1],
				(int)vertex->v[2],0,
				tcp->v[0], (tcp++)->v[1],
				vNormalPtr->v[X], vNormalPtr->v[Y], vNormalPtr->v[Z], xluFact);

			vertex = baseVertices + faceIndex->v[Y];
			vNormalPtr = baseVNormalPtr + faceIndex->v[Y];
			V((vtxPtr+1), 
				(int)vertex->v[0],
				(int)vertex->v[1],
				(int)vertex->v[2],0,
				tcp->v[0], (tcp++)->v[1],
				vNormalPtr->v[X], vNormalPtr->v[Y], vNormalPtr->v[Z], xluFact);

			vertex = baseVertices + faceIndex->v[Z];
			vNormalPtr = baseVNormalPtr + faceIndex->v[Z];
			V((vtxPtr+2), 
				(int)vertex->v[0],
				(int)vertex->v[1],
				(int)vertex->v[2],0,
				tcp->v[0], (tcp++)->v[1],
				vNormalPtr->v[X], vNormalPtr->v[Y], vNormalPtr->v[Z], xluFact);
  
			face++;			
			vtxPtr+=3;

		}

		gSPVertex(glistp++, batchPtr , 30, 0);
	    gDPPipeSync(glistp++);


		for(j = 0;j < 10;j++)
		{
			if(texture != obj->mesh->textureIDs[face-10+j]	)
			{
				texture = obj->mesh->textureIDs[face-10+j];
			    gDPPipeSync(glistp++);
				LoadTexture(texture);
			}

//			FindTexture(&texture,UpdateCRC("fgeye.bmp"),YES);
//		    gDPPipeSync(glistp++);
//			LoadTexture(texture);

			gSP1Triangle(glistp++, j*3, j*3+1, j*3+2, 2);
		}
	}

//handle remaining faces
	batchPtr = vtxPtr;

	for(y = 0; y < mod; y++)
	{		
		faceIndex = baseFaceIndex + face;

		vertex = baseVertices + faceIndex->v[0];
		vNormalPtr = baseVNormalPtr + faceIndex->v[X];
		V((vtxPtr), 
			(int)vertex->v[0],
			(int)vertex->v[1],
			(int)vertex->v[2],0,
			tcp->v[0], (tcp++)->v[1],
			vNormalPtr->v[X], vNormalPtr->v[Y], vNormalPtr->v[Z], xluFact);

		vertex = baseVertices + faceIndex->v[1];
		vNormalPtr = baseVNormalPtr + faceIndex->v[Y];
		V((vtxPtr+1), 
			(int)vertex->v[0],
			(int)vertex->v[1],
			(int)vertex->v[2],0,
			tcp->v[0], (tcp++)->v[1],
			vNormalPtr->v[X], vNormalPtr->v[Y], vNormalPtr->v[Z], xluFact);

		vertex = baseVertices + faceIndex->v[2];
		vNormalPtr = baseVNormalPtr + faceIndex->v[Z];
  		V((vtxPtr+2), 
			(int)vertex->v[0],
			(int)vertex->v[1],
			(int)vertex->v[2],0,
			tcp->v[0], (tcp++)->v[1],
			vNormalPtr->v[X], vNormalPtr->v[Y], vNormalPtr->v[Z], xluFact);

		face++;			
		vtxPtr+=3;
	}

	if(mod)
	{
		face -= mod;
		gSPVertex(glistp++, batchPtr , mod * 3, 0);
		temp = 0;
		for(x = 0; x < mod; x++)
		{
			if(texture != obj->mesh->textureIDs[face])
			{
				texture = obj->mesh->textureIDs[face];
			    gDPPipeSync(glistp++);
				LoadTexture(texture);
			}

//			FindTexture(&texture,UpdateCRC("fgeye.bmp"),YES);
//		    gDPPipeSync(glistp++);
//			LoadTexture(texture);

			gSP1Triangle(glistp++, temp++, temp++, temp++, 2);
			face++;
		}
	}

    gDPPipeSync(glistp++);
}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void LoadTexture(TEXTURE *texture)
{
	if(texture == 0)
		return;

	switch(texture->pixSize)
	{
		case G_IM_SIZ_16b:
			gDPSetTextureLUT(glistp++,	G_TT_NONE);
			if(mirrorTextures)
			{
				gDPLoadTextureBlock(glistp++, texture->data , texture->format, G_IM_SIZ_16b, texture->sx, texture->sy, 0,G_TX_WRAP | G_TX_MIRROR, G_TX_WRAP | G_TX_MIRROR,texture->TCScaleX, texture->TCScaleY, G_TX_NOLOD, G_TX_NOLOD);
			}
			else
			{
				gDPLoadTextureBlock(glistp++, texture->data , texture->format, G_IM_SIZ_16b, texture->sx, texture->sy, 0,G_TX_WRAP /*| G_TX_MIRROR*/, G_TX_WRAP /*| G_TX_MIRROR*/,texture->TCScaleX, texture->TCScaleY, G_TX_NOLOD, G_TX_NOLOD);
			}
			break;

		case G_IM_SIZ_8b:

			gDPSetTextureLUT(glistp++,	G_TT_RGBA16);
			gDPLoadTLUT_pal256(glistp++, texture->palette);
			if(mirrorTextures)
			{
				gDPLoadTextureBlock(glistp++, texture->data , texture->format, G_IM_SIZ_8b, texture->sx, texture->sy, 0,G_TX_WRAP | G_TX_MIRROR, G_TX_WRAP | G_TX_MIRROR,texture->TCScaleX, texture->TCScaleY, G_TX_NOLOD, G_TX_NOLOD);
			}
			else
			{
				gDPLoadTextureBlock(glistp++, texture->data , texture->format, G_IM_SIZ_8b, texture->sx, texture->sy, 0,G_TX_WRAP/* | G_TX_MIRROR*/, G_TX_WRAP/* | G_TX_MIRROR*/,texture->TCScaleX, texture->TCScaleY, G_TX_NOLOD, G_TX_NOLOD);
			}
			break;

		case G_IM_SIZ_4b:

			gDPSetTextureLUT(glistp++,	G_TT_RGBA16);
			gDPLoadTLUT_pal16(glistp++,0,texture->palette);
			if(mirrorTextures)
			{
				gDPLoadTextureBlock_4b(glistp++, texture->data , texture->format, texture->sx, texture->sy, 0,G_TX_WRAP | G_TX_MIRROR, G_TX_WRAP | G_TX_MIRROR,texture->TCScaleX, texture->TCScaleY, G_TX_NOLOD, G_TX_NOLOD);
			}
			else
			{
				gDPLoadTextureBlock_4b(glistp++, texture->data , texture->format, texture->sx, texture->sy, 0,G_TX_WRAP/* | G_TX_MIRROR*/, G_TX_WRAP/* | G_TX_MIRROR*/,texture->TCScaleX, texture->TCScaleY, G_TX_NOLOD, G_TX_NOLOD);
			}
			break;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void StopDrawing(char *whereami)
{
	if(codeDrawingRequest == TRUE)
		dprintf"Stop Drawing Requested from '%s'\n", whereami));
	codeDrawingRequest = FALSE;
	codeRunning = 0;
	while(gfxIsDrawing == TRUE);
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void StartDrawing(char *whereami)
{
	if(codeDrawingRequest == FALSE)
		dprintf"Start Drawing Requested from '%s'\n", whereami));
	codeDrawingRequest = TRUE;
	codeRunning = 0;
	while(gfxIsDrawing == FALSE)
		dispFrameCount = 0;
}


/*	--------------------------------------------------------------------------------
	Function		: SetObjectXlu
	Purpose			: sets an object to be translucent
	Parameters		: OBJECT *,BOOL
	Returns			: void
	Info			: 
*/
void SetObjectXlu(OBJECT *object,BOOL xlu)
{
	if(xlu)
		object->flags |= OBJECT_FLAGS_XLU;
	else
		object->flags &= -1 - OBJECT_FLAGS_XLU;
	if(object->children)
		SetObjectXlu(object->children,xlu);
	if(object->next)
		SetObjectXlu(object->next,xlu);
}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void SetXluInDrawList(Gfx *dl,UBYTE xlu)
{
	u8 *temp = (u8 *)dl;

	while(*temp != 0xDF)
	{
		if(*temp == 0xFA)
		{
			*(temp+7) = xlu;
		}
		temp += 8;
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void ReplaceTextureInDrawList(Gfx *dl,u32 texDataToReplace,u32 newTexData, short once)
{
	u8 *temp = (u8 *)dl; 
	u32 address,*addrP;

	while(*temp != 0xDF)
	{
		if(*temp == 0xFD)
		{
			addrP = (u32 *)(temp + 4);
			address = *addrP;
			if(*(temp + 8) == 0xf5)
			{			
				if((texDataToReplace == NULL) || (address == texDataToReplace))
				{
					*addrP = newTexData;
					if(once)
						return;
				}
			}
		}
		temp += 8;
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void ReplaceAnyTextureInDrawList(Gfx *dl,u32 newTexData, short once)
{
	u8 *temp = (u8 *)dl; 
	u32 address,*addrP;

	while(*temp != 0xDF)
	{
		if(*temp == 0xFD)
		{
			addrP = (u32 *)(temp + 4);
			address = *addrP;
			if(*(temp + 8) == 0xf5)
			{
				*addrP = newTexData;
			
				if(once)
					return;
			}
		}
		temp += 8;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
COLOUR watCol = {60,255,200,255};
void WriteObjectDisplayListGouraudPhong(OBJECT *obj)
{
	int x, y, face = 0,j;
	MESH	*mesh = obj->mesh;
	USHORT2DVECTOR	*tcp = mesh->faceTC;
	VECTOR		*baseVertices = mesh->vertices;
	VECTOR		*vertex;
	SHORTVECTOR	*baseFaceIndex = mesh->faceIndex;
	SHORTVECTOR *faceIndex;
	Vtx			*batchPtr;
	int			fC = mesh->numFaces / 10;
	int			mod = mesh->numFaces % 10, temp;
	BYTEVECTOR	*vNormalPtr;
	BYTEVECTOR	*basevNormalPtr = mesh->vertexNormals;


    gDPPipeSync(glistp++);

	gDPSetPrimColor(glistp++,0,0,watCol.r,watCol.g,watCol.b,xluFact);

	for(x = 0; x < fC; x++)
	{
		batchPtr = vtxPtr;

		for(y = 0; y < 10; y++)
		{		
			faceIndex = baseFaceIndex + face;

			vertex = baseVertices + faceIndex->v[X];
			vNormalPtr = basevNormalPtr + faceIndex->v[X];
  			V((vtxPtr), 
				(int)vertex->v[0],
				(int)vertex->v[1],
				(int)vertex->v[2],0,
				tcp->v[0], (tcp++)->v[1],
				vNormalPtr->v[X], vNormalPtr->v[Y], vNormalPtr->v[Z], xluFact);

			vertex = baseVertices + faceIndex->v[Y];
			vNormalPtr = basevNormalPtr + faceIndex->v[Y];
			V((vtxPtr+1), 
				(int)vertex->v[0],
				(int)vertex->v[1],
				(int)vertex->v[2],0,
				tcp->v[0], (tcp++)->v[1],
				vNormalPtr->v[X], vNormalPtr->v[Y], vNormalPtr->v[Z], xluFact);

			vertex = baseVertices + faceIndex->v[Z];
			vNormalPtr = basevNormalPtr + faceIndex->v[Z];
			V((vtxPtr+2), 
				(int)vertex->v[0],
				(int)vertex->v[1],
				(int)vertex->v[2],0,
				tcp->v[0], (tcp++)->v[1],
				vNormalPtr->v[X], vNormalPtr->v[Y], vNormalPtr->v[Z], xluFact);
  
			face++;			
			vtxPtr+=3;

		}

		gSPVertex(glistp++, batchPtr , 30, 0);
	    gDPPipeSync(glistp++);


		for(j = 0;j < 10;j++)
		{
			gSP1Triangle(glistp++, j*3, j*3+1, j*3+2, 2);
		}
	}

//handle remaining faces
	batchPtr = vtxPtr;

	if(mod)
	{
		for(y = 0; y < mod; y++)
		{		
			faceIndex = baseFaceIndex + face;

			vertex = baseVertices + faceIndex->v[X];
			vNormalPtr = basevNormalPtr + faceIndex->v[X];
			V((vtxPtr), 
				(int)vertex->v[0],
				(int)vertex->v[1],
				(int)vertex->v[2],0,
				tcp->v[0], (tcp++)->v[1],
				vNormalPtr->v[X], vNormalPtr->v[Y], vNormalPtr->v[Z], xluFact);

			vertex = baseVertices + faceIndex->v[Y];
			vNormalPtr = basevNormalPtr + faceIndex->v[Y];
			V((vtxPtr+1), 
				(int)vertex->v[0],
				(int)vertex->v[1],
				(int)vertex->v[2],0,
				tcp->v[0], (tcp++)->v[1],
				vNormalPtr->v[X], vNormalPtr->v[Y], vNormalPtr->v[Z], xluFact);

			vertex = baseVertices + faceIndex->v[Z];
			vNormalPtr = basevNormalPtr + faceIndex->v[Z];
  			V((vtxPtr+2), 
				(int)vertex->v[0],
				(int)vertex->v[1],
				(int)vertex->v[2],0,
				tcp->v[0], (tcp++)->v[1],
				vNormalPtr->v[X], vNormalPtr->v[Y], vNormalPtr->v[Z], xluFact);

			face++;			
			vtxPtr+=3;

		}

		face -= mod;
		gSPVertex(glistp++, batchPtr , mod * 3, 0);
		temp = 0;
		for(x = 0; x < mod; x++)
		{
			gSP1Triangle(glistp++, temp++, temp++, temp++, 2);
			face++;
		}
	}

    gDPPipeSync(glistp++);
}


