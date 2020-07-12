#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <errno.h>
#include "network.h"

using namespace std;

int localSocket;
sockaddr_in servInetInfo;

extern int errno;

void updateEvilCraft(char *);

void handleMessage(char * message) {
	char size = message[M_SIZE];
	switch(message[M_TYPE]) {
		case MSG_TEXT:
			cout << "Somebody said, \"" << &message[2] << "\"\n";
			break;
		case MSG_MVMT_UPD:
			updateEvilCraft(&message[2]);
			break;
	} // end switch()
} // end handleMessage()

void* listeningThread(void* ptr) {
	bool listening = true;
	char recvBuffer[1024];

	while(listening) {
		memset(recvBuffer, 0, 1024);
		// wait for data to come -- recv() is blocking
		int bytesRead   = recv(localSocket, recvBuffer, 1024, 0);
		// extract the message type
		int messageType = recvBuffer[0];

		if(bytesRead > 0)
		{
			int bytesToRead;

			if(messageType == MSG_BOOT)
			{
				cout << "The server booted you!\n";
				close(localSocket);
				listening = false;
				pthread_exit(0);
			}
			else if(bytesRead > 1)
				bytesToRead = (int)recvBuffer[1];
			else
				bytesToRead = 2;

			while(bytesRead < bytesToRead)
			{
				cout << "Had to recv bytes in more than one burst...\n";
				int t = recv(localSocket, recvBuffer, bytesRead, 1024 - bytesRead);

				if(t == 0)
				{
					cout << "Received 0 bytes, bailing!\n";
					pthread_exit(0);
					close(localSocket);
					exit(0);
				}

				bytesRead += t;
				if(bytesRead == 1024)
				{
					cout << "Error, buffer overflow (bad)\n";
					pthread_exit(0);
				}

				bytesToRead = (int)recvBuffer[1];
			}
			handleMessage(recvBuffer);
		}
		else // bytesRead is 0 -- connection has blown up
		{
			cout << "Error, connection broken HARD...\n";
			pthread_exit(0);
		}
	}
} // end listeningThread()

void sendMessage(char* message, int size, char type) { 
	char * netData = new char[size+2];
	netData[M_TYPE] = type;
	netData[M_SIZE] = size+2;
	if(size != 0)
		strcpy(&netData[2], message); 
	// send the buffer with the specified size
	int i = 0;
	for(int totalBytesSent = 0; totalBytesSent < size; ++i)
	{
		// let us know if we had to send bytes in more than one burst
		// -- this is abnormal but it can happen -- should be ok
		if(i > 0)
			cout << "Had to send bytes in more than one burst...\n";

		int bytesSent = send(localSocket, &netData[totalBytesSent], size - totalBytesSent, 0);

		// if a send ever returns 0 the connection has blown up
		if(bytesSent < 1)
		{
			cout << "Error, sent 0 bytes (connection broken)\n";
			return;
		}
		totalBytesSent += bytesSent;
	}
} // end sendMessage()

void stopNetwork(void) {
	sendMessage(0, 0, MSG_DISCONNECT);
	close(localSocket);
} // end stopNetwork()

void startNetwork(const char * addr) {
	// used for storing results of function calls
	int result;

	localSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(socket < 0)
	{ 
		cout << "Error: couldn't create socket, errno = " << errno;
		exit(-1);
	}

	// Without letting the sockets be reusable, linger time prevents binding on them for
	// a while -- irritating
	int on = 1;
	setsockopt(localSocket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	// Set up the server info
	servInetInfo.sin_family = AF_INET;
	servInetInfo.sin_addr.s_addr = inet_addr(addr);
	servInetInfo.sin_port = htons(4444);

	result = connect(localSocket, (struct sockaddr *)&servInetInfo, sizeof(servInetInfo));

	if(result < 0) 
	{
		cout << " Error: couldn't connect to server, errno = " << errno << '\n';
		close(localSocket);
		exit(-1);
	}

	pthread_t slt;
	pthread_create(&slt, NULL, listeningThread, NULL);

	cout << "Client started.\n";
} // end startNetwork()
