all:
	g++ -W -Wextra src/*.cpp -o mines.out -std=c++11 -lSDL\
	    -lSDL_image -lSDL_ttf -lpthread -fpermissive
