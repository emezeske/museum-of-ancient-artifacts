/***********************************************
*	Catmull-Clark style mesh smoothing     * 
*	Written by cthulhu 		       *
***********************************************/

#include <vector>
#include <cstdio>
#include <iostream>
using namespace std;

struct Vtx;
struct Face;
struct BustedEdge;

vector<Vtx> vrtcs;
vector<Face> faces;
vector<Face> busted_faces;
vector<BustedEdge> busted_edges;

// Used for keeping track of which edges were busted into what new edges during the
// busting loop so we don't end up with several vertices at the same point
struct BustedEdge {
	BustedEdge(int V1, int V2, int NEWV) : v1(V1), v2(V2), newv(NEWV) { }
	// First vertex, second vertex, and the new vertex that
	// resulted from snapping the edge made up by the first and
	// second vertices into two equal parts
	int v1, v2, newv;
}; // end struct BustedEdge

// Vertex struct
struct Vtx {
	double x, y, z;
	// Some information to keep track of which faces this vertex belongs to.
	int num_owners;
	Face* owners;
	void add_owner(Face*);
	Vtx(void) : x(0.0), y(0.0), z(0.0) { } 
	Vtx(double X, double Y, double Z) : x(X), y(Y), z(Z) { }
}; // end struct Vtx

// Face struct
struct Face {
	// Indices of the vertices that make up this quad
	int indices[4];
	Face(void) { }
	Face(int v0, int v1, int v2, int v3) {  indices[0] = v0;
						indices[1] = v1;
						indices[2] = v2;
						indices[3] = v3; }
	// Find centroid of this quad's vertices and bust the quad
	// into four new quads that share the centroid as a vertex
	void BustTo4(void);
}; // end struct Face

void Face::BustTo4(void) {
	int c;
	// Find the centroid of the whole quad
	Vtx centroid;
	for(c=0;c<4;++c) {
		centroid.x += vrtcs[indices[c]].x;
		centroid.y += vrtcs[indices[c]].y;
		centroid.z += vrtcs[indices[c]].z;
	}
	centroid.x /= 4;
	centroid.y /= 4;
	centroid.z /= 4;
	int centIndx = vrtcs.size();
	vrtcs.push_back(centroid);
	// Bust each edge into two edges and save the new vertices
	Vtx tempVert;
	int newVertIndx[4];
	for(c=0;c<4;++c) {
		bool wasAlreadyBusted = false;
		for(int z=0; z<busted_edges.size(); ++z)
			if((busted_edges[z].v1 == indices[c]) || (busted_edges[z].v2 == indices[c]) )
				if((busted_edges[z].v1 == indices[(c+1)%4]) || (busted_edges[z].v2 == indices[(c+1)%4]) )
				{
					newVertIndx[c] = busted_edges[z].newv;
					wasAlreadyBusted = true;
				}
		if(!wasAlreadyBusted) {
			tempVert.x = (vrtcs[indices[c]].x + vrtcs[indices[(c+1)%4]].x)/2;
			tempVert.y = (vrtcs[indices[c]].y + vrtcs[indices[(c+1)%4]].y)/2;
			tempVert.z = (vrtcs[indices[c]].z + vrtcs[indices[(c+1)%4]].z)/2;
			newVertIndx[c] = vrtcs.size();
			vrtcs.push_back(tempVert);
			busted_edges.push_back(BustedEdge(indices[c], indices[(c+1)%4], newVertIndx[c]));
		}
	}
	// Finally, use the centroid and the new vertices to form four new quads
	for(c=0; c<4;++c) {
		busted_faces.push_back(Face(indices[c], newVertIndx[c], centIndx, newVertIndx[(c+3)%4]));
	}
} // end BustTo4()

void bustEmAll(void) {
	// Bust each of the faces into four smaller faces 
	for(int i=0; i<faces.size(); ++i) {
		faces[i].BustTo4();
	}
	faces = busted_faces;
	busted_faces.clear();
	busted_edges.clear();
} // end bustEmAll()

int main(void) {
	// Make a cube
	vrtcs.push_back(Vtx(-0.5, -0.5, -0.5));
	vrtcs.push_back(Vtx( 0.5, -0.5, -0.5));
	vrtcs.push_back(Vtx( 0.5,  0.5, -0.5));
	vrtcs.push_back(Vtx(-0.5,  0.5, -0.5));
	vrtcs.push_back(Vtx(-0.5, -0.5,  0.5));
	vrtcs.push_back(Vtx( 0.5, -0.5,  0.5));
	vrtcs.push_back(Vtx( 0.5,  0.5,  0.5));
	vrtcs.push_back(Vtx(-0.5,  0.5,  0.5));

	faces.push_back(Face(0, 1, 2, 3));
	faces.push_back(Face(0, 1, 5, 4));
	faces.push_back(Face(1, 2, 6, 5));
	faces.push_back(Face(2, 3, 7, 6));
	faces.push_back(Face(3, 0, 4, 7));
	faces.push_back(Face(4, 5, 6, 7));

	int num = 0;
	printf("Catmull-Clark mesh smoother at your service.  How many iterations of smoothing would you like today?\n> ");
	cin >> num;
	if(num > 20)
		printf("A fuckload of iterations it is! (This will not finish during your lifetime)\n");
	else if(num > 5)
		printf("%d iterations it is! (This may take a while)\n", num);
	else if(num > 1)
		printf("%d iterations it is!\n", num);
	else if(num == 1)
		printf("One iteration it is!\n");
	else {
		printf("Fuck off!\n");
		exit(69);
	}

	for(int z=0;z<num;++z)
		bustEmAll();

	printf("Now there are %d faces and %d vertices\n", faces.size(), vrtcs.size());

	return 0;
} // end main()
