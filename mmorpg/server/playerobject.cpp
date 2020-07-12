////////////////////////////////////////////////////////////////////////////////
// Filename: playerobject.cpp
////////////////////////////////////////////////////////////////////////////////
#include "playerobject.h"


////////////////////////////////////////////////////////////////////////////////
// Function name: PlayerObject
// Purpose: Constructor.
// Inputs: None.
// Outputs: None.
// Details: Initializes the player state.
////////////////////////////////////////////////////////////////////////////////
PlayerObject::PlayerObject()
{
  active = false;  // Not active.
  posX = 10.0f;
  posY = 10.0f;
  posZ = 10.0f;
  rot = 180.0f;
  movementState = 0;  // Not moving.
}


////////////////////////////////////////////////////////////////////////////////
// Function name: PlayerObject
// Purpose: Copy constructor.
// Inputs: other - The player we are copying into this object.
// Outputs: None.
// Details: None.
////////////////////////////////////////////////////////////////////////////////
PlayerObject::PlayerObject(const PlayerObject& other)
{
}


////////////////////////////////////////////////////////////////////////////////
// Function name: ~PlayerObject
// Purpose: Destructor.
// Inputs: None.
// Outputs: None.
// Details: None.
////////////////////////////////////////////////////////////////////////////////
PlayerObject::~PlayerObject()
{
}


////////////////////////////////////////////////////////////////////////////////
// Function name: IsActive
// Purpose: Indicates if the player is active or not to the caller.
// Inputs: None.
// Outputs: active - Boolean variable indicating if player is active or not.
// Details: None.
////////////////////////////////////////////////////////////////////////////////
bool PlayerObject::IsActive()
{
  return active;
}


////////////////////////////////////////////////////////////////////////////////
// Function name: UpdateMovement
// Purpose: Updates the player's movement state.
// Inputs: movement - The state of movement.
//         x - X coordinate of the player.
//         y - Y coordinate of the player.
//         z - Z coordinate of the player.
//         r - The rotation of the player.
// Outputs: None.
// Details: None.
////////////////////////////////////////////////////////////////////////////////
void PlayerObject::UpdateMovement(short movement, float x, float y, float z, float r)
{
  movementState = movement;
  posX = x;
  posY = y;
  posZ = z;
  rot  = r;
  return;
}


////////////////////////////////////////////////////////////////////////////////
// Function name: SetActive
// Purpose: Sets the player to active.
// Inputs: None.
// Outputs: None.
// Details: Also initializes the position and state of the player.
////////////////////////////////////////////////////////////////////////////////
void PlayerObject::SetActive()
{
  active = true;
  posX = 10.0f;
  posY = 10.0f;
  posZ = 10.0f;
  rot = 180.0f;
  movementState = 0;
  return;
}


////////////////////////////////////////////////////////////////////////////////
// Function name: SetInactive
// Purpose: Sets the player to inactive.
// Inputs: None.
// Outputs: None.
// Details: None.
////////////////////////////////////////////////////////////////////////////////
void PlayerObject::SetInactive()
{
  active = false;
  return;
}


////////////////////////////////////////////////////////////////////////////////
// Function name: GetMovement
// Purpose: Gives the caller the position and movement state of the player.
// Inputs: None.
// Outputs: movement - State of player movement.
//          x - The x coordinate of the player.
//          y - The y coordinate of the player.
//          z - The z coordinate of the player.
// Details: None.
////////////////////////////////////////////////////////////////////////////////
void PlayerObject::GetMovement(short& movement, float& x, float& y, float& z, float& r)
{
  x = posX;
  y = posY;
  z = posZ;
  z = rot;
  movement = movementState;
  return;
}
