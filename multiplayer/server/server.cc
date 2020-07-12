#include <signal.h>

#include "game_networking.h"
#include "utilities.h"
#include "configuration.h"

volatile bool keep_going = true;

void stop_server( int ) { keep_going = false; }

bool sa_setup()
{
	struct sigaction stop_server_action;
	sigemptyset( &stop_server_action.sa_mask );
	stop_server_action.sa_flags = 0;
	stop_server_action.sa_handler = stop_server;
	return sigaction( SIGINT, &stop_server_action, 0 ) == 0;
}

int main( int ac, char **av )
{
	configuration config;
	game_networking::server game_server;

	utilities::set_notify_level( VERBOSE );

	if ( config.parse_options( ac, av ) )
	{
		VNOTIFY( INFO, "Server configured to use port %hd, and to accept a maximum of %d clients.", config.server_port(), config.server_max_connections() );

		if ( sa_setup() )
		{
			if ( game_server.initialize( config.server_port(), config.server_max_connections() ) )
			{
				while ( keep_going ) game_server.process_one_event();
				game_server.shutdown();
			}
		}
		else NOTIFY( FAULT, "Error setting up signal action." );
	}

	return 0;
}
