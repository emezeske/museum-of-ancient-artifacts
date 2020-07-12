#include "../shared/gCWorld.h"
#include <stdio.h>

unsigned char	OctalTree::recursionLevel=0;
gCWorld *	OctalTree::lvl = NULL;
bool *		triangleHasBeenAdded;

extern gWorld	*world;
extern Tri	*triangle;

void gCWorld::writeToFile(FILE *out) {
	// initialize
	CWorldHeader	fileHeader;
	CSectorHeader	curSectorHeader;
	fileHeader.nSectors =	1;
	fileHeader.nTextures =	numtextures;
	for(int n=0; n<numtextures; ++n) {
		strncpy(&fileHeader.textureFilename[n][0], &world->textureFilename[n][0], 32);
	} // end for n

	// write the file
	fwrite(&fileHeader, sizeof(fileHeader), 1, out);
	curSectorHeader.nTriangles = nPolys;
	curSectorHeader.nPortals = 0;
	fwrite(&curSectorHeader, sizeof(curSectorHeader), 1, out);
	// write list of portals
	// write the octal tree
	treeRoot->writeToFile(out);
} // end function writeToFile()

bool OctalTree::vertexContained(Vector3 v) {
	if(	v.x<	boundingBox.xmin|| 
		v.x>	boundingBox.xmax|| 
		v.y<	boundingBox.ymin|| 
		v.y>	boundingBox.ymax|| 
		v.z<	boundingBox.zmin|| 
		v.z>	boundingBox.zmax) return false;
	return true;
} // end function vertexContained()

// return true iff poly is ENTIRELY contained within boundingBox
bool OctalTree::polygonContained(Tri *poly) {
	bool contained = true;
	for(int v=0; v<3; v++)
		contained &= vertexContained(poly->vertex[v].pos);
	return contained;
} // end function polygonContained()

// returns the number of triangles completely within the bounding box
int OctalTree::numberOfTrisContained(Tri *polys, int total) {
	int count=0;
	for(int i=0; i<total; i++)
		if(polygonContained(&polys[i])) count++;
	return count;
} // end function numberOfTrisContained()

// returns the number of triangle for which this is the smallest bounding box
// this MUST be called for a parent AFTER it is called for the children
int OctalTree::numberOfTrisOwned(Tri *polys, int total) {
	int count=0;
	for(int i=0; i<total; i++) 
		if(!triangleHasBeenAdded[i]&&polygonContained(&polys[i])) {
			count++;
			printf("triangle %d was added at level %d\n", i, recursionLevel);
		}
	return count;
} // end function numberOfTrisContained()

char result[256];

char *someTabs(int n) {
	strcpy(result, "\0");
	for(int c = 0; c < n; ++c) {
		strcat(result, "  ");
	} // end for c
	return result;
} // end function someTabs()

// add all triangles which fit completely within the node to the node's list
void OctalTree::AddBoundedTriangles(Tri *polys, int nPolys) {
	unsigned int tri = 0;
	for(int i=0; i<nPolys; i++)
		if(!triangleHasBeenAdded[i]&&polygonContained(&polys[i])) {
			triangleHasBeenAdded[i] = true;
			printf("%sadding triangle %d as the %dth triangle\n",someTabs(recursionLevel), i, tri);
			triangleIndeces[tri++] = i;
		} // end if, for i

} // end function AddBoundedTriangles()

// write tree, or write a "special" null indicator to the file
void OctalTree::writeOrNull(OctalTree *tree, FILE *out) {
	unsigned short nullIndicator = 0x6969;
	if(tree==NULL)
	{
		fwrite(&nullIndicator,	sizeof(nullIndicator),	1,	out);
	}
	else
		tree->writeToFile(out);
} // end function writeOrNull()

void OctalTree::writeToFile(FILE *out) {
	fwrite(&boundingSphere,	sizeof(boundingSphere),	1,	out);
	fwrite(&boundingBox,	sizeof(boundingBox),	1,	out);
	fwrite(&tris,		sizeof(tris),		1,	out);
	for(int t=0; t < tris; ++t) {
		printf("index = %d\tmax triangles = %d\n", triangleIndeces[t], world->sectors->polygons*2);
		fwrite(&triangle[triangleIndeces[t]],	sizeof(Tri),	1,	out);
	} // end for t
	// write each child, if necessary
	writeOrNull(_000, out);
	writeOrNull(_001, out);
	writeOrNull(_010, out);
	writeOrNull(_011, out);
	writeOrNull(_100, out);
	writeOrNull(_101, out);
	writeOrNull(_110, out);
	writeOrNull(_111, out);

} // end function writeToFile()

