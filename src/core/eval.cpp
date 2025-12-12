#include <eval.h>
#include <unordered_map>

namespace Evaluation {

    int evaluate(Board &board) {
        int white_material = count_material(board, chess::color::WHITE);
        int black_material = count_material(board, chess::color::BLACK);

        int evaluation = white_material - black_material;
        int perspective = board.turn_ == chess::color::WHITE ? 1 : -1;

        return evaluation * perspective;
    }

    int count_material(Board &board, bool color) {
        int material = 0;

        for (int sq=0; sq < 64; sq++) {
            auto piece = Piece(board.board_[sq].piece_type_, board.board_[sq].piece_color_);
            if (piece.piece_type_ != chess::piece::EMPTY && piece.piece_type_ != chess::piece::KING && piece.piece_color_ == color ) {
                material += chess::evaluation::PIECE_VALUE_MAP.at(piece.piece_type_) + get_position_bonus(piece.piece_type_, sq, color);
            }
        }
        return material;
    }

    int get_position_bonus(int piece_type, int square, bool color) {
        std::vector<int> piece_sq_table = chess::piece_sq_table::PIECE_TABLE_MAP.at(piece_type);

        if (color == chess::color::WHITE) {
            // we will reverse the board for white
            return piece_sq_table[chess::square::H8-square];
        }
        return piece_sq_table[square];
    }
}