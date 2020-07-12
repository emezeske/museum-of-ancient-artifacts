#include <cstdlib>
#include <cstdio>
#include <math.h>
#include <unistd.h>

#include "SocketClient.h"

using namespace std;

#define TRAP_RADIUS	0
#define NUM_ITERS	1
#define TRAP_ANGLE	2

struct FRGBClr {
	double r, g, b;
}; 

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

int MAX_ITERS;
const double NOT_TRAPPED = 99999;
SocketClient *client;

double getTrapRadSq(double cr, double ci) {
	double zr, zi;
	zr = cr; 
        zi = ci;
	double radiussq = NOT_TRAPPED;
	for(int i=0; i<MAX_ITERS; ++i) {
		double 	zrsq = zr*zr,
			zisq = zi*zi,
			zizr = zi*zr,
			zizrt2 = zizr + zizr;
		zr = zrsq - zisq + cr;
		zi = zizrt2 + ci;
		double zrsqzisq = zrsq + zisq;
		if(zrsqzisq < 1.0) {
			radiussq  = zrsqzisq;
		} else if(zrsqzisq > 4.0)
			break;
	}
	return radiussq;
} // end getTrapRadSq()

double getTrapAngle(double cr, double ci) {
	double zr, zi, tr, ti;
	bool trapped = false;
	zr = cr; 
        zi = ci;
	for(int i=0; i<MAX_ITERS; ++i) {
		double 	zrsq = zr*zr,
			zisq = zi*zi,
			zizr = zi*zr,
			zizrt2 = zizr + zizr;
		zr = zrsq - zisq + cr;
		zi = zizrt2 + ci;
		double zrsqzisq = zrsq + zisq;
		if(zrsqzisq < 1.0) {
			trapped = true;
			tr = zr;
			ti = zi;
		} else if(zrsqzisq > 4.0)
			break;
	}
	if(trapped)
		return atan2(ti, tr);
	else
		return NOT_TRAPPED;
} // end getTrapRadSq()

int getNumIters(double cr, double ci) {
	double zr, zi;
	zr = cr; 
        zi = ci;
	int i=0;
	for(; i<MAX_ITERS; ++i) {
		double 	zrsq = zr*zr,
			zisq = zi*zi,
			zizr = zi*zr,
			zizrt2 = zizr + zizr;
		zr = zrsq - zisq + cr;
		zi = zizrt2 + ci;
		if(zrsq+zisq > 4.0)
			break;
	}
	return i;
} // end getColorAt()

unsigned char* buildFractal(	int color_meth,
				int sample_block_w,	
				double pal_offset,
				double sp_oversample,
				int pxW,
				int pxH,
				double cmplxX,
				double cmplxY,
				double cmplxW,
				double cmplxH) {
	double COLOR_CONV = (double)255/((double)sample_block_w*(double)sample_block_w);
	double totr, totg, totb;
	double lower_bound = 0.0 - sp_oversample,
	       upper_bound = 1.0 + sp_oversample,
	       total_bounds = 1.0 + 2.0 * sp_oversample;
	double cr, ci;
	double 	pixel_plane_ratio_x = 1/(double)pxW*cmplxW,
		pixel_plane_ratio_y = 1/(double)pxH*cmplxH;
	unsigned char *pixels = new (unsigned char)[pxW*pxH*3];
	for(int x=0; x<pxW; ++x)
		for(int y=0; y<pxH; ++y) {
			totr = totg = totb = 0;
			for(double fx=lower_bound; fx<upper_bound; fx+=total_bounds/(double)sample_block_w)
				for(double fy=lower_bound; fy<upper_bound; fy+=total_bounds/(double)sample_block_w) {
					cr = cmplxX + ((double)x+fx)*pixel_plane_ratio_x; 
        				ci = cmplxY + ((double)y+fy)*pixel_plane_ratio_y;
					switch(color_meth) {
						case TRAP_RADIUS:
							{
								double rsq = getTrapRadSq(cr, ci);
								if(rsq != NOT_TRAPPED) {
									totr += fabs(sin(rsq*4.1 + pal_offset));
									totg += 0;//fabs(sin(rsq + pal_offset));
									totb += fabs(sin(rsq*1.5 + pal_offset));
								} else {
									totr += 0;
									totg += 0;
									totb += 0;
								}
								break;
							}
						case NUM_ITERS:
							{
								int i = getNumIters(cr, ci);
								if(i < MAX_ITERS-1) {
									totr += fabs(sin(0.032*M_PI*i + pal_offset));
									totg += 0;//fabs(sin(15*radiussq + pal_offset));
									totb += fabs(cos(0.0555*M_PI*i + pal_offset));
								} else {
									totr += 0;
									totg += 0;
									totb += 0;
								}
								break;
							}
						case TRAP_ANGLE:
							{
								double ang = getTrapAngle(cr,ci);
								if(ang != NOT_TRAPPED) {
									totr += fabs(sin(4*ang + pal_offset));
									totg += 0;//fabs(sin(rsq*rsq + pal_offset));
									totb += fabs(sin(7*ang + pal_offset));
								} else {
									totr += 0;
									totg += 0;
									totb += 0;
								}
								break;
							}
					} // end switch
				}
			pixels[y*pxW*3+3*x]   = (unsigned char)((totr)*COLOR_CONV);
			pixels[y*pxW*3+3*x+1] = (unsigned char)((totg)*COLOR_CONV);
			pixels[y*pxW*3+3*x+2] = (unsigned char)((totb)*COLOR_CONV);
		}
	return pixels;
} // end buildFractal()

void processFractalBuildRequests() {
	FractalBuildRequest* fbr;
	while(client->active) {
		// receive data from server
		fbr = (FractalBuildRequest*)client->GetNextData();
		if(fbr != NULL) {
			MAX_ITERS = fbr->max_iters;
			unsigned char* image = buildFractal(	fbr->color_meth,
								fbr->sample_block_w,	
								fbr->pal_offset,
								fbr->sp_oversample,
								fbr->pxW,
								fbr->pxH,
								fbr->cmplxX,
								fbr->cmplxY,
								fbr->cmplxW,
								fbr->cmplxH);
			delete [] fbr;
			client->SendData(fbr->pxW*fbr->pxH*3, (void*)image);
			delete []image;
		}
	}
} // end processFractalBuildRequests()

int main(int argc, char **argv) {
	client = new SocketClient;
	while(true) {
		// Attempt to connect to the server once every second
		while(!client->OpenConnection("127.0.0.1", 4444))
			usleep(1000);
		printf("Connected to server!\n");
		processFractalBuildRequests();
	}
	return 0;
} // end function main()
