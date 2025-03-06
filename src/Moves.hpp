#pragma once

#include <iostream>
#include <vector>
#include <array>
#include <bit>
#include <bitset>
#include <cstdint>
#include <map>

#include "BitboardOps.hpp"

class Moves {

    public:
        Moves();

        /* INITIALISATION FUNCTIONS - Setters */
        void GenerateWhitePawnMoves();
        void GenerateBlackPawnMoves();
        void GenerateKingMoves();
        void GenerateBishopMoves();
        void GenerateRookMoves();
        void GenerateQueenMoves();
        void GenerateKnightMoves();

        /* GETTERS */
        
        uint64_t GetMoves(char pieceType, int currentSquare, uint64_t &whiteBitboard, uint64_t &blackBitboard);
        
        /* HELPER FUNCTIONS */
        // Checks whether the move the player wants to make is in the set of possible moves
        bool CheckCanMakeMove(int secondClickIdx, uint64_t &possibleMoves);

        bool CheckCheck(char activeColour, std::map<char, uint64_t*> &charToBitboardMap, uint64_t &whiteBitboard, uint64_t &blackBitboard);
        
    private:
        uint64_t GetWhitePawnMoves(int idx, uint64_t &ownBitboard, uint64_t &oppBitboard);
        uint64_t GetBlackPawnMoves(int idx, uint64_t &ownBitboard, uint64_t &oppBitboard);
        uint64_t GetKnightMoves(int idx, uint64_t &ownBitboard);
        uint64_t GetBishopMoves(char pieceType, int idx, uint64_t &ownBitboard, uint64_t &oppBitboard);
        uint64_t GetRookMoves(char pieceType, int idx, uint64_t &ownBitboard, uint64_t &oppBitboard);
        uint64_t GetQueenMoves(char pieceType, int idx, uint64_t &ownBitboard, uint64_t &oppBitboard);
        uint64_t GetKingMoves(int idx, uint64_t &ownBitboard);
        
        // Removes squares which the piece cannot move to in the current board state
        uint64_t FilterPawnMoves(char colour, int idx, uint64_t availableMoves, uint64_t &ownBitboard, uint64_t &oppBitboard);
        uint64_t FilterSlideMoves(char pieceType, int idx, uint64_t availableMoves, uint64_t &ownBitboard, uint64_t &oppBitboard);
        
    public:
        
    private:
        /* LOOKUP TABLES */
        std::map<int, uint64_t> whitePawnLookupTable;
        std::map<int, uint64_t> blackPawnLookupTable;
        std::map<int, uint64_t> knightLookupTable;
        std::map<int, uint64_t> bishopLookupTable;
        std::map<int, uint64_t> rookLookupTable;
        std::map<int, uint64_t> queenLookupTable;
        std::map<int, uint64_t> kingLookupTable;

        // All of the theoretically possible moves with no constraints
        enum moveDirection {
            UP = -8, 
            DOWN = 8, 
            LEFT = -1, 
            RIGHT = 1,
            UP_LEFT = -9, 
            UP_RIGHT = -7, 
            DOWN_LEFT = 7, 
            DOWN_RIGHT = 9
        };

        const std::array<int, 4> bishopMoves = {UP_LEFT, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT};
        const std::array<int, 4> rookMoves = {UP, DOWN, LEFT, RIGHT};
        const std::array<int, 8> queenMoves = {UP, DOWN, LEFT, RIGHT, UP_LEFT, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT};

        // Used to check whether the piece can move without going off the board
        const uint64_t AFile = 0b0000000100000001000000010000000100000001000000010000000100000001ULL;
        const uint64_t BFile = AFile << 1;
        const uint64_t CFile = BFile << 1;
        const uint64_t DFile = CFile << 1;
        const uint64_t EFile = DFile << 1;
        const uint64_t FFile = EFile << 1;
        const uint64_t GFile = FFile << 1;
        const uint64_t HFile = GFile << 1;

        const uint64_t Rank1 = 0b1111111100000000000000000000000000000000000000000000000000000000ULL;
        const uint64_t Rank2 = Rank1 >> 8;
        const uint64_t Rank3 = Rank2 >> 8;
        const uint64_t Rank4 = Rank3 >> 8;
        const uint64_t Rank5 = Rank4 >> 8;
        const uint64_t Rank6 = Rank5 >> 8;
        const uint64_t Rank7 = Rank6 >> 8;
        const uint64_t Rank8 = Rank7 >> 8;

        const uint64_t OuterEdge = AFile | HFile | Rank1 | Rank8;
        const uint64_t InnerEdge = BFile | GFile | Rank2 | Rank7;

        const std::map<int, uint64_t> boundaryMap = {
            {UP, Rank8},
            {DOWN, Rank1},
            {LEFT, AFile},
            {RIGHT, HFile},
            {UP_LEFT, Rank8 | AFile},
            {UP_RIGHT, Rank8 | HFile},
            {DOWN_LEFT, Rank1 | AFile},
            {DOWN_RIGHT, Rank1 | HFile}
        };
};