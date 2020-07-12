#include <iostream>

#include "game_networking.h"
#include "utilities.h"
#include "configuration.h"

using namespace std;

int main( int ac, char **av )
{
	configuration config;
	game_networking::client game_client;

	utilities::set_notify_level( VERBOSE );

	if ( config.parse_options( ac, av ) )
	{
		VNOTIFY( INFO, "Configured for server %s:%hd.", config.server_address().c_str(), config.server_port() );

		if ( game_client.connect( config.server_address(), config.server_port() ) )
		{
			game_client.process_one_event( 100 );
		
			while ( game_client.connected() )
			{
				string message;
				utilities::ustring umessage;

				cout << "message?> " << flush;
				getline( cin, message );

				umessage = message;
		
				if ( message.size() == 0 )
				{
					 game_client.process_one_event( 100 );
				}
				else if ( message.find( "quit" ) == string::npos )
				{
					 game_client.send_message( reinterpret_cast< const unsigned char * >( message.data() ), message.size(), game_networking::RELIABLE_LOW_PRIORITY );
				}
				else break;
			}
		
			game_client.shutdown();
		}
	}

	return 0;
}
