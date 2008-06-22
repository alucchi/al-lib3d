/**
* File : renderer.h
* Description : Render objects on the display
*				  
* Author(s) : ALucchi
* Date of creation : 11/06/2007
* Modification(s) :
*/

#ifndef RENDERER_H
#define RENDERER_H

//-------------------------------------------------------------------- INCLUDES
#include "Display.h"
#include "Maths/math3D.h"
#include "TextureManager.h"

//---------------------------------------------------------------------- CONSTS

#define MIN_FOV		60
#define MAX_FOV		100

//----------------------------------------------------------------------- TYPES
typedef struct
{
	// Start and end position of the span
	long xStart;
	long xEnd;
	long uStart;
	long uEnd;
	long vStart;
	long vEnd;
} HSpan;

//--------------------------------------------------------------------- CLASSES

class Renderer
{
private:
	Display* display;

	Mat4x4		matWorld;			// World matrix

	long		minY,				// Y position of the first span
				maxY;				// Y position of the last span
	float		diffY;			// maxY-minY

	long x1, x2; // x positions of a span

	float dx;

	float u0,v0,u1,v1,u2,v2; // texture coordinates

	// variables used for texture mapping
	float ul,vl; // left
	float ur,vr; // right
	float ui,vi; // current
	float du,dv; // u,v interpolants
	float dudyl,dvdyl;
	float dudyr,dvdyr;

	int iLeft, iRight; // indices of the left and right vertices in a triangle

	byte* pixel; // pointer on the current pixel

	int yMiddle; // y coordinate of the middle vertex on the y axis

	float fTemp; // variable used for the SWAP macro

	HSpan		spans[SCR_HEIGHT];	// Horizontal spans

	Mat4x4		transMat;			// transform matrix

	float		fov,				// field-of-view
				focal;				// focal distance
	
	long		vp[4];				// viewport
	float		halfVpW,			// half viewport width
				halfVpH;			// half viewport height

	void		*frameBuffer;		// frame buffer bits
	long		frameBufferPitch;	// frame buffer pitch

	Texture* currentTexture;		// currentTexture (set with SetTexture)

	void calcFocal(void);

	// Project the specified vertex v
	inline void project(Vertex *v)
	{
		// We shouldn't perform division by zero, no ?
		if(v->coordsWorld.z >= -FLT_ERROR && v->coordsWorld.z <= FLT_ERROR)
			v->coordsWorld.z = 1.0f;

		// Project the point	
		float inv = ((float)FOCAL / v->coordsWorld.z);
		v->scr[0] = v->coordsWorld.x * inv + halfVpW + vp[0];
		v->scr[1] = -v->coordsWorld.y * inv + halfVpH + vp[1];
	}

	void scanEdge(const Vertex *v1, const Vertex *v2);	

	void rasterizeFace(Object* obj,int index,float col);

public:
	Renderer(Display* display);
	~Renderer(void);

	bool Init(void);
	void Deinit(void);

	float GetFOV(void) const;	
	void GetViewport(long *x, long *y, long *w, long *h);
	void GetViewport(long *viewport);
	void Identity();
	void RenderObject(Object *obj);
	void Rotate(const Vector3& vec);
	void SetCurrentTexture(Texture* texture);
	void SetFOV(float FOV);
	void SetFrameBuffer(void *bits, long pitch, dword bpp);
	void SetTransMat(const Mat4x4& mat);
	void SetViewport(long x, long y, long w, long h);
	void Translate(const Vector3& vec);	

#ifdef DEBUG
	int L3DCBmp(); //display a L3DC texture
#endif //DEBUG
};

#endif // RENDERER_H
