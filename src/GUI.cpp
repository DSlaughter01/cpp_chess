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

    // Create a renderer, and set its draw colour to dark grey
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

        tempName = "assets/" + filenames[i] + ".svg";
        rw = SDL_RWFromFile(tempName.c_str(), "r");

        if (!rw) {
            std::cerr << "Failed to load file: " << SDL_GetError() << std::endl;
            return;
        }
        
        pieceSurfaces[i] = IMG_LoadSVG_RW(rw);
        if (!pieceSurfaces[i]) {
            std::cerr << "Failed to load SVG: " << IMG_GetError() << std::endl;
            return;
        }

        else {
            pieceTextures[i] = SDL_CreateTextureFromSurface(renderer, pieceSurfaces[i]);
            if (!pieceTextures[i]) {
                std::cerr << "Failed to create texture from SVG: " << SDL_GetError() << std::endl;
                return;
            }

            SDL_FreeSurface(pieceSurfaces[i]);
        }
    }
}


void GUI::RenderBoard(int clickCount, int clickIdx) {

    int clicki = INVALID_CLICK_IDX;
    int clickj = INVALID_CLICK_IDX;

    // Index in 2D boardDest array where the player has clicked on 1st click
    if (clickCount == 1 && clickIdx != INVALID_CLICK_IDX) {
        clicki = clickIdx % 8;
        clickj = clickIdx / 8;
    }

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {

            // Highlight the square the user has clicked on
            if (i == clicki && j == clickj) {
                SDL_SetRenderDrawColor(renderer, 0, 0, 200, SDL_ALPHA_OPAQUE);
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

        if (i == 32)
            break;
        
        else if (isalpha(i)) {
            
            x = squareIdx % 8;
            y = squareIdx / 8;

            // Render the right piece on the right square
            SDL_RenderCopy(renderer, charToTextureMap[i], NULL, &boardDest[x][y]);
            
            squareIdx++;
        }

        else if (isdigit(i)) {
            squareIdx += i - '0';
        }
    }
}


void GUI::RenderScreen(std::string &fenString, int clickCount, int clickIdx) {
    SDL_RenderClear(renderer);

    RenderBoard(clickCount, clickIdx);
    RenderPieces(fenString);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

    SDL_RenderPresent(renderer);
}