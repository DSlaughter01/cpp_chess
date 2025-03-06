#include "Moves.hpp"

Moves::Moves() {

    GenerateWhitePawnMoves();
    GenerateBlackPawnMoves();
    GenerateKnightMoves();
    GenerateBishopMoves();
    GenerateRookMoves();
    GenerateQueenMoves();
    GenerateKingMoves();
}


bool Moves::CheckCanMakeMove(int secondClickIdx, uint64_t &possibleMoves) {

    if ((possibleMoves & (1ULL << secondClickIdx)) != BitboardOps::EMPTY_BITBOARD)
        return true;
    else
        return false;
}


void Moves::GenerateWhitePawnMoves() {

    // White pawns move UP, UP_LEFT, UP_RIGHT, 2 * UP_RIGHT

    // This range used because rank 8 pawns get promoted, and pawns start on rank 2
    for (int squareIdx = 8; squareIdx < 56; squareIdx++) {

        uint64_t currentBitboard = BitboardOps::EMPTY_BITBOARD;
        uint64_t moveBitboard = BitboardOps::EMPTY_BITBOARD;

        BitboardOps::SetBit(currentBitboard, squareIdx);

        bool isAFile = (currentBitboard & AFile);
        bool isHFile = (currentBitboard & HFile);

        // Normal pawns (not rank 2)
        if (squareIdx < 48) {
            BitboardOps::SetBit(moveBitboard, squareIdx + UP);
            if (!isAFile) 
                BitboardOps::SetBit(moveBitboard, squareIdx + UP_LEFT);
            if (!isHFile)
                BitboardOps::SetBit(moveBitboard, squareIdx + UP_RIGHT);
        }

        // Rank 2 pawns
        else {
            BitboardOps::SetBit(moveBitboard, squareIdx + UP);
            BitboardOps::SetBit(moveBitboard, squareIdx + 2 * UP);
            if (!isAFile) 
                BitboardOps::SetBit(moveBitboard, squareIdx + UP_LEFT);
            if (!isHFile)
                BitboardOps::SetBit(moveBitboard, squareIdx + UP_RIGHT);
        }

        whitePawnLookupTable[squareIdx] = moveBitboard;
    }
}


void Moves::GenerateBlackPawnMoves() {

    // Black pawns move: 2 * DOWN, DOWN, DOWN_LEFT, DOWN_RIGHT

    // This range used because bottom rank can't move, and pawns start on rank 7
    for (int squareIdx = 8; squareIdx < 56; squareIdx++) {
        
        uint64_t currentBitboard = BitboardOps::EMPTY_BITBOARD;
        uint64_t moveBitboard = BitboardOps::EMPTY_BITBOARD;

        BitboardOps::SetBit(currentBitboard, squareIdx);

        bool isAFile = (currentBitboard & AFile);
        bool isHFile = (currentBitboard & HFile);

        // Normal pawns (not rank 7)
        if (squareIdx > 15) {
            BitboardOps::SetBit(moveBitboard, squareIdx + DOWN);
            if (!isAFile) 
                BitboardOps::SetBit(moveBitboard, squareIdx + DOWN_LEFT);
            if (!isHFile)
                BitboardOps::SetBit(moveBitboard, squareIdx + DOWN_RIGHT);  
        }

        // Rank 7 pawns
        else {
            BitboardOps::SetBit(moveBitboard, squareIdx + DOWN);
            BitboardOps::SetBit(moveBitboard, squareIdx + 2 * DOWN);
            if (!isAFile) 
                BitboardOps::SetBit(moveBitboard, squareIdx + DOWN_LEFT);
            if (!isHFile)
                BitboardOps::SetBit(moveBitboard, squareIdx + DOWN_RIGHT);
        }

        blackPawnLookupTable[squareIdx] = moveBitboard;
    }
}


