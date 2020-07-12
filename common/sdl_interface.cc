#include "sdl_interface.h"
#include "utilities.h"
#include "exceptions.h"

//////////////////////////////////////////////////////////////////////////////////
// Function definitions for SDL_Window:
//////////////////////////////////////////////////////////////////////////////////

SDL_Window::SDL_Window( const int w, const int h, const int bpp, const Uint32 flags, const std::string &title ) :
    screen_( 0 ),
    screen_width_( w ), 
    screen_height_( h ), 
    screen_bpp_( bpp ),
    sdl_video_flags_( flags ),
    title_( title )
{
}

void SDL_Window::createWindow()
{
    bool ok = false;

    if( SDL_Init( SDL_INIT_VIDEO ) != -1 )
    {
        if( ( screen_ = SDL_SetVideoMode( screen_width_, screen_height_, screen_bpp_, sdl_video_flags_ ) ) )
        {
            SDL_WM_SetCaption( title_.c_str(), NULL );
            SDL_FillRect( screen_, NULL, SDL_MapRGBA( screen_->format, 0, 0, 0, 0 ) );
        
            reshapeWindow();
            ok = true;
        }
        else VNOTIFY( FAULT, "SDL_SetVideoMode() failed: %s", SDL_GetError() );
    }
    else VNOTIFY( FAULT, "SDL_Init() failed: %s", SDL_GetError() );

    if ( !ok ) throw LoadingError( "Error creating SDL window." );
}

void SDL_Window::reshapeWindow( const int w, const int h )
{
    screen_width_ = w;
    screen_height_ = h;

    reshapeWindow();
}

void SDL_Window::flip()
{
    SDL_Flip( screen_ );
}

void SDL_Window::reshapeWindow()
{
    // TODO Only will happen if SDL_RESIZABLE is passed to SDL_SetVideoMode
}

//////////////////////////////////////////////////////////////////////////////////
// Function definitions for SDL_Interface:
//////////////////////////////////////////////////////////////////////////////////

SDL_Interface::SDL_Interface( SDL_Window &window, const int fps_limit ) :
    run_( false ),
    fps_limit_( fps_limit ),
    fps_( 0 ),
    window_( window )
{
    window_.createWindow();
}

SDL_Interface::~SDL_Interface()
{
    SDL_Quit();
}

void SDL_Interface::toggleFullscreen()
{
    SDL_Surface *s;

    s = SDL_GetVideoSurface();

    if( !s || ( SDL_WM_ToggleFullScreen( s ) != 1 ) )
    {
        VNOTIFY( FAULT, "Unable to toggle fullscreen: %s", SDL_GetError() );
    }
}

void SDL_Interface::processEvents( const bool doHandle )
{
    SDL_Event event;

    while( SDL_PollEvent( &event ) ) if ( doHandle ) handleEvent( event );
}

void SDL_Interface::handleEvent( SDL_Event &event )
{
    switch ( event.type )
    {
        case SDL_KEYDOWN:
            handleKeyDownEvent( event.key.keysym.sym, event.key.keysym.mod );
            break;

        case SDL_KEYUP:
            handleKeyUpEvent( event.key.keysym.sym, event.key.keysym.mod );
            break;

        case SDL_MOUSEMOTION:
            handleMouseMotionEvent( event.button.button, event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel );
            break;

        case SDL_MOUSEBUTTONDOWN:
            handleMouseDownEvent( event.button.button, event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel );
            break;

        case SDL_MOUSEBUTTONUP:
            handleMouseUpEvent( event.button.button, event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel );
            break;
    
        case SDL_VIDEORESIZE:
            window_.reshapeWindow( event.resize.w, event.resize.h );
            break;
    
        case SDL_QUIT:
            run_ = false;
            break;
    }
}

void SDL_Interface::handleKeyDownEvent( const int key, const int /* mod */ )
{
    switch ( key )
    {
        case SDLK_ESCAPE:
            run_ = false;
            break;

        case SDLK_F1:
            toggleFullscreen();
            break;
    }
}

void SDL_Interface::mainLoop()
{
    run_ = true;

    long fps_timer = last_step_time_ = SDL_GetTicks();

    while ( run_ )
    {
        long current_time = SDL_GetTicks();
        long step_time = current_time - last_step_time_;
        double step_time_seconds = static_cast<double>( step_time ) * 0.001;

        if ( ( fps_limit_ == 0 && step_time > 0 ) || step_time_seconds >= 1.0 / static_cast<double>( fps_limit_ ) )
        {
            last_step_time_ = current_time;

            // ++fps_;

            // // TODO
            // if ( fps_timer + 1000 < current_time )
            // {
            //     VNOTIFY( VERBOSE, "FPS: %d\n", fps_ );
            //     fps_ = 0;
            //     fps_timer = current_time;
            // }
            
            processEvents( true );
            doOneStep( step_time_seconds );
            
            draw();
        }
        else usleep( 1000 ); // TODO Maybe sleep until the next step?
    }
}
