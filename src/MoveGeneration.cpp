#include "MoveGeneration.hpp"

MoveGeneration::MoveGeneration() {

    GenerateWhitePawnMoves();
    GenerateBlackPawnMoves();
    GenerateKnightMoves();
    GenerateKingMoves();
}


uint64_t MoveGeneration::CheckForPawnTake(int firstClickIdx, uint64_t generatedMoves, char colour, uint64_t opponentBitboard) {

    // Check if there is a piece on each diagonal    
    if (colour == 'w') {
        if ((opponentBitboard & (1ULL << (firstClickIdx + UP_LEFT))) == 0) 
            generatedMoves &= ~(1ULL << (firstClickIdx + UP_LEFT));        
        if ((opponentBitboard & (1ULL << (firstClickIdx + UP_RIGHT ))) == 0) 
            generatedMoves &= ~(1ULL << (firstClickIdx + UP_RIGHT));        
    }
    else {
        if ((opponentBitboard & (1ULL << (firstClickIdx + DOWN_RIGHT))) == 0) 
            generatedMoves &= ~(1ULL << (firstClickIdx + DOWN_RIGHT));        
        if ((opponentBitboard & (1ULL << (firstClickIdx + DOWN_LEFT))) == 0) 
            generatedMoves &= ~(1ULL << (firstClickIdx + DOWN_LEFT));        
    }

    // Use UP_LEFT, DOWN_LEFT, UP_RIGHT, UP_LEFT as appropriate
    return generatedMoves;
}


inline bool MoveGeneration::CheckIsOccupied(int targetSquareIdx, uint64_t allPieceBitboard) {

    if ((allPieceBitboard & (1ULL << (targetSquareIdx))) == 0ULL) 
        return false;
    else 
        return true;
}


inline bool MoveGeneration::CheckIsOccupiedByOwn(int targetSquareIdx, uint64_t ownBitboard)
{
        if ((ownBitboard & (1ULL << (targetSquareIdx))) == 0ULL) 
            return false;
        else
            return true;
}


inline bool MoveGeneration::CheckIsOccupiedByOpponent(int targetSquareIdx, uint64_t opponentBitboard)
{
        if ((opponentBitboard & (1ULL << (targetSquareIdx))) == 0ULL) 
            return false;
        else
            return true;
}

inline void MoveGeneration::SetBit(uint64_t &bitBoard, int idx) {
    bitBoard |= (1ULL << idx);
}


bool MoveGeneration::CheckCanMakeMove(int secondClickIdx, uint64_t generatedMoves) {

    uint64_t desiredMove = 1ULL << secondClickIdx;

    if ((generatedMoves & (1ULL << secondClickIdx)) != 0ULL)
        return true;
    else
        return false;
}


void MoveGeneration::GenerateWhitePawnMoves() {

    // These move up: UP, UP_LEFT, UP_RIGHT, 2 * UP_RIGHT

    // This range used because rank 8 pawns get promoted, and pawns start on rank 2
    for (int squareIdx = 8; squareIdx < 56; squareIdx++) {

        uint64_t currentBitboard = 0ULL;
        uint64_t moveBitboard = 0ULL;

        SetBit(currentBitboard, squareIdx);

        bool isAFile = (currentBitboard & AFile);
        bool isHFile = (currentBitboard & HFile);

        // Normal pawns
        if (squareIdx < 48) {
            SetBit(moveBitboard, squareIdx + UP);
            if (!isAFile) 
                SetBit(moveBitboard, squareIdx + UP_LEFT);
            if (!isHFile)
                SetBit(moveBitboard, squareIdx + UP_RIGHT);
        }

        // Rank 2 pawns
        else {
            SetBit(moveBitboard, squareIdx + UP);
            SetBit(moveBitboard, squareIdx + 2 * UP);
            if (!isAFile) 
                SetBit(moveBitboard, squareIdx + UP_LEFT);
            if (!isHFile)
                SetBit(moveBitboard, squareIdx + UP_RIGHT);
        }

        whitePawnLookupTable[squareIdx] = moveBitboard;
    }
}


