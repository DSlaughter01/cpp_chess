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
        MoveGeneration moveGeneration;

        /* HELPER FUNCTIONS */
        // Sets the bit at idx to 1
        inline void SetBit(uint64_t &bitBoard, int idx);

        // Checks if the player's own piece occupies the square at clickIdx
        inline bool CheckIsOwnPiece(int clickIdx);

        // Checks if an opponent piece occupies the square at clickIdx
        inline bool CheckIsOpponentPiece(int clickIdx);

        // returns the index in pieceVector at which the piece type can be found
        inline int FindPieceIdx(int clickIdx);

        /* VARIABLE INITIALISATION */
        void InitialiseBitboards();
        void InitialiseFenConversionMaps();

        /* VARIABLE UPDATE */
        // Takes a FEN string as input, and updates bitboards and other FEN variables
        void ConvertFenToGameVariables(std::string fenString);

        // Updates currentFen according to the game's bitboards and other variables
        void ConvertGameVariablesToFen();


        /* CLICK EVENTS */
        // Called when the user clicks and clickCount == 0
        void HandleFirstClickEvent();

        // Called when the user clicks and clickCount == 1
        void HandleSecondClickEvent();

        // Returns the index of the square the player clicks on
        int GetClickSquareIdx();

        // Sets clickCount, firstClickIdx, and secondClickIdx depending on input
        void SetClickVariables(int clickIdx, bool isOwnPiece = false);

        // Sets clickCount to 0, and firstClickIdx and secondClickIdx to INVALID_CLICK_IDX
        void ResetClickVariables();

        /* PIECE MOVES */
        // Queries moveGeneration to update possibleMoves according to the piece type clicked on and current game state 
        void LookUpPossibleMoves();

        // Updates own bitboard and opponent piece bitboard accordingly
        void MovePiece(bool isOpponentPiece);

        // Updates game variables according to the new board position etc.
        void UpdateVariablesAfterMove();
        
    public:

    private: 

        uint64_t possibleMoves;  

        // Looping bools for game state
        bool isRunning, isGameOver;

        // The board's physical dimensions, used to (in)validate click inputs
        int boardX, boardY, boardW, boardH, squareDim;

        // Click variables
        const int INVALID_IDX = -1;
        int clickCount;
        int firstClickIdx, secondClickIdx;

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
        uint64_t bP, bR, bN, bB, bQ, bK;
        uint64_t whitePieceBitboard, blackPieceBitboard, allPieceBitboard;

        // An array containing all of the bitboards, used for piece lookup
        std::array<uint64_t*, 12> pieceArray {};
        int srcPieceArrayIdx, destPieceArrayIdx;

        // Used to convert between FEN strings and bitboards
        std::map<char, uint64_t*> charToBitboardMap;
        std::map<uint64_t*, char> bitboardToCharMap;
};