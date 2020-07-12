#include <GL/glut.h>

#include "utilities.h"
#include "world_obj.h"

world_obj::world_obj( const vector< string > &sv ) : 
	physics_obj()
{
	bool ok = false;
	unsigned i = 0;

	if ( sscanf( sv[ i++ ].c_str(), " %f %f %f", &(_pos._x), &(_pos._y), &_rot ) == 3 )
	{
		if ( sscanf( sv[ i++ ].c_str(), " %f %f %f", &(_vel._x), &(_vel._y), &_vrot ) == 3 )
		{
			if ( sscanf( sv[ i++ ].c_str(), " %f %f %f", &(_acc._x), &(_acc._y), &_racc ) == 3 )
			{
				if ( sscanf( sv[ i++ ].c_str(), " %f", &_mass ) == 1 )
				{
					ok = true;

					for ( ;i < sv.size(); ++i )
					{
						wu_t
							vx,
							vy;

						if ( sscanf( sv[ i ].c_str(), " %f %f", &vx, &vy ) == 2 )
						{
							_shape.add_vertex( vec_2d( vx, vy ) );
						}
						else
						{
							ok = false;
							VNOTIFY( FAULT, "Error parsing vertex: '%s', should be 'xcomp ycomp'.", sv[ i ].c_str() );
							break;
						}
					}

					if ( ok ) _shape.precalculate();
					precalculate();
				}
				else VNOTIFY( FAULT, "Error parsing mass '%s', should be 'mass'.", sv[ --i ].c_str() );
			}
			else VNOTIFY( FAULT, "Error parsing acceleration '%s', should be 'ax ay arot'.", sv[ --i ].c_str() );
		}
		else VNOTIFY( FAULT, "Error parsing velocity '%s', should be 'vx vy vrot'.", sv[ --i ].c_str() );
	} 
	else VNOTIFY( FAULT, "Error parsing position '%s', should be 'xpos ypos rot'.", sv[ --i ].c_str() );

	if ( !ok ) ; // Throw exception
}

/* virtual */ void world_obj::draw()
{
	glPushMatrix();
	glColor3f( 0.9, 0.1, 0.6 );
	glTranslatef( _pos._x, _pos._y, 0.0 );
	glRotatef( _rot * geom::deg_per_rad, 0.0, 0.0, 1.0 );
	glBegin( GL_LINE_LOOP );
	
	ray_2d_vector_iter
		edge_i = _shape._edges.begin(),
		edge_end = _shape._edges.end();
	
	for ( ; edge_i != edge_end; ++edge_i )
	{
		vec_2d &p = ( *edge_i )._origin;
		glVertex2f( p._x, p._y );
	}
	
	glEnd();

	glPointSize( 2 );

	glBegin( GL_POINTS );
		glVertex2d( 0.0, 0.0 );
	glEnd();

	glPopMatrix();

	//glPushMatrix();
	//glColor3f( 1.0, 0.5, 0.1 );
	//glBegin( GL_LINE_LOOP );
	//	glVertex2d( _bb._ll._x, _bb._ll._y );
	//	glVertex2d( _bb._ll._x, _bb._ur._y );
	//	glVertex2d( _bb._ur._x, _bb._ur._y );
	//	glVertex2d( _bb._ur._x, _bb._ll._y );
	//glEnd();
	//glPopMatrix();
}
