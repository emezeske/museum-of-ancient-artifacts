#include <string>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "client.h"

const char MAX_PLAYERS = 20;

using namespace std;

Client clients[MAX_PLAYERS];
int serverSocket;

void* serverListeningThread(void* ptr) {
	// Set up a struct for holding client info
	sockaddr_in clientInetInfo;
	unsigned int clientInetInfoLen = sizeof(clientInetInfo);

	bool listening = true;

	while(listening) {

		// listen for connection
		int clientSocket = accept(serverSocket, (sockaddr*)&clientInetInfo, & clientInetInfoLen);
		
		// find a free ID number to assign to client
		int freeID = 0;
		for(; freeID < MAX_PLAYERS; ++freeID)
		{
			if(!clients[freeID].active)
				break;
		}
		// server is full -- do something
		if(freeID == MAX_PLAYERS)
		{
			cout << "\033[31mServer is full\n";
			continue;
		}
		else
		{
			// Create new client thread and set to active
			clients[freeID].start(clientInetInfo, clientSocket, freeID);
		}
	}
} // end serverListeningThread()

void handleMessage(char* message, int fromID) {
	if(message[0] == MSG_LOGON)
	{
		clients[fromID].inLobby = true;
		int len = (int)message[1] - 3;
		char tempName[len];

		for(int i = 0; i < len; ++i)
			tempName[i] = message[3 + i];

		tempName[len] = '\0';
		clients[fromID].name = tempName;

		for(int i = 0; i < MAX_PLAYERS; ++i)
			if(clients[i].active && fromID != i)
			{
				if(clients[i].inLobby)
				{
					char tMsg[3 + clients[i].name.length()];
					tMsg[0] = MSG_ENTER_LOBBY;
					tMsg[1] = (char)(3 + clients[i].name.length());
					tMsg[2] = (char)i;
					for(int z = 0; z < clients[i].name.length(); ++z)
						tMsg[3 + z] = clients[i].name[z];
					clients[fromID].sendMessage(tMsg);
				}
				if(clients[i].myGameIsActive)
				{
					char tMsg[4 + clients[i].name.length()];
					tMsg[0] = MSG_NEW_GAME;
					tMsg[1] = (char)(4 + clients[i].myGame.length());
					tMsg[2] = (char)i;
					tMsg[3] = (char)clients[i].myGameNumPlayers;
					for(int z = 0; z < clients[i].myGame.length(); ++z)
						tMsg[4 + z] = clients[i].myGame[z];
					clients[fromID].sendMessage(tMsg);
				}
			}
		message[0] = MSG_ENTER_LOBBY;
	}
	if(message[0] == MSG_NEW_GAME)
	{
		int len = (int)message[1] - 4;
		char tempGameName[len];

		for(int i = 0; i < len; ++i)
			tempGameName[i] = message[4 + i];

		tempGameName[len] = '\0';
		clients[fromID].myGame = tempGameName;
		clients[fromID].myGameNumPlayers = (int)message[3];
		clients[fromID].myGameIsActive = true;
	}
	if(message[0] == MSG_JOIN_GAME)
	{
		int gameID = (int)message[3];
		for(int i = 0; i < 2; ++i)
			if(clients[gameID].otherPlayersInMyGame[i] == -1 && clients[gameID].myGameIsActive)
			{
				clients[gameID].otherPlayersInMyGame[i] = fromID;
				if(i + 2 == clients[gameID].myGameNumPlayers)
				{
					if(clients[gameID].myGameNumPlayers == 2)
					{
						char gameStartMessage[4];
						gameStartMessage[0] = MSG_START_GAME;
						gameStartMessage[1] = (char)4;
						gameStartMessage[2] = (char)gameID;
						gameStartMessage[3] = (char)clients[gameID].otherPlayersInMyGame[0];
						// Tell everyone in game that it started.
						clients[gameID].sendMessage(gameStartMessage);
						clients[clients[gameID].otherPlayersInMyGame[0]].sendMessage(gameStartMessage);
						// Fill in values for everyone's other players and stuff
						clients[clients[gameID].otherPlayersInMyGame[0]].otherPlayersInMyGame[0] = gameID;
						clients[clients[gameID].otherPlayersInMyGame[0]].myGameNumPlayers = 2;
						// Yank from lobby
						clients[gameID].inLobby = false;
						clients[clients[gameID].otherPlayersInMyGame[0]].inLobby = false;
					}
					else
					{
						char gameStartMessage[5];
						gameStartMessage[0] = MSG_START_GAME;
						gameStartMessage[1] = (char)5;
						gameStartMessage[2] = (char)gameID;
						gameStartMessage[3] = (char)clients[gameID].otherPlayersInMyGame[0];
						gameStartMessage[4] = (char)clients[gameID].otherPlayersInMyGame[1];
						// Tell everyone in game that it started.
						clients[gameID].sendMessage(gameStartMessage);
						clients[clients[gameID].otherPlayersInMyGame[0]].sendMessage(gameStartMessage);
						clients[clients[gameID].otherPlayersInMyGame[1]].sendMessage(gameStartMessage);
						// Fill in values for everyone's other players and stuff
						clients[clients[gameID].otherPlayersInMyGame[0]].otherPlayersInMyGame[0] = gameID;
						clients[clients[gameID].otherPlayersInMyGame[0]].otherPlayersInMyGame[1] = clients[gameID].otherPlayersInMyGame[1];
						clients[clients[gameID].otherPlayersInMyGame[0]].myGameNumPlayers = 3;
						clients[clients[gameID].otherPlayersInMyGame[1]].otherPlayersInMyGame[0] = gameID;
						clients[clients[gameID].otherPlayersInMyGame[1]].otherPlayersInMyGame[1] = clients[gameID].otherPlayersInMyGame[0];
						clients[clients[gameID].otherPlayersInMyGame[1]].myGameNumPlayers = 3;
						clients[gameID].inLobby = false;
						clients[clients[gameID].otherPlayersInMyGame[0]].inLobby = false;
						clients[clients[gameID].otherPlayersInMyGame[1]].inLobby = false;
					}
					clients[gameID].myGameIsActive = false;
				}
				else
					break;
			}
	}
	if(message[0] == MSG_CANCEL_GAME)
	{
		int id = (int)message[2];
		if(clients[id].myGameIsActive == true && id == fromID)
		{
			// Client has a game, needs to be removed
			clients[id].myGameIsActive = false;
			clients[id].otherPlayersInMyGame[0] = -1;
			clients[id].otherPlayersInMyGame[1] = -1;
		}
	}
	if(message[0] == MSG_UNJOIN_GAME)
	{
		int id = (int)message[3];
		// Client just wants to un-join a game
		if(clients[id].otherPlayersInMyGame[0] == fromID)
			clients[id].otherPlayersInMyGame[0] = -1;
		else if(clients[id].otherPlayersInMyGame[1] == fromID)
			clients[id].otherPlayersInMyGame[1] = -1;
	}
	if(message[0] == MSG_ENTER_LOBBY)
		clients[fromID].inLobby = true;
	if(message[0] == MSG_EXIT_LOBBY)
		clients[fromID].inLobby = false;
	// If the message is an in-game message only send it to others in same game
	if(message[0] == MSG_GAME_CHAT || message[0] == MSG_MVMT_UPDATE || message[0] == MSG_PLACE_PIECE)
	{
		for(int i = 0; i < clients[fromID].myGameNumPlayers - 1; ++i)
			clients[clients[fromID].otherPlayersInMyGame[i]].sendMessage(message);
		return;
	}
	// If a message makes it here, redistribute it to everyone
	for(int i = 0; i < MAX_PLAYERS; ++i)
		if(clients[i].active && (fromID != i))
			clients[i].sendMessage(message);
} // end handleMessage()

