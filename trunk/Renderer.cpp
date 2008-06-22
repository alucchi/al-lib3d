/**
* File : renderer.cpp
* Description : Render objects on the display
*				  
* Author(s) : ALucchi
* Date of creation : 11/06/2007
* Modification(s) :
*/

//-------------------------------------------------------------------- INCLUDES

#include "Object.h"
#include "defs.h"
#include "converter.h"
#include "Application.h"
#include "Log.h"
#include "Renderer.h"

#ifndef USE_SDL
#include "minimal.h"
#endif

//--------------------------------------------------------------------- GLOBALS

#ifdef DEBUG
#include <list>
#include <algorithm>
/* list used to store faces for which errors have been detected when
*  rasterizing the face. */
std::list<int> listErrorFaces;
#endif

//--------------------------------------------------------------------- CLASSES

Renderer::Renderer(Display* d)
{
	display=d;
	currentTexture=0;
	Init();
}

Renderer::~Renderer(void)
{
	if(currentTexture)
	{
		delete[] currentTexture->data;
		delete currentTexture;
	}
	Deinit();
}

bool Renderer::Init(void)
{
	matWorld.identity();
	SetViewport(0,0,SCR_WIDTH,SCR_HEIGHT);
	return true;
}

void Renderer::Deinit(void)
{
	fov	= 0;
	focal = 0;
	memset(vp, 0, 4 * sizeof(long));
	halfVpW	= 0;
	halfVpH = 0;
	frameBuffer	= NULL;
	frameBufferPitch = 0;
}

void Renderer::SetFOV(float FOV)
{
	fov = CLAMP(FOV, MIN_FOV, MAX_FOV);
	calcFocal();
}

float Renderer::GetFOV(void) const
{
	return fov;
}

void Renderer::SetViewport(long x, long y, long w, long h)
{
	vp[0] = x;
	vp[1] = y;
	vp[2] = abs(w);
	vp[3] = abs(h);
	calcFocal();
}

void Renderer::GetViewport(long *x, long *y, long *w, long *h)
{
	if(x) *x = vp[0];
	if(y) *y = vp[1];
	if(w) *w = vp[2];
	if(h) *h = vp[3];	
}

void Renderer::GetViewport(long *viewport)
{
	assert(viewport != NULL);
	viewport[0] = vp[0];
	viewport[1] = vp[1];
	viewport[2] = vp[2];
	viewport[3] = vp[3];
}

void Renderer::SetTransMat(const Mat4x4& mat)
{
	transMat = mat;
}

void Renderer::Identity()
{
	matWorld.identity();
}

void Renderer::Rotate(const Vector3& vec)
{
	matWorld.rotate(vec.x,vec.y,vec.z);
}

void Renderer::Translate(const Vector3& vec)
{
	matWorld.translate(vec.x,vec.y,vec.z);
}

void Renderer::calcFocal(void)
{
	halfVpW = (float) (vp[2] - 1) * 0.5f;
	halfVpH = (float) (vp[3] - 1) * 0.5f;
	focal = halfVpW * cot( degstorads(fov / 2) );
}

void Renderer::SetFrameBuffer(void *bits, long pitch, dword bpp)
{
	assert(bits != NULL);

	switch(bpp) {
		case 32: pitch >>= 2;
			break;
		default: return;
	}

	frameBuffer				= bits;
	frameBufferPitch		= pitch;
}

void Renderer::SetCurrentTexture(Texture* texture)
{
	currentTexture = texture;
}

