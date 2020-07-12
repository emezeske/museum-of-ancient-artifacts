#ifndef EDGE_DETECTOR_H
#define EDGE_DETECTOR_H

#include <vector>

using std::vector;

struct color
{
	unsigned char _r, _g, _b;

	color( unsigned char r, unsigned char g, unsigned char b ) : _r( r ), _g( g ), _b( b ) {}

	color( const unsigned char *buf, unsigned x, unsigned y, unsigned width = CAM_W ) :
		_r( buf[ ( y * width + x ) * 3 + 2 ] ),
		_g( buf[ ( y * width + x ) * 3 + 1 ] ),
		_b( buf[ ( y * width + x ) * 3 + 0 ] )
	{}

	void write_to_buf( unsigned char *buf, unsigned x, unsigned y, unsigned width = CAM_W )
	{
		buf[ ( y * width + x ) * 3 + 2 ] = _r;
		buf[ ( y * width + x ) * 3 + 1 ] = _g;
		buf[ ( y * width + x ) * 3 + 0 ] = _b;
	}

	bool fuzzy_compare( const color &c, const float fuzziness )
	{
 		float 
			t1 = _r - c._r,
			t2 = _g - c._g,
			t3 = _b - c._b;

		return t1 * t1 + t2 * t2 + t3 * t3 < 3 * 255 * 255 * fuzziness;
	}
};

struct point
{
	unsigned
		_x,
		_y;

	point( unsigned x, unsigned y ) : _x( x ), _y( y ) {}
};

class edge_detector
{
	public:
		edge_detector( float threshold ) : _thresh( threshold ) {}
		void set_threshold( float threshold ) { _thresh = threshold; VNOTIFY( VERBOSE, "threshold set to %f", _thresh ); }
		void clear_buffers();
		bool test_pixel( const unsigned char *img, unsigned x, unsigned y, color c );
		void detect( const unsigned char *img, unsigned seed_x, unsigned seed_y );
		void draw_debug( unsigned char *img );

	private:
		float _thresh;

		vector< point > seeds;

		bool
			been_checked[ CAM_W ][ CAM_H ],
			is_edge[ CAM_W ][ CAM_H ];
};

#endif // EDGE_DETECTOR_H
