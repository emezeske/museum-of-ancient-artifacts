#include "fracterrain.h"

#include <cstdlib>
#include <assert.h>
#include <cstdio>
#include <cstring>
#include <math.h>
#include <GL/glut.h>

using std::printf;
using std::rand;
using std::srand;
using std::memset;

Fracterrain::Fracterrain( 	unsigned seed /* = 1 */,
							float jitter /* = 1.0 */, float roughness /* = 0.5 */ )
{
	generate( seed, jitter, roughness );
} // end ctor Fracterrain()

void Fracterrain::draw( void )
{
	//glEnable( GL_TEXTURE_2D );
	glPushMatrix();

	glTranslatef( -( float )TER_W / 2, -( float )TER_W / 2, 0 );

	//glBindTexture( GL_TEXTURE_2D, tex );

	for ( unsigned x = 0; x + 1 < TER_W; ++x )
	{
		glBegin( GL_TRIANGLE_STRIP );

		for ( unsigned y = 0; y < TER_W; ++y )
		{
			glColor3f( fabs( hmap[x][y] ) / 5.0, 0.0, 0.0 );
			glVertex3f( x, y, hmap[x][y] );

			glColor3f( fabs( hmap[x + 1][y] ) / 5.0, 0.0, 0.0 );
			glVertex3f( x + 1, y, hmap[x + 1][y] );
		}

		glEnd();
	}

	glPopMatrix();
} // end fnc draw()

inline float Fracterrain::get_random_height( float jitter )
{
	return ( jitter * 2 * ( ( ( float )rand() / RAND_MAX ) - 0.5 ) );
}

void Fracterrain::generate( unsigned seed, float jitter, float roughness )
{
	for ( unsigned x = 0; x < TER_W; ++x )
		for ( unsigned y = 0; y < TER_W; ++y )
			hmap[x][y] = 0.0;

	srand( seed );

	for ( unsigned ival = TER_W / 2; ival > 1; ival /= 2, jitter *= roughness )
		for ( unsigned x = 0; x + ival < TER_W; x += ival )
			for ( unsigned y = 0; y + ival < TER_W; y += ival )
				do_one_square( jitter, x, y, x + ival, y + ival );
}

void Fracterrain::do_one_square( 	float jitter,
									unsigned lX, unsigned lY,
									unsigned uX, unsigned uY )
{
	int dX = uX - lX,
		dY = uY - lY;

	int cX = lX + dX / 2,
		cY = lY + dY / 2;
	
	float avg_h;
	
	avg_h = ( hmap[lX][lY] + hmap[uX][lY] + hmap[lX][uY] + hmap[uX][uY] ) / 4;
	hmap[cX][cY] = avg_h + get_random_height( jitter );
	
	avg_h = ( hmap[lX][lY] + hmap[uX][lY] ) / 2;
	hmap[cX][lY] = avg_h + get_random_height( jitter );
	
	avg_h = ( hmap[uX][lY] + hmap[uX][uY] ) / 2;
	hmap[uX][cY] = avg_h + get_random_height( jitter );
	
	avg_h = ( hmap[uX][uY] + hmap[lX][uY] ) / 2;
	hmap[cX][uY] = avg_h + get_random_height( jitter );
	
	avg_h = ( hmap[lX][uY] + hmap[lX][lY] ) / 2;
	hmap[lX][cY] = avg_h + get_random_height( jitter );
} // end fnc do_one_square()
