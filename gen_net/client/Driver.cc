#include "SocketClient.h"
#include <iostream>
using namespace std;

int main(void) {
	SocketClient *client = new SocketClient;
	if(!client->OpenConnection("127.0.0.1", 4444))
		return -1;
	char h;
	while(true) {
		/*
		// receive data from server
		char* ptr;
		if(ptr = client->GetNextData()) {
			delete [] ptr;
		}
		*/
		// send data to server
		char *str = new char[512*512*3];
		memset(str, 66, 512*512*3);
		if(rand()%1000000 == 500) {
			client->SendData(512*512*3, str);
		}
	}
	return 0;
}