// this function calls itself for the 8 children if necessary
NodeResult gCWorld::buildTree(OctalTree *node, Tri *polys, int nPolys) {
	node->tris = node->numberOfTrisContained(polys, nPolys);
	//printf("%sa node contains %d triangles\n", someTabs(node->recursionLevel), node->tris);
	if(node->tris==0) {
		node->recursionLevel--;
		return NODE_EMPTY;
	}

	if((node->tris)>MAX_POLYS_PER_NODE) {

		float quarterLength = (node->boundingBox.xmax-node->boundingBox.xmin)*0.25;

		node->_000 = new OctalTree(node->boundingBox.xmin + quarterLength, node->boundingBox.ymin + quarterLength, node->boundingBox.zmin + quarterLength, quarterLength, this);
		if(buildTree(node->_000, polys, nPolys)==NODE_EMPTY) {
			delete node->_000;
			node->_000 = NULL; }

		node->_001 = new OctalTree(node->boundingBox.xmin + quarterLength, node->boundingBox.ymin + quarterLength, node->boundingBox.zmax - quarterLength, quarterLength, this);
		if(buildTree(node->_001, polys, nPolys)==NODE_EMPTY) {
			delete node->_001;
			node->_001 = NULL; }

		node->_010 = new OctalTree(node->boundingBox.xmin + quarterLength, node->boundingBox.ymax - quarterLength, node->boundingBox.zmin + quarterLength, quarterLength, this);
		if(buildTree(node->_010, polys, nPolys)==NODE_EMPTY) {
			delete node->_010;
			node->_010 = NULL; }

		node->_011 = new OctalTree(node->boundingBox.xmin + quarterLength, node->boundingBox.ymax - quarterLength, node->boundingBox.zmax - quarterLength, quarterLength, this);
		if(buildTree(node->_011, polys, nPolys)==NODE_EMPTY) {
			delete node->_011;
			node->_011 = NULL; }

		node->_100 = new OctalTree(node->boundingBox.xmax - quarterLength, node->boundingBox.ymin + quarterLength, node->boundingBox.zmin + quarterLength, quarterLength, this);
		if(buildTree(node->_100, polys, nPolys)==NODE_EMPTY) {
			delete node->_100;
			node->_100 = NULL; }

		node->_101 = new OctalTree(node->boundingBox.xmax - quarterLength, node->boundingBox.ymin + quarterLength, node->boundingBox.zmax - quarterLength, quarterLength, this);
		if(buildTree(node->_101, polys, nPolys)==NODE_EMPTY) {
			delete node->_101;
			node->_101 = NULL; }

		node->_110 = new OctalTree(node->boundingBox.xmax - quarterLength, node->boundingBox.ymax - quarterLength, node->boundingBox.zmin + quarterLength, quarterLength, this);
		if(buildTree(node->_110, polys, nPolys)==NODE_EMPTY) {
			delete node->_110;
			node->_110 = NULL; }

		node->_111 = new OctalTree(node->boundingBox.xmax - quarterLength, node->boundingBox.ymax - quarterLength, node->boundingBox.zmax - quarterLength, quarterLength, this);
		if(buildTree(node->_111, polys, nPolys)==NODE_EMPTY) {
			delete node->_111;
			node->_111 = NULL; }

		//printf("%sfinished subdividing node\n", someTabs(node->recursionLevel));
		node->tris = node->numberOfTrisOwned(polys, nPolys);
		node->triangleIndeces = new unsigned int[node->tris];
		node->AddBoundedTriangles(polys, nPolys);
		printf("tris = %d\n", node->tris);
		node->recursionLevel--;
		return NODE_SUBDIVIDED;

	} else {

		/*printf("%send node reached: %d\t%+4.2f < x < %+4.2f\t%+4.2f < y < %+4.2f\t%+4.2f < z < %+4.2f\t\n",
						someTabs(node->recursionLevel),
						node->tris,
						node->boundingBox.xmin,
						node->boundingBox.xmax,
						node->boundingBox.ymin,
						node->boundingBox.ymax,
						node->boundingBox.zmin,
						node->boundingBox.zmax);
		*/	

		node->triangleIndeces = new unsigned int[node->tris];
		node->AddBoundedTriangles(polys, nPolys);

		node->recursionLevel--;
		return NODE_END;

	} // end if

} // end function buildTree()

