////////////////////////////////////////////////////////////////////////////////
// Filename: socketobject.h
////////////////////////////////////////////////////////////////////////////////
#include "socketobject.h"


/////////////
// GLOBALS //
/////////////
threadParamType threadParam;


////////////////////////////////////////////////////////////////////////////////
// Function name: SocketObject
// Purpose: Constructor.
// Inputs: None.
// Outputs: None.
// Details: This function initializes certain member variables.
////////////////////////////////////////////////////////////////////////////////
SocketObject::SocketObject()
{
  // Set the server status to offline.
  online = false;
  
  // Set the server to not listening for new connections.
  listening = false;

  // Set the socket and the pointers to null.
  serverSocket = 0;
  Clients = NULL;
  ZonePtr = NULL;
}


////////////////////////////////////////////////////////////////////////////////
// Function name: SocketObject
// Purpose: Copy constructor.
// Inputs: other - The socket object being copied.
// Outputs: None.
// Details: None.
////////////////////////////////////////////////////////////////////////////////
SocketObject::SocketObject(const SocketObject& other)
{
}


////////////////////////////////////////////////////////////////////////////////
// Function name: ~SocketObject
// Purpose: Destructor.
// Inputs: None.
// Outputs: None.
// Details: None.
////////////////////////////////////////////////////////////////////////////////
SocketObject::~SocketObject()
{
  // Release the pointers.
  ZonePtr = NULL;
}


////////////////////////////////////////////////////////////////////////////////
// Function name: Initialize
// Purpose: Initialize all the clients and then bring the server online.
// Inputs: None.
// Outputs: A boolean variable inidicating success.
// Details: None.
////////////////////////////////////////////////////////////////////////////////
bool SocketObject::Initialize(ZoneObject* Zone)
{
  bool result;
  int i;


  cout << "> Initializing the server." << endl;

  // Save a copy of the zone pointer.
  ZonePtr = Zone;

  // Set the server status to online.
  online = true;

  // Read in the data for the server from a file.
  result = ReadInServerData();
  if(!result)
  {
    cout << "> Error: Could not read in server data." << endl;
    online = false;
    return false;
  }

  // Create the client objects.
  Clients = new ClientObject[maxPlayers];

  // Let the zone know how many players it can have maximum.
  ZonePtr->SetMaxPlayers(maxPlayers);

  // Give each client a pointer to this class.  We start at index 1 here because
  // id number 0 is the server's id.
  for(i=1; i<maxPlayers; i++)
  {
    Clients[i].GetPtrs(this, ZonePtr);
  }

  // Create the zone server.
  result = CreateServer();
  if(!result)
  {
    cout << "> Error: Could not create server." << endl;
    online = false;
    return false;
  }

  // Set the server status to listening for new connections.
  listening = true;

  cout << "> Server initialized and online." << endl;

  return true;
}


////////////////////////////////////////////////////////////////////////////////
// Function name: ReadInServerData
// Purpose: Reads in the server configuration data from the serverdata.txt file.
// Inputs: None.
// Outputs: A boolean variable indicating success.
// Details: Reads the port number and maximum number of players.  The results
//          are stored in member variables of this class.
////////////////////////////////////////////////////////////////////////////////
bool SocketObject::ReadInServerData()
{
  ifstream fin;
  char input;


  // Open the file.
  fin.open("serverdata.txt");

  // If it can't open the file then return an error.
  if(fin.fail())
  {
    cout << "> Error: Could not open the serverdata.txt file." << endl;
    return false;
  }

  // Read in the port number.
  fin.get(input);
  while(input != ':')
  {
    fin.get(input);
  }
  fin >> portNumber;

  // Read in the maximum number of players.
  fin.get(input);
  while(input != ':')
  {
    fin.get(input);
  }
  fin >> maxPlayers;

  // Close the file.
  fin.close();

  // Do some error checking.
  if(maxPlayers < 1)
  {
    cout << "> Error: Maximum players is invalid." << endl;
    return false;
  }
  
  // Display the results.
  cout << "> Port number: " << portNumber << endl;
  cout << "> Max players: " << maxPlayers << endl;

  return true;
}


