all:
	g++ -W -Wextra -g -O0 *.cpp -o mines.out -std=c++11 -lSDL\
	    -lSDL_image -lSDL_ttf -fpermissive