gCWorld::gCWorld(int numtextures, int nPolys, Tri *tri) {
	this->numtextures = numtextures;
	this->nPolys = nPolys;
	// loop through triangles, find the least and greatest vertex coord for each dimension
	float	leastX=1000.0,
		leastY=1000.0,
		leastZ=1000.0,
		greatestX=-1000.0,
		greatestY=-1000.0,
		greatestZ=-1000.0;
	for(int t=0; t<nPolys; ++t) {
		for(int v=0; v<3; ++v) {
			if(tri[t].vertex[v].pos.x < leastX)
				leastX = tri[t].vertex[v].pos.x;
			if(tri[t].vertex[v].pos.y < leastY)
				leastY = tri[t].vertex[v].pos.y;
			if(tri[t].vertex[v].pos.z < leastZ)
				leastZ = tri[t].vertex[v].pos.z;

			if(tri[t].vertex[v].pos.x > greatestX)
				greatestX = tri[t].vertex[v].pos.x;
			if(tri[t].vertex[v].pos.y > greatestY)
				greatestY = tri[t].vertex[v].pos.y;
			if(tri[t].vertex[v].pos.z > greatestZ)
				greatestZ = tri[t].vertex[v].pos.z;
		} // end for v
	} // end for t
	
	// FIXME for debugging
	//printf("%f<x<%f\t%f<y<%f\t%f<z<%f\n", leastX, greatestX, leastY, greatestY, leastZ, greatestZ);

	// which range is greatest?
	float	rangeX = greatestX-leastX,
		rangeY = greatestY-leastY,
		rangeZ = greatestZ-leastZ,
		greatestRange = rangeX;
	if(rangeY>greatestRange) greatestRange = rangeY;
	if(rangeZ>greatestRange) greatestRange = rangeZ;

	float	centerX = (greatestX+leastX)/2,
		centerY = (greatestY+leastY)/2,
		centerZ = (greatestZ+leastZ)/2;

	// create the root node
	treeRoot = new OctalTree(centerX, centerY, centerZ, greatestRange/2, this);
	triangleHasBeenAdded = new bool[nPolys];
	memset(triangleHasBeenAdded, 0, nPolys * sizeof(bool));
	buildTree(treeRoot, tri, nPolys);
	FILE *out= fopen("./cworld", "w");
	writeToFile(out);
	fclose(out);
} // end constructor gCWorld()

gCWorld::~gCWorld() {
	delete [] triangleHasBeenAdded;
	delete treeRoot;
} // end deconstructor ~gCWorld()

OctalTree::OctalTree(float x, float y, float z, float halfLength, gCWorld* level) {
	boundingBox.xmin = x-halfLength;
	boundingBox.xmax = x+halfLength;
	boundingBox.ymin = y-halfLength;
	boundingBox.ymax = y+halfLength;
	boundingBox.zmin = z-halfLength;
	boundingBox.zmax = z+halfLength;
	boundingSphere.c.x = x;
	boundingSphere.c.y = y;
	boundingSphere.c.z = z;
	boundingSphere.radiusSq = halfLength*halfLength*3;
	boundingSphere.radius = sqrt(boundingSphere.radiusSq);
	_000 = _001 = _010 = _011 = _100 = _101 = _110 = _111 = NULL;
	tris = 0;
	lvl = level;
	recursionLevel++;
} // end constructor OctalTree()

OctalTree::~OctalTree() {
	if(_000!=NULL) delete _000;
	if(_001!=NULL) delete _001;
	if(_010!=NULL) delete _010;
	if(_011!=NULL) delete _011;
	if(_100!=NULL) delete _100;
	if(_101!=NULL) delete _101;
	if(_110!=NULL) delete _110;
	if(_111!=NULL) delete _111;
	delete [] triangleIndeces;
} // end deconstructor ~OctalTree()

