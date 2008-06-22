/**
* File : math3D.cpp
* Description : 3D Math library
*
* Author(s) : ALucchi
* Date of creation : 18/12/2005
* Modification(s) : ALucchi => 11/06/2007 : Refactoring
*/

//-------------------------------------------------------------------- INCLUDES
#include "math3D.h"

//------------------------------------------------------------------- VARIABLES
MathLuts *mathLuts = NULL;

//------------------------------------------------------------------- FUNCTIONS

void mathInit(void)
{
	double val = 0;
	mathLuts = new MathLuts;
	for(int i = 0; i < 360; i++, val += RAD)
	{
		mathLuts->sin[i] = (float)sin(val);
		mathLuts->cos[i] = (float)cos(val);
	}
}

void mathDeinit(void)
{
	if(mathLuts) {
		delete mathLuts;
		mathLuts = NULL;
	}
}