void Moves::GenerateKnightMoves() {

    uint64_t ABFile = AFile | BFile;
    uint64_t GHFile = GFile | HFile;
    uint64_t Rank12 = Rank1 | Rank2;
    uint64_t Rank78 = Rank7 | Rank8;

    std::array<int, 8> knightMoves = {-17, -15, -10, -6, 6, 10, 15, 17};

    for (int squareIdx = 0; squareIdx < 64; squareIdx++) {

        uint64_t currentBitboard = BitboardOps::EMPTY_BITBOARD;
        uint64_t moveBitboard = BitboardOps::EMPTY_BITBOARD; 

        BitboardOps::SetBit(currentBitboard, squareIdx);

        // Knights on the middle squares can use any move
        if ((currentBitboard & (OuterEdge | InnerEdge)) == BitboardOps::EMPTY_BITBOARD) {
            for (auto &move : knightMoves) {
                BitboardOps::SetBit(moveBitboard, squareIdx + move);
            }
        }

        // Consider other squares
        else {
            for (auto &move : knightMoves) {

                // Move has to stay on the board
                if (move + squareIdx < 64 && move + squareIdx > -1) {

                    // Consider the posibility for specific moves 
                    if (move == -17 && (currentBitboard & (AFile | Rank78)) == BitboardOps::EMPTY_BITBOARD) 
                        BitboardOps::SetBit(moveBitboard, squareIdx + move);
                    
                    else if (move == -15 && (currentBitboard & (HFile | Rank78)) == BitboardOps::EMPTY_BITBOARD) 
                        BitboardOps::SetBit(moveBitboard, squareIdx + move);

                    else if (move == -10 && (currentBitboard & (ABFile | Rank8)) == BitboardOps::EMPTY_BITBOARD) 
                        BitboardOps::SetBit(moveBitboard, squareIdx + move);
    
                    else if (move == -6 && (currentBitboard & (GHFile | Rank8)) == BitboardOps::EMPTY_BITBOARD) 
                        BitboardOps::SetBit(moveBitboard, squareIdx + move);

                    else if (move == 6 && (currentBitboard & (ABFile | Rank1)) == BitboardOps::EMPTY_BITBOARD) 
                        BitboardOps::SetBit(moveBitboard, squareIdx + move);

                    else if (move == 10 && (currentBitboard & (GHFile | Rank1)) == BitboardOps::EMPTY_BITBOARD)
                        BitboardOps::SetBit(moveBitboard, squareIdx + move);

                    else if (move == 15 && (currentBitboard & (AFile | Rank12)) == BitboardOps::EMPTY_BITBOARD) 
                        BitboardOps::SetBit(moveBitboard, squareIdx + move);

                    else if (move == 17 && (currentBitboard & (HFile | Rank12)) == BitboardOps::EMPTY_BITBOARD)
                        BitboardOps::SetBit(moveBitboard, squareIdx + move);
                }
            }
        }       

        // Add possible moves from a certain square to the lookup table   
        knightLookupTable[squareIdx] = moveBitboard;
    }
}


void Moves::GenerateKingMoves() {

    std::array<int, 8> kingMoves = {UP, DOWN, LEFT, RIGHT, UP_LEFT, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT};

    for (int squareIdx  = 0; squareIdx < 64; squareIdx++) {
        
        uint64_t currentBitboard = BitboardOps::EMPTY_BITBOARD;
        uint64_t moveBitboard = BitboardOps::EMPTY_BITBOARD; 

        BitboardOps::SetBit(currentBitboard, squareIdx);
        
        // Kings not on the outer edge of the board can move anywhere
        if ((currentBitboard & OuterEdge) == BitboardOps::EMPTY_BITBOARD) {
            for (auto &move : kingMoves)
                BitboardOps::SetBit(moveBitboard, squareIdx + move);  
        }

        else {
            for (auto &move : kingMoves) {

                // Checks for pieces leaving the board vertically
                if (squareIdx + move > -1 && squareIdx + move < 64) {

                    if (move == UP || move == DOWN)
                        BitboardOps::SetBit(moveBitboard, squareIdx + move);
                    
                    // Check for pieces wrapping around the board left
                    else if ((move == LEFT || move == DOWN_LEFT || move == UP_LEFT) && 
                        (currentBitboard & AFile) == BitboardOps::EMPTY_BITBOARD)
                    {
                        BitboardOps::SetBit(moveBitboard, squareIdx + move);
                    }

                    // And right
                    else if ((move == RIGHT || move == DOWN_RIGHT || move == DOWN_LEFT) && 
                             (currentBitboard & HFile) == BitboardOps::EMPTY_BITBOARD)
                    {
                        BitboardOps::SetBit(moveBitboard, squareIdx + move);
                    }
                } 
            }
        }

        kingLookupTable[squareIdx] = moveBitboard;
    }
}


