#include <errno.h>
#include <string.h>

#include "utilities.h"
#include "parser.h"

namespace sect_parser
{

handler_map user_handlers;

void add_section_handler( string sect_name, sect_handler h )
{
	user_handlers[ sect_name ] = h;
}

bool get_next_section( FILE *ifs, sect_info &sect )
{
	bool found_section = false;
	char line[ 255 ];

	sect._contents.clear();

	while ( !feof( ifs ) && ( fgets( line, sizeof( line ), ifs ) != NULL ) )
	{
		char sec_name[ 32 ];
		char sec_id[ 32 ];

		char *first_char = line;
		while ( isspace( *first_char ) ) ++first_char;

		if ( *first_char != '\0' )
		{
			if ( *first_char != '#' )
			{
				int nl = strlen( line ) - 1;
				if ( line[ nl ] == '\n' ) line[ nl ] = '\0';

				if ( found_section )
				{
					if ( *first_char != '}' )
					{
						sect._contents.push_back( first_char );
					}
					else break;
				}
				else if ( sscanf( line, " section: %31s %31s { ", sec_name, sec_id ) == 2 )
				{
					sect._name = sec_name;
					sect._id = sec_id;
					found_section = true;
				}
				else VNOTIFY( INFO, "Discarding tween-section data: '%s'.", first_char );
			}
		}
	}

	return found_section;
}

bool parse_file( const char *filename )
{
	bool ok = false;

	FILE *ifs;

	if ( ( ifs = fopen( filename, "r" ) ) != NULL )
	{
		ok = true;

		sect_info sect;
		
		while ( get_next_section( ifs, sect ) )
		{
			if ( user_handlers.find( sect._name ) != user_handlers.end() )
			{
				( user_handlers[ sect._name ] )( sect );
			}
			else VNOTIFY( INFO, "Discarding unknown section: %s", sect._name.c_str() );
		}

		fclose( ifs );
	}
	else VNOTIFY( FAULT, "Error opening file '%s': %s", filename, strerror( errno ) );

	return ok;
}

} // end namespace sect_parser
