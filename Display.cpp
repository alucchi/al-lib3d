/**
* File : Display.cpp
* Description : Object to manage the display area on the screen and a few other
*  things associated with display.
* Author(s) : ALucchi
* Date of creation : 28/10/2007
* Modification(s) :
*/

//-------------------------------------------------------------------- INCLUDES
#ifdef USE_SDL
#include <SDL/SDL.h>
#endif

#ifdef GP2X_MODE
#include "minimal.h"
#endif

#include <stdio.h> // FILE

#include "defs.h"
#include "converter.h"
#include "Display.h"

//--------------------------------------------------------------------- CLASSES

Display::Display()
{
	screen		= NULL;
}

Display::~Display()
{
	Deinit();
}

bool Display::Init(dword bpp)
{
#ifdef GP2X_MODE
	gp2x_init(1000, bpp, 44100,16,1,50,1);
#endif

#ifdef USE_SDL
	SDL_VideoInfo	*info;
	dword			flags = 0;
	bool fullscr = true;		// Should we run in fullscreen mode ?
	bool doubleBuf = true;    // Should we try to create a double buffer ?

	assert(screen == NULL);
	
	// Initialize inner SDL video system ( should be already done since app must call SDL_Init(..) )
	if(!SDL_WasInit(SDL_INIT_VIDEO))
	{
		if(SDL_InitSubSystem(SDL_INIT_VIDEO) < 0)
		{
			printf("SDL video initialization failed : %s\n", SDL_GetError());
			return false;
		}
	}

	// Check manualy for valid bit-depth, because of a weird bug in SDL

	switch(bpp)
	{
		case 8:
		case 15:
		case 16:
		case 24:
		case 32:
			break;
		default:
			printf("Invalid bit depth!");
			Deinit();
			return false;
	}

	// Set display flags
	if(fullscr)
		flags |= SDL_FULLSCREEN;

	info = (SDL_VideoInfo*)SDL_GetVideoInfo();	
	if(info->hw_available)
	{
		flags |= SDL_HWSURFACE;
		if(doubleBuf)
			flags |= SDL_DOUBLEBUF;
	}

	// Check if the required display mode is supported 
	if(fullscr)
	{
		if(SDL_VideoModeOK(SCR_WIDTH, SCR_HEIGHT, bpp, flags) != bpp)
		{
			printf("Unsupported display mode: %dx%d %d bpp", SCR_WIDTH, SCR_HEIGHT, bpp);
			flags &= ~SDL_FULLSCREEN;
		}
	}	

	// Init the display
	flags=SDL_SWSURFACE;
	surface = SDL_SetVideoMode(SCR_WIDTH, SCR_HEIGHT, bpp, flags);	
	if(!surface && info->hw_available)
	{
		// Try again with software surface
		flags &= ~SDL_HWSURFACE;
		if(doubleBuf)
			flags &= ~SDL_DOUBLEBUF;
		surface = SDL_SetVideoMode(SCR_WIDTH, SCR_HEIGHT, bpp, flags);
	}	

	if(!surface) {
		printf("Cannot create display surface!\n");
		Deinit();
		return false;
	}

	// Everything is OK. We can use the surface
	screen=(Screen)surface->pixels;

	if(info->wm_available)
		SDL_WM_SetCaption(TITLE, NULL);

	SDL_ShowCursor(SDL_DISABLE);

#else
	#if SCR_BPP==8
		screen=gp2x_video_RGB[0].screen8;		
	#elif SCR_BPP==16
		screen=gp2x_video_RGB[0].screen16;		
	#elif SCR_BPP==32
		screen=gp2x_video_RGB[0].screen32;		
	#endif
#endif	

	return true;
}

void Display::Deinit()
{
#ifdef USE_SDL
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
#endif
	screen = NULL;
}

void Display::Clear()
// Clear screen
{
#ifdef USE_SDL
	SDL_FillRect(surface, NULL, SCREEN_COLOR);
#else
	for(int n=0;n<SCR_HEIGHT*SCR_WIDTH;n++)
	{
    	screen[n]=SCREEN_COLOR;
	}
#endif
}

