#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <ctype.h>
#include <bitset>
#include <sstream>
#include "SDL2/SDL.h"
#include "GUI.hpp"
#include "MoveGeneration.hpp"

class Game {

    public:
        // Constructor and destructor
        Game();
        ~Game();
    
        void GameLoop();

    private:

        GUI gui;

        /* VARIABLE INITIALISATION */
        void InitialiseBitboards();
        void InitialiseFenConversionMaps();

        /* VARIABLE UPDATE */
        // Takes a FEN string as input, and updates bitboards and other FEN variables
        void ConvertFenToGameVariables(std::string fenString);
        void ConvertGameVariablesToFen();
        void SetBit(uint64_t &bitBoard, int idx);

        /*  */
        bool CheckIsOwnPiece(int clickIdx);
        bool CheckIsOpponentPiece(int clickIdx);

        /* HANDLING CLICK EVENTS */
        void HandleFirstClickEvent();
        void HandleSecondClickEvent();
        void ResetClickVariables();
        int GetClickSquareIdx();
        void SetClickSquareIdx(int clickIdx, bool changeFocus = false);

        void MovePiece(bool isOpponentPiece);
        void UpdateVariablesAfterMove();
        
    public:

    private:      
        // Looping bools for game state
        bool isRunning;
        bool isGameOver;

        const int INVALID_CLICK_IDX = -1;
        int clickCount;
        int firstClickIdx;
        int secondClickIdx;

        int boardX, boardY, boardW, boardH, squareDim;

        // FEN
        const std::string initialFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
        std::string currentFen;

        // Either 'w' or 'b'
        char activeColour; 

        // Starts as "KQkq", then as the possibility is removed, characters are removed.
        std::vector<char> castlingRights;

        // Will require a way of converting square index to square name, e.g. e3, d4
        std::string enPassantTarget;
        int halfMoveClock;
        int fullMove;

        // Bitboards
        uint64_t wP, wR, wN, wB, wQ, wK;
        uint64_t bP, bR,  bN, bB, bQ, bK;
        uint64_t whitePieceBitboard, blackPieceBitboard, allPieceBitboard;
        std::vector<uint64_t*> pieceVector {};

        // Used to convert between FEN strings and bitboards
        std::map<char, uint64_t*> charToBitboardMap;
        std::map<uint64_t*, char> bitboardToCharMap;
};