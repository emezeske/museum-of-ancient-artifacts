#ifndef SDL_INTERFACE_H
#define SDL_INTERFACE_H

#include <SDL/SDL.h>
#include <string>

#include "utilities.h"

class SDL_Window
{
    public:
        SDL_Window( const int w, const int h, const int bpp, const Uint32 flags, const std::string &title ); // May throw a LoadingError().
        virtual ~SDL_Window() {}

        virtual void createWindow();
        virtual void reshapeWindow( const int w, const int h );
        virtual void flip();
        virtual SDL_Surface *screen() { return screen_; }

        virtual int width() const { return screen_width_; }
        virtual int height() const { return screen_height_; }

    protected:
        virtual void reshapeWindow();

		SDL_Surface *screen_;

		int
			screen_width_,
			screen_height_,
			screen_bpp_,
			sdl_video_flags_;

        std::string title_;
};

class SDL_Interface
{
	public:
        SDL_Interface( SDL_Window &initializer, const int fps_limit );
		virtual ~SDL_Interface();

		virtual void toggleFullscreen();
		virtual void mainLoop();

	protected:
		virtual void processEvents( const bool doHandle );
		virtual void handleEvent( SDL_Event &event );
		virtual void handleKeyDownEvent( const int key, const int mod );
		virtual void handleKeyUpEvent( const int /* key */, const int /* mod */ ) {}
		virtual void handleMouseMotionEvent( const int /* button */, const int /* x */, const int /* y */, const int /* xrel */, const int /* yrel */ ) {}
		virtual void handleMouseDownEvent( const int /* button */, const int /* x */, const int /* y */, const int /* xrel */, const int /* yrel */ ) {}
		virtual void handleMouseUpEvent( const int /* button */, const int /* x */, const int /* y */, const int /* xrel */, const int /* yrel */ ) {}
		virtual void doOneStep( const double stepTime ) = 0;
		virtual void draw() = 0;

		bool run_;

		long last_step_time_;

        int 
            fps_limit_,
            fps_;

        SDL_Window &window_;
};

#endif // SDL_INTERFACE_H
