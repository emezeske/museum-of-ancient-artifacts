#include "game_networking_impl.h"
#include "utilities.h"

namespace game_networking
{

client::client() :
	connected_( false ),
	server_address_( "" ),
	server_port_( 0 )
{
}

bool client::connect( const string &server_address, const short server_port )
{
	bool success = false;

	server_address_ = server_address;
	server_port_ = server_port;

	if ( connected_ ) shutdown(); // Abort any current connection in lieu of the newly requested one.

	if ( enet_initialize() == 0 )
	{
		if ( enet_address_set_host( &server_enet_addr_, server_address_.c_str() ) == 0 )
		{
			server_enet_addr_.port = server_port_;

			if ( ( local_enet_host_ = enet_host_create( 0, 1, 0, 0 ) ) )
			{
				if ( ( server_enet_peer_ = enet_host_connect( local_enet_host_, &server_enet_addr_, NUM_CHANNELS ) ) )
				{
					ENetEvent event;
			
					if ( enet_host_service( local_enet_host_, &event, 5000 ) > 0 && event.type == ENET_EVENT_TYPE_CONNECT ) 
					{
						VNOTIFY( INFO, "Connected to %s:%d.", server_address_.c_str(), server_port_ );
						success = connected_ = true;
					}
					else
					{
						VNOTIFY( FAULT, "Failed to connect to %s:%d.", server_address_.c_str(), server_port_ );
						enet_peer_reset( server_enet_peer_ );
					}
				}
				else NOTIFY( FAULT, "enet_host_connect() failed." );
			
				if ( !success ) enet_host_destroy( local_enet_host_ );
			}
			else NOTIFY( FAULT, "enet_host_create() failed." );
		}
		else NOTIFY( FAULT, "enet_address_set_host() failed." );
		
		if ( !success ) enet_deinitialize();
	}
	else NOTIFY( FAULT, "enet_initialize() failed." );

	return success;
}

bool client::send_message( const unsigned char *message, const unsigned message_length, const message_channel channel )
{
	bool success = false;

	if ( connected_ )
	{
		success = game_networking_impl::send_message( server_enet_peer_, local_enet_host_, message, message_length, channel );
	}
	else NOTIFY( FAULT, "Cannot send message while not connected!" );

	return success;
}

void client::handle_incoming_message( const unsigned char *message, const unsigned message_length, const message_channel channel )
{
	// ...
}

bool client::process_one_event( const int n_millis_to_wait )
{
	ENetEvent event;
	bool event_occurred = false;
	
	if ( enet_host_service( local_enet_host_, &event, n_millis_to_wait ) )
	{
		event_occurred = true;

		switch ( event.type )
		{
			case ENET_EVENT_TYPE_RECEIVE:
			{
				VNOTIFY( VERBOSE, "Packet from: '%x:%x', channel: %u, length: %u", event.peer->address.host, event.peer->address.port, event.channelID, event.packet->dataLength );
				handle_incoming_message( event.packet->data, event.packet->dataLength, static_cast< message_channel >( event.channelID ) );
				enet_packet_destroy( event.packet );
				break;
			}
	
			case ENET_EVENT_TYPE_DISCONNECT:
			{
				NOTIFY( INFO, "Server disconnected." );
				connected_ = false;
				break;
			}
	
			default:

				VNOTIFY( INFO, "Received unhandled event: %d.", event.type );
				break;
		}
	}

	return event_occurred;
}

void client::shutdown()
{
	if ( connected_ )
	{
		bool disconnected_cleanly = false;
		ENetEvent event;
		
		enet_peer_disconnect( server_enet_peer_, 0 );
		
		if ( enet_host_service( local_enet_host_, &event, 5000 ) )
		{
			switch ( event.type )
			{
				case ENET_EVENT_TYPE_RECEIVE:
		
					enet_packet_destroy( event.packet );
					break;
		
				case ENET_EVENT_TYPE_DISCONNECT:
		
					NOTIFY( INFO, "Disconnected cleanly from server." );
					disconnected_cleanly = true;
					break;
		
				default:
		
					VNOTIFY( INFO, "Received unhandled event: %d, while waiting for ENET_EVENT_TYPE_DISCONNECT.", event.type );
					break;
			}
		}
		
		if ( !disconnected_cleanly ) 
		{
			enet_peer_reset( server_enet_peer_ );
			NOTIFY( INFO, "Disconnected harshly from server." );
		}

		enet_host_destroy( local_enet_host_ );
		enet_deinitialize();

		connected_ = false;
	}
}

} // end namespace game_networking
