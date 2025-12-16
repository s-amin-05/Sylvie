#include <eval.h>
#include <unordered_map>


namespace Evaluation {

    int evaluate(const Board &board) {
        int score = 0;

        // One loop, 64 iterations.
        for (int sq = 0; sq < 64; ++sq) {
            const auto& piece = board.board_[sq];

            if (piece == chess::piece_type::EMPTY) continue;

            int piece_type = Piece::type_(piece);
            int value = get_piece_material_value(piece_type);
            int position_bonus = get_position_bonus(piece_type, sq, Piece::color_(piece));
            int perspective = (Piece::color_(piece) == chess::color::WHITE ? 1 : -1);
            score += perspective * (value + position_bonus);
        }

        // 3. Return perspective score
        return (board.turn_ == chess::color::WHITE) ? score : -score;
    }

    int get_piece_material_value(int piece_type) {
        return chess::evaluation::PIECE_VALUES[piece_type];
    }

    int get_position_bonus(int piece_type, int square, int color) {
        const std::vector<int>& table = chess::piece_sq_table::PIECE_SQUARE_TABLE[piece_type];
        if (color == chess::color::WHITE) {
            return table[chess::square::H8 - square];
        }
        return table[square];
    }

}
