#include "Frustum.h"
#include "../shared/gCWorld.h"

extern gCWorld *world;
extern Frustum *viewFrustum;

unsigned char OctalTree::recursionLevel=0;// FIXME: note this starts at 0 while the compiler starts at 1; change the compiler.

OctalTree::OctalTree() {
	// does nothing
}  // end constructor OctalTree()

OctalTree * OctalTree::readOrNull(FILE *in) {
	unsigned short nullIndicator = 0x6969;
	unsigned short nextShort = 0;
	fread(&nextShort, sizeof(nextShort) , 1, in);
	if(nextShort == nullIndicator) {
		return NULL;
	} else  {
		OctalTree * temp = new OctalTree();
		fseek(in, -sizeof(unsigned short), SEEK_CUR);
		temp->readFromFile(in);
		return temp;
	}
} // end function readOrNull()

void OctalTree::readFromFile(FILE *in) {
	fread(&boundingSphere,	sizeof(boundingSphere), 1, in);
	fread(&boundingBox,	sizeof(boundingBox), 	1, in);
	fread(&tris,		sizeof(tris), 		1, in);
	tri = new Tri[tris];
	for(int t=0; t < tris; ++t)
		fread(&tri[t],	sizeof(Tri),		1, in);

	_000 = readOrNull(in);
	_001 = readOrNull(in);
	_010 = readOrNull(in);
	_011 = readOrNull(in);
	_100 = readOrNull(in);
	_101 = readOrNull(in);
	_110 = readOrNull(in);
	_111 = readOrNull(in);
} // end function readFromFile()

void OctalTree::drawSelfAndMaybeChildren(bool drawAllChildren) {
	draw();
	if(_000 != NULL) _000->testDraw(drawAllChildren);
	if(_001 != NULL) _001->testDraw(drawAllChildren);
	if(_010 != NULL) _010->testDraw(drawAllChildren);
	if(_011 != NULL) _011->testDraw(drawAllChildren);
	if(_100 != NULL) _100->testDraw(drawAllChildren);
	if(_101 != NULL) _101->testDraw(drawAllChildren);
	if(_110 != NULL) _110->testDraw(drawAllChildren);
	if(_111 != NULL) _111->testDraw(drawAllChildren);
} // end function drawAllChildren()

void OctalTree::testDraw(bool drawAllChildren) {
	recursionLevel++;
	//printf("node being testDrawn\n");
	// this node's parent is entirely within the frustum
	if(drawAllChildren) {
		drawSelfAndMaybeChildren(true);
		recursionLevel--;
		return;
	} // end if

	// this node's parent intersects with the frustum
	FrustumResult result = viewFrustum->IntersectionWithAaBox(boundingSphere.c, recursionLevel);
	//printf("frustumresult == FR_OUT %d\n", result==FR_OUT);
	if     (result==FR_OUT) {
		recursionLevel--;
		return;
	} else if(result==FR_IN) {
		drawSelfAndMaybeChildren(true);	// draw all children
		recursionLevel--;
		return;
	} else {
		drawSelfAndMaybeChildren(false);	// test all children
		recursionLevel--;
	}

	
} // end function testDraw()

void OctalTree::draw() {
	for(int p=0; p<tris; ++p) {
		glBindTexture(GL_TEXTURE_2D, world->texture[tri[p].texture]);
		glBegin(GL_TRIANGLES);
			glTexCoord2f(	tri[p].vertex[0].tex.u, tri[p].vertex[0].tex.v);
			glVertex3f(	tri[p].vertex[0].pos.x, tri[p].vertex[0].pos.y, tri[p].vertex[0].pos.z);
			glTexCoord2f(	tri[p].vertex[1].tex.u, tri[p].vertex[1].tex.v);
			glVertex3f(	tri[p].vertex[1].pos.x, tri[p].vertex[1].pos.y, tri[p].vertex[1].pos.z);
			glTexCoord2f(	tri[p].vertex[2].tex.u, tri[p].vertex[2].tex.v);
			glVertex3f(	tri[p].vertex[2].pos.x, tri[p].vertex[2].pos.y, tri[p].vertex[2].pos.z);
		glEnd();
	}

} // end function draw()

