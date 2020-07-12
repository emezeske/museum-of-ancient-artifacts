#include "flame.h"

Flame::Flame(int w, int h, int t, GLuint tex) {
	W = w;
	H = h;
	updateRate = t;
	texture = tex;
	timeAccum = 0;
	firing = false;
	innerColor = 0x39C4EC00;
	outerColor = 0x0712A300;

	pixels = new unsigned int[W*H];
	memset(pixels, 0, W*H*sizeof(int));

	glBindTexture(GL_TEXTURE_2D, texture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
	glTexImage2D (GL_TEXTURE_2D,
		      0,
		      GL_RGBA,
		      W,
		      H,
		      0,
		      GL_RGBA,
		      GL_UNSIGNED_INT_8_8_8_8,
		      pixels);
} // end constructor Flame()

void Flame::ChangeColor(int inner, int outer) {
	innerColor = inner;
	outerColor = outer;
} // end ChangeColor()

void Flame::BeginFiring(void) {
	firing = true;
} // end BeginFiring()

void Flame::EndFiring(void) {
	firing = false;
} // end EndFiring()

void Flame::Update(int delta) {
	timeAccum += delta;
	if(timeAccum < updateRate)
		return;
	timeAccum = 0;
	// Seed the random number generator
	srand(glutGet(GLUT_ELAPSED_TIME));
	// Seed the bottom row of pixels with new values
	int r1 = 0, r2 = 0;
	if(firing)
		for(int x=0; x<W;) {
			int r = rand();	
			int mask = 1;
			for(int j=1; mask<RAND_MAX;) {
				if(r & mask) {
					if( (x > W/3) && (x < 2*W/3) )
						pixels[x]   = ~(0x0000006F|innerColor);
					else
						pixels[x]   = ~(0x0000006F|outerColor);
					r1++;
				}
				else 
				{
					if( (x > W/3) && (x < 2*W/3) )
						pixels[x]   = ~(0x0000000F|innerColor);
					else
						pixels[x]   = ~(0x0000000F|outerColor);
					r2++;
				}
				++x;
				++j;
				mask = (int)pow(2, j);
				if(x == W)
					break;
			} // end for
		} // end for
	else
		for(int x=0; x<W; ++x)
			pixels[x] = 0;
	// Raise the flame
	for(int x=0; x<W; ++x)
		for(int y=1; y<H; ++y) {
			// Figure out the indices for interpolation, if the indices
			// are out of bounds assign them -1 as a marker
			int 	i1 = (x == 0) ? -1 : x-1+(y-1)*W,
				i2 = x+(y-1)*W,
				i3 = (x == W-1) ? -1 : x+1+(y-1)*W, 
				i4 = (y == 1) ? i2 : x+(y-2)*W;

			int alpha = 0;
			alpha += (i1 == -1) ? 0 : pixels[i1] & 0x000000FF;
			alpha += (i2 == -1) ? 0 : pixels[i2] & 0x000000FF;
			alpha += (i3 == -1) ? 0 : pixels[i3] & 0x000000FF;
			alpha /= 3;
			if(alpha > 0)
				alpha -= (int)((float)255 / (float)H);
			if(alpha < 0)
				alpha = 0;
			int r = 0;
			r += (i1 == -1) ? 0 : (pixels[i1] & 0xFF000000) >> 24;
			r += (i2 == -1) ? 0 : (pixels[i2] & 0xFF000000) >> 24;
			r += (i3 == -1) ? 0 : (pixels[i3] & 0xFF000000) >> 24;
			r /= 3;
			int g = 0;
			g += (i1 == -1) ? 0 : (pixels[i1] & 0x00FF0000) >> 16;
			g += (i2 == -1) ? 0 : (pixels[i2] & 0x00FF0000) >> 16;
			g += (i3 == -1) ? 0 : (pixels[i3] & 0x00FF0000) >> 16;
			g /= 3;
			int b = 0;
			b += (i1 == -1) ? 0 : (pixels[i1] & 0x0000FF00) >> 8;
			b += (i2 == -1) ? 0 : (pixels[i2] & 0x0000FF00) >> 8;
			b += (i3 == -1) ? 0 : (pixels[i3] & 0x0000FF00) >> 8;
			b /= 3;

			pixels[x+y*W] = (alpha) | (r << 24) | (g << 16) | (b << 8);
		} // end for

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D (GL_TEXTURE_2D,
		      0,
		      GL_RGBA,
		      W,
		      H,
		      0,
		      GL_RGBA,
		      GL_UNSIGNED_INT_8_8_8_8,
		      pixels);
} // end AnimateThruster()
