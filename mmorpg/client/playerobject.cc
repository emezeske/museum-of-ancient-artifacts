///////////////////////////////////////////////////////////////////////////////
// Filename: playerobject.cpp
///////////////////////////////////////////////////////////////////////////////
#include "playerobject.h"


///////////////////////////////////////////////////////////////////////////////
// Function name: PlayerObject
// Purpose: Constructor.
// Inputs: None.
// Outputs: None.
// Details: Initializes the position and state of the player.
///////////////////////////////////////////////////////////////////////////////
PlayerObject::PlayerObject()
{
	posX = 10.0f;
	posY = 10.0f;
	posZ = 10.0f;
	yRot = 180.0f;
	movingForward  = false;
	movingBackward = false;
	turningLeft    = false;
	turningRight   = false;
	stateChange = false;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: PlayerObject
// Purpose: Copy constructor.
// Inputs: other - The object this function is copying.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
PlayerObject::PlayerObject(const PlayerObject& other)
{
}


///////////////////////////////////////////////////////////////////////////////
// Function name: ~PlayerObject
// Purpose: Destructor.
// Inputs: None.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
PlayerObject::~PlayerObject()
{
}

///////////////////////////////////////////////////////////////////////////////
// Function name: Frame
// Purpose: Updates the player's position and state.
// Inputs:  Delta - elapsed time since last call of this func.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void PlayerObject::Frame(int delta)
{
	if(movingForward) {
		posX -= sinf(yRot * 0.0174532925f) * PLAYER_MOVEMENT_RATE * delta;
		posZ -= cosf(yRot * 0.0174532925f) * PLAYER_MOVEMENT_RATE * delta;
		SetNewPlayerState(1, true);
	} else	if(movingBackward)
	{
		posX += sinf(yRot * 0.0174532925f) * PLAYER_MOVEMENT_RATE * delta;
		posZ += cosf(yRot * 0.0174532925f) * PLAYER_MOVEMENT_RATE * delta;
		SetNewPlayerState(2, true);
	}

	if(turningLeft)
	{
		yRot += PLAYER_ROTATION_RATE * delta;
		if(yRot > 360.0f)
			yRot -= 360.0f;
		SetNewPlayerState(4, true);
	} else if(turningRight)
	{
		yRot -= PLAYER_ROTATION_RATE * delta;
		if(yRot < 0.0f)
			yRot += 360.0f;
		SetNewPlayerState(3, true);
	}

} // end Frame()

///////////////////////////////////////////////////////////////////////////////
// Function name: MoveForward
// Purpose: Moves the player forward.
// Inputs: keydown - If the player is pressing the forward key.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void PlayerObject::MoveForward(bool keydown)
{

	// Set the proper movement state for the player.
	if((movingForward == true) && (keydown == false))
	{
		movingForward = false;
		SetNewPlayerState(1, false);
	}
	if((movingForward == false) && (keydown == true))
	{
		movingForward = true;
		SetNewPlayerState(1, true);
	}

	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: MoveBackward
// Purpose: Moves the player backward.
// Inputs: keydown - If the player is pressing the backward key.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void PlayerObject::MoveBackward(bool keydown)
{

	// Set the proper movement state for the player.
	if((movingBackward == true) && (keydown == false))
	{
		movingBackward = false;
		SetNewPlayerState(2, false);
	}
	if((movingBackward == false) && (keydown == true))
	{
		movingBackward = true;
		SetNewPlayerState(2, true);
	}

	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: TurnLeft
// Purpose: Turns the player left.
// Inputs: keydown - If the player is pressing the left key.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void PlayerObject::TurnLeft(bool keydown)
{
	// Update their state.
	if((turningLeft == true) && (keydown == false))
	{
		turningLeft = false;
		SetNewPlayerState(4, false);
	}
	if((turningLeft == false) && (keydown == true))
	{
		turningLeft = true;
		SetNewPlayerState(4, true);
	}

	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: TurnRight
// Purpose: Turns the player right.
// Inputs: keydown - If the player is pressing the right key.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void PlayerObject::TurnRight(bool keydown)
{
	// Update the players state.
	if((turningRight == true) && (keydown == false))
	{
		turningRight = false;
		SetNewPlayerState(3, false);
	}
	if((turningRight == false) && (keydown == true))
	{
		turningRight = true;
		SetNewPlayerState(3, true);
	}

	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: SetNewPlayerState
// Purpose: To indicate how the player is moving.
// Inputs: type - The type of movement the player is doing.
//         move - To start or stop the movement.
// Outputs: None.
// Details: type - 1: Move Forward
//                 2: Move Backward
//                 3: Turn Right
//                 4: Turn Left
//          move - true:  Start moving.
//                 false: Stop moving.
///////////////////////////////////////////////////////////////////////////////
void PlayerObject::SetNewPlayerState(int type, bool move)
{
	stateChange = true;
	state1 = type;
	state2 = move;
	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: GetPlayerState
// Purpose: Gives the caller the players state.
// Inputs: None.
// Outputs: type - What they are doing.
//          move - If they are doing it or if they have stopped.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
bool PlayerObject::GetPlayerState(int& type, bool& move)
{
	if(!stateChange)
	{
		return false;
	}

	stateChange = false;

	type = state1;
	move = state2;

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: GetPosition
// Purpose: Gives the caller the position of the player.
// Inputs: None.
// Outputs: x - The x position of the player.
//          y - The y position of the player.
//          z - The z position of the player.
//          r - The rotation of the player.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void PlayerObject::GetPosition(float& x, float& y, float& z, float& r)
{
	x = posX;
	y = posY;
	z = posZ;
	r = yRot;
	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: SetPosition
// Purpose: Sets the position of the player.
// Inputs: x - The x position of the player.
//         y - The y position of the player.
//         z - The z position of the player.
//         r - The rotation of the player.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void PlayerObject::SetPosition(float x, float y, float z, float r)
{
	posX = x;
	posY = y;
	posZ = z;
	yRot = r;
	return;
}

