#ifndef MINESWEEPER_H
#define MINESWEEPER_H

#include <vector>
#include <list>
#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <string>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include "EventHandler.hpp"
#include "Render.hpp"
#include "SDL/SDL_ttf.h"
//#undef main
using namespace std;

#define PATH "./"
#define IMAGE_PATH "minesImg/images.png"
#define FONT_PATH "fonts/arial.ttf"
#define BOMB 9
#define WIDTH 30
#define HEIGHT 30
#define BLANK 0
#define OFFSET 30 //px
#define NANOSEC 100000000
SDL_Color textColor = {255,255,255};

typedef list<vector<Uint32>> listVector;

class Minesweeper: public EventHandler{
    private:
        Uint32 SIZE_ROW, SIZE_COL, BOMB_TOTAL;
        string TOP_SCORE;
        mutex mtx;
        stringstream cheat;
        bool cheatActived;
        void checkCheat();

        time_t startTime;
        bool initTime;

        struct square{
            Uint32 value = 0;
            bool visibility = false;
            bool flag = false;
        };

        SDL_Surface* displayVideo, *images;
        TTF_Font* textFont;
        bool gameOver, done;
        Uint32 flagsQuantity, visibilityQuantity;

        vector< vector< square > > mineTable;
        vector< vector < Uint32 > > minesCoord;
        unordered_map<Uint32,Uint32> digitsCoord;

        SDL_Surface* getTextSurface(const char*);

        void randomizeCoords(Uint32*,Uint32*);
        void incrementKeyDigit(Uint32,Uint32);
        listVector getAroundCoords(Uint32,Uint32);
        bool isPlayerWinner();
        void renderGame();
        void renderText(SDL_Surface*,int,int,int,int);
        void renderFlagText();
        void renderTimerText();
        void revealBlankPositions(Uint32,Uint32);
        void showMines();
        void waitPlayer(SDL_Event*);
        void resetParams();
        void resetGame();
        const char* getTimer();
        string getScoreFilename();
        void saveScore();
        string getTopScore();

    public:
        Minesweeper();
        void setGameParams(int, int, int);
        bool initGame();
        void startGame();
        void insertMines();
        void insertDigits();
        void printMines();

        void quitGame();
        void keyPressedDown(SDLKey);
        void leftButtonDown(int, int);
        void rightButtonDown(int, int);
};

#endif
