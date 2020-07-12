///////////////////////////////////////////////////////////////////////////////
// Filename: networkobject.cpp
///////////////////////////////////////////////////////////////////////////////
#include "networkobject.h"


///////////////////////////////////////////////////////////////////////////////
// Function name: NetworkObject
// Purpose: Constructor.
// Inputs: None.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
NetworkObject::NetworkObject()
{
	// Set the pointers to NULL.
	Socket = NULL;
	GamePtr = NULL;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: ~NetworkObject
// Purpose: Destructor.
// Inputs: None.
// Outputs: None.
// Details: Releases pointers to memory.
///////////////////////////////////////////////////////////////////////////////
NetworkObject::~NetworkObject()
{
	if(Socket)
	{
		delete Socket;
		Socket = NULL;
	}
	GamePtr = NULL;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: NetworkObject
// Purpose: Copy constructor.
// Inputs: other - The object that is being copied.
// Outputs: other - The object that was copied to.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
NetworkObject::NetworkObject(const NetworkObject& other)
{
}


///////////////////////////////////////////////////////////////////////////////
// Function name: Initialize
// Purpose: Initializes the network communication between the client and the
//          server.
// Inputs:  ptr - Pointer to the main game object for interprocess 
//          communication.
// Outputs: A boolean value indicating success.
// Details:
///////////////////////////////////////////////////////////////////////////////
bool NetworkObject::Initialize(GameObject* ptr)
{
	bool result;

	// Store a copy of the pointer to the game object.
	GamePtr = ptr;

	// Create the new socket.
	Socket = new SocketObject;
	if(!Socket)
	{
		return false;
	}

	// Connect to the server.
	result = Socket->Initialize(this, "192.168.1.1", 7890);
	if(!result)
	{
		return false;
	}

	// Initialize the ping time and ping speed.
	pingTime  = glutGet(GLUT_ELAPSED_TIME);
	pingSpeed = 0;

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: Shutdown
// Purpose: Shuts down the network.
// Inputs: None.
// Outputs: A boolean value indicating success.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
bool NetworkObject::Shutdown()
{
	bool result;

	// If it was not initialized in the first place then quit.
	if(!Socket)
	{
		return false;
	}

	// Shutdown the communication to the server.
	result = Socket->Shutdown();

	// Release the memory.
	delete Socket;
	Socket = NULL;

	// If there was an error then return a false value.
	if(!result)
	{
		return false;
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: Frame
// Purpose: Main function for handling network processes each frame.
// Inputs: None.
// Outputs: A boolean value indicating success.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
bool NetworkObject::Frame()
{
	// See if it is time to send another ping.
	ProcessPing();

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: ProcessPing
// Purpose: Sends a ping message every 5 seconds.
// Inputs: None.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void NetworkObject::ProcessPing()
{
	bool result;

	
	// If 5 seconds is up then send a ping message to the server.
	if(glutGet(GLUT_ELAPSED_TIME) >= (pingTime + 5000))
	{
		pingTime = glutGet(GLUT_ELAPSED_TIME);
		result = SendPing();
	}

	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: SendPing
// Purpose: Sends a Ping message to the server.
// Inputs: None.
// Outputs: A boolean value indicating if the entire message was sent or not.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
bool NetworkObject::SendPing()
{
	MSG_GENERIC_DATA pingMessage;
	int messageSize;
	bool result;


	// Create the ping message.
	pingMessage.type = MSG_PING;
	pingMessage.toId = ID_SERVER;
	pingMessage.fromId = Socket->GetId();

	// Get the size of the message.
	messageSize = sizeof(MSG_GENERIC_DATA);

	// Send the ping message.
	result = Socket->SendMessage((char*)&pingMessage, messageSize);
	if(!result)
	{
		return false;
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: GotPing
// Purpose: Calculates the ping speed when the reply to the ping from the 
//          server is received.
// Inputs: None.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void NetworkObject::GotPing()
{
	pingSpeed = glutGet(GLUT_ELAPSED_TIME) - pingTime;

	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: GetPing
// Purpose: Gives the calling function the ping speed.
// Inputs: None.
// Outputs: pingSpeed - The current ping speed.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
int NetworkObject::GetPing()
{
	return (int)pingSpeed;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: SendMovementChange
// Purpose: Sends any changes to the player's state to the server.
// Inputs: type - The type of movement made.
//         move - If they are starting or stopping that movement.
//         x - The x position of the player.
//         y - The y position of the player.
//         z - The z position of the player.
//         r - The rotation of the player.
// Outputs: A boolean value indicating success.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
bool NetworkObject::SendMovementChange(int type, bool move, float x, float y, 
									   float z, float r)
{
	MSG_MOVEMENT_DATA movementMessage;
	int messageSize;
	bool result;
	short movementType;

	// Determine the movement type.
	movementType = (short)type;

	// Add 4 if it is the start of that movement type, otherwise leave the 
	// value as is to indicate it is stopping that movement type.
	if(move)
	{
		movementType += 4;
	}

	// Setup the message for the server.
	movementMessage.type = MSG_MOVEMENT;
	movementMessage.toId = ID_SERVER;
	movementMessage.fromId = Socket->GetId();
	movementMessage.movementType = movementType;
	movementMessage.posX = x;
	movementMessage.posY = y;
	movementMessage.posZ = z;
	movementMessage.rot  = r;
	movementMessage.playerId = 0;  // Not used here, only used by server sending to others.
	
	// Get the size of the message.
	messageSize = sizeof(MSG_MOVEMENT_DATA);

	// Send the message.
	result = Socket->SendMessage((char*)&movementMessage, messageSize);
	if(!result)
	{
		return false;
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: HandleOtherPlayerMovement
// Purpose: When a message from the server is received indicating that another
//          player has moved then this function is called to update the client.
// Inputs: msg - The message containing the data of the player that moved.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void NetworkObject::HandleOtherPlayerMovement(MSG_GENERIC_DATA* msg)
{
	MSG_MOVEMENT_DATA* movementMessage;
	short movementType, playerId;
	float posX, posY, posZ, rot;
  

	// Co-erce the message into a movement message type.
	movementMessage = (MSG_MOVEMENT_DATA*)msg;

	// Get the data out of the message.
	movementType = movementMessage->movementType;
	posX = movementMessage->posX;
	posY = movementMessage->posY;
	posZ = movementMessage->posZ;
	rot = movementMessage->rot;
	playerId = movementMessage->playerId;

	// Update that player's state on this client.
	GamePtr->UpdateMovement(playerId, movementType, posX, posY, posZ, rot);

	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: HandleActiveMessage
// Purpose: Handles messages from the server regarding if another player 
//          enters or leaves a zone.
// Inputs: msg - The message contains the active status of the player.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void NetworkObject::HandleActiveMessage(MSG_GENERIC_DATA* msg)
{
	MSG_ACTIVE_DATA* message;
	short id, active;


	// Change the message type into a active message.
	message = (MSG_ACTIVE_DATA*)msg;

	// Get the id and active state of the player.
	id = message->playerId;
	active = message->active;

	// Set that player as active or inactive.
	GamePtr->SetOtherPlayerActiveStatus(id, active);

	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: SendTextMessage
// Purpose: Sends text messages from the client to the server.
// Inputs: textMessage - The message to be sent.
// Outputs: A boolean value indicating success.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
bool NetworkObject::SendTextMessage(char* textMessage)
{
	MSG_TEXTMESSAGE_DATA message;
	int messageSize;
	bool result;
	

	// Setup the text message to be sent.
	message.type = MSG_TEXTMESSAGE;
	message.toId = ID_SERVER;
	message.fromId = Socket->GetId();
	strcpy(message.text, textMessage);

	// Get the size of the message.
	messageSize = sizeof(MSG_TEXTMESSAGE_DATA);

	// Send the message.
	result = Socket->SendMessage((char*)&message, messageSize);
	if(!result)
	{
		return false;
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: HandleTextMessage
// Purpose: Handles incoming text messages from the server.
// Inputs: msg - The incoming text message.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void NetworkObject::HandleTextMessage(MSG_GENERIC_DATA* msg)
{
	MSG_TEXTMESSAGE_DATA* message;
	char text[80];


	// Coerce the message into a text message data type.
	message = (MSG_TEXTMESSAGE_DATA*)msg;

	// Copy the text data out of the message.
	strcpy(text, message->text);
	
	// Send the message to the game object to display onto the screen.
	GamePtr->NewNetworkTextMessage(text);

	return;
}
