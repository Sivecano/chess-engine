// chess code by Fadri Lardon

#include "chess.h"
#include <vector>

namespace chess{


    const int worth[7] = {0, 1, 5, 3, 3, 9, 1000};

    piece board[64];

    int score[2];
    
    bool castling_moved[2][3]; // castling_moved[player][0: king, 1: rook1, 2: rook2]
    int last_enpassant;
    int last_enpassant_victim;

    void init()
    {
        for (int i = 0; i < 16; i++) board[i].colour = white;
        for (int i = 48; i < 64; i++ ) board[i].colour = black;
        board[4].type = board[60].type = king;
        board[3].type = board[59].type = queen;
        board[0].type = board[7].type = board[56].type = board[63].type = rook;
        board[1].type = board[6].type = board[57].type = board[62].type = knight;
        board[2].type = board[5].type = board[58].type = board[61].type = bishop;

        for (int i = 8; i < 16; i++) board[i].type = pawn;
        for (int i = 48; i < 56; i++) board[i].type = pawn;
        
        score[0] = score[1] = 0;

        for (int i = 0; i < 2; i++)
            for (int j = 0; j < 3; j++)
                castling_moved[i][j] = false;

        last_enpassant = -1;

    }

    std::vector<int> rook_moves(int pos)
    {
        std::vector<int> out;
        const int direction[4] {1, -1, 8, -8};

        for (int i = 0; i < 4; i++)
        {
            int temp = pos;
            while(true)
            {
                
                if (i == 0 && temp % 8 == 7) break;
                if (i == 1 && temp % 8 == 0) break;

                temp += direction[i];

                if (temp  > 63 || temp  < 0) break;

                if (board[temp].type != none) 
                {
                    if (board[temp].colour != board[pos].colour) 
                    {
                        out.push_back(temp);
                    }

                    break;

                }
                
                out.push_back(temp);
            }
        }

        return out;
    }

    std::vector<int> bishop_moves(int pos)
    {
        std::vector<int> out;
        const int direction[4] {9, -7, -9, 7};

        for (int i = 0; i < 4; i++)
        {
            int temp = pos;
            while(true)
            {
                if (temp % 8 == 7 && i < 2) break;
                if (temp % 8 == 0 && i > 1) break;
                temp += direction[i];
                if (temp > 63 || temp  < 0) break;
                
                if (board[temp].type != none) 
                {
                    if (board[temp].colour != board[pos].colour) 
                    {
                        out.push_back(temp);
                    }

                    break;

                }
                
                out.push_back(temp);
            }
        }

        return out;
    }

    std::vector <int > pawn_moves(int pos)
    {
        std::vector<int> out;


        if (board[pos + 8 * ((board[pos].colour == white) * 2 - 1)].type == none) out.push_back(pos + 8 * ((board[pos].colour == white) * 2 - 1));

        for (int i = -1; i < 2; i += 2)
        {
            if (pos % 8 == 0 && i == -1) continue;
            if (pos % 8 == 7 && i == 1) continue;
            if (((board[pos + 8 * ((board[pos].colour == white) * 2 - 1) + i].colour != board[pos].colour 
                && board[pos + 8 * ((board[pos].colour == white) * 2 - 1) + i].type != none)) 
                || ((pos + 8 * ((board[pos].colour == white) * 2 - 1) + i) == last_enpassant 
                && board[last_enpassant_victim].colour != board[pos].colour)) 
                out.push_back(pos + 8 * ((board[pos].colour == white) * 2 - 1) + i);
        }

        if (int(pos / 8) == ((board[pos].colour == black) * 5 + 1) 
            && board[pos + 16 * ((board[pos].colour == white) * 2 - 1)].type == none) 
            out.push_back(pos + 16 * ((board[pos].colour == white) * 2 - 1));

        return out;

    }

    std::vector <int > knight_moves(int pos)
    {
        std::vector<int> out;
        const int knight_dirs[4] = {17, -15, 10, -6};

            for (int i = -1; i < 2; i += 2)
            {   
                for (int j = 0; j < 4; j++)
                {
                    if (pos % 8 >= 6 + (j < 2) && (i > 0)) continue;
                    if (pos % 8 <= 1 - (j < 2) && (i < 0)) continue;
                    if (pos + i * knight_dirs[j] > 63 || pos + i* knight_dirs[j] < 0) continue;
                    if (board[pos + i * knight_dirs[j]].colour != board[pos].colour || board[pos + i * knight_dirs[j]].type == none)
                        out.push_back(pos + i * knight_dirs[j]);
                }
            }

        return out;
    }

    std::vector <int > king_moves(int pos)
    {
        std::vector<int> out;
        for(int i = -1; i < 2; i ++)
            for (int j = -1; j < 2; j++)
                if (i != j)
                {
                    if (pos % 8 == 0 && i < 0) continue;
                    if (pos % 8 == 7 && i > 0) continue;
                    if ( pos + i + 8 * j < 0 || pos + i + 8 * j > 63) continue;
                    if (board[pos + i + 8 * j].type == none || board[pos + i + 8 * j].colour != board[pos].colour) out.push_back(pos + i + 8 * j);
                }

        return out;
    }

