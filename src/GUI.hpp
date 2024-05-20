#pragma once

#include <iostream>
#include <array>
#include <vector>
#include <map>

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

class GUI {
    public:
        GUI();
        ~GUI();

        void RenderScreen(std::string &fenString, int clickCount, int clickIdx);

        // Returns {x, y, w, h}
        std::vector<int> GetBoardDimensions();

    private:
        void InitialiseCharToTextureMap();
        void InitialiseBoard();
        void LoadPieceTextures();
            
        void RenderBoard(int clickCount, int clickIdx);
        void RenderPieces(std::string &fenString);

    private:

        const int INVALID_CLICK_IDX = -1;

        SDL_Window* window;
        SDL_Renderer* renderer;

        // BoardDimensions 
        int windowWidth, windowHeight;
        int sidePadding, bottomPadding, topPadding;
        int squareDim;

        std::array<std::array<SDL_Rect, 8>, 8> boardDest;
        std::array<SDL_Surface*, 12> pieceSurfaces;
        std::array<SDL_Texture*, 12> pieceTextures;

        enum pieceEnum {wP, wR, wN, wB, wQ, wK, bP, bR, bN, bB, bQ, bK};

        std::map<char, SDL_Texture*> charToTextureMap;
};