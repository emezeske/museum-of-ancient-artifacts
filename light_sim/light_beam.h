#include "geometry.h"

class light_beam 
{
	private:
		enum beam_state { INF_LEN, FINITE_LEN };

		line_2d left_side,
				right_side;

		beam_state cur_state;

	public:
		light_beam( void );
		bool is_point_inside( const point_2d &p ) const;
		void draw( void ) const;
}; // end class light_beam
