#include <SDL/SDL.h>

#include "vibrator.h"

Vibrator::Vibrator( const std::string &control_file ) :
    file_( control_file.c_str() ),
    pulse_end_( 0 )
{
}

Vibrator::~Vibrator()
{
    turn_off();
}

void Vibrator::set_level( float level )
{
    if ( level < 0.0f ) level = 0.0f;
    else if ( level > 1.0f ) level = 1.0f;

    file_ << static_cast<int>( level * 255 ) << std::endl;
}

void Vibrator::pulse( float level, int length_ms )
{
    set_level( level );
    pulse_end_ = SDL_GetTicks() + length_ms;
}

void Vibrator::step()
{
    if ( pulse_end_ != 0 && SDL_GetTicks() >= pulse_end_ ) 
    {
        turn_off();
        pulse_end_ = 0;
    }
}

void Vibrator::turn_off()
{
    file_ << 0 << std::endl;
}
