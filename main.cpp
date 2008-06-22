/**
* File : main.cpp
* Description : Main class
*
* Author(s) : ALucchi
* Date of creation : 27/12/2007
* Modification(s) : 20/04/2008
*/

//-------------------------------------------------------------------- INCLUDES
#include <stdio.h>
#include "Application.h"

//------------------------------------------------------------------------ MAIN

int main(int argc, char *argv[])
{
	printf("Lib3dGp2x v3.1\n");
	return Application::Instance().Start();
}