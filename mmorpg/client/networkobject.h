///////////////////////////////////////////////////////////////////////////////
// Filename: networkobject.h
///////////////////////////////////////////////////////////////////////////////
#ifndef _NETWORKOBJECT_H_
#define _NETWORKOBJECT_H_


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "socketobject.h"
#include "gameobject.h"


///////////////////////
// CLASS PRE-DEFINES //
///////////////////////
class SocketObject;
class GameObject;


///////////////////////////////////////////////////////////////////////////////
// Class name: NetworkObject
///////////////////////////////////////////////////////////////////////////////
class NetworkObject
{
public:
	NetworkObject();
	~NetworkObject();
	NetworkObject(const NetworkObject&);

	bool Initialize(GameObject*);
	bool Shutdown();
	bool Frame();

	void GotPing();
	int  GetPing();
	bool SendMovementChange(int, bool, float, float, float, float);
	void HandleOtherPlayerMovement(MSG_GENERIC_DATA*);
	void HandleActiveMessage(MSG_GENERIC_DATA*);
	bool SendTextMessage(char*);
	void HandleTextMessage(MSG_GENERIC_DATA*);

private:
	void ProcessPing();
	bool SendPing();

private:
	SocketObject* Socket;     // Pointer to the socket for communication.
	GameObject* GamePtr;      // Pointer to the main game object for ipc.
	unsigned long pingTime;   // The time to send the next ping.
	unsigned long pingSpeed;  // The current ping speed.
};

#endif
