#include <algorithm>
#include <random>
#include <iostream>
#include <thread>

#include "board.hpp"
#include "engine.hpp"
#include "butils.hpp"

#define INT_MAX 1e8
#define INT_MIN -1e8
#define DEPTH INT_MAX

using namespace std;
clock_t first_start;
clock_t start;
double time_limit;
unordered_map<int, int> boardPositions;

int evaluate(const Board& b, U8 player);
int minimax(Board& b, int alpha, int beta, bool maximizingPlayer, int depth, U8 player);

int get_hash_index(const Board& b) {
    int index = 0;
    vector<U8> pieces;
    if (b.data.board_type == 1) {
        pieces = {
            b.data.w_king,
            b.data.w_bishop,
            b.data.w_rook_1,
            b.data.w_rook_2,
            b.data.w_pawn_1,
            b.data.w_pawn_2,
            b.data.b_king,
            b.data.b_bishop,
            b.data.b_rook_1,
            b.data.b_rook_2,
            b.data.b_pawn_1,
            b.data.b_pawn_2
        };
        
    } else if (b.data.board_type == 2) {
        pieces = {
            b.data.w_king,
            b.data.w_bishop,
            b.data.w_rook_1,
            b.data.w_rook_2,
            b.data.w_pawn_1,
            b.data.w_pawn_2,
            b.data.w_pawn_3,
            b.data.w_pawn_4,
            b.data.b_king,
            b.data.b_bishop,
            b.data.b_rook_1,
            b.data.b_rook_2,
            b.data.b_pawn_1,
            b.data.b_pawn_2,
            b.data.b_pawn_3,
            b.data.b_pawn_4
        };
    } else if (b.data.board_type == 3) {
        pieces = {
            b.data.w_king,
            b.data.w_bishop,
            b.data.w_rook_1,
            b.data.w_rook_2,
            b.data.w_pawn_1,
            b.data.w_pawn_2,
            b.data.w_pawn_3,
            b.data.w_pawn_4,
            b.data.w_knight_1,
            b.data.w_knight_2,
            b.data.b_king,
            b.data.b_bishop,
            b.data.b_rook_1,
            b.data.b_rook_2,
            b.data.b_pawn_1,
            b.data.b_pawn_2,
            b.data.b_pawn_3,
            b.data.b_pawn_4,
            b.data.b_knight_1,
            b.data.b_knight_2
        };
    }
    for (int i = 1; i <= pieces.size(); i++) {
        if (pieces[i-1] != 0xff) {
            index = index + (pieces[i-1] * i);
        }
    }
    // cout << "INDEX: " << (index%100) << endl;
    return (index % 100);
}


pair<int, int> pawnPromotion(U16 move, U8 player) {
    int white_score = 0;
    int black_score = 0;
    string m = move_to_str(move);
    if (m.size() > 4) {
        if (player == WHITE) {
            if (m[4] == 'r') {
                white_score += 4;
            } else {
                white_score += 5;
            }
        } else if (player == BLACK) {
            if (m[4] == 'r') {
                black_score += 4;
            } else {
                black_score += 5;
            }
        }
        
    } 
    return make_pair(white_score, black_score);
}

// pair<int, int> piecesWeight(Board& b) {
//     string representation = board_to_str(&b.data);
//     int white_score = 0;
//     int black_score = 0;
//     if (b.data.board_type == 1) {
//         for (int i = 0; i < 7; i++){
//             for (int j = 0; j < 7; j++){
//                 char piece_found = representation[8*i + j];
//                 if (piece_found == 'P') {
//                     white_score += 1;
//                 } else if (piece_found == 'K') {
//                     white_score += 10;
//                 } else if (piece_found == 'R') {
//                     white_score += 4;
//                 } else if (piece_found == 'B') {
//                     white_score += 5;
//                 } else if (piece_found == 'p') {
//                     black_score += 1;
//                 } else if (piece_found == 'k') {
//                     black_score += 10;
//                 } else if (piece_found == 'r') {
//                     black_score += 4;
//                 } else if (piece_found == 'b') {
//                     black_score += 5;
//                 }
//             }
//         }
//     }
//     // aanya to write the other loops
//     return make_pair(white_score, black_score);
// }

