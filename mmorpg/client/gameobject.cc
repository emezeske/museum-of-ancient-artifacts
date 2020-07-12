 ///////////////////////////////////////////////////////////////////////////////
// Filename: gameobject.cpp
///////////////////////////////////////////////////////////////////////////////
#include "gameobject.h"


///////////////////////////////////////////////////////////////////////////////
// Function name: GameObject
// Purpose: Constructor.
// Inputs: None.
// Outputs: None.
// Details: Sets the pointers for the object to null.
///////////////////////////////////////////////////////////////////////////////
GameObject::GameObject()
{	
	OpenGL        = NULL;
	Camera        = NULL;
	Frustum       = NULL;
	Light         = NULL;
	Terrain       = NULL;
	QuadTree      = NULL;
	Model1        = NULL;
	UserInterface = NULL;
	Fps           = NULL;
	Player        = NULL;
	OtherPlayers  = NULL;
	Network       = NULL;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: ~GameObject
// Purpose: Destructor.
// Inputs: None.
// Outputs: None.
// Details: Releases memory if it hasn't been done so already.
///////////////////////////////////////////////////////////////////////////////
GameObject::~GameObject()
{
	if(OpenGL)
	{
		delete OpenGL;
		OpenGL = NULL;
	}
	if(Camera)
	{
		delete Camera;
		Camera = NULL;
	}
	if(Frustum)
	{
		delete Frustum;
		Frustum = NULL;
	}
	if(Light)
	{
		delete Light;
		Light = NULL;
	}
	if(Terrain)
	{
		delete Terrain;
		Terrain = NULL;
	}
	if(QuadTree)
	{
		delete QuadTree;
		QuadTree = NULL;
	}
	if(Model1)
	{
		delete Model1;
		Model1 = NULL;
	}
	if(UserInterface)
	{
		delete UserInterface;
		UserInterface = NULL;
	}
	if(Fps)
	{
		delete Fps;
		Fps = NULL;
	}
	if(Player)
	{
		delete Player;
		Player = NULL;
	}
	if(OtherPlayers)
	{
		delete [] OtherPlayers;
		OtherPlayers = NULL;
	}
	if(Network)
	{
		delete Network;
		Network = NULL;
	}
}


///////////////////////////////////////////////////////////////////////////////
// Function name: Initialize
// Purpose: Initializes the entire game.
// Inputs: None.
// Outputs: A boolean value indicating success.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
bool GameObject::Initialize()
{
	bool result;
	
	// Initialize Glut
	InitGlut();

	// Initialize OpenGL.
	OpenGL = new OpenGLObject;
	result = OpenGL->Initialize(SCREEN_SIZE_X, SCREEN_SIZE_Y, 
				    SCREEN_DEPTH , SCREEN_NEAR);
	if(!result)
	{
		std::cerr << "Error Initializing OpenGL\n";
		return false;
	}

	// Initialize the camera.
	Camera = new CameraObject;

	// Initialize the Frustum.
	Frustum = new FrustumObject;

	// Initialize the light.
	Light = new LightObject;
	Light->Initialize(0);
	Light->SetLightType(LIGHT_TYPE_DIRECTIONAL);
	Light->SetPosition(0.0f, 1.0f, 0.0f);
	Light->SetDiffuse(1.0f, 1.0f, 1.0f);
	Light->SetAmbient(1.0f, 1.0f, 1.0f);
	Light->SetSpecular(0.0f, 0.0f, 0.0f);
	Light->SetGlobalAmbient(0.3f, 0.3f, 0.3f);
	Light->TurnOn();

	// Initialize the terrain.
	Terrain = new TerrainObject;
	result = Terrain->Initialize("data/ter03.ter");
	if(!result)
	{
		std::cerr << "Error Initializing TerrainObject\n";
		return false;
	}

	// Initialize the quad tree.
	QuadTree = new QuadTreeObject;
	result = QuadTree->Initialize(Terrain);
	if(!result)
	{
		std::cerr << "Error Initializing QuadTreeObject\n";
		return false;
	}

	// Initialize the 3D models.
	Model1 = new StaticModelObject;
	result = Model1->Initialize("data/gatehouse/gatehouse.mdl");
	if(!result)
	{
		std::cerr << "Error Initializing StaticModelObject\n";
		return false;
	}
	Model1->SetPosition(50.0f, 0.5f, 50.0f);

	// Initialize the user interface.
	UserInterface = new UserInterfaceObject;
	result = UserInterface->Initialize();
	if(!result)
	{
		std::cerr << "Error Initializing UserInterfaceObject\n";
		return false;
	}

	// Initialize the FPS counter.
	Fps = new FpsObject;

	// Initialize the player.
	Player = new PlayerObject;

	// Initialize the other players.
	OtherPlayers = new OtherPlayerObject[MAX_OTHER_PLAYERS];

	// Initialize the network.
	if(NETWORK_ONLINE)
	{
		Network = new NetworkObject;
		result = Network->Initialize(this);
		if(!result)
		{
			return false;
		}
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: Shutdown
// Purpose: Shuts down the client completely.
// Inputs: None.
// Outputs: A boolean value indicating success.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
bool GameObject::Shutdown()
{
	bool result;
	bool retValue;


	retValue = true;

	// Shutdown OpenGL.
	if(OpenGL)
	{
		result = OpenGL->Shutdown();
		if(!result)
		{
			std::cerr << "Error shutting down OpenGL\n";
			retValue = false;
		}
		delete OpenGL;
		OpenGL = NULL;
	}

	// Shutdown the camera.
	if(Camera)
	{
		delete Camera;
		Camera = NULL;
	}

	// Shutdown the frustum.
	if(Frustum)
	{
		delete Frustum;
		Frustum = NULL;
	}

	// Shutdown the light.
	if(Light)
	{
		Light->Shutdown();
		delete Light;
		Light = NULL;
	}

	// Shutdown the terrain.
	if(Terrain)
	{
		result = Terrain->Shutdown();
		if(!result)
		{
			std::cerr << "Error shutting down TerrainObject\n";
			retValue = false;
		}
		delete Terrain;
		Terrain = NULL;
	}

	// Shutdown the quad tree.
	if(QuadTree)
	{
		result = QuadTree->Shutdown();
		if(!result)
		{
			std::cerr << "Error shutting down QuadTreeObject\n";
			retValue = false;
		}
		delete QuadTree;
		QuadTree = NULL;
	}

	// Shutdown the models.
	if(Model1)
	{
		result = Model1->Shutdown();
		if(!result)
		{
			std::cerr << "Error shutting down StaticModelObject\n";
			retValue = false;
		}
		delete Model1;
		Model1 = NULL;
	}

	// Shutdown the user interface.
	if(UserInterface)
	{
		result = UserInterface->Shutdown();
		if(!result)
		{
			std::cerr << "Error shutting down UserInterfaceObject\n";
			retValue = false;
		}
		delete UserInterface;
		UserInterface = NULL;
	}

	// Kill the fps counter.
	if(Fps)
	{
		delete Fps;
		Fps = NULL;
	}

	// Destroy the player object.
	if(Player)
	{
		delete Player;
		Player = NULL;
	}

	// Destroy the other players.
	if(OtherPlayers)
	{
		delete [] OtherPlayers;
		OtherPlayers = NULL;
	}

	// Shutdown the network.
	if(NETWORK_ONLINE)
	{
		if(Network)
		{
			result = Network->Shutdown();
			if(!result)
			{
				retValue = false;
			}
			delete Network;
			Network = NULL;
		}
	}

	return retValue;
}
/////////////////////////////////////////////////////////////////////////////
//Function name: Run
//Purpose: enter glut's main loop.  (self explanitory) 
//blah 
//blah
//blah
/////////////////////////////////////////////////////////////////////////////

int GameObject::Run()
{
	glutMainLoop();

	return 0;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: Frame
// Purpose: This main loop the game runs in.
// Inputs: None.
// Outputs: A boolean value indicating success.
// Details: This frame is called over and over and calls all the processing.
///////////////////////////////////////////////////////////////////////////////
void GameObject::Frame()
{
	// Find out how much time has elapsed since the 
	// last time this function was called so we can 
	// pass it to other frame funcs for real time stuff
	int time = glutGet(GLUT_ELAPSED_TIME);
	int delta = time - lastTime;
	lastTime = time;

	// Update the Player
	Player->Frame(delta);

	// Check on the network
	ProcessNetwork();

	// See if the fpsobject thinks it's time to
	// display a frame, and if so, do it
	if(Fps->Frame())
		glutPostWindowRedisplay(currentWindow);

}

///////////////////////////////////////////////////////////////////////////////
// Function name: ProcessKeyDown
// Purpose: Handles the user input for the game.
// Inputs: None.
// Outputs: A boolean value indicating success.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void GameObject::ProcessKeyDown(unsigned char key, int x, int y)
{

	switch(key) {
		case 27 :
			Shutdown();			
			exit(0);
		case 'w':
			Player->MoveForward(true);
			break;
		case 's':
			// Check if the backward key has been pressed.
			Player->MoveBackward(true);
			break;
		case 'a':
			// Check if the left turn key has been pressed.
			Player->TurnLeft(true);
			break;
		case 'd':
			// Check if the right turn key has been pressed.
			Player->TurnRight(true);
			break;
		case '1':
			// If 1 pressed the turn on/off the debug info.
			UserInterface->ChangeDebugRender();
			break;
		case '2':
			// If 2 pressed change the render mode.
			OpenGL->ChangeRenderMode();
			break;
		default:
			// Otherwise send the key to the user interface.
			UserInterface->KeyPressed( (int)key );
			break;
	} // end switch

} // end ProcessKeyDown()


///////////////////////////////////////////////////////////////////////////////
// Function name: ProcessKeyUp
// Purpose: Handles the key up event.
// Inputs:  The key that was pressed, and the mouse's x and y coords.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void GameObject::ProcessKeyUp(unsigned char key, int x, int y)
{
	switch(key) {
		case 'w':
			Player->MoveForward(false);
			break;
		case 's':
			Player->MoveBackward(false);
			break;
		case 'a':
			Player->TurnLeft(false);
			break;
		case 'd':
			Player->TurnRight(false);
			break;
	} // end switch

} // end ProcessKeyUp()

///////////////////////////////////////////////////////////////////////////////
// Function name: ProcessMouseMotion
// Purpose: Handles glut mouse movement events.
// Inputs: Mouse x and y.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void GameObject::ProcessMouseKey(int button, int state, int x, int y) {
	if(state == GLUT_DOWN)
	{
		if(button == GLUT_LEFT_BUTTON)
		{
			UserInterface->SetMousePos(x, y);
			UserInterface->SetMouseButton(1);
		}
	}else if(state == GLUT_UP)
	{
		if(button == GLUT_LEFT_BUTTON)
		{
			UserInterface->SetMousePos(x, y);
			UserInterface->SetMouseButton(0);
		}	
	}

}

///////////////////////////////////////////////////////////////////////////////
// Function name: ProcessMouseMotion
// Purpose: Handles glut mouse movement events.
// Inputs: Mouse x and y.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void GameObject::ProcessMouseMotion(int x, int y) {
	UserInterface->SetMousePos(x, y);
} // end ProcessMouseMotion()

///////////////////////////////////////////////////////////////////////////////
// Function name: ProcessNetwork
// Purpose: Handles network communication for the client.
// Inputs: None.
// Outputs: A boolean value indicating success.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
bool GameObject::ProcessNetwork()
{
	bool result, move;
	int type;
	float x, y, z, r;
	char message[80];

	// If we are debugging and the network is not online then leave the 
	// function.
	if(!NETWORK_ONLINE)
	{
		return true;
	}

	// Check if the player has changed their movement state.
	// If they have then send it to the server.
	if(Player->GetPlayerState(type, move))
	{
		if(glutGet(GLUT_ELAPSED_TIME) >= networkLastTime + NETWORK_WAIT_TIME)
		{
			Player->GetPosition(x, y, z, r);
			result = Network->SendMovementChange(type, move, x, y, z, r);
			networkLastTime = glutGet(GLUT_ELAPSED_TIME);
		}
	}

	// Check if there is a new text message to send to the server.
	if(UserInterface->NewMessage())
	{
		// If so then get the message and send it to the server.
		UserInterface->GetNewMessage(message);
		result = Network->SendTextMessage(message);
	}

	// Do any required network tasks like sending pings and such.
	if(!Network->Frame())
	{
		return false;
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: ProcessGraphics
// Purpose: Handles all the graphics needed for the client.
// Inputs: None.
// Outputs: A boolean value indicating success.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void GameObject::ProcessGraphics()
{
	// init variables 
	int totalPolys, drawnPolys, fps, ping, renderMode;
	float x, y, z, r, height;
	bool result;
	float xPos, yPos, zPos, size;
	int i;
	float tempX, tempY, tempZ, tempR, tempHeight;


	// Initialize the poly counts.
	totalPolys = 0;
	drawnPolys = 0;

	// Get the frames per second speed.
	fps = Fps->GetFps();
	if(fps > 9999)
	{
		fps = 9999;
	}

	// Get the position of the player.
	Player->GetPosition(x, y, z, r);

	// Get the height of the terrain at the position of the player.
	result = QuadTree->GetHeightAtPosition(x, z, height);
	if(result) 
	{
		// Move the height up to the eyes of the player.
		y = height + 1.0f;
	}

	// Set the position of the player according to the height of the terrain.
	Player->SetPosition(x, y, z, r);

	// Set the position of the camera.
	Camera->SetPosition(x, y, z, r);

	// Get the network latency.
	ping = 0;
	if(NETWORK_ONLINE)
	{
		ping = Network->GetPing();
	}
	if(ping > 9999)
	{
		ping = 9999;
	}

	// Get the render mode from the OpenGL object.
	renderMode = OpenGL->GetRenderMode();

	// Begin the graphics scene.
	OpenGL->BeginScene();

	// Translate to the camera position.
	Camera->Render();

	// Build the frustum for culling objects.
	Frustum->ConstructFrustum();

	// Render the terrain.
	QuadTree->Render(Frustum);
	totalPolys += (int)Terrain->GetNumFaces();
	drawnPolys += (int)QuadTree->GetPolysDrawn();

	// Render the models.
	Model1->GetBoundingVolume(xPos, yPos, zPos, size);
	if(Frustum->CheckCube(xPos, yPos, zPos, size))
	{
		Model1->Render();
		drawnPolys += Model1->GetPolyCount();
	}
	totalPolys += Model1->GetPolyCount();

	// Render the other players.
	for(i=1; i<MAX_OTHER_PLAYERS; i++)
	{
		if(OtherPlayers[i].IsActive())
		{
			// Get the position of the other player.
			OtherPlayers[i].GetPosition(tempX, tempY, tempZ, tempR);

			// Get the height of the terrain at the position of the player.
			result = QuadTree->GetHeightAtPosition(tempX, tempZ, tempHeight);
			if(result) 
			{
				tempY = tempHeight + 0.85f;
			}

			// Set the position of the player according to the height of the terrain.
			OtherPlayers[i].SetPosition(tempX, tempY, tempZ, tempR);

			OtherPlayers[i].Render();
		}
	}

	// Render the user interface.
	UserInterface->Render(OpenGL, totalPolys, drawnPolys, fps, 
		              x, y, z, r, ping, renderMode);

	// End the graphics scene.
	OpenGL->EndScene();
}


///////////////////////////////////////////////////////////////////////////////
// Function name: UpdateMovement
// Purpose: The network uses this function to update the movement of other 
//          players in the zone on the clients machine.
// Inputs: id - ID number of the player.
//         movement - The movement state of the player.
//         posX - The x position of the player.
//         posY - The y position of the player.
//         posZ - The z position of the player.
//         posR - The rotation of the player.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void GameObject::UpdateMovement(short id, short movement, float posX, float posY, 
								float posZ, float posR)
{
	if((id > 0) && (id < MAX_OTHER_PLAYERS))
	{
		// If the player id is not active then the player is new to the zone so
		// add them as active.
		if(OtherPlayers[id].IsActive() == false)
		{
			OtherPlayers[id].SetActive(true);
		}

		// Update their position.
		OtherPlayers[id].UpdateMovement(movement, posX, posY, posZ, posR);
	}

	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: SetOtherPlayerActiveStatus
// Purpose: The network uses this function to set a player active or inactive
//          such as if they have entered or left the zone on the server.
// Inputs: id - The ID number of the player.
//         active - If they are active or not.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void GameObject::SetOtherPlayerActiveStatus(short id, short active)
{
	if((id > 0) && (id < MAX_OTHER_PLAYERS))
	{
		// If the player has left the zone then set them to inactive.
		if(active == 0)
		{
			OtherPlayers[id].SetActive(false);
		}
	
		// If the player has entered the zone then set them to active.
		if(active == 1)
		{
			OtherPlayers[id].SetActive(true);
		}
	}

	return;
}

///////////////////////////////////////////////////////////////////////////////
//blah
//blah
//blah
///////////////////////////////////////////////////////////////////////////////
void GameObject::InitGlut(void) {

	glutIgnoreKeyRepeat(1);
	glutSetCursor(GLUT_CURSOR_NONE);
	
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(getKeyDown);
	glutKeyboardUpFunc(getKeyUp);
	glutMotionFunc(mouseMotionFunc);
	glutPassiveMotionFunc(mouseMotionFunc);
	glutMouseFunc(mouseKeyFunc);

	currentWindow = glutGetWindow();

} // end initGlut()

///////////////////////////////////////////////////////////////////////////////
// Function name: NewNetworkTextMessage
// Purpose: The network uses this function to notify the client of a new text
//          message sent from the server.
// Inputs: text - The new text message.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void GameObject::NewNetworkTextMessage(char* text)
{
	UserInterface->AddMessageToTalkbar(text);
	return;
}