void MoveGeneration::GenerateBlackPawnMoves() {

    // These move down: 2 * DOWN, DOWN, DOWN_LEFT, DOWN_RIGHT

    // This range used because bottom rank can't move, and pawns start on rank 7
    for (int squareIdx = 8; squareIdx < 56; squareIdx++) {
        
        uint64_t currentBitboard = 0ULL;
        uint64_t moveBitboard = 0ULL;

        SetBit(currentBitboard, squareIdx);

        bool isAFile = (currentBitboard & AFile);
        bool isHFile = (currentBitboard & HFile);

        if (squareIdx > 15) {
            SetBit(moveBitboard, squareIdx + DOWN);
            if (!isAFile) 
                SetBit(moveBitboard, squareIdx + DOWN_LEFT);
            if (!isHFile)
                SetBit(moveBitboard, squareIdx + DOWN_RIGHT);  
        }
        else {
            SetBit(moveBitboard, squareIdx + DOWN);
            SetBit(moveBitboard, squareIdx + 2 * DOWN);
            if (!isAFile) 
                SetBit(moveBitboard, squareIdx + DOWN_LEFT);
            if (!isHFile)
                SetBit(moveBitboard, squareIdx + DOWN_RIGHT);
        }

        blackPawnLookupTable[squareIdx] = moveBitboard;
    }
}


void MoveGeneration::GenerateKnightMoves() {

    uint64_t ABFile = AFile | BFile;
    uint64_t GHFile = GFile | HFile;
    uint64_t Rank12 = Rank1 | Rank2;
    uint64_t Rank78 = Rank7 | Rank8;

    std::vector<int> knightMoves = {-17, -15, -10, -6, 6, 10, 15, 17};

    for (int squareIdx = 0; squareIdx < 64; squareIdx++) {

        uint64_t currentBitboard = 0ULL;
        uint64_t moveBitboard = 0ULL; 

        SetBit(currentBitboard, squareIdx);

        // Knights on the middle squares can use any move
        if ((currentBitboard & (OuterEdge | InnerEdge)) == 0ULL) {
            for (auto &move : knightMoves) {
                SetBit(moveBitboard, squareIdx + move);
            }
        }

        // Consider other squares
        else {
            for (auto &move : knightMoves) {

                // Move has to stay on the board
                if (move + squareIdx < 64 && move + squareIdx > -1) {

                    // Consider the posibility for specific moves 
                    if (move == -17 && (currentBitboard & (AFile | Rank78)) == 0ULL) 
                        SetBit(moveBitboard, squareIdx + move);
                    
                    else if (move == -15 && (currentBitboard & (HFile | Rank78)) == 0ULL) 
                        SetBit(moveBitboard, squareIdx + move);

                    else if (move == -10 && (currentBitboard & (ABFile | Rank8)) == 0ULL) 
                        SetBit(moveBitboard, squareIdx + move);
    
                    else if (move == -6 && (currentBitboard & (GHFile | Rank8)) == 0ULL) 
                        SetBit(moveBitboard, squareIdx + move);

                    else if (move == 6 && (currentBitboard & (ABFile | Rank1)) == 0ULL) 
                        SetBit(moveBitboard, squareIdx + move);

                    else if (move == 10 && (currentBitboard & (GHFile | Rank1)) == 0ULL)
                        SetBit(moveBitboard, squareIdx + move);

                    else if (move == 15 && (currentBitboard & (AFile | Rank12)) == 0ULL) 
                        SetBit(moveBitboard, squareIdx + move);

                    else if (move == 17 && (currentBitboard & (HFile | Rank12)) == 0ULL)
                        SetBit(moveBitboard, squareIdx + move);
                }
            }
        }       

        // Add possible moves from a certain square to the lookup table   
        knightLookupTable[squareIdx] = moveBitboard;
    }
}

void MoveGeneration::GenerateKingMoves() {

    std::vector<int> kingMoves = {UP, DOWN, LEFT, RIGHT, UP_LEFT, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT};

    for (int squareIdx  = 0; squareIdx < 64; squareIdx++) {
        
        uint64_t currentBitboard = 0ULL;
        uint64_t moveBitboard = 0ULL; 

        SetBit(currentBitboard, squareIdx);
        
        if ((currentBitboard & OuterEdge) == 0ULL) {
            for (auto &move : kingMoves)
                SetBit(moveBitboard, squareIdx + move);  
        }

        else {
            for (auto &move : kingMoves) {

                // Checks for pieces leaving the board vertically
                if (squareIdx + move > -1 && squareIdx + move < 64) {

                    if (move == UP || move == DOWN)
                        SetBit(moveBitboard, squareIdx + move);
                    
                    // Check for pieces wrapping around the board left
                    else if ((move == LEFT || move == DOWN_LEFT || move == UP_LEFT) && 
                        (currentBitboard & AFile) == 0ULL)
                    {
                        SetBit(moveBitboard, squareIdx + move);
                    }

                    // And right
                    else if ((move == RIGHT || move == DOWN_RIGHT || move == DOWN_LEFT) && 
                             (currentBitboard & HFile) == 0ULL)
                    {
                        SetBit(moveBitboard, squareIdx + move);
                    }
                } 
            }
        }

        kingLookupTable[squareIdx] = moveBitboard;
    }
}
