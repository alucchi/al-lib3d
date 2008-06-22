/**
* File : TextureManager.h
* Description : Class to manage textures
*
* Author(s) : ALucchi
* Date of creation : 25/05/2007
*/

#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

//-------------------------------------------------------------------- INCLUDES
#include "defs.h"
#include <vector>

using namespace std;

//----------------------------------------------------------------------- TYPES

typedef struct {
	//char		name[32];	//texture name
	int		id;
	int		width;		//dimensions
	int		height;
	int		bpp;
	int		size;			//width * height
	byte		colorTable[SIZE_BMP_PALETTE_8BITS];
	byte		*data;		//data
} Texture;

//--------------------------------------------------------------------- CLASSES

class TextureManager
{
public :
	TextureManager();
	~TextureManager();
	static TextureManager &Instance();
	static void Destroy();

	int AddTexture(const char *szFilename);
	int LoadPalette(int textureID);
	int LoadTexture(int textureID);

	void FreeTexture (int nID);
	void FreeAll();
	
	int GetNumTextures();

private :
	static TextureManager *m_instance;

	int LoadBitmapFile (const char *fileName);

	/*
	UBYTE *LoadBitmapFile (const char *filename, int &nWidth, int &nHeight, int &nBPP);
	UBYTE *LoadTargaFile (const char *filename, int &nWidth, int &nHeight, int &nBPP);
	*/

	Texture* GetNewTexture();

private :
	int m_NumTextures;
	vector<Texture*> m_pTextures;
};

#endif //TEXTURE_MANAGER_H
