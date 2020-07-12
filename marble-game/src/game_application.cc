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

int world_to_screen( const float world )
{
     return static_cast<int>( roundf( world * 10.0f ) );
}

float screen_to_world( const int screen )
{
     return static_cast<float>( screen ) / 10.0f;
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
// Member function definitions for Drawable:
////////////////////////////////////////////////////////////////////////////////

Drawable::Drawable( SDL_Window &window ) :
    window_( window )
{
}

////////////////////////////////////////////////////////////////////////////////
// Member function definitions for Marble:
////////////////////////////////////////////////////////////////////////////////

Marble::Marble( SDL_Window &window, b2World *world, const b2Vec2 &position ) :
    Drawable( window ),
    image_( IMG_LoadFormatted( "/usr/share/pixmaps/marble-image.png" ) )
{
    b2BodyDef bodyDef;
    b2CircleDef shapeDef;

    bodyDef.position.Set( position.x, position.y );
    bodyDef.linearDamping = 0.3f;
    body_ = world->CreateBody( &bodyDef );

    shapeDef.radius = screen_to_world( image_->w ) / 2.0f;
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

void Marble::undraw( RectVector &update_rects )
{
    SDL_FillRect( window_.screen(), &erase_rect_, SDL_MapRGB( window_.screen()->format, 0, 0, 0 ) );
    update_rects.push_back( erase_rect_ );
}

void Marble::draw( RectVector &update_rects )
{
    SDL_Rect dest_rect;
    b2Vec2 position = body_->GetPosition();

    // The destination width and height are ignored by SDL_BlitSurface().
    set_draw_position( body_->GetPosition(), dest_rect );

    SDL_BlitSurface( image_, NULL, window_.screen(), &dest_rect );

    update_rects.push_back( dest_rect );

    erase_rect_.x = dest_rect.x;
    erase_rect_.y = dest_rect.y;
}

void Marble::set_draw_position( const b2Vec2 position, SDL_Rect &rect )
{
    rect.x = world_to_screen( position.x ) - image_->w / 2;
    rect.y = world_to_screen( position.y ) - image_->h / 2;
}

////////////////////////////////////////////////////////////////////////////////
// Member function definitions for Block:
////////////////////////////////////////////////////////////////////////////////

Block::Block( SDL_Window &window, b2World *world, const b2Vec2 &position, const b2Vec2 &size ) :
    Drawable( window )
{
    b2BodyDef bodyDef;
    b2PolygonDef shapeDef;

    bodyDef.position.Set( position.x, position.y );
    body_ = world->CreateBody( &bodyDef );
    shapeDef.SetAsBox( size.x / 2.0f, size.y / 2.0f );
    body_->CreateShape( &shapeDef );

    rect_.x = world_to_screen( position.x - size.x / 2.0f );
    rect_.y = world_to_screen( position.y - size.y / 2.0f );
    rect_.w = world_to_screen( size.x );
    rect_.h = world_to_screen( size.y );
}

void Block::undraw( RectVector &update_rects )
{
}

void Block::draw( RectVector &update_rects )
{
    SDL_FillRect( window_.screen(), &rect_, SDL_MapRGB( window_.screen()->format, 0, 0, 255 ) );
    update_rects.push_back( rect_ );
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

GameApplication::GameApplication( GameWindow &initializer, const int fps ) :
    SDL_Interface( initializer, fps ),
    accelerometer_( "/dev/input/event3" ),
    vibrator_( "/sys/class/leds/neo1973:vibrator/brightness" ),
    update_rects_( 256 ) // TODO magica overload!
{
    b2AABB worldAABB;
    worldAABB.lowerBound.Set( -100.0f, -100.0f );
    worldAABB.upperBound.Set(  100.0f,  100.0f );

    world_ = new b2World( worldAABB, b2Vec2( 0.0f, 0.0f ), false );

    world_->SetContactListener( &contact_listener_ );

    // outer wall bodies
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

    // new Block( window_, world_, b2Vec2( 24.0f,  -1.0f ), b2Vec2( 24.0f,  2.0f ) );
    // new Block( window_, world_, b2Vec2( 24.0f,  65.0f ), b2Vec2( 24.0f,  2.0f ) );
    // new Block( window_, world_, b2Vec2( -1.0f,  32.0f ), b2Vec2(  2.0f, 32.0f ) );
    // new Block( window_, world_, b2Vec2( 49.0f, -32.0f ), b2Vec2(  2.0f, 32.0f ) );

    drawables_.push_back( new Marble( window_, world_, b2Vec2( 24.0f, 48.0f ) ) );
    drawables_.push_back( new Marble( window_, world_, b2Vec2( 24.0f, 12.0f ) ) );
    // drawables_.push_back( new Marble( window_, world_, b2Vec2( 12.0f, 32.0f ) ) );
    // drawables_.push_back( new Marble( window_, world_, b2Vec2( 36.0f, 32.0f ) ) );

    drawables_.push_back( new Block( window_, world_, b2Vec2( 12.0f, 16.0f ), b2Vec2( 12.0f, 2.0f ) ) );
    drawables_.push_back( new Block( window_, world_, b2Vec2( 36.0f, 16.0f ), b2Vec2( 12.0f, 2.0f ) ) );
    drawables_.push_back( new Block( window_, world_, b2Vec2( 24.0f, 32.0f ), b2Vec2( 12.0f, 2.0f ) ) );
    drawables_.push_back( new Block( window_, world_, b2Vec2( 12.0f, 48.0f ), b2Vec2( 12.0f, 2.0f ) ) );
    drawables_.push_back( new Block( window_, world_, b2Vec2( 36.0f, 48.0f ), b2Vec2( 12.0f, 2.0f ) ) );
}

GameApplication::~GameApplication()
{
    delete world_;
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
                       -static_cast<double>( reading.acceleration_[AXIS_Y] ) * ACCELERATION_FACTOR );

        world_->SetGravity( gravity );
    }

    contact_listener_.was_collision_ = false;

    world_->Step( step_time, 10 ); // TODO constantize 10

    if ( contact_listener_.was_collision_ ) vibrator_.pulse( contact_listener_.collision_strength_, 150 );

    vibrator_.step();
}

void GameApplication::draw()
{
    SDL_Rect obstacle_rect;

    update_rects_.clear();

    for ( vector<Drawable *>::iterator drawable = drawables_.begin(); drawable != drawables_.end(); ++drawable )
    {
        (*drawable)->undraw( update_rects_ );
    }

    for ( vector<Drawable *>::iterator drawable = drawables_.begin(); drawable != drawables_.end(); ++drawable )
    {
        (*drawable)->draw( update_rects_ );
    }

    // obstacle_rect.x = 130;
    // obstacle_rect.y = 310;
    // obstacle_rect.w = 220;
    // obstacle_rect.h = 20;

    // update_rects_.push_back( obstacle_rect );

    // SDL_FillRect( window_.screen(), &obstacle_rect, SDL_MapRGB( window_.screen()->format, 255, 255, 255 ) );

    SDL_UpdateRects( window_.screen(), update_rects_.size(), &update_rects_[0] );
}

