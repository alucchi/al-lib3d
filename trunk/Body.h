/**
* File : Body.h
* Description : Class used to store physical parameters relative to an object.
* Author(s) : ALucchi
* Date of creation : 28/10/2007
* Modification(s) :
*/

#ifndef BODY_H
#define BODY_H

//-------------------------------------------------------------------- INCLUDES
#include "defs.h"

//---------------------------------------------------------------------- MACROS

//----------------------------------------------------------------------- TYPES

//-------------------------------------------------------------------- CLASSES

class Body
{
public:

	// no need to hide these

	uint		mass;
	Vector3	pos;	//position
	Vector3	v;		//velocity
	Vector3	a;		//acceleration

	uint lastUpdateTime;

	Body(void);
	~Body(void);

	void Update();
};

#endif // BODY_H
