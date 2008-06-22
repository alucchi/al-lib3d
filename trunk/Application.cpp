/**
* File : Application.cpp
* Description : Class to manage the application
* Author(s) : ALucchi
* Date of creation : 20/04/2008
* Modification(s) :
*/

//--------------------------------------------------------------------- INCLUDE
#include "Application.h"

//----------------------------------------------------------------------- TYPES

//--------------------------------------------------------------------- CLASSES

Application* Application::pInstance = 0;// initialize pointer

Application& Application::Instance() 
{
	if (pInstance == 0)  // is it the first call?
	{  
		pInstance = new Application(); // create sole instance
	}
	return *pInstance; // address of sole instance
}

Application::Application()
{
	xRotation=false;
	yRotation=false;
	zRotation=false;
	pause=false;
	showFPS=true;
	RenderingMode=TEXTURED; // default mode

	init();
}

Application::~Application()
{
	deinit();
}

void Application::init()
{
	display = new Display();

	// Init display
	if(!display->Init(SCR_BPP))
	{
		display->Deinit();
		printf("Initialization failed\n");
	}

    // Initialize the stuff we need
    mathInit();

	printf("Initialization OK\n");
}

void Application::deinit()
{
	// Shutdown display
	if(display)
	{
		display->Deinit();
		delete display;
	}

	// Deinit math module
	mathDeinit();
}

bool Application::msgLoop()
{
#ifdef GP2X_MODE
    //Exit condition
	unsigned long  pad=gp2x_joystick_read();
	if ((pad & GP2X_L) && (pad & GP2X_R))
	{
		printf("Press L+R to exit\n");
        deinit();  //deinit Mathematical stuff and GP2X
		return false;
	}
	return true;
	
	if ((pad & GP2X_A) || (pad & GP2X_B))
	{
		printf("pause\n");
        pause= !pause;       
    }
#else
	SDL_Event event;
	
	// Parse the SDL events and evetually take some actions

	while(SDL_PollEvent(&event)) {
		switch(event.type)
		{
		case SDL_KEYDOWN:
			switch(event.key.keysym.sym)
			{
				case SDLK_ESCAPE:
					return false;
					break;
				case SDLK_SPACE:
					pause=!pause;
					ticks = GetTimer(); // reset time
					break;
				case SDLK_t:
					RenderingMode ^= TEXTURED;
					break;
				case SDLK_w:
					RenderingMode ^= WIREFRAME;
					break;
				case SDLK_x:
					xRotation=!xRotation;
					break;
				case SDLK_y:
					yRotation=!yRotation;
					break;
				case SDLK_z:
					zRotation=!zRotation;
					break;
				case SDLK_MINUS:
				case SDLK_m:
					vecTrans.z-=STEP_ZTRANS;
					break;
				case SDLK_PLUS:
				case SDLK_p:
					vecTrans.z+=STEP_ZTRANS;
					break;
			}
			break;			
		case SDL_QUIT:
			return false;
			break;
		}
	}
#endif // GP2X_MODE

	return true;
}

//TODO : a Timer class should be implemented
dword Application::GetTimer()
{
#ifdef USE_SDL
	return SDL_GetTicks();
#else
	return gp2x_timer_read();
#endif
}

Vector3& Application::GetRotation()
{
	return vecRot;
}

int Application::Start()
{
	unsigned short color;

    /*Should we initialize the palette??*/
/*
	L3DC_Color surfPal[256];

	for (int i=0; i<192;i++)
	{
		surfPal[i].r = 0;
		surfPal[i].g = 0;
		surfPal[i].b = (i*63/192);
	}
		
	for (int i=192;i<256;i++)
	{
		surfPal[i].r = i-192;
		surfPal[i].g = i-192;
		surfPal[i].b = 63;
	}

//grey
	for(int i = 0; i < 256; i++)
	{
		surfPal[i].r = i;
		surfPal[i].g = i;
		surfPal[i].b = i;
	}
	
	display->setPalette(surfPal);
*/

	renderer=new Renderer(display);

	obj=new Object;
	if(obj->GetMesh(XML_FILE)<0)
	{
		printf("Error while loading %s\n", XML_FILE);
		return -1;
	}
	else
		printf("Loading OK : %d faces was loaded\n",obj->numFaces);	
	
	obj->body.pos.y=2.0f;
	obj->body.v.y=-0.1f;
	obj->body.a.y=-0.1f;

	Object* obj2=new Object;
	if(obj2->GetMesh(XML_FILE2)<0)
	{
		printf("Error while loading %s\n", XML_FILE2);
		return -1;
	}
	else
		printf("Loading OK : %d faces was loaded\n",obj2->numFaces);	

	// Initialize time/fps counters
	timePassed = 0; //minimal version!!
 	frames = 0;
	ticks = 0;
	tacks = 0;

	vecTrans=Vector3(0, 0, 3);

	/*
	if(renderer->SetTexture(obj->materialName)<0)
	{
		printf("Error while loading texture files\n");
		return -1;
	}
	*/
	TextureManager::Instance().LoadPalette(1);

	// Enter the message loop
	while(msgLoop())
	{
		if(!pause)
		{
			display->Clear();

			// Calculate passed time in seconds for proper rotation (at fixed speed)
	    	dword currTicks = GetTimer();
         float secs = (currTicks -ticks)*0.001;
			ticks = currTicks;         
            
            if(showFPS)
            {
                frames++;
                if ( (GetTimer() - tacks) > 1000 )
                {
                  maxfps=frames;
                  frames=0;
                  tacks = GetTimer();
                }
#ifdef GP2X_MODE
                gp2x_printf(NULL, 0, 0,"%i fps",maxfps);
#else
					printf("%d fps\n",maxfps);
#endif //GP2X_MODE
            }
            
			// Update rotations
			if(xRotation)
				vecRot.x += 60.0 * secs;
			if(yRotation)
				vecRot.y += 45.0 * secs;
			if(zRotation)
				vecRot.z += 40.0 * secs;

			renderer->Identity();
			renderer->Translate(vecTrans);
			renderer->Rotate(vecRot);

			// render the objects
			renderer->RenderObject(obj);
			renderer->RenderObject(obj2);

			//obj->body.Update();

			display->Flip();	
		}
	}
	return 0;
}