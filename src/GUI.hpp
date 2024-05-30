#pragma once

#include <iostream>
#include <array>
#include <vector>
#include <map>
#include <cstdint>

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

class GUI {
    public:
        GUI();
        ~GUI();

        void RenderScreen(std::string &fenString, int clickCount, int clickIdx, uint64_t possibleMoves);

        // Returns {x, y, w, h}
        std::vector<int> GetBoardDimensions();

    private:
        /* INITIALISATION FUNCTIONS */
        void InitialiseCharToTextureMap();
        void InitialiseBoard();
        void LoadPieceTextures();

        /* RENDERING */
        // Renders the squares on the board
        void RenderBoard(int clickCount, int clickIdx, uint64_t possibleMoves);

        // Renders the pieces still present on the board
        void RenderPieces(std::string &fenString);

    private:

        const int INVALID_IDX = -1;

        SDL_Window* window;
        SDL_Renderer* renderer;

        // BoardDimensions 
        int windowWidth, windowHeight;
        int sidePadding, bottomPadding, topPadding;
        int squareDim;

        // Destination rectangles for the 64 squares
        std::array<std::array<SDL_Rect, 8>, 8> boardDest;

        // Surfaces and textures of the 12 possible piece types
        std::array<SDL_Surface*, 12> pieceSurfaces;
        std::array<SDL_Texture*, 12> pieceTextures;

        // enum relating to indices in pieceTextures, for ease of rendering where appropriate
        enum pieceEnum {wP, wR, wN, wB, wQ, wK, bP, bR, bN, bB, bQ, bK};

        // Maps FEN string characters to relevant textures
        std::map<char, SDL_Texture*> charToTextureMap;
};