pair<int, int> piecesWeight(Board& b) {
    string representation = board_to_str(&b.data);
    int white_score = 0;
    int black_score = 0;
    if (b.data.board_type == 1) {
        for (int i = 0; i < 7; i++){
            for (int j = 0; j < 7; j++){
                char piece_found = representation[8*i + j];
                if (piece_found == 'P') {
                    white_score += 1;
                    // black_score -= 1;
                } else if (piece_found == 'K') {
                    white_score += 10;
                    // black_score -= 10;
                } else if (piece_found == 'R') {
                    white_score += 8;
                    // black_score -= 4;
                } else if (piece_found == 'B') {
                    white_score += 8;
                    // black_score -= 5;
                } else if (piece_found == 'p') {
                    black_score += 1;
                    // white_score -= 1;
                } else if (piece_found == 'k') {
                    black_score += 10;
                    // white_score -= 10;
                } else if (piece_found == 'r') {
                    black_score += 8;
                    // white_score -= 4;
                } else if (piece_found == 'b') {
                    black_score += 8;
                    // white_score -= 5;
                }
            }
        }
    }

    else if (b.data.board_type == 2 || b.data.board_type == 3 ){
        for (int i = 0; i < 8; i++){
            for (int j = 0; j < 8; j++){
                char piece_found = representation[9*i + j];
                if (piece_found == 'P') {
                    white_score += 1;
                    // black_score -= 1;
                } else if (piece_found == 'K') {
                    white_score += 10;
                    // black_score -= 10;
                } else if (piece_found == 'R') {
                    white_score += 8;
                    // black_score -= 4;
                } else if (piece_found == 'B') {
                    white_score += 8;
                    // black_score -= 5;
                } else if (piece_found == 'N') {
                    white_score += 3;
                    // black_score -= 3;
                } else if (piece_found == 'p') {
                    black_score += 1;
                    // white_score -= 1;
                } else if (piece_found == 'k') {
                    black_score += 10;
                    // white_score -= 10;
                } else if (piece_found == 'r') {
                    black_score += 8;
                    // white_score -= 4;
                } else if (piece_found == 'b') {
                    black_score += 8;
                    // white_score -= 5;
                } else if (piece_found == 'n') {
                    black_score += 3;
                    // white_score -= 3;
                }
            }
        }

    }
    // aanya to write the other loops
    return make_pair(white_score, black_score);
}

pair<int, int> capture(Board& b) { // checks if an opponent piece is killed
    int white_score = 0;
    int black_score = 0;

    if (b.data.last_killed_piece != 0) {

        if (b.data.last_killed_piece == (BLACK | PAWN)) {
            // cout << "Black pawn\n";
            white_score += 1;
            // black_score -= 1;
        } else if (b.data.last_killed_piece == (BLACK | ROOK)) {
            // cout << "Black rook\n";
            white_score += 8;
            // black_score -= 4;
        } else if (b.data.last_killed_piece == (BLACK | BISHOP)) {
            // cout << "Black bishop\n";
            white_score += 8;
            // black_score -= 5;
        } else if (b.data.last_killed_piece == (WHITE | PAWN)) {
            // cout << "White pawn\n";
            black_score += 1;
            // white_score -= 1;
        } else if (b.data.last_killed_piece == (WHITE | ROOK)) {
            // cout << "White rook\n";
            black_score += 8;
            // white_score -= 4;
        } else if (b.data.last_killed_piece == (WHITE | BISHOP)) {
            // cout << "White bishop\n";
            black_score += 8;
            // white_score -= 5;
        }
        
        if (b.data.board_type == 3) {
            if (b.data.last_killed_piece == (BLACK | KNIGHT)) {
                white_score += 3;
                // black_score -= 3;
            } else if (b.data.last_killed_piece == (WHITE | KNIGHT)) {
                black_score += 3;
                // white_score -= 3;
            }
        }
        
    }


    return make_pair(white_score, black_score);
}

bool under_threat(Board& b, U8 piece_pos, U8 color) {
    auto pseudolegal_moves = b.get_pseudolegal_moves_for_side(color);

    for (auto move : pseudolegal_moves) {
        // std::cout << move_to_str(move) << " ";
        if (getp1(move) == piece_pos) {
            // std::cout << "<- causes check\n";
            return true;
        }
    }
    return false;
}

