////////////////////////////////////////////////////////////////////////////////
// Filename: clientobject.cpp
////////////////////////////////////////////////////////////////////////////////
#include "clientobject.h"


////////////////////////////////////////////////////////////////////////////////
// Function name: ClientObject
// Purpose: Constructor.
// Inputs: None.
// Outputs: None.
// Details: Initializes member variables.
////////////////////////////////////////////////////////////////////////////////
ClientObject::ClientObject()
{
  online = false;      // Set the client to offline.
  processing = false;  // Set the client to not processing in a thread.

  // Set the pointers and socket to null.
  SockPtr = NULL;  
  ZonePtr = NULL;
  clientSocket = 0;
}


////////////////////////////////////////////////////////////////////////////////
// Function name: ClientObject
// Purpose: Copy constructor.
// Inputs: other - The other object it is copying.
// Outputs: None.
// Details: None.
////////////////////////////////////////////////////////////////////////////////
ClientObject::ClientObject(const ClientObject& other)
{
}


////////////////////////////////////////////////////////////////////////////////
// Function name: ~ClientObject
// Purpose: Destructor.
// Inputs: None.
// Outputs: None.
// Details: Releases pointers to the objects.
////////////////////////////////////////////////////////////////////////////////
ClientObject::~ClientObject()
{
  SockPtr = NULL;
  ZonePtr = NULL;
}


////////////////////////////////////////////////////////////////////////////////
// Function name: GetPtrs
// Purpose: Gets a pointer to the SocketObject and the ZoneObject.
// Inputs: ptr1 - Pointer to the socket object.
//         ptr2 - Pointer to the zone object.
// Outputs: None.
// Details: Stores pointers to other objects for easy communication between 
//          them.
////////////////////////////////////////////////////////////////////////////////
void ClientObject::GetPtrs(SocketObject* ptr1, ZoneObject* ptr2)
{
  SockPtr = ptr1;
  ZonePtr = ptr2;
  return;
}


////////////////////////////////////////////////////////////////////////////////
// Function name: ProcessMessage.
// Purpose: Determines which type of message the server has received for this 
//          client.
// Inputs: None.
// Outputs: None.
// Details: None.
////////////////////////////////////////////////////////////////////////////////
void ClientObject::ProcessMessage()
{
  MSG_GENERIC_DATA* msg;


  // Coerce the data buffer into a generic message so this function can figure 
  // out what type of message it is.
  msg = (MSG_GENERIC_DATA*)buffer;

  switch(msg->type)
  {
    case MSG_CLOSECONNECTION:
    {
      HandleDisconnectMessage();
      break;
    }
    case MSG_PING:
    {
      HandlePingMessage();
      break;
    }
    case MSG_MOVEMENT:
    {
      HandleMovementMessage();
      break;
    }
    case MSG_TEXTMESSAGE:
    {
      HandleTextMessage();
      break;
    }
    default:
    {
      cout << "Error: A client sent an unknown message." << endl;
      break;
    }
  }

  return;
}


////////////////////////////////////////////////////////////////////////////////
// Function name: HandleDisconnectMessage
// Purpose: Handles a disconection message from a client.
// Inputs: None.
// Outputs: None.
// Details: None.
////////////////////////////////////////////////////////////////////////////////
void ClientObject::HandleDisconnectMessage()
{
  MSG_GENERIC_DATA* msg;
  msg = (MSG_GENERIC_DATA*)buffer;

  // Have the socket object disconnect this client from the server.
  SockPtr->ClientDisconnection(msg->fromId);

  return;
}


////////////////////////////////////////////////////////////////////////////////
// Function name: HandlePingMessage
// Purpose: Handles a ping message from a client.
// Inputs: None.
// Outputs: None.
// Details: None.
////////////////////////////////////////////////////////////////////////////////
void ClientObject::HandlePingMessage()
{
  MSG_GENERIC_DATA* msg;
  short id;
  MSG_GENERIC_DATA pingMessage;
  unsigned int messageSize;
  bool result;


  // Coerce the buffer to a generic message to get the id of the client that 
  // sent the ping.
  msg = (MSG_GENERIC_DATA*)buffer;
  id = msg->fromId;

  // Create a ping message to send back to them.
  pingMessage.type   = MSG_PING;
  pingMessage.fromId = ID_SERVER;
  pingMessage.toId   = id;
  messageSize        = sizeof(MSG_GENERIC_DATA);  

  // Send the ping message back.
  result = SockPtr->SendMessage(id, (char*)&pingMessage, messageSize);
  if(!result)
  {
    cout << "> Error: Ping message for client " << id << " could not be sent." << endl;
  }

  return;
}


////////////////////////////////////////////////////////////////////////////////
// Function name: HandlePingMessage
// Purpose: Handles movement messages from clients.
// Inputs: None.
// Outputs: None.
// Details: None.
////////////////////////////////////////////////////////////////////////////////
void ClientObject::HandleMovementMessage()
{
  MSG_MOVEMENT_DATA* movementMessage;
  short type, id;
  float x, y, z, r;


  // Coerce the buffer into a movement message.
  movementMessage = (MSG_MOVEMENT_DATA*)buffer;

  // Get the id and movement data from the message.
  id = movementMessage->fromId;
  type = movementMessage->movementType;
  x = movementMessage->posX;
  y = movementMessage->posY;
  z = movementMessage->posZ;
  r = movementMessage->rot;

  // Have the zone update the clients info.
  ZonePtr->UpdatePlayerMovement(id, type, x, y, z, r);

  return;
}


////////////////////////////////////////////////////////////////////////////////
// Function name: HandleTextMessage
// Purpose: Handles text messages from client.
// Inputs: None.
// Outputs: None.
// Details: None.
////////////////////////////////////////////////////////////////////////////////
void ClientObject::HandleTextMessage()
{
  MSG_TEXTMESSAGE_DATA* message;
  short id;
  char text[80];


  // Coerce the buffer into a text message format.
  message = (MSG_TEXTMESSAGE_DATA*)buffer;

  // Get the id of the client who sent it as well as the text message.
  id = message->fromId;
  strcpy(text, message->text);

  // Notify the zone of the new text message from this client.
  ZonePtr->NewTextMessage(id, text);

  return;
}
