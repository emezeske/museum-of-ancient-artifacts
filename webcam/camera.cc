#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>

#include "camera.h"
#include "utilities.h"

camera::camera( const char *dev, int channel ) throw( camera_exception ) :
	_dev( dev ),
	_frame_buf( 0 ),
	_do_double_buffer( false )
{
	bool ok = false;

	if ( ( _fd = open( _dev.c_str(), O_RDWR ) ) != -1 )
	{
    	if ( ioctl( _fd, VIDIOCGCAP, &_vcap ) != -1 )
		{
			if ( _vcap.type & VID_TYPE_CAPTURE ) 
			{
				if ( channel < _vcap.channels )
				{
    				VNOTIFY( VERBOSE, "Device %s: image maxwidth: %d maxheight: %d", _dev.c_str(), _vcap.maxwidth, _vcap.maxheight );
    				VNOTIFY( VERBOSE, "Device %s: image minwidth: %d minheight: %d", _dev.c_str(), _vcap.minwidth, _vcap.minheight );

					video_channel chan;
					chan.channel = channel;

					if ( ioctl( _fd, VIDIOCGCHAN, &chan ) != -1 ) 
					{
						VNOTIFY( VERBOSE, "Device %s: identifies as %s", _dev.c_str(), chan.name );

						if ( ioctl( _fd, VIDIOCGWIN, &_vwin ) != -1 )
						{
							if ( chan.type & VIDEO_TYPE_CAMERA )
							{
								chan.norm = VIDEO_MODE_NTSC;
							
								if ( ioctl( _fd, VIDIOCSCHAN , &chan ) != -1 )
								{
									if ( ioctl( _fd, VIDIOCGPICT, &_vpic ) != -1 )
									{
										ok = true;
									}
									else VNOTIFY( FAULT, "Failed to get video_picture information for device %s: %m", _dev.c_str() );
								}
								else VNOTIFY( FAULT, "Failed to change device %s to VIDEO_MODE_NTSC mode: %m", _dev.c_str() );
							}
							else VNOTIFY( FAULT, "Channel %d on device %s is not a VIDEO_TYPE_CAMERA channel.", channel, _dev.c_str() );
						}
						else VNOTIFY( FAULT, "Failed to get window information for device %s: %m", _dev.c_str() );
					}
					else VNOTIFY( FAULT, "Error getting channel information for channel %d on device %s: %m", channel, _dev.c_str() );
				}
				else VNOTIFY( FAULT, "Device %s has %d channels: cannot use requested channel of %d.", _dev.c_str(), _vcap.channels, channel );
			}
			else VNOTIFY( FAULT, "Device %s is not a capture device.", _dev.c_str() );
		}
		else VNOTIFY( FAULT, "Error getting device capabilities for %s: %m", _dev.c_str() );
	}
	else VNOTIFY( FAULT, "Error opening %s: %m", _dev.c_str() );

	if ( !ok ) throw camera_exception( "Could not open and verify device." );
}

camera::~camera()
{
	close( _fd );
}

bool camera::set_size( unsigned w, unsigned h )
{
	bool ok = false;

	w = ( w == 0 ? _vwin.width : w ); 
	h = ( h == 0 ? _vwin.height : h ); 

	if ( w <= static_cast< unsigned > ( _vcap.maxwidth ) && h <= static_cast< unsigned > ( _vcap.maxheight ) )
	{
		_vwin.width = w;
		_vwin.height = h;

		if ( ioctl( _fd, VIDIOCSWIN, &_vwin ) != -1 )
		{
			if ( ioctl( _fd, VIDIOCGWIN, &_vwin ) != -1 )
			{
				if ( _vwin.width == w && _vwin.height == h )
				{
					ok = true;
				}
				else VNOTIFY( FAULT, "Window size settings (%d x %d) ignored by driver for  device %s.", w, h, _dev.c_str() );
			}
			else VNOTIFY( FAULT, "Failed to get window information for device %s: %m", _dev.c_str() );
		}
		else VNOTIFY( FAULT, "Failed to set window information for device %s: %m", _dev.c_str() );
	}
	else VNOTIFY( FAULT, "Attempted to set oversized size (%d x %d) for device %s: %m", w, h, _dev.c_str() );

	return ok;
}

bool camera::set_24_bit_color()
{
	bool ok = false;

	_vpic.depth = 24;
	_vpic.palette = VIDEO_PALETTE_RGB24;

    if ( ioctl( _fd, VIDIOCSPICT, &_vpic ) != -1 )
	{
		ok = true;
	}
	else VNOTIFY( FAULT, "Failed to set video_picture information for device %s: %m", _dev.c_str() );

	return ok;
}

