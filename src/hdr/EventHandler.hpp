#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include "SDL/SDL.h"

class EventHandler{
    public:
        void eventType(SDL_Event* event);
        void checkEvents(SDL_Event* event);
        virtual void quitGame();
        virtual void keyPressedDown(SDLKey key);
        virtual void leftButtonDown(int x, int y);
        virtual void rightButtonDown(int x, int y);
};

#endif
