all:
	g++ -std=c++20 -I include/ -L lib/ -o main src/main.cpp src/Game.cpp src/GUI.cpp src/Moves.cpp src/BitboardOps.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image