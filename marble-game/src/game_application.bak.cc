#include <SDL/SDL_image.h>
#include <math.h>

#include "utilities.h"
#include "sdl_utilities.h"
#include "exceptions.h"
#include "game_application.h"

using namespace std;

SDL_Surface *IMG_LoadFormatted( const string &filename ) // TODO Move me
{
    SDL_Surface 
        *surface = IMG_Load( filename.c_str() ),
        *formatted_surface = 0;

    if ( surface )
    {
        formatted_surface = SDL_DisplayFormatAlpha( surface ); // TODO NON alpha?
        SDL_FreeSurface( surface );
    }

    if ( !surface ) throw LoadingError( "Failed to load texture file: " + filename );

    return formatted_surface;
}

////////////////////////////////////////////////////////////////////////////////
// Member function definitions for ContactListener:
////////////////////////////////////////////////////////////////////////////////

void ContactListener::Add( const b2ContactPoint *point )
{
    //printf( "Add\n" );
    was_collision_ = true;

    collision_strength_ = point->velocity.Length() / 50.0f; // TODO Holy arbitrary magic constant, batman!
}

void ContactListener::Persist( const b2ContactPoint *point )
{
    //printf( "Persist\n" );
}

void ContactListener::Remove( const b2ContactPoint *point )
{
    //printf( "Remove\n" );
}

////////////////////////////////////////////////////////////////////////////////
// Member function definitions for Marble:
////////////////////////////////////////////////////////////////////////////////

Marble::Marble( SDL_Window *window, b2World *world, const b2Vec2 position )
    window_( window ),
    image_( IMG_LoadFormatted( "/usr/share/pixmaps/marble-image.png" ) ),
{
    b2BodyDef bodyDef;
    b2CircleDef shapeDef;

    bodyDef.position.Set( position.x, position.y );
    body_ = world->CreateBody( &bodyDef );

    shapeDef.radius = 0.8f;
    shapeDef.density = 1.0f;
    shapeDef.friction = 0.3f;
    shapeDef.restitution = 0.3f;

    body_->CreateShape( &shapeDef );
    body_->SetMassFromShapes();

    set_draw_position( position, erase_rect_ );
    erase_rect_.w = image_->w;
    erase_rect_.h = image_->h;
}

Marble::~Marble()
{
    SDL_FreeSurface( image_ );
}

void Marble::undraw()
{
    SDL_FillRect( window_.screen(), &erase_rect_, SDL_MapRGB( window_.screen()->format, 0, 0, 0 ) );
    SDL_UpdateRect( window_.screen(), erase_rect_.x, erase_rect_.y, erase_rect_.w, erase_rect_.h );
}

void Marble::draw()
{
    SDL_Rect dest_rect;
    b2Vec2 position = body_->GetPosition();

    // The destination width and height are ignored by SDL_BlitSurface().
    set_draw_position( body_->GetPosition(), dest_rect );

    SDL_BlitSurface( image_, NULL, window_.screen(), &dest_rect );
    SDL_UpdateRect( window_.screen(), dest_rect.x, dest_rect.y, dest_rect.w, dest_rect.h );

    erase_rect_.x = dest_rect.x;
    erase_rect_.y = dest_rect.y;
}

int Marble::world_to_screen_x( const b2Vec2 world_position )
{
     return static_cast<int>( round( world_position.x * 10.0f ) );
}

int Marble::world_to_screen_y( const b2Vec2 world_position )
{
     return window_.height() - static_cast<int>( round( world_position.y * 10.0f ) );
}

void Marble::set_draw_position( const b2Vec2 position, SDL_Rect &rect )
{
    rect_.x = world_to_screen_x( position ) - image_->w / 2;;
    rect_.y = world_to_screen_y( position ) - image_->h / 2;;
}

////////////////////////////////////////////////////////////////////////////////
// Member function definitions for GameWindow:
////////////////////////////////////////////////////////////////////////////////

GameWindow::GameWindow( const int w, const int h, const int bpp, const Uint32 flags, const std::string &title ) : 
    SDL_Window( w, h, bpp, flags, title )
{
}

void GameWindow::createWindow()
{
    SDL_Window::createWindow();

    SDL_ShowCursor( SDL_DISABLE );
}

////////////////////////////////////////////////////////////////////////////////
// Member function definitions for GameApplication:
////////////////////////////////////////////////////////////////////////////////

