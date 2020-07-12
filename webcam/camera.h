#ifndef CAMERA_H
#define CAMERA_H

#include <string>
#include <linux/videodev.h>

using std::string;

class camera_exception
{
	string _why;

	public:
		camera_exception( string why ) : _why( why ) {}
		const char *what() { return _why.c_str(); }
};

enum video_capture_mode
{
	CAPTURE_VIA_MMAP,
	CAPTURE_VIA_READ
};

class camera
{
	private:
		int _fd;
		string _dev;
		size_t _frame_buf_sz;
		unsigned char *_frame_buf;
		bool _do_double_buffer;
		video_capability _vcap;
		video_mbuf _vid_mbuf;
		video_picture _vpic;
		video_window _vwin;
		video_mmap _vmap;
		video_capture_mode _capmode;

	public:
		camera( const char *dev, const int chan_no ) throw( camera_exception );
		~camera();
		bool set_size( unsigned w = 0, unsigned h = 0 );
		bool set_24_bit_color();
		bool init_framebuffer( video_capture_mode capture_mode );
		bool release_framebuffer();
		unsigned char *next_frame();
	
	private:
		unsigned char *get_frame_via_read();
		unsigned char *get_frame_via_mmap_sync();
};

#endif // CAMERA_H
