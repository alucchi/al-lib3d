/**
* File : Display.h
* Description : Object to manage the display area on the screen and a few other
*  things associated with display.
* Author(s) : ALucchi
* Date of creation : 28/10/2007
* Modification(s) :
*/

#ifndef DISP_H
#define DISP_H

//--------------------------------------------------------------------- INCLUDE

//----------------------------------------------------------------------- TYPES

//--------------------------------------------------------------------- CLASSES

class Display
{
private:

	Screen		screen;				// Screen's surface

#ifdef USE_SDL
	SDL_Surface* surface; // we must keep the surface to use SDL functions
#endif

public:

	Display();
	~Display();
	
	bool	Init(dword bpp);
	void	Deinit();

	// Palette
	int	SetPalette(const L3DC_Color* pal);
	void	GetPalette(byte *pal);
	
	// Clear screen
	void	Clear();	

	// Flip buffers and wait for synchronisation with screen
	bool	Flip();	

	// Load a BMP image
	int		LoadBmp(char* name);

	// Get a pointer on the screen
	Screen	GetScreen();	

};

#endif // DISP_H
