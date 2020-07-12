#include <cstdio>
#include <arpa/inet.h>
#include <unistd.h>
#include <list>
#include <pthread.h>
using namespace std;

template<class T> class RequestDistributor {
	public:
	class ProcessRequest {
		public:
		T* req;
		char* target;
		int len;
		ProcessRequest(T* r, char* t, int l) : req(r), target(t), len(l) { }
	};
	// A user function that can break down a type T request into a number
	// of type T requests.  Each smaller request 
	typedef list<ProcessRequest*> req_list;
	typedef req_list*(*RequestSplitter)(ProcessRequest*, int);
	private:
	class NetProcessor {
		private:
		int sock;
		pthread_t worker;
		bool working, connActive;
		ProcessRequest *curReq;
		static void* DoWork(void*);
		public:
		static RequestDistributor* prd;
		NetProcessor(int s);
		void ConnectionLost();
		void ExecuteRequest(ProcessRequest *pr);
	};
	static void* ListenForConnections(void* ptr);
	bool active;
	int sock;
	pthread_t listener;
	// A pointer to the user function that can take a ProcessRequest 
	// and break it into a list of smaller requests
	RequestSplitter split;
	// The list of currently connected NetProcessors
	req_list processors;
	// The list of NetProcessors that failed
	req_list dead_processors;
	public:
	// Constructor must be called with a pointer to a function that can 
	// split a request into any number of requests
	RequestDistributor(RequestSplitter);
	// Start listening for connections on port #p
	bool Start(int p);
	void Stop();
	// Send a request of type T and store the result of the
	// request at target with length len
	void Distribute(ProcessRequest *pr);
};

template<class T>
RequestDistributor<T>::RequestDistributor(RequestSplitter sp) : split(sp) { 
	RequestDistributor<T>::NetProcessor::prd = this;
} // end ctor RequestDistributor()

template<class T>
bool RequestDistributor<T>::Start(int p) {
	// used for storing results of function calls
	int result;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0)
	{ 
		printf("RD: Error: couldn't create socket");
		return false;
	}

	// Without letting the sockets be reusable, linger time prevents binding on them for
	// a while -- irritating
	int on = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	// Set up the server address information
	sockaddr_in serverInetInfo;
	serverInetInfo.sin_family      = AF_INET;
	serverInetInfo.sin_addr.s_addr = htonl(INADDR_ANY);
	serverInetInfo.sin_port        = htons(p);

	// Bind the socket to the serverInetInfo address
	result = bind(sock, (struct sockaddr*)&serverInetInfo, sizeof(serverInetInfo));
	if(result < 0) 
	{
		printf("RD: Error: couldn't bind the socket\n");
		printf("RD: linger time may not have elapsed\n");
		close(sock);
		return false;
	}

	// Turn the socket on
	result = listen(sock, 3);
	if(result < 0)
	{
		printf("RD: Error: couldn't start listening on socket\n");
		close(sock);
		return false;
	}
	active = true;
	pthread_create(&listener, NULL, RequestDistributor::ListenForConnections, this);
	return true;
} // end fnc Start()

template<class T>
void RequestDistributor<T>::Stop() {
	pthread_cancel(listener);
	active = false;
	for(typename list<NetProcessor*>::iterator it = processors.begin(); it != processors.end(); ++it)
		delete *it;
	processors.clear();
	dead_processors.clear();
} // end fnc Stop()

template<class T>
void RequestDistributor<T>::Distribute(ProcessRequest *pr) {
	if(processors.empty())
		return;
	req_list* reqs = split(pr, processors.size());
	typename req_list::iterator req_it = reqs->begin();
	typename list<NetProcessor*>::iterator proc_it = processors.begin();
	while((req_it != reqs->end()) && (proc_it != processors.end()))
		(*proc_it++)->ExecuteRequest(*req_it++);
	reqs->clear();
	delete reqs;
	if(!dead_processors.empty()) {
		req_list failed_reqs;
		while(!dead_processors.empty()) {
			req_list::iterator it = dead_processors.begin();
			failed_reqs.push_back((*it)->curReq);
			dead_processors.erase(it);
		}
		int numFailedReqs = failed_reqs.size();
		int numProcsPerFReq = processors.size() / numFailedReqs;
	}

} // end fnc Distribute()

template<class T>
void* RequestDistributor<T>::ListenForConnections(void* ptr) {
	// Set up a struct for holding client info
	sockaddr_in clientInetInfo;
	unsigned int clientInetInfoLen = sizeof(clientInetInfo);
	RequestDistributor *serv = (RequestDistributor*)ptr;

	while(serv->active) {
		// listen for connection
		int clientSocket = accept(serv->sock, (sockaddr*)&clientInetInfo, &clientInetInfoLen);
		if(clientSocket == -1) {
			printf("RD: Something broke with the accept call\n");
			serv->active = false;
			pthread_exit(0);
		}
		serv->processors.push_back(new NetProcessor(clientSocket));
	}
} // end fnc ListenForConnections()

template<class T> RequestDistributor<T>* RequestDistributor<T>::NetProcessor::prd;

template<class T>
RequestDistributor<T>::NetProcessor::NetProcessor(int s) : sock(s), connActive(true) {
} // end ctor NetProcessor()

template<class T>
void RequestDistributor<T>::NetProcessor::ConnectionLost() {
	working = false;
	connActive = false;
	prd->dead_processors.push_back(this);
} // end fnc ConnectionLost()

template<class T>
void RequestDistributor<T>::NetProcessor::ExecuteRequest(ProcessRequest *pr) {
	working = true;
	curReq = pr;
	pthread_create(&worker, NULL, RequestDistributor::NetProcessor::DoWork, this);
} // end fnc ExecuteRequest()

template<class T>
void* RequestDistributor<T>::NetProcessor::DoWork(void* ptr) {
	RequestDistributor::NetProcessor* self = (RequestDistributor::NetProcessor*)ptr;
	// Send the request
	int len = sizeof(T);
	if(send(self->sock, &len, sizeof(len), 0) < sizeof(len)) {
		self->ConnectionLost();
		pthread_exit(0);
	}
	if(send(self->sock, self->curReq->req, sizeof(T), 0) < sizeof(T)) {
		self->ConnectionLost();
		pthread_exit(0);
	}
	// Wait for all the processed data
	int bytesRead = 0;
	while(bytesRead < self->curReq->len)
	{
		int t;
		if(!(t = recv(self->sock, self->curReq->target+bytesRead, self->curReq->len-bytesRead, 0)))
			self->ConnectionLost();
		bytesRead += t;
	}
	self->working = false;
	delete self->curReq;
} // end fnc DoWork()
