#include "ltga.h"
#include "libtarga.h"
#include "vec3d.h"
#include <cmath>
using namespace std;

int main(int argc, char** argv) {
	if(argc < 2) {
		printf("Usage:  normalizebm <filename>\n");
		exit(0);
	}
	LTGA l(argv[1]);
	bool hasAlpha = l.GetAlphaDepth() == 8;
	int bpp = (hasAlpha) ? 4 : 3;
	int w = l.GetImageWidth(),
	    h = l.GetImageHeight();
	unsigned char *pixels = l.GetPixels();

	for(unsigned char* cur_pix = pixels; cur_pix < pixels + w*h*bpp; cur_pix +=bpp) {
		Vec3D v;
		v.x = 2*((float)cur_pix[0]/255*2.0 - 1.0);
		v.y = 2*((float)cur_pix[1]/255*2.0 - 1.0);
		v.z = 2*((float)cur_pix[2]/255*2.0 - 1.0);
		//printf("%d, %d, %d, %f, %f, %f\n", cur_pix[0], cur_pix[1], cur_pix[2], v.x, v.y, v.z);
		v.Normalize();
		cur_pix[0] = (unsigned char)((v.x+1.0)*255/2);
		cur_pix[1] = (unsigned char)((v.y+1.0)*255/2);
		cur_pix[2] = (unsigned char)((v.z+1.0)*255/2);
	}

	int format = (hasAlpha) ? TGA_TRUECOLOR_32 : TGA_TRUECOLOR_24;
	int len = strlen(argv[1]);
	char* fname = new char[len + 4];
	strncpy(fname, argv[1], len-3);
	strcat(fname, "NORM.tga");
	tga_write_raw(fname, w, h, pixels, format);
}
