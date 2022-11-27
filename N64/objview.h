/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: objview.h
	Programmer	: Andrew Eder
	Date		: 11/10/98

----------------------------------------------------------------------------------------------- */

#ifndef OBJVIEW_H_INCLUDED
#define OBJVIEW_H_INCLUDED

#define OBJVIEW_MAXMODES	2

#define SPEED_UP			0.025
#define SPEED_DOWN			-0.025


//extern int			objectMatrix;

// ----- [ STRUCTURE TO KEEP TOGETHER VALUES USED FOR OBJECT VIEWING ] ---------- //

typedef struct _OBJECT_VIEWER
{
	ACTOR2			*currObj;
	
	unsigned int	mode;						// Object viewer mode
	unsigned int	viewMode;					// Object viewing mode

	int				currWorldID;				// ID of current world
	int				currLevelID;				// ID of current level
	int				currObjNum;					// Current number of object for viewing
	int				numObjects;					// Number of objects for viewing

	VECTOR			objPos;						// Object position

	int				rotateAboutX;				// Angle of rotation about the x-axis
	int				rotateAboutY;				// Angle of rotation about the y-axis
	int				rotateAboutZ;				// Angle of rotation about the z-axis

} OBJECT_VIEWER;

extern OBJECT_VIEWER	objectViewer;

// ----- [ VIEWING MODES ] ----- //
enum
{
	OBJVIEW_GEOM,
	OBJVIEW_ANIM,
};

enum
{
	SELECTOBJECTBANK_MODE,
	VIEWOBJECTBANK_MODE,
};

extern char showObjectInfo;

// ----- [ FUNCTION PROTOTYPES ] ---------- //

extern void ObjViewInit();

extern void RunObjectViewer();
extern void SelectObjectBank();
extern void ViewObjectBank();

extern void ObjViewResetObjectView();
extern void ObjViewChangeViewMode();

extern void ObjViewGotoNextObject();
extern void ObjViewGotoPreviousObject();
extern void ObjViewGotoFirstObject();
extern void ObjViewGotoLastObject();

extern void ObjViewGotoNextAnimation();
extern void ObjViewGotoPreviousAnimation();
extern void ObjViewGotoFirstAnimation();
extern void ObjViewGotoLastAnimation();
extern void ObjViewChangeAnimSpeed(float speedChange);

extern void ObjViewReadControllerGeom();
extern void ObjViewReadControllerAnim();


#endif	// OBJVIEW_H_INCLUDED //