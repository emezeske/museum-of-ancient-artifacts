#include <cmath>
#include <cstdlib>
#include <cassert>
#include <jack/jack.h>
#include <jack/midiport.h>

#include <vector>
#include <stdexcept>
#include <iostream>

#include <boost/date_time/posix_time/posix_time.hpp>

using std::cout;
using std::cerr;
using std::endl;

struct particle
{
    particle( const float mass = 1.0f, const float position = 0.0f, const float velocity = 0.0f, const float conservation = 1.0f ) :
        mass_( mass ),
        position_( position ),
        velocity_( velocity ),
        conservation_( conservation )
    {
    }

    float
        mass_,
        position_,
        velocity_,
        conservation_;
};

typedef std::vector<particle> particle_vector;

struct particle_string
{
    particle_string( const size_t size, const float tension, const particle& default_particle = particle() ) :
        particles_( size, default_particle ),
        left_sampler_( particles_.begin() ),
        right_sampler_( particles_.begin() ),
        tension_( tension )
    {
        assert( particles_.size() >= 2 );
    }

    void set_left_sampler( const size_t location )
    {
        assert( location < particles_.size() );
        left_sampler_ = particles_.begin() + location;
    }

    void set_right_sampler( const size_t location )
    {
        assert( location < particles_.size() );
        right_sampler_ = particles_.begin() + location;
    }

    void strike( const size_t center, const size_t radius, const float displacement )
    {
        assert( static_cast<int>( center ) - static_cast<int>( radius ) >= 0 );
        assert( center + radius < particles_.size() );

        particles_[center].position_ = displacement;

        for ( size_t i = 1; i < radius; ++i )
        {
            float d = displacement * static_cast<float>( radius - i ) / static_cast<float>( radius );
            particles_[center + i].position_ = d;
            particles_[center - i].position_ = d;
        }
    }

    void pluck( const float displacement )
    {
        size_t i = 0;
        float d = displacement / static_cast<float>( particles_.size() );

        for ( ; i < particles_.size() / 2; ++i )
        {
            particles_[i].position_ = static_cast<float>( i ) * d;
        }
        
        for ( ; i < particles_.size(); ++i )
        {
            particles_[i].position_ = static_cast<float>( particles_.size() - i ) * d;
        }
    }

    void silence()
    {
        for ( particle_vector::iterator it = particles_.begin(); it != particles_.end(); ++it )
        {
            it->position_ = 0.0f;
            it->velocity_ = 0.0f;
        }
    }

    void next_sample( std::pair<float, float>& sample )
    {
        particle_vector::iterator 
            left = particles_.begin(),
            right = left + 1;
        
        for ( ; right != particles_.end(); ++left, ++right )
        {
            float
                force = ( right->position_ - left->position_ ) * tension_,
                acceleration = force / left->mass_;
        
            left->velocity_ += acceleration;
            right->velocity_ -= acceleration;
        
            left->position_ = ( left->position_ + left->velocity_ ) * left->conservation_;
        }
        
        left->position_ = ( left->position_ + left->velocity_ ) * left->conservation_;
        
        sample.first = left_sampler_->position_;
        sample.second = right_sampler_->position_;
    }

    particle_vector particles_;

    particle_vector::iterator
        left_sampler_,
        right_sampler_;

    float tension_;
};

struct JackClient
{
    JackClient( const std::string& name )
    {
        client_ = jack_client_new( name.c_str() );

        if ( !client_ )
            throw std::runtime_error( "Could not connect to JACK server" );

        jack_set_process_callback( client_, &JackClient::process_cb, this );
        jack_set_sample_rate_callback( client_, &JackClient::set_sample_rate_cb, this );
        jack_on_shutdown( client_, &JackClient::shutdown_cb, this );

	    midi_input_port_ = jack_port_register( client_, "midi_in", JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0 );
        output_port_ = jack_port_register( client_, "audio_out", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0 );
        
        if ( jack_activate( client_ ) != 0 )
            throw std::runtime_error( "Could not activate JACK client" );

        const char **ports = jack_get_ports( client_, NULL, NULL, JackPortIsPhysical | JackPortIsInput );
        
        if ( !ports )
            throw std::runtime_error( "Cannot find any physical playback ports" );
        
        for ( int i = 0; ports[i]; ++i )
        {
            if ( jack_connect( client_, jack_port_name( output_port_ ), ports[i] ) != 0 )
                throw std::runtime_error( "Cannot connect to JACK output port" );
        }
    
        std::free( ports );

        sample_rate_ = jack_get_sample_rate( client_ );
    }
    
    virtual ~JackClient()
    {
        jack_client_close( client_ );
    }

