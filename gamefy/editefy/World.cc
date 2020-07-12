#include "World.h"

extern EditefySelection *selection;
extern EditefyWorld	*world;

void EditefyWorld::LoadLevel(const char *level_filename) {
	levelModel = new Object3D(level_filename);
} // end function LoadLevel()

void EditefyWorld::LoadMaterials() {
	for(int i=0; i<materials.size(); ++i) {
		char *f_name = new char[strlen(materials[i].filename) + 10];
		strcpy(f_name, "textures/");
		strcat(f_name, materials[i].filename);
		LTGA *l = new LTGA(f_name);
		if(l == 0)
		{
			printf("Error loading texture\n");
			exit(0);
		}

		glGenTextures(1, &materials[i].tex_num);
		glBindTexture(GL_TEXTURE_2D, materials[i].tex_num);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D (GL_TEXTURE_2D,
			      0,
			      GL_RGB,
			      l->GetImageWidth(),
			      l->GetImageHeight(),
			      0,
			      GL_RGB,
			      GL_UNSIGNED_BYTE,
			      l->GetPixels() );
		delete l;	
	}
} // end function LoadMaterials()

void EditefyWorld::AddStaticObject(unsigned short obj_indx) {
} // end function AddStaticObject()

void EditefyWorld::RemoveStaticObject(unsigned short obj_indx) {
} // end function RemoveStaticObject()

void EditefyWorld::RenderScene() {
	glColor3f(1.0, 1.0, 1.0);
	glEnable(GL_TEXTURE_2D);
		// draw the quads
		unsigned short cur = 0;
		while(cur<static_objects.size()) {
			if(selection->IsInSelection(cur))
				glColor3f(1.0, 0.5, 0.5);
			else
				glColor3f(1.0, 1.0, 1.0);
			static_objects[cur].Draw();
		} // end while
		levelModel->Draw();
	glDisable(GL_TEXTURE_2D);
} // end function RenderScene()

void EditefyWorld::RenderSketch() {
	glColor3f(1.0, 1.0, 1.0);
		// draw the quads
		unsigned short cur = 0;
		while(cur<static_objects.size()) {
			if(selection->IsInSelection(cur))
				glColor3f(1.0, 0.5, 0.5);
			else
				glColor3f(1.0, 1.0, 1.0);
			static_objects[cur].Sketch();
		} // end while
		levelModel->Sketch();
} // end function RenderSketch()()

void StaticObject::Draw() {
	glPushMatrix();
	glRotatef(xr, 1.0, 0.0, 0.0);
	glRotatef(yr, 0.0, 1.0, 0.0);
	glRotatef(zr, 0.0, 0.0, 1.0);
	glTranslatef(x, y, z);

	world->objectModels[model_indx].Draw();

	glPopMatrix();
} // end function Draw()

void StaticObject::Sketch() {
	glPushMatrix();
	glRotatef(xr, 1.0, 0.0, 0.0);
	glRotatef(yr, 0.0, 1.0, 0.0);
	glRotatef(zr, 0.0, 0.0, 1.0);
	glTranslatef(x, y, z);

	world->objectModels[model_indx].Sketch();

	glPopMatrix();
} // end function Sketch()
