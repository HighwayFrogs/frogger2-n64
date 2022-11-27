#ifndef __MATHS_H
#define __MATHS_H


#define Fabs(a) ((a) > 0 ? (a) : -(a))

#define PI			3.141592654
#define PI_OVER_2	3.141592654/2.0F
#define PI_OVER_4	3.141592654/4.0F
#define PI2			3.141592654*2.0F

#define ULONG unsigned long

#define DotProduct(v1,v2)	((((v1)->v[X])*((v2)->v[X])) + (((v1)->v[Y])*((v2)->v[Y])) + (((v1)->v[Z])*((v2)->v[Z])))
#define DotProduct2D(v1,v2)	((((v1)->v[X])*((v2)->v[X])) + (((v1)->v[Z])*((v2)->v[Z])))
#define MagnitudeSquared(vect) ((vect)->v[X]*(vect)->v[X] + (vect)->v[Y]*(vect)->v[Y] + (vect)->v[Z]*(vect)->v[Z])
#define MagnitudeSquared2D(vect) ((vect)->v[X]*(vect)->v[X] + (vect)->v[Z]*(vect)->v[Z])
#define Magnitude2D(vect) (sqrtf((vect)->v[X]*(vect)->v[X] + (vect)->v[Z]*(vect)->v[Z]))
#define Magnitude(vect) (sqrtf((vect)->v[X]*(vect)->v[X] + (vect)->v[Y]*(vect)->v[Y] + (vect)->v[Z]*(vect)->v[Z]))
#define MagnitudeQuat(q) (sqrt(q->w*q->w + q->x*q->x + q->y*q->y + q->z*q->z))

/*
#define ZeroVector(vect) SetVector(vect,&zero)
*/

#define CreateVector(vec,x,y,z) { (vec)->v[X] = (x); (vec)->v[Y] = (y); (vec)->v[Z] = (z); }
#define AddVector(res,v1,v2) {(res)->v[X] = (v1)->v[X] + (v2)->v[X];(res)->v[Y] = (v1)->v[Y] + (v2)->v[Y];(res)->v[Z] = (v1)->v[Z] + (v2)->v[Z];}
#define AddToVector(res,v1) {(res)->v[X] += (v1)->v[X];(res)->v[Y] += (v1)->v[Y];(res)->v[Z] += (v1)->v[Z];}
#define SubFromVector(res,v1) {(res)->v[X] -= (v1)->v[X];(res)->v[Y] -= (v1)->v[Y];(res)->v[Z] -= (v1)->v[Z];}
#define SubVector(res,v1,v2) {(res)->v[X] = (v1)->v[X] - (v2)->v[X];(res)->v[Y] = (v1)->v[Y] - (v2)->v[Y];(res)->v[Z] = (v1)->v[Z] - (v2)->v[Z];}
#define ScaleVector(v1,scale) {(v1)->v[X] *= scale;(v1)->v[Y] *= scale;(v1)->v[Z] *= scale;}
#define ScaleVector2D(v1,scale) {(v1)->v[X] *= scale;(v1)->v[Z] *= scale;}

#define SetMatrix(m1,m2) (*((MATRIX *)m1)) = (*((MATRIX *)m2))


#define ZeroVector2D(vect) SetVector2D(vect,&zero)


#define ZeroVector(vect) (vect)->v[X] = (vect)->v[Y] = (vect)->v[Z] = 0

#define SetVector(v1,v2)	(*(v1)) = (*(v2))

#define SetQuaternion(q1,q2) (*(q1)) = (*(q2))


#define ZeroQuaternion(quat) SetQuaternion(quat,&zeroQuat)

#define LOAD_VECTOR(p, x, y, z) { p.v[0] = (x); p.v[1] = (y); p.v[2] = (z);}
#define LOAD_BYTEVECTOR(p, x, y, z) {p.v[0] = (x); p.v[1] = (y); p.v[2] = (z);}
#define LOAD_SHORTVECTOR(p, x, y, z) {p.v[0] = (x); p.v[1] = (y); p.v[2] = (z);}
#define LOAD_SHORT2DVECTOR(p, x, y) {p.v[0] = (x); p.v[1] = (y);}

#define Bound(a,x,b) Min(b,Max(a,x))

