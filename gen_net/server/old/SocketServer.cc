#include "SocketServer.h"

SocketServer* Client::serv = NULL;
CALLBACK Client::new_client_callback = NULL;
CALLBACK Client::client_lost_callback = NULL;

IncomingData::~IncomingData() {
	delete [] data;
} // end deconstructor IncomingData()

bool Client::OpenConnection(sockaddr_in addr, int socket) {
	socket_no = socket;
	printf("New client on socket_no: %d\n", socket_no);
	active = true;
	pthread_create(&thread, NULL, ClientListener, (void*)this);
	pthread_detach(thread);
	if(new_client_callback)
		new_client_callback(id);
	return true;
} // end OpenConnection()

bool Client::CloseConnection() {
	active = false;
	close(socket_no);
	if(client_lost_callback)
		client_lost_callback(id);
	return true;
} // end CloseConnection()

void ConnectionLost(Client* client) {
	client->CloseConnection();
	pthread_exit(0);
} // end ConnectionLost()

void* ClientListener(void* client_ptr) {
	Client* self = (Client*)client_ptr;

	while(self->active)
	{
		unsigned int messageLen;
		if(!recv(self->socket_no, &messageLen, 4, 0))
			ConnectionLost(self);
		unsigned char messageType;
		if(!recv(self->socket_no, &messageType, 1, 0))
			ConnectionLost(self);

		char* data = new char[messageLen];

		int bytesRead = 0;
		while(bytesRead < messageLen)
		{
			int t;
			if(!(t = recv(self->socket_no, &data[bytesRead], messageLen-bytesRead, 0)))
				ConnectionLost(self);
			bytesRead += t;
		}
		self->serv->HandleIncomingData(messageType, self->id, data);
	}
} // end ClientListener()

void* ListenForConnections(void* ptr) {
	// Set up a struct for holding client info
	sockaddr_in clientInetInfo;
	unsigned int clientInetInfoLen = sizeof(clientInetInfo);
	SocketServer *serv = (SocketServer*)ptr;

	while(serv->active) {
		// listen for connection
		int clientSocket = accept(serv->socket_no, (sockaddr*)&clientInetInfo, &clientInetInfoLen);
		if(clientSocket == -1) {
			printf("Something broke with the accept call\n");
			pthread_exit(0);
		}
		
		// find a free ID number to assign to client
		int freeID = 0;
		for(; freeID < MAX_CLIENTS; ++freeID)
		{
			if(!serv->clients[freeID].active)
				break;
		}
		// server is full -- do something
		if(freeID == MAX_CLIENTS)
		{
			printf("Server is full\n");
			continue;
		}
		else
		{
			// Create new client thread and set to active
			serv->clients[freeID].OpenConnection(clientInetInfo, clientSocket);
		}
	}
} // end serverListeningThread()

SocketServer::SocketServer(void) {
	// Initialize clients
	clients = new Client[MAX_CLIENTS];
	Client::serv = this;
	for(int i=0; i<MAX_CLIENTS; ++i) 
		clients[i].id = i;
} // end constructor SocketServer()

SocketServer::~SocketServer(void) {
	delete [] clients;
} // end deconstructor SocketServer()

void SocketServer::RegNewClientCallback(CALLBACK fnc_ptr) {
	Client::new_client_callback = fnc_ptr;
} // end RegNewClientCallback()

void SocketServer::RegClientLostCallback(CALLBACK fnc_ptr) {
	Client::client_lost_callback = fnc_ptr;
} // end RegClientLostCallback()

void SocketServer::HandleIncomingData(int message_type, int client_id, char* data) {
	switch(message_type) {
		case MSG_CONNECT_DATA:
			// This message is information for the server about the
			// connection, let the server deal with it
			break;
		case MSG_GENERIC_DATA:
			// This message is information for the queue
			dataQueue.insert(dataQueue.begin(), new IncomingData(client_id, data));
			break;
	} // end switch
} // end HandleIncomingData()

bool SocketServer::StartListening(int port_no) {
	// used for storing results of function calls
	int result;

	socket_no = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_no < 0)
	{ 
		printf("Error: couldn't create socket");
		return false;
	}

	// Without letting the sockets be reusable, linger time prevents binding on them for
	// a while -- irritating
	int on = 1;
	setsockopt(socket_no, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	// Set up the server address information
	sockaddr_in serverInetInfo;
	serverInetInfo.sin_family      = AF_INET;
	serverInetInfo.sin_addr.s_addr = htonl(INADDR_ANY);
	serverInetInfo.sin_port        = htons(port_no);

	// Bind the socket to the serverInetInfo address
	result = bind(socket_no, (struct sockaddr*)&serverInetInfo, sizeof(serverInetInfo));
	if(result < 0) 
	{
		printf("Error: couldn't bind the socket\n");
		printf("linger time may not have elapsed\n");
		close(socket_no);
		return false;
	}

	// Turn the socket on
	result = listen(socket_no, 3);
	if(result < 0)
	{
		printf("Error: couldn't start listening on socket\n");
		close(socket_no);
		return false;
	}

	active = true;
	pthread_create(&thread, NULL, ListenForConnections, this);
	pthread_detach(thread);
	return true;
} // end constructor StartListening()

void SocketServer::StopListening() {
	active = false;
	for(int i=0; i<MAX_CLIENTS; ++i)
		if(clients[i].active)
			clients[i].CloseConnection();
} // end StopListening()

IncomingData* SocketServer::GetNextData() {
	IncomingData* inc_ptr;
	if(dataQueue.empty())
		inc_ptr = NULL;
	else {
		inc_ptr = *(dataQueue.end()-1);
		dataQueue.pop_back();
	}
	return inc_ptr;
} // end GetNextData()

bool SocketServer::SendDataToClient(int id, size_t len, void* data) {
	if(!clients[id].active) {
		printf("Attempted to send bytes to id #%d, but they aren't active!\n", id);
		return false;
	}
	unsigned int 	new_len   = len+MSG_DATA_OFFSET;
	unsigned char* 	byte_data = (unsigned char*)data;
	unsigned char* 	byte_len  = (unsigned char*)&len;
	unsigned char 	net_data[new_len];
	net_data[MSG_LEN_OFFSET]   = byte_len[0];
	net_data[MSG_LEN_OFFSET+1] = byte_len[1];
	net_data[MSG_LEN_OFFSET+2] = byte_len[2];
	net_data[MSG_LEN_OFFSET+3] = byte_len[3];
	net_data[MSG_TYPE_OFFSET]  = (unsigned char)(MSG_GENERIC_DATA);
	for(int i=0; i<len; ++i)
		net_data[i+MSG_DATA_OFFSET] = byte_data[i];
	// send the buffer with the specified size
	for(int totalBytesSent = 0; totalBytesSent < new_len;)
	{
		int bytesSent = send(clients[id].socket_no, &net_data[totalBytesSent], new_len - totalBytesSent, 0);

		// if a send ever returns 0 the connection has blown up
		if(bytesSent < 1)
		{
			printf("Error, sent 0 bytes (connection broken)\n");
			clients[id].CloseConnection();
			return false;
		}
		totalBytesSent += bytesSent;
	}
	return true;
} // end SendDataToClient()
