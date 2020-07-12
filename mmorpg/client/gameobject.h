///////////////////////////////////////////////////////////////////////////////
// Filename: gameobject.h 
///////////////////////////////////////////////////////////////////////////////
#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include <iostream>
#include "openglobject.h"
#include "cameraobject.h"
#include "frustumobject.h"
#include "lightobject.h"
#include "terrainobject.h"
#include "quadtreeobject.h"
#include "staticmodelobject.h"
#include "userinterfaceobject.h"
#include "fpsobject.h"
#include "playerobject.h"
#include "networkobject.h"
#include "otherplayerobject.h"
#include "mainglobals.h"


///////////////////////
// CLASS PRE-DEFINES //
///////////////////////
class NetworkObject;

extern void idle(void);
extern void display(void);
extern void getKeyUp(unsigned char, int, int);
extern void getKeyDown(unsigned char, int, int);
extern void mouseMotionFunc(int, int);
extern void mouseKeyFunc(int, int, int, int);

const int NETWORK_WAIT_TIME = 100;

//////////////////////////////////////////////////////////////////////////////
// Class name: GameObject
///////////////////////////////////////////////////////////////////////////////
class GameObject
{
public:
	GameObject();   // Constructor.
	~GameObject();  // Destructor.

	bool Initialize();  // Game initialization function.
	bool Shutdown();    // Game shutdown function.
	void Frame();       // Game per-frame processing function.
	int  Run();         // What do you think it does?
	void InitGlut();

	// Functions for the network to affect the client game state.
	void UpdateMovement(short, short, float, float, float, float);
	void SetOtherPlayerActiveStatus(short, short);
	void NewNetworkTextMessage(char*);

	void ProcessKeyDown(unsigned char, int, int);
	void ProcessKeyUp(unsigned char, int, int);
	void ProcessMouseMotion(int, int);
	void ProcessMouseKey(int, int, int, int);
	void ProcessGraphics();
	bool ProcessNetwork();

private:
	int currentWindow;
	int lastTime;
	int networkLastTime;

	OpenGLObject* OpenGL;                // The OpenGL graphics object.
	CameraObject* Camera;                // The camera object.
	FrustumObject* Frustum;              // The frustum for culling.
	LightObject* Light;                  // A light for the scene.
	TerrainObject* Terrain;              // The terrain data.
	QuadTreeObject* QuadTree;            // The quad tree for the terrain.
	StaticModelObject* Model1;           // A 3D model.
	UserInterfaceObject* UserInterface;  // The user interface.
	FpsObject* Fps;                      // The frame per second counter.
	PlayerObject* Player;                // The player object.
	OtherPlayerObject* OtherPlayers;     // The other player objects.
	NetworkObject* Network;              // The network object.
};

#endif
