#include <eval.h>
#include <iostream>

namespace Evaluation {

    int evaluate(const Board &board) {
        // score should be +ve for white and -ve for black

        int white_score = 0;
        int black_score = 0;

        int white_material_score = 0;
        int black_material_score = 0;

        int white_position_score = 0;
        int black_position_score = 0;

        int endgame_phase = get_endgame_phase(board);

        for (int sq = 0; sq < 64; ++sq) {
            const auto& piece = board.board_[sq];

            if (piece == chess::piece_type::EMPTY) continue;

            int piece_type = Piece::type_(piece);
            int piece_color = Piece::color_(piece);
            if (piece_color == chess::color::WHITE) {
                white_material_score += get_piece_material_value(piece_type);
                white_position_score += get_position_bonus(piece_type, sq, piece_color, endgame_phase);
            }else {
                black_material_score += get_piece_material_value(piece_type);
                black_position_score += get_position_bonus(piece_type, sq, piece_color, endgame_phase);
            }

        }

        int white_mopup_score = get_mopup_score(board, chess::color::WHITE, white_material_score, black_material_score, endgame_phase);
        int black_mopup_score = get_mopup_score(board, chess::color::BLACK, black_material_score, white_material_score, endgame_phase);

        white_score = white_material_score + white_position_score + white_mopup_score;
        black_score = black_material_score + black_position_score + black_mopup_score;

        int score = white_score - black_score;
        int perspective = board.turn_ == chess::color::WHITE ? 1 : -1;
        // return the positive evaluation for whoever's turn it is to move
        return perspective * score;
    }

    // Tapered Eval
    int get_endgame_phase(const Board &board) {
        using namespace chess::evaluation;
        int weight_sum = 0;

        int white_queen_count  = __builtin_popcountll(board.piece_bitboard_[chess::piecelists::WHITE_QUEEN]);
        int black_queen_count  = __builtin_popcountll(board.piece_bitboard_[chess::piecelists::BLACK_QUEEN]);

        int white_rook_count   = __builtin_popcountll(board.piece_bitboard_[chess::piecelists::WHITE_ROOK]);
        int black_rook_count   = __builtin_popcountll(board.piece_bitboard_[chess::piecelists::BLACK_ROOK]);

        int white_bishop_count = __builtin_popcountll(board.piece_bitboard_[chess::piecelists::WHITE_BISHOP]);
        int black_bishop_count = __builtin_popcountll(board.piece_bitboard_[chess::piecelists::BLACK_BISHOP]);

        int white_knight_count = __builtin_popcountll(board.piece_bitboard_[chess::piecelists::WHITE_KNIGHT]);
        int black_knight_count = __builtin_popcountll(board.piece_bitboard_[chess::piecelists::BLACK_KNIGHT]);

        weight_sum += (white_queen_count + black_queen_count) * ENDGAME_WEIGHT_QUEEN;
        weight_sum += (white_rook_count + black_rook_count) * ENDGAME_WEIGHT_ROOK;
        weight_sum += (white_bishop_count + black_bishop_count) * ENDGAME_WEIGHT_BISHOP;
        weight_sum += (white_knight_count + black_knight_count) * ENDGAME_WEIGHT_KNIGHT;

        // Ensure we don't divide by zero, though unlikely with constants

        float endgame_t = 1.0f - std::min(1.0f, (float)weight_sum / ENDGAME_START_WEIGHT);
        return (int)(endgame_t * 256);

    }

    int get_mopup_score(const Board &board, const int color, const int friend_material_score,
                        const int enemy_material_score, const int endgame_phase) {

        if (friend_material_score - enemy_material_score > 2 * chess::evaluation::PAWN && endgame_phase > 0 ) {

            int friend_king_square = color == chess::color::WHITE ? board.white_king_square_ : board.black_king_square_;
            int enemy_king_square = color == chess::color::WHITE ? board.black_king_square_ : board.white_king_square_;

            int cmd = center_manhattan_distance(enemy_king_square);
            int md = manhattan_distance(friend_king_square, enemy_king_square);
            int mopup_score = 10 * cmd + 4 * (14 - md);

            return (mopup_score * endgame_phase) / 256;
        }
        return 0;
    }

    int center_manhattan_distance(int sq) {
        return chess::evaluation::CENTER_MANHATTAN_DISTANCE[sq];
    }

    int manhattan_distance(int sq1, int sq2) {
        int file1 = Square::file_(sq1);
        int file2 = Square::file_(sq2);
        int rank1 = Square::rank_(sq1);
        int rank2 = Square::rank_(sq2);
        return abs (rank2 - rank1) + abs (file2 - file1);
    }



    int get_piece_material_value(int piece_type) {
        return chess::evaluation::PIECE_VALUES[piece_type];
    }

    int get_position_bonus(int piece_type, int square, int color, int endgame_phase) {
        using namespace chess::piece_sq_table;
        int pst_square = (color == chess::color::WHITE) ? chess::square::H8 - square : square;
        int mg_score = PIECE_SQUARE_TABLE_MG[piece_type][pst_square];
        int eg_score = PIECE_SQUARE_TABLE_EG[piece_type][pst_square];

        return mg_score + ((endgame_phase * (eg_score - mg_score)) / 256);
    }

}
