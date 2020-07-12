#include "SocketServer.h"
#include <iostream>
using namespace std;

void newClient(int client_num) {
	printf("newClient was called with client_num %d\n", client_num);
} // end newClient

void clientLost(int client_num) {
	printf("clientLost was called with client_num %d\n", client_num);
} // end newClient

int main(void) {
	SocketServer *serv = new SocketServer();
	if(!serv->StartListening(4444)) {
		printf("Failed to start server\n");
		return -1;
	}
	serv->RegNewClientCallback(newClient);
	serv->RegClientLostCallback(clientLost);

	IncomingData* data;
	while(true) {
		// Send data to client
		// Receive data from top of queue
		IncomingData* ptr;
		if(ptr = serv->GetNextData()) {
			delete ptr;
		}
	}
	return 0;
} 
