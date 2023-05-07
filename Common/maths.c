/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.
	
		File		: maths.c
		Programmer	: 
		Date		: 29/04/99 11:08:07
		
----------------------------------------------------------------------------------------------- */

#include <ultra64.h>

#include "incs.h"

unsigned int xseed = (7789<<16)+13399;	

VECTOR zero = {{0, 0, 0}};
QUATERNION zeroQuat = {0,0,0,1};
QUATERNION vertQ = {0,1,0,0};
MATRIXSTACK	matrixStack;
MATRIXSTACK	rMatrixStack;

VECTOR upVec	= { 0,1,0 };
VECTOR inVec	= { 0,0,1 };
VECTOR outVec	= { 0,0,-1 };
VECTOR rightVec	= { 1,0,0 };


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
ULONG rseed=398623;

int Random(ULONG lim)
{

	if(lim == 0)
		return 0;

	rseed = rseed^(rseed<<1);
	if ((rseed & 0x80000000)==0)
		rseed++;
	else
		if (rseed == 0x80000000)
			rseed = 0;

	return (rseed%lim);
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void SeedRandom()
{
	rseed = 398623;
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		: destination CANNOT be same as source
*/
void CrossProduct(VECTOR *result,VECTOR *operand1,VECTOR *operand2)
{
	result->v[X] = operand1->v[Y] * operand2->v[Z] - operand1->v[Z] * operand2->v[Y];
	result->v[Y] = operand1->v[Z] * operand2->v[X] - operand1->v[X] * operand2->v[Z];
	result->v[Z] = operand1->v[X] * operand2->v[Y] - operand1->v[Y] * operand2->v[X];
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		: destination CANNOT be same as source
*/
void CrossProductVertical(VECTOR *result,VECTOR *operand2)
{
	result->v[X] = operand2->v[Z];
	result->v[Y] = 0;
	result->v[Z] = -operand2->v[X];
}

/*	--------------------------------------------------------------------------------
	Function 	: acos(float val)
	Purpose 	: returns the arc cosine of a value (-1/1)
	Parameters 	: value
	Returns 	: float
	Info 		:
*/
#ifndef PC_VERSION
float acos(float val)
{
    int index;
    float ret;
    USHORT *table = (USHORT *)&acostab;

    if(val > 1.000001)
	{
//		dprintf"\n%f\n",val));
		Crash("acos");
		val = 1.000000F;
	}
	if(val < -1.000001)
	{
//		dprintf"\n%f\n",val));
		Crash("acos");
		val = -1.000000F;
	}

	if(val == 1.0)
		return 0;

    index = val * 10000;

    table += index + 10000;
    ret = (*table);
    ret /= 20000;
	return ret;
}

/*	--------------------------------------------------------------------------------
	Function 	: acos(float val)
	Purpose 	: returns the arc cosine of a value (-1/1)
	Parameters 	: value
	Returns 	: float
	Info 		:
*/
float acosBetter(float val)
{
    int index;
    float ret,ret2,ratio;
    USHORT *table = (USHORT *)&acostab;

    if((val > 1) || (val < -1))
            Crash("ACOSBETTER");

    index = val * (1 / 0.0001);

	if(index == 10000)
		return 0;

    table += index + 10000;
    ret = (*table);
    ret /= 20000;

	if(val * 10000 > index)
	{
		ratio = val * 10000 - index;
		index++;
		if(index == 10000)
		{
			ret = ret * (1 - ratio);
		}
		else
		{
			table = (USHORT *)&acostab;
			table += index + 10000;
			ret2 = (*table);
			ret2 /= 20000;
			ret = ret * (1 - ratio) + ret2 * ratio;
		}
	}

	return ret;
}
#endif


#ifndef PC_VERSION
USHORT atanStep,maxAtanVal;
/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void InitAtan()
{
	USHORT *table = (USHORT *)&atantab;

	atanStep = *(table+1);
	maxAtanVal = (*table)/(atanStep);
}

/*	--------------------------------------------------------------------------------
	Function 	: Atan(float val)
	Purpose 	: returns the arc tangent of val
	Parameters 	: float val
	Returns 	: float
	Info 		: 
*/
float atan(float val)
{
	USHORT *table = (USHORT *)&atantab;
	float ret;

	if(val > maxAtanVal)
		val = maxAtanVal;

	table += 2 + (int)(val * atanStep);
	ret = *table;
	ret /= 40000;
	return ret;
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
float atan2(float z,float x)
{
	if((x == 0) && (z == 0))
		return 0;

	if(x > 0)
	{
		if(z > 0)
			return atan(z/x);
		else
			return -atan(-z/x);
	}
	else if(x < 0)
	{
		if(z > 0)
			return PI_OVER_2 + atan(-x/z);
		else if(z < 0)
			return -PI_OVER_2 - atan(x/z);
		else
			return PI;
	}
	else
	{
		if(z > 0)
			return PI_OVER_2;
		else
			return -PI_OVER_2;
	}
}
#endif

/*	--------------------------------------------------------------------------------
	Method : Slerp
	Purpose : interpolate between two quaternions
	Parameters : quaternion #1, quaternion #2, interpolant (normalised), dest quaternion
	Returns : 
	Info : 
*/

#define QEPSILON	((float)0.00001)
#define QHALFPI		((float)1.570796326794895)

void QuatSlerp(QUATERNION *src1, QUATERNION *sp2, float t, QUATERNION *dquat)
{
	float			omega, cosom, sinom, sclp, sclq, pdist,ndist;
	float			x,y,z,w;
	QUATERNION	negs2, *src2;

	x = src1->x - sp2->x;
	y = src1->y - sp2->y;
	z = src1->z - sp2->z;
	w = src1->w - sp2->w;
	pdist = x*x + y*y + z*z + w*w;

	x = src1->x + sp2->x;
	y = src1->y + sp2->y;
	z = src1->z + sp2->z;
	w = src1->w + sp2->w;
	ndist = x*x + y*y + z*z + w*w;

	if (pdist < ndist)
	{
		// +ve arc is shorter, so interpolate along that
		src2 = sp2;
	}
	else
	{
		// -ve arc is shorter, so interpolate along that
		negs2.x = -sp2->x;
		negs2.y = -sp2->y;
		negs2.z = -sp2->z;
		negs2.w = -sp2->w;
		src2 = &negs2;
	}

	cosom = src1->x*src2->x + src1->y*src2->y + src1->z*src2->z + src1->w*src2->w;

	if ((((float)1.0)+cosom) > QEPSILON)
	{
		if ((((float)1.0)-cosom) > QEPSILON)
		{
			omega = (float)acosBetter(cosom);
			sinom = (float)1.0/sinf(omega);
			sclp = (float)sinf(((float)1.0-t)*omega)*sinom;
			sclq = (float)sinf(t*omega)*sinom;
		}
		else
		{
			sclp = (float)1.0-t;
			sclq = t;
		}
		dquat->x = sclp*src1->x + sclq*src2->x;
		dquat->y = sclp*src1->y + sclq*src2->y;
		dquat->z = sclp*src1->z + sclq*src2->z;
		dquat->w = sclp*src1->w + sclq*src2->w;
	}
	else
	{
		dquat->x = -src1->y;
		dquat->y = src1->x;
		dquat->z = -src1->w;
		dquat->w = src1->z;
		sclp = (float)sinf(((float)1.0-t)*QHALFPI);
		sclq = (float)sinf(t*QHALFPI);
		dquat->x = sclp*src1->x + sclq*dquat->x;
		dquat->y = sclp*src1->y + sclq*dquat->y;
		dquat->z = sclp*src1->z + sclq*dquat->z;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
BOOL QuatSlerpAtFixedSpeed(QUATERNION *src1, QUATERNION *src2, float speed, QUATERNION *dquat)
{
	float tempFloat,tempFloat2;

	tempFloat = (src1->x - src2->x)*(src1->x - src2->x) + (src1->y - src2->y)*(src1->y - src2->y) + (src1->z - src2->z)*(src1->z - src2->z) + (src1->w - src2->w)*(src1->w - src2->w);
	tempFloat2 = (src1->x + src2->x)*(src1->x + src2->x) + (src1->y + src2->y)*(src1->y + src2->y) + (src1->z + src2->z)*(src1->z + src2->z) + (src1->w + src2->w)*(src1->w + src2->w);
	
	if(tempFloat2 > tempFloat)
		tempFloat = src1->x*src2->x + src1->y*src2->y + src1->z*src2->z + src1->w*src2->w;
	else
		tempFloat = -(src1->x*src2->x + src1->y*src2->y + src1->z*src2->z + src1->w*src2->w);

	tempFloat = Fabs(acos(tempFloat));

	if(tempFloat > speed)
	{
		QuatSlerp(src1,src2,speed/tempFloat,dquat);
		return NO;
	}
	else
	{
		SetQuaternion(dquat,src2);
		return YES;
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
BOOL QuatSlerpAtFixedSpeedOld(QUATERNION *src1, QUATERNION *src2, float speed, QUATERNION *dquat)
{
	float tempFloat,tempFloat2;

	tempFloat = (src1->x - src2->x)*(src1->x - src2->x) + (src1->y - src2->y)*(src1->y - src2->y) + (src1->z - src2->z)*(src1->z - src2->z) + (src1->w - src2->w)*(src1->w - src2->w);
	tempFloat2 = (src1->x + src2->x)*(src1->x + src2->x) + (src1->y + src2->y)*(src1->y + src2->y) + (src1->z + src2->z)*(src1->z + src2->z) + (src1->w + src2->w)*(src1->w + src2->w);
	
	if(tempFloat2 < tempFloat)
		tempFloat = src1->x*src2->x + src1->y*src2->y + src1->z*src2->z + src1->w*src2->w;
	else
		tempFloat = -(src1->x*src2->x + src1->y*src2->y + src1->z*src2->z + src1->w*src2->w);

	tempFloat = Fabs(acos(tempFloat));

	if(tempFloat > speed)
	{
		QuatSlerp(src1,src2,speed/tempFloat,dquat);
		return NO;
	}
	else
	{
		SetQuaternion(dquat,src2);
		return YES;
	}
}

//	--------------------------------------------------------------------------------
//	Function : QuaternionToMatrix
//	Purpose : Translate a stationary quaternion to a standard rotation matrix
//	Parameters : quaternion, matrix to fill
//	Returns : 
//	Info : 
void QuaternionToMatrix(QUATERNION *squat, MATRIX *dmatrix)
{
	float	s, xs,ys,zs, wx,wy,wz, xx,xy,xz, yy,yz,zz;

	s = ((float)2.0)/(squat->x*squat->x + squat->y*squat->y + squat->z*squat->z + squat->w*squat->w);

	xs = squat->x*s;
	ys = squat->y*s;
	zs = squat->z*s;

	wx = squat->w*xs;
	wy = squat->w*ys;
	wz = squat->w*zs;

	xx = squat->x*xs;
	xy = squat->x*ys;
	xz = squat->x*zs;

	yy = squat->y*ys;
	yz = squat->y*zs;
	zz = squat->z*zs;

	dmatrix->matrix[0][0] = ((float)1.0)-(yy+zz);
	dmatrix->matrix[0][1] = xy+wz;
	dmatrix->matrix[0][2] = xz-wy;

	dmatrix->matrix[1][0] = xy-wz;
	dmatrix->matrix[1][1] = ((float)1.0)-(xx+zz);
	dmatrix->matrix[1][2] = yz+wx;

	dmatrix->matrix[2][0] = xz+wy;
	dmatrix->matrix[2][1] = yz-wx;
	dmatrix->matrix[2][2] = ((float)1.0)-(xx+yy);

	dmatrix->matrix[0][3] = (float)0.0;
	dmatrix->matrix[1][3] = (float)0.0;
	dmatrix->matrix[2][3] = (float)0.0;
	dmatrix->matrix[3][3] = (float)1.0;
	dmatrix->matrix[3][0] = (float)0.0;
	dmatrix->matrix[3][1] = (float)0.0;
	dmatrix->matrix[3][2] = (float)0.0;
}

//	--------------------------------------------------------------------------------
//	Function : MatrixToQuaternion
//	Purpose : Translate a matrix to a stationary quaternion
//	Parameters : matrix, quaternion to fill
//	Returns : 
//	Info : 
void MatrixToQuaternion(MATRIX *smatrix, QUATERNION *dquat)
{
	float	tr, s, *fi,*fj,*fk;
	int		i,j,k;

	tr = smatrix->matrix[0][0]+smatrix->matrix[1][1]+smatrix->matrix[2][2];
	if (tr > (float)0.0)
	{
		s = sqrtf(tr+(float)1.0);
		dquat->w = s*(float)0.5;
		s = ((float)0.5)/s;

		dquat->x = (smatrix->matrix[1][2]-smatrix->matrix[2][1])*s;
		dquat->y = (smatrix->matrix[2][0]-smatrix->matrix[0][2])*s;
		dquat->z = (smatrix->matrix[0][1]-smatrix->matrix[1][0])*s;
	}
	else
	{
		if (smatrix->matrix[1][1] > smatrix->matrix[0][0])
		{
			if (smatrix->matrix[2][2] > smatrix->matrix[1][1])
			{
				fi = &(dquat->z); i = 2;
				fj = &(dquat->x); j = 0;
				fk = &(dquat->y); k = 1;
			}
			else
			{
				fi = &(dquat->y); i = 1;
				fj = &(dquat->z); j = 2;
				fk = &(dquat->x); k = 0;
			}
		}
		else
		{
			if (smatrix->matrix[2][2] > smatrix->matrix[0][0])
			{
				fi = &(dquat->z); i = 2;
				fj = &(dquat->x); j = 0;
				fk = &(dquat->y); k = 1;
			}
			else
			{
				fi = &(dquat->x); i = 0;
				fj = &(dquat->y); j = 1;
				fk = &(dquat->z); k = 2;
			}
		}

		s = sqrtf((smatrix->matrix[i][i]-(smatrix->matrix[j][j]+smatrix->matrix[k][k]))+(float)1.0);

		*fi = s*(float)0.5;
		s = ((float)0.5)/s;
		dquat->w = (smatrix->matrix[j][k]-smatrix->matrix[k][j])*s;
		*fj = (smatrix->matrix[i][j]+smatrix->matrix[j][i])*s;
		*fk = (smatrix->matrix[i][k]+smatrix->matrix[k][i])*s;
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: QuaternionMultiply()
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		: destination CAN be same as source
*/
void QuaternionMultiply(QUATERNION *dest,QUATERNION *src1,QUATERNION *src2)
{
	float dp;
	QUATERNION temp;

	dp = src1->x*src2->x + src1->y*src2->y + src1->z*src2->z;

	temp.w = src1->w*src2->w - dp;
	temp.x = src1->w*src2->x + src2->w*src1->x + src1->y*src2->z - src1->z*src2->y;
	temp.y = src1->w*src2->y + src2->w*src1->y + src1->z*src2->x - src1->x*src2->z;
	temp.z = src1->w*src2->z + src2->w*src1->z + src1->x*src2->y - src1->y*src2->x;

	memcpy(dest,&temp,sizeof(QUATERNION));
}

/*	--------------------------------------------------------------------------------
	Function 	: GetRotationFromQuaternion()
	Purpose 	: converts a quaternion into a rotation of theta degrees about an axis
	Parameters 	: destination, source
	Returns 	: nun
	Info 		:
*/
void GetRotationFromQuaternion(QUATERNION *destQ,QUATERNION *srcQ)
{
	float theta,sinThetaOver2,m;

	theta = 2 * acos(srcQ->w);
	sinThetaOver2 = sinf(theta/2);
	
	destQ->w = theta;
	if(sinThetaOver2)
	{
		destQ->x = srcQ->x/sinThetaOver2;
		destQ->y = srcQ->y/sinThetaOver2;
		destQ->z = srcQ->z/sinThetaOver2;

		m = sqrtf(destQ->x*destQ->x + destQ->y*destQ->y + destQ->z*destQ->z);
		if(m)
		{
			destQ->x /= m;
			destQ->y /= m;
			destQ->z /= m;
		}
	}
	else
	{
		destQ->x = 0;
		destQ->y = 1;
		destQ->z = 0;
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: GetQuaternionFromRotation()
	Purpose 	: converts from a rotation of theta about an axis to a quaternion
	Parameters 	: dest, source
	Returns 	: nun
	Info 		:
*/
void GetQuaternionFromRotation(QUATERNION *destQ,QUATERNION *srcQ)
{
	float thetaOver2;
	float sinThetaOver2;

	thetaOver2 = srcQ->w/2;
	sinThetaOver2 = sinf(thetaOver2);

	destQ->w = cosf(thetaOver2);
	destQ->x = sinThetaOver2 * srcQ->x;
	destQ->y = sinThetaOver2 * srcQ->y;
	destQ->z = sinThetaOver2 * srcQ->z;
}

/*	--------------------------------------------------------------------------------
	Function 	: AddOneScaledVector()
	Purpose 	: 
	Parameters 	: pointers to resultant and two operand vectors, two scale factors
	Returns 	: none
	Info 		: destination CAN be same as source
*/
void AddOneScaledVector(VECTOR *result,VECTOR *operand1,VECTOR *operand2,float scale2)
{
	result->v[X] = operand1->v[X] + operand2->v[X]*scale2;
	result->v[Y] = operand1->v[Y] + operand2->v[Y]*scale2;
	result->v[Z] = operand1->v[Z] + operand2->v[Z]*scale2;
}

/*	--------------------------------------------------------------------------------
	Function 	: AddToOneScaledVector()
	Purpose 	: 
	Parameters 	: pointers to resultant and two operand vectors, two scale factors
	Returns 	: none
	Info 		: destination CAN be same as source
*/
void AddToOneScaledVector(VECTOR *result,VECTOR *operand2,float scale2)
{
	result->v[X] = result->v[X] + operand2->v[X]*scale2;
	result->v[Y] = result->v[Y] + operand2->v[Y]*scale2;
	result->v[Z] = result->v[Z] + operand2->v[Z]*scale2;
}

/*	--------------------------------------------------------------------------------
	Function 	: AddToOneScaledVector()
	Purpose 	: 
	Parameters 	: pointers to resultant and two operand vectors, two scale factors
	Returns 	: none
	Info 		: destination CAN be same as source
*/
void AddToOneScaledVector2D(VECTOR *result,VECTOR *operand2,float scale2)
{
	result->v[X] = result->v[X] + operand2->v[X]*scale2;
	result->v[Z] = result->v[Z] + operand2->v[Z]*scale2;
}

/*	--------------------------------------------------------------------------------
	Function 	: AddOneScaledVector()
	Purpose 	: adds 2 vectors together, multilpying each by a scaling factor into a resultant vector
	Parameters 	: pointers to resultant and two operand vectors, two scale factors
	Returns 	: none
	Info 		: destination CAN be same as source
*/
void AddOneScaledVector2D(VECTOR *result,VECTOR *operand1,VECTOR *operand2,float scale2)
{
	result->v[X] = operand1->v[X] + operand2->v[X]*scale2;
	result->v[Z] = operand1->v[Z] + operand2->v[Z]*scale2;
}

/*	--------------------------------------------------------------------------------
	Function 	: DistanceBetweenPoints2D()
	Purpose 	: finds the distance between two points
	Parameters 	: vector 1, vector 2
	Returns 	: distance
	Info 		: destination CAN be same as source
*/
float DistanceBetweenPoints2D(VECTOR *v1, VECTOR *v2)
{
	VECTOR tempVect;

	SubVector2D(&tempVect,v1,v2);
	return Magnitude2D(&tempVect);
}

/*	--------------------------------------------------------------------------------
	Function 	: DistanceBetweenPoints2D()
	Purpose 	: finds the distance between two points
	Parameters 	: vector 1, vector 2
	Returns 	: distance
	Info 		: destination CAN be same as source
*/
float DistanceBetweenPoints2DSquared(VECTOR *v1, VECTOR *v2)
{
	VECTOR tempVect;

	SubVector2D(&tempVect,v1,v2);
	return MagnitudeSquared2D(&tempVect);
}

/*	--------------------------------------------------------------------------------
	Function 	: DistanceBetweenPoints()
	Purpose 	: finds the distance between two points
	Parameters 	: vector 1, vector 2
	Returns 	: distance
	Info 		:
*/
float DistanceBetweenPoints(VECTOR *v1, VECTOR *v2)
{
	VECTOR tempVect;

	SubVector(&tempVect,v1,v2);
	return Magnitude(&tempVect);
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
float DistanceBetweenPointsSquared(VECTOR *v1, VECTOR *v2)
{
	VECTOR tempVect;

	SubVector(&tempVect,v1,v2);
	return MagnitudeSquared(&tempVect);
}
/*	--------------------------------------------------------------------------------
	Function 	: IsPointWithinBox
	Purpose 	: determine whether a point is within a box
	Parameters 	: box* vector*
	Returns 	: BOOL
	Info 		:
*/
BOOL IsPointWithinBox(BOX *box, VECTOR *pos)
{
	if((pos->v[X] >= box->pos.v[X]) && (pos->v[X] <= box->pos.v[X] + box->size.v[X]) &&
	   (pos->v[Y] >= box->pos.v[Y]) && (pos->v[Y] <= box->pos.v[Y] + box->size.v[Y]) &&
	   (pos->v[Z] >= box->pos.v[Z]) && (pos->v[Z] <= box->pos.v[Z] + box->size.v[Z]))
	   return TRUE;

	return FALSE;
}

/*	--------------------------------------------------------------------------------
	Function 	: IsPointWithinBox
	Purpose 	: determine whether a point is within a box
	Parameters 	: box* vector*
	Returns 	: BOOL
	Info 		:
*/
BOOL IsPointWithinBoxPlusHeight(BOX *box, VECTOR *pos,float height)
{
	if((pos->v[X] >= box->pos.v[X]) && (pos->v[X] <= box->pos.v[X] + box->size.v[X]) &&
	   (pos->v[Y] >= box->pos.v[Y]) && (pos->v[Y] <= box->pos.v[Y] + box->size.v[Y] + height) &&
	   (pos->v[Z] >= box->pos.v[Z]) && (pos->v[Z] <= box->pos.v[Z] + box->size.v[Z]))
	   return TRUE;

	return FALSE;
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
BOOL IsPointWithinBox2D(BOX *box, VECTOR *pos)
{
	if((pos->v[X] >= box->pos.v[X]) && (pos->v[X] <= box->pos.v[X] + box->size.v[X]) &&
	   (pos->v[Z] >= box->pos.v[Z]) && (pos->v[Z] <= box->pos.v[Z] + box->size.v[Z]))
	   return TRUE;

	return FALSE;
}

/*	--------------------------------------------------------------------------------
	Function 	: RotateVector2D
	Purpose 	: rotates the X and Z points around the Y axis, by given angle
	Parameters 	: result vector, source vector, angle
	Returns 	: 
	Info 		: destination CAN be same as source
*/
void RotateVector2D(VECTOR *dest, VECTOR *source, float angle)
{
	VECTOR temp;
	float cosangle;
	float sinangle;

	angle = -angle;

	cosangle = cosf(angle);
	sinangle = sinf(angle);

	temp.v[Y] = source->v[Y];
	temp.v[X] = (cosangle * source->v[X]) + ((-sinangle) * source->v[Z]);
	temp.v[Z] = (cosangle * source->v[Z]) + (sinangle * source->v[X]);

	SetVector(dest, &temp);
}

/*	--------------------------------------------------------------------------------
	Function 	: RotateVector2D
	Purpose 	: rotates the X and Z points around the Y axis, by given angle
	Parameters 	: result vector, source vector, angle
	Returns 	: 
	Info 		: destination CAN be same as source
*/
void RotateVector2DXYZ(VECTOR *dest, VECTOR *source, float angle, BYTE axis)
{
	VECTOR	temp;
	BYTE	x,z;

	switch(axis)
	{
		case X:
			x = Y;
			z = Z;
			break;
		case Y:
			x = Z;
			z = X;
			break;
		case Z:
			x = X;
			z = Y;
			break;
	}

	temp.v[axis] = source->v[axis];
	temp.v[x] = (cosf(angle) * source->v[x]) + (-sinf(angle) * source->v[z]);
	temp.v[z] = (cosf(angle) * source->v[z]) + (sinf(angle) * source->v[x]);

	SetVector(dest, &temp);
}
/*	--------------------------------------------------------------------------------
	Function 	: FlipVector()
	Purpose 	: flips a vector
	Parameters 	: pointer to vector
	Returns 	: none
	Info 		: destination CAN be same as source
*/
void FlipVector(VECTOR *vector)
{
	vector->v[X] = -vector->v[X];
	vector->v[Y] = -vector->v[Y];
	vector->v[Z] = -vector->v[Z];
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void FlipVector2D(VECTOR *vector)
{
	vector->v[X] = -vector->v[X];
	vector->v[Z] = -vector->v[Z];
}
/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
//void SetQuaternion(QUATERNION *q1,QUATERNION *q2)
//{
//	q1->x = q2->x;
//	q1->y = q2->y;
//	q1->z = q2->z;
//	q1->w = q2->w;
//}
/*	--------------------------------------------------------------------------------
	Function 	: SetVector2D()
	Purpose 	: sets one vector equal to another - only in X and Z axis
	Parameters 	: pointer to result vector, pointer to original vector
	Returns 	: none
	Info 		:
*/
void SetVector2D(VECTOR *v1,VECTOR *v2)
{
	v1->v[X] = v2->v[X];
	v1->v[Z] = v2->v[Z];
}

/*	--------------------------------------------------------------------------------
	Function 	: AddVector()
	Purpose 	: adds 2 vectors together into a resultant vector
	Parameters 	: pointers to resultant and two operand vectors
	Returns 	: none
	Info 		: destination CAN be same as source
*/
//void AddVector(VECTOR *result,VECTOR *operand1,VECTOR *operand2)
//{
//	result->v[X] = operand1->v[X] + operand2->v[X];
//	result->v[Y] = operand1->v[Y] + operand2->v[Y];
//	result->v[Z] = operand1->v[Z] + operand2->v[Z];
//}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
//void AddToVector(VECTOR *result,VECTOR *operand)
//{
//	result->v[X] += operand->v[X];
//	result->v[Y] += operand->v[Y];
//	result->v[Z] += operand->v[Z];
//}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void AddToVector2D(VECTOR *result,VECTOR *operand)
{
	result->v[X] += operand->v[X];
	result->v[Z] += operand->v[Z];
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
//void SubFromVector(VECTOR *result,VECTOR *operand)
//{
//	result->v[X] -= operand->v[X];
//	result->v[Y] -= operand->v[Y];
//	result->v[Z] -= operand->v[Z];
//}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void SubFromVector2D(VECTOR *result,VECTOR *operand)
{
	result->v[X] -= operand->v[X];
	result->v[Z] -= operand->v[Z];
}

/*	--------------------------------------------------------------------------------
	Function 	: AddVector2D()
	Purpose 	: adds 2 vectors together into a resultant vector
	Parameters 	: pointers to resultant and two operand vectors
	Returns 	: none
	Info 		:
*/
void AddVector2D(VECTOR *result,VECTOR *operand1,VECTOR *operand2)
{
	result->v[X] = operand1->v[X] + operand2->v[X];
	result->v[Z] = operand1->v[Z] + operand2->v[Z];
}

/*	--------------------------------------------------------------------------------
	Function 	: AddScaledVector()
	Purpose 	: adds 2 vectors together, multilpying each by a scaling factor into a resultant vector
	Parameters 	: pointers to resultant and two operand vectors, two scale factors
	Returns 	: none
	Info 		:
*/
void AddScaledVector(VECTOR *result,VECTOR *operand1,float scale1,VECTOR *operand2,float scale2)
{
	result->v[X] = operand1->v[X]*scale1 + operand2->v[X]*scale2;
	result->v[Y] = operand1->v[Y]*scale1 + operand2->v[Y]*scale2;
	result->v[Z] = operand1->v[Z]*scale1 + operand2->v[Z]*scale2;
}

/*	--------------------------------------------------------------------------------
	Function 	: AddScaledVector2D()
	Purpose 	: adds 2 vectors together, multilpying each by a scaling factor into a resultant vector
	Parameters 	: pointers to resultant and two operand vectors, two scale factors
	Returns 	: none
	Info 		:
*/
void AddScaledVector2D(VECTOR *result,VECTOR *operand1,float scale1,VECTOR *operand2,float scale2)
{
	result->v[X] = operand1->v[X]*scale1 + operand2->v[X]*scale2;
	result->v[Z] = operand1->v[Z]*scale1 + operand2->v[Z]*scale2;
}

/*	--------------------------------------------------------------------------------
	Function 	: SubVector()
	Purpose 	: subtracts one vector from another
	Parameters 	: pointers to resultant and two operand vectors
	Returns 	: none
	Info 		:
*/
//void SubVector(VECTOR *result,VECTOR *operand1,VECTOR *operand2)
//{
//	result->v[X] = operand1->v[X] - operand2->v[X];	
//	result->v[Y] = operand1->v[Y] - operand2->v[Y];	
//	result->v[Z] = operand1->v[Z] - operand2->v[Z];	
//}

/*	--------------------------------------------------------------------------------
	Function 	: SubVector2D()
	Purpose 	: subtracts one vector from another
	Parameters 	: pointers to resultant and two operand vectors
	Returns 	: none
	Info 		:
*/
void SubVector2D(VECTOR *result,VECTOR *operand1,VECTOR *operand2)
{
	result->v[X] = operand1->v[X] - operand2->v[X];	
	result->v[Z] = operand1->v[Z] - operand2->v[Z];	
}

/*	--------------------------------------------------------------------------------
	Function 	: FindShortestAngle()
	Purpose 	: returns the smallest angle between 2 directions (rads)
	Parameters 	: angle1, angle2
	Returns 	: angle
	Info 		:
*/
float FindShortestAngle(float val1, float val2)
{
	float temp;

	if(Fabs(val1 - val2) > PI)	
	{
		if(val1 > val2)
		{
			temp = val2 - val1;
		}
		else
		{
			temp = val1 - val2;
		}
		temp += PI * 2;
		return temp;
	}
	else
	{
	   return Fabs(val1 - val2);
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
float FindShortestAngleSigned(float ang1,float ang2)
{
	float temp = ang1 - ang2;
	if((temp > -PI) && (temp <= PI))
		return temp;
	else if(temp > PI)
		return ang1 - ang2 - 2*PI;
	else
		return ang1 + 2*PI - ang2;
}

/*	--------------------------------------------------------------------------------
	Function 	: Magnitude()
	Purpose 	: find the magnitude of a vector
	Parameters 	: pointer to vector
	Returns 	: magnitude
	Info 		:
*/
//float Magnitude(VECTOR *vect)
//{
//	return sqrtf(vect->v[X]*vect->v[X] + vect->v[Y]*vect->v[Y] + vect->v[Z]*vect->v[Z]);
//}

/*	--------------------------------------------------------------------------------
	Function 	: Magnitude()
	Purpose 	: find the magnitude of a vector
	Parameters 	: pointer to vector
	Returns 	: magnitude
	Info 		:
*/
/*
float MagnitudeSquared(VECTOR *vect)
{
	return (vect->v[X]*vect->v[X] + vect->v[Y]*vect->v[Y] + vect->v[Z]*vect->v[Z]);
}
*/
/*	--------------------------------------------------------------------------------
	Function 	: Magnitude2D()
	Purpose 	: find the 2-dimensional (XZ) magnitude of a vector
	Parameters 	: pointer to vector
	Returns 	: magnitude
	Info 		:
*/
//float Magnitude2D(VECTOR *vect)
//{
//	return sqrtf(vect->v[X]*vect->v[X] + vect->v[Z]*vect->v[Z]);
//}

/*	--------------------------------------------------------------------------------
	Function 	: MakeUnit()
	Purpose 	: makes a vector unit magnitude
	Parameters 	: pointer to vector
	Returns 	: none
	Info 		:
*/
void MakeUnit(VECTOR *vect)
{
	float m = Magnitude(vect);

	if(m != 0)
	{
		vect->v[X] /= m;
		vect->v[Y] /= m;
		vect->v[Z] /= m;
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: MakeUnit()
	Purpose 	: makes a vector unit magnitude
	Parameters 	: pointer to vector
	Returns 	: none
	Info 		:
*/
void MakeUnit2D(VECTOR *vect)
{
	float m = Magnitude2D(vect);

	if(m != 0)
	{
		vect->v[X] /= m;
		vect->v[Z] /= m;
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: ScaleVector()
	Purpose 	: multiplies a vector's length by a scaling factor
	Parameters 	: pointer to vector, scale
	Returns 	: none
	Info 		:
*/
//void ScaleVector(VECTOR *vect,float scale)
//{
//	vect->v[X] *= scale;
//	vect->v[Y] *= scale;
//	vect->v[Z] *= scale;
//}
/*	--------------------------------------------------------------------------------
	Function 	: ScaleVector2D()
	Purpose 	: multiplies a vector's length by a scaling factor
	Parameters 	: pointer to vector, scale
	Returns 	: none
	Info 		:
*/
//void ScaleVector2D(VECTOR *vect,float scale)
//{
//	vect->v[X] *= scale;
//	vect->v[Z] *= scale;
//}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void RoundToZero(float *f)
{
	if(Fabs(*f) < 0.0001)
		*f = 0;
}

/*	--------------------------------------------------------------------------------
	Function 	: Aabs()
	Purpose 	: calculates a valid angle (makes sure it is between 0 and PI*2)
	Parameters 	: angle
	Returns 	: absolute angle
	Info 		:
*/
float Aabs(float angle)
{
	angle = angle > (2 * PI) ? angle - 2 * PI : angle;
	angle = angle < 0 ? angle += 2 * PI : angle;

	return angle;
}

/*	--------------------------------------------------------------------------------
	Function 	: arse
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
float SineWave(float freq,float phase,int base)
{
	float temp = base + phase;
	return (sinf(temp / freq));
}

float SineWave2(float freq,float phase)
{
	return (sinf((2 * PI * phase) / freq));
}


void SetVectorLength(VECTOR *vect,float length)
{
	float m = Magnitude(vect)/length;

	if(m != 0)
	{
		vect->v[X] /= m;
		vect->v[Y] /= m;
		vect->v[Z] /= m;
	}
}


//*************************************************************************************
//*************************************************************************************
//*************************************************************************************
//matrix functions follow


/*	--------------------------------------------------------------------------------
	Function 	: InitMatrixStack()
	Purpose 	: clears the matrix stack and resets the position pointer
	Parameters 	: none
	Returns 	: none
	Info 		:
*/
void InitMatrixStack()
{
	int	x, y, i;
	
	matrixStack.stackPosition = 0;
	for(i = 0; i < MAX_STACK_SIZE; i++)
	{
		for(x = 0; x < 4; x++)
		{
			for(y = 0; y < 4; y++)
			{
				matrixStack.stack[i][x][y] = 0;
			}
		}
	}
	
	//set first matrix in stack to be a unit matrix
	matrixStack.stack[0][0][0] = 1.0;
	matrixStack.stack[0][1][1] = 1.0;
	matrixStack.stack[0][2][2] = 1.0;
	matrixStack.stack[0][3][3] = 1.0;


}

/*	--------------------------------------------------------------------------------
	Function 	: InitMatrixStack()
	Purpose 	: clears the matrix stack and resets the position pointer
	Parameters 	: none
	Returns 	: none
	Info 		:
*/
void InitRMatrixStack()
{
	int	x, y, i;
	
	rMatrixStack.stackPosition = 0;
	for(i = 0; i < MAX_STACK_SIZE; i++)
	{
		for(x = 0; x < 4; x++)
		{
			for(y = 0; y < 4; y++)
			{
				rMatrixStack.stack[i][x][y] = 0;
			}
		}
	}
	
	//set first matrix in stack to be a unit matrix
	rMatrixStack.stack[0][0][0] = 1.0;
	rMatrixStack.stack[0][1][1] = 1.0;
	rMatrixStack.stack[0][2][2] = 1.0;
	rMatrixStack.stack[0][3][3] = 1.0;
}



/*	--------------------------------------------------------------------------------
	Function 	: SetMatrix()
	Purpose 	: copies one matrix into another
	Parameters 	: float*, float*
	Returns 	: none
	Info 		:
*/
//void SetMatrix(float *dest, float *source)
//{
//	char x;
//
//	for(x = 0; x < 4 * 4; x++)
//		*(dest++) = *(source++);
//}
/*	--------------------------------------------------------------------------------
	Function 	: LoadMatrix()
	Purpose 	: replaces the matrix at the top of the stack with this one 
	Parameters 	: float *
	Returns 	: none
	Info 		:
*/
void LoadMatrix(float *matrix)
{
	if(matrixStack.stackPosition < 0)
		matrixStack.stackPosition = 0;

	SetMatrix(*(matrixStack.stack[matrixStack.stackPosition]), matrix);
}

/*	--------------------------------------------------------------------------------
	Function 	: PushMatrix()
	Purpose 	: pushes a matrix onto the stack
	Parameters 	: float *
	Returns 	: none
	Info 		:
*/
/*void PushMatrix(float *matrix)
{
	char sPos;
	
	matrixStack.stackPosition++;
	sPos = matrixStack.stackPosition;
/*
	if(sPos > MAX_STACK_SIZE)
	{
		dprintf"MatrixStack overflow\n"));
		Crash("PUSH MATRIX");
	}
*/
/*	SetMatrix(*(matrixStack.stack[sPos]), matrix);

	guMtxCatF(matrixStack.stack[sPos], matrixStack.stack[sPos-1], matrixStack.stack[sPos]);


}
  */

/*	--------------------------------------------------------------------------------
	Function 	: PopMatrix()
	Purpose 	: pops a matrix from the top of the stack
	Parameters 	: none
	Returns 	: none
	Info 		:
*/
/*void PopMatrix()
{
	if(matrixStack.stackPosition == -1)
	{
		dprintf"MatrixStack underflow\n"));
		Crash("POP MATRIX");
	}

	matrixStack.stackPosition--;

}
  */
/*	--------------------------------------------------------------------------------
	Function 	: ComputeResultMatrix()
	Purpose 	: multiplies all the matrices currently on the stack, puts result into .result
	Parameters 	: none
	Returns 	: none
	Info 		:
*/
void ComputeResultMatrix()
{
	int	x;
		
	SetMatrix(*matrixStack.result, *matrixStack.stack[matrixStack.stackPosition]);
//	SetMatrix(*matrixStack.result, *matrixStack.stack[0]);
	return;
	for(x = 1; x <= matrixStack.stackPosition; x++)
	{
		guMtxCatF(matrixStack.stack[x], matrixStack.result, matrixStack.result);
	}


}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
/*
int IntersectEdges(float *x,float *y,POINT2D *v1,POINT2D *v2,POINT2D *v3,POINT2D *v4)
{
	float hi, lo, ax, bx, cx, ay, by, cy, d, e, f;


	ax = v2->x - v1->x;
	bx = v3->x - v4->x;

	if (ax<(float)0.0) 							// X bound box test
	{
		lo = v2->x;
		hi = v1->x;
	}
	else
	{
		hi = v2->x;
		lo = v1->x;
	}
	if (bx>(float)0.0)
	{
		if ((hi<v4->x)||(v3->x<lo))
			return 0;
	}
	else
	{
		if ((hi<v3->x)||(v4->x<lo))
			return 0;
	}

	ay = v2->z - v1->z;
	by = v3->z - v4->z;

	if (ay<(float)0.0)						// Y bound box test
	{
		lo = v2->z;
		hi = v1->z;
	}
	else
	{
		hi = v2->z;
		lo = v1->z;
	}
	if (by>(float)0.0)
	{
		if ((hi < v4->z) || (v3->z<lo))
			return 0;
	}
	else
	{
		if ((hi<v3->z) || (v4->z<lo))
			return 0;
	}

	cx = v1->x - v3->x;
	cy = v1->z - v3->z;
	d = by * cx - bx * cy;						// Calculate line fraction
	f = ay * bx - ax * by;
	if (f>(float)0.0)							// On line tests
	{
		if ((d < (float)0.0) || (d > f))
			return 0;
	}
	else
	{
		if ((d > (float)0.0) || (d < f))
			return 0;
	}
	e = ax * cy - ay * cx;
	if (f > (float)0.0)
	{
		if ((e < (float)0.0) || (e > f))
			return 0;
	}
	else
	{
		if ((e > (float)0.0) || (e < f))
			return 0;
	}
	if (f == (float)0.0)						// Coolinear test
		return 0;

	*x = v1->x + (d * ax) / f;					// Intersection point
	*y = v1->z + (d * ay) / f;
	return 1;
}
*/

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
int lookforbug = -1;
void RotateVectorByQuaternion(VECTOR *result,VECTOR *vect,QUATERNION *q)
{
	QUATERNION rot;

	GetRotationFromQuaternion(&rot,q);
	RotateVectorByRotation(result,vect,&rot);
}
/*	--------------------------------------------------------------------------------
	Function 	: RotateVectorByQuaternion
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		: destination CAN be same as source
*/
void RotateVectorByRotation(VECTOR *result,VECTOR *vect,QUATERNION *rot)
{
	float m,n,sinTheta,cosTheta;
	VECTOR mVec,pVec,vVec;

	m = vect->v[X]*rot->x;
	m += vect->v[Y]*rot->y;
	m += vect->v[Z]*rot->z;

	mVec.v[X] = m*rot->x;				
	mVec.v[Y] = m*rot->y;
	mVec.v[Z] = m*rot->z;				

	SubVector(&pVec,vect,&mVec);

	CrossProduct(&vVec,(VECTOR *)rot,&pVec);

	m = Magnitude(&pVec);

	if(m == 0)
		SetVector(result,vect);
	else
	{
		n = Magnitude(&vVec);
		if(n)
		{
			m /= n;
			ScaleVector(&vVec,m);
		}

		cosTheta = cosf(rot->w);
		sinTheta = sinf(rot->w);

		result->v[X] = mVec.v[X] + cosTheta * pVec.v[X] + sinTheta * vVec.v[X];
		result->v[Y] = mVec.v[Y] + cosTheta * pVec.v[Y] + sinTheta * vVec.v[Y];
		result->v[Z] = mVec.v[Z] + cosTheta * pVec.v[Z] + sinTheta * vVec.v[Z];
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void NormalToQuaternion(QUATERNION *q,VECTOR *normal)
{
	if(normal->v[X])
	{
		q->z = sqrtf(1/(1 + (normal->v[Z]*normal->v[Z])/(normal->v[X]*normal->v[X])));
		if(normal->v[X] > 0)
			q->z = -q->z;
		q->x = -(normal->v[Z]*q->z) / normal->v[X];
		q->w = acos(normal->v[Y])/2;

		q->x *= sinf(q->w);
		q->z *= sinf(q->w);
		q->w = cosf(q->w);
	}
	else if(normal->v[Z])
	{
		q->z = 0;
		q->w = acos(normal->v[Y])/2;

		if(normal->v[Z] > 0)
			q->x = sinf(q->w);
		else
			q->x = -sinf(q->w);
		q->w = cosf(q->w);
	}
	else if(normal->v[Y] > 0)
	{
		q->x = 0;
		q->z = 0;
		q->w = 1;
	}
	else
	{
		q->x = 0;
		q->z = -1;
		q->w = 0;
	}
	q->y = 0;
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void CalculateQuatForPlane(ACTOR *actor,QUATERNION *qAim,PLANE *plane)
{
	QUATERNION tempRot,tempQ;
	VECTOR tempVect;

	if(plane->normal.v[Y] == -1)
	{
		vertQ.w = Aabs(2*PI-actor->rot.v[Y]);// + PI);
		tempRot.w = PI;
		tempRot.x = 1;
		tempRot.y = 0;
		tempRot.z = 0;
	}
	else
	{
		vertQ.w = actor->rot.v[Y];
		SetVector2D(&tempVect,&plane->normal);
		tempVect.v[Y] = 0;
		MakeUnit2D(&tempVect);
		CrossProductVertical((VECTOR *)&tempRot,&tempVect);
		tempRot.w = acos(plane->normal.v[Y]);
	}
	GetQuaternionFromRotation(qAim,&vertQ);
	GetQuaternionFromRotation(&tempQ,&tempRot);
	QuaternionMultiply(qAim,&tempQ,qAim);
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void CalculateQuatForPlane2(float yRot,QUATERNION *qAim,VECTOR *normal)
{
	QUATERNION tempRot,tempQ;
	VECTOR tempVect;
	
	if(normal->v[Y] == -1)
	{
		vertQ.w = Aabs(2*PI-yRot);// + PI);
		tempRot.w = PI;
		tempRot.x = 1;
		tempRot.y = 0;
		tempRot.z = 0;
	}
	else
	{
		vertQ.w = 0;	//yRot;
		SetVector2D(&tempVect,normal);
		tempVect.v[Y] = 0;
		MakeUnit2D(&tempVect);
		CrossProductVertical((VECTOR *)&tempRot,&tempVect);
		tempRot.w = acos(normal->v[Y]);
	}
	GetQuaternionFromRotation(qAim,&vertQ);
	GetQuaternionFromRotation(&tempQ,&tempRot);
	QuaternionMultiply(qAim,&tempQ,qAim);
}


/*	--------------------------------------------------------------------------------
	Function 	: OrientateQuaternion
	Purpose 	: Creates an orientation as a quaternion from a forward and an up vector
	Parameters 	: QUATERNION*, up, forwards
	Returns 	: 
	Info 		: up and fwd should be UNIT vectors or it won't work!
*/
void OrientateQuaternion(QUATERNION *q, VECTOR *fwd, VECTOR *up)
{
	float mag, thetaOver2, sinThetaOver2;
	VECTOR axis;

	CrossProduct(&axis, fwd, up);		// axis is perp to two vectors
	mag = Magnitude(&axis);

	if (mag != 0)
	{
		ScaleVector(&axis, 1.0f/mag);

		thetaOver2 = (float)acos(mag) * 0.5f;
		sinThetaOver2 = sinf(thetaOver2);

		q->x = sinThetaOver2 * fwd->v[0];
		q->y = sinThetaOver2 * fwd->v[1];
		q->z = sinThetaOver2 * fwd->v[2];
		q->w = cosf(thetaOver2);
	}
	else
	{
		*q = zeroQuat;
	}
}

#ifdef PC_VERSION	//----------------------------------------------------------------------------


void guRotateF(MATRIX *m1,MATRIX *m2,MATRIX *m3)
{
}

void guMtxF2L(MATRIX *m1,MATRIX *m2,MATRIX *m3)
{
}

void guMtxIdent (float a[4][4])
{
	int i,j;
	for (i=0; i<4; i++)
		for (j=0; j<4; j++)
			a[i][j] = (i==j) ? 1.0f : 0.0f;
}

void guScaleF(float a[4][4], float dx, float dy, float dz)
{
	guMtxIdent (a);
	
	a[0][0] = dx;
	a[1][1] = dy;
	a[2][2] = dz;
}

void guTranslateF(float a[4][4], float dx, float dy, float dz)
{
	guMtxIdent (a);
	
	a[3][0] = dx;
	a[3][1] = dy;
	a[3][2] = dz;		
}

void guMtxCatF(float b[4][4], float a[4][4], float ret[4][4])
{
	float adj[4][4]={0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
	int i,j,k;

 	for (i=0; i<4; i++) 
		for (j=0; j<4; j++) 
			for (k=0; k<4; k++) 
				adj[i][j] += a[k][j] * b[i][k];

	memcpy (ret,adj,sizeof(float)*16);
}

void guMtxXFMF(float m[4][4],float srcX,float srcY,float srcZ,float *destX,float *destY,float *destZ)
{
	*destX = (m[0][0]*srcX)+(m[1][0]*srcY)+(m[2][0]*srcZ)+(m[3][0]);
	*destY = (m[0][1]*srcX)+(m[1][1]*srcY)+(m[2][1]*srcZ)+(m[3][1]);
	*destZ = (m[0][2]*srcX)+(m[1][2]*srcY)+(m[2][2]*srcZ)+(m[3][2]);
}

void guNormalise (float *x, float *y, float *z)
{
	float length = sqrt((*x)*(*x)+(*y)*(*y)+(*z)*(*z));
	(*x)/=length;
	(*y)/=length;
	(*z)/=length;
}

void guLookAtF (float m[4][4],
				float xEye, float yEye, float zEye,
				float xAt, float yAt, float zAt,
				float xUp, float yUp, float zUp)
{
    VECTOR  up, right, view_dir,world_up,from;
    float dx,dy,dz;

	guMtxIdent (m);

	view_dir.v[X] = xEye - xAt;
	view_dir.v[Y] = yEye - yAt;
	view_dir.v[Z] = zEye - zAt;
	
	world_up.v[X] = xUp;
	world_up.v[Y] = yUp;
	world_up.v[Z] = zUp;
	
	from.v[X] = xAt;
	from.v[Y] = yAt;
	from.v[Z] = zAt;
	
	MakeUnit(&view_dir);
    
	CrossProduct(&right, &world_up, &view_dir);
    CrossProduct(&up, &view_dir, &right);
    
    MakeUnit(&right);
    MakeUnit(&up);
    
    m[0][0] = right.v[X];
    m[1][0] = right.v[Y];
    m[2][0] = right.v[Z];
    m[0][1] = up.v[X];
    m[1][1] = up.v[Y];
    m[2][1] = up.v[Z];
    m[0][2] = view_dir.v[X];
    m[1][2] = view_dir.v[Y];
    m[2][2] = view_dir.v[Z];
    m[3][0] = -DotProduct(&right, &from);
    m[3][1] = -DotProduct(&up, &from);
    m[3][2] = -DotProduct(&view_dir, &from);
    
    //if (roll != 0.0f) 
	//    view = MatrixMult(RotateZ(-roll), view); 
}

#endif	//----------------------------------------------------------------------------------------
