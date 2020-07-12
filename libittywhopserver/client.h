#include <string>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "messages.h"

using namespace std;

// external function defined in gameserver.cc
void handleMessage(char *, int);

struct Client {
		bool active;
		bool inLobby;
		string name;
		int playerID;

		string myGame;
		int    myGameNumPlayers;
		bool   myGameIsActive;
		int    otherPlayersInMyGame[2];

		int clientSocket;
		pthread_t thread;

		Client(void);
		void reset(void);
		void start(sockaddr_in, int, int);
		void disconnect(void);
		void sendMessage(char *);
	

};

void* clientThreadFunc(void*);
