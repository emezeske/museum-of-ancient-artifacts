#include <boost/program_options.hpp>
#include <iostream>
#include <fstream>

#include "configuration.h"

using namespace std;

namespace po = boost::program_options;

bool configuration::parse_options( int ac, char **av )
{
	bool ok = false;

    try
	{
        po::options_description command_line( "Command line options" );
        command_line.add_options()
            ("help", "produce help message")
			;
    
        po::options_description config_file( "Configuration file options" );
        config_file.add_options()
            ( "server-port", po::value< short >( &server_port_ )->default_value( 1500 ), "port for server to listen on" )
            ( "server-max-connections", po::value< int >( &server_max_connections_ )->default_value( 16 ), "maximum number of clients" )
            ;

        po::options_description command_line_options;
        command_line_options.add( command_line ).add( config_file );

        po::options_description config_file_options;
        config_file_options.add( config_file );

        po::options_description visible( "Allowed options" );
        visible.add( command_line ).add( config_file );
        
        po::variables_map vm;
        store( po::command_line_parser( ac, av ).options( command_line_options ).run(), vm );

        ifstream ifs( "server.cfg" );
        store( parse_config_file( ifs, config_file_options ), vm );
        notify( vm );
    
        if ( vm.count( "help" ) )
		{
			cout << visible << endl;
		}
		else ok = true;
    }
    catch( exception& e )
    {
        cout << e.what() << endl;
    }    
	
	return ok;
}

