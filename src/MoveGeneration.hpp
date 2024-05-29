#pragma once
#include <iostream>
#include <vector>
#include <bitset>
#include <cstdint>
#include <map>

class MoveGeneration {

    public:
        MoveGeneration();
        void GenerateWhitePawnMoves();
        void GenerateBlackPawnMoves();
        void GenerateKnightMoves();
        void GenerateKingMoves();
        void GenerateBishopMoves();
        void GenerateRookMoves();
        void GenerateQueenMoves();

        inline void SetBit(uint64_t &bitBoard, int idx);

        uint64_t FilterPawnMoves(int firstClickIdx, uint64_t generatedMoves, char colour, uint64_t opponentBitboard);
        inline bool CheckIsOccupied(int targetSquareIdx, uint64_t allPieceBitboard);
        inline bool CheckIsOccupiedByOwn(int targetSquareIdx, uint64_t ownBitboard);
        inline bool CheckIsOccupiedByOpponent(int targetSquareIdx, uint64_t opponentBitboard);

        bool CheckCanMakeMove(int secondClickIdx, uint64_t generatedMoves);

    public:
        std::map<int, uint64_t> whitePawnLookupTable;
        std::map<int, uint64_t> blackPawnLookupTable;
        std::map<int, uint64_t> knightLookupTable;
        std::map<int, uint64_t> bishopLookupTable;
        std::map<int, uint64_t> rookLookupTable;
        std::map<int, uint64_t> queenLookupTable;
        std::map<int, uint64_t> kingLookupTable;

    private:
        // All of the theoretically possible moves with no constraints
        enum moveDirection {UP = -8, 
                            DOWN = 8, 
                            LEFT = -1, 
                            RIGHT = 1,
                            UP_LEFT = -9, 
                            UP_RIGHT = -7, 
                            DOWN_LEFT = 7, 
                            DOWN_RIGHT = 9};
        
        // These are used to check whether the piece can move without going off the board
        const uint64_t AFile = 0b0000000100000001000000010000000100000001000000010000000100000001ULL;
        const uint64_t BFile = 0b0000001000000010000000100000001000000010000000100000001000000010ULL;
        const uint64_t GFile = 0b0100000001000000010000000100000001000000010000000100000001000000ULL;
        const uint64_t HFile = 0b1000000010000000100000001000000010000000100000001000000010000000ULL;
        const uint64_t Rank8 = 0b0000000000000000000000000000000000000000000000000000000011111111ULL;
        const uint64_t Rank7 = 0b0000000000000000000000000000000000000000000000001111111100000000ULL;
        const uint64_t Rank2 = 0b0000000011111111000000000000000000000000000000000000000000000000ULL;
        const uint64_t Rank1 = 0b1111111100000000000000000000000000000000000000000000000000000000ULL;

        uint64_t OuterEdge = AFile | HFile | Rank1 | Rank8;
        uint64_t InnerEdge = BFile | GFile | Rank2 | Rank7;
};