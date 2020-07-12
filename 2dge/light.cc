#include <algorithm>
#include <vector>
#include "light.h"
#include "renderer.h"
using std::vector;
using std::find;

// The light ctor initializes it's values and then adds itself to the
// list of lights that need to be rendered
Light::Light(Vec3D P, float R, float G, float B, float I) :
	       pos(P),    r(R),    g(G),    b(B),    i(I) {
	Renderer::lights.push_back(this);
} // end ctor Light()

// The light dtor finds itself in the lights vector and removes itself
Light::~Light() {
	vector<Light*>::iterator me = find(Renderer::lights.begin(), Renderer::lights.end(), this);
	if(me != Renderer::lights.end())
		Renderer::lights.erase(me);
} // end dtor ~Light()
