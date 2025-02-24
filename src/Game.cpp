#include "Game.hpp"

Game::Game() :
    isRunning(true), isGameOver(false), 
    clickCount(0), firstClickIdx(INVALID_IDX), secondClickIdx(INVALID_IDX),
    possibleMoves(0ULL), srcPieceArrayIdx(INVALID_IDX), destPieceArrayIdx(INVALID_IDX),
    currentFen(initialFen), castlingRights({}), enPassantTarget("-")
    {

    // Initialise SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cerr << "SDL not initialised properly: " << SDL_GetError() << std::endl;
        isRunning = false;
        return;
    }

    InitialiseFenConversionMaps();
    InitialiseBitboards();

    std::array<int, 4> boardDimensions = gui.GetBoardDimensions();
    boardX = boardDimensions[0];
    boardY = boardDimensions[1];
    boardW = boardDimensions[2];
    boardH = boardDimensions[3];

    squareDim = boardW / 8;  
}


Game::~Game() {
    SDL_Quit();
}


inline void Game::SetBit(uint64_t &bitBoard, int idx) {
    bitBoard |= (1ULL << idx);
}


inline bool Game::CheckIsOwnPiece(int clickIdx) {
    
    if (activeColour == 'w') {
        if ((whitePieceBitboard & (1ULL << clickIdx)) != 0) 
            return true;
        else
            return false;
    }

    else {
        if ((blackPieceBitboard & (1ULL << clickIdx)) != 0) 
            return true;
        else
            return false;
    }
}


inline bool Game::CheckIsOpponentPiece(int clickIdx) {

    if (activeColour == 'w') {
        if ((blackPieceBitboard & (1ULL << clickIdx)) != 0) 
            return true;
        else
            return false;
    }

    else {
        if ((whitePieceBitboard & (1ULL << clickIdx)) != 0) 
            return true;
        else
            return false;
    }
}


inline int Game::FindPieceIdx(int clickIdx) {

    // Get the correct piece type
    for (int i = 0; i < 12; i++) {
        if ((*pieceArray[i] & (1ULL << clickIdx)) != 0ULL)
            return i;     
    }

    return -1;
}


void Game::InitialiseFenConversionMaps() {
    
    charToBitboardMap = {
        {'P', &wP}, {'R', &wR}, {'N', &wN}, {'B', &wB}, {'Q', &wQ}, {'K', &wK},
        {'p', &bP}, {'r', &bR}, {'n', &bN}, {'b', &bB}, {'q', &bQ}, {'k', &bK}
    };

    bitboardToCharMap = {
        {&wP, 'P'}, {&wR, 'R'}, {&wN, 'N'}, {&wB, 'B'}, {&wQ, 'Q'}, {&wK, 'K'},
        {&bP, 'p'}, {&bR, 'r'}, {&bN, 'n'}, {&bB, 'b'}, {&bQ, 'q'}, {&bK, 'k'}
    };
}


void Game::InitialiseBitboards() {

    // Set bitboard to empty
    wP = wR = wN = wB = wQ = wK = 0ULL;
    bP = bR = bN = bB = bQ = bK = 0ULL;

    // Populate bitboards and other game variables
    ConvertFenToGameVariables(initialFen);

    // Store references to bitboards
    pieceArray = {&wP, &wR, &wN, &wB, &wQ, &wK, 
                   &bP, &bR, &bN, &bB, &bQ, &bK};

    // Update composite bitboards
    whitePieceBitboard = wP | wR | wN | wB | wQ | wK;
    blackPieceBitboard = bP | bR | bN | bB | bQ | bK;
    allPieceBitboard = whitePieceBitboard | blackPieceBitboard;
}


void Game::ConvertFenToGameVariables(std::string fenString) {

    castlingRights.clear();

    // Loop through every character in the fen string
    int stringIdx = 0;
    int bitIdx = 0;
    
    // Update bitboards until hitting a space character
    while (fenString[stringIdx] != ' ') {

        // If the character is a number, advance bitIdx by the number
        if (isdigit(fenString[stringIdx]))
            bitIdx += (fenString[stringIdx] - '0');
        
        // If the character is a letter
        else if (isalpha(fenString[stringIdx])) {
            // Map it to a bitboard, set the correct bit to 1
            SetBit(*(charToBitboardMap[fenString[stringIdx]]), bitIdx);

            // Move to the next square
            bitIdx++;
        }

        stringIdx++;
    }

    // Assign active colour
    stringIdx++;
    activeColour = fenString[stringIdx];
    stringIdx += 2;

    // Assign castling rights
    while (fenString[stringIdx] != 32) {
        castlingRights.emplace_back(fenString[stringIdx]);
        stringIdx++;
    }

    stringIdx++;

    // Assign en passant target:
    enPassantTarget.clear();
    while (fenString[stringIdx] != 32) {
        enPassantTarget += fenString[stringIdx];
        stringIdx++;
    }
    stringIdx++;

    // Half-move clock
    std::string hmc = "";
    while (fenString[stringIdx] != ' ') {
        hmc += fenString[stringIdx];
        stringIdx++;
    }
    halfMoveClock = std::stoi(hmc);
    stringIdx++;

    // Full-move count
    std::string fmc = "";
    while (fenString[stringIdx] != '\n') {
        fmc += fenString[stringIdx];
        stringIdx++;
    }
    fullMove = std::stoi(fmc);
}