GameApplication::GameApplication( SDL_Window &initializer, const int fps ) :
    SDL_Interface( initializer, fps ),
    accelerometer_( "/dev/input/event3" ),
    vibrator_( "/sys/class/leds/neo1973:vibrator/brightness" ),
    erase_position_x_( window_.width() / 2 ),
    erase_position_y_( window_.height() / 2 )
{
    b2AABB worldAABB;
    worldAABB.lowerBound.Set( -100.0f, -100.0f );
    worldAABB.upperBound.Set(  100.0f,  100.0f );

    world_ = new b2World( worldAABB, b2Vec2( 0.0f, 0.0f ), false );

    world_->SetContactListener( &contact_listener_ );

    /////////////////////////////// create ground bodies
    b2BodyDef groundBodyDef;
    b2PolygonDef groundShapeDef;

    groundBodyDef.position.Set( 24.0f, -1.0f );
    b2Body *bottomWall = world_->CreateBody( &groundBodyDef );
    groundShapeDef.SetAsBox( 24.0f, 2.0f );
    bottomWall->CreateShape( &groundShapeDef );

    groundBodyDef.position.Set( 24.0f, 65.0f );
    b2Body *topWall = world_->CreateBody( &groundBodyDef );
    groundShapeDef.SetAsBox( 24.0f, 2.0f );
    topWall->CreateShape( &groundShapeDef );

    groundBodyDef.position.Set( -1.0f, 32.0f );
    b2Body *leftWall = world_->CreateBody( &groundBodyDef );
    groundShapeDef.SetAsBox( 2.0f, 32.0f );
    leftWall->CreateShape( &groundShapeDef );

    groundBodyDef.position.Set( 49.0f, 32.0f );
    b2Body *rightWall = world_->CreateBody( &groundBodyDef );
    groundShapeDef.SetAsBox( 2.0f, 32.0f );
    rightWall->CreateShape( &groundShapeDef );

    // obstacle body
    groundBodyDef.position.Set( 24.0f, 32.0f );
    b2Body *obstacle = world_->CreateBody( &groundBodyDef );
    groundShapeDef.SetAsBox( 12.0f, 2.0f );
    obstacle->CreateShape( &groundShapeDef );

}

GameApplication::~GameApplication()
{
    delete world_;

    SDL_FreeSurface( marble_image_ );
}

void GameApplication::handleMouseMotionEvent( const int button, const int x, const int y, const int xrel, const int yrel )
{
}

void GameApplication::handleMouseDownEvent( const int button, const int x, const int y, const int /* xrel */, const int /* yrel */ )
{
    switch ( button )
    {
        case SDL_BUTTON_LEFT:
            run_ = false;
            break;

        case SDL_BUTTON_RIGHT:
            break;
    }
}

void GameApplication::doOneStep( const double step_time )
{
    AccelerometerReading reading;

    if ( accelerometer_.read( reading ) ) 
    {
        b2Vec2 gravity( static_cast<double>( reading.acceleration_[AXIS_X] ) * ACCELERATION_FACTOR,
                        static_cast<double>( reading.acceleration_[AXIS_Y] ) * ACCELERATION_FACTOR );

        world_->SetGravity( gravity );
    }

    contact_listener_.was_collision_ = false;

    world_->Step( step_time, 10 ); // TODO constantize 10

    if ( contact_listener_.was_collision_ ) vibrator_.pulse( contact_listener_.collision_strength_, 150 );

    vibrator_.step();
}

void GameApplication::draw()
{
    SDL_Rect dest_rect, erase_rect, obstacle_rect;

    b2Vec2 position = marble_body_->GetPosition();

    // The destination width and height are ignored by SDL_BlitSurface().
    dest_rect.x = static_cast<int>( round( position.x * 10.0f ) ) - marble_image_->w / 2;
    dest_rect.y = window_.height() - static_cast<int>( round( position.y * 10.0f ) ) - marble_image_->h / 2;

    erase_rect.x = erase_position_x_;
    erase_rect.y = erase_position_y_;
    erase_rect.w = marble_image_->w;
    erase_rect.h = marble_image_->h;

    obstacle_rect.x = 120;
    obstacle_rect.y = 310;
    obstacle_rect.w = 240;
    obstacle_rect.h = 20;

    SDL_FillRect( window_.screen(), &obstacle_rect, SDL_MapRGB( window_.screen()->format, 255, 255, 255 ) );
    SDL_UpdateRect( window_.screen(), obstacle_rect.x, obstacle_rect.y, obstacle_rect.w, obstacle_rect.h );

    SDL_FillRect( window_.screen(), &erase_rect, SDL_MapRGB( window_.screen()->format, 0, 0, 0 ) );
    SDL_BlitSurface( marble_image_, NULL, window_.screen(), &dest_rect );

    SDL_UpdateRect( window_.screen(), erase_rect.x, erase_rect.y, erase_rect.w, erase_rect.h );
    SDL_UpdateRect( window_.screen(), dest_rect.x, dest_rect.y, dest_rect.w, dest_rect.h );

    erase_position_x_ = dest_rect.x;
    erase_position_y_ = dest_rect.y;
}

