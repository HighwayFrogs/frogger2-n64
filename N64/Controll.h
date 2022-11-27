/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: Controll.h
	Programmer	: Andrew Eder
	Date		: 12/1/98

----------------------------------------------------------------------------------------------- */

#ifndef CONTROLL_H_INCLUDED
#define CONTROLL_H_INCLUDED


extern OSMesgQueue	controllerMsgQ;
extern OSMesg		controllerMsgBuf[];
extern OSMesgQueue     serialMsgQ;
extern OSMesg          serialMsg;

extern OSPfs	pfsRumble[MAXCONTROLLERS];

extern int initControllers(void);
extern OSContStatus     statusdata[MAXCONTROLLERS];
extern OSContPad        controllerdata[MAXCONTROLLERS];

extern int ControllerMode;
extern char debugCtrlMode[20];
extern char	controllerPresent;
extern char rumblePresent[4];

extern int initControllers(void);
extern void ReadController1(void);
extern void ReadController2(void);


#endif
