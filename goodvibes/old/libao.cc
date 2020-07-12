#include <stdio.h>
#include <ao/ao.h>
#include <math.h>
#include <vector>
#include <iostream>

typedef std::vector< std::pair<float,float> > sample_vector;

using std::cout;
using std::endl;

struct sound_driver
{
    sound_driver( const int rate )
    {
	    format_.bits = BYTES * 8;
	    format_.channels = CHANNELS;
	    format_.rate = rate;
	    format_.byte_format = AO_FMT_LITTLE;

	    ao_initialize();
	    device_ = ao_open_live( ao_default_driver_id(), &format_, NULL );
        if ( !device_ ) throw "TODO";
    }

    void play( const sample_vector& samples )
    {
        std::vector<char> buffer( samples.size() * CHANNELS * BYTES );
        char* p = &buffer[0];

        for ( sample_vector::const_iterator sample_it = samples.begin(); sample_it != samples.end(); ++sample_it )
        {
            int sample_l = clamp( static_cast<int>( roundf( sample_it->first * MAX_SAMPLE ) ) );
            *p++ = static_cast<char>( sample_l & 0xff );
            *p++ = static_cast<char>( ( sample_l >> 8 ) & 0xff );
            // *p++ = static_cast<char>( ( sample_l >> 16 ) & 0xff );

            int sample_r = clamp( static_cast<int>( roundf( sample_it->second * MAX_SAMPLE ) ) );
            *p++ = static_cast<char>( sample_r & 0xff );
            *p++ = static_cast<char>( ( sample_r >> 8 ) & 0xff );
            // *p++ = static_cast<char>( ( sample_r >> 16 ) & 0xff );
	    }

        ao_play( device_, &buffer[0], buffer.size() );
    }

    ~sound_driver()
    {
	    ao_close( device_ );
	    ao_shutdown();
    }

private:
    static const int
        CHANNELS = 2,
        BYTES = 3,
        MAX_SAMPLE = ( 0x1 << ( BYTES * 8 - 1 ) - 1 );

	ao_sample_format format_;
    ao_device* device_;

    int clamp( const int value )
    {
        if ( value < -MAX_SAMPLE ) return -MAX_SAMPLE;
        else if ( value > MAX_SAMPLE ) return MAX_SAMPLE;
        else return value;
    }
};

int main( int /* argc */, char ** /* argv */ )
{
    const int rate = 44100;
	const float frequency = 440.0;

    sound_driver driver( rate );

    sample_vector samples( rate );

    for ( size_t i = 0; i < samples.size(); ++i )
    {
		float sample = 0.5f * sinf( 2.0f * static_cast<float>( M_PI ) * frequency * static_cast<float>( i ) / rate );
        samples[i] = std::make_pair<float, float>( sample, sample );
	}

    driver.play( samples );

    return 0;
}