void Renderer::RenderObject(Object *obj)
{ 
	// Loop through the tris and transform their vertices
	int a,b,c;
	Vector3 v1,v2;
	int* visible=new int[obj->numFaces];
	int numVisible=0;

	// Load the texture associated to the object
	TextureManager::Instance().LoadTexture(obj->textureID);

	for(int i = 0; i < obj->numFaces; i++)
	{		
		a=obj->faces[i].a;
		b=obj->faces[i].b;
		c=obj->faces[i].c;
		
		obj->verts[a].coordsWorld = (obj->body.pos + obj->verts[a].coordsLocal) * matWorld;
		obj->verts[b].coordsWorld = (obj->body.pos + obj->verts[b].coordsLocal) * matWorld;
		obj->verts[c].coordsWorld = (obj->body.pos + obj->verts[c].coordsLocal) * matWorld;
		
		obj->faces[i].cenZ = (obj->verts[a].coordsWorld.z + 
									obj->verts[b].coordsWorld.z + 
									obj->verts[c].coordsWorld.z)	/ 3;			

		obj->faces[i].normal = (obj->verts[a].normal + 
									obj->verts[b].normal + 
									obj->verts[c].normal);
		obj->faces[i].normal /= 3;

#ifdef BACK_FACE_CULLING
		/* face not visible if dot product of surface normal and projector to any
		point on surface is nonnegative */
		v1 = obj->verts[b].coordsWorld - obj->verts[a].coordsWorld;
		v2 = obj->verts[c].coordsWorld - obj->verts[a].coordsWorld;
		float norm = v1.x*v2.y - v1.y*v2.x;		        
		//if (norm>=0)
		if (norm<0)
#endif //BACK_FACE_CULLING
		{
			visible[numVisible] = i;
			numVisible++;
		}
	}

	// Sort using selection algorithm
	int temp=obj->numFaces;
	obj->numFaces=numVisible;
	int pos;
	for(int i = 0; i < obj->numFaces - 1; i++)
	{
		pos = i;
		
		for(int j = i + 1; j < obj->numFaces; j++)
		{
			if(obj->faces[visible[j]].cenZ > obj->faces[visible[pos]].cenZ)
				pos = j;
		}

		if(pos != i) 
        {
			int temp = visible[i];
			visible[i] = visible[pos];
			visible[pos] = temp;
		}
	}

	// Render
	Vector3 L(0,0,1);
	L.Normalize();
	unsigned char col=40;
	unsigned char AMBIANT=20;
	char DIFFUSE=215;
	for(int i = 0; i < obj->numFaces; i++)
	{
		float angle=Dot(obj->faces[i].normal,L);
		if (angle<0)
			col = AMBIANT;
		else
			col = AMBIANT + (float) DIFFUSE * angle;

		rasterizeFace(obj,visible[i],fabs(angle));
	}
	delete[] visible;
	obj->numFaces=temp;
}

/* Rasterize the index-th face of the specified object
	Vertices have to be specified using the following system coordinates.
	They also have to be ordered in a clockwize direction.
	^ +y
	|
	|
	|	
	(0,0)-----> +x
	/
  /
  +z

  System coordinate used for texture mapping
	^ -v
	|
	|
	|		
  (0,0)-----> +u
  */
