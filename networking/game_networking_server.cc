#include <algorithm>
#include <sstream>
#include <iomanip>

#include "game_networking_impl.h"
#include "utilities.h"

namespace game_networking
{

using std::find;
using std::ostringstream;

server::server() :
	running_( 0 ),
	local_port_( 0 ),
	max_num_connections_( 1 ),
	client_no_( 0 )
{
}

bool server::initialize( const short local_port, const int max_num_connections )
{
	bool success = false;

	if ( running_ ) shutdown(); // If the server is already running, shut it down and start over.

	local_port_ = local_port;
	max_num_connections_ = max_num_connections;

	if ( enet_initialize() == 0 )
	{
		local_enet_addr_.host = ENET_HOST_ANY;
		local_enet_addr_.port = local_port_;

		if ( ( local_enet_host_ = enet_host_create( &local_enet_addr_, max_num_connections, 0, 0 ) ) )
		{
			VNOTIFY( INFO, "Server is listening on port %d.", local_port_ );
			success = running_ = true;
		}
		else 
		{
			NOTIFY( FAULT, "enet_host_create() failed." );
			enet_deinitialize();
		}
	}
	else NOTIFY( FAULT, "enet_initialize() failed." );

	return success;
}

bool server::process_one_event( const int n_millis_to_wait )
{
	bool event_occurred = false;

	if ( running_ )
	{
		ENetEvent event;
		server_client *temp_client = 0;
		
		if ( enet_host_service( local_enet_host_, &event, n_millis_to_wait ) )
		{
			event_occurred = true;

			switch ( event.type )
			{
				case ENET_EVENT_TYPE_CONNECT:
				{
					event.peer->data = reinterpret_cast< void * >( temp_client = create_client( event.peer ) );
					VNOTIFY( INFO, "Client connected: '%s'.", temp_client->descriptive_name().c_str() );
					break;
				}
		
				case ENET_EVENT_TYPE_RECEIVE:
				{
					if ( ( temp_client = get_client( &event ) ) )
					{
						VNOTIFY( VERBOSE, "Packet from: '%s', channel: %u, length: %u.", temp_client->descriptive_name().c_str(), event.channelID, event.packet->dataLength );
						temp_client->handle_incoming_message( event.packet->data, event.packet->dataLength, static_cast< message_channel >( event.channelID ) );
					}
					enet_packet_destroy( event.packet );
					break;
				}
		
				case ENET_EVENT_TYPE_DISCONNECT:
				{
					if ( ( temp_client = get_client( &event ) ) )
					{
						VNOTIFY( INFO, "Client disconnected: '%s'.", temp_client->descriptive_name().c_str() );
						destroy_client( temp_client );
						event.peer->data = 0; // Just in case.
					}
					break;
				}
		
				default:
		
					VNOTIFY( INFO, "Received unhandled event: %d.", event.type );
					break;
			}
		}
	}

	return event_occurred;
}

bool server::broadcast_message( const unsigned char *message, const unsigned message_length, const message_channel channel )
{
	bool success = true;

	if ( running_ )
	{
		for ( server_client_v::iterator i = clientele.begin(); i != clientele.end(); ++i )
		{
			if ( ( *i )->broadcast_enabled() ) success &= ( *i )->send_message( message, message_length, channel );
		}
	}
	else success = false;

	return success;
}

void server::shutdown()
{
	if ( running_ )
	{
		for ( server_client_v::iterator i = clientele.begin(); i != clientele.end(); ++i )
		{
			( *i )->disconnect();
			delete *i;
		}

		clientele.clear();

		enet_host_destroy( local_enet_host_ );
		enet_deinitialize();

		NOTIFY( INFO, "Server successfully stopped." );
		running_ = false;
	}
}

server_client *server::create_client( ENetPeer *enet_peer )
{
	server_client *c = new server_client( *this, local_enet_host_, enet_peer );
	clientele.push_back( c );
	return c;
}

server_client *server::get_client( ENetEvent *event )
{
	server_client *c = reinterpret_cast< server_client * >( event->peer->data );

#ifdef EXPENSIVE_SANITY_CHECKING
	if ( c )
	{
		server_client_v::iterator i = find( clientele.begin(), clientele.end(), c );
		
		if ( i == clientele.end() )
		{
			VNOTIFY( INFO, "WARNING: Nonexistent client: %p", c );
			c = 0;
		}
	}
	else NOTIFY( INFO, "WARNING: Null client!" );
#endif

	return c;
}

void server::destroy_client( server_client *client )
{
	server_client_v::iterator i = find( clientele.begin(), clientele.end(), client );

	if ( i != clientele.end() )
	{ 
		server_client *c = *i;
		clientele.erase( i );
		delete c;
	}
}

server_client::server_client( server &owner, ENetHost *local_enet_host, ENetPeer *enet_peer, const int disconnect_ack_timeout_millis ) :
	disconnect_ack_timeout_millis_( disconnect_ack_timeout_millis ),
	broadcast_enabled_( true ),
	owner_( owner ),
	local_enet_host_( local_enet_host ),
	enet_peer_( enet_peer )
{
	ostringstream oss;
	oss << std::hex << enet_peer_->address.host << std::dec << ":" << enet_peer_->address.port;
	descriptive_name_ = oss.str();
}

bool server_client::send_message( const unsigned char *message, const unsigned message_length, const message_channel channel )
{
	return game_networking_impl::send_message( enet_peer_, local_enet_host_, message, message_length, channel );
}

void server_client::handle_incoming_message( const unsigned char *message, const unsigned message_length, const message_channel channel )
{
	bool saved_broadcast_enabled = broadcast_enabled_;
	broadcast_enabled_ = false;
	owner_.broadcast_message( message, message_length, channel );
	broadcast_enabled_ = saved_broadcast_enabled;
}

void server_client::disconnect()
{
	bool disconnected_cleanly = false;
	ENetEvent event;
	
	enet_peer_disconnect( enet_peer_, 0 );
	
	if ( enet_host_service( local_enet_host_, &event, disconnect_ack_timeout_millis_ ) )
	{
		switch ( event.type )
		{
			case ENET_EVENT_TYPE_RECEIVE:
	
				enet_packet_destroy( event.packet );
				break;
	
			case ENET_EVENT_TYPE_DISCONNECT:
	
				VNOTIFY( INFO, "Disconnected client cleanly from server: '%s'.", descriptive_name().c_str() );
				disconnected_cleanly = true;
				break;
	
			default:
	
				VNOTIFY( INFO, "Received unhandled event: %d, while waiting for ENET_EVENT_TYPE_DISCONNECT.", event.type );
				break;
		}
	}
	
	if ( !disconnected_cleanly ) 
	{
		enet_peer_reset( enet_peer_ );
		VNOTIFY( INFO, "Disconnected client harshly from server: '%s'.", descriptive_name().c_str() );
	}
}

} // end namespace game_networking

