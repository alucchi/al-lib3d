/**
* File : math3D.h
* Description : 3D Math library
*
* Author(s) : ALucchi
* Date of creation : 18/12/2005
* Modification(s) : ALucchi => 11/06/2007 : Refactoring
*/

#ifndef MATH3D_H
#define MATH3D_H

//-------------------------------------------------------------------- INCLUDES

#include <math.h>		// Standard C Math
#include <stdio.h>		// DEBUG ONLY!!!
#include "Vector2.h"
#include "Vector3.h"
#include "Matrix4.h"

//---------------------------------------------------------------------- CONSTS
#define FLT_ERROR	0.0001

#define RAD			0.017453292519943
#define DEG			57.2957795130824

//---------------------------------------------------------------------- MACROS
#define SIN(x)		(mathLuts->sin[x])
#define COS(x)		(mathLuts->cos[x])

//----------------------------------------------------------------------- TYPES

typedef struct {
	float	sin[360],
			cos[360];
} MathLuts;

//--------------------------------------------------------------------- CLASSES

//------------------------------------------------------------------- FUNCTIONS
void mathInit(void);
void mathDeinit(void);

//------------------------------------------------------------------- VARIABLES
extern MathLuts *mathLuts;

#endif // MATH3D_H
