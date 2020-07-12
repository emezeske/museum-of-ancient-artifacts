#ifndef __SOCKETSERVER_H__
#define __SOCKETSERVER_H__

#include <cstdio>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <pthread.h>
using namespace std;

#define MSG_LEN_OFFSET		0
#define MSG_TYPE_OFFSET		4
#define MSG_DATA_OFFSET		5

#define MSG_GENERIC_DATA	1
#define MSG_CONNECT_DATA	2

#define MAX_CLIENTS 		20

class IncomingData {
	public:
	int client_id;
	char* data;
	IncomingData(int id, char* ptr) : client_id(id), data(ptr) { }
	~IncomingData();
}; // end class IncomingData

typedef void(*CALLBACK)(int);

class SocketServer;

class Client {
	public:
	static SocketServer* serv;
	static CALLBACK new_client_callback;
	static CALLBACK client_lost_callback;
	int id;
	bool active;
	int socket_no;
	pthread_t thread;

	Client() : active(false) { }
	bool OpenConnection(sockaddr_in, int);
	bool CloseConnection();
}; // end class Client

void* ClientListener(void*);

class SocketServer {
	public:
	bool active;
	Client* clients;
	pthread_t thread;
	vector<IncomingData*> dataQueue;
	int socket_no;

	SocketServer();
	~SocketServer();
	bool StartListening(int);
	void StopListening();
	void AddDataToQueue(int, void*);
	bool SendDataToClient(int, size_t, void*);
	void HandleIncomingData(int, int, char*);
	void RegNewClientCallback(CALLBACK);
	void RegClientLostCallback(CALLBACK);
	IncomingData* GetNextData();
}; // end class SocketServer


void* ListenForConnections(void*);

#endif
