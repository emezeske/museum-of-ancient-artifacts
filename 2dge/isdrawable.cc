#include <vector>
#include <algorithm>
#include "isdrawable.h"
#include "renderer.h"
using std::vector;
using std::find;

// Add this new drawable object to the list of objects that the renderer
// will be drawing
IsDrawable::IsDrawable(void) {
	Renderer::drawList.push_back(this);
} // end ctor IsDrawable()

// Remove this object from the renderer's draw list
IsDrawable::~IsDrawable(void) {
	vector<IsDrawable*>::iterator me = find(Renderer::drawList.begin(), Renderer::drawList.end(), this);
	if(me != Renderer::drawList.end())
		Renderer::drawList.erase(me);
} // end dtor ~IsDrawable()
