///////////////////////////////////////////////////////////////////////////////
// Filename: otherplayerobject.cpp
///////////////////////////////////////////////////////////////////////////////
#include "otherplayerobject.h"


///////////////////////////////////////////////////////////////////////////////
// Function name: OtherPlayerObject
// Purpose: Constructor.
// Inputs: None.
// Outputs: None.
// Details: Initializes the position and state of the player.
///////////////////////////////////////////////////////////////////////////////
OtherPlayerObject::OtherPlayerObject()
{
	active = false;
	posX = 10.0f;
	posY = 10.0f;
	posZ = 10.0f;
	posR = 180.0f;
	movement = 0;

	Texture = new TextureObject;
	Texture->Load("data/temp.tga");
}


///////////////////////////////////////////////////////////////////////////////
// Function name: OtherPlayerObject
// Purpose: Copy constructor.
// Inputs: other - The object this function is copying.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
OtherPlayerObject::OtherPlayerObject(const OtherPlayerObject& other)
{
}


///////////////////////////////////////////////////////////////////////////////
// Function name: ~OtherPlayerObject
// Purpose: Destructor.
// Inputs: None.
// Outputs: None.
// Details: Releases memory associated with this object.
///////////////////////////////////////////////////////////////////////////////
OtherPlayerObject::~OtherPlayerObject()
{
	if(Texture)
	{
		Texture->Free();
		delete Texture;
		Texture = NULL;
	}
}


///////////////////////////////////////////////////////////////////////////////
// Function name: SetActive
// Purpose: Sets the player active or inactive.
// Inputs: activeState - Whether the player should be set active or not.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void OtherPlayerObject::SetActive(bool activeState)
{
	active = activeState;
	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: IsActive
// Purpose: Lets the calling function know if this player is active or not.
// Inputs: None.
// Outputs: active - If the player is active or not.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
bool OtherPlayerObject::IsActive()
{
	return active;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: Render
// Purpose: Draws the player in the scene.
// Inputs: None.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void OtherPlayerObject::Render()
{
	// Update the position of the player, dead-reckoning basically.
	Update();

	// Render a cube to represent the player.
	glPushMatrix();
		glTranslatef(posX, posY, posZ);
		glRotatef((360.0f - posR), 0.0f, 1.0f, 0.0f);

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		
		Texture->Bind();

		glScalef(0.5f, 0.5f, 0.5f);

		glBegin(GL_QUADS);
			glNormal3f(0.0f, 0.0f, 1.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
			glNormal3f(0.0f, 0.0f, -1.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
			glNormal3f(0.0f, 1.0f, 0.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
			glNormal3f(0.0f, -1.0f, 0.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
			glNormal3f(1.0f, 0.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
			glNormal3f(-1.0f, 0.0f, 0.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
		glEnd();

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glPopMatrix();

	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: UpdateMovement
// Purpose: Updates the movement state and position of the player.
// Inputs: movementState - What the player is doing in regards to movement.
//         x - The x position of the player.
//         y - The y position of the player.
//         z - The z position of the player.
//         r - The rotation of the player.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void OtherPlayerObject::UpdateMovement(short movementState, float x, float y, 
									   float z, float r)
{
	posX = x;
	posY = y;
	posZ = z;
	posR = r;
	movement = movementState;

	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: Update
// Purpose: Updates the position of the player.
// Inputs: None.
// Outputs: None.
// Details: Since the server sends us only the position and movement state we
//          do our own calculations for where the player should be.  This is
//          called dead-reckoning, meaning prediction based on what we knew the
//          player was doing last.  We use the last movement state we got from
//          them plus their last position and figure out where they should be 
//          now.
///////////////////////////////////////////////////////////////////////////////
void OtherPlayerObject::Update()
{
	// Move forward.
	if(movement == 5)
	{
		posX -= sinf(posR * 0.0174532925f) * 0.3f;
		posZ -= cosf(posR * 0.0174532925f) * 0.3f;
	}
	// Move backward.
	if(movement == 6)
	{
		posX += sinf(posR * 0.0174532925f) * 0.3f;
		posZ += cosf(posR * 0.0174532925f) * 0.3f;
	}
	// Turn right.
	if(movement == 7)
	{
		posR -= 3.0f;
	}
	// Turn left.
	if(movement == 8)
	{
		posR += 3.0f;
	}

	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: GetPosition
// Purpose: Gives the calling function the position of this player.
// Inputs: None.
// Outputs: X - The x position of the player.
//          Y - The y position of the player.
//          Z - The z position of the player.
//          R - The rotation of the player.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void OtherPlayerObject::GetPosition(float& X, float& Y, float& Z, float& R)
{
	X = posX;
	Y = posY;
	Z = posZ;
	R = posR;
	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: SetPosition
// Purpose: Sets the position of the player.
// Inputs: X - The x position of the player.
//         Y - The y position of the player.
//         Z - The z position of the player.
//         R - The rotation of the player.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void OtherPlayerObject::SetPosition(float X, float Y, float Z, float R)
{
	posX = X;
	posY = Y;
	posZ = Z;
	posR = R;
	return;
}
