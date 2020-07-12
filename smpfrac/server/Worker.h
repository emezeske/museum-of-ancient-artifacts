#ifndef __WORKER_H__
#define __WORKER_H__

#include "SocketServer.h"

struct FractalBuildRequest {
	int max_iters;
	int color_meth;
	int sample_block_w;	
	double pal_offset;
	double sp_oversample;
	int pxW;
	int pxH;
	double cmplxX;
	double cmplxY;
	double cmplxW;
	double cmplxH;
}; // end struct FractalBuildRequest

class Worker {
	public:
	FractalBuildRequest cur_req;
	int client_no;
	int posX, posY;

	Worker(int c) : client_no(c) { }
}; // end class Worker

#endif
