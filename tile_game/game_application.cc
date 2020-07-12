#include <SDL/SDL_opengl.h>

#include "utilities.h"
#include "game_application.h"

////////////////////////////////////////////////////////////////////////////////
// Member function definitions for GameWindow:
////////////////////////////////////////////////////////////////////////////////

GameWindow::GameWindow( const int w, const int h, const int bpp, const Uint32 flags, const std::string &title ) : 
    SDL_GL_Window( w, h, bpp, flags, title )
{
}

void GameWindow::createWindow()
{
    SDL_GL_Window::createWindow();

    // SDL_ShowCursor( SDL_DISABLE ); // Use a custom cursor?
    // SDL_WM_GrabInput( SDL_GRAB_ON );
}

void GameWindow::reshapeWindow() // Override the default reshapeWindow(), to use an orthogonal projection matrix.
{
  	glMatrixMode( GL_PROJECTION );
  	glLoadIdentity();

	glOrtho( 0, screen_width_, 0, screen_height_, 0, 1.0 );

  	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
}

////////////////////////////////////////////////////////////////////////////////
// Member function definitions for GameApplication:
////////////////////////////////////////////////////////////////////////////////

GameApplication::GameApplication( SDL_GL_Window &initializer, const int fps ) :
    SDL_GL_Interface( initializer, fps ),
    camera_( Vector2Df( 0.0f, -512.0f ) ),
    map_( "bg.png", Vector2Di( 32, 32 ) ),
    drone_timer_( DRONE_INTERVAL ),
    up_key_is_down_( false ),
    down_key_is_down_( false ),
    left_key_is_down_( false ),
    right_key_is_down_( false )
{
    cursor_tower_.setHighlightMode( Structure::highlight_unplaced );
}

GameApplication::~GameApplication()
{
}

void GameApplication::handleKeyDownEvent( const int key, const int mod )
{
    bool handled = true;

    switch ( key )
    {
        case SDLK_w:
            up_key_is_down_ = true;
            break;

        case SDLK_s:
            down_key_is_down_ = true;
            break;

        case SDLK_a:
            left_key_is_down_ = true;
            break;

        case SDLK_d:
            right_key_is_down_ = true;
            break;

        case SDLK_r:
            break;

        case SDLK_SPACE:
            break;

        default:
            handled = false;
            break;
    }

    updateCameraMovement();

    if ( !handled ) SDL_GL_Interface::handleKeyDownEvent( key, mod );
}

void GameApplication::handleKeyUpEvent( const int key, const int /* mod */ )
{
    switch ( key )
    {
        case SDLK_w:
            up_key_is_down_ = false;
            break;

        case SDLK_s:
            down_key_is_down_ = false;
            break;

        case SDLK_a:
            left_key_is_down_ = false;
            break;

        case SDLK_d:
            right_key_is_down_ = false;
            break;
    }

    updateCameraMovement();
}

void GameApplication::handleMouseMotionEvent( const int button, const int x, const int y, const int xrel, const int yrel )
{
    setMouseCoordinates( x, y );
}

void GameApplication::handleMouseDownEvent( const int button, const int x, const int y, const int /* xrel */, const int /* yrel */ )
{
    setMouseCoordinates( x, y );
    Vector2Di cursor_tile_position = cursorTilePosition();

    switch ( button )
    {
        case SDL_BUTTON_LEFT:
            if ( map_.setStructure( cursor_tile_position, new TeslaTower() ) )
            {
                // FIXME
                // for ( DroneList::iterator d = drones_.begin(); d != drones_.end(); ++d )
                // {
                //     if ( d->pathIncludes( cursor_tile_position ) )
                //     {
                //         d->setDestination( map_, BASE_B );
                //     }
                // }
            }
            break;

        case SDL_BUTTON_RIGHT:
            if ( map_.removeStructure( cursor_tile_position ) )
            {
                // FIXME
                // for ( DroneList::iterator d = drones_.begin(); d != drones_.end(); ++d )
                // {
                //     d->setDestination( map_, BASE_B );
                // }
            }
            break;
    }
}

void GameApplication::doOneStep( const double step_time )
{
    camera_.doOneStep( step_time );

    Vector2Di cursor_tile_position = cursorTilePosition();
    cursor_tower_.setTilePosition( cursor_tile_position );
    map_.attachToTile( cursor_tile_position, &cursor_tower_ );

    if ( ( drone_timer_ += step_time ) > DRONE_INTERVAL )
    {
        drones_.push_back( new HoverDrone( map_, GameMap::centerOfTile( map_.basePosition( BASE_A ) ), BASE_B ) );
        drone_timer_ = 0.0f;
    }

    for ( DroneList::iterator d = drones_.begin(); d != drones_.end(); /* incremented in body */ )
    {
        if ( !d->readyForCleanup() )
        {
            d->doOneStep( step_time );
            d->attachToTile();
            ++d;
        }
        else drones_.erase( d++ );
    }
}   

void GameApplication::draw()
{
    camera_.draw(); // Perform camera based translation.
    map_.draw();
}

void GameApplication::setMouseCoordinates( const int x, const int y )
{
    mouse_coordinates_ = Vector2Di( x, window_.height() - 1 - y );
    updateCameraMovement();
}

Vector2Di GameApplication::cursorTilePosition() const
{
    return GameMap::absoluteToIndex( dimetricToWorld( camera_.translateMouseCoordinates( mouse_coordinates_ ) ) );
}

void GameApplication::updateCameraMovement()
{
    camera_.moveLeft(  ( mouse_coordinates_.x_ == 0                    ) || left_key_is_down_  );
    camera_.moveRight( ( mouse_coordinates_.x_ == window_.width() - 1  ) || right_key_is_down_ );
    camera_.moveDown(  ( mouse_coordinates_.y_ == 0                    ) || down_key_is_down_  );
    camera_.moveUp(    ( mouse_coordinates_.y_ == window_.height() - 1 ) || up_key_is_down_    );
}
