#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <boost/lexical_cast.hpp>

using namespace std;

int main()
{
    cout << boost::lexical_cast<uint32_t>( "1593578931" ) << endl
}
