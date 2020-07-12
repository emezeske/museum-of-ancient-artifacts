///////////////////////////////////////////////////////////////////////////////
// Filename: userinterfaceobject.cpp
///////////////////////////////////////////////////////////////////////////////
#include "userinterfaceobject.h"


///////////////////////////////////////////////////////////////////////////////
// Function name: UserInterfaceObject
// Purpose: Constructor.
// Inputs: None.
// Outputs: None.
// Details: Initialize the member data.
///////////////////////////////////////////////////////////////////////////////
UserInterfaceObject::UserInterfaceObject()
{
	Font       = NULL;
	Mouse      = NULL;
	UITextures = NULL;

	renderDebug = true;
	talkBar1[0] = '\0';
	talkBar2[0] = '\0';
	talkBar3[0] = '\0';
	talkBar4[0] = '\0';
	talkBar5[0] = '\0';
	talkBarPos1 = 0;
	talkBarPos2 = 0;
	talkBarPos3 = 0;
	talkBarPos4 = 0;
	talkBarPos5 = 0;

	serverMessage[0] = '\0';
	newMessageReady = false;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: UserInterfaceObject
// Purpose: Copy constructor.
// Inputs: other - The object this function is copying.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
UserInterfaceObject::UserInterfaceObject(const UserInterfaceObject& other)
{
}


///////////////////////////////////////////////////////////////////////////////
// Function name: ~UserInterfaceObject
// Purpose: Destructor.
// Inputs: None.
// Outputs: None.
// Details: Releases the memory associated with this object.
///////////////////////////////////////////////////////////////////////////////
UserInterfaceObject::~UserInterfaceObject()
{
	if(Font)
	{
		delete Font;
		Font = NULL;
	}
	if(Mouse)
	{
		delete Mouse;
		Mouse = NULL;
	}
	if(UITextures)
	{
		delete [] UITextures;
		UITextures = NULL;
	}
}


///////////////////////////////////////////////////////////////////////////////
// Function name: Initialize
// Purpose: Initializes the user interface and loads in the data for it.
// Inputs: None.
// Outputs: A boolean value indicating success.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
bool UserInterfaceObject::Initialize()
{
	bool result;


	// Initialize the font.
	Font = new FontObject;
	result = Font->Initialize("data/font01.tga", "data/font01data.txt");
	if(!result)
	{
		std::cerr << "Error in FontObject\n";
		return false;
	}

	// Initialize the mouse object.
	Mouse = new MouseObject;
	result = Mouse->Initialize();
	if(!result)
	{
		std::cerr << "Error in MouseObject\n";
		return false;
	}

	// Initialize the user interface textures.
	UITextures = new TextureObject[3];
	
	result = UITextures[0].Load("data/ui-top.tga");
	if(!result)
	{
		std::cerr << "Error loading ui Textures\n";
		return false;
	}

	result = UITextures[1].Load("data/ui-bottom.tga");
	if(!result)
	{
		std::cerr << "Error loading ui Textures\n";
		return false;
	}

	// Load in the talkbar for chat messages to be drawn on.
	result = UITextures[2].Load("data/ui-talkbar.tga");
	if(!result)
	{
		std::cerr << "Error Loading ui Textures\n";
		return false;
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: Shutdown
// Purpose: Releases the memory associated with the object.
// Inputs: None.
// Outputs: A boolean value indicating success.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
bool UserInterfaceObject::Shutdown()
{
	bool result, returnValue;


	// Initialize the return value.
	returnValue = true;

	// Shutdown the font.
	if(Font)
	{
		result = Font->Shutdown();
		if(!result)
		{
			std::cerr << "Error Shutting Down Font\n";
			returnValue = false;
		}
		delete Font;
		Font = NULL;
	}

	// Shutdown the mouse.
	if(Mouse)
	{
		result = Mouse->Shutdown();
		if(!result)
		{
			std::cerr << "Error Shutting Down Mouse\n";
			returnValue = false;
		}
		delete Mouse;
		Mouse = NULL;
	}

	// Release the UI textures.
	if(UITextures)
	{
		UITextures[0].Free();
		UITextures[1].Free();
		UITextures[2].Free();
		delete [] UITextures;
		UITextures = NULL;
	}

	return returnValue;
}
///////////////////////////////////////////////////////////////////////////
// Function: SetMousePos
//
///////////////////////////////////////////////////////////////////////////
void UserInterfaceObject::SetMousePos(int x, int y) {

	// Set the position of the mouse.
	Mouse->SetPosition(x, y);

}

///////////////////////////////////////////////////////////////////////////
// Function: SetMouseButton
//
//////////////////////////////////////////////////////////////////////////
void UserInterfaceObject::SetMouseButton(bool down) {

	// Set the mouse button state
	Mouse->SetButton(down);
}


///////////////////////////////////////////////////////////////////////////////
// Function name: Render
// Purpose: Draw the 2D user interface.
// Inputs: OpenGL - Pointer to the OpenGL object for drawing.
//         totalPolys - The count of polygons for the scene.
//         drawnPolys - The count of polygons currently drawn.
//         mouseX - The x position of the mouse.
//         mouseY - The x position of the mouse.
//         fps - The frames per second.
//         x - The x position of the player.
//         y - The y position of the player.
//         z - The z position of the player.
//         r - The rotation of the player.
//         ping - The current ping speed.
//         renderMode - The current render mode.
// Outputs: A boolean value indicating success.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
bool UserInterfaceObject::Render(OpenGLObject* OpenGL, int totalPolys, int drawnPolys, 
				 int fps, float x, float y,
				 float z, float r, int ping, int renderMode)
{ 
	string pString1, pString2;
	string mString1, mString2;
	string fpsString, pingString;
	string xString, yString, zString, rString;
	string renderString;

	// Setup the poly count strings.
	pString1 = "Total Polys: ";
	pString1 += itoa(totalPolys);
	
	pString2 = "Drawn Polys: ";
	pString2 += itoa(drawnPolys);

	// Setup the mouse strings.
	mString1 = "Mouse X: ";
	mString1 += itoa(Mouse->getX());

	mString2 = "Mouse Y: ";
	mString2 += itoa(Mouse->getY());
	
	// Setup the fps string.
	fpsString = "FPS: ";
	fpsString += itoa(fps);

	// Setup the positional strings.
	xString = "X: ";	
	xString += itoa((int)x); 

	yString = "Y: ";	
	yString += itoa((int)y); 

	zString = "Z: ";	
	zString += itoa((int)z); 

	rString = "R: ";
	rString += itoa((int)r);

	// Setup the network latency string.
	pingString = "Ping: ";
	pingString += itoa(ping);
	pingString += "ms";

	// Setup the render mode string.
	renderString = "Render mode: ";
	renderString += itoa(renderMode);

	// Make sure UI is always drawn normal, even in debug mode.
	if(OpenGL->InLineMode())
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	// Render the two UI strips.
	glPushAttrib(GL_TEXTURE_BIT | GL_LIGHTING_BIT);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	OpenGL->OrthoMode();

	// Render the top UI texture.
	UITextures[0].Bind();
	glLoadIdentity();
	glTranslated(0, 0, 0);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); 
		glVertex2i(0, 96);
		glTexCoord2f(1.0f, 0.0f); 
		glVertex2i(1024, 96);
		glTexCoord2f(1.0f, 1.0f); 
		glVertex2i(1024, 0);
		glTexCoord2f(0.0f, 1.0f); 
		glVertex2i(0, 0);
	glEnd();

	// Render the bottom UI texture.
	UITextures[1].Bind();
	glLoadIdentity();
	glTranslated(0, 672, 0);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); 
		glVertex2i(0, 96);
		glTexCoord2f(1.0f, 0.0f); 
		glVertex2i(1024, 96);
		glTexCoord2f(1.0f, 1.0f); 
		glVertex2i(1024, 0);
		glTexCoord2f(0.0f, 1.0f); 
		glVertex2i(0, 0);
	glEnd();

	// Render the talkbar UI texture.
	UITextures[2].Bind();
	glLoadIdentity();
	glTranslated(256, 677, 0);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); 
		glVertex2i(0, 96);
		glTexCoord2f(1.0f, 0.0f); 
		glVertex2i(512, 96);
		glTexCoord2f(1.0f, 1.0f); 
		glVertex2i(512, 0);
		glTexCoord2f(0.0f, 1.0f); 
		glVertex2i(0, 0);
	glEnd();

	// Revert back to normal settings now.
	OpenGL->PerspectiveMode();
	glPopAttrib();

	// Render the data strings.
	Font->DrawSentence(OpenGL, fpsString, 5, 5, 0.0f, 1.0f, 0.0f);
	Font->DrawSentence(OpenGL, pingString, 5, 20, 0.0f, 1.0f, 0.0f);

	// Render the talk bar strings.
	Font->DrawSentence(OpenGL, talkBar1, 263, 684, 1.0f, 1.0f, 1.0f);
	Font->DrawSentence(OpenGL, talkBar2, 263, 699, 1.0f, 1.0f, 1.0f);
	Font->DrawSentence(OpenGL, talkBar3, 263, 714, 1.0f, 1.0f, 1.0f);
	Font->DrawSentence(OpenGL, talkBar4, 263, 729, 1.0f, 1.0f, 1.0f);
	Font->DrawSentence(OpenGL, talkBar5, 263, 747, 1.0f, 1.0f, 1.0f);

	// Render these data strings if debug mode is on.
	if(renderDebug)
	{
		Font->DrawSentence(OpenGL, xString, 90, 5, 0.0f, 1.0f, 0.0f);
		Font->DrawSentence(OpenGL, yString, 90, 20, 0.0f, 1.0f, 0.0f);
		Font->DrawSentence(OpenGL, zString, 90, 35, 0.0f, 1.0f, 0.0f);
		Font->DrawSentence(OpenGL, rString, 90, 50, 0.0f, 1.0f, 0.0f);
		Font->DrawSentence(OpenGL, pString1, 160, 5, 0.0f, 1.0f, 0.0f);
		Font->DrawSentence(OpenGL, pString2, 160, 20, 0.0f, 1.0f, 0.0f);
		Font->DrawSentence(OpenGL, renderString, 160, 35, 0.0f, 1.0f, 0.0f);
		Font->DrawSentence(OpenGL, mString1, 160, 50, 0.0f, 1.0f, 0.0f);
		Font->DrawSentence(OpenGL, mString2, 160, 65, 0.0f, 1.0f, 0.0f);
	}

	// Render the mouse icon.
	Mouse->Render(OpenGL);

	// Return the graphics mode to line if in that debug mode.
	if(OpenGL->InLineMode())
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	return true;
}

