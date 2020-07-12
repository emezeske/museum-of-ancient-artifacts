#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <string>

using std::string;

class configuration
{
	public:
		
		bool parse_options( int ac, char **av );

		short server_port() const { return server_port_; }
		string server_address() const { return server_address_; }

	private:

		short server_port_;
		string server_address_;
};

#endif // CONFIGURATION_H
