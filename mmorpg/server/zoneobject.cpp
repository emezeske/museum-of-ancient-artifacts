////////////////////////////////////////////////////////////////////////////////
// Filename: zoneobject.cpp
////////////////////////////////////////////////////////////////////////////////
#include "zoneobject.h"


////////////////////////////////////////////////////////////////////////////////
// Function name: ZoneObject
// Purpose: Constructor.
// Inputs: None.
// Outputs: None.
// Details: Sets the pointers to NULL.
////////////////////////////////////////////////////////////////////////////////
ZoneObject::ZoneObject()
{
  Players = NULL;
  SockPtr = NULL;
}


////////////////////////////////////////////////////////////////////////////////
// Function name: ZoneObject
// Purpose: Copy constructor.
// Inputs: other - The zone object to be copied.
// Outputs: None. 
// Details: None.
////////////////////////////////////////////////////////////////////////////////
ZoneObject::ZoneObject(const ZoneObject& other)
{
}


////////////////////////////////////////////////////////////////////////////////
// Function name: ~ZoneObject
// Purpose: Destructor.
// Inputs: None.
// Outputs: None.
// Details: Frees the allocated object memeory.
////////////////////////////////////////////////////////////////////////////////
ZoneObject::~ZoneObject()
{
  // Delete all the players.
  if(Players)
  {
    delete [] Players;
    Players = NULL;
  }

  // Release the pointer to the socket object.
  SockPtr = NULL;
}


////////////////////////////////////////////////////////////////////////////////
// Function name: GetSocketPtr
// Purpose: Gets a copy of the socket object for communication purposes.
// Inputs: ptr - The pointer to the socket object.
// Outputs: None.
// Details: None.
////////////////////////////////////////////////////////////////////////////////
void ZoneObject::GetSocketPtr(SocketObject* ptr)
{
  SockPtr = ptr;
  return;
}


////////////////////////////////////////////////////////////////////////////////
// Function name: SetMaxPlayers
// Purpose: Sets the maximum players for a zone and creates the player objects.
// Inputs: max - The maximum number of players.
// Outputs: None.
// Details: None.
////////////////////////////////////////////////////////////////////////////////
void ZoneObject::SetMaxPlayers(int max)
{
  // Save the max number of players.
  maxPlayers = max;

  // Create the players.
  Players = new PlayerObject[maxPlayers];

  return;
}


////////////////////////////////////////////////////////////////////////////////
// Function name: UpdatePlayerMovement
// Purpose: Updates the position of the player and notifies all the other 
//          players in the zone.
// Inputs: id - The id number of the player.
//         movement - The movement state of the player.
//         x - The x position of the player.
//         y - The y position of the player.
//         z - The z position of the player.
//         r - The rotation of the player.
// Outputs: None.
// Details: None.
////////////////////////////////////////////////////////////////////////////////
void ZoneObject::UpdatePlayerMovement(short id, short movement, float x, float y, float z, float r)
{
  short i;
  MSG_MOVEMENT_DATA movementMessage;
  int messageSize;
  bool result;


  // Update the player data.
  Players[id].UpdateMovement(movement, x, y, z, r);

  // Setup the notification message for the other players.
  movementMessage.type = MSG_MOVEMENT;
  movementMessage.fromId = ID_SERVER;
  movementMessage.movementType = movement;
  movementMessage.posX = x;
  movementMessage.posY = y;
  movementMessage.posZ = z;
  movementMessage.rot  = r;
  movementMessage.playerId = id;
  messageSize = sizeof(MSG_MOVEMENT_DATA);

  // Notify the other players of the change.
  for(i=1; i<maxPlayers; i++)
  {
    if(Players[i].IsActive() && (i != id))
    {
      movementMessage.toId = i;
  
      result = SockPtr->SendMessage(i, (char*)&movementMessage, messageSize);
      if(!result)
      {
	cout << "> Error: Could not send message in ZoneObject::UpdatePlayerMovement." << endl;
      }
    }
  }

  return;
}