void Renderer::rasterizeFace(Object* obj,int index,float col)
{
	Vertex *verts[3];
   // Get pointers to vertices
	verts[0] = &obj->verts[obj->faces[index].a];
	verts[1] = &obj->verts[obj->faces[index].b];
	verts[2] = &obj->verts[obj->faces[index].c];	

	// Project points
	project(verts[0]);
	project(verts[1]);
	project(verts[2]);

	// Init span information
	minY = 10000;
	maxY = -10000;

	for(int i = 0; i < SCR_HEIGHT; i++)
	{
		spans[i].xStart = minY;
		spans[i].xEnd = maxY;
	}

	// Scan-convert the triangle
	scanEdge(verts[0], verts[1]);
	scanEdge(verts[1], verts[2]);
	scanEdge(verts[2], verts[0]);	

	Screen screen=display->GetScreen();	

	// Looking for the point having texture coord (u,v)=(0,0)
	int UVIndex0=-1;
	TRIANGLE_TYPE triangle_type = GENERAL;
	int indexMiddle=-1;
	for(int i=0;(i<3) && (UVIndex0==-1);i++)
	{
		for(int j=0;j<3;j++)
		{
			if(j==i) continue;
			if(verts[i]->scr[1] == verts[j]->scr[1])
			{
				if(verts[i]->scr[1] > verts[3-i-j]->scr[1])
				{
					triangle_type=FLAT_TOP; // 2 upper y-coordinates
					UVIndex0 = 3-i-j;					
				}
				else
				{
					triangle_type=FLAT_BOTTOM; // 2 lower y-coordinates

					if(verts[i]->scr[0] < verts[j]->scr[0])
						UVIndex0 = i;
					else
						UVIndex0 = j;
				}
				break;
			}
			else
			if(verts[i]->scr[1] < verts[j]->scr[1])
			{				
				if(verts[j]->scr[1] < verts[3-i-j]->scr[1])
				{
					// GENERAL triangle
					UVIndex0=i;
					indexMiddle=j;
					break;
				}
			}
		}
	}

	/* Vertices are ordered in a clockwize manner (before and after projection)
		sUVIndex points to the pixel having the lowest x and y coordinates
		In consequence, the next pixel (sUVIndex+1) is on the right and the one
		after is on the left */
	iLeft = UVIndex0+1;
	if(iLeft>2) iLeft=0;
	iRight = iLeft+1;
	if(iRight>2) iRight=0;
	
	Vector2 vAB;
	vAB.x = verts[UVIndex0]->scr[0] - verts[iLeft]->scr[0];
	vAB.y = verts[UVIndex0]->scr[1] - verts[iLeft]->scr[1];
	Vector2 vAC;
	vAC.x = verts[iRight]->scr[0] - verts[iLeft]->scr[0];
	vAC.y = verts[iRight]->scr[1] - verts[iLeft]->scr[1];
	float fNorm = vAB.x*vAC.y - vAB.y*vAC.x;
	if(fNorm<0)
	{
		SWAP(fTemp,iLeft,iRight);
	}

	/* Initialize the 3 texture coordinates for the 3 vertices of the specified
		triangle.
		Important : Vertices are ordered in a clockwize manner (before and after
		projection)
		*/
	u0=verts[UVIndex0]->texCoord.u*currentTexture->width;
	v0=verts[UVIndex0]->texCoord.v*currentTexture->height;

	u1=verts[iLeft]->texCoord.u*currentTexture->width;
	v1=verts[iLeft]->texCoord.v*currentTexture->height;

	u2=verts[iRight]->texCoord.u*currentTexture->width;
	v2=verts[iRight]->texCoord.v*currentTexture->height;

	diffY=1.0/(maxY-minY);

	if(triangle_type == FLAT_TOP)
	{
		ul=u0;
		vl=v0;
		ur=u0;
		vr=v0;

		if(currentTexture)
		{
			pixel=currentTexture->data;
			dudyl=(float)(u1-u0)*diffY;
			dvdyl=(float)(v1-v0)*diffY;
			dudyr=(float)(u2-u0)*diffY;
			dvdyr=(float)(v2-v0)*diffY;
		}
	}
	else
	if(triangle_type == FLAT_BOTTOM)
	{
		if(currentTexture)
		{
			pixel=currentTexture->data;
			dudyl=(float)(u1-u0)*diffY;
			dvdyl=(float)(v1-v0)*diffY;
			dudyr=(float)(u1-u2)*diffY;
			dvdyr=(float)(v1-v2)*diffY;
		}

		ul=u0;
		vl=v0;
		ur=u2;
		vr=v2;
	}
	else
	{
		// GENERAL
		ul=u0;
		vl=v0;
		ur=u0;
		vr=v0;

		if(currentTexture)
		{
			pixel=currentTexture->data;
			dudyl=(float)(verts[iLeft]->texCoord.u-verts[UVIndex0]->texCoord.u)*currentTexture->width/(verts[iLeft]->scr[1]-minY);
			dvdyl=(float)(verts[iLeft]->texCoord.v-verts[UVIndex0]->texCoord.v)*currentTexture->height/(verts[iLeft]->scr[1]-minY);
			dudyr=(float)(verts[iRight]->texCoord.u-verts[UVIndex0]->texCoord.u)*currentTexture->width/(verts[iRight]->scr[1]-minY);
			dvdyr=(float)(verts[iRight]->texCoord.v-verts[UVIndex0]->texCoord.v)*currentTexture->height/(verts[iRight]->scr[1]-minY);
		}
	}

	// Update yMiddle
	if(indexMiddle != -1)
		yMiddle = verts[indexMiddle]->scr[1];
	else
		yMiddle = maxY;

#ifdef DEBUG
	int cptError=0;
#endif //DEBUG

	for(int i = minY; i < maxY; i++)
	{
		if(i == yMiddle)
		{
			if(indexMiddle == iLeft)
			{
				// Change left side
				ul=verts[iLeft]->texCoord.u*currentTexture->width;
				vl=verts[iLeft]->texCoord.v*currentTexture->height;
				// Going from left to right
				dudyl=(float)(verts[iRight]->texCoord.u-verts[iLeft]->texCoord.u)*currentTexture->width/(maxY-verts[indexMiddle]->scr[1]);
				dvdyl=(float)(verts[iRight]->texCoord.v-verts[iLeft]->texCoord.v)*currentTexture->height/(maxY-verts[indexMiddle]->scr[1]);
			}
			else
			{
				// Change right side
				ur=verts[iRight]->texCoord.u*currentTexture->width;
				vr=verts[iRight]->texCoord.v*currentTexture->height;
				// Going from right to left
				dudyr=(float)(verts[iLeft]->texCoord.u-verts[iRight]->texCoord.u)*currentTexture->width/(maxY-verts[indexMiddle]->scr[1]);
				dvdyr=(float)(verts[iLeft]->texCoord.v-verts[iRight]->texCoord.v)*currentTexture->height/(maxY-verts[indexMiddle]->scr[1]);
			}
		}

		// Get the start and end points of the current span		
		x1 = spans[i].xStart;
		x2 = spans[i].xEnd;

		// Clip span
		if(x1 >= SCR_WIDTH || (x2 < 0))
			continue;
		if(x1 < 0)
		{
			x1 = 0;
		}
		if(x2 >= SCR_WIDTH)
		{
			x2 = SCR_WIDTH - 1;
		}

		ui=ul;
		vi=vl;

		// Compute u,v interpolants
		du=(ur-ul);
		dv=(vr-vl);

		dx=x2-x1;
		if(dx!=0)
		{
			du/=dx;
			dv/=dx;
		}

		Screen vidBits = &screen[x1+i*SCR_WIDTH];

		// Render span			
		for(int j = x1; j <= x2; j++)
		{
			if(Application::Instance().RenderingMode & Application::TEXTURED)
			{
				int indexPixel = int (ui) + (int) vi*currentTexture->width;
				//if(ui >= 0 && ui <= currentTexture->width && vi >= 0 && vi <= currentTexture->height)
				if(indexPixel < 0)
					indexPixel=0;
				if(indexPixel >= currentTexture->size)
					indexPixel=currentTexture->size-1;
				*vidBits = pixel[indexPixel];

//				else
//				{
//					*vidBits = col; // DEBUG
//#ifdef DEBUG
//					cptError++;
//#endif //DEBUG
//				}
			}
			if(Application::Instance().RenderingMode & Application::WIREFRAME)
			{
				if(j==x1 || j==x2 || i==minY || i==maxY-1)
					*vidBits=0;
			}

			vidBits++;

			// interpolate u,v
			ui+=du;
			vi+=dv;
		}

		//interploate u,v along right and left side
		ul+=dudyl;
		vl+=dvdyl;

		ur+=dudyr;
		vr+=dvdyr;
	}
#ifdef DEBUG_MODE
	if(cptError>10)
	{
		std::list<int>::iterator where = std::find(listErrorFaces.begin(), listErrorFaces.end(), index);
		if(where == listErrorFaces.end())
		{
			listErrorFaces.push_back(index);
			sysLog << "cptError: " << cptError <<"\n";
			sysLog << "Rotation: " << Application::Instance().GetRotation()<<"\n";
			sysLog << "index: " << index <<"\n";
			sysLog << "triangle_type: " << triangle_type <<"\n";			
			sysLog << "a: " << obj->faces[index].a <<", ";
			sysLog << "b: " << obj->faces[index].b <<", ";
			sysLog << "c: " << obj->faces[index].c <<"\n";		

			Vector3 vAB = obj->verts[obj->faces[index].b].coordsWorld - obj->verts[obj->faces[index].a].coordsWorld;
			Vector3 vAC = obj->verts[obj->faces[index].c].coordsWorld - obj->verts[obj->faces[index].a].coordsWorld;
			float fNorm = vAB.x*vAC.y - vAB.y*vAC.x;
			//sysLog << "vA: " << obj->verts[obj->faces[index].a].coordsWorld <<"\n";
			//sysLog << "vB: " << obj->verts[obj->faces[index].b].coordsWorld <<"\n";
			//sysLog << "vC: " << obj->verts[obj->faces[index].c].coordsWorld <<"\n";
			sysLog << "vA=[" <<
				obj->verts[obj->faces[index].a].scr[0] << " " <<
				obj->verts[obj->faces[index].a].scr[1] <<"]\n";
			sysLog << "vB=[" <<
				obj->verts[obj->faces[index].b].scr[0] << " " <<
				obj->verts[obj->faces[index].b].scr[1] <<"]\n";
			sysLog << "vC=[" <<
				obj->verts[obj->faces[index].c].scr[0] << " " <<
				obj->verts[obj->faces[index].c].scr[1] <<"]\n";			

			float UVNorm = (u1 - u0)*(v2 - v0) - (v1 - v0)*(u2 - u0);

			sysLog << "uv0='" <<
				obj->verts[obj->faces[index].a].texCoord.u << "," <<
				obj->verts[obj->faces[index].a].texCoord.v <<"'\n";
			sysLog << "uv1='" <<
				obj->verts[obj->faces[index].b].texCoord.u << "," <<
				obj->verts[obj->faces[index].b].texCoord.v <<"'\n";
			sysLog << "uv2='" <<
				obj->verts[obj->faces[index].c].texCoord.u << "," <<
				obj->verts[obj->faces[index].c].texCoord.v <<"'\n";

			sysLog << "Face normal: " << fNorm <<"\n";
			sysLog << "UV normal: " << UVNorm <<"\n";

			sysLog << "-------------------------\n";
		}
	}
#endif //DEBUG
}

