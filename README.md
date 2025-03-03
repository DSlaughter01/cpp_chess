A chess engine coded in C++, using SDL2 as the graphics library and bitboards as a compact way to represent the positions of the board's pieces.

When the programme is loaded, SDL2 is initialised along with its image and font libraries, and a table of possible moves is generated for each piece type.
A chess game's initial FEN string is passed to the Game class, and bitboards are created from this string - one for each piece type of each colour, and 3 composite bitbords - one for all white pieces combined, one for all black pieces combined, and one for all pieces.

Each user is in turn required to click on the board, with checks in place to ensure proper usage (click on own piece first, opponent piece or empty square second, with the possibility to change piece after the first click by clicking on another of the user's own pieces).

Depending on the piece selected, moves are identified from the previously generated tables, and then filtered according to the board occupancy at that time. Sliding piece (bishop, rook, queen) moves are retrieved and then filtered using a ray tracing technique, which consists of testing moves in each direction permitted by the rules of chess and removing blocked options. 

At each stage under the hood, bitboards are updated, and FEN strings created.

The GUI reflects user input and game changes such as highlighting the selected piece, showing available moves for that piece, and only showing pieces that still remain on the board.

The project is still in progress, and next steps will be including castling rights, correcting the half move clock and full move counter, and implementing conditions for check and check mate.

Eventually, there is the potential for creating an AI opponent using a minimax approach with alpha-beta pruning and depth limitation.
