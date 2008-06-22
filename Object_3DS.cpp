/**
* File : Object_3DS.cpp
* Description : Class to manage 3DS objects
* Author(s) : ALucchi
* Date of creation : 28/10/2007
* Modification(s) :
*/

//-------------------------------------------------------------------- INCLUDES
#include "Object_3DS.h"
#include "Log.h"

//---------------------------------------------------------------------- CONSTS

//--------------------------------------------------------------------- CLASSES

// 3DS parsing functions

// NOTE: don't blow near those functions!!!! they're so 
// brute forced that if you do it they'll fall appart >:)

Object_3DS::Object_3DS()
{
	numMats		= 0;
	mats		= NULL;
}

bool Object_3DS::load3DS(const char *filename) {
	assert(filename != NULL);

	fp = fopen(filename, "rb");
	if(!fp) {
		sysLog << "ERROR: cannot open 3DS file " << filename << "\n";
		return false;
	}

	// set some initial stuff

	currChunk	= -1;
	
	numLists	= 0;
	lists		= NULL;	

	// read first chunk and check the validness of the file

	readChunk();

	if(chunks[currChunk].id != MAIN) {
		sysLog << "ERORR: " << filename << " is invalid 3DS file\n";
		return false;
	}

	// parse the file chunks

	while(chunks[currChunk].read < chunks[currChunk].len) {
		processChunk();		
	}

	// close and clean up

	fclose(fp);
	fp = NULL;

	currChunk = -1;

	// have we read enough object data ?

	if(!verts || !faces) {
		freeLists();
		free();
		sysLog << "ERROR: insufficient object information loaded from 3DS file " << filename << "\n";
		return false;
	}

	// alloc visibility info array

	visible = new int[numFaces];
	
	// calculate stuff and solve material references
	// with other words, see which face which material uses

	if(lists && mats) {

		int matindex = -1, currFace;

		for(int i = 0; i < numLists; i++) {
	
			// find the material we need

			for(int j = 0; j < numMats; j++) {
				if(!strcmp(lists[i].matname, mats[j].name)) {
					matindex = j;
					break;
				}
			}

			// have we found a material ?

			if(matindex == -1)
				continue;

			// update the polys

			for(int j = 0; j < lists[i].numFaces; j++) {
				currFace = lists[i].faces[j];
				faces[currFace].mat = matindex;
			}
		}
	}

	freeLists();
	
	// finally calculate normals

	calcNormals();

	// good to go

	return true;
}

void Object_3DS::free(void) {
	if(verts) {
		delete [] verts;
		verts = NULL;
	}
	if(faces) {
		delete [] faces;
		faces = NULL;
	}
	if(mats) {
		delete [] mats;
		mats = NULL;
	}

	//numVerts	= 0;
	numFaces	= 0;
	numMats		= 0;

	if(visible) {
		delete [] visible;
		visible = NULL;
	}

	//numVisible	= 0;
}

void Object_3DS::processChunk(void) {

	readChunk();

	switch(chunks[currChunk].id) {
	case OBJ_EDITOR:

		while(chunks[currChunk].read < chunks[currChunk].len) {
			processChunk();
		}

		break;
	case OBJECT:

		chunks[currChunk].read += readString(NULL);

		while(chunks[currChunk].read < chunks[currChunk].len) {
			processChunk();
		}

		break;
	case MESH:

		if(!verts && !faces)
			readMesh();

		break;
	case MATERIAL:
	
		readMaterial();

		break;
	}

	skipChunk();
}

void Object_3DS::readMesh(void) {
	
	while(chunks[currChunk].read < chunks[currChunk].len) {
		
		readChunk();

		switch(chunks[currChunk].id) {
		case VERTICES:
			readVertexData();
			break;
		case FACES:
			readFaceData();
		}

		skipChunk();
	}
}

void Object_3DS::readVertexData(void)
{
	word count;

	fread(&count, 2, 1, fp);

	verts = new Vertex[count];
	for(int i = 0; i < count; i++) {
		fread(&verts[i].coordsLocal, sizeof(Vector3), 1, fp);
	}
	
	numVerts = count;

	chunks[currChunk].read += 2;
	chunks[currChunk].read += count * sizeof(Vector3);
}

void Object_3DS::readFaceData(void) {
	word count, faceData[4];

	fread(&count, 2, 1, fp);
	
	faces = new Face[count];
	
	for(int i = 0; i < count; i++) {
		fread(faceData, 8, 1, fp);
		faces[i].verts[0] = faceData[0];
		faces[i].verts[1] = faceData[1];
		faces[i].verts[2] = faceData[2];
	
		faces[i].mat = -1;	// no material initially
	}

	numFaces = count;

	chunks[currChunk].read += 2;
	chunks[currChunk].read += count * 8;	

	// read material lists if any

	while(chunks[currChunk].read < chunks[currChunk].len) {

		readChunk();

		if(chunks[currChunk].id != FACE_MATLIST) {
			skipChunk();
			continue;
		}
		
		// alloc new mat list
	
		MatList *newLists = new MatList[numLists+1];
		if(lists) {
			memcpy(newLists, lists, numLists);
			delete [] lists;
		}
		lists = newLists;
		int currList = numLists;
		memset(&lists[currList], 0, sizeof(MatList));
		numLists++;
		
		// read the material list

		chunks[currChunk].read += readString(lists[currList].matname);

		fread(&lists[currList].numFaces, 2, 1, fp);
		lists[currList].faces = new word[lists[currList].numFaces];		
		fread(lists[currList].faces, 2 * lists[currList].numFaces, 1, fp);
	
		skipChunk();
	}
}
	
