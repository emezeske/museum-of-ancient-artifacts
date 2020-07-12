#ifndef GAME_APPLICATION_H
#define GAME_APPLICATION_H

#include <vector>

#include "sdl_interface.h"
#include "accelerometer.h"
#include "vibrator.h"
#include "Box2D.h"

typedef std::vector<SDL_Rect> RectVector;

class ContactListener : public b2ContactListener
{
    public:
        void Add( const b2ContactPoint *point );
        void Persist( const b2ContactPoint *point );
        void Remove( const b2ContactPoint *point );

        bool was_collision_;
        float collision_strength_;
};

class Drawable
{
    public:
        Drawable( SDL_Window &window );
        virtual ~Drawable() {};

        virtual void undraw( RectVector &update_rects ) = 0;
        virtual void draw( RectVector &update_rects ) = 0;

        SDL_Window &window_;
};

class Marble : public Drawable
{
    public:
        Marble( SDL_Window &window, b2World *world, const b2Vec2 &position );
        ~Marble();

        void undraw( RectVector &update_rects );
        void draw( RectVector &update_rects );

    protected:
        void set_draw_position( const b2Vec2 position, SDL_Rect &rect );

        b2Body *body_;
        SDL_Surface *image_;
        SDL_Rect erase_rect_;
};

class Block : public Drawable
{
    public:
        Block( SDL_Window &window, b2World *world, const b2Vec2 &position, const b2Vec2 &size );

        void undraw( RectVector &update_rects );
        void draw( RectVector &update_rects );

    protected:
        b2Body *body_;
        SDL_Rect rect_;
};

////////////////////////////////////////////////////////////////////////////////
// The GameWindow class takes care of the SDL window initialization.
////////////////////////////////////////////////////////////////////////////////

class GameWindow : public SDL_Window
{
    public:
        GameWindow( const int w, const int h, const int bpp, const Uint32 flags, const std::string &title );
        virtual ~GameWindow() {}

        virtual void createWindow();
};

////////////////////////////////////////////////////////////////////////////////
// The GameApplication class represents a running instance of the game.
////////////////////////////////////////////////////////////////////////////////

class GameApplication : public SDL_Interface
{
    public:
        GameApplication( GameWindow &initializer, const int fps );
        virtual ~GameApplication();

    protected:
        virtual void handleMouseMotionEvent( const int button, const int x, const int y, const int xrel, const int yrel );
        virtual void handleMouseDownEvent( const int button, const int x, const int y, const int xrel, const int yrel );
        virtual void doOneStep( const double stepTime );
        virtual void draw();

        static const double ACCELERATION_FACTOR = 0.15f;

        AccelerometerReader accelerometer_;
        Vibrator vibrator_;
        b2World *world_;
        ContactListener contact_listener_;
        //Marble *marble_;
        std::vector<Drawable *> drawables_;
        RectVector update_rects_;
};

#endif // GAME_APPLICATION_H