/* Scans the edge between the 2 specified vertices (v1 and v2) and fills
the array of spans (horizontal lines) used to display the pixels */
void Renderer::scanEdge(const Vertex *v1, const Vertex *v2)
{
	Vertex *vF, *vL;
	Vertex_TexCoord *tF,*tL;
	long	fy, ly;

	// Skip horizontal edge (for now ;))
	if(v1->scr[1] >= v2->scr[1] - FLT_ERROR &&
	   v1->scr[1] <= v2->scr[1] + FLT_ERROR) return;

	// Order the vertices in increasing y order
	if(v1->scr[1] < v2->scr[1]) {
		vF = (Vertex*)v1;
		vL = (Vertex*)v2;
	}
	else {
		vF = (Vertex*)v2;
		vL = (Vertex*)v1;
	}

	// Round the y positions	
	fy = (long)vF->scr[1];
	ly = (long)vL->scr[1];

	// Clip (or reject) the edge
	if(fy >= SCR_HEIGHT)
		return;
	if(ly < 0)
		return;
	if(ly >= SCR_HEIGHT)
		ly = SCR_HEIGHT-1;

	// Calculate slope(s) of the edge
	float slopeX,x;
	long sx;

	x	= vF->scr[0];
	slopeX	= (vL->scr[0] - vF->scr[0]) / (vL->scr[1] - vF->scr[1]);

	// Increase x if needed (special case of clip)
	if(fy < 0)
	{
		x += slopeX * (0 - fy);
		fy = 0;
	}

	// Update the min/max span positions	
	if(fy < minY)
		minY = fy;
	if(ly > maxY)
		maxY = ly;

	// Scan convert the edge
	for(int i = fy; i < ly; i++)
	{
		sx = (long)x;

		if(sx < spans[i].xStart)
		{
			spans[i].xStart = sx;
		}
		if(sx > spans[i].xEnd)
		{
			spans[i].xEnd = sx;
		}
		x += slopeX;
	}
}

#ifdef DEBUG_MODE
int Renderer::L3DCBmp()
{

	unsigned int sizeBmp;
	Screen screen=display->GetScreen();

	int i,x,y;
	unsigned char r,g,b,c;
	unsigned int height,width;
	unsigned short bpp;

	/* Reading HEADER */
	width=currentTexture->width;
	height=currentTexture->height;
	bpp=currentTexture->bpp;
	sizeBmp=width*height*bpp/sizeof(char);

	int scrPadding=SCR_WIDTH-width;

#ifdef FLIP_Y
	byte* ptrData = &currentTexture->data[(height-1)*width];
	for (y=0;y<height;y++)
	{		
		for (x=0;x<width;x++)
		{
			*screen++ = *ptrData++;
		}		
		screen+=scrPadding;
		ptrData-=2*width;
	}
#else
	byte* ptrData = currentTexture->data;
	for (y=0;y!=height;y++)
	{		
		for (x=0;x<width;x++)
		{
			*screen++=*ptrData++;
		}		
		screen+=scrPadding;
	}
#endif

	return 0;
}
#endif //DEBUG