////////////////////////////////////////////////////////////////////////////////
// Function name: CreateServer
// Purpose: Creates the server socket and begins listening for new clients 
//          trying to connect.
// Inputs: None.
// Outputs: A boolean variable indicating success.
// Details: None.
////////////////////////////////////////////////////////////////////////////////
bool SocketObject::CreateServer()
{
  struct sockaddr_in inetServerAddress;
  int result;
  pthread_t serverThreadId;
  unsigned long set;


  // If we have gone offline then quit.
  if(!online)
  {
    listening = false;
    return false;
  }

  // Create a UDP socket.
  serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
  if(serverSocket == -1)
  {
    cout << "> Error: Could not create socket." << endl;
    return false;
  }

  // Fill in the address information for binding the socket to.
  memset((char*)&inetServerAddress, 0, sizeof(inetServerAddress));
  inetServerAddress.sin_family = AF_INET;
  inetServerAddress.sin_port = htons(portNumber);
  inetServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);

  // Bind the socket to the address.
  result = bind(serverSocket, (struct sockaddr*)&inetServerAddress, sizeof(inetServerAddress));
  if(result == -1)
  {
    cout << "> Error: Could not bind the socket." << endl;
    close(serverSocket);
    return false;
  }

  // Set the socket to non-blocking I/O.
  set = 1;
  ioctl(serverSocket, FIONBIO, &set);

  // Create a thread to listen for and accept incoming connections from clients.
  pthread_create(&serverThreadId, NULL, ServerListenFunction, (void*)this);

  return true;
}


////////////////////////////////////////////////////////////////////////////////
// Function name: ServerListenFunction
// Purpose: Listens for new connections, if a new connection is made then it
//          sets up the client.
// Inputs: ptr - Pointer to the object calling this function.
// Outputs: None.
// Details: None.
////////////////////////////////////////////////////////////////////////////////
void* ServerListenFunction(void* ptr)
{
  SocketObject* socketPtr;
  char recvBuffer[1024];
  char *ipAddress;
  bool done, result;
  struct sockaddr_in clientAddress;
  struct sockaddr_in newAddress;
  unsigned int clientLength, newLength;
  int bytesRead, newSocket, error;
  MSG_GENERIC_DATA* message;
  short newId;
  pthread_t threadId;


  // Get a pointer to the calling object.
  socketPtr = (SocketObject*)ptr;

  // Set the size of the address.
  clientLength = sizeof(clientAddress);

  // Loop and listen for connections from clients.
  done = false;
  while(!done)
  {
    // If the server has gone offline it is time to leave this listening thread.
    if(socketPtr->Online() == false)
    {
      done = true;
    }

    // Check if there is a message from a client.
    if(socketPtr->Online())
    {
      bytesRead = socketPtr->Read(socketPtr->serverSocket, recvBuffer, sizeof(MSG_GENERIC_DATA), 
				  (struct sockaddr*)&clientAddress, &clientLength);
    }

    // If there is a message then handle it.
    if(bytesRead > 0)
    {
      // Coerce the incoming message into a generic message. 
      message = (MSG_GENERIC_DATA*)recvBuffer;

      // Check to see if this is a knock message from a client.
      if(message->type == MSG_KNOCK)
      {
	// Close the server socket for now.
	close(socketPtr->serverSocket);

	// Display who we got a knock message from.
	ipAddress = inet_ntoa(clientAddress.sin_addr);
	cout << "> Received a knock message from: " << ipAddress << endl;

	// Get an ID number for this new client.
	newId = socketPtr->GetNextFreeIdNumber();
	if(newId < 1)
	{
	  cout << "> Could not assign an id number, server full." << endl;
	}
	else
	{
	  cout << "> Assigning id number to new client: " << newId << endl;
	  
	  // Set the new client to online and save its address.
	  socketPtr->Clients[newId].online = true;
	  socketPtr->Clients[newId].socketAddress = clientAddress;
	  socketPtr->Clients[newId].addressLength = clientLength;

	  // Create a socket to send the ID number to the client.
	  newSocket = socket(AF_INET, SOCK_DGRAM, 0);

	  // Setup the address of the socket.
	  memset((char*)&newAddress, 0, sizeof(newAddress));
	  newAddress.sin_family = AF_INET;
	  newAddress.sin_port = htons(0);
	  newAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	  newLength = sizeof(newAddress);

	  // Bind the new socket.
	  error = bind(newSocket, (struct sockaddr*)&newAddress, newLength);
	  if(error == -1)
	  {
	    cout << "> Error: Could not bind a socket to send the ID number back to the client." << endl;
	    socketPtr->Clients[newId].online = false;
	  }
	  else
	  {
	    // Get the local address associated with the socket.
	    getsockname(newSocket, (struct sockaddr*)&newAddress, &newLength);

	    // Save the socket in the client object.
	    socketPtr->Clients[newId].clientSocket = newSocket;

	    // Send the new ID number to the client.
	    result = socketPtr->SendIdNumber(newId, socketPtr->Clients[newId].socketAddress);
	    if(!result)
	    {
	      cout << "> Error: Could not send ID message back to client." << endl;
	      socketPtr->Clients[newId].online = false;
	      close(socketPtr->Clients[newId].clientSocket);
	    }
	    else
	    {
	      // Start an I/O thread for this client.
	      socketPtr->threadAlive = false;
	      threadParam.id = newId;
	      threadParam.sock = socketPtr;
	      pthread_create(&threadId, NULL, IOThreadFunction, (void*)&threadParam);
	      
	      // Wait for the thread to start.
	      while(!socketPtr->threadAlive)
	      {
	      }

	      // Add the player to the zone.
	      socketPtr->ZonePtr->SetPlayerActive(newId);
	    }
	  }
	}
	done = true;
      }
    }
  }

  // Start the server again and listen for connections.
  socketPtr->CreateServer();

  return 0;
}


