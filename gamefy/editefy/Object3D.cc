#include "Object3D.h"

extern EditefyWorld *world;

Object3D::Object3D(const char *filename) {
	unsigned short chunk_id;
	unsigned int chunk_len;
	char t_name[32];
	FILE *model_file = fopen(filename, "r");

	vertices = NULL;
	tris = NULL;

	struct stat stat_buf;
	fstat(fileno(model_file), &stat_buf);
	int filesize = stat_buf.st_size;

	Material t_mat;

	// Go through the file and read in all the texture information first
	while(ftell(model_file) < filesize) {
		// Find out what kind of chunk is next and how big it is
		fread(&chunk_id,  sizeof(short), 1, model_file);	
		fread(&chunk_len, sizeof(int),   1, model_file);	

		// Deal with the chunk 
		switch(chunk_id) {
			// These chunks are just parent chunks with no data in them,
			// so we simply continue on to the next sub-chunk
			case MAIN_CHUNK:
			case ED_3D_CHUNK:
			case ED_MAT_CHUNK:
			case MAT_TEX1_CHUNK:
				break;
			// This contains the 3DSMAX name for a material
			case MAT_NAME_CHUNK:
				for(int i=0; i<32; ++i) {
					fread(&t_name[i], sizeof(char), 1, model_file);
					if(t_name[i] == '\0') {
						t_mat.name = new char[i+1];
						for(int z=0; z<i+1; ++z)
							t_mat.name[z] = t_name[z];
						break;
					}
				}
				break;
			// This contains the filename of the last material we read
			case MAT_FNAME_CHUNK:
				for(int i=0; i<32; ++i) {
					fread(&t_name[i], sizeof(char), 1, model_file);
					if(t_name[i] == '\0') {
						t_mat.filename = new char[i+1];
						for(int z=0; z<i+1; ++z)
							t_mat.filename[z] = t_name[z];
						break;
					}
				}
				world->materials.push_back(t_mat);
				break;
			// Skip this chunk because we don't know what it is
			default:
				fseek(model_file, chunk_len-6, SEEK_CUR);
				break;
		} // end switch
	} // end while

	// Move the file pointer to the beginning and extract the
	// other information
	rewind(model_file);
	while(ftell(model_file) < filesize) {
		// Find out what kind of chunk is next and how big it is
		fread(&chunk_id,  sizeof(short), 1, model_file);	
		fread(&chunk_len, sizeof(int),   1, model_file);	

		// Deal with the chunk 
		switch(chunk_id) {
			// These chunks are just parent chunks with no data in them,
			// so we simply continue on to the next sub-chunk
			case MAIN_CHUNK:
			case ED_3D_CHUNK:
			case ED_MAT_CHUNK:
			case MAT_TEX1_CHUNK:
			case TRI_CHUNK:
				break;
			// This chunk contains the name of the object
			case OBJ_CHUNK:
				for(int i=0; i<32; ++i) {
					fread(&t_name[i], sizeof(char), 1, model_file);
					if(t_name[i] == '\0') {
						name = new char[i+1];
						for(int z=0; z<i+1; ++z)
							name[z] = t_name[z];
						break;
					}
				}
				break;
			// Contains number of vertices and then a list of their x,y,z values
			case VERT_CHUNK:
				if(vertices == NULL) {
					fread(&num_vertices, sizeof(short), 1, model_file);
					vertices = new Vertex[num_vertices];
				} else 
					fseek(model_file, sizeof(short), SEEK_CUR);
				for(int i=0; i<num_vertices; ++i) {
					fread(&vertices[i].x, sizeof(float), 1, model_file);
					fread(&vertices[i].y, sizeof(float), 1, model_file);
					fread(&vertices[i].z, sizeof(float), 1, model_file);
				}
				break;
			// Contains the vertices used for each triangle
			case FACES_CHUNK:
				fread(&num_tris, sizeof(short), 1, model_file);
				tris = new Triangle[num_tris];
				for(int i=0; i<num_tris; ++i) {
					fread(&tris[i].vert_indxs[0], sizeof(short), 1, model_file);
					fread(&tris[i].vert_indxs[1], sizeof(short), 1, model_file);
					fread(&tris[i].vert_indxs[2], sizeof(short), 1, model_file);
					fseek(model_file, sizeof(short), SEEK_CUR);
				} // end for
				break;
			case FACE_MAT_CHUNK:
				// Extract material name
				for(int i=0; i<32; ++i) {
					fread(&t_name[i], sizeof(char), 1, model_file);
					if(t_name[i] == '\0')
						break;
				}
				// Find index of material with aforementioned name
				int mat_num;
				for(mat_num=0; mat_num<world->materials.size(); ++mat_num)
					if(!strcmp(t_name, world->materials[mat_num].name))
							break;
				// Read number of faces with this material
				short num_faces;
				fread(&num_faces, sizeof(short), 1, model_file);
				// Go through all the faces that use this material and assign them
				// this material number
				for(int i=0; i<num_faces; ++i) {
					short face_no;
					fread(&face_no, sizeof(short), 1, model_file);
					tris[face_no].material_num = mat_num;
				}
				break;
			// Contains the texture coordinates for each vertex
			case TEX_COORD_CHUNK:
				if(vertices == NULL) {
					fread(&num_vertices, sizeof(short), 1, model_file);
					vertices = new Vertex[num_vertices];
				} else 
					fseek(model_file, sizeof(short), SEEK_CUR);
				for(int i=0; i<num_vertices; ++i) {
					fread(&vertices[i].u, sizeof(float), 1, model_file);
					fread(&vertices[i].v, sizeof(float), 1, model_file);
				}
				break;
			// Skip this chunk because we don't know what it is
			default:
				fseek(model_file, chunk_len-6, SEEK_CUR);
				break;
		} // end switch
	}
	printf("num_tris: %d, num_vertices: %d\n", num_tris, num_vertices);
	fclose(model_file);
} // end constructor Object3D()

void Object3D::Draw() {
	glBindTexture(GL_TEXTURE_2D, world->materials[tris[0].material_num].tex_num);
	glBegin(GL_TRIANGLES);
	for(int t=0; t<num_tris; ++t)
		for(int v=0; v<3; ++v) {
			glTexCoord2f(vertices[tris[t].vert_indxs[v]].u, vertices[tris[t].vert_indxs[v]].v);
			glVertex3f(	vertices[tris[t].vert_indxs[v]].x,
					vertices[tris[t].vert_indxs[v]].y,
					vertices[tris[t].vert_indxs[v]].z);
		} // end for
	glEnd();
} // end function Draw()

void Object3D::Sketch() {
	glBegin(GL_LINE_LOOP);
	for(int t=0; t<num_tris; ++t)
		for(int v=0; v<3; ++v) {
			glVertex3f(	vertices[tris[t].vert_indxs[v]].x,
					vertices[tris[t].vert_indxs[v]].y,
					vertices[tris[t].vert_indxs[v]].z);
		} // end for
	glEnd();
} // end function Skecth()

Object3D::~Object3D() {
	delete [] vertices;
	delete [] tris;
	delete [] name;
} // end destructor ~Object3D()
