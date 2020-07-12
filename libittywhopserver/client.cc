#include "client.h"

Client::Client(void) {
	active = false;
} // end Client()

void Client::start(sockaddr_in clientInfo, int tempSock, int id) {
	cout << "\033[34mReceived a connection from " << inet_ntoa(clientInfo.sin_addr) << '\n';

	// Set the player up with appropriate socket/id and set to active status
	playerID = id;
	clientSocket = tempSock;
	active = true;
	myGameIsActive = false;
	inLobby = false;
	myGameNumPlayers = 0;
	name = "Pending";
	otherPlayersInMyGame[0] = -1;
	otherPlayersInMyGame[1] = -1;

	// Send the MSG_ID and give the player their new id
	char msg[3];
	msg[0] = MSG_ID;
	msg[1] = 3;
	msg[2] = playerID;
	sendMessage(msg);

	// start the listening thread for this client
	pthread_create(&thread, NULL, clientThreadFunc, (void*)this);
} // end start()

// To be called when something goes awry 
void Client::reset(void) {
	// Send out lobby exit message to all clients
	if(inLobby)
	{
		char exitMessage[] = { MSG_EXIT_LOBBY, (char)3, (char)playerID };
		handleMessage(exitMessage, playerID);
	}
	// Reset state of client
	active = false;
	myGameIsActive = false;
	otherPlayersInMyGame[0] = -1;
	otherPlayersInMyGame[1] = -1;
	// Shut down socket
	close(clientSocket);
}

void Client::disconnect(void) {
	if(!active)
	{
		reset();
		return;
	}
	// Send disconnect signal
	char msg[2];
	msg[0] = MSG_DISCONNECT;
	msg[1] = 2;
	sendMessage(msg);
	// Fix settings for this playerID
	reset();
	cout << "\033[34mConnection closed to client with playerID: " << playerID << '\n';
} // end stop()

void Client::sendMessage(char * charBuffer) {
	if(!active)
	{
		cout << "\033[31mAttempted to send bytes to playerID " << playerID << " but they are not active...\n";
		return;
	}
	int size = charBuffer[1];
	// send the buffer with the specified size
	int i = 0;
	for(int totalBytesSent = 0; totalBytesSent < size; ++i)
	{
		// let us know if we had to send bytes in more than one burst
		// -- this is abnormal but it can happen -- should be ok
		if(i > 0)
			cout << "\033[31mHad to send bytes to playerID " << playerID << " in more than one burst...\n";

		int bytesSent = send(clientSocket, &charBuffer[totalBytesSent], size - totalBytesSent, 0);

		// if a send ever returns 0 the connection has blown up
		if(bytesSent < 1)
		{
			cout << "\033[31mError, sent 0 bytes (connection broken)\n";
			reset();
			return;
		}
		totalBytesSent += bytesSent;
	}
} // end sendMessage()

void* clientThreadFunc(void* clientPtr) {
	Client* self = (Client*)clientPtr;
	char recvBuffer[1024];

	while(self->active)
	{
		// wait for data to come -- recv() is blocking
		int bytesRead   = recv(self->clientSocket, recvBuffer, 1024, 0);
		// extract the message type
		int messageType = recvBuffer[0];

		if(bytesRead > 0)
		{
			int bytesToRead;

			if(messageType == MSG_DISCONNECT)
			{
				self->disconnect();
				pthread_exit(0);
			}
			else if(bytesRead > 1)
				bytesToRead = (int)recvBuffer[1];
			else
				bytesToRead = 2;

			while(bytesRead < bytesToRead)
			{
				cout << "\033[31mHad to recv bytes in more than one burst...\n";
				int t = recv(self->clientSocket, recvBuffer, bytesRead, 1024 - bytesRead);

				if(t == 0)
				{
					cout << "\033[31mError, connection to playerID " << self->playerID << " broken HARD, recovering...\n";
					self->reset();
					pthread_exit(0);
				}

				bytesRead += t;
				if(bytesRead == 1024)
				{
					cout << "\033[31mError, buffer overflow in playerID " << self->playerID << "'s listening function\n";
					self->reset();
					pthread_exit(0);
				}

				bytesToRead = (int)recvBuffer[1];
			}
			handleMessage(recvBuffer, self->playerID);
		}
		else // bytesRead is 0 -- connection has blown up
		{
			cout << "\033[31mError, connection to playerID " << self->playerID << " broken HARD, recovering...\n";
			self->reset();
			pthread_exit(0);
		}
	}
} // end clientThreadFunc()
