/**
* File : TextureManager.h
* Description : Class to manage textures
*
* Author(s) : ALucchi
* Date of creation : 25/05/2007
*/

//-------------------------------------------------------------------- INCLUDES

#include "TextureManager.h"
#include "Application.h"
#include "converter.h"
#include <stdio.h>

//--------------------------------------------------------------------- GLOBALS
TextureManager *TextureManager::m_instance = 0; // instance

//--------------------------------------------------------------------- CLASSES

/**
* This constructor should not be called directly. The Instance method is the
* only one that should call the constructor.
**/
TextureManager::TextureManager()
{
	m_NumTextures = 0;
}

TextureManager::~TextureManager()
{
}

TextureManager &TextureManager::Instance()
{
	if (!m_instance) {
		m_instance = new TextureManager;
	}

	return *m_instance;
}

void TextureManager::Destroy ()
{
	if (m_instance) {
		delete m_instance;
		m_instance = 0;
	}
}

int TextureManager::AddTexture (const char *szFilename)
{
	int status=-1;
	// Determine the type and actually load the file
	int nLen = strlen (szFilename);
	char* szCapFilename =new char[nLen+1];	

	for (int c = 0; c <= nLen; c++)	// <= to include the NULL as well
		szCapFilename [c] = toupper (szFilename [c]);
	
	// Loading the file
	if (strcmp (szCapFilename + (nLen - 3), "BMP") == 0) {
		status=LoadBitmapFile(szFilename);
	}
	/*
	else if (strcmp (szCapFilename + (nLen - 3), "TGA") == 0) {
		status=LoadTargaFile (szFilename);

	}
	*/
	else {
		printf ("ERROR : Unable to load extension [%s]", szCapFilename + (nLen - 3));
		delete[] szCapFilename;
		return -1;
	}

	delete[] szCapFilename;
	return status;
}

int TextureManager::LoadBitmapFile (const char *fileName)
{
	FILE *f;
	unsigned int sizeBmp;
	unsigned char* ptrTex;
	unsigned char* bmp;	
	unsigned char* sBmp;	// save the pointer to delete it
							// at the end of this function !

	f=fopen(fileName,"rb");
	if (!f)
		return ERR_LOADING_BMP;

	unsigned char header[SIZE_BMP_HEADER];
	fread(header,1,SIZE_BMP_HEADER,f);

	int i,x,y;
	unsigned char r,g,b,c;
	unsigned long height,width;
	unsigned short bpp;

	/* Reading HEADER */
	width=getlong(header+18);
	height=getlong(header+22);
	bpp=getshort(header+28);
	sizeBmp=width*height*bpp/sizeof(char);
	bmp=new unsigned char[sizeBmp];
	sBmp=bmp;

	int padding=(height*8)%32;
	int wp; // width+padding
	if(padding!=0)
	{
		padding=(32-padding)/8;
		wp=width+padding;
	}
	else
		wp=width;

	Texture* currentTexture=GetNewTexture();
	if(!currentTexture)
		return ERR_LOADING_BMP;

	//strcpy(currentTexture->name,"default");;
	currentTexture->data=new unsigned char[sizeBmp];
	ptrTex=currentTexture->data;
	currentTexture->width=width;
	currentTexture->height=height;
	currentTexture->size=width*height;
	currentTexture->bpp=bpp;
	
	// Set Color table
	/*
	fread(currentTexture->colorTable,1,SIZE_BMP_PALETTE_8BITS,f);
	L3DC_Color l3dc_col[256];
	byte* ptrCol=currentTexture->colorTable;
	for(int i=0;i<256;i++)
	{
		l3dc_col[i].b=*ptrCol++;
		l3dc_col[i].g=*ptrCol++;
		l3dc_col[i].r=*ptrCol++;
		ptrCol++;
	}
	*/
	L3DC_Color l3dc_col[256];
	fread(l3dc_col,1,SIZE_BMP_PALETTE_8BITS,f);	
	byte* ptrCol=currentTexture->colorTable;
	for(int i=0;i<256;i++)
	{
		*ptrCol++ = l3dc_col[i].b;
		*ptrCol++ = l3dc_col[i].g;
		*ptrCol++ = l3dc_col[i].r;
		ptrCol++;
	}

	fread(bmp,1,sizeBmp,f);

#define FLIP_Y_COPY
#ifdef FLIP_Y_COPY
	unsigned int increment=2*wp-padding;
	bmp+=(height-1)*wp; // we start at the last line
	for(y=0;y<height;y++)
	{		
		for(x=0;x<width;x++)
		{
			*ptrTex++=*bmp++;
		}		
		bmp-=increment;
	}
#else
	for(y=0;y<height;y++)
	{		
		for(x=0;x<width;x++)
		{
			*ptrTex++=*bmp++;
		}		
		bmp+=padding;
	}
#endif

	delete[] sBmp;
	fclose(f);
	return currentTexture->id;
}

