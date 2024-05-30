#include "GUI.hpp"

GUI::GUI() :
    window(nullptr), renderer(nullptr), windowWidth(680), windowHeight(680)
{

    // Initialise SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cerr << "Problem initialising GUI: " << SDL_GetError() << std::endl;
        return;
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "SDL_image not initialised properly: " << IMG_GetError() << std::endl;
        return;
    }

    // Create a resizeable window
    window = SDL_CreateWindow("Chess", 
                              SDL_WINDOWPOS_CENTERED, 
                              SDL_WINDOWPOS_CENTERED, 
                              windowWidth,
                              windowHeight,
                              SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cerr << "Window could not be initialised: " << SDL_GetError() << std::endl;
        return;
    }

    // Create a renderer
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        std::cerr << "Renderer could not be initialised: " << SDL_GetError() << std::endl;
        return;
    }

    LoadPieceTextures();
    InitialiseBoard();
    InitialiseCharToTextureMap();
}


GUI::~GUI() {

    for (auto &i : pieceTextures) {

        if (i) 
            SDL_DestroyTexture(i);
    }

    if (window)
        SDL_DestroyWindow(window);

    if (renderer)
        SDL_DestroyRenderer(renderer);

    SDL_Quit();
}


std::vector<int> GUI::GetBoardDimensions() {

    return {sidePadding, 
            topPadding, 
            windowWidth - (sidePadding * 2), 
            windowHeight - (bottomPadding * 2)};
}


void GUI::InitialiseCharToTextureMap() {

    charToTextureMap = {
        {'P', pieceTextures[wP]}, 
        {'R', pieceTextures[wR]}, 
        {'N', pieceTextures[wN]}, 
        {'B', pieceTextures[wB]}, 
        {'Q', pieceTextures[wQ]}, 
        {'K', pieceTextures[wK]},
        {'p', pieceTextures[bP]}, 
        {'r', pieceTextures[bR]}, 
        {'n', pieceTextures[bN]}, 
        {'b', pieceTextures[bB]}, 
        {'q', pieceTextures[bQ]}, 
        {'k', pieceTextures[bK]}
    };
}


void GUI::InitialiseBoard() {
    
    sidePadding = topPadding = bottomPadding = 20;
    squareDim = (windowWidth - (sidePadding * 2)) / 8;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            boardDest[i][j] = {sidePadding + (i * squareDim),
                               topPadding + (j * squareDim), 
                               squareDim, 
                               squareDim};
        }
    }
}

   
void GUI::LoadPieceTextures() {

    std::vector<std::string> filenames = {"wP", "wR", "wN", "wB", "wQ", "wK",
                                          "bP", "bR", "bN", "bB", "bQ", "bK"};

    std::string tempName;
    SDL_RWops *rw;

    for (int i = 0; i < filenames.size(); i++) {

        // Create a RWOps object
        tempName = "assets/" + filenames[i] + ".svg";
        rw = SDL_RWFromFile(tempName.c_str(), "r");

        if (!rw) {
            std::cerr << "Failed to load file: " << SDL_GetError() << std::endl;
            return;
        }
        
        // Load the SVG from the RWops object
        pieceSurfaces[i] = IMG_LoadSVG_RW(rw);

        if (!pieceSurfaces[i]) {
            std::cerr << "Failed to load SVG: " << IMG_GetError() << std::endl;
            return;
        }

        else {

            // Create a texture from the surface to pass to the renderer
            pieceTextures[i] = SDL_CreateTextureFromSurface(renderer, pieceSurfaces[i]);
            if (!pieceTextures[i]) {
                std::cerr << "Failed to create texture from SVG: " << SDL_GetError() << std::endl;
                return;
            }

            // Clean up
            SDL_FreeSurface(pieceSurfaces[i]);
        }
    }
}


void GUI::RenderBoard(int clickCount, int clickIdx, uint64_t possibleMoves) {

    int clicki = INVALID_IDX;
    int clickj = INVALID_IDX;

    // Index in 2D boardDest array where the player has clicked on 1st click
    if (clickCount == 1 && clickIdx != INVALID_IDX) {
        clicki = clickIdx % 8;
        clickj = clickIdx / 8;
    }

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {

            // Highlight the square the user has clicked on
            if (i == clicki && j == clickj) {
                SDL_SetRenderDrawColor(renderer, 0, 102, 0, SDL_ALPHA_OPAQUE);
                SDL_RenderFillRect(renderer, &boardDest[i][j]);
            }

            // Highlight possible moves
            else if ((possibleMoves & (1ULL << (i + j * 8))) != 0) {
                SDL_SetRenderDrawColor(renderer, 204, 102, 0, SDL_ALPHA_OPAQUE);
                SDL_RenderFillRect(renderer, &boardDest[i][j]);
            }
            
            // Dark squares
            else if ((i + j) % 2) {
                SDL_SetRenderDrawColor(renderer, 50, 50, 50, SDL_ALPHA_OPAQUE);
                SDL_RenderFillRect(renderer, &boardDest[i][j]);
            }

            // Light squares
            else {
                SDL_SetRenderDrawColor(renderer, 200, 200, 200, SDL_ALPHA_OPAQUE);
                SDL_RenderFillRect(renderer, &boardDest[i][j]);
            }
        }
    }
}


void GUI::RenderPieces(std::string &fenString) {

    int squareIdx = 0;
    int x, y;

    for (auto &i : fenString) {

        if (i == ' ')
            break;
        
        // Render the piece denoted by the character
        else if (isalpha(i)) {
            
            x = squareIdx % 8;
            y = squareIdx / 8;

            // Render the right piece on the right square
            SDL_RenderCopy(renderer, charToTextureMap[i], NULL, &boardDest[x][y]);
            
            squareIdx++;
        }

        // Move on if the character denotes one or multiple empty squares
        else if (isdigit(i)) {
            squareIdx += i - '0';
        }
    }
}


void GUI::RenderScreen(std::string &fenString, int clickCount, int clickIdx, uint64_t possibleMoves) {

    SDL_RenderClear(renderer);

    RenderBoard(clickCount, clickIdx, possibleMoves);
    RenderPieces(fenString);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

    SDL_RenderPresent(renderer);
}