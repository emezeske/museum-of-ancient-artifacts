#include "SocketClient.h"

extern int errno;

void ConnectionLost(SocketClient* socketClient) {
	printf("Connection to server lost\n");
	socketClient->CloseConnection();
	socketClient->dataQueue.clear();
	pthread_exit(0);
} // end ConnectionLost()

void* ListeningThread(void* ptr) {
	SocketClient* socketClient = (SocketClient*)ptr;

	while(socketClient->active)
	{
		unsigned int messageLen;
		if(!recv(socketClient->socket_no, &messageLen, 4, 0))
			ConnectionLost(socketClient);
		unsigned char messageType;
		if(!recv(socketClient->socket_no, &messageType, 1, 0))
			ConnectionLost(socketClient);

		char* data = new char[messageLen];

		int bytesRead = 0;
		while(bytesRead < messageLen)
		{
			int t;
			if(!(t = recv(socketClient->socket_no, &data[bytesRead], messageLen-bytesRead, 0)))
				ConnectionLost(socketClient);
			bytesRead += t;
		}
		socketClient->HandleIncomingData(messageType, data);
	}
} // end ListeningThread()

void SocketClient::HandleIncomingData(int message_type, char* data) {
	switch(message_type) {
		case MSG_CONNECT_DATA:
			// This message is information for the server about the
			// connection, let the network client deal with it
			break;
		case MSG_GENERIC_DATA:
			// This message is information for the queue
			dataQueue.insert(dataQueue.begin(), (char*)data);
			break;
	} // end switch
} // end HandleIncomingData()

char* SocketClient::GetNextData() {
	char* ptr;
	if(dataQueue.empty())
		ptr = NULL;
	else {
		ptr = *(dataQueue.end()-1);
		dataQueue.pop_back();
	}
	return ptr;
} // end GetNextData()

bool SocketClient::OpenConnection(const char* serv_addr, int port_no) {
	// used for storing results of function calls
	int result;

	socket_no = socket(AF_INET, SOCK_STREAM, 0);
	if(socket < 0) {
		printf("Error: couldn't create socket, errno = %d\n", errno);
		return false;
	}

	// Without letting the sockets be reusable, linger time prevents binding on them for
	// a while -- irritating
	int on = 1;
	setsockopt(socket_no, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	// Set up the server info
	servInetInfo.sin_family = AF_INET;
	servInetInfo.sin_addr.s_addr = inet_addr(serv_addr);
	servInetInfo.sin_port = htons(port_no);

	result = connect(socket_no, (struct sockaddr *)&servInetInfo, sizeof(servInetInfo));

	if(result < 0) {
		close(socket_no);
		return false;
	}
	// Start the listening thread
	active = true;
	pthread_create(&thread, NULL, ListeningThread, this);
	return true;
} // end OpenConnection()

void SocketClient::CloseConnection() {
	active = false;
	close(socket_no);
} // end CloseConnection()

bool SocketClient::SendData(size_t len, void* data) {
	if(!active) {
		printf("Attempted to send data on inactive connection!\n");
		return false;
	}
	unsigned int 	new_len   = len+MSG_DATA_OFFSET;
	unsigned char* 	byte_data = (unsigned char*)data;
	unsigned char* 	byte_len  = (unsigned char*)&len;
	//unsigned char 	net_data[new_len];
	unsigned char* net_data = new (unsigned char)[new_len];
	net_data[MSG_LEN_OFFSET]   = byte_len[0];
	net_data[MSG_LEN_OFFSET+1] = byte_len[1];
	net_data[MSG_LEN_OFFSET+2] = byte_len[2];
	net_data[MSG_LEN_OFFSET+3] = byte_len[3];
	net_data[MSG_TYPE_OFFSET] = (unsigned char)(MSG_GENERIC_DATA);
	for(int i=0; i<len; ++i)
		net_data[i+MSG_DATA_OFFSET] = byte_data[i];
	// send the buffer with the specified size
	for(int totalBytesSent = 0; totalBytesSent < new_len;) {
		int bytesSent = send(socket_no, &net_data[totalBytesSent], new_len - totalBytesSent, 0);

		// if a send ever returns 0 the connection has blown up
		if(bytesSent < 1) {
			printf("Error, sent 0 bytes (connection broken)\n");
			CloseConnection();
			return false;
		}
		totalBytesSent += bytesSent;
	}
	delete [] net_data;
	return true;
} // end SendData()
