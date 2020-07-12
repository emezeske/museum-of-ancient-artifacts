#ifndef CONFIGURATION_H
#define CONFIGURATION_H

class configuration
{
	public:
		
		bool parse_options( int ac, char **av );

		short server_port() const { return server_port_; }
		int server_max_connections() const { return server_max_connections_; }

	private:

		short server_port_;
		int server_max_connections_;
};

#endif // CONFIGURATION_H