void Object_3DS::readMaterial(void) {
	int currMat;	

	// allocate new material

	Material *matList = new Material[numMats+1];
	if(mats) {
		memcpy(matList, mats, numMats * sizeof(Material));
		delete [] mats;
	}
	mats = matList;
	currMat = numMats;
	memset(&mats[currMat], 0, sizeof(Material));
	numMats++;

	// load material info	

	while(chunks[currChunk].read < chunks[currChunk].len) {
	
		readChunk();
		
		switch(chunks[currChunk].id) {
		case MAT_NAME:

			chunks[currChunk].read += readString(mats[currMat].name);

			break;
		case MAT_AMBIENT:

			readColor(&mats[currMat].ambient);
	
			break;
		case MAT_DIFFUSE:
			
			readColor(&mats[currMat].diffuse);
	
		break;
		case MAT_SPECULAR:
	
			readColor(&mats[currMat].specular);

			break;
		case MAT_SHININESS:

			readPercent(&mats[currMat].sp);
		}

		skipChunk();
	}
}

void Object_3DS::readColor(RGBA_F *col) {
	
	readChunk();

	switch(chunks[currChunk].id) {
	case RGB_INT:
	case RGB_INT_GAMMA:
		byte rgb[3];
		fread(rgb, 3, 1, fp);
		col->r = (float)rgb[0] / 255;
		col->g = (float)rgb[1] / 255;
		col->b = (float)rgb[2] / 255;
		chunks[currChunk].read += 3;
		break;
	case RGB_FLOAT:
	case RGB_FLOAT_GAMMA:
		fread(&col->r, 4, 1, fp);
		fread(&col->g, 4, 1, fp);
		fread(&col->b, 4, 1, fp);
		chunks[currChunk].read += 12;
	}
	
	col->a = 1.0f;

	skipChunk();
}

void Object_3DS::readPercent(float *percent) {
	readChunk();

	switch(chunks[currChunk].id) {
	case PERCENT_INT:
		word val;
		fread(&val, 2, 1, fp);
		*percent = (float)val;
		chunks[currChunk].read += 2;
		break;
	case PERCENT_FLOAT:
		fread(percent, 4, 1, fp);
		chunks[currChunk].read += 4;
	}

	skipChunk();
}

void Object_3DS::readChunk(void) {
	int next = currChunk + 1;
	fread(&chunks[next].id, 2, 1, fp);
	fread(&chunks[next].len, 4, 1, fp);
	chunks[next].read = 6;
	currChunk = next;
}

void Object_3DS::skipChunk(void) {
	int left = chunks[currChunk].len - chunks[currChunk].read;
	if(left) {
		fseek(fp, left, SEEK_CUR);
	}

	// upadate parent

	if(currChunk)
		chunks[currChunk-1].read += chunks[currChunk].len;

	// decrement stack

	currChunk--;
}

int Object_3DS::readString(char *buf) {
	int c, bytes = 0;	
	do {
		c = fgetc(fp);
		if(buf) buf[bytes] = c;
		bytes++;
	} while( c );
	return bytes;
}

void Object_3DS::freeLists(void) {
	if(lists) {
		for(int i = 0; i < numLists; i++) {
			if(lists[i].faces)
				delete [] lists[i].faces;
		}
	
		delete [] lists;
		lists = NULL;
	}

	numLists = 0;
}

void Object_3DS::calcNormals(void)
{
	int *tblVertRefs, a, b, c, num;	
	Vector3 v1, v2, norm;

	// allocate the vertex referece table and clear it
	tblVertRefs = new int[numVerts];
	memset(tblVertRefs, 0, sizeof(int) * numVerts);
	
	// calculate polygon normals and fill the vertex ref table
	for(int i = 0; i < numFaces; i++) {
		a = faces[i].verts[0];
		b = faces[i].verts[1];
		c = faces[i].verts[2];
		
		v1 = verts[b].coordsLocal - verts[a].coordsLocal;
		v2 = verts[c].coordsLocal - verts[b].coordsLocal;

		norm = Cross(v1, v2);
		norm.Normalize();

		faces[i].normal = norm;

		verts[a].normal += norm;
		verts[b].normal += norm;
		verts[c].normal += norm;

		tblVertRefs[a]++;
		tblVertRefs[b]++;
		tblVertRefs[c]++;
	}

	// calc the vertex normals
	for(int i = 0; i < numVerts; i++) {
		verts[i].normal /= tblVertRefs[i];
		verts[i].normal.Normalize();
	}

	// free vert ref table
	delete [] tblVertRefs;
}