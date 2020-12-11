#ifndef CHESS_H
#include <vector>

#define CHESS_H
#define white 0
#define black 1
/*
-------BOARD LAYOUT-------
        black
    __________________
    |56|57 |...|62|63|
    |48|             |
    |.               |
    |.               |
    |.               |
    |0|1|2|...       |

        white
*/
namespace chess{

    typedef enum piecetype{
        none,
        pawn,
        rook,
        bishop,
        knight,
        queen,
        king
    };

    struct piece
    {
        piecetype type = none;
        int colour; // false is black, true is white
    };

    void init();                    // initializes everything
    std::vector<int> moves(int pos); // gives all possible moves fromthat position
    piece piece_at(int pos);        // returns piece at position
    bool legal(int player, int from, int to);  // checks if move from from to to is legal
    bool move(int player, int from, int to); // checks if move legal and performs move + returns true if yes returns false if no
    int win(); // return -1 if no, 0 if white, 1 if black
    bool king_is_threatened(int colour); // checks whether coloured king is threatened
    bool castling_possible(int player, int king, int rook);
    bool castle(int player, int king, int rook);
}
#endif // CHESS_H