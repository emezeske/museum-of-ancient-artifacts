#include "utilities.h"
#include "globals.h"
#include "edge_detector.h"

void edge_detector::clear_buffers()
{
	for ( unsigned i = 0; i < CAM_W; ++i ) memset( &is_edge[ i ][ 0 ], 0, CAM_H );
	for ( unsigned i = 0; i < CAM_W; ++i ) memset( &been_checked[ i ][ 0 ], 0, CAM_H );
}

bool edge_detector::test_pixel( const unsigned char *img, unsigned x, unsigned y, color c )
{
	bool check_passed = false;

	if ( !been_checked[ x ][ y ] )
	{
		if ( c.fuzzy_compare( color( img, x, y ), _thresh ) )
		{
			check_passed = true;
			been_checked[ x ][ y ] = true;
		
			unsigned up_y = y + 1;
		
			if ( up_y < CAM_H )
			{
				if ( !been_checked[ x ][ up_y ] )
				{
					if ( c.fuzzy_compare( color( img, x, up_y ), _thresh ) ) 
					{
						seeds.push_back( point( x, up_y ) );
					}
					else is_edge[ x ][ up_y ] = true;
				}
			}
			else is_edge[ x ][ y ] = true;
			
			if ( y > 0 )
			{
				unsigned dn_y = y - 1;
		
				if ( !been_checked[ x ][ dn_y ] )
				{
					if ( c.fuzzy_compare( color( img, x, dn_y ), _thresh ) )
					{
						seeds.push_back( point( x, dn_y ) );
					}
					else is_edge[ x ][ dn_y ] = true;
				}
			}
			else is_edge[ x ][ y ] = true;
		}
		else is_edge[ x ][ y ] = true;

		if ( x == 0 || x == CAM_W - 1 ) is_edge[ x ][ y ] = true;
	}

	return check_passed;
}

void edge_detector::detect( const unsigned char *img, unsigned seed_x, unsigned seed_y )
{
	clear_buffers();
	seeds.clear();

	seeds.push_back( point( seed_x, seed_y ) );
	color cur_color( img, seed_x, seed_y );

	while( !seeds.empty() )
	{
		point s = seeds.back();
		seeds.pop_back();

		for ( unsigned x = s._x, y = s._y; x < CAM_W; ++x )
		{
			if ( !test_pixel( img, x, y, cur_color ) ) break;
		}

		for ( unsigned x = s._x - 1, y = s._y; x != 0; --x )
		{
			if ( !test_pixel( img, x, y, cur_color ) ) break;
		}
	}
}

void edge_detector::draw_debug( unsigned char *img )
{
	for ( unsigned x = 0; x < CAM_W; ++x )
	{
		for ( unsigned y = 0; y < CAM_H; ++y )
		{
			if ( is_edge[ x ][ y ] ) color( 255, 0, 0 ).write_to_buf( img, x, y );
			else color( 1, 0, 0 ).write_to_buf( img, x, y );
		}
	}
}
