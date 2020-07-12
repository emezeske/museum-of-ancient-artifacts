#ifndef __SOCKETCLIENT_H__
#define __SOCKETCLIENT_H__

#include <arpa/inet.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <vector>

#define RECV_BUFFER_SIZE	1024

#define MSG_LEN_OFFSET		0
#define MSG_TYPE_OFFSET		4
#define MSG_DATA_OFFSET		5

#define MSG_GENERIC_DATA	1
#define MSG_CONNECT_DATA	2

using namespace std;

class SocketClient {
	public:
	int socket_no;
	bool active;
	sockaddr_in servInetInfo;
	vector<char*> dataQueue;
	pthread_t thread;

	bool OpenConnection(const char*, int);
	void CloseConnection();
	bool SendData(size_t, void*);
	char* GetNextData();
	void HandleIncomingData(int, char*);
}; // end class SocketClient

void* ListeningThread(void*);

#endif
