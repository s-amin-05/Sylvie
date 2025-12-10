#pragma once
#include <board.h>
#include <climits>
#include <unordered_map>

namespace Evaluation {
    int evaluate(Board &board);
    int count_material(Board &board, int color);
}

namespace Evaluation::PieceValues {
    constexpr int PAWN = 100;
    constexpr int KNIGHT = 300;
    constexpr int BISHOP = 350;
    constexpr int ROOK = 500;
    constexpr int QUEEN = 900;

    const std::unordered_map<int, int> piece_value_map = {
        {chess::piece::PAWN, PAWN},
        {chess::piece::KNIGHT, KNIGHT},
        {chess::piece::BISHOP, BISHOP},
        {chess::piece::ROOK, ROOK},
        {chess::piece::QUEEN, QUEEN}
    };
}