pair<int, int> threat(Board& b) { // to check if any of the players piece is under threat by the opponent
    int white_score = 0;
    int black_score = 0;

    if (b.data.board_type == 1) {
        // black
        if (under_threat(b, b.data.w_pawn_1, BLACK) == true) {
            black_score += 1;
            // white_score -= 1;
        }
        if (under_threat(b, b.data.w_pawn_2, BLACK) == true) {
            black_score += 1;
            // white_score -= 1;
        }
        if (under_threat(b, b.data.w_rook_1, BLACK) == true) {
            black_score += 8;
            // white_score -= 4;
        }
        if (under_threat(b, b.data.w_rook_2, BLACK) == true) {
            black_score += 8;
            // white_score -= 4;
        }
        if (under_threat(b, b.data.w_bishop, BLACK) == true) {
            black_score += 8;
            // white_score -= 5;
        }
        if (under_threat(b, b.data.w_king, BLACK) == true) {
            black_score += 10;
            // white_score -= 10;
        }
        // white
        if (under_threat(b, b.data.b_pawn_1, WHITE) == true) {
            white_score += 1;
            // black_score -= 1;
        }
        if (under_threat(b, b.data.b_pawn_2, WHITE) == true) {
            white_score += 1;
            // black_score -= 1;
        }
        if (under_threat(b, b.data.b_rook_1, WHITE) == true) {
            white_score += 8;
            // black_score -= 4;
        }
        if (under_threat(b, b.data.b_rook_2, WHITE) == true) {
            white_score += 8;
            // black_score -= 4;
        }
        if (under_threat(b, b.data.b_bishop, WHITE) == true) {
            white_score += 8;
            // black_score -= 5;
        }
        if (under_threat(b, b.data.b_king, WHITE) == true) {
            white_score += 10;
            // black_score -= 10;
        }
    } else if (b.data.board_type == 2) {
        // black
        if (under_threat(b, b.data.w_pawn_1, BLACK) == true) {
            black_score += 1;
            // white_score -= 1;
        }
        if (under_threat(b, b.data.w_pawn_2, BLACK) == true) {
            black_score += 1;
            // white_score -= 1;
        }
        if (under_threat(b, b.data.w_pawn_3, BLACK) == true) {
            black_score += 1;
            // white_score -= 1;
        }
        if (under_threat(b, b.data.w_pawn_4, BLACK) == true) {
            black_score += 1;
            // white_score -= 1;
        }
        if (under_threat(b, b.data.w_rook_1, BLACK) == true) {
            black_score += 8;
            // white_score -= 4;
        }
        if (under_threat(b, b.data.w_rook_2, BLACK) == true) {
            black_score += 8;
            // white_score -= 4;
        }
        if (under_threat(b, b.data.w_bishop, BLACK) == true) {
            black_score += 8;
            // white_score -= 5;
        }
        if (under_threat(b, b.data.w_king, BLACK) == true) {
            black_score += 10;
            // white_score -= 10;
        }
        // white
        if (under_threat(b, b.data.b_pawn_1, WHITE) == true) {
            white_score += 1;
            // black_score -= 1;
        }
        if (under_threat(b, b.data.b_pawn_2, WHITE) == true) {
            white_score += 1;
            // black_score -= 1;
        }
        if (under_threat(b, b.data.b_pawn_3, WHITE) == true) {
            white_score += 1;
            // black_score -= 1;
        }
        if (under_threat(b, b.data.b_pawn_4, WHITE) == true) {
            white_score += 1;
            // black_score -= 1;
        }
        if (under_threat(b, b.data.b_rook_1, WHITE) == true) {
            white_score += 8;
            // black_score -= 4;
        }
        if (under_threat(b, b.data.b_rook_2, WHITE) == true) {
            white_score += 8;
            // black_score -= 4;
        }
        if (under_threat(b, b.data.b_bishop, WHITE) == true) {
            white_score += 8;
            // black_score -= 5;
        }
        if (under_threat(b, b.data.b_king, WHITE) == true) {
            white_score += 10;
            // black_score -= 10;
        }
    } else if (b.data.board_type == 3) {
        // black
        if (under_threat(b, b.data.w_pawn_1, BLACK) == true) {
            black_score += 1;
            // white_score -= 1;
        }
        if (under_threat(b, b.data.w_pawn_2, BLACK) == true) {
            black_score += 1;
            // white_score -= 1;
        }
        if (under_threat(b, b.data.w_pawn_3, BLACK) == true) {
            black_score += 1;
            // white_score -= 1;
        }
        if (under_threat(b, b.data.w_pawn_4, BLACK) == true) {
            black_score += 1;
            // white_score -= 1;
        }
        if (under_threat(b, b.data.w_rook_1, BLACK) == true) {
            black_score += 8;
            // white_score -= 4;
        }
        if (under_threat(b, b.data.w_rook_2, BLACK) == true) {
            black_score += 8;
            // white_score -= 4;
        }
        if (under_threat(b, b.data.w_knight_1, BLACK) == true) {
            black_score += 3;
            // white_score -= 3;
        }
        if (under_threat(b, b.data.w_knight_2, BLACK) == true) {
            black_score += 3;
            // white_score -= 3;
        }
        if (under_threat(b, b.data.w_bishop, BLACK) == true) {
            black_score += 8;
            // white_score -= 5;
        }
        if (under_threat(b, b.data.w_king, BLACK) == true) {
            black_score += 10;
            // white_score -= 10;
        }
        // white
        if (under_threat(b, b.data.b_pawn_1, WHITE) == true) {
            white_score += 1;
            // black_score -= 1;
        }
        if (under_threat(b, b.data.b_pawn_2, WHITE) == true) {
            white_score += 1;
            // black_score -= 1;
        }
        if (under_threat(b, b.data.b_pawn_3, WHITE) == true) {
            white_score += 1;
            // black_score -= 1;
        }
        if (under_threat(b, b.data.b_pawn_4, WHITE) == true) {
            white_score += 1;
            // black_score -= 1;
        }
        if (under_threat(b, b.data.b_rook_1, WHITE) == true) {
            white_score += 8;
            // black_score -= 4;
        }
        if (under_threat(b, b.data.b_rook_2, WHITE) == true) {
            white_score += 8;
            // black_score -= 4;
        }
        if (under_threat(b, b.data.b_knight_1, WHITE) == true) {
            white_score += 3;
            // black_score -= 3;
        }
        if (under_threat(b, b.data.b_knight_2, WHITE) == true) {
            white_score += 3;
            // black_score -= 3;
        }
        if (under_threat(b, b.data.b_bishop, WHITE) == true) {
            white_score += 8;
            // black_score -= 5;
        }
        if (under_threat(b, b.data.b_king, WHITE) == true) {
            white_score += 10;
            // black_score -= 10;
        }
    }

    return make_pair(white_score, black_score);
}

