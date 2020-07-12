///////////////////////////////////////////////////////////////////////////////
// Filename: playerobject.h
///////////////////////////////////////////////////////////////////////////////
#ifndef _PLAYEROBJECT_H_
#define _PLAYEROBJECT_H_


//////////////
// INCLUDES //
//////////////
#include <iostream>
#include <math.h>


///////////////////////////////////////////////////////////////////////////////
// Class name: PlayerObject
///////////////////////////////////////////////////////////////////////////////
class PlayerObject
{
public:
	PlayerObject();
	PlayerObject(const PlayerObject&);
	~PlayerObject();

	void Frame(int);
	void MoveForward(bool);			// sends 1 to SetNewPlayerState()
	void MoveBackward(bool);		// sends 2 to SetNewPlayerState()		
	void TurnLeft(bool);			// sends 4 to SetNewPlayerState()		
	void TurnRight(bool);			// sends 3 to SetNewPlayerState()		

	bool GetPlayerState(int&, bool&);
	void GetPosition(float&, float&, float&, float&);
	void SetPosition(float, float, float, float);

private:
	void SetNewPlayerState(int, bool);

	float posX, posY, posZ, yRot;        // The position and rotation.
	bool movingForward, movingBackward;  // If they are moving forward or back.
	bool turningLeft, turningRight;      // If they are turning left or right.
	bool stateChange;                    // If the player has changed its state.
	int  state1;                         // The state of the player.
	bool state2;                         // If they are doing or stopped state1.

	static const float PLAYER_MOVEMENT_RATE = .010;
	static const float PLAYER_ROTATION_RATE = .07;
};

#endif
