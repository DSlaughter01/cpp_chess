#pragma once
#include <iostream>
#include <vector>
#include <bitset>
#include <cstdint>
#include <map>

class MoveGeneration {

    public:
        MoveGeneration();

        /* INITIALISATION FUNCTIONS - Setters */
        void GenerateWhitePawnMoves();
        void GenerateBlackPawnMoves();
        void GenerateKingMoves();
        void GenerateBishopMoves();
        void GenerateRookMoves();
        void GenerateQueenMoves();
        void GenerateKnightMoves();

        /* GETTERS */
        uint64_t GetWhitePawnsMoves(int idx) {return whitePawnLookupTable[idx];}
        uint64_t GetBlackPawnsMoves(int idx) {return blackPawnLookupTable[idx];}
        uint64_t GetKnightMoves(int idx) {return knightLookupTable[idx];}
        uint64_t GetBishopMoves(int idx) {return bishopLookupTable[idx];}
        uint64_t GetRookMoves(int idx) {return rookLookupTable[idx];}
        uint64_t GetQueenMoves(int idx) {return queenLookupTable[idx];}
        uint64_t GetKingMoves(int idx) {return kingLookupTable[idx];}

        /* HELPER FUNCTIONS */
        // Sets the bit at idx to 1
        inline void SetBit(uint64_t &bitBoard, int idx);

        // Checks whether targetSquareIdx is occupied by a piece
        inline bool CheckIsOccupied(int targetSquareIdx, uint64_t allPieceBitboard);
        
        // Checks whether targetSquareIdx is occupied by the player's piece
        inline bool CheckIsOccupiedByOwn(int targetSquareIdx, uint64_t ownBitboard);

        // Checks whether targetSquareIdx is occupied by the opponent's piece
        inline bool CheckIsOccupiedByOpponent(int targetSquareIdx, uint64_t opponentBitboard);

        // Checks whether the move the player wants to make is in the set of possible moves
        bool CheckCanMakeMove(int secondClickIdx, uint64_t possibledMoves);

        // Removes squares which the pawn cannot move to in the current board state
        uint64_t FilterPawnMoves(int firstClickIdx, uint64_t generatedMoves, char colour, uint64_t opponentBitboard);

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
        enum moveDirection {UP = -8, 
                            DOWN = 8, 
                            LEFT = -1, 
                            RIGHT = 1,
                            UP_LEFT = -9, 
                            UP_RIGHT = -7, 
                            DOWN_LEFT = 7, 
                            DOWN_RIGHT = 9};
        
        const uint64_t EMPTY_BITBOARD = 0ULL;

        // These are used to check whether the piece can move without going off the board
        const uint64_t AFile = 0b0000000100000001000000010000000100000001000000010000000100000001ULL;
        const uint64_t BFile = 0b0000001000000010000000100000001000000010000000100000001000000010ULL;
        const uint64_t GFile = 0b0100000001000000010000000100000001000000010000000100000001000000ULL;
        const uint64_t HFile = 0b1000000010000000100000001000000010000000100000001000000010000000ULL;
        const uint64_t Rank8 = 0b0000000000000000000000000000000000000000000000000000000011111111ULL;
        const uint64_t Rank7 = 0b0000000000000000000000000000000000000000000000001111111100000000ULL;
        const uint64_t Rank2 = 0b0000000011111111000000000000000000000000000000000000000000000000ULL;
        const uint64_t Rank1 = 0b1111111100000000000000000000000000000000000000000000000000000000ULL;

        const uint64_t OuterEdge = AFile | HFile | Rank1 | Rank8;
        const uint64_t InnerEdge = BFile | GFile | Rank2 | Rank7;
};