#include "Game.hpp"

Game::Game() :
    isRunning(true), isGameOver(false), 
    clickCount(0), firstClickIdx(INVALID_CLICK_IDX), secondClickIdx(INVALID_CLICK_IDX),
    boardX(0), boardY(0), boardW(0), boardH(0), squareDim(0),
    wP(0ULL), wR(0ULL), wN(0ULL), wB(0ULL), wQ(0ULL), wK(0ULL), 
    bP(0ULL), bR(0ULL),  bN(0ULL), bB(0ULL), bQ(0ULL), bK(0ULL),
    currentFen(initialFen), castlingRights({}), enPassantTarget("-")
    {

    // Initialise SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cerr << "SDL not initialised properly: " << SDL_GetError() << std::endl;
        isRunning = false;
    }

    else {
        InitialiseFenConversionMaps();
        InitialiseBitboards();
    }
}


Game::~Game() {
    SDL_Quit();
}


void Game::SetBit(uint64_t &bitBoard, int idx) {
    bitBoard |= (1ULL << idx);
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

    // Reset bitboards
    wP = wR = wN = wB = wQ = wK = 0ULL;
    bP = bR = bN = bB = bQ = bK = 0ULL;

    ConvertFenToGameVariables(initialFen);

    pieceVector = {&wP, &wR, &wN, &wB, &wQ, &wK, 
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

        // If the character is a number, advance bitIdx by the character
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
    while (fenString[stringIdx] != 32) {
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

            for (auto pieceType : pieceVector) {
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


bool Game::CheckIsOwnPiece(int clickIdx) {
    
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


bool Game::CheckIsOpponentPiece(int clickIdx) {

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


void Game::HandleFirstClickEvent() {

    int clickIdx = GetClickSquareIdx();

    if (clickIdx == INVALID_CLICK_IDX)
        return;
    
    // Nothing happens if the player's first click is not on their own piece
    bool isOwnPiece = CheckIsOwnPiece(clickIdx);

    if (isOwnPiece)
        SetClickSquareIdx(clickIdx);
}


void Game::HandleSecondClickEvent() {

    int clickIdx = GetClickSquareIdx();

    // Reset click variables if user clicks off screen
    if (clickIdx == INVALID_CLICK_IDX) {
        ResetClickVariables();
        return;
    }

    // If the user clicks on one of their own pieces again, change the focus
    bool isOwnPiece = CheckIsOwnPiece(clickIdx);
    bool isOpponentPiece = CheckIsOpponentPiece(clickIdx);
    // bool isEmpty = !(isOwnPiece || isOpponentPiece);

    SetClickSquareIdx(clickIdx, isOwnPiece);

    // TODO: Check if it is a valid move, and move accordingly
    if (clickCount == 2) {
        MovePiece(isOpponentPiece);
        ResetClickVariables();
    }  
}


void Game::ResetClickVariables() {

    clickCount = 0;
    firstClickIdx = secondClickIdx = INVALID_CLICK_IDX;
}


int Game::GetClickSquareIdx() {

    int x, y, clickIdx;
    SDL_GetMouseState(&x, &y);

    // Check for clicks off the board (invalid clicks)
    if (x < boardX || x > boardX + boardW || y < boardY || y > boardY + boardH)
        clickIdx = INVALID_CLICK_IDX;
    
    else {
        x = (x - boardX) / squareDim;
        y = (y - boardY) / squareDim;

        clickIdx = x + y * 8;
    }

    return clickIdx;
}


// changeFocus is whether the player clicks on another of their pieces on the second click
void Game::SetClickSquareIdx(int clickIdx, bool changeFocus) {

    if (clickCount == 0) {
        firstClickIdx = clickIdx;
        clickCount++;   
    }

    else if (clickCount == 1 && changeFocus)
        firstClickIdx = clickIdx;

    else {
        secondClickIdx = clickIdx;
        clickCount++;
    }
}


void Game::MovePiece(bool isOpponentPiece) {
    
    uint64_t *ownBitboard = nullptr;
    uint64_t *opponentBitboard = nullptr;

    // Find out which type of piece is moving
    if (activeColour == 'w') {
        // Get the correct piece type bitboard
        for (int i = 0; i < 6; i++) {
            if ((*pieceVector[i] & (1ULL << firstClickIdx)) != 0ULL) {
                ownBitboard = pieceVector[i];
                break;
            }
        }
    }

    else {
        // Get the correct piece type bitboard
        for (int i = 6; i < 12; i++) {
            if ((*pieceVector[i] & (1ULL << firstClickIdx)) != 0ULL) {
                ownBitboard = pieceVector[i];
                break;
            }
        }
    }

    // Toggle bits on own piece bitboards
    *ownBitboard &= ~(1ULL << firstClickIdx);
    *ownBitboard |= (1ULL << secondClickIdx);

    if (isOpponentPiece) {
        if (activeColour == 'w') {
            // Get the correct piece type bitboard
            for (int i = 6; i < 12; i++) {
                if ((*pieceVector[i] & (1ULL << secondClickIdx)) != 0ULL) {
                    opponentBitboard = pieceVector[i];
                }
            }
        }

        else {
            // Get the correct piece type bitboard
            for (int i = 0; i < 6; i++) {
                if ((*pieceVector[i] & (1ULL << secondClickIdx)) != 0ULL) {
                    opponentBitboard = pieceVector[i];
                }
            }
        }
    }
    
    // Update bitboard (if not nullptr) to reflect taking a piece
    if (opponentBitboard)
        *opponentBitboard ^= (1ULL << secondClickIdx);
    

    // Update game variables
    UpdateVariablesAfterMove();
    ConvertGameVariablesToFen();
}


void Game::UpdateVariablesAfterMove() {

    // Update composite bitboards
    whitePieceBitboard = wP | wR | wN | wB | wQ | wK;
    blackPieceBitboard = bP | bR | bN | bB | bQ | bK;
    allPieceBitboard = whitePieceBitboard | blackPieceBitboard;

    // Change player
    activeColour = (activeColour == 'w') ? 'b' : 'w';

    // TODO
    // Check if castling still possible
    // Update halfMove
    // Update fullMove    
}


void Game::GameLoop() {

    std::vector<int> boardDimensions = gui.GetBoardDimensions();
    boardX = boardDimensions[0];
    boardY = boardDimensions[1];
    boardW = boardDimensions[2];
    boardH = boardDimensions[3];
    squareDim = boardW / 8;
    
    SDL_Event event;

    while (isRunning) {

        if (SDL_WaitEvent(&event)) {

            switch (event.type) {

                case (SDL_QUIT) :
                    isRunning = false;
                    break;

                case (SDL_MOUSEBUTTONUP) :
                    if (clickCount == 0)
                        HandleFirstClickEvent();
                    else if (clickCount == 1) {
                        HandleSecondClickEvent();
                    }
                    break;
            }
        }
        
        gui.RenderScreen(currentFen, clickCount, firstClickIdx);
    }
}