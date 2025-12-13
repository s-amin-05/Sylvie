#include <eval.h>
#include <unordered_map>

namespace Evaluation {

    int evaluate(const Board &board) {
        const int white_material = count_material(board, chess::color::WHITE);
        const int black_material = count_material(board, chess::color::BLACK);

        const int evaluation = white_material - black_material;
        const int perspective = board.turn_ == chess::color::WHITE ? 1 : -1;

        return evaluation * perspective;
    }

    int count_material(const Board &board, const bool color) {
        int material = 0;

        for (int sq=0; sq < 64; sq++) {
            int piece_type = board.board_[sq].piece_type_;
            int piece_color = board.board_[sq].piece_color_;
            if (piece_type != chess::piece::EMPTY && piece_type != chess::piece::KING && piece_color == color ) {
                material += chess::evaluation::PIECE_VALUE_MAP.at(piece_type) + get_position_bonus(piece_type, sq, color);
            }
        }
        return material;
    }

    int get_position_bonus(const int piece_type, const int square, const bool color) {
        const std::vector<int>& piece_sq_table = chess::piece_sq_table::PIECE_TABLE_MAP.at(piece_type);

        if (color == chess::color::WHITE) {
            // we will reverse the board for white
            return piece_sq_table[chess::square::H8-square];
        }
        return piece_sq_table[square];
    }
}