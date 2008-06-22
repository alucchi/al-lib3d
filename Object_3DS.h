/**
* File : Object_3DS.cpp
* Description : Class to manage 3DS objects
* Author(s) : ALucchi
* Date of creation : 28/10/2007
* Modification(s) :
*/

#ifndef OBJECT_3DS_H
#define OBJECT_3DS_H

//-------------------------------------------------------------------- INCLUDES
#include "defs.h"

//---------------------------------------------------------------------- CONSTS
// 3DS chunk identifiers

#define RGB_INT						0x0011
#define RGB_INT_GAMMA				0x0013
#define RGB_FLOAT					0x0010
#define RGB_FLOAT_GAMMA				0x0012

#define PERCENT_INT					0x0030
#define PERCENT_FLOAT				0x0031

#define MAIN						0x4D4D
#define OBJ_EDITOR					0x3D3D

#define OBJECT						0x4000
#define MESH						0x4100
#define VERTICES					0x4110
#define FACES						0x4120
#define FACE_MATLIST				0x4130

#define MATERIAL					0xAFFF
#define MAT_NAME					0xA000
#define MAT_AMBIENT					0xA010
#define MAT_DIFFUSE					0xA020
#define MAT_SPECULAR				0xA030
#define MAT_SHININESS				0xA040

//----------------------------------------------------------------------- TYPES

typedef struct {
	char	name[64];				// name (used during loading only)

	RGBA_F	ambient,				// ambient reflectivity
			diffuse,				// diffuse reflectivity
			specular;				// specular reflectivty

	float	sp;						// specular power (shininess)
} Material;

//--------------------------------------------------------------------- CLASSES

class Object_3DS
{
private:
	// 3DS parsing
	FILE	*fp;				// 3DS file

	typedef struct {
		word	id;				// chunk identifier
		dword	len,			// chunk length
				read;			// bytes already read
	} _3DS_Chunk;

	_3DS_Chunk	chunks[16];		// chunk stack (max depth 16)
	int			currChunk;		// top of chunk stack

	// internal stuff to handle face materials

	typedef struct {
		char	matname[64];
		word	numFaces,
				*faces;
	} MatList;

	int			numLists;		// number of material lists
	MatList		*lists;			// material lists
	int			numMats;				// number of materials
	Material		*mats;					// materials

	int				numVerts;				// number of vertices
	int				numFaces;				// number of faces
	Vertex			*verts;					// vertices
	Face				*faces;					// faces
	int				*visible;				// used internally. tells which polygons are visible (in sorted order)


	void calcNormals(void);

	void free(void);
	void freeLists(void);

	void processChunk(void);

	void readMesh(void);
	void readVertexData(void);
	void readFaceData(void);
	
	void readMaterial(void);

	int readString(char *buf);
	void readColor(RGBA_F *col);
	void readPercent(float *percent);

	void readChunk(void);
	void skipChunk(void);

public:
	Object_3DS();

	bool load3DS(const char *filename);
};

#endif //OBJECT_3DS_H