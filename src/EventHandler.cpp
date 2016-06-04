#include "hdr/EventHandler.hpp"

void EventHandler::eventType(SDL_Event* event){
    switch(event->type){
        case SDL_KEYDOWN:
            keyPressedDown(event->key.keysym.sym);
            break;
        case SDL_MOUSEBUTTONDOWN:
            switch(event->button.button){
                case SDL_BUTTON_LEFT:
                    leftButtonDown(event->button.x, event->button.y);
                    break;
                case SDL_BUTTON_RIGHT:
                    rightButtonDown(event->button.x, event->button.y);
                    break;
            }
            break;
        case SDL_QUIT:
            quitGame();
            break;
    }
}

void EventHandler::checkEvents(SDL_Event* event){
    while (SDL_PollEvent(event))
        eventType(event);
}

void EventHandler::quitGame(){}
void EventHandler::keyPressedDown(SDLKey key){}
void EventHandler::leftButtonDown(int x, int y){}
void EventHandler::rightButtonDown(int x, int y){}
