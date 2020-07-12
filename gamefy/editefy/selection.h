#ifndef __SELECTION_H__
#define __SELECTION_H__

#include <vector>

#include "../shared/vector.h"
#include "../shared/matrix.h"
#include "World.h"
#include "Object3D.h"

class EditefySelection {
	std::vector<int> selected_objects;

	public:
	void AddToSelection(unsigned short);
	void RemoveFromSelection(unsigned short);
	bool IsInSelection(unsigned short);
	void ClearSelection();
	void DeleteSelection();
	void DuplicateSelection();
	void MoveSelection(Vector3 &);
	void RotateXYZ(float, float, float);
}; // end class EditefySelection

#endif