OctalTree::~OctalTree() {
	if(_000 != NULL) delete _000;
	if(_001 != NULL) delete _001;
	if(_010 != NULL) delete _010;
	if(_011 != NULL) delete _011;
	if(_100 != NULL) delete _100;
	if(_101 != NULL) delete _101;
	if(_110 != NULL) delete _110;
	if(_111 != NULL) delete _111;
	delete [] tri;
} // end deconstructor ~OctalTree()

gCWorld::gCWorld(const char *filename) {
	FILE *in = fopen(filename, "r");
	if(in != NULL)
	{
		readFromFile(in);
		fclose(in);
	} else
		exit(0);
} // end constructor gCWorld()

void gCWorld::readFromFile(FILE *in) {
	CWorldHeader	fileHeader;
	CSectorHeader	curSectorHeader;
	fread(&fileHeader, 	sizeof(fileHeader), 		1, in);
	fread(&curSectorHeader, sizeof(curSectorHeader), 	1, in);

	nPolys = curSectorHeader.nTriangles;
	loadTextures(fileHeader);

	treeRoot = new OctalTree();
	treeRoot->readFromFile(in);
} // end function readFromFile()

void gCWorld::loadTextures(const CWorldHeader &fileHeader) {
	numtextures = fileHeader.nTextures;

	texture = new GLuint[numtextures];
	glGenTextures(numtextures, texture);
	LTGA tLdr;

	for(int n=0; n<numtextures; ++n)
	{
		tLdr.LoadFromFile(&fileHeader.textureFilename[n][0]);
		glBindTexture(GL_TEXTURE_2D, texture[n]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, tLdr.GetImageWidth(), tLdr.GetImageHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, tLdr.GetPixels());
	}
} // end function loadTextures()

void gCWorld::renderScene() {
	treeRoot->testDraw(false);
} // end function renderScene()

void gCWorld::renderSkybox() {
	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0, 0.5, 0.5);
	glBindTexture(GL_TEXTURE_2D, texture[0]);//topSBtx]);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-32.0, 32.0, -32.0);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(+32.0, 32.0, -32.0);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(+32.0, 32.0, +32.0);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-32.0, 32.0, +32.0);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texture[0]);//sidesSBtx]);//left

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-32.0, -32.0, +32.0);
	glTexCoord2f(0.25, 0.0);
	glVertex3f(-32.0, -32.0, -32.0);
	glTexCoord2f(0.25, 1.0);
	glVertex3f(-32.0, +32.0, -32.0);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-32.0, +32.0, +32.0);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texture[0]);//sidesSBtx]);//right

	glBegin(GL_QUADS);
	glTexCoord2f(0.5, 0.0);
	glVertex3f(+32.0, -32.0, -32.0);
	glTexCoord2f(0.75, 0.0);
	glVertex3f(+32.0, -32.0, +32.0);
	glTexCoord2f(0.75, 1.0);
	glVertex3f(+32.0, +32.0, +32.0);
	glTexCoord2f(0.5, 1.0);
	glVertex3f(+32.0, +32.0, -32.0);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texture[0]);//sidesSBtx]);//back

	glBegin(GL_QUADS);
	glTexCoord2f(0.75, 0.0);
	glVertex3f(+32.0, -32.0, +32.0);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-32.0, -32.0, +32.0);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-32.0, +32.0, +32.0);
	glTexCoord2f(0.75, 1.0);
	glVertex3f(+32.0, +32.0, +32.0);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texture[0]);//sidesSBtx]);//front

	glBegin(GL_QUADS);
	glTexCoord2f(0.5, 1.0);
	glVertex3f(+32.0, +32.0, -32.0);
	glTexCoord2f(0.25, 1.0);
	glVertex3f(-32.0, +32.0, -32.0);
	glTexCoord2f(0.25, 0.0);
	glVertex3f(-32.0, -32.0, -32.0);
	glTexCoord2f(0.5, 0.0);
	glVertex3f(+32.0, -32.0, -32.0);
	glEnd();
} // end function renderSkybox()

gCWorld::~gCWorld() {
	delete treeRoot;
	delete [] texture;
}  // end deconstructor ~gCWorld()

