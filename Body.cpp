/**
* File : Body.cpp
* Description : Class used to store physical parameters relative to an object.
* Author(s) : ALucchi
* Date of creation : 28/10/2007
* Modification(s) :
*/

//-------------------------------------------------------------------- INCLUDES
#include "Body.h"
#include "Application.h"

//---------------------------------------------------------------------- CONSTS

//--------------------------------------------------------------------- CLASSES

Body::Body(void) {
	mass	= 0;
	lastUpdateTime = Application::Instance().GetTimer();
}

Body::~Body(void)
{
}

// Update
void Body::Update()
{
	// Elapsed time since the last update
	uint currentTime=Application::Instance().GetTimer();
	float t = currentTime - lastUpdateTime;
	t*=0.001f;
	lastUpdateTime=currentTime;

	pos+=(v*t)+a*t*t*0.5;

	v+=a*t;

	if(pos.y <= 0)
	{
		v=-v;
	}
}