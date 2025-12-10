#pragma once
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

    int count_material(Board &board, int color) {
        int material = 0;

        for (int sq=0; sq < 64; sq++) {
            auto piece = Piece(board.board_[sq].piece_type_, board.board_[sq].piece_color_);
            if (piece.piece_type_ != chess::piece::EMPTY && piece.piece_type_ != chess::piece::KING && piece.piece_color_ == color ) {
                material += PieceValues::piece_value_map.at(piece.piece_type_);
            }
        }
        return material;
    }
}