    bool threatened(int pos)
    {   
        bool threatened = false;
        const int knight_dirs[4] = {17, -15, 10, -6};
        const int dirs[8] = {-1, 7, -9, -7, 9, 1, 8, -8};

        for (int i = 0; i < 8; i++)
        {
            int temp = pos;
            while(true)
            {

                if (temp % 8 == 7 && i > 3 && i < 6) break;
                if (temp % 8 == 0 && i < 4) break;
                temp += dirs[i];
                if (temp > 63 || temp < 0) break;
                
                if (board[temp].type != none) 
                {
                    if (board[temp].colour != board[pos].colour) 
                    {
                        if (board[temp].type == queen) threatened = true;
                        else if (board[temp].type == rook && i > 4 || i == 0) threatened = true;
                        else if (board[temp].type == bishop && i > 0 && i < 5) threatened = true;
                    }
                    break;
                }
            }
        }

        
        for (int i: knight_moves(pos))
        {
            if (board[i].type == knight) threatened = true;
        }

        return threatened;
    }



    std::vector<int> moves(int pos)
    {
        std::vector<int> out;
        std::vector<int> temp;
        
        
        switch (board[pos].type)
        {
        case none:
            break;

        case pawn:
            out = pawn_moves(pos);
            break;

        case knight:
            out = knight_moves(pos);
            break;

        case rook:
            out = rook_moves(pos);
            break;

        case bishop:
            out = bishop_moves(pos);
            break;

        case queen:
            out = rook_moves(pos);
            temp = bishop_moves(pos);
            for (int i : temp) out.push_back(i);
            break;

        case king:
            out = king_moves(pos);
            break;
                    
        default:
            break;
        }

        return out;
    }


    bool king_is_threatened(int colour)
    {
        int king;

        for (int i = 0; i < 64; i++) 
            if (board[i].colour == colour && board[i].type == king)
            {
                king = i;
                break;
            }

        return threatened(king);
    }

    bool legal(int colour, int from, int to)
    {
        if (colour != board[from].colour) return false;
        std::vector<int> possibles = moves(from);
        piece aggressor = board[from];
        piece victim = board[to];


        bool can_move = false;

        for (int i : possibles)
        {
            can_move = can_move || (i == to);
        }

        // king threat check
        board[to].colour = board[from].colour;
        board[to].type = board[from].type;
        board[from].type = none;

        can_move = can_move && !king_is_threatened(colour);

        board[to].colour = victim.colour;
        board[to].type = victim.type;
        board[from].type = aggressor.type;
        
        
        return can_move;
    }

    bool move(int player, int from, int to)
    {
        bool can_move = legal(player, from, to);
        if (!can_move) return false;

        if (board[from].type == pawn)
        {
            if (abs(from - to) == 16)
            {
                last_enpassant = from + 8 * ((board[from].colour == white) * 2 - 1);
                last_enpassant_victim = to;
            }

            else last_enpassant = -1;

            if (to == last_enpassant)
            {
                score[board[from].colour] += worth[board[last_enpassant_victim].type];
                board[last_enpassant_victim].type = none;
            }

        }

        if (board[from].type == king)
        {
            castling_moved[player][0] = true;
        }

        if (board[from].type == rook && (from % 8== 1 || from % 8== 7) )
        {
            castling_moved[player][(from % 8- 1) / 6 + 1];
        }

        score[board[from].colour] += worth[board[to].type];

        board[to].colour = board[from].colour;
        board[to].type = board[from].type;
        board[from].type = none;

        return  true;
    }

    piece piece_at(int pos)
    {
        return board[pos];
    }

    bool castling_possible(int player, int king, int rook)
    {
        if (threatened(king)) return false;
        if (board[king].colour != player || board[rook].colour != player) return false;
        if (rook != 0 && rook != 7 && rook != 56 && rook !=  63) return false;
        if (castling_moved[player][0] || castling_moved[player][(rook % 8- 1) / 6 + 1]) return false;

        bool possible = true;

        for (int i = king; i < rook; i += (rook - king > 0) ? 1 : -1)
            possible = possible && (board[i].type == none);
        

        return possible;
    }

    bool castle (int player, int king, int rook)
    {
        if (!castling_possible(player, king, rook)) return false;

        int dir = (rook - king > 0) ? 1 : -1;
        int kdest;
        int rdest;

        kdest = king + 2 * dir;
        rdest = king + dir;

        board[kdest].type = chess::king;
        board[kdest].colour = player;

        board[rdest].type = chess::rook;
        board[rdest].colour = player;
        board[king].type = none;
        board[rook].type = none;
    }

    int win()
    {
        bool kings[2] = {false, false};

        for (int i = 0; i < 64; i++)
        {
            if (board[i].type = king)
            {
                kings[board[i].colour] = true;
            }
        }

        if (!kings[white]) return white;
        if (!kings[black]) return black;
        return -1;

    }
};

