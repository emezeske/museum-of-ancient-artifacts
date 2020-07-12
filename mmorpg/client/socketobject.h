///////////////////////////////////////////////////////////////////////////////
// Filename: socketobject.h
///////////////////////////////////////////////////////////////////////////////
#ifndef _SOCKETOBJECT_H_
#define _SOCKETOBJECT_H_


//////////////
// INCLUDES //
//////////////
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <unistd.h>


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "messages.h"
#include "networkobject.h"
#include <GL/glut.h>


/////////////
// DEFINES //
/////////////
#define ID_SERVER 0                // The id number for the server.
#define MAX_READ 1024              // Maximum read in bytes.
#define IO_TIMEOUT 100             // The timeout in ms for the network.
const int MAX_OTHER_PLAYERS = 20;  // Max number of people in the same zone.


///////////////////////
// CLASS PRE_DEFINES //
///////////////////////
class NetworkObject;

// I'm not sure if this is a 
// quick hack or what, but it
// seems to work.  I do not know
// much about sockets....
typedef int SOCKET;

///////////////////////////////////////////////////////////////////////////////
// Class name: SocketObject
///////////////////////////////////////////////////////////////////////////////
class SocketObject
{
public:
	SocketObject();
	~SocketObject();
	SocketObject(const SocketObject&);

	bool Initialize(NetworkObject*, char*, unsigned short);
	bool Shutdown();
	bool Disconnect();

	short GetId();
	bool  SendMessage(char*, int);
	bool ProcessMessage(int);

	bool online;
	bool thread;
	char mainBuffer[1024];
	SOCKET clientSocket;
	sockaddr_in socketAddress;
	int addressLength;
	short idNumber;
	NetworkObject* NetworkPtr;

private:
	bool ConnectToServer(char*, unsigned short);
	int  Send(SOCKET, char*, int, struct sockaddr*, int);
	int  Read(SOCKET, char*, int, struct sockaddr*, int*);
	void HandleServerMessage();
	bool CloseConnection();
	bool SendMessageDisconnect();

};


/////////////////////////
// FUNCTION PROTOTYPES //
/////////////////////////
void* IOThreadFunction(void*);


#endif
