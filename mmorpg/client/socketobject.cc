///////////////////////////////////////////////////////////////////////////////
// Filename: socketobject.cpp
///////////////////////////////////////////////////////////////////////////////
#include "socketobject.h"


//////////////
// TYPEDEFS //
//////////////
typedef struct
{
	int id;
	SocketObject* Sock;
} threadParamType;


/////////////
// GLOBALS //
/////////////
threadParamType threadParam;


///////////////////////////////////////////////////////////////////////////////
// Function name: SocketObject
// Purpose: Constructor.
///////////////////////////////////////////////////////////////////////////////
SocketObject::SocketObject()
{
	NetworkPtr = NULL;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: ~SocketObject
// Purpose: Destructor
///////////////////////////////////////////////////////////////////////////////
SocketObject::~SocketObject()
{
	NetworkPtr = NULL;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: SocketObject
// Purpose: Copy constructor.
///////////////////////////////////////////////////////////////////////////////
SocketObject::SocketObject(const SocketObject& other)
{
}


///////////////////////////////////////////////////////////////////////////////
// Function name: Initialize
// Purpose: This function initializes the socket and connects to the server.
// Inputs: netPtr -  Pointer to the object creating this object.
//         ip - The IP address of the server.
//         port - The port number to connect to on the server.
//         hwnd - Handle to the window calling this function.
// Outputs: A boolean value indicating success of this function.
///////////////////////////////////////////////////////////////////////////////
bool SocketObject::Initialize(NetworkObject* netPtr, char* ip, 
			      unsigned short portNumber)
{
	bool result;

	
	NetworkPtr = netPtr;
	if(!NetworkPtr)
	{
		return false;
	}

	result = ConnectToServer(ip, portNumber);
	if(!result)
	{
		std::cerr << "PANIC\n";
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Function name: ConnectToServer
// Purpose: This function connects the client to the server and starts 
//          listening for messages from it.
// Inputs: ipAddress - The IP address of the server.
//         port - The port the server is listening on.
//         hwnd - Handle to the window calling this function.
// Outputs: A boolean variable indicating success.
///////////////////////////////////////////////////////////////////////////////
bool SocketObject::ConnectToServer(char* ipAddress, unsigned short portNumber)
{
	unsigned long inetAddress;
	struct sockaddr_in serverAddress; 
	MSG_GENERIC_DATA knockMessage, *message;
	int bytesSent, bytesRead, newLength;
	unsigned long threadId, startTime;
	bool done, gotId;
	char recvBuffer[1024];
	struct sockaddr_in newAddress;
	pthread_t clientThreadId;

	// Create the UDP socket.
	clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if(clientSocket == -1)
	{
		cerr << "could not create client socket\n";
		return false;
	}

	// Setup the address of the server we are connecting to.
	inetAddress = inet_addr(ipAddress);
	memset((char*)&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(portNumber);
	serverAddress.sin_addr.s_addr = inetAddress;

	// Set the socket to non-blocking I/O.
	unsigned long set = 1;
	ioctl(clientSocket, FIONBIO, &set);


	// Setup a knock message to send to the server.
	knockMessage.type   = MSG_KNOCK;
	knockMessage.toId   = ID_SERVER;
	knockMessage.fromId = -1;  // Since we don't know our ID yet.

	// Send the knock message to the server.
	bytesSent = Send(clientSocket, (char*)&knockMessage, sizeof(MSG_GENERIC_DATA),
		        (struct sockaddr*)&serverAddress, sizeof(serverAddress));
	if(bytesSent < 0)
	{
		return false;
	}

	// Set the length of the address.
	newLength = sizeof(newAddress);

	// Save the start time of our loop.
	startTime = glutGet(GLUT_ELAPSED_TIME);

	// Indicate the client has not gotten its Id yet.
	gotId = false;

	// Loop and wait for an ID number from the server.
	done = false;
	while(!done)
	{
		// Check to see if this loop has been running for longer than 5 seconds.
		if(glutGet(GLUT_ELAPSED_TIME) > (startTime + 5000))
		{
			// If we have been waiting too long then quit the loop.
			done = true;
		}

		// Check for data from the server.
		bytesRead = Read(clientSocket, recvBuffer, sizeof(MSG_GENERIC_DATA),
			        (struct sockaddr*)&newAddress, &newLength);
		if(bytesRead > 0)
		{
			// Save the address from the server.
			socketAddress = newAddress;
			addressLength = newLength;

			// See if it sent an ID message.
			message = (MSG_GENERIC_DATA*)recvBuffer;
			if(message->type == MSG_ID)
			{
				// Get the ID number and set our loop flag to being finished.
				idNumber = message->toId;

				// Check to make sure the Id number is valid.
				if(idNumber > 0)
				{
					done = true;
					gotId = true;
				}
			}
		}
	}

	// If the client did not get an Id from the server then report the error 
	// and quit.
	if(!gotId)
	{
		std::cerr << "Server did not respond after 5 seconds.";
		return false;
	}

	// Set our status to online.
	online = true;

	// Setup the thread paramters.
	thread = false;
	threadParam.id = ID_SERVER;
	threadParam.Sock = this;

	// Create a thread to listen for I/O from the server.
  	pthread_create(&clientThreadId, NULL, IOThreadFunction, (void*)&threadParam);

	while(!thread)
	{
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: Send
// Purpose: Sends data to a socket at a specified address.
// Inputs: sock - The socket to send the data on.
//         buffer - The data buffer to send.
//         bufferSize - Size of the data being sent.
//         socketAddress - Address of the socket.
//         socketLength - Size of the address.
// Outputs: bytesSent - The number of bytes actually sent.
// Details: sendto is used here since the socket is non-blocking.
///////////////////////////////////////////////////////////////////////////////
int SocketObject::Send(SOCKET sock, char* buffer, int bufferSize, 
		       struct sockaddr* socketAddress, int socketLength)
{
	int bytesSent;
	
	bytesSent = sendto(sock, buffer, bufferSize, 0, socketAddress, socketLength);
	
	return bytesSent;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: Read
// Purpose: Reads incoming data on a socket from a specified address.
// Inputs: sock - The socket to read the data on.
//         buffer - The buffer to read the data into.
//         bufferSize - The maximum amount of bytes to read.
//         socketAddress - The address to read from.
//         socketLength - The size of the address.
// Outputs: buffer - The data is read into this buffer,.
// Details: recvfrom is used here since the socket is non-blocking.
///////////////////////////////////////////////////////////////////////////////
int SocketObject::Read(SOCKET sock, char* buffer, int bufferSize,
		       struct sockaddr* socketAddress, int* socketLength)
{
	int bytesRead;

	bytesRead = recvfrom(sock, buffer, bufferSize, 0, socketAddress, (socklen_t*)socketLength);
	
	return bytesRead;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: IOThreadFunction
// Purpose: Listens for incoming data from the server to this client.
// Inputs: ptr - Pointer to the calling object.
// Outputs: None.
// Details: The IO_TIMEOUT is in terms of milliseconds.
///////////////////////////////////////////////////////////////////////////////
void* IOThreadFunction(void* ptr)
{
	int maxfd, id;
	SocketObject* socketPtr;
	bool result;
  	struct timeval timeValue;
  	fd_set allSet, reading;


	// Get a pointer to the calling object.
	socketPtr = ((threadParamType*)ptr)->Sock;

	// Get the Id number of the calling object.
	id = ((threadParamType*)ptr)->id;

	// Set the maximum file descriptor to be the socket for the client.
	maxfd = socketPtr->clientSocket;

	// Reset socket event system.
	FD_ZERO(&allSet);
	FD_SET(socketPtr->clientSocket, &allSet);

	// Set the thread status to active.
	socketPtr->thread = true;
	
	// Loop while the client is still online.
	while(socketPtr->online)
	{
    		// Set the time value for the select function to time out.
   		timeValue.tv_sec = 10;
		timeValue.tv_usec = 0;

		// Monitor for incoming events.
		reading = allSet;
		result = select(maxfd + 1, &reading, NULL, NULL, &timeValue);    

		// Handle the event.
		result = socketPtr->ProcessMessage(id);
		if(result)
		{
			continue;
		}
		else
		{
			std::cerr << "Error in IOThreadFunction";
			socketPtr->online = false;
			break;
		}

		break;
    }
	
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Function name: ProcessMessage
// Purpose: Reads in the data from the socket into the main buffer.
// Inputs: id - The Id number of the client.
// Outputs: None.
// Details: MAX_READ is set to the largest message possible, so the function 
//          will read in up to a maximum number of bytes.  If something is read 
//          in then the HandleServerMessage function handles that which was 
//          read.
///////////////////////////////////////////////////////////////////////////////
bool SocketObject::ProcessMessage(int id)
{
	int bytesRead;

	bytesRead = Read(clientSocket, mainBuffer, MAX_READ, 
			(struct sockaddr*)&socketAddress, &addressLength);
	if(bytesRead > 0)
	{
		HandleServerMessage();
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: HandleServerMessage
// Purpose: Interprets the message received from the server and decides what to
//          do with it.
// Inputs: None.
// Outputs: None.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
void SocketObject::HandleServerMessage()
{
	MSG_GENERIC_DATA* msg;

	// Coerce the main buffer into a generic message so that it can be 
	// determined what kind of message was received.
	msg = (MSG_GENERIC_DATA*)mainBuffer;
	
	// Determine the message type.
	switch(msg->type)
	{
		case MSG_CLOSECONNECTION:
		{
			CloseConnection();
			std::cerr << "Received server disconnect signal\n";
			break;
		}
		case MSG_PING:
		{
			NetworkPtr->GotPing();
			break;
		}
		case MSG_MOVEMENT:
		{
			NetworkPtr->HandleOtherPlayerMovement(msg);
			break;
		}
		case MSG_ACTIVE:
		{
			NetworkPtr->HandleActiveMessage(msg);
			break;
		}
		case MSG_TEXTMESSAGE:
		{
			NetworkPtr->HandleTextMessage(msg);
			break;
		}
		default:
		{
			std::cerr << "Unknown message received.\n";
			break;
		}
	}

	return;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: CloseConnection
// Purpose: Closes the socket and its input event handle.
// Inputs: None.
// Outputs: A boolean variable to indicate success.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
bool SocketObject::CloseConnection()
{
	bool result = true;

	// Close the socket.
	if(close(clientSocket) == -1)
	{
		std::cerr << "Error closing socket...\n";
		result = false;
	}

	clientSocket = 0;

	// Set the client to offline.
	online = false;
	
	return result;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: Shutdown
// Purpose: Disconnects from the server if still connected and then shuts down
//          winsock.
// Inputs: None.
// Outputs: A boolean variable indicating success.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
bool SocketObject::Shutdown()
{
	// If the client is still online then disconnect.
	if(online)
	{
		Disconnect();
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: Disconnect
// Purpose: Sends a message to the server notifying it that this client is 
//          disconnecting and then closes its connection.
// Inputs: None.
// Outputs: A boolean variable indicating success.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
bool SocketObject::Disconnect()
{
	bool result;

	result = SendMessageDisconnect();
	if(!result)
	{
		std::cerr << "Error sending SendMessageDisconnect\n";
		return false;
	}

	result = CloseConnection();
	if(!result)
	{
		std::cerr << "Error sending CloseConnection\n";
		return false;
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: SendMessageDisconnect
// Purpose: Sends a message to the server notifying it that this client is
//          disconnecting from it.
// Inputs: None.
// Outputs: A boolean variable indicating success.
// Details: None.
///////////////////////////////////////////////////////////////////////////////
bool SocketObject::SendMessageDisconnect()
{
	MSG_GENERIC_DATA msgData;
	int thisWrite, messageSize;


	// Setup the close connection message.
	msgData.type   = MSG_CLOSECONNECTION;
	msgData.toId   = ID_SERVER;
	msgData.fromId = idNumber;
	messageSize    = sizeof(MSG_GENERIC_DATA);

	// Send the message to the server.
	thisWrite = sendto(clientSocket, (char*)&msgData, messageSize, 0, 
			  (struct sockaddr*)&socketAddress, addressLength);

	// Check to make sure the entire message was sent.
	if(thisWrite != messageSize)
	{
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Function name: GetId
// Purpose: Gives the calling function the Id number of this client.
// Inputs: None.
// Outputs: idNumber - The Id number of this client.
// Details: The Id number is assigned by the server when we connect.
///////////////////////////////////////////////////////////////////////////////
short SocketObject::GetId()
{
	return idNumber;
}


///////////////////////////////////////////////////////////////////////////////
// Function name: SendMessage
// Purpose: Sends a message to the server.
// Inputs: buffer - The message to send.
//         size - The size of the message being sent.
// Outputs: A boolean value indicating if the message was sent correctly.
// Details: sendto is used since the I/O is non-blocking.
///////////////////////////////////////////////////////////////////////////////
bool SocketObject::SendMessage(char* buffer, int size)
{
	int bytesSent;

	bytesSent = sendto(clientSocket, buffer, size, 0, 
			  (struct sockaddr*)&socketAddress, addressLength);

	if(bytesSent != size)
	{
		return false;
	}

	return true;
}