void Game::ConvertGameVariablesToFen() {

    currentFen.clear();
    int emptySquares = 0;

    // Loop through all 64 indices in allPiecesBitboard
    for (int i = 0; i < 64; i++) {

        // End of the row
        if (i % 8 == 0 && i > 0) {
            if (emptySquares > 0) {
                currentFen += std::to_string(emptySquares);
                emptySquares = 0;
            }
            currentFen += "/";
        }

        // If a piece is present on this square
        if ((allPieceBitboard & (1ULL << i)) != 0) {

            for (auto pieceType : pieceArray) {

                // See if this is the type of character present
                if ((*pieceType & (1ULL << i)) != 0) {

                    // Add the character corresponding to that pieceType to currentFen
                    if (emptySquares > 0) {
                        currentFen += std::to_string(emptySquares);
                        emptySquares = 0;
                    }
                    currentFen += bitboardToCharMap[pieceType];
                    break;
                }
            }
        }
        else 
            emptySquares++;
    }

    // Add other game variables to the FEN string
    currentFen += " ";
    currentFen += activeColour;
    currentFen += " ";

    if (castlingRights.size() > 0) {
        for (auto &i: castlingRights)
            currentFen += i;
    }
    else
        currentFen += "-";
    
    currentFen += " " + enPassantTarget;
    currentFen += " " + std::to_string(halfMoveClock);
    currentFen += " " + std::to_string(fullMove);
}


void Game::HandleFirstClickEvent() {

    int clickIdx = GetClickSquareIdx();

    if (clickIdx == INVALID_IDX)
        return;
    
    // Player has to click on their own piece for something to happen
    bool isOwnPiece = CheckIsOwnPiece(clickIdx);

    if (isOwnPiece) {

        SetClickVariables(clickIdx);
        srcPieceArrayIdx = FindPieceIdx(firstClickIdx);
        LookUpPossibleMoves();
    }
}


void Game::HandleSecondClickEvent() {

    // Find out where the user clicks
    int clickIdx = GetClickSquareIdx();

    // Reset click variables if user clicks off screen
    if (clickIdx == INVALID_IDX) {
        ResetClickVariables();
        return;
    }

    // If the user clicks on one of their own pieces again, change the focus
    bool isOwnPiece = CheckIsOwnPiece(clickIdx);

    SetClickVariables(clickIdx, isOwnPiece);

    if (isOwnPiece) {
        srcPieceArrayIdx = FindPieceIdx(firstClickIdx);
        LookUpPossibleMoves();
    }

    // Check if it is a valid move, and move accordingly
    if (clickCount == 2) {

        // The piece at the square where the player wants to move
        destPieceArrayIdx = FindPieceIdx(secondClickIdx);
        bool isOpponentPiece = CheckIsOpponentPiece(secondClickIdx);

        if (moveGeneration.CheckCanMakeMove(secondClickIdx, possibleMoves)) {

            MovePiece(isOpponentPiece);
            
            // Update game variables
            UpdateVariablesAfterMove();
            ConvertGameVariablesToFen();
        }

        else {
            clickCount = 1;
        }
    }
}


int Game::GetClickSquareIdx() {

    int x, y, clickIdx;
    SDL_GetMouseState(&x, &y);

    // Check for clicks off the board (invalid clicks)
    if (x < boardX || x > boardX + boardW || y < boardY || y > boardY + boardH)
        clickIdx = INVALID_IDX;
    
    else {
        x = (x - boardX) / squareDim;
        y = (y - boardY) / squareDim;

        clickIdx = x + y * 8;
    }

    return clickIdx;
}


void Game::SetClickVariables(int clickIdx, bool isOwnPiece) {

    if (clickCount == 0) {
        firstClickIdx = clickIdx;
        clickCount++;   
    }

    else if (clickCount == 1 && isOwnPiece)
        firstClickIdx = clickIdx;

    else {
        secondClickIdx = clickIdx;
        clickCount++;
    }
}


