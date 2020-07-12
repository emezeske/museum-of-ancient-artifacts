#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>

#include "World.h"
#include "../shared/utils.h"
#include "Player.h"
#include "selection.h"

#define ORTHO_TOP	10
#define ORTHO_SIDE	11
#define ORTHO_FRONT	12

#define		CTRL_IDLE			100

// ORTHO control states
#define		CTRL_MOVING			103
#define		CTRL_SCALING			104
#define		CTRL_PANNING			105
#define		CTRL_ROTATING			106

// Mouse states
#define		MS_NORMAL			54
#define		MS_EPLANE			55

// PERSPECTIVE control states
#define CTRL_MOVING_CAMERA	69

#define EDIT_PLANE_ADJUST_SENSITIVITY 15 

extern int halfHeight;
extern EditefySelection  *selection;
extern bool flippedNormal;
extern int EditorState, EditingMode, ControlMode;

class EditefyDisplay {
	public:
	EditefyWorld *world;
	int ctrlState, lastCtrlState, mouseState;
	int displayOffX, displayOffY;
	int displayWidth, displayHeight;
	static bool leftButton, middleButton, rightButton;
	
	void SetViewport();
	void SetDisplayPosition(int, int);
	void SetDisplaySize(int, int);

	virtual void Draw()=0;			// draws the display
	virtual void KeyDown(int)=0;
	virtual void KeyUp(int)=0;
	virtual void Mouse(int, int, int, int)=0;
	virtual void MouseMove(int, int)=0;
	virtual void Reset()=0;
	EditefyDisplay(EditefyWorld *);
}; // end class EditefyDisplay

class EditefyDisplayPerspective:public EditefyDisplay {
	public:
	int lx, ly;
	double sensitivity;			// mouse sensitivity
	editefyPlayer *camera;
	unsigned short SelectObject(int, int);
	void clampCamera();
	void PrepareToDraw();
	void Draw();
	void KeyDown(int);
	void KeyUp(int);
	void Mouse(int, int, int, int);
	void MouseMove(int, int);
	void Reset();
	EditefyDisplayPerspective(EditefyWorld *, double, double, double, int, int, int, int);
	~EditefyDisplayPerspective();
}; // end class EditefyDisplayPerspective

class EditefyDisplayOrtho:public EditefyDisplay {
	public:
	int OrthoView;
	double zoom;
	double offX, offY;
	int lx, ly;
	double GridSize;
	double left, right, top, bottom;			// the extent of the display in world space
	static bool SnapToGrid;
	static double editPlaneX, editPlaneY, editPlaneZ;
	int adjustingEditingPlane;
	int editPlaneDeltaAccum;			// amount cy has changed since last movement of edit plane
	Vector3 tmpPts[4];
	void updateExtents();
	void DrawGrid();
	Vector3 GridSnap(Vector3);
	double GridSnap(double);
	void Draw();
	void DrawEditPlanes();
	void KeyDown(int);
	void KeyUp(int);
	void Mouse(int, int, int, int);
	void MouseMove(int, int);
	Vector3 MouseToWorld(int, int);
	void MouseRotating(int);
	void MouseStartMoving(int x, int y);
	void MouseStartPanning(int x, int y);
	void MouseStartAdding(int x, int y);
	void MouseMoving(int x, int y);
	void MouseScaling(int x);
	void MousePanning(int x, int y);
	void MouseAdding(int x, int y);
	void Reset();
	EditefyDisplayOrtho(EditefyWorld *, int, int, int);
}; // end class EditefyDisplayOrtho
