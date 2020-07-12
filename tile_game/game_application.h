#ifndef GAME_APPLICATION_H
#define GAME_APPLICATION_H

#include <vector>
#include <boost/ptr_container/ptr_list.hpp>

#include "sdl_gl_interface.h"
#include "camera.h"
#include "game_map.h"
#include "drone.h"

////////////////////////////////////////////////////////////////////////////////
// The GameWindow class takes care of the SDL/OpenGL window initialization.
////////////////////////////////////////////////////////////////////////////////

class GameWindow : public SDL_GL_Window
{
    public:
        GameWindow( const int w, const int h, const int bpp, const Uint32 flags, const std::string &title );
        virtual ~GameWindow() {}

        virtual void createWindow();

    protected:
        virtual void reshapeWindow();
};

////////////////////////////////////////////////////////////////////////////////
// The GameApplication class represents a running instance of the game.
////////////////////////////////////////////////////////////////////////////////

class GameApplication : public SDL_GL_Interface
{
    public:
        GameApplication( SDL_GL_Window &initializer, const int fps );
        virtual ~GameApplication();

    protected:
        typedef boost::ptr_list<Drone> DroneList;

        static const float DRONE_INTERVAL = 0.1f;

        virtual void handleKeyDownEvent( const int key, const int mod );
        virtual void handleKeyUpEvent( const int key, const int mod );
        virtual void handleMouseMotionEvent( const int button, const int x, const int y, const int xrel, const int yrel );
        virtual void handleMouseDownEvent( const int button, const int x, const int y, const int xrel, const int yrel );
        virtual void doOneStep( const double stepTime );
        virtual void draw();

        virtual void setMouseCoordinates( const int x, const int y );
        virtual Vector2Di cursorTilePosition() const;
        virtual void updateCameraMovement();

        Camera camera_;
        GameMap map_;
        DroneList drones_;

        TeslaTower cursor_tower_;

        Vector2Di mouse_coordinates_;

        double drone_timer_;

        // TODO This is silly nonsense -- I should use a map<int, bool> to keep track of depressed keys.
        bool
            up_key_is_down_,
            down_key_is_down_,
            left_key_is_down_,
            right_key_is_down_;
};

#endif // GAME_APPLICATION_H
