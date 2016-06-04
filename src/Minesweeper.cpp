#include "hdr/Minesweeper.hpp"

Minesweeper::Minesweeper(){
  done = false;
  displayVideo = NULL;
  images = NULL;
  textFont = NULL;
  flagsQuantity = 0;
  resetParams();
}

void Minesweeper::resetParams(){
  visibilityQuantity = 0;
  gameOver = false;
  initTime = false;
  startTime = 0;
  cheatActived = false;
  srand(time(NULL));
}

void Minesweeper::resetGame(){
  resetParams();
  flagsQuantity = BOMB_TOTAL;

  mineTable.clear();
  minesCoord.clear();
  digitsCoord.clear();

  vector<square> rows(SIZE_COL);
  mineTable.resize(SIZE_ROW,rows);

  TOP_SCORE = getTopScore();

  insertMines();
  insertDigits();
}

string Minesweeper::getTopScore(){
  ifstream file;

  file.open(getScoreFilename().c_str(), ifstream::in);

  if (!file.is_open()) return string("-");

  string line, bestWord;
  int hour, min, sec, total, best = 24*60*60;
  while (getline(file, line)){
    sscanf(line.c_str(), "%d:%d:%d", &hour, &min, &sec);
    total = (hour * 60 * 60) + (min * 60) + sec;
    if (total < best){
      best = total;
      bestWord = string(line);
    }
  }
  return bestWord;
}

void Minesweeper::setGameParams(int row, int col, int bomb_total){
  SIZE_ROW = (Uint32)row;
  SIZE_COL = (Uint32)col;
  BOMB_TOTAL = (Uint32)bomb_total;
  TOP_SCORE = getTopScore();
  flagsQuantity = BOMB_TOTAL;
  vector<square> rows(SIZE_COL);
  mineTable.resize(SIZE_ROW,rows);
}

void Minesweeper::insertMines(){
  Uint32 row,col;
  for(Uint32 i = 0; i < BOMB_TOTAL; i++){
    randomizeCoords(&row,&col);
    while (mineTable[row][col].value == BOMB)
      randomizeCoords(&row,&col);
    mineTable[row][col].value = BOMB;
    minesCoord.push_back({row,col});
  }
}

void Minesweeper::insertDigits(){
  for(Uint32 i = 0; i < BOMB_TOTAL; i++)
    for(auto& coords: getAroundCoords(minesCoord[i][0], minesCoord[i][1]))
      incrementKeyDigit(coords[0],coords[1]);
  for(auto& itMap: digitsCoord){
    Uint32 row = itMap.first / SIZE_COL;
    Uint32 col = itMap.first % SIZE_COL;
    mineTable[row][col].value = itMap.second;
  }
}

listVector Minesweeper::getAroundCoords(Uint32 row, Uint32 col){
  listVector coords;
  int neighbours[][2] = {{0,1},{0,-1},{-1,0},{-1,1},{-1,-1},{1,0},
    {1,1},{1,-1}};
  for (Uint32 i = 0; i < 8; i++){
    int newRow = row + neighbours[i][0];
    int newCol = col + neighbours[i][1];
    if (newRow >= 0 && newRow < (int)SIZE_ROW && newCol >= 0 &&
        newCol < (int)SIZE_COL)
      coords.push_back({(Uint32)newRow,(Uint32)newCol});
  }
  return coords;
}

void Minesweeper::incrementKeyDigit(Uint32 row, Uint32 col){
  Uint32 coord = row * SIZE_COL + col;
  if (mineTable[row][col].value != BOMB){
    if (digitsCoord.find(coord) == digitsCoord.end()){
      digitsCoord[coord] = 1;
    }
    else
      digitsCoord[coord]++;
  }
}

bool Minesweeper::isPlayerWinner(){
  if (visibilityQuantity == (SIZE_ROW * SIZE_COL - BOMB_TOTAL)
      && flagsQuantity == 0)
    return true;
  else
    return false;
}

void Minesweeper::quitGame(){
  done = true;
}

void Minesweeper::keyPressedDown(SDLKey key){
  if (key == SDLK_ESCAPE)
    quitGame();
  else if (key == SDLK_r)
    resetGame();
  else if (key == SDLK_s){
    cheat << "s";
  }
  else if (key == SDLK_h){
    cheat << "h";
  }
  else if (key == SDLK_o){
    cheat << "o";
  }
  else if (key == SDLK_w){
    cheat << "w";
  }
}