pair<int, int> king_under_threat(Board& b) {  // new board, new player is opponent, player is parent player
    int white_score = 0;
    int black_score = 0;
    if (b.data.player_to_play == BLACK) {
        if (b.under_threat(b.data.b_king) == true && !b.get_legal_moves().empty()) { // check
            white_score = 1e4;
        } else if (b.under_threat(b.data.b_king) == true && b.get_legal_moves().empty()) { // checkmate
            white_score = INT_MAX;
        }
    }
    else if (b.data.player_to_play == WHITE) {
        if (b.under_threat(b.data.w_king) == true && !b.get_legal_moves().empty()) { // check
            black_score = 1e4;
        } else if (b.under_threat(b.data.w_king) == true && b.get_legal_moves().empty()) { // checkmate
            black_score = INT_MAX;
        }
    }

    return make_pair(white_score, black_score);
    
}

pair<int, int> manhattanDistance(Board& b) {
    int white_score = 0;
    int black_score = 0;

    if (b.data.board_type == 1){
        if (b.data.w_pawn_1 != DEAD) {
            int x1 = getx(b.data.w_pawn_1);
            int y1 = gety(b.data.w_pawn_1);

            // cout << "X1: " << x1 << " Y2: " << y1 << endl;

            white_score += abs(x1-4) + abs(y1-5);
            white_score += abs(x1-4) + abs(y1-6);
        }
        if (b.data.w_pawn_2 != DEAD) {
            int x1 = getx(b.data.w_pawn_2);
            int y1 = gety(b.data.w_pawn_2);

            white_score += abs(x1-4) + abs(y1-5);
            white_score += abs(x1-4) + abs(y1-6);
        }
        if (b.data.b_pawn_1 != DEAD) {
            int x1 = getx(b.data.b_pawn_1);
            int y1 = gety(b.data.b_pawn_1);

            black_score += abs(x1-2) + abs(y1-0);
            black_score += abs(x1-2) + abs(y1-1);
        }
        if (b.data.b_pawn_2 != DEAD) {
            int x1 = getx(b.data.b_pawn_2);
            int y1 = gety(b.data.b_pawn_2);

            black_score += abs(x1-2) + abs(y1-0);
            black_score += abs(x1-2) + abs(y1-1);
        }
        
    } else if (b.data.board_type == 2) {
        if (b.data.w_pawn_1 != DEAD) {
            int x1 = getx(b.data.w_pawn_1);
            int y1 = gety(b.data.w_pawn_1);

            white_score += abs(x1-5) + abs(y1-6);
            white_score += abs(x1-5) + abs(y1-7);
        }
        if (b.data.w_pawn_2 != DEAD) {
            int x1 = getx(b.data.w_pawn_2);
            int y1 = gety(b.data.w_pawn_2);

            white_score += abs(x1-5) + abs(y1-6);
            white_score += abs(x1-5) + abs(y1-7);
        }
        if (b.data.w_pawn_3 != DEAD) {
            int x1 = getx(b.data.w_pawn_3);
            int y1 = gety(b.data.w_pawn_3);

            white_score += abs(x1-5) + abs(y1-6);
            white_score += abs(x1-5) + abs(y1-7);
        }
        if (b.data.w_pawn_4 != DEAD) {
            int x1 = getx(b.data.w_pawn_4);
            int y1 = gety(b.data.w_pawn_4);

            white_score += abs(x1-5) + abs(y1-6);
            white_score += abs(x1-5) + abs(y1-7);
        }
        if (b.data.b_pawn_1 != DEAD) {
            int x1 = getx(b.data.b_pawn_1);
            int y1 = gety(b.data.b_pawn_1);

            black_score += abs(x1-2) + abs(y1-0);
            black_score += abs(x1-2) + abs(y1-1);
        }
        if (b.data.b_pawn_2 != DEAD) {
            int x1 = getx(b.data.b_pawn_2);
            int y1 = gety(b.data.b_pawn_2);

            black_score += abs(x1-2) + abs(y1-0);
            black_score += abs(x1-2) + abs(y1-1);
        }
        if (b.data.b_pawn_3 != DEAD) {
            int x1 = getx(b.data.b_pawn_3);
            int y1 = gety(b.data.b_pawn_3);

            black_score += abs(x1-2) + abs(y1-0);
            black_score += abs(x1-2) + abs(y1-1);
        }
        if (b.data.b_pawn_4 != DEAD) {
            int x1 = getx(b.data.b_pawn_4);
            int y1 = gety(b.data.b_pawn_4);

            black_score += abs(x1-2) + abs(y1-0);
            black_score += abs(x1-2) + abs(y1-1);
        }
    } else if (b.data.board_type == 3) {
        if (b.data.w_pawn_1 != DEAD) {
            int x1 = getx(b.data.w_pawn_1);
            int y1 = gety(b.data.w_pawn_1);

            white_score += abs(x1-4) + abs(y1-5);
            white_score += abs(x1-4) + abs(y1-6);
            white_score += abs(x1-4) + abs(y1-7);
        }
        if (b.data.w_pawn_2 != DEAD) {
            int x1 = getx(b.data.w_pawn_2);
            int y1 = gety(b.data.w_pawn_2);

            white_score += abs(x1-4) + abs(y1-5);
            white_score += abs(x1-4) + abs(y1-6);
            white_score += abs(x1-4) + abs(y1-7);
        }
        if (b.data.w_pawn_3 != DEAD) {
            int x1 = getx(b.data.w_pawn_3);
            int y1 = gety(b.data.w_pawn_3);

            white_score += abs(x1-4) + abs(y1-5);
            white_score += abs(x1-4) + abs(y1-6);
            white_score += abs(x1-4) + abs(y1-7);
        }
        if (b.data.w_pawn_4 != DEAD) {
            int x1 = getx(b.data.w_pawn_4);
            int y1 = gety(b.data.w_pawn_4);

            white_score += abs(x1-4) + abs(y1-5);
            white_score += abs(x1-4) + abs(y1-6);
            white_score += abs(x1-4) + abs(y1-7);
        }
        if (b.data.b_pawn_1 != DEAD) {
            int x1 = getx(b.data.b_pawn_1);
            int y1 = gety(b.data.b_pawn_1);

            black_score += abs(x1-3) + abs(y1-0);
            black_score += abs(x1-3) + abs(y1-1);
            black_score += abs(x1-3) + abs(y1-2);
        }
        if (b.data.b_pawn_2 != DEAD) {
            int x1 = getx(b.data.b_pawn_2);
            int y1 = gety(b.data.b_pawn_2);

            black_score += abs(x1-3) + abs(y1-0);
            black_score += abs(x1-3) + abs(y1-1);
            black_score += abs(x1-3) + abs(y1-2);
        }
        if (b.data.b_pawn_3 != DEAD) {
            int x1 = getx(b.data.b_pawn_3);
            int y1 = gety(b.data.b_pawn_3);

            black_score += abs(x1-3) + abs(y1-0);
            black_score += abs(x1-3) + abs(y1-1);
            black_score += abs(x1-3) + abs(y1-2);
        }
        if (b.data.b_pawn_4 != DEAD) {
            int x1 = getx(b.data.b_pawn_4);
            int y1 = gety(b.data.b_pawn_4);

            black_score += abs(x1-3) + abs(y1-0);
            black_score += abs(x1-3) + abs(y1-1);
            black_score += abs(x1-3) + abs(y1-2);
        }
    }
    
    
    return make_pair(white_score, black_score);
}

