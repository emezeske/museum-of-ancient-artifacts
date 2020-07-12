////////////////////////////////////////////////////////////////////////////////
// Filename: clientobject.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _CLIENTOBJECT_H_
#define _CLIENTOBJECT_H_


//////////////
// INCLUDES //
//////////////
#include <netinet/in.h>
#include <iostream.h>


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "messages.h"
#include "socketobject.h"
#include "zoneobject.h"


//////////////////////////
// EXTERN CLASS DEFINES //
//////////////////////////
class SocketObject;
class ZoneObject;


////////////////////////////////////////////////////////////////////////////////
// Class name: ClientObject
////////////////////////////////////////////////////////////////////////////////
class ClientObject
{
public:
  ClientObject();                     // Constructor.
  ClientObject(const ClientObject&);  // Copy constructor.
  ~ClientObject();                    // Destructor.

  void GetPtrs(SocketObject*, ZoneObject*);  // Gets pointers to object for IPC.
  void ProcessMessage();  // Processes incoming messages from this client.

private:
  void HandleDisconnectMessage();  // Handles disconnection messages from client.
  void HandlePingMessage();        // Handles ping messages from client.
  void HandleMovementMessage();    // Handles movement messages from client.
  void HandleTextMessage();        // Handles text messages from client.

public:
  bool online;                 // Indicates if this client is online or not.
  bool processing;             // Indicates if this object is processing in a thread.
  sockaddr_in socketAddress;   // The address of this client.
  unsigned int addressLength;  // The size of the address of the client.
  int clientSocket;            // The socket used by this client to communicate.
  char buffer[1024];           // The buffer for reading data from this client.

private:
  SocketObject* SockPtr;  // Pointer to the socket object using this object.
  ZoneObject* ZonePtr;    // Pointer to the zone object using this object.
};

#endif