void Minesweeper::leftButtonDown(int x, int y){
  if (!initTime){
    startTime = time(NULL);
    initTime = true;
  }
  if (y - OFFSET < 0) return;
  Uint32 col = x / WIDTH;
  Uint32 row = (y - OFFSET) / HEIGHT;
  if (mineTable[row][col].flag == true ||
      mineTable[row][col].visibility == true) return;
  mineTable[row][col].visibility = true;
  if (mineTable[row][col].value == BOMB){
    gameOver = true;
    showMines();
  }
  else if (mineTable[row][col].value != BLANK) visibilityQuantity++;
  else{
    visibilityQuantity++;
    revealBlankPositions(row,col);
  }
}

void Minesweeper::revealBlankPositions(Uint32 row, Uint32 col){
  for(auto& coords: getAroundCoords(row,col)){
    if (mineTable[coords[0]][coords[1]].visibility == true ||
        mineTable[coords[0]][coords[1]].flag == true) continue;
    mineTable[coords[0]][coords[1]].visibility = true;
    visibilityQuantity++;
    if (mineTable[coords[0]][coords[1]].value == BLANK)
      revealBlankPositions(coords[0],coords[1]);
  }
}

void Minesweeper::rightButtonDown(int x, int y){
  if (gameOver) return;
  if (y - OFFSET < 0 || isPlayerWinner()) return;
  int col = x / WIDTH;
  int row = (y - OFFSET) / HEIGHT;

  if (mineTable[row][col].visibility == true && 
      mineTable[row][col].value == BOMB){
    mineTable[row][col].visibility = false;
    return;
  }

  if (mineTable[row][col].visibility == true) return;
  if (mineTable[row][col].flag == false && flagsQuantity > 0){
    mineTable[row][col].flag = true;
    flagsQuantity--;
  }
  else if (mineTable[row][col].flag == true){
    mineTable[row][col].flag = false;
    flagsQuantity++;
  }
}

void Minesweeper::randomizeCoords(Uint32* row, Uint32* col){
  *row = rand() % SIZE_ROW;
  *col = rand() % SIZE_COL;
}

bool Minesweeper::initGame(){
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0 || TTF_Init() < 0){
    cout << SDL_GetError() << endl;
    return false;
  }

  displayVideo = SDL_SetVideoMode(WIDTH * SIZE_COL, HEIGHT * SIZE_ROW + OFFSET,
      32, SDL_HWSURFACE | SDL_DOUBLEBUF);

  images = Render::assignImage(IMAGE_PATH);

  textFont = TTF_OpenFont(FONT_PATH, 23);

  if (displayVideo == NULL || images == NULL || textFont == NULL){
    cout << SDL_GetError() << endl;
    return false;
  }

  SDL_WM_SetCaption("Minesweeper", "Mines");

  return true;
}

void Minesweeper::waitPlayer(SDL_Event* event){
  while ((isPlayerWinner() || gameOver) && !done){
    checkEvents(event);
    struct timespec timespec_st = {0,NANOSEC};
    nanosleep(&timespec_st,NULL);
  }
}

void Minesweeper::checkCheat(){
  if (((int) cheat.str().find("show")) != -1){
    cheatActived = true;
    showMines();
    cheat.str(string());
  }
}

void Minesweeper::startGame(){
  SDL_Event event;
  thread timer(&Minesweeper::renderTimerText,this);
  while (!done){
    checkEvents(&event);
    mtx.lock();
    renderGame();
    mtx.unlock();
    if (isPlayerWinner() || gameOver){
      if (isPlayerWinner() && !cheatActived)
        saveScore();
      waitPlayer(&event);
    }
    checkCheat();
    struct timespec timespec_st = {0,NANOSEC};
    nanosleep(&timespec_st,NULL); 
    //used to not consume 100% of the cpu 0,1s
  }
  timer.join();
  SDL_FreeSurface(displayVideo);
  SDL_FreeSurface(images);
  SDL_Quit();
  TTF_Quit();
}

void Minesweeper::saveScore(){
  ofstream file;

  file.open(getScoreFilename().c_str(), ofstream::app);

  file << getTimer() << endl;

  file.close();
}

string Minesweeper::getScoreFilename(){

  stringstream o;
  o << "/scores/" << SIZE_ROW << "x" << SIZE_COL << "x" << BOMB_TOTAL;
  o << ".txt";

  return string(PATH) + o.str();
}

