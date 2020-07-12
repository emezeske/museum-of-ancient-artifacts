////////////////////////////////////////////////////////////////////////////////
// Filename: playerobject.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _PLAYEROBJECT_H_
#define _PLAYEROBJECT_H_


//////////////
// INCLUDES //
//////////////
#include <iostream.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: PlayerObject
////////////////////////////////////////////////////////////////////////////////
class PlayerObject
{
public:
  PlayerObject();                     // Constructor.
  PlayerObject(const PlayerObject&);  // Copy constructor.
  ~PlayerObject();                    // Destructor.

  bool IsActive();                                           // Indicates if this player is active or not.
  void UpdateMovement(short, float, float, float, float);    // Updates movement of the player.
  void SetActive();                                          // Sets the player to active.
  void SetInactive();                                        // Sets the player to inactive.
  void GetMovement(short&, float&, float&, float&, float&);  // Retrieves players current movement state.

private:
  bool active;                  // If the player is active or not.
  float posX, posY, posZ, rot;  // Position of the player in 3D space.
  short movementState;          // Current state of the player in terms of movement.
};

#endif