int evaluate(Board& b, U8 player) { // player is parent player that is player before playing the game
    // U8 player = b.data.player_to_play;
    // U8 player;

    int white_score = 0;
    int black_score = 0;
    pair<int, int> score;

    // cout << "3\n";

    score = piecesWeight(b); // 1
    white_score += 200*(score.first);
    black_score += 200*(score.second);
    // cout << "4\n";

    score = capture(b); // 2
    white_score += 150*(score.first);
    black_score += 150*(score.second);
    // cout << "5\n";

    score = threat(b); // 3
    white_score += 70*(score.first);
    black_score += 70*(score.second);
    // // cout << "6\n";

    score = king_under_threat(b); // 5
    white_score += score.first;
    black_score += score.second;
    // // cout << "8\n";

    score = manhattanDistance(b);
    white_score -= 20*(score.first);
    black_score -= 20*(score.second);

    // if (player == BLACK) {
    //     return white_score - black_score;
    // } else if (player == WHITE) {
    //     return black_score - white_score;
    // }

    // return 0;
    return white_score - black_score;
    
}


int minimax(Board& b, int alpha, int beta, bool maximizingPlayer, int depth, U8 player) {

    unordered_set<U16> moves = b.get_legal_moves();

    if (depth == 0 || moves.size() == 0) {
        // cout << "Score: " << evaluate(b, player) << endl;
        return evaluate(b, player);
    }

    if (maximizingPlayer) { // our turn
        // cout << "Max turn\n";

        int maxScore = INT_MIN;
        for (U16 move: moves) {
            Board newBoard(b);
            newBoard.do_move_(move);

            start = clock();
            if ((start-first_start)/CLOCKS_PER_SEC >= (time_limit)*0.9) {
                break;
            }

            int eval = minimax(newBoard, alpha, beta, false, depth-1, player);
            if (eval == INT_MAX) {
                maxScore = INT_MAX;
                break;
            }
            // newBoard._undo_last_move(move);
            maxScore = max(maxScore, eval);
            alpha = max(alpha, maxScore);
            if (alpha >= beta) {
                // cout << "pruned\n";
                break;
            }

            start = clock();
            if ((start-first_start)/CLOCKS_PER_SEC >= (time_limit)*0.9) {
                break;
            }
           
        }
        
        return maxScore;
        
    } else {
        int minScore = INT_MAX;
        for (U16 move: moves) {
            Board newBoard(b);
            newBoard.do_move_(move);

            start = clock();
            if ((start-first_start)/CLOCKS_PER_SEC >= (time_limit)*0.99) {
                break;
            }

            int eval = minimax(newBoard, alpha, beta, true, depth-1, player);
            if (eval == INT_MIN) {
                minScore = INT_MIN;
                break;
            }
            // newBoard._undo_last_move(move);
            minScore = min(minScore, eval);
            beta = min(beta, minScore);
            if (alpha >= beta) {
                // cout << "pruned\n";
                break;
            }

            start = clock();
            if ((start-first_start)/CLOCKS_PER_SEC >= (time_limit)*0.9) {
                break;
            }
            
        }
        return minScore;
    } 
    

}

