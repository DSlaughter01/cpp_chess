#include <iostream>
#include "Game.hpp"

int main(int argc, char** args) {
    
    Game game;
    GUI gui;

    game.GameLoop(gui);

    return 0;
}