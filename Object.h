/**
* File : Object.h
* Description : Class used to store all the information relative to an object :
* coordinates, material, physical parameters,...
* Author(s) : ALucchi
* Date of creation : 28/10/2007
* Modification(s) :
*/

#ifndef OBJECT_ASE_H
#define OBJECT_ASE_H

//-------------------------------------------------------------------- INCLUDES
#include "defs.h"
#include "Body.h"
#include "tinyxml/tinyxml.h" // xml parsing

//---------------------------------------------------------------------- MACROS

#define CLAMP(x, min, max)	((x) < (min) ? (min) : (x) > (max) ? (max) : (x))

//#define CLAMP_TO_UNIT(x)	(CLAMP(x, 0, 1))
//#define CLAMP_TO_BYTE(x)	(CLAMP(x, 0, 255))
//
//#define CLAMP_RGBA_F(dst, src) {		\
//	dst.r = CLAMP_TO_UNIT(src.r);		\
//	dst.g = CLAMP_TO_UNIT(src.g);		\
//	dst.b = CLAMP_TO_UNIT(src.b);		\
//	dst.a = CLAMP_TO_UNIT(src.a);		\
//}
//
//#define CLAMP_RGBA_I(dst, src) {		\
//	dst.r = CLAMP_TO_BYTE(src.r);		\
//	dst.g = CLAMP_TO_BYTE(src.g);		\
//	dst.b = CLAMP_TO_BYTE(src.b);		\
//	dst.a = CLAMP_TO_BYTE(src.a);		\
//}
//
//#define SET_RGBA_F(dst, red, green, blue, alpha) {	\
//	dst.r = CLAMP_TO_UNIT(red);						\
//	dst.g = CLAMP_TO_UNIT(green);					\
//	dst.b = CLAMP_TO_UNIT(blue);					\
//	dst.a = CLAMP_TO_UNIT(alpha);					\
//}
//
//#define SET_RGBA_I(dst, red, green, blue, alpha) {	\
//	dst.r = CLAMP_TO_BYTE(red);						\
//	dst.g = CLAMP_TO_BYTE(green);					\
//	dst.b = CLAMP_TO_BYTE(blue);					\
//	dst.a = CLAMP_TO_BYTE(alpha);					\
//}

//----------------------------------------------------------------------- TYPES

typedef Vector3i Color;

//-------------------------------------------------------------------- CLASSES

class Object
{
public:

	// no need to hide these

	int				numVerts;				// number of vertices
	int				numFaces;				// number of faces
	int				numTexVertex;			// number of vertices texture
	Vertex			*verts;					// vertices
	Triangle			*faces;					// faces
	Vertex_TexCoord	*texVerts;				// vertices texture
	bool					bHasTexture;

	int			numVisible,				// used internally. number of visible polygons
					*visible;				// used internally. tells which polygons are visible (in sorted order)	

	// To keep ASE compatinility
	// TODO : The ASE loader has to be refactored to be better integrated with
	// the texture manager
	int	materialID;
	char* materialName;

	int textureID;

	Body body;

	Object(void);
	~Object(void);
	
	// import the ASE models from the specified xml file
	int GetMesh(const char* filename);
	void free();

	friend class CLoadASE;
};

#endif // OBJECT_ASE_H