int main(void) {
	cout << "\033[35m=======================================================\n";

	// used for storing results of function calls
	int result;

	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(serverSocket < 0)
	{ 
		cout << "\033[31mError: couldn't create socket";
		cout << "\033[0m";
		exit(-1);
	}

	// Without letting the sockets be reusable, linger time prevents binding on them for
	// a while -- irritating
	int on = 1;
	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof(on));

	// Set up the server address information
	sockaddr_in serverInetInfo;
	serverInetInfo.sin_family      = AF_INET;
	serverInetInfo.sin_addr.s_addr = htonl(INADDR_ANY);
	serverInetInfo.sin_port        = htons(4444);

	// Bind the socket to the serverInetInfo address
	result = bind(serverSocket, (struct sockaddr*)&serverInetInfo, sizeof(serverInetInfo));
	if(result < 0) 
	{
		cout << "\033[31m Error: couldn't bind the socket\n";
		cout << "     : most likely linger time not elapsed\n";
		cout << "\033[0m";
		close(serverSocket);
		exit(-1);
	}

	// Turn the socket on
	result = listen(serverSocket, 3);
	if(result < 0)
	{
		cout << "\033[31mError: couldn't start listening on socket\n";
		cout << "\033[0m";
		close(serverSocket);
		exit(-1);
	}

	pthread_t slt;
	pthread_create(&slt, NULL, serverListeningThread, NULL);

	cout << "\033[35mLibittywhop server started.\n";

	// Wait for user input
	char input;
	while(true)
	{
		// Get the user input and parse the command out
		char command[255];
		char input[32];
		cin.getline(input, 255);

		sscanf(input, "%s32", command);

		if(!strcmp(command, "quit") || !strcmp(command, "q") || !strcmp(command, "exit"))
		{
			// Shut down all client sockets
			for(int i = 0; i < MAX_PLAYERS; ++i)
				if(clients[i].active)
					clients[i].disconnect();
			// Shut down the socket and exit
			close(serverSocket);
			cout << "\033[35mExiting\n\n\033[0m";
			exit(0);
		}
		else if(!strcmp(command, "kick"))
		{
			int id = -1;
			sscanf(input, "%*s %d", &id);
			if(id != -1)
			{
				if(clients[id].active && id >= 0 && id < MAX_PLAYERS)
					clients[id].disconnect();
				else
					cout << "\033[35mNo active player with id: " << id << '\n';
			}
			else
			{
				cout << "\033[35mThat didn't make sense!\n";
				cout << "\033[35mSyntax is: kick [playerID]\n";
			}
		}
		else if (!strcmp(command, "who"))
		{
			bool anyoneActive = false;

			for(int i = 0; i < MAX_PLAYERS; ++i)
				if(clients[i].active)
				{
					anyoneActive = true;
					break;
				}
			if(anyoneActive)
			{
				cout << "\033[35mActive Players:\n";
				for(int i = 0; i < MAX_PLAYERS; ++i)
					if(clients[i].active)
						cout << "\033[35m\tPlayer " << i << " / " << clients[i].name << '\n';
			}
			else
				cout << "\033[35mNo active players.\n";
		}
		else if (!strcmp(command, "help"))
		{
			cout << "\033[35mCommands:\n"
			     << "\thelp        -- List of commands\n"
			     << "\tquit/q/exit -- Shut down server\n"
			     << "\twho         -- List active players\n"
			     << "\tkick [id]   -- Kick a player from server\n";
		}
		else
		{
			cout << "\033[35mCommand: \"" << command << "\" not found\n";
		}
	}
} // end main()