void Engine::find_best_move(const Board& b) {

    // pick a random move
    first_start = clock();
    time_limit = 1.5;
    std::chrono::milliseconds time_left_ms = this->time_left;
    double time_left_sec = static_cast<double>(time_left_ms.count()) / 1000.0; // Convert milliseconds to seconds

    double total_time = 60;
    cout << time_left_sec << endl;
    if(time_left_sec>total_time - total_time/8)
    {
        time_limit = 0.9;
    }
    else if(time_left_sec>total_time - total_time/4)
    {
        time_limit = 1.25;
    }
    else if(time_left_sec>total_time/4)
    {
        time_limit = 1.5;
    }
    else if(time_left_sec>total_time/8)
    {
        time_limit = 0.9;
    }
    else
    {
        time_limit = 1;
    }
    cout << "Time Limit: " << time_limit << endl;

    auto moveset = b.get_legal_moves();
    if (moveset.size() == 0) {
        std::cout << "Could not get any moves from board!\n";
        std::cout << board_to_str(&b.data);
        this->best_move = 0;
    }
    else {
        // std::vector<U16> moves;
        std::cout << show_moves(&b.data, moveset) << std::endl;
        
        std::cout << std::endl;
        int x1 = getx(b.data.w_pawn_1);
        int y1 = gety(b.data.w_pawn_1);

        cout << "X1: " << x1 << " Y2: " << y1 << endl;
        U8 player = b.data.player_to_play;
        cout << "Player: " << player << endl;
        vector<U16> moves(moveset.begin(), moveset.end()); 
        // for (auto m : moves) {
        //     std::cout << move_to_str(m) << " ";
        // }
        cout << endl;
        cout << "Eval: ";
        for (auto m: moves) {
            Board n(b);
            n.do_move_(m);
            int x = evaluate(n, player);
            cout << move_to_str(m) << ": " << x << " ";
        }
        cout << endl;
        int m = INT_MIN;
        for (int depth = 1; depth <= DEPTH; depth++) {
                
            int alpha = INT_MIN;
            int beta = INT_MAX;
            int val;
            // auto moveset = b.get_legal_moves();
            int reps = 0;
            int len = moves.size();
            

            for (U16 move: moveset) {
                Board newBoard(b);
                newBoard.do_move_(move);
                start = clock();
                if ((start-first_start)/CLOCKS_PER_SEC >= (time_limit)*0.995) {
                    break;
                }

                // int currentPosition = get_hash_index(newBoard);
                
                // if (boardPositions[currentPosition] >= 2) {
                //     cout << "Three Fold Repetition\n";
                //     reps++;
                //     if (reps == len) {
                //         this->best_move = moves[0];
                //     }
                //     continue;
                // }

                val = minimax(newBoard, alpha, beta, false, depth, player);
                // cout << "move: " << move_to_str(move) << " Value1: " << val << endl;
                if (val > m) {
                    // cout<< "move: " << move_to_str(move) << " Value2: " << val << endl;
                    m = val;
                    this->best_move = move;
                }

                start = clock();
                if ((start-first_start)/CLOCKS_PER_SEC >= (time_limit)*0.995) {
                    break;
                }

            }
            // cout << "Depth: " << depth << endl;
            start = clock();
            if ((start-first_start)/CLOCKS_PER_SEC >= (time_limit)*0.995) {
                break;
            }
            
        }

    }


    // if (this->best_move > 0) {
    //     Board nb(b);
    //     nb.do_move_(this->best_move);
    //     int currentPosition = get_hash_index(nb);
    //     cout << "Pos: " << currentPosition << endl; 
    //     boardPositions[currentPosition]++;
    // }

    // just for debugging, to slow down the moves
    // std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}