bool camera::init_framebuffer( video_capture_mode capture_mode )
{
	bool ok = false;

	_vmap.width = _vwin.width;
	_vmap.height = _vwin.height;
	_vmap.format = _vpic.palette;
	_capmode = capture_mode;

	if( ioctl( _fd, VIDIOCGMBUF, &_vid_mbuf ) != -1 )
	{
		_vmap.frame = 0;

		switch ( _capmode )
		{
			case CAPTURE_VIA_MMAP:
			{
				_do_double_buffer = _vid_mbuf.frames > 1;
				
				VNOTIFY( VERBOSE, "Using %s buffering on device %s.", _do_double_buffer ? "double" : "single", _dev.c_str() );
				
				if ( release_framebuffer() )
				{
					if ( ( _frame_buf = reinterpret_cast< unsigned char * >( mmap(  0, _vid_mbuf.size, PROT_READ | PROT_WRITE, MAP_PRIVATE, _fd, getpagesize() ) ) ) != MAP_FAILED )
					{
						VNOTIFY( VERBOSE, "_frame_buf mmap() begins at 0x%08x for device %s", _frame_buf, _dev.c_str() );
						_frame_buf_sz = _vid_mbuf.size;
				
						if ( _do_double_buffer )
						{
							if ( ioctl( _fd, VIDIOCMCAPTURE, &_vmap ) != -1 ) 
							{
								ok = true;
							}
							else VNOTIFY( FAULT, "Failed to start capture of first frame for device %s: %m.", _dev.c_str() );
						}
						else ok = true;
					}
					else VNOTIFY( FAULT, "Failed to mmap() memory for device %s: %m", _dev.c_str() );
				}
				break;
			}

			case CAPTURE_VIA_READ:
			{
				_frame_buf_sz = _vmap.width * _vmap.height * _vpic.depth / 8;
				_frame_buf = new unsigned char[ _frame_buf_sz ];
				ok = true;
				break;
			}

			default: VNOTIFY( FAULT, "Invalid video_capture_mode: %d", _capmode );
		}
	}
	else VNOTIFY( FAULT, "Failed to read video mbuf information for device %s: %m", _dev.c_str() );

	return ok;
}

bool camera::release_framebuffer()
{
	bool ok = true;

	if ( _frame_buf != 0 )
	{
		switch ( _capmode )
		{
			case CAPTURE_VIA_MMAP:
			{
				if ( munmap( _frame_buf, _frame_buf_sz ) == -1 )
				{
					NOTIFY( FAULT, "Failed to munmap() frame buffer: %m" );
					ok = false;
				}
				break;
			}

			case CAPTURE_VIA_READ:
			{
				delete [] _frame_buf;
				ok = false;
				break;
			}

			default: VNOTIFY( FAULT, "Invalid video_capture_mode: %d", _capmode );
		}

		_frame_buf = 0;
	}

	return ok;
}

unsigned char *camera::next_frame()
{
	unsigned char *pixel_data = 0;

	switch ( _capmode )
	{
		case CAPTURE_VIA_MMAP:
			pixel_data = get_frame_via_mmap_sync();
			break;

		case CAPTURE_VIA_READ:
			pixel_data = get_frame_via_read();
			break;

		default: VNOTIFY( FAULT, "Invalid video_capture_mode: %d", _capmode );
	}

	return pixel_data;
}

unsigned char *camera::get_frame_via_mmap_sync()
{
	unsigned char *pixel_data = 0;

	int wait_on_frame = _vmap.frame;

	if ( _do_double_buffer )
	{
		// Swap buffers
		_vmap.frame = ( _vmap.frame == 0 ? 1 : 0 );
	}

	// Read into the 'back buffer' frame
    if ( ioctl( _fd, VIDIOCMCAPTURE, &_vmap ) != -1 )
	{
		// Sync with what we are considering to be the 'front buffer'
    	if ( ioctl( _fd, VIDIOCSYNC, &wait_on_frame ) != -1 )
		{
			pixel_data = _frame_buf + _vid_mbuf.offsets[ wait_on_frame ];
		}
		else VNOTIFY( FAULT, "Failed to sync capture for device %s: %m", _dev.c_str() );
	}
	else VNOTIFY( FAULT, "Failed to start VIDIOCMCAPTURE for device %s: %m", _dev.c_str() );

	return pixel_data;
}

unsigned char *camera::get_frame_via_read()
{
	int bytes_read;
	unsigned char *pixel_data = 0;

	while ( ( bytes_read = read( _fd, _frame_buf, _frame_buf_sz ) ) == -1 && errno == EINTR )
		/* Read again! */;

	if ( bytes_read == static_cast< int >( _frame_buf_sz ) )
	{
		pixel_data = _frame_buf;
	}
	else if ( bytes_read == -1 )
	{
		VNOTIFY( FAULT, "Error reading frame from device %s: %m", _dev.c_str() );
	}
	else if ( bytes_read == 0 )
	{
		VNOTIFY( FAULT, "Encountered EOF while reading frame from device %s: %m", _dev.c_str() );
	}
	else VNOTIFY( FAULT, "Short read from device %s: read %d bytes, wanted %d.", _dev.c_str(), bytes_read, _frame_buf_sz );

	return pixel_data;
}