void Minesweeper::showMines(){
  for(auto& coords: minesCoord)
    mineTable[coords[0]][coords[1]].visibility = true;
}

SDL_Surface* Minesweeper::getTextSurface(const char* message){
  return TTF_RenderUTF8_Solid(textFont,message,textColor);
}

void Minesweeper::renderGame(){
  for (Uint32 i = 0; i < SIZE_ROW; i++){
    for (Uint32 j = 0; j < SIZE_COL; j++){
      Uint32 x = j * WIDTH;
      Uint32 y = i * HEIGHT;
      if (mineTable[i][j].visibility == false){
        if (mineTable[i][j].flag == false)
          Render::drawImage(displayVideo,images,x,y + OFFSET,
              0,11*WIDTH,WIDTH,HEIGHT);
        else
          Render::drawImage(displayVideo,images,x,y + OFFSET
              ,0,10*WIDTH,WIDTH,HEIGHT);
      }
      else
        Render::drawImage(displayVideo,images,x,y + OFFSET,0,
            mineTable[i][j].value*WIDTH,WIDTH,HEIGHT);

    }
  }
  renderFlagText();

  // render the best score
  renderText(getTextSurface(TOP_SCORE.c_str()),(WIDTH/2) * SIZE_COL -
      2*(WIDTH/2)-WIDTH,0, WIDTH+5*WIDTH/2,HEIGHT);
  SDL_Flip(displayVideo);
}

void Minesweeper::renderText(SDL_Surface* surface, int x, int y,
    int w, int h){
  Render::drawRect(displayVideo,x,y,w,h,0,0,0);
  Render::drawImage(displayVideo,surface,x,y,0,0,w,h);
  SDL_FreeSurface(surface);
}

void Minesweeper::renderFlagText(){
  stringstream message;
  message << setw(3) << setfill('0') << flagsQuantity;
  renderText(getTextSurface(message.str().c_str()),0,0,
      WIDTH+WIDTH/2,HEIGHT);
}

const char* Minesweeper::getTimer(){
  time_t timeNow = 0;
  int hours,minutes,seconds;
  hours = minutes = seconds = 0;
  if (initTime){
    timeNow = time(NULL) - startTime;
    hours = timeNow / 3600;
    minutes = (timeNow / 60) % 60;
    seconds = timeNow % 60;
  }
  stringstream messageHour,messageMinutes,messageSeconds;
  messageHour << setw(2) << setfill('0') << hours;
  messageMinutes << setw(2) << setfill('0') << minutes;
  messageSeconds << setw(2) << setfill('0') << seconds;
  return (messageHour.str() + ":" + messageMinutes.str() +
      ":" + messageSeconds.str()).c_str();
}

void Minesweeper::renderTimerText(){
  while(!done){
    if (!gameOver && !isPlayerWinner()){
      mtx.lock();
      renderText(getTextSurface(getTimer()),WIDTH * SIZE_COL -
          4*(WIDTH/2)-WIDTH,0,
          WIDTH+5*WIDTH/2,HEIGHT);
      SDL_Flip(displayVideo);
      mtx.unlock();
    }
    struct timespec timespec_st;
    if (initTime) {
      timespec_st = {1,0};
      nanosleep(&timespec_st,NULL);
    }
    else {
      timespec_st = {0,NANOSEC};
      nanosleep(&timespec_st,NULL);
    }
  }
}

void Minesweeper::printMines(){
  for(Uint32 i = 0; i < SIZE_ROW; i++){
    for(Uint32 j = 0; j < SIZE_COL; j++){
      cout << mineTable[i][j].value << " ";
    }
    cout << endl;
  }
}

int main(int argc, char **argv){
  Minesweeper obj;
  if (argc == 2 && atoi(argv[1]) == 1 || argc == 1)
    obj.setGameParams(9,9,10);
  else if (argc == 2 && atoi(argv[1]) == 2)
    obj.setGameParams(16,16,40);
  else if (argc == 2 && atoi(argv[1]) == 3)
    obj.setGameParams(16,30,99);
  else{
    if (argc < 4) return 1;
    int row = atoi(argv[1]);
    int col = atoi(argv[2]);
    int mines = atoi(argv[3]);

    if (col >= 9 && mines <= 999 && mines <= row * col)
      obj.setGameParams(row,col,mines);
    else
      return 1;
  }
  if (!obj.initGame()) return 1;
  obj.insertMines();
  obj.insertDigits();
  obj.startGame();
  return 0;
}
