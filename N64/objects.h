#ifndef OBJECTS_H_INCLUDED
#define OBJECTS_H_INCLUDED

#define MAX_OBJECT_BANKS 5


float CalcRadius(MESH *mesh);
void LoadObjectBank(int num);
void InitObjectBanks();
void FindObject(OBJECT_CONTROLLER **, int, char *,BOOL vital);
void FindSubObject(OBJECT **obj, int objID);
OBJECT *FindSubObjectFromObject(OBJECT *obj, int objID);
void LoadTexture(TEXTURE *texture);

void SetObjectXlu(OBJECT *object,BOOL xlu);

void DoubleBufferSkinVtx();
void AddOffsetToVertexLoads(int offset, Gfx *dl);

void ReplaceTextureInDrawList(Gfx *dl,u32 texDataToReplace,u32 newTexData, short once);
void ReplaceAnyTextureInDrawList(Gfx *dl,u32 newTexData, short once);


extern OBJECT_BANK	objectBanks[];
extern int numoftc;


#endif