int Display::SetPalette(const L3DC_Color* pal)
{
	assert(screen != NULL);
	assert(pal != NULL);
	
#ifdef USE_SDL
	if(surface->format->BitsPerPixel > 8)
	{
		printf("Error : surface->format->BitsPerPixel > 8");
		return ERR_LOADING_BMP;
	}
	
	return SDL_SetPalette(surface, SDL_PHYSPAL | SDL_LOGPAL, (SDL_Color*)pal, 0, 256);
#else		
	for (int i=0;i<256;i++)
	{
		gp2x_video_RGB_color8(i,pal->r,pal->g,pal->b);
		pal++;
	}
	gp2x_video_RGB_setpalette();
#endif

	return 0;
}

void Display::GetPalette(byte *pal)
{
	assert(screen != NULL);
	assert(pal != NULL);

#ifdef USE_SDL	
	if(surface->format->BitsPerPixel > 8)
		return;

	SDL_Palette *surfPal = surface->format->palette;
	byte *dst = pal;

	memset(dst, 0, 768);
	
	for(int i = 0; i < surfPal->ncolors; i++)
	{
		*dst++ = surfPal->colors[i].r;
		*dst++ = surfPal->colors[i].g;
		*dst++ = surfPal->colors[i].b;
	}
#endif
}

bool Display::Flip()
// Flip buffers and wait for synchronisation with screen
{
	//assert(screen != NULL); // it's not really good for the FPS
#ifdef USE_SDL
	return (SDL_Flip(surface) == 0);
#else
	/* minimal.h modified to call vsync & hsync, see if newer
	   version implements this in flip procedure
	*/

	//gp2x_video_waitvsync();
	gp2x_video_RGB_flip(0);
	#if SCR_BPP==8
		screen=gp2x_video_RGB[0].screen8;
	#elif SCR_BPP==16
		screen=gp2x_video_RGB[0].screen16;
	#elif SCR_BPP==32
		screen=gp2x_video_RGB[0].screen32;
	#endif
	return true;
#endif
}

Screen Display::GetScreen()
{
	return screen;
}

int Display::LoadBmp(char* name)
{
	FILE *f;
	unsigned int sizeBmp;
	unsigned char* bmp;	
	unsigned char* sBmp;	// save the pointer to delete it
							// at the end of this function !

	f=fopen(name,"rb");
	if (f)
	{		
		unsigned char temp[6];
		fread(temp,1,6,f);
		sizeBmp=getlong(temp+2);
	}
	else
		return ERR_LOADING_BMP;

	bmp=new unsigned char[sizeBmp];
	sBmp=bmp;
	fread(bmp,1,sizeBmp-6,f);
	fclose(f);

	int i,x,y;
	unsigned char r,g,b,c;
	unsigned int height,width;

	/* Reading HEADER */
	bmp+=12;
	// reading image width and image height (in pixels)
	width=getlong(bmp);
	bmp+=4;
	height=getlong(bmp);

	int padding=(height*8)%32;
	int wp; // width+padding
	if(padding!=0)
	{
		padding=(32-padding)/8;
		wp=width+padding;
	}
	else
		wp=width;

	bmp+=32; /* Skip INFOHD */
	
	byte palette[SIZE_BMP_PALETTE_8BITS];

	L3DC_Color l3dc_pal[256];
	for(int i=0;i<256;i++)
	{
		l3dc_pal[i].b=*bmp++;
		l3dc_pal[i].g=*bmp++;
		l3dc_pal[i].r=*bmp++;
		bmp++;
	}
	SetPalette(l3dc_pal);

	/* Set Bitmap */
	Screen out=screen;
	unsigned int pitch=SCR_WIDTH-width;
	unsigned int increment=2*wp-padding;

	bmp+=(height-1)*wp; // we start at the last line
	for (y=height-1;y!=-1;y--)
	{		
		for (x=0;x<width;x++)
		{
			*out++=*bmp++;
		}		
		bmp-=increment;
		out+=pitch;
	}

	delete sBmp;
	return 0;
}