void Moves::GenerateBishopMoves() {

    for (int squareIdx = 0; squareIdx < 64; squareIdx++) {

        uint64_t currentBitboard = BitboardOps::EMPTY_BITBOARD;
        uint64_t moveBitboard = BitboardOps::EMPTY_BITBOARD;
        
        BitboardOps::SetBit(currentBitboard, squareIdx);

        for (auto &move : bishopMoves) {

            // See where the move would land
            int currentSquare =  squareIdx;
            int nextSquare = currentSquare + move;

            bool leftMove = (move == UP_LEFT || move == DOWN_LEFT);
            bool rightMove = (move == UP_RIGHT || move == DOWN_RIGHT);

            // See if the current square is on the edge of the board
            bool isAFile = (1ULL << currentSquare) & AFile;
            bool isHFile = (1ULL << currentSquare) & HFile;

            while (nextSquare > -1 && nextSquare < 64) {

                if ((leftMove && isAFile) || (rightMove && isHFile))
                    break;

                BitboardOps::SetBit(moveBitboard, nextSquare);
                currentSquare += move;
                nextSquare += move;

                isAFile = (1ULL << (currentSquare)) & AFile;
                isHFile = (1ULL << (currentSquare)) & HFile;
            }
        }

        bishopLookupTable[squareIdx] = moveBitboard;
    }
}


void Moves::GenerateRookMoves() {

    for (int squareIdx = 0; squareIdx < 64; squareIdx++) {

        uint64_t currentBitboard = BitboardOps::EMPTY_BITBOARD;
        uint64_t moveBitboard = BitboardOps::EMPTY_BITBOARD;
        
        BitboardOps::SetBit(currentBitboard, squareIdx);

        for (auto &move : rookMoves) {

            // See where the move would land
            int currentSquare = squareIdx;
            int nextSquare = currentSquare + move;

            // See if the current square is on the edge
            bool isAFile = (1ULL << currentSquare) & AFile;
            bool isHFile = (1ULL << currentSquare) & HFile;

            while (nextSquare > -1 && nextSquare < 64) {

                if ((move == LEFT && isAFile) || (move == RIGHT && isHFile))
                    break;

                BitboardOps::SetBit(moveBitboard, nextSquare);
                currentSquare += move;
                nextSquare += move;

                isAFile = (1ULL << (currentSquare)) & AFile;
                isHFile = (1ULL << (currentSquare)) & HFile;
            }
        }

        rookLookupTable[squareIdx] = moveBitboard;
    }
}


void Moves::GenerateQueenMoves() {

    for (int squareIdx = 0; squareIdx < 64; squareIdx++) {

        uint64_t currentBitboard = BitboardOps::EMPTY_BITBOARD;
        uint64_t moveBitboard = BitboardOps::EMPTY_BITBOARD;
        
        BitboardOps::SetBit(currentBitboard, squareIdx);

        for (auto &move : queenMoves) {

            // See where the move would land
            int currentSquare =  squareIdx;
            int nextSquare = currentSquare + move;

            // See if the current square is on the edge
            bool isAFile = (1ULL << currentSquare) & AFile;
            bool isHFile = (1ULL << currentSquare) & HFile;

            bool leftMove = (move == UP_LEFT || move == LEFT || move == DOWN_LEFT);
            bool rightMove = (move == UP_RIGHT || move == RIGHT || move == DOWN_RIGHT);

            // Prevent moving off the board vertically
            while (nextSquare > -1 && nextSquare < 64) {

                if ((leftMove && isAFile) || (rightMove && isHFile))
                    break;

                BitboardOps::SetBit(moveBitboard, nextSquare);

                currentSquare += move;
                nextSquare += move;

                isAFile = (1ULL << (currentSquare)) & AFile;
                isHFile = (1ULL << (currentSquare)) & HFile;
            }
        }

        queenLookupTable[squareIdx]= moveBitboard;
    }
}


