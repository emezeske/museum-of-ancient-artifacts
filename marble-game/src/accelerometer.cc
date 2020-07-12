#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>

#include "accelerometer.h"
#include "utilities.h"

using std::string;
using std::vector;

// TODO
//    AccelerometerReader reader = AccelerometerReader( "/dev/input/event3" );
//
//    AccelerometerReadingVector readings;
//
//    struct timeval next_dump;
//    timerclear( &next_dump );
//
//    while ( true )
//    {
//        struct timeval now;
//        AccelerometerReading reading;
//
//        gettimeofday( &now, NULL ); // TODO test return
//
//        if ( reader.read( reading ) ) readings.push_back( reading );
//
//        if ( timercmp( &now, &next_dump, > ) )
//        {
//            AccelerometerReading average;
//
//            average_accelerometer_readings( readings, average );
//
//            average.print();
//
//            NOTIFY( VERBOSE, "\n" );
//
//            readings.clear();
//
//            next_dump.tv_sec = now.tv_sec;
//            next_dump.tv_usec = now.tv_usec + 10000;
//        }
//    }

// TODO Where to __u16 and __s32 come from?
typedef short _u16;
typedef int _s32;

enum EventType
{
    EVENT_SYNCHRONIZE = 0,
    EVENT_MOVEMENT    = 2
};

struct __attribute__((packed)) input_event
{
    struct timeval time;
    _u16 type;
    _u16 code;
    _s32 value;
};

//////////////////////////////////////////////////////////////////////////////////
// Member function definitions for AccelerometerReading
//////////////////////////////////////////////////////////////////////////////////

AccelerometerReading::AccelerometerReading() { reset(); }

bool AccelerometerReading::is_set() const
{
    bool set = true;

    for ( int i = 0; set && i < NUM_AXES; ++i ) 
    {
        set = timerisset( &measured_at_[i] );
    }

    return set;
}

void AccelerometerReading::reset()
{
    for ( int i = 0; i < NUM_AXES; ++i ) 
    {
        acceleration_[i] = 0;
        timerclear( &measured_at_[i] );
    }
}

void AccelerometerReading::print() const
{
    VNOTIFY( VERBOSE, "X: %16d:%16d %f\n", measured_at_[AXIS_X].tv_sec, measured_at_[AXIS_X].tv_usec, acceleration_[AXIS_X] / 1000.0f );
    VNOTIFY( VERBOSE, "Y: %16d:%16d %f\n", measured_at_[AXIS_Y].tv_sec, measured_at_[AXIS_Y].tv_usec, acceleration_[AXIS_Y] / 1000.0f );
    VNOTIFY( VERBOSE, "Z: %16d:%16d %f\n", measured_at_[AXIS_Z].tv_sec, measured_at_[AXIS_Z].tv_usec, acceleration_[AXIS_Z] / 1000.0f );
}

//////////////////////////////////////////////////////////////////////////////////
// Member function definitions for AccelerometerReader
//////////////////////////////////////////////////////////////////////////////////

AccelerometerReader::AccelerometerReader( const string &device_file ) :
    device_file_( device_file )
{
    if ( ( fd_ = open( device_file_.c_str(), O_NONBLOCK | O_RDONLY ) ) == -1 )
    {
        VNOTIFY( VERBOSE, "Error opening %s for reading: %s.\n", device_file_.c_str(), strerror( errno ) );
        // TODO throw thingie;
    }
}

AccelerometerReader::~AccelerometerReader()
{
    close( fd_ );
}

bool AccelerometerReader::read( AccelerometerReading &reading )
{
    bool 
        done = false,
        was_error = false; // TODO Reopen when errors happen?
    
    char buffer[sizeof( input_event )];
    unsigned buffer_consumed = 0;

    reading.reset();

    while ( !done && !was_error )
    {
        fd_set read_fds;
        struct timeval wait_time;
        int select_result;
    
        FD_ZERO( &read_fds );
        FD_SET( fd_, &read_fds );
    
        wait_time.tv_sec = 5;
        wait_time.tv_usec = 0;

        if ( ( select_result = select( fd_ + 1, &read_fds, NULL, NULL, &wait_time ) ) == 1 )
        {
            ssize_t bytes_read;
    
            if ( ( bytes_read = ::read( fd_, buffer + buffer_consumed, sizeof( buffer ) - buffer_consumed ) ) > 0 )
            {
                buffer_consumed += bytes_read;
    
                if ( buffer_consumed >= sizeof( input_event ) )
                {
                    input_event *event = reinterpret_cast<input_event*>( buffer );
    
                    switch ( event->type )
                    {
                        case EVENT_MOVEMENT:
                            if ( event->code < NUM_AXES )
                            {
                                reading.acceleration_[event->code] = event->value;
                                reading.measured_at_[event->code] = event->time;
                            }
                            break;
    
                        case EVENT_SYNCHRONIZE:
                            done = reading.is_set();
                            break;
                    }
    
                    buffer_consumed = 0;
                }
            }
            else if ( bytes_read == 0 )
            {
                was_error = true;
                NOTIFY( VERBOSE, "read() returned EOF.\n" );
            }
            else
            {
                was_error = true;
                VNOTIFY( VERBOSE, "Error in read(): %s.\n", strerror( errno ) );
            }
        }
        else if ( select_result == -1 && errno != EINTR )
        {
            was_error = true;
            VNOTIFY( VERBOSE, "Error in select(): %s.\n", strerror( errno ) );
        }
    }

    return !was_error;
}

//////////////////////////////////////////////////////////////////////////////////
// Function definitions for TODO Namespace?
//////////////////////////////////////////////////////////////////////////////////

void average_accelerometer_readings( const AccelerometerReadingVector &readings, AccelerometerReading &average )
{
    if ( readings.size() )
    {
        long int accumulator[NUM_AXES] = { 0, 0, 0 };

        for ( AccelerometerReadingVector::const_iterator it = readings.begin(); it != readings.end(); ++it )
        {
            for ( int i = 0; i < NUM_AXES; ++i ) accumulator[i] += it->acceleration_[i];
        }

        for ( int i = 0; i < NUM_AXES; ++i ) 
        {
            average.acceleration_[i] = accumulator[i] / static_cast<long int>( readings.size() ); // TODO Accurate? FP?
        }
    }
}