void Game::ResetClickVariables() {

    clickCount = 0;
    firstClickIdx = secondClickIdx = INVALID_IDX;
}


void Game::LookUpPossibleMoves() {

    switch (srcPieceArrayIdx) {
        case 0 :
            possibleMoves = moveGeneration.GetWhitePawnsMoves(firstClickIdx);
            possibleMoves = moveGeneration.FilterPawnMoves(firstClickIdx, possibleMoves, activeColour, blackPieceBitboard);
            break;

        case 1 : 
            possibleMoves = moveGeneration.GetRookMoves(firstClickIdx);
            break;

        case 2 : 
            possibleMoves = moveGeneration.GetKnightMoves(firstClickIdx);
            break;

        case 3 :
            possibleMoves = moveGeneration.GetBishopMoves(firstClickIdx);
            break;

        case 4 :
            possibleMoves = moveGeneration.GetQueenMoves(firstClickIdx);
            break;

        case 5 :
            possibleMoves = moveGeneration.GetKingMoves(firstClickIdx);
            break;

        case 6 :
            possibleMoves = moveGeneration.GetBlackPawnsMoves(firstClickIdx);
            possibleMoves = moveGeneration.FilterPawnMoves(firstClickIdx, possibleMoves, activeColour, whitePieceBitboard);
            break;

        case 7 : 
            possibleMoves = moveGeneration.GetRookMoves(firstClickIdx);
            break;

        case 8 : 
            possibleMoves = moveGeneration.GetKnightMoves(firstClickIdx);
            break;

        case 9 :
            possibleMoves = moveGeneration.GetBishopMoves(firstClickIdx);
            break;

        case 10 :
            possibleMoves = moveGeneration.GetQueenMoves(firstClickIdx);
            break;

        case 11 :
            possibleMoves = moveGeneration.GetKingMoves(firstClickIdx);
            break;
    }

    // Remove squares which are occupied by a player's own pieces
    possibleMoves = (activeColour == 'w') ? possibleMoves &= ~whitePieceBitboard : possibleMoves &= ~blackPieceBitboard;
}


void Game::MovePiece(bool isOpponentPiece) {
    
    uint64_t *ownBitboard = pieceArray[srcPieceArrayIdx];
    uint64_t *opponentBitboard = nullptr;

    // Toggle bits on own piece bitboards
    *ownBitboard &= ~(1ULL << firstClickIdx);
    *ownBitboard |= (1ULL << secondClickIdx);

    // Opponent bitboard referenced if user tries to take piece
    if (isOpponentPiece && destPieceArrayIdx != INVALID_IDX)
        opponentBitboard = pieceArray[destPieceArrayIdx];
    
    // Update bitboard (if not nullptr) to reflect taking a piece
    if (opponentBitboard)
        *opponentBitboard ^= (1ULL << secondClickIdx);

    // Update halfMove - if a pawn moves or a piece is taken, halfMove = 0
    if (srcPieceArrayIdx == 0 || srcPieceArrayIdx == 6 || opponentBitboard) 
        halfMoveClock = 0;
    else 
        halfMoveClock++;

    // If halfMove == 50, the game is a draw
    if (halfMoveClock == 50)
        isGameOver = true;

    // Incrememnt fullMove by 1 every time black moves
    if (activeColour == 'b')
        fullMove++;    
}


void Game::UpdateVariablesAfterMove() {

    ResetClickVariables();
    possibleMoves = 0ULL;

    // Update composite bitboards
    whitePieceBitboard = wP | wR | wN | wB | wQ | wK;
    blackPieceBitboard = bP | bR | bN | bB | bQ | bK;
    allPieceBitboard = whitePieceBitboard | blackPieceBitboard;

    // Change player
    activeColour = (activeColour == 'w') ? 'b' : 'w';

    // TODO
    // Check if castling still possible
        // If either rook moves, then one of the options is removed
        // If the king moves, both options are removed
}


void Game::GameLoop() {

    SDL_Event event;

    while (isRunning) {

        if (SDL_WaitEvent(&event)) {

            switch (event.type) {

                // User quits
                case (SDL_QUIT) :
                    isRunning = false;
                    break;

                // User clicks
                case (SDL_MOUSEBUTTONUP) :

                    if (clickCount == 0) {
                        HandleFirstClickEvent();
                    }
                    
                    else if (clickCount == 1) {
                        HandleSecondClickEvent();
                    }
                    break;
            }
        }
        
        gui.RenderScreen(currentFen, clickCount, firstClickIdx, possibleMoves);
    }
}