void TextureManager::FreeTexture (int nID)
{
}

void TextureManager::FreeAll ()
{
}

Texture* TextureManager::GetNewTexture()
{
	Texture* texture = new Texture();
	texture->id = m_NumTextures++;
	m_pTextures.push_back(texture);
	return texture;
}

int TextureManager::GetNumTextures()
{
	return m_NumTextures;
}

int TextureManager::LoadPalette(int textureID)
{
	Texture* texture = m_pTextures[textureID];
	if(texture)
		return Application::Instance().GetDisplay()->SetPalette((const L3DC_Color*)texture->colorTable);
	else
		return ERR_LOADING_PALETTE;
}

int TextureManager::LoadTexture(int textureID)
{
	//Display* display = Application::Instance().GetDisplay();
	//display->SetPalette((const L3DC_Color*)texture->colorTable);
	Texture* texture = m_pTextures[textureID];
	if(texture)
		Application::Instance().GetRenderer()->SetCurrentTexture(texture);
	else
		return ERR_LOADING_TEXTURE;
}

/*
UBYTE *TextureManager::LoadBitmapFile (const char *filename, int &nWidth, int &nHeight, int &nBPP) {
	

	// These are both defined in Windows.h
	BITMAPFILEHEADER	BitmapFileHeader;
	BITMAPINFOHEADER	BitmapInfoHeader;
	
	// Old Skool C-style code	
	FILE	*pFile;
	UBYTE	*pImage;			// bitmap image data
	UBYTE	tempRGB;				// swap variable

	// open filename in "read binary" mode
	pFile = fopen(filename, "rb");
	if (pFile == 0) {
		
		sprintf (m_instance->szErrorMessage, "ERROR : [%s] File Not Found!", filename);	
		return 0;
	}

	// Header
	fread (&BitmapFileHeader, sizeof (BITMAPFILEHEADER), 1, pFile);
	if (BitmapFileHeader.bfType != 'MB') {
		
		sprintf (m_instance->szErrorMessage, "ERROR : [%s] Is not a valid Bitmap!", filename);
		fclose (pFile);
		return 0;
	}

	// Information
	fread (&BitmapInfoHeader, sizeof (BITMAPINFOHEADER), 1, pFile);

	if (!CheckSize (BitmapInfoHeader.biWidth) || !CheckSize (BitmapInfoHeader.biHeight)) {

		sprintf (m_instance->szErrorMessage, "ERROR : Improper Dimension");
		fclose (pFile);
		return 0;
	}

	
	fseek (pFile, BitmapFileHeader.bfOffBits, SEEK_SET);
	pImage = new UBYTE [BitmapInfoHeader.biSizeImage];
	if (!pImage) {
		delete [] pImage;
		
		sprintf (m_instance->szErrorMessage, "ERROR : Out Of Memory!");

		fclose (pFile);
		return 0;
	}
	fread (pImage, 1, BitmapInfoHeader.biSizeImage, pFile);

	// Turn BGR to RBG
	for (int i = 0; i < (int) BitmapInfoHeader.biSizeImage; i += 3) {
		tempRGB = pImage [i];
		pImage [i + 0] = pImage [i + 2];
		pImage [i + 2] = tempRGB;
	}

	fclose(pFile);

	// THIS IS CRUCIAL!  The only way to relate the size information to the
	// OpenGL functions back in ::LoadTexture ()
	nWidth  = BitmapInfoHeader.biWidth;
	nHeight = BitmapInfoHeader.biHeight;
	nBPP    = 3;	// Only load 24-bit Bitmaps

	return pImage;
}

UBYTE *TextureManager::LoadTargaFile (const char *filename, int &nWidth, int &nHeight, int &nBPP) {

	// Get those annoying data structures out of the way...
	struct {
		unsigned char imageTypeCode;
		short int imageWidth;
		short int imageHeight;
		unsigned char bitCount;
	} TgaHeader;

	// Let 'er rip!
	FILE	*pFile;
	UBYTE	uCharDummy;
	short	sIntDummy;
	UBYTE	colorSwap;	// swap variable
	UBYTE	*pImage;	// the TGA data

	// open the TGA file
	pFile = fopen (filename, "rb");
	if (!pFile) {
	
		sprintf (m_instance->szErrorMessage, "ERROR : [%s] File Not Found!", filename);
		return 0;
	}

	// Ignore the first two bytes
	fread (&uCharDummy, sizeof (UBYTE), 1, pFile);
	fread (&uCharDummy, sizeof (UBYTE), 1, pFile);

	// Pop in the header
	fread(&TgaHeader.imageTypeCode, sizeof (unsigned char), 1, pFile);

	// Only loading RGB and RGBA types
	if ((TgaHeader.imageTypeCode != 2) && (TgaHeader.imageTypeCode != 3)) {

		sprintf (m_instance->szErrorMessage, "ERROR : Unsuported Image Type (Color Depth or Compression)");
		fclose (pFile);
		return 0;
	}

	// More data which isn't important for now
	fread (&uCharDummy, sizeof (unsigned char), 1, pFile);
	fread (&sIntDummy,  sizeof (short), 1, pFile);
	fread (&sIntDummy,  sizeof (short), 1, pFile);
	fread (&sIntDummy,  sizeof (short), 1, pFile);
	fread (&sIntDummy,  sizeof (short), 1, pFile);

	// Get some rather important data
	fread (&TgaHeader.imageWidth,  sizeof (short int), 1, pFile);
	fread (&TgaHeader.imageHeight, sizeof (short int), 1, pFile);
	fread (&TgaHeader.bitCount, sizeof (unsigned char), 1, pFile);

	// Skip past some more
	fread (&uCharDummy, sizeof (unsigned char), 1, pFile);

	// THIS IS CRUCIAL
	nBPP    = TgaHeader.bitCount / 8;
	nWidth  = TgaHeader.imageWidth;
	nHeight = TgaHeader.imageHeight;
	

	if (!CheckSize (nWidth) || !CheckSize (nHeight)) {

		sprintf (m_instance->szErrorMessage, "ERROR : Improper Dimension");
		fclose (pFile);
		return 0;
	}


	int nImageSize = nWidth * nHeight * nBPP;
	pImage = new UBYTE [nImageSize];
	if (pImage == 0) {
		
		sprintf (m_instance->szErrorMessage, "ERROR : Out Of Memory");
		return 0;
	}

	// actually read it (finally!)
	fread (pImage, sizeof (UBYTE), nImageSize, pFile);

	// BGR to RGB
	for (int i = 0; i < nImageSize; i += nBPP) {
		colorSwap = pImage [i + 0];
		pImage [i + 0] = pImage [i + 2];
		pImage [i + 2] = colorSwap;
	}
	fclose (pFile);

	return pImage;
}
*/