////////////////////////////////////////////////////////////////////////////////
// Function name: Online.
// Purpose: Lets the calling function know if the server is online or not.
// Inputs: None.
// Outputs: online - The online status.
// Details: None.
////////////////////////////////////////////////////////////////////////////////
bool SocketObject::Online()
{
  return online;
}


////////////////////////////////////////////////////////////////////////////////
// Function name: Read
// Purpose: Reads in data from the socket from a specified address.
// Inputs: sock - The socket to read the data from.
//         buffer - The buffer to store the incoming data into.
//         count - The maximum number of bytes to read.
//         socketAddress - The address from whom we are reading from.
//         socketLength - The size of the address.
// Outputs: buffer - The data that was read.
//          bytesRead - How many bytes were actually read in.
// Details: We use recvfrom here since our sockets are non-blocking.
////////////////////////////////////////////////////////////////////////////////
int SocketObject::Read(int sock, char* buffer, int count, struct sockaddr* socketAddress, unsigned int* socketLength)
{
  int bytesRead;

  bytesRead = recvfrom(sock, buffer, count, 0, socketAddress, socketLength);

  return bytesRead;
}


////////////////////////////////////////////////////////////////////////////////
// Function name: Send
// Purpose: Sends data to a socket at a specified address.
// Inputs: sock - The socket to send the data on.
//         buffer - The data to send.
//         count - The number of bytes to send.
//         socketAddress - The address we are sending the data to.
//         socketLength - The size of the address.
// Outputs: bytesSent - The number of bytes it actually sent.
// Details: sendto is used here since the socket is non-blocking.
////////////////////////////////////////////////////////////////////////////////
int SocketObject::Send(int sock, char* buffer, int count, struct sockaddr* socketAddress, unsigned int socketLength)
{
  int bytesSent;

  bytesSent = sendto(sock, buffer, count, 0, socketAddress, socketLength);

  return bytesSent;
}


////////////////////////////////////////////////////////////////////////////////
// Function name: GetNextFreeIdNumber
// Purpose: Finds the next free id number for a client.
// Inputs: None.
// Outputs: id - The new id number.
// Details: The count starts at 1 since the server id is 0.  -1 is returned if a
//          free id was not found.
////////////////////////////////////////////////////////////////////////////////
short SocketObject::GetNextFreeIdNumber()
{
  short id;
  int i;


  // Go through the list of clients.
  id = -1;
  for(i=1; i<maxPlayers; i++)
  {
    // Check to see if the client is offline.
    if(!Clients[i].online)
    {
      // Make sure it still isn't inside a processing thread.
      if(!Clients[i].processing)
      {
	// Assign that client the free id.
	id = i;

	// Break out of the loop.
	i = maxPlayers;
      }
    }
  }

  return id;
}


