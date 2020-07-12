////////////////////////////////////////////////////////////////////////////////
// Filename: socketobject.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _SOCKETOBJECT_H_
#define _SOCKETOBJECT_H_


//////////////
// INCLUDES //
//////////////
#include <iostream.h>
#include <fstream.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "messages.h"
#include "clientobject.h"
#include "zoneobject.h"


///////////////////////
// CLASS PRE-DEFINES //
///////////////////////
class ClientObject;
class SocketObject;
class ZoneObject;


//////////////
// TYPEDEFS //
//////////////
struct threadParamType
{
  short id;
  SocketObject* sock;
};


/////////////
// DEFINES //
/////////////
#define ID_SERVER 0    // The id number of the server.
#define MAX_READ 1024  // The maximum size of data read in a single read.


////////////////////////////////////////////////////////////////////////////////
// Class name: SocketObject
////////////////////////////////////////////////////////////////////////////////
class SocketObject
{
public:
  SocketObject();                     // Constructor.
  SocketObject(const SocketObject&);  // Copy constructor.
  ~SocketObject();                    // Destructor.

  bool Initialize(ZoneObject*);  // Initializes the server and brings it online.
  bool CreateServer();           // Creates the server socket and starts the listening thread.
  bool Online();                 // Indicates if the server is online of not.
  int Read(int, char*, int, struct sockaddr*, unsigned int*);  // Reads socket data.
  int Send(int, char*, int, struct sockaddr*, unsigned int);   // Sends socket data.
  short GetNextFreeIdNumber();                   // Finds the next free id number for a client.
  bool SendIdNumber(short, sockaddr_in);         // Sends id messages to new clients.
  int ReadIncomingData(short);                   // Reads data from a socket when it signals that it has new data.
  bool Shutdown();                               // Shuts down the entire server.
  void ClientDisconnection(short);               // Handles when a client disconnects from the server.
  void SetOffline();                             // Sets the server to offline status.
  bool SendMessage(short, char*, unsigned int);  // Sends messages from server to the client.

private:
  bool ReadInServerData();            // Reads in the number of players and server port number.
  bool SendDisconnectMessage(short);  // Sends a message to a client when server disconnects them.
  bool CloseConnection(short);        // Close a client connection socket.

public:
  int serverSocket;       // The socket for the server.
  ClientObject* Clients;  // Pointer to the client objects.
  bool threadAlive;       // If the server is processing in a thread.
  ZoneObject* ZonePtr;    // Pointer to the zone object.

private:
  unsigned short portNumber;  // The port number the server is listening on.
  int maxPlayers;             // The maximum number of players allowed to connect at once.
  bool online;                // Online status of the server.
  bool listening;             // Listening to connections status of the server.
};


/////////////////////////
// FUNCTION PROTOTYPES //
/////////////////////////
void* ServerListenFunction(void*);  // Listens for connection from clients.
void* IOThreadFunction(void*);      // Listens to a client for data being sent by them.

#endif
