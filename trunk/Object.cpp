/**
* File : Object.cpp
* Description : Class used to store all the information relative to an object :
* coordinates, material, physical parameters,...
* Author(s) : ALucchi
* Date of creation : 28/10/2007
* Modification(s) :
*/

//-------------------------------------------------------------------- INCLUDES
#include "converter.h"
#include "Log.h"
#include "Maths/math3D.h"
#include "Object.h"
#include "TextureManager.h"

//---------------------------------------------------------------------- CONSTS

//--------------------------------------------------------------------- CLASSES

Object::Object(void) {
	numVerts	= 0;
	numFaces	= 0;

	verts				= NULL;
	faces				= NULL;
	materialName	= NULL;
	textureID		= -1;

	visible		= NULL;
	numVisible	= 0;

	numTexVertex = 0;
	bHasTexture = false;
}

Object::~Object(void)
{
	free();	
}

// import a mesh from an XML file
int Object::GetMesh(const char* filename)
{
	TiXmlDocument doc( filename );
	bool loadOkay = doc.LoadFile();

	if ( !loadOkay )
	{
		printf( "Could not load file '%s'. Error='%s'. Exiting.\n", filename, doc.ErrorDesc() );
		return ERR_TINY_READING_FILE;
	}

	TiXmlElement* mSubmeshesNode;
	TiXmlElement* rootElem = doc.RootElement();

	// submeshes
	mSubmeshesNode = rootElem->FirstChildElement("submeshes");
	if (!mSubmeshesNode)
	{
		return ERR_PARSING_MESH;
	}

	// Variables used to construct faces and triangles
	int iTriangle=0;
	int iVertex=0;
	int firstVertex=0;

	// Allocate memory to store faces and triangles
	int faceCount=0;
	for (TiXmlElement* smElem = mSubmeshesNode->FirstChildElement();
            smElem != 0; smElem = smElem->NextSiblingElement())
   {
      // Faces
      TiXmlElement* faces = smElem->FirstChildElement("faces");
      faceCount+=parseInt(faces->Attribute("count"));		
	}
	this->faces=new Triangle[faceCount];
	this->verts=new Vertex[faceCount*3];

	for (TiXmlElement* smElem = mSubmeshesNode->FirstChildElement();
            smElem != 0; smElem = smElem->NextSiblingElement())
   {
			const char* mat = smElem->Attribute("material");
			if (mat)
			{
				char* materialName;
				
#ifdef TRIM_MATERIAL_NAME
				// Search for the last occurence of the directory delimiter
				const char* posDel = strrchr(mat,'/');
				if(posDel)
				{
					materialName = new char[strlen(mat)-(posDel-mat)];
					strcpy(materialName,posDel+1);
				}
				else
#endif //TRIM_MATERIAL_NAME
				{
					materialName = new char[strlen(mat)+1];
					strcpy(materialName,mat);
				}

				// Create a new texture
				textureID = TextureManager::Instance().AddTexture(materialName);
			}

		/*
      // Read operation type
      const char* optype = smElem->Attribute("operationtype");
      if (optype)
      {
            if (!strcmp(optype, "triangle_list"))
            {
               sm->operationType = RenderOperation::OT_TRIANGLE_LIST;
            }
            else if (!strcmp(optype, "triangle_fan"))
            {
               sm->operationType = RenderOperation::OT_TRIANGLE_FAN;
            }
            else if (!strcmp(optype, "triangle_strip"))
            {
               sm->operationType = RenderOperation::OT_TRIANGLE_STRIP;
            }			
      }
		*/
      
      // Geometry		
      TiXmlElement* geomNode = smElem->FirstChildElement("geometry");
      if (geomNode)
      {
			int vertexCount=parseInt(geomNode->Attribute("vertexcount"));

			// save index of the first vertex that will be used later on when constructing triangles
			firstVertex=iVertex;

			// Iterate over all children (vertexbuffer entries) 
			for (TiXmlElement* elem = geomNode->FirstChildElement("vertexbuffer");
					elem != 0; elem = elem->NextSiblingElement())
			{

				// Skip non-vertexbuffer elems
				//if (stricmp(vbElem->Value(), "vertexbuffer")) continue;
			   				
				for (TiXmlElement* vbElem = elem->FirstChildElement("vertex");
				vbElem != 0; vbElem = vbElem->NextSiblingElement())
				{
					if(iVertex==vertexCount)
					{
						delete[] this->faces;
						delete[] this->verts;
						return ERR_PARSING_MESH;
					}

					// coordinates
					TiXmlElement* pos=vbElem->FirstChildElement("position");
					if(pos)
					{
						this->verts[iVertex].coordsLocal.x=parseReal(pos->Attribute("x"));
						this->verts[iVertex].coordsLocal.y=parseReal(pos->Attribute("y"));
						this->verts[iVertex].coordsLocal.z=parseReal(pos->Attribute("z"));

						// texture coordinates
						TiXmlElement* texpos=vbElem->FirstChildElement("texcoord");
						if(texpos)
						{
							this->verts[iVertex].texCoord.u=parseReal(texpos->Attribute("u"));
							this->verts[iVertex].texCoord.v=parseReal(texpos->Attribute("v"));

							if(this->verts[iVertex].texCoord.u < 0)
								this->verts[iVertex].texCoord.u=0;
							else
							if(this->verts[iVertex].texCoord.u > 1)
								this->verts[iVertex].texCoord.u=1;

							if(this->verts[iVertex].texCoord.v < 0)
								this->verts[iVertex].texCoord.v=0;
							else
							if(this->verts[iVertex].texCoord.v > 1)
								this->verts[iVertex].texCoord.v=1;
						}

						// normal
						TiXmlElement* normal=vbElem->FirstChildElement("normal");
						if(normal)
						{
							this->verts[iVertex].normal.x=parseReal(normal->Attribute("x"));
							this->verts[iVertex].normal.y=parseReal(normal->Attribute("y"));
							this->verts[iVertex].normal.z=parseReal(normal->Attribute("z"));
						}

						iVertex++;
					}
					else
					{
						delete[] this->faces;
						delete[] this->verts;
						printf("Number of vertices exceed");
						return ERR_PARSING_MESH;
					}
				}
			}
		}

      // Faces
      TiXmlElement* faces = smElem->FirstChildElement("faces");
      int faceCount=parseInt(faces->Attribute("count"));
      TiXmlElement* faceElem;
		// Constructing triangles
      for (faceElem = faces->FirstChildElement();
            faceElem != 0; faceElem = faceElem->NextSiblingElement())
      {
			if(iTriangle==faceCount)
			{
				delete[] this->faces;
				delete[] this->verts;
				return ERR_PARSING_MESH;
			}

			this->faces[iTriangle].a=firstVertex+parseInt(faceElem->Attribute("v1"));
			this->faces[iTriangle].b=firstVertex+parseInt(faceElem->Attribute("v2"));
			this->faces[iTriangle].c=firstVertex+parseInt(faceElem->Attribute("v3"));
			iTriangle++;
      }		

	}
	this->numFaces=iTriangle;
	this->numVerts=iVertex;

	return iTriangle;
}

void Object::free()
{
	if(verts) {
		delete [] verts;
		verts = NULL;
	}
	if(faces) {
		delete [] faces;
		faces = NULL;
	}

	numVerts	= 0;
	numFaces	= 0;

	if(visible) {
		delete [] visible;
		visible = NULL;
	}

	numVisible	= 0;
}