////////////////////////////////////////////////////////////////////////////////
// Function name: SendIdNumber
// Purpose: Sets up the Id message and sends it to the appropriate client.
// Inputs: idNumber - The id number of the new client.
//         clientAddress - The address of the new client.
// Outputs: A boolean variable indicating if it sent the message properly.
// Details: None.
////////////////////////////////////////////////////////////////////////////////
bool SocketObject::SendIdNumber(short idNumber, sockaddr_in clientAddress)
{
  MSG_GENERIC_DATA idMessage;
  int bytesSent;


  // Setup the message to be sent.
  idMessage.type   = MSG_ID;
  idMessage.toId   = idNumber;
  idMessage.fromId = ID_SERVER;

  // Send the message.
  bytesSent = Send(Clients[idNumber].clientSocket, (char*)&idMessage, sizeof(MSG_GENERIC_DATA), 
		   (struct sockaddr*)&clientAddress, sizeof(clientAddress));

  // If the entire message was not sent then return an error.
  if(bytesSent != sizeof(MSG_GENERIC_DATA))
  {
    return false;
  }
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////
// Function name: IOThreadFunction
// Purpose: Listens to a client for data being sent by them.
// Inputs: ptr - Pointer to the object calling this function.
// Outputs: None.
// Details: None.
////////////////////////////////////////////////////////////////////////////////
void* IOThreadFunction(void* ptr)
{
  SocketObject* socketPtr;
  short id;
  int maxfd, nready, bytesRead;
  fd_set allSet, reading;
  struct timeval timeValue;


  // Get the pointer to the socket object calling this function.
  socketPtr = ((threadParamType*)ptr)->sock;

  // Get the id number of the client this thread is listening for.
  id = ((threadParamType*)ptr)->id;

  // Set the client to being active inside the thread.
  socketPtr->Clients[id].processing = true;

  // Indicate the listening thread for the client is running.
  cout << "> Listening thread starting for client: " << id << endl;

  // Set the maximum file descriptor to be the socket for the client.
  maxfd = socketPtr->Clients[id].clientSocket;

  // Reset socket event system.
  FD_ZERO(&allSet);
  FD_SET(socketPtr->Clients[id].clientSocket, &allSet);

  // Set the thread to alive.
  socketPtr->threadAlive = true;

  while(socketPtr->Clients[id].online)
  {
    // Set the time value for the select function to time out.
    timeValue.tv_sec = 10;
    timeValue.tv_usec = 0;

    // Monitor for incoming events.
    reading = allSet;
    nready = select(maxfd + 1, &reading, NULL, NULL, &timeValue);    

    // Data is ready to be read in.
    if(nready > 0)
    {
      bytesRead = socketPtr->ReadIncomingData(id);

      // Data was properly read in.
      if(bytesRead > 0)
      {
	socketPtr->Clients[id].ProcessMessage();
      }

      // Read a FIN, this should not happen with UDP.
      if(bytesRead == 0)
      {
	cerr << "> Error: Read a FIN in listening thread for client: " << id << endl;
      }

      // Error reading.
      if(bytesRead == -1)
      {
	cerr << "> Error reading data in listening thread for client: " << id << endl;
      }      
    }

    // Timeout occured in select.
    if(nready == 0)
    {
      cout << "> Timeout occured in listening thread for client: " << id << endl;
    }

    // Error occured in select.
    if(nready == -1)
    {
      cerr << "> Select error occured in listening thread for client: " << id << endl;
    }
  }

  // Set the client to being finished the thread.
  socketPtr->Clients[id].processing = false;

  // Indicate the listening thread for the client is finished.
  cout << "> Listening thread finished for client: " << id << endl;

  return 0;
}


////////////////////////////////////////////////////////////////////////////////
// Function name: ReadIncomingData
// Purpose: Reads data from a socket when it signals that it has new data.
// Inputs: id - The Id number of the client who sent the data.
// Outputs: bytesRead - The number of bytes read from the client.
// Details: MAX_READ is the maximum number of bytes we will read, this should
//          always be bigger than the maximum message structure we have.
////////////////////////////////////////////////////////////////////////////////
int SocketObject::ReadIncomingData(short id)
{
  int bytesRead;

  bytesRead = Read(Clients[id].clientSocket, Clients[id].buffer, MAX_READ, 
		   (struct sockaddr*)&Clients[id].socketAddress, 
		   &Clients[id].addressLength);

  return bytesRead;
}


////////////////////////////////////////////////////////////////////////////////
// Function name: Shutdown
// Purpose: Shuts down the entire server and disconnects all the clients.
// Inputs: None.
// Outputs: A boolean variable indicating success.
// Details: None.
////////////////////////////////////////////////////////////////////////////////
bool SocketObject::Shutdown()
{
  short i;
  bool result, ret;


  cout << "> Shutting down server." << endl;

  // Set the server status to offline.
  online = false;

  // If there are no clients then there is nothing else to shutdown.
  if(!Clients)
  {
    cout << "> Server shutdown and offline." << endl;
    return true;
  }

  // Set our return value to true.
  ret = true;

  // Disconnect all the players and close the socket connections.
  cout << "> Disconnecting all players." << endl;

  for(i=1; i<maxPlayers; i++)
  {
    if(Clients[i].online)
    {
      // Send that client a disconnect message letting them know the server is shutting down.
      result = SendDisconnectMessage(i);
      if(!result)
      {
	cout << "> Error: Sending disconnect message to client: " << i << endl;
	ret = false;
      }

      // Set the player to inactive.
      ZonePtr->SetPlayerInactive(i);

      // Close the connection to that client.
      result = CloseConnection(i);
      if(!result)
      {
	cout << "> Error: Closing connection to client: " << i << endl;
	ret = false;
      }
    }
  }

  // Wait for all the clients to exit their listening threads.
  cout << "> Waiting for all player threads to finish." << endl;

  for(i=1; i<maxPlayers; i++)
  {
    if(Clients[i].processing)
    {
      while(Clients[i].processing)
      {
      }
    }
  }
 
  // Release the memory.
  delete [] Clients;
  Clients = NULL;

  // Wait for the server to leave its listening for new connections thread.
  cout << "> Waiting for server listening thread to finish." << endl;

  while(listening)
  {
  }

  cout << "> Server shutdown and offline." << endl;

  return ret;
}


////////////////////////////////////////////////////////////////////////////////
// Function name: SendDisconnectMessage
// Purpose: Sends a message to the client from the server telling it that the
//          server has disconnected it.
// Inputs: id - The Id of the client to send the message to.
// Outputs: A boolean variable indicating success.
// Details: None.
////////////////////////////////////////////////////////////////////////////////
bool SocketObject::SendDisconnectMessage(short id)
{
  MSG_GENERIC_DATA messageData;
  int bytesSent, messageSize;


  // Setup the disconnect message.
  messageData.type   = MSG_CLOSECONNECTION;
  messageData.toId   = id;
  messageData.fromId = ID_SERVER;
  messageSize        = sizeof(MSG_GENERIC_DATA);

  // Send the message to the client.
  bytesSent = Send(Clients[id].clientSocket, (char*)&messageData, messageSize, 
		   (struct sockaddr*)&Clients[id].socketAddress, sizeof(Clients[id].socketAddress));

  // Check if the entire message got sent.
  if(bytesSent != messageSize)
  {
    cout << "Error: Entire message was not sent." << endl;
    return false;
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////
// Function name: CloseConnection
// Purpose: Close a client connection socket.
// Inputs: id - The Id number of the client that is being closed.
// Outputs: A boolean variable indicating success.
// Details: None.
////////////////////////////////////////////////////////////////////////////////
bool SocketObject::CloseConnection(short id)
{
  int error;
  bool result;


  result = true;

  // Close the socket.
  error = close(Clients[id].clientSocket);
  if(error == -1)
  {
    cout << "> Error closing socket for id: " << id << endl;
    result = false;
  }

  // Set the socket to NULL.
  Clients[id].clientSocket = 0;

  // Set the client to offline.
  Clients[id].online = false;

  cout << "> Closed connection to id: " << id << endl;

  return result;
}


////////////////////////////////////////////////////////////////////////////////
// Function name: ClientDisconnection
// Purpose: If the server receives a message from a client that the client has
//          disconnected then we close that connection.
// Inputs: clientId - The Id of the client that disconnected.
// Outputs: None.
// Details: This function is called from the ClientObject.
////////////////////////////////////////////////////////////////////////////////
void SocketObject::ClientDisconnection(short clientId)
{
  // Set the player to inactive.
  ZonePtr->SetPlayerInactive(clientId);

  // Close the client's socket.
  CloseConnection(clientId);

  return;
}


////////////////////////////////////////////////////////////////////////////////
// Function name: SetOffline
// Purpose: Sets the server to offline.
// Inputs: None.
// Outputs: None.
// Details: None.
////////////////////////////////////////////////////////////////////////////////
void SocketObject::SetOffline()
{
  online = false;
  return;
}


////////////////////////////////////////////////////////////////////////////////
// Function name: SendMessage
// Purpose: Sends messages from the server to the client.
// Inputs: id - The id of the client the server is sending the message to.
//         buffer - The data message being sent to the client.
//         size - The size of the message being sent.
// Outputs: A boolean variable indicating if the message was successfully sent.
// Details: sendto is used since this is non-blocking I/O.
////////////////////////////////////////////////////////////////////////////////
bool SocketObject::SendMessage(short id, char* buffer, unsigned int size)
{
  unsigned int bytesSent;

  // Send the message to the appropriate client on their socket.
  bytesSent = sendto(Clients[id].clientSocket, buffer, size, 0,
		     (struct sockaddr*)&Clients[id].socketAddress, Clients[id].addressLength);

  // If the entire message was not sent then return an error.
  if(bytesSent != size)
  {
    return false;
  }

  return true;
}
