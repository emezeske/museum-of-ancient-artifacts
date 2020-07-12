#include "reqdist.h"

class RequestType {
	public:
	int top, bottom;
	RequestType(int t, int b) : top(t), bottom(b) { }
}; 

template<class T>
typename RequestDistributor<T>::req_list* SplitRequest(typename RequestDistributor<T>::ProcessRequest* prq, int num) {
	typename RequestDistributor<T>::req_list* reqs = new typename RequestDistributor<T>::req_list;
	reqs->push_back(prq);
	return reqs;
} // end fnc SplitRequest()

int main(void) {
	typedef RequestDistributor<RequestType> RD;

	RD::RequestSplitter rs = SplitRequest<RequestType>;
	RD* rd = new RD(rs);
	if(!rd->Start(4343))
		printf("RequestDistributer Start() failed\n");

	const int BUF_SIZE = 10000;
	char* buf = new char[BUF_SIZE];
	RD::ProcessRequest* pr = new RD::ProcessRequest(new RequestType(100, 100), buf, BUF_SIZE);
	rd->Distribute(pr);

	rd->Stop();
	return 0;
}
