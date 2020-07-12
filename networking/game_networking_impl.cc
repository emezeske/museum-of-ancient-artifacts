#include "game_networking_impl.h"
#include "utilities.h"

namespace game_networking_impl
{

bool send_message( ENetPeer *peer, ENetHost *host, const unsigned char *message, const unsigned message_length, const message_channel channel )
{
	bool success = false;

	ENetPacket *packet;
	int flags = ( channel == UNRELIABLE ) ? 0 : ENET_PACKET_FLAG_RELIABLE;
	
	if ( ( packet = enet_packet_create( message, message_length, flags ) ) )
	{
		if ( enet_peer_send( peer, channel, packet ) == 0 )
		{
			enet_host_flush( host );
			success = true;
		}
		else NOTIFY( FAULT, "enet_peer_send() failed." );
	}
	else NOTIFY( FAULT, "enet_packet_create() failed." );

	return success;
}

} // end namespace game_networking_impl

