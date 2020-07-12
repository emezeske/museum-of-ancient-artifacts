#ifndef GAME_NETWORKING_H
#define GAME_NETWORKING_H

#include <enet/enet.h>
#include <string>
#include <vector>

#define EXPENSIVE_SANITY_CHECKING // Enables some sanity checking that might be too costly in a production release, but good for debugging.

namespace game_networking 
{

using std::string;
using std::vector;

const short SERVER_PORT = 1500;
const int MAX_SERVER_CONNECTIONS = 64;

enum message_channel
{
	UNRELIABLE,
	RELIABLE_LOW_PRIORITY,
	RELIABLE_HIGH_PRIORITY,
	NUM_CHANNELS
};

class server;

class server_client
{
	public:

		server_client( server &owner, ENetHost *local_enet_host, ENetPeer *enet_peer, const int disconnect_ack_timeout_millis = 5000 );

		bool send_message( const unsigned char *message, const unsigned message_length, const message_channel channel );
		void handle_incoming_message( const unsigned char *message, const unsigned message_length, const message_channel channel );
		void disconnect();

		string descriptive_name() const { return descriptive_name_; }
		bool broadcast_enabled() const { return broadcast_enabled_; }

	private:
		
		string descriptive_name_;
		int disconnect_ack_timeout_millis_;
		bool broadcast_enabled_;

		server &owner_;

		ENetHost *local_enet_host_;
		ENetPeer *enet_peer_;
};

class server
{
	public:

		server();

		bool initialize( const short local_port, const int max_num_connections );
		bool process_one_event( const int n_millis_to_wait = 1000 );
		bool broadcast_message( const unsigned char *message, const unsigned message_length, const message_channel channel );
		void shutdown();

		bool running() const { return running_; }
	
	protected:

		server_client *create_client( ENetPeer *enet_peer );
		server_client *get_client( ENetEvent *event );
		void destroy_client( server_client *client );

		bool running_;
		short local_port_;
		int max_num_connections_;
		int client_no_;

		typedef vector< server_client * > server_client_v;
		server_client_v clientele;

		ENetAddress local_enet_addr_;
		ENetHost *local_enet_host_;
};

class client
{
	public:

		client();

		bool connect( const string &server_address, const short server_port );
		bool send_message( const unsigned char *message, const unsigned message_length, const message_channel channel );
		void handle_incoming_message( const unsigned char *message, const unsigned message_length, const message_channel channel );
		bool process_one_event( const int n_millis_to_wait = 1000 );
		void shutdown();

		bool connected() const { return connected_; }

	private:

		bool connected_;
		string server_address_;
		short server_port_;

		ENetAddress server_enet_addr_;
		ENetPeer *server_enet_peer_;
		ENetHost *local_enet_host_;
};

} // end namespace game_networking

#endif // GAME_NETWORKING_H