////////////////////////////////////////////////////////////////////////////////
// Function name: SetPlayerActive
// Purpose: Adds a new player to the zone and notifies the other players.  It
//          also lets the new player know where everyone else is also.
// Inputs: id - The id number of the new player.
// Outputs: None.
// Details: None.
////////////////////////////////////////////////////////////////////////////////
void ZoneObject::SetPlayerActive(short id)
{
  MSG_ACTIVE_DATA message;
  short i;
  bool result;
  int messageSize;
  short movement;
  float x, y, z, r;
  MSG_MOVEMENT_DATA movementMessage;


  // Set the player status to active.
  Players[id].SetActive();

  // Notify all the other players that someone new has entered the zone.
  message.type = MSG_ACTIVE;
  message.fromId = ID_SERVER;
  message.active = 1;
  message.playerId = id;
  messageSize = sizeof(MSG_ACTIVE_DATA);

  for(i=1; i<maxPlayers; i++)
  {
    if(Players[i].IsActive() && (i != id))
    {
      message.toId = i;
  
      result = SockPtr->SendMessage(i, (char*)&message, messageSize);
      if(!result)
      {
	cout << "> Error: Could not send message in ZoneObject::SetPlayerActive." << endl;
      }
    }
  }

  // Notify the new player of all the people in the zone.
  for(i=1; i<maxPlayers; i++)
  {
    if(Players[i].IsActive() && (i != id))
    {
      Players[i].GetMovement(movement, x, y, z, r);

      movementMessage.type = MSG_MOVEMENT;
      movementMessage.fromId = ID_SERVER;
      movementMessage.toId = id;
      movementMessage.movementType = movement;
      movementMessage.posX = x;
      movementMessage.posY = y;
      movementMessage.posZ = z;
      movementMessage.rot  = r;
      movementMessage.playerId = i;
      messageSize = sizeof(MSG_MOVEMENT_DATA);

      result = SockPtr->SendMessage(id, (char*)&movementMessage, messageSize);
      if(!result)
      {
	cout << "> Error: Could not send message in ZoneObject::SetPlayerActive." << endl;
      }
    }
  }

  return;
}


////////////////////////////////////////////////////////////////////////////////
// Function name: SetPlayerInactive
// Purpose: Removes a player from the zone and notifies the other players.
// Inputs: id - The id number of the player.
// Outputs: None.
// Details: None.
////////////////////////////////////////////////////////////////////////////////
void ZoneObject::SetPlayerInactive(short id)
{
  MSG_ACTIVE_DATA message;
  short i;
  bool result;
  int messageSize;


  // Set the player status to inactive.
  Players[id].SetInactive();

  // Notify all the other players that a player has left the zone.
  message.type = MSG_ACTIVE;
  message.fromId = ID_SERVER;
  message.active = 0;
  message.playerId = id;
  messageSize = sizeof(MSG_ACTIVE_DATA);

  for(i=1; i<maxPlayers; i++)
  {
    if(Players[i].IsActive() && (i != id))
    {
      message.toId = i;
  
      result = SockPtr->SendMessage(i, (char*)&message, messageSize);
      if(!result)
      {
	cout << "> Error: Could not send message in ZoneObject::SetPlayerInactive." << endl;
      }
    }
  }

  return;
}


////////////////////////////////////////////////////////////////////////////////
// Function name: NewTextMessage
// Purpose: Receives a message from a player and sends it to everyone in the 
//          zone.
// Inputs: id - The id number of the player who sent the message.
//         text - The text message sent from the player.
// Outputs: None.
// Details: None.
////////////////////////////////////////////////////////////////////////////////
void ZoneObject::NewTextMessage(short id, char* text)
{
  short i;
  MSG_TEXTMESSAGE_DATA message;
  int messageSize;
  bool result;


  // Copy the text message to a message for the other players.
  message.type = MSG_TEXTMESSAGE;
  message.fromId = ID_SERVER;
  strcpy(message.text, text);
  messageSize = sizeof(MSG_TEXTMESSAGE_DATA);

  // Send the text message to the other active players.
  for(i=1; i<maxPlayers; i++)
  {
    if(Players[i].IsActive() && (i != id))
    {
      message.toId = i;
  
      result = SockPtr->SendMessage(i, (char*)&message, messageSize);
      if(!result)
      {
	cout << "> Error: Could not send message in ZoneObject::NewTextMessage." << endl;
      }
    }
  }

  return;
}
