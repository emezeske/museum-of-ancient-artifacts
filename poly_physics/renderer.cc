#include "renderer.h"

#include <algorithm>

using std::find;

void renderer::draw_all()
{
	for ( drawable_vec_iter i = _draw_list.begin(); i != _draw_list.end(); ++i )
		( *i )->draw();
}

void renderer::add_drawable( drawable *d )
{
	_draw_list.push_back( d );
}

void renderer::rm_drawable( drawable *d )
{
	drawable_vec_iter i = find( _draw_list.begin(), _draw_list.end(), d );
	if( i != _draw_list.end() ) _draw_list.erase( i );
}