uint64_t Moves::FilterPawnMoves(char colour, int idx, uint64_t availableMoves, uint64_t &ownBitboard, uint64_t &oppBitboard) {

    if (colour == 'w') {
        if ((oppBitboard & (1ULL << (idx + UP_LEFT))) == 0) 
            availableMoves &= ~(1ULL << (idx + UP_LEFT));        
        if ((oppBitboard & (1ULL << (idx + UP_RIGHT ))) == 0) 
            availableMoves &= ~(1ULL << (idx + UP_RIGHT)); 

        // Block the pawn if there is a piece in front of it 
        if ((oppBitboard & (1ULL << (idx + UP))) || (ownBitboard & (1ULL << (idx + UP)))) {
            availableMoves &= ~(1ULL << (idx + 2 * UP));
            availableMoves &= ~(1ULL << (idx + UP));
        }
        if ((oppBitboard & (1ULL << (idx + 2 * UP))))
            availableMoves &= ~(1ULL << (idx + 2 * UP));
    }

    else {
        if ((oppBitboard & (1ULL << (idx + DOWN_RIGHT))) == 0) 
            availableMoves &= ~(1ULL << (idx + DOWN_RIGHT));        
        if ((oppBitboard & (1ULL << (idx + DOWN_LEFT))) == 0) 
            availableMoves &= ~(1ULL << (idx + DOWN_LEFT)); 
                   
        // Block the pawn if there is a piece in front of it 
        if ((oppBitboard & (1ULL << (idx + DOWN))) || (ownBitboard & (1ULL << (idx + DOWN)))) {
            availableMoves &= ~(1ULL << (idx + 2 * DOWN));
            availableMoves &= ~(1ULL << (idx + DOWN));
        }
        if ((oppBitboard & (1ULL << (idx + 2 * DOWN))))
            availableMoves &= ~(1ULL << (idx + 2 * DOWN));
    }

    return availableMoves;
}


uint64_t Moves::FilterSlideMoves(char pieceType, int idx, uint64_t availableMoves, uint64_t &ownBitboard, uint64_t &oppBitboard) {

    // Change pieceType to lowerCase
    pieceType = std::tolower(pieceType);
    std::vector<int> directions; // vector not array because queenMoves.size() != rookMoves.size() or bishopMoves.size()

    switch (pieceType) {
        case 'r':
            directions.assign(rookMoves.begin(), rookMoves.end());
            break;
        case 'b':
            directions.assign(bishopMoves.begin(), bishopMoves.end());
            break;
        case 'q':
            directions.assign(queenMoves.begin(), queenMoves.end());
            break;
    }

    for (auto &move : directions) {

        int currentSquare = idx;
        bool removeSquare = false;

        while (!((1ULL << (currentSquare)) & boundaryMap.at(move))) {
            
            currentSquare += move;

            // If it intersects with own bitboard, cut off straight away
            if (!removeSquare && (ownBitboard & (1ULL << currentSquare)))
                removeSquare = true;
            
            // Remove option
            if (removeSquare)
                availableMoves &= ~(1ULL << currentSquare);

            // If it intersects with opponent bitboard, allow landing on that square but cut off afterwards
            if (!removeSquare && (oppBitboard & (1ULL << currentSquare))) 
                removeSquare = true;
        }
    }

    return availableMoves;
}

uint64_t Moves::GetWhitePawnMoves(int idx, uint64_t &ownBitboard, uint64_t &oppBitboard) {
    return FilterPawnMoves('w', idx, whitePawnLookupTable[idx], ownBitboard, oppBitboard);
}

uint64_t Moves::GetBlackPawnMoves(int idx, uint64_t &ownBitboard, uint64_t &oppBitboard) {
    return FilterPawnMoves('b', idx, blackPawnLookupTable[idx], ownBitboard, oppBitboard);
}

uint64_t Moves::GetKnightMoves(int idx, uint64_t &ownBitboard) {

    uint64_t availableMoves = knightLookupTable[idx];
    availableMoves &= ~ownBitboard;
    return availableMoves;
}

uint64_t Moves::GetBishopMoves(char pieceType, int idx, uint64_t &ownBitboard, uint64_t &oppBitboard) {
    return FilterSlideMoves(pieceType, idx, bishopLookupTable[idx], ownBitboard, oppBitboard);
}

