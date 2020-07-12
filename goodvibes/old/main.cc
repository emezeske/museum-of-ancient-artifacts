#include <math.h>
#include <alsa/asoundlib.h>

#include <vector>
#include <stdexcept>
#include <iostream>

#define ALSA_CALL(function, error_message) \
    do \
    { \
        int err = function; \
        if ( err < 0 ) \
            throw std::runtime_error( std::string( error_message ) + ":" + snd_strerror( err ) ); \
    } \
    while ( 0 )

typedef std::vector< std::pair<float,float> > sample_vector;

using std::cout;
using std::endl;

struct sound_driver
{
    sound_driver( const unsigned _rate ) :
        rate_( _rate )
    {
        ALSA_CALL( snd_pcm_open( &handle_, "default", SND_PCM_STREAM_PLAYBACK, 0 ), "cannot open audio device" );
        ALSA_CALL( snd_pcm_set_params( handle_, SND_PCM_FORMAT_S24_LE, SND_PCM_ACCESS_RW_INTERLEAVED, CHANNELS, rate_, 1, 10000 ), "cannot initialize hardware parameter structure" );
    }

    unsigned rate() const
    {
        return rate_;
    }

    void play( const sample_vector& samples )
    {
        std::vector<int> buffer( samples.size() * CHANNELS );
        int* p = &buffer[0];

        for ( sample_vector::const_iterator sample_it = samples.begin(); sample_it != samples.end(); ++sample_it )
        {
            *p++ = clamp( static_cast<int>( roundf( sample_it->first * MAX_SAMPLE ) ) );
            *p++ = clamp( static_cast<int>( roundf( sample_it->second * MAX_SAMPLE ) ) );
        }

        int frames = snd_pcm_writei( handle_, &buffer[0], buffer.size() / CHANNELS );

        if ( frames < 0 ) frames = snd_pcm_recover( handle_, frames, 0 );
        if ( frames < 0 ) throw std::runtime_error( std::string( "write to audio interface failed: " ) + snd_strerror( frames ) );
        // if ( frames > 0 && frames < buffer.size() / CHANNELS ) cout("Short write (expected %li, wrote %li)\n", (long)sizeof(buffer), frames);

    }

    ~sound_driver()
    {
        snd_pcm_close( handle_ );
    }

private:
    static const int
        CHANNELS = 2,
        BYTES = 3,
        MAX_SAMPLE = ( 0x1 << ( BYTES * 8 - 1 ) ) - 1;

    snd_pcm_t* handle_;

    unsigned rate_;

    int clamp( const int value )
    {
        if ( value < -MAX_SAMPLE ) return -MAX_SAMPLE;
        else if ( value > MAX_SAMPLE ) return MAX_SAMPLE;
        else return value;
    }
};

int main( int /* argc */, char** /* argv */ )
{
    sound_driver driver( 48000 );
    sample_vector samples( driver.rate() * 6 );

    particle_string ps( 8, 0.11111f, particle( 1.0f, 0.0f, 0.0f, 0.9999f ) );

    ps.particles_.front().conservation_ = 0.51f;
    ps.particles_.back().conservation_ = 0.51f;

    ps.set_left_sampler( 3 );
    ps.set_right_sampler( 5 );

    float z = 50.502f;

    for ( particle_vector::iterator particle = ps.particles_.begin(); particle != ps.particles_.end(); ++particle )
    {
        particle->mass_ = z + 1.1f;
    }

    //ps.strike( 4, 1, 0.9f );
    ps.pluck( 1.0f );

    for ( size_t i = 0; i < samples.size(); ++i )
    {
        ps.next_sample( samples[i] );
    }

    while ( true ) driver.play( samples );

    return 0;
}