string UserInterfaceObject::itoa(int tempInt) {

	string tempString;
	string numbers = "0123456789";

	if(tempInt == 0) { tempString += numbers[0]; return tempString; }

	if(tempInt >= 100000)
		tempString += numbers[int(tempInt / 100000 % 10)];
	if(tempInt >= 10000)
		tempString += numbers[int(tempInt / 10000  % 10)];
	if(tempInt >= 1000)
		tempString += numbers[int(tempInt / 1000   % 10)];
	if(tempInt >= 100)
		tempString += numbers[int(tempInt / 100    % 10)];
	if(tempInt >= 10)
		tempString += numbers[int(tempInt / 10     % 10)];
	if(tempInt >= 1)
		tempString += numbers[int(tempInt          % 10)];

	while(tempString[0] == '0')
			tempString.erase(0, 1);

	return tempString;

} // end itoa()

///////////////////////////////////////////////////////////////////////////////
// Function name: ChangeDebugRender
// Purpose: Turns on and off the debug draw mode.
// Inputs: None.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void UserInterfaceObject::ChangeDebugRender()
{
	renderDebug = !renderDebug;
	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: KeyPressed
// Purpose: Lets the UI know if a key has been pressed and what it is.
// Inputs: key - The key the user pressed.
// Outputs: None.
// Details: If the key is text then it is added to the talkbar, if it is a 
//          function key of some sort then its function is performed.
///////////////////////////////////////////////////////////////////////////////
void UserInterfaceObject::KeyPressed(int key)
{
	if(key == 0)
	{
		return;
	}

	// Back space.
	if(key == 8)
	{
		if(talkBarPos5 > 0)
		{
			talkBarPos5--;
			talkBar5[talkBarPos5] = '\0';
		}
		return;
	}

	// Enter key.
	if(key == 13)
	{
		AddMessageToTalkbar(talkBar5);
		AddNewMessageForServer(talkBar5);
		talkBar5[0] = '\0';
		talkBarPos5 = 0;
		return;
	}

	if(talkBarPos5 < 35)
	{
		talkBar5[talkBarPos5] = key;
		talkBarPos5++;
		talkBar5[talkBarPos5] = '\0';
	}
	
	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: AddMessageToTalkbar
// Purpose: Adds a message to the talkbar from the network.
// Inputs: msg - The message to be added.
// Outputs: None.
// Details: The other messages are moved up the talkbar to make room for the 
//          new message.
///////////////////////////////////////////////////////////////////////////////
void UserInterfaceObject::AddMessageToTalkbar(char* msg)
{
	strcpy(talkBar1, talkBar2);
	strcpy(talkBar2, talkBar3);
	strcpy(talkBar3, talkBar4);
	strcpy(talkBar4, msg);
	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: AddNewMessageForServer
// Purpose: Adds a message for the network to transfer to the other players.
// Inputs: message - The message to send.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void UserInterfaceObject::AddNewMessageForServer(char* message)
{
	strcpy(serverMessage, message);
	newMessageReady = true;
	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: NewMessage
// Purpose: Lets the calling function know if a message is ready for the 
//          network.
// Inputs: None.
// Outputs: newMessageReady - Message ready indicator.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
bool UserInterfaceObject::NewMessage()
{
	return newMessageReady;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: GetNewMessage
// Purpose: Gives the calling function the message that is ready to be sent
//          over the network.
// Inputs: None.
// Outputs: outputMessage - The message to be sent.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void UserInterfaceObject::GetNewMessage(char* outputMessage)
{
	strcpy(outputMessage, serverMessage);
	newMessageReady = false;
	return;
}
