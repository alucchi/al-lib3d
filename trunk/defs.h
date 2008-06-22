/**
* File : defs.h
* Description : Constants, types,...
* Author(s) : ALucchi
* Date of creation : 12/08/2005
* Modification(s) :
*/

#ifndef DEFS_H
#define DEFS_H

//-------------------------------------------------------------------- INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>

#include "Maths/math3D.h"

#ifdef USE_SDL
	#include <SDL/SDL.h>
#endif

//---------------------------------------------------------------------- CONSTS

//#define BACK_FACE_CULLING
//#define RENDERER_WIRE
#define DEBUG

#ifdef WIN32
#define SEPARATOR "\\"
#else
#define SEPARATOR "/"
#endif

#define TITLE			"Gp2X Engine"
#define XML_DIRECTORY "models"
#define XML_FILE		XML_DIRECTORY SEPARATOR "Sphere.mesh.xml"
#define XML_FILE2		XML_DIRECTORY SEPARATOR "Plane.mesh.xml"
#define SCREEN_COLOR	0xFFFFFF

#define STEP_ZTRANS 0.2f

/* Screen sizes */
// we consider that the height screen
#ifdef GP2X_MODE
	// it doesn't work with another resolution on the gp2x
	#define SCR_WIDTH 320
	#define SCR_HEIGHT 240
	#define SCR_BPP	16
#else
	#define SCR_WIDTH 640
	#define SCR_HEIGHT 480
	#define SCR_BPP	8 // under review (we should use 16 bits under Windows)
#endif

/* Focal length :
The focal length determines the field (or angle) of view, and also how much
objects will be magnified.
The field of view decreases with increasing focal length. The relationship
is through the arc tangent function (the formula is FOV = 2 arctan (x / (2 f))
NB : To be displayed on the screen, a pixel must have coordinates included in :
	x : between -(SCR_WIDTH/2)/inv and +(SCR_WIDTH/2)/inv
	y : between -(SCR_HEIGHT/2)/inv and +(SCR_HEIGHT/2)/inv
	where inv = FOCAL / v->coordsWorld.z
*/
#define FOCAL		150

// arbitary floating point error value
//#define FLT_ERROR		1e-4
#define PI				3.14159265359
#define DEGS_PER_RAD	0.01745329252
#define RADS_PER_DEG	57.29577951307

#define SIZE_BMP_HEADER			54
#define SIZE_BMP_PALETTE_8BITS	1024 // 256*4

// Errors
#define ERR_PARSING_MESH		-10

#define ERR_TINY_READING_FILE	-20

#define ERR_LOADING_BMP			-30
#define ERR_LOADING_PALETTE	-31
#define ERR_LOADING_TEXTURE	-32

// enums
enum TRIANGLE_TYPE {FLAT_BOTTOM, FLAT_TOP, GENERAL};

//---------------------------------------------------------------------- MACROS

#define degstorads(x)	((x) * (DEGS_PER_RAD))
#define radstodegs(x)	((x) * (RADS_PER_DEG))

#define tan(x)			(sin(x) / cos(x))
#define cot(x)			(cos(x) / sin(x))

#define INVERSE(x)		((1.0f) / (x))

#define SQUARE(x)		((x) * (x))
#define CUBE(x)			((x) * (x) * (x))

#define SWAP(t,x,y) {t=x;x=y;y=t;}
#define SWAPINT(x, y) (x ^= y ^= x ^= y);


//----------------------------------------------------------------------- TYPES

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long dword;
typedef unsigned int uint;

// Screen's surface
#if SCR_BPP == 8
	typedef byte * Screen;	// 8 bits
#elif SCR_BPP == 16
	typedef word * Screen;	// 16 bits
#elif SCR_BPP == 32
	typedef dword * Screen;	// 32 bits
#endif

// Color
typedef struct L3DC_Color {
	byte r;
	byte g;
	byte b;
	byte unused;
} L3DC_Color;

// --- Geometrical structures

typedef struct {
	union {
		struct {
			float r, g, b, a;
		};
		float rgba[4];	
	};
} RGBA_F;

typedef struct {
	union {
		struct {
			int r, g, b, a;
		};
		int rgba[4];
	};
} RGBA_I;

struct Vertex_TexCoord
{
    float u, v;
};

typedef struct {
	Vector3	coordsLocal,			// Default coordinates
		 	coordsWorld;			// World coordinates
	Vector3 normal;					// normal
	Vertex_TexCoord texCoord;
	float	scr[2];					// Screen coordinates
	RGBA_F	col;				// point's color
} Vertex;

typedef unsigned short TIndex;

// We should keep Triangle or Vertex but not both
typedef struct {
	// Vertex indices
	TIndex	a,b,c;
	// UV indices
	TIndex	UVIndex1,UVIndex2,UVIndex3;
	char	col;					// Color
	float	cenZ;					// Z centroid (used for sorting)
	Vector3	normal;					// face normal
} Triangle;

typedef struct {
	int		verts[3];				// vertex indices
	Vector3	normal;					// face normal
	float	cenZ;					// centroid's z component
	int		mat;					// material, which this face uses (-1 = none)
} Face;

#endif //DEFS_H
