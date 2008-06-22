/**
* File : Application.h
* Description : Class to manage the application
* Author(s) : ALucchi
* Date of creation : 20/04/2008
* Modification(s) :
*/

#ifndef APPLICATION_H
#define APPLICATION_H

//--------------------------------------------------------------------- INCLUDE
#include "defs.h"
#include "Display.h"
#include "Object.h"
#include "Renderer.h"
#include "Ase.h"

#include <string.h>
#include <time.h>

#ifdef USE_SDL
#include <SDL/SDL.h>
#endif

#ifdef GP2X_MODE
#include "minimal.h"
#include "math-sll.h"
#endif //GP2X_MODE

//----------------------------------------------------------------------- TYPES

//--------------------------------------------------------------------- GLOBALS

#ifdef GP2X_MODE
/*Necessary to compile we don't use it, yet ;-)*/
void gp2x_sound_frame(void *blah, void *bufferg, int samples){}
#endif //GP2X_MODE

//--------------------------------------------------------------------- CLASSES

class Application
{
private:
	static Application* pInstance;

	Display*		display;		// Application's display object
	Object*		obj;			// Application's display object
	Renderer*	renderer;	// our 3D renderer

	Vector3 vecRot;			// rotation vector applied to the scene
	Vector3 vecTrans;			// translation vector applied to the scene

	// Variables used for FPS measurement
	dword	    ticks,tacks;		// Passed ticks (used for timed rotation)
	dword     timePassed;		// Ticks passed since start of program
	int	    frames,maxfps;	// Number of frames rendered

	bool pause;
	bool showFPS;
	bool xRotation;
	bool yRotation;
	bool zRotation;

	Application();
	~Application();
	void init();
	void deinit();
	bool msgLoop();	

public:
	enum MODE {
		WIREFRAME=1,
		TEXTURED=2,
		SHADED=3};
	byte RenderingMode;

	Display* GetDisplay() { return display;}
	Renderer* GetRenderer() { return renderer;}
	Vector3& GetRotation();
	dword GetTimer();
	int Start();	
	
	static Application& Instance();
};

#endif // APPLICATION_H