    static void error_cb( const char *description )
    {
        cerr << "JACK error: " << description << endl;
    }


    static int process_cb( jack_nframes_t num_frames, void *self )
    {
        return static_cast<JackClient*>( self )->process( num_frames );
    }

    static int set_sample_rate_cb( jack_nframes_t sr, void *self )
    {
        return static_cast<JackClient*>( self )->set_sample_rate( sr );
    }

    static void shutdown_cb( void *self )
    {
        static_cast<JackClient*>( self )->shutdown();
    }

protected:
    jack_client_t* client_;
    jack_port_t* midi_input_port_;
    jack_port_t* output_port_;
    jack_nframes_t sample_rate_;

    virtual int process( jack_nframes_t num_frames ) = 0;
    
    virtual int set_sample_rate( jack_nframes_t sr )
    {
        sample_rate_ = sr;
        return 0;
    }
    
    virtual void shutdown()
    {
        std::terminate();
    }
};

struct AcousticSimulator : public JackClient
{
    AcousticSimulator( const std::string& name ) :
        JackClient( name ),
        ps_( 8, 0.11111f, particle( 1.0f, 0.0f, 0.0f, 0.9999f ) )
    {
        ps_.particles_.front().conservation_ = 0.51f;
        ps_.particles_.back().conservation_ = 0.51f;

        ps_.set_left_sampler( 3 );
        ps_.set_right_sampler( 5 );

        float z = 50.502f;

        for ( particle_vector::iterator particle = ps_.particles_.begin(); particle != ps_.particles_.end(); ++particle )
        {
            particle->mass_ = z + 1.1f;
        }

        ps_.strike( 4, 1, 0.9f );
        //ps_.pluck( 1.0f );
    }

    virtual int process( jack_nframes_t num_frames )
    {
	    void* midi_input_buffer = jack_port_get_buffer( midi_input_port_, num_frames );
        jack_default_audio_sample_t *output_buffer = static_cast<jack_default_audio_sample_t*>( jack_port_get_buffer( output_port_, num_frames ) );

	    jack_nframes_t
            midi_event_index = 0,
            midi_event_count = jack_midi_get_event_count( midi_input_buffer );

	    jack_midi_event_t midi_event;
	    jack_midi_event_get( &midi_event, midi_input_buffer, 0 );
        
        for ( jack_nframes_t i = 0; i < num_frames; ++i )
        {
            if ( midi_event_index < midi_event_count && midi_event.time == i )
            {
                // TODO: MIDI parsing library?
                const unsigned char MIDI_EVENT_NOTE_ON = 0x90;

                int event_code = ( midi_event.buffer[0] & 0xf0 );

			    if ( event_code == MIDI_EVENT_NOTE_ON )
			    {
			    	unsigned char
                        pitch = midi_event.buffer[1],
			    	    velocity = midi_event.buffer[2];

                    ps_.tension_ = 0.11111f + 9.0f * static_cast<float>( pitch ) / 255.0f;
                    ps_.silence();
                    ps_.pluck( static_cast<float>( velocity ) / 255.0f );
			    }

			    if ( ++midi_event_index < midi_event_count )
	                jack_midi_event_get( &midi_event, midi_input_buffer, midi_event_index );
            }

            std::pair<float, float> sample;
            ps_.next_sample( sample );
            output_buffer[i] = sample.first;
        }
                         
        return 0;
    }

private:
    particle_string ps_;
};

int main( int /* argc */, char** /* argv */ )
{
    // jack_set_error_function( &JackClient::error_cb );

    // AcousticSimulator jc( "goodvibes" );

    // while ( true )
    // {
    //     sleep( 1 );
    // }

    // For benchmarking:
    //
    // particle_string ps( 512, 0.11111f, particle( 1.0f, 0.0f, 0.0f, 0.9999f ) );
    // ps.particles_.front().conservation_ = 0.51f;
    // ps.particles_.back().conservation_ = 0.51f;
    // 
    // ps.set_left_sampler( 3 );
    // ps.set_right_sampler( 5 );
    // 
    // float z = 50.502f;
    // 
    // for ( particle_vector::iterator particle = ps.particles_.begin(); particle != ps.particles_.end(); ++particle )
    // {
    //     particle->mass_ = z + 1.1f;
    // }
    // 
    // ps.strike( 4, 1, 0.9f );
    // std::pair<float, float> sample;

    // boost::posix_time::ptime begin( boost::posix_time::microsec_clock::universal_time() );

    // for ( int i = 0; i < 44100 * 30; ++i )
    //     ps.next_sample( sample );

    // boost::posix_time::ptime end( boost::posix_time::microsec_clock::universal_time() );

    // cout << "took: " << end - begin << endl;

    return 0;
}
