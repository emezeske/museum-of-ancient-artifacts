#include "selection.h"

extern EditefyWorld *world;

void EditefySelection::AddToSelection(unsigned short obj_indx) {
	for(int i=0; i<selected_objects.size(); ++i)
		if(selected_objects[i] == obj_indx)
			return;
	selected_objects.push_back(obj_indx);
} // end function AddToSelection()

void EditefySelection::RemoveFromSelection(unsigned short obj_indx) {
	for(int i=0; i<selected_objects.size(); ++i) 
		if(selected_objects[i] == obj_indx) {
			selected_objects.erase(selected_objects.begin() + obj_indx);
			break;
		}

} // end function RemoveFromSelection()

bool EditefySelection::IsInSelection(unsigned short obj_indx) {
	for(int i=0; i<selected_objects.size(); ++i) 
		if(selected_objects[i] == obj_indx)
			return true;
	return false;
} // end function IsInSelection()

void EditefySelection::ClearSelection() {
	selected_objects.clear();
} // end function ClearSelection()

void EditefySelection::DeleteSelection() {
	for(int i=0; i<selected_objects.size(); ++i) 
		world->RemoveStaticObject(selected_objects[i]);
	selected_objects.clear();
} // end function DeleteSelection()

void EditefySelection::MoveSelection(Vector3 &p) {
	for(int i=0; i<selected_objects.size(); ++i)  {
		world->static_objects[selected_objects[i]].x += p.x;
		world->static_objects[selected_objects[i]].y += p.y;
		world->static_objects[selected_objects[i]].z += p.z;
	}
} // end function MoveSelection()

void EditefySelection::DuplicateSelection() {
	for(int i=0; i<selected_objects.size(); ++i)
		world->AddStaticObject(world->static_objects[selected_objects[i]].model_indx);
} // end function DuplicateSelection()

// Rotate the selection
void EditefySelection::RotateXYZ(float XR, float YR, float ZR) {
	for(int i=0; i<selected_objects.size(); ++i)  {
		world->static_objects[selected_objects[i]].xr += XR;
		world->static_objects[selected_objects[i]].yr += YR;
		world->static_objects[selected_objects[i]].zr += ZR;
	}
} // end function RotateXYZ()
