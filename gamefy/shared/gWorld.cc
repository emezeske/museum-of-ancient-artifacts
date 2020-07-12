#include "gWorld.h"
#include "utils.h"

gWorld *wQuad::world;

void wQuad::CalculateNormal() {
	Vector3 v1 = vertex[0].pos - vertex[1].pos;
	Vector3 v2 = vertex[3].pos - vertex[1].pos;
	normal = v1 * v2;	// cross product
	normal.Normalize();
} // end function CalculateNormal()

// reverse vertex order
void wQuad::InvertNormal() {
	// swap 0 and 3
	Vector3 tmpPt = vertex[0].pos;
	vertex[0].pos = vertex[3].pos;
	vertex[3].pos = tmpPt;

	// swap 1 and 2
	tmpPt = vertex[1].pos;
	vertex[1].pos = vertex[2].pos;
	vertex[2].pos = tmpPt;

	CalculateNormal();
} // end function InvertNormal()

void wQuad::Move(Vector3 &m) {
	for(int v=0; v<4; ++v) 
		vertex[v].pos = vertex[v].pos + m;
} // end function Move()

void wQuad::SetVertices(Vector3 * points) {
	for(int i = 0; i < 4; ++i) {
		vertex[i].pos.x = points[i].x;
		vertex[i].pos.y = points[i].y;
		vertex[i].pos.z = points[i].z;
	} // end for i
	CalculateNormal();
} // end function SetVertices()

void wQuad::SetTextureCoords(Texture2 * tex) {
	for(int i = 0; i < 4; ++i) {
		vertex[i].tex.u = tex[i].u;
		vertex[i].tex.v = tex[i].v;
	}
} // end function SetVertices()

void wQuad::SetTexture(int txt) {
	texture = txt;
} // end function SetTexture()

gWorld::gWorld(char *filename) {
	numtextures = numheightmaps = numterrains = 0;
	loadFromFile(filename);
	fileIn = NULL;
} // end constructor gWorld()

void gWorld::loadPolyFromFile(wQuad *cur) {
	float x, y, z, u, v;
	char aline[255];
	int curtexture = 0, curheightmap = 0;
	readString(fileIn, aline);
	sscanf(aline, "TEXTURE %d", &curtexture);
	cur->texture = curtexture;
	for(int vertex=0; vertex<4; vertex++) {
		readString(fileIn, aline);
		sscanf(aline, "%f %f %f %f %f", &x, &y, &z, &u, &v);
		cur->vertex[vertex].pos.x = x;
		cur->vertex[vertex].pos.y = y;
		cur->vertex[vertex].pos.z = z;
		cur->vertex[vertex].tex.u = u;
		cur->vertex[vertex].tex.v = v;
	} // end for vertex
	cur->next=NULL;
} // end function loadPolyFromFile()

wSector::wSector() {
	next = NULL;
	quads = NULL;
	polygons = 0;
} // end constructor wSector()

void gWorld::loadFromFile(char *world_filename) {
	int curtexture = 0, curheightmap = 0;
	char aline[255];
	char filename[255];
	float x, y, z, u, v;
	LTGA tLdr;

	fileIn = fopen(world_filename, "r");
	if(fileIn==NULL) {
		printf("Failed to load world \"%s\", exiting.\n", world_filename);
		exit(1);
	} // end if

	// read number of textures
	readString(fileIn, aline);
	sscanf(aline, "NUMTEXTURES %d", &numtextures);
	texture = new GLuint[128];
	glGenTextures(numtextures, texture);
	for(int t=0; t<numtextures; t++) {
		readString(fileIn, aline);
		sscanf(aline, "TGA %s", filename);
		strncpy(textureFilename[t], filename, 32);
		if(tLdr.LoadFromFile(filename)) {
			glBindTexture(GL_TEXTURE_2D, texture[t]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, tLdr.GetImageWidth(), tLdr.GetImageHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, tLdr.GetPixels());
		} else {
			printf("Error loading texture %d.\n", t);
			exit(1);
		}
	} // end for t

	// read number of terrains
	readString(fileIn, aline);
	sscanf(aline, "NUMTERRAINS %d", &numterrains);

	// read number of quads
	readString(fileIn, aline);
	readString(fileIn, aline);
	sectors = new wSector;	// just one sector
	int numpolys;
	sscanf(aline, "NUMQUADS %d", &numpolys);

	wQuad *cur;

	for(int quad=0; quad<numpolys; quad++) {
		cur = new wQuad;
		loadPolyFromFile(cur);
		sectors->AddQuad(cur);
		cur->CalculateNormal();
	} // end for quad

	fclose(fileIn);
	cur->world = this;
	printf("done reading world\n");
} // end function loadFromFile()

gWorld::~gWorld() {
	delete [] texture;
/*	{
		wTerrain *tmp1 = terrains, *tmp2 = NULL;
		while(tmp1!=NULL) {
			tmp2 = tmp1->next;
			delete tmp1;
			tmp1 = tmp2;
		}
		delete terrains;
	}*/
	{
		wQuad *tmp1 = sectors->quads, *tmp2 = NULL;
		while(tmp1!=NULL) {
			tmp2 = tmp1->next;
			delete tmp1;
			tmp1 = tmp2;
		} // end while
		delete sectors->quads;
	}
} // end function DeleteWorld()

void wSector::AddQuad(wQuad *newQuad) {
	newQuad->next = quads;
	newQuad->prev = NULL;
	quads = newQuad;
	if(quads->next!=NULL)quads->next->prev = quads;
	polygons++;
} // end function AddQuad()

void wSector::DeleteQuad(wQuad *condemned) {
	if(condemned->prev!=NULL)condemned->prev->next = condemned->next;	
	else quads = condemned->next;
	if(condemned->next!=NULL)condemned->next->prev = condemned->prev;
	delete condemned;
	polygons--;
} // end function DeleteQuad()

