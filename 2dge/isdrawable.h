#ifndef __ISDRAWABLE_H__
#define __ISDRAWABLE_H__

// This is the abstract mixin class to provide an interface for
// objects that are drawable.  To use this interface, all a class must
// do is inherit from it publicly and implement the two pure virtual
// functions.  When an object that inherits from IsDrawable is instantiated,
// the IsDrawable ctor takes care of adding it to the display list and the dtor
// removes it.
class IsDrawable {
	public:
		IsDrawable();
		virtual void DrawBumpMap() = 0;
		virtual void Draw() = 0;
		~IsDrawable();
}; // end class IsDrawable
#endif
