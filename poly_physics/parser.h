#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <map>
#include <cstdio>

using std::string;
using std::vector;
using std::map;

namespace sect_parser
{

struct sect_info
{
	string
		_name,
		_id;

	vector< string > _contents;
};

typedef bool ( *sect_handler )( const sect_info & );

typedef map< string, sect_handler > handler_map;
typedef map< string, sect_handler >::iterator handler_map_iter;

void add_section_handler( string sect_name, sect_handler h );
bool get_next_section( FILE *ifs, sect_info &sect );
bool parse_file( const char *filename );

} // end namespace sect_parser

#endif // PARSER_H