uint64_t Moves::GetRookMoves(char pieceType, int idx, uint64_t &ownBitboard, uint64_t &oppBitboard) {
    return FilterSlideMoves(pieceType, idx, rookLookupTable[idx], ownBitboard, oppBitboard);
}

uint64_t Moves::GetQueenMoves(char pieceType, int idx, uint64_t &ownBitboard, uint64_t &oppBitboard) {
    return FilterSlideMoves(pieceType, idx, queenLookupTable[idx], ownBitboard, oppBitboard);
}

uint64_t Moves::GetKingMoves(int idx, uint64_t &ownBitboard) {

    uint64_t availableMoves = kingLookupTable[idx];
    availableMoves &= ~ownBitboard;
    return availableMoves;
}

uint64_t Moves::GetMoves(char pieceType, int currentSquare, uint64_t &whiteBitboard, uint64_t &blackBitboard) {

    switch (pieceType) {

        // Pawns
        case 'P':
            return GetWhitePawnMoves(currentSquare, whiteBitboard, blackBitboard);
            break;
        case 'p':
            return GetBlackPawnMoves(currentSquare, blackBitboard, whiteBitboard);
            break;
    
        // Rooks
        case 'R':
            return GetRookMoves('R', currentSquare, whiteBitboard, blackBitboard);
            break;
        case 'r':
            return GetRookMoves('r', currentSquare, blackBitboard, whiteBitboard);
            break;

        // Knight
        case 'N':
            return GetKnightMoves(currentSquare, whiteBitboard);
            break;
        case 'n':
            return GetKnightMoves(currentSquare, blackBitboard);
            break;

        // Bishop
        case 'B':
            return GetBishopMoves('B', currentSquare, whiteBitboard, blackBitboard);
            break;
        case 'b':
            return GetBishopMoves('b', currentSquare, blackBitboard, whiteBitboard);
            break;

        // Queen
        case 'Q':
            return GetQueenMoves('Q', currentSquare, whiteBitboard, blackBitboard);
            break;
        case 'q':
            return GetQueenMoves('q', currentSquare, blackBitboard, whiteBitboard);
            break;

        // King
        case 'K':
            return GetKingMoves(currentSquare, whiteBitboard);
            break;
        case 'k':
            return GetKingMoves(currentSquare, blackBitboard);
            break;

        default:
            return BitboardOps::EMPTY_BITBOARD;
            break;
    }
}


bool Moves::CheckCheck(char activeColour, std::map<char, uint64_t*> &charToBitboardMap, uint64_t &whiteBitboard, uint64_t &blackBitboard) {

    // Retrieve the index of the opposing king and the attacking pieces (5 different types, not including the king which cannot take the other king)
    std::array<char, 5> attackingPieces;
    uint64_t* defendingKingBb;

    if (activeColour == 'b') {
        attackingPieces = {'p', 'r', 'n', 'b', 'q'};
        defendingKingBb = charToBitboardMap['K'];
    }
    else if (activeColour == 'w') {
        attackingPieces = {'P', 'R', 'N', 'B', 'Q'};
        defendingKingBb = charToBitboardMap['k'];
    }

    // std::countr_zero(*defendingKingBb);
    
    // Cycle through each attacking piece type
    for (auto &piece : attackingPieces) {
        
        // Access the bitboard for that piece (early exit for empty bitboards)
        uint64_t* pieceBb = charToBitboardMap[piece];
        if (*pieceBb == BitboardOps::EMPTY_BITBOARD)
            continue;

        uint64_t attackMoves {};

        // See which square the piece is on (seems inefficient, doing worst case 320 iterations? Thought about cutting short on queen because there is only one but after promotion could be more. Could query own bitboard to see presence, then would have do fewer iterations, also could use leading and trailing zeros to not loop over entire board but just from the first to the last. Would work well on cases where pieces are close together)
        for (int i = 0; i < 64; i++) {

            if ((1ULL << i) & *pieceBb) {

                // Check which moves the piece has available
                attackMoves = GetMoves(piece, i, whiteBitboard, blackBitboard);
                
                // See if any of these is an attack on the king
                 if (BitboardOps::HasIntersection(attackMoves, *defendingKingBb)) {
                    std::cout << "Check!" << std::endl;
                    return true;
                }
            }
        }
    }

    return false;
}