//#define SetVector(v1,v2) {(v1)->v[X] = (v2)->v[X];(v1)->v[Y] = (v2)->v[Y];(v1)->v[Z] = (v2)->v[Z];}

// New for the skinning

#define PushMatrix(matrix)	{ guMtxCatF((matrix), matrixStack.stack[matrixStack.stackPosition], matrixStack.stack[matrixStack.stackPosition+1]); matrixStack.stackPosition++; }
#define PopMatrix() matrixStack.stackPosition--

#define PushRMatrix(matrix)	{ guMtxCatF((matrix), rMatrixStack.stack[rMatrixStack.stackPosition++], rMatrixStack.stack[rMatrixStack.stackPosition]); rMatrixStack.stackPosition++; } 
#define PopRMatrix() rMatrixStack.stackPosition--

#define LinearInterp(result,from,to,interp) {(result)->v[0] = (from)->v[0] + (((to)->v[0] - (from)->v[0]) * interp);(result)->v[1] = (from)->v[1] + (((to)->v[1] - (from)->v[1]) * interp);(result)->v[2] = (from)->v[2] + (((to)->v[2] - (from)->v[2]) * interp);}



#define QEPSILON	((float)0.00001)
#define QHALFPI		((float)1.570796326794895)

#define MAX_STACK_SIZE	16

typedef struct
{

	float	stack[MAX_STACK_SIZE][4][4];
	short	stackPosition;
	float	result[4][4];
}MATRIXSTACK;

extern MATRIXSTACK	matrixStack;
extern MATRIXSTACK	rMatrixStack;


#ifndef PC_VERSION
extern char *atantab;
extern char *acostab;
#endif

extern VECTOR zero;
extern QUATERNION zeroQuat;
extern QUATERNION vertQ;

extern VECTOR upVec;
extern VECTOR inVec;
extern VECTOR outVec;
extern VECTOR rightVec;


//maths functions

#ifndef PC_VERSION
void InitAtan();
float acos(float val);
float acosBetter(float val);
float atan(float val);
float atan2(float z,float x);
#else
#define acosBetter(x)	(acos(x))
#endif


void RoundToZero(float *f);
BOOL IntervalsOverlap(float lo1,float hi1,float lo2,float hi2);
int Random(ULONG val);
void SeedRandom();

//matrix / quat functions
void QuatSlerp(QUATERNION *src1, QUATERNION *src2, float t, QUATERNION *dquat);
BOOL QuatSlerpAtFixedSpeed(QUATERNION *src1, QUATERNION *sp2, float speed, QUATERNION *dquat);
BOOL QuatSlerpAtFixedSpeedOld(QUATERNION *src1, QUATERNION *src2, float speed, QUATERNION *dquat);
void QuaternionToMatrix(QUATERNION *squat, MATRIX *dmatrix);
void QuaternionToMatrixZeroY(QUATERNION *squat, MATRIX *dmatrix);
void MatrixToQuaternion(MATRIX *smatrix, QUATERNION *dquat);
void GetRotationFromQuaternion(QUATERNION *destQ,QUATERNION *srcQ);
void GetQuaternionFromRotation(QUATERNION *destQ,QUATERNION *srcQ);
void GetQuaternionFromXZRotation(QUATERNION *destQ,QUATERNION *srcQ);
void QuaternionMultiply(QUATERNION *res,QUATERNION *q1,QUATERNION *q2);
/*
int IntersectEdges(float *x,float *y,POINT2D *v1,POINT2D *v2,POINT2D *v3,POINT2D *v4);
*/
//void SetQuaternion(QUATERNION *q1,QUATERNION *q2);
void NormalToQuaternion(QUATERNION *q,VECTOR *normal);

//vector functions
//void ZeroVector(VECTOR *v1);
void RotateVectorByQuaternion(VECTOR *result,VECTOR *vect,QUATERNION *q);
void RotateVectorByRotation(VECTOR *result,VECTOR *vect,QUATERNION *rot);
void RotateVectorByXZRotation(VECTOR *result,VECTOR *vect,QUATERNION *rot);
//void SetVector(VECTOR *v1,VECTOR *v2);
//void SubVector(VECTOR *result,VECTOR *operand1,VECTOR *operand2);
void SubVector2D(VECTOR *result,VECTOR *operand1,VECTOR *operand2);
float FindShortestAngle(float val1, float val2);
float FindShortestAngleSigned(float ang1,float ang2);
//float Magnitude(VECTOR *vect);
void MakeUnit(VECTOR *vect);
void MakeUnit2D(VECTOR *vect);
void MakeUnitQuat( QUATERNION *q );
//void ScaleVector(VECTOR *vect,float scale);
//void ScaleVector2D(VECTOR *vect,float scale);
float Aabs(float angle);
float DistanceBetweenPoints(VECTOR *v1, VECTOR *v2);
void RotateVector2D(VECTOR *dest, VECTOR *source, float angle);
void AddScaledVector(VECTOR *result,VECTOR *operand1,float scale1,VECTOR *operand2,float scale2);
void RotateVector2DXYZ(VECTOR *dest, VECTOR *source, float angle, BYTE axis);
void CrossProduct(VECTOR *result,VECTOR *operand1,VECTOR *operand2);
void CrossProductVertical(VECTOR *result,VECTOR *operand2);
void AddOneScaledVector(VECTOR *result,VECTOR *operand1,VECTOR *operand2,float scale2);
//void AddToVector(VECTOR *result,VECTOR *operand);
//void SubFromVector(VECTOR *result,VECTOR *operand);
void AddToVector2D(VECTOR *result,VECTOR *operand);
void SubFromVector2D(VECTOR *result,VECTOR *operand);
void AddToOneScaledVector(VECTOR *result,VECTOR *operand2,float scale2);
void AddToOneScaledVector2D(VECTOR *result,VECTOR *operand2,float scale2);

void AddOneScaledVector2D(VECTOR *result,VECTOR *operand1,VECTOR *operand2,float scale2);
float DistanceBetweenPoints2D(VECTOR *v1, VECTOR *v2);
float DistanceBetweenPoints(VECTOR *v1, VECTOR *v2);
float DistanceBetweenPoints2DSquared(VECTOR *v1, VECTOR *v2);
float DistanceBetweenPointsSquared(VECTOR *v1, VECTOR *v2);
BOOL IsPointWithinBox(BOX *box, VECTOR *pos);
BOOL IsPointWithinBoxPlusHeight(BOX *box, VECTOR *pos,float height);
void FlipVector(VECTOR *vector);
void FlipVector(VECTOR *vector);
void SetVector2D(VECTOR *v1,VECTOR *v2);
//void AddVector(VECTOR *result,VECTOR *operand1,VECTOR *operand2);
void AddVector2D(VECTOR *result,VECTOR *operand1,VECTOR *operand2);
void AddScaledVector2D(VECTOR *result,VECTOR *operand1,float scale1,VECTOR *operand2,float scale2);
float SineWave(float freq,float phase,int base);
float SineWave2(float freq,float phase);
void SetVectorLength(VECTOR *vect,float length);
BOOL SphereIntersectsBox(BOX *box,VECTOR *pos,float radius);
BOOL IsPointWithinBox2D(BOX *box, VECTOR *pos);





void CalculateQuatForPlane2(float yRot,QUATERNION *qAim,VECTOR *normal);



void InitMatrixStack();
//void PushMatrix(float *matrix);
//void PopMatrix();
void ComputeResultMatrix();
//void SetMatrix(float *dest, float *source);
void LoadMatrix(float *matrix);
void CalculateQuatForPlane(ACTOR *actor,QUATERNION *qAim,PLANE *plane);
void FlipVector2D(VECTOR *vector);


#ifdef PC_VERSION

void guRotateF(MATRIX *m1,MATRIX *m2,MATRIX *m3);
void guMtxF2L(MATRIX *m1,MATRIX *m2,MATRIX *m3);
void guMtxIdent (float a[4][4]);
void guScaleF(float a[4][4], float dx, float dy, float dz);
void guTranslateF(float a[4][4], float dx, float dy, float dz);
void guMtxCatF(float b[4][4], float a[4][4], float ret[4][4]);
void guMtxXFMF(float m[4][4],float srcX,float srcY,float srcZ,float *destX,float *destY,float *destZ);
void guNormalise (float *x, float *y, float *z);
void guLookAtF (float m[4][4],
				float xEye, float yEye, float zEye,
				float xAt, float yAt, float zAt,
				float xUp, float yUp, float zUp);

#endif



#endif

