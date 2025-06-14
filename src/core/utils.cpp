#include<utils.h>
#include <sstream>

namespace Utils {
    std::vector<std::string> split(const std::string &s, char delim) {
        std::stringstream ss(s);
        std::string token;
        std::vector<std::string> string_parts;
        while (std::getline(ss, token, delim)) {
            string_parts.push_back(token);
        }

        return string_parts;
    }

    bool is_digit(char c) {
        return c >= '0' && c <= '9';
    }
}

namespace MoveUtils {
    // we will only change the state of Move and not the Board
    Move move_from_string(std::string move_string, Board board) {
        // This line sets
        // starting & target square and promotion piece if any
        Move move = Move(move_string);

        Square starting_square = move.starting_square_;
        Square target_square = move.target_square_;
        Piece moving_piece = board.board_[starting_square.square_];
        Piece target_piece = board.board_[target_square.square_];

        // check for enpassant
        if (moving_piece.piece_type_ == chess::piece::PAWN) {
            if (target_square.square_ == board.enpassant_target_.square_) {
                move.is_en_passant_ = true;
                move.is_capture_ = true;
            }
        }

        // check for captures
        if (target_piece.piece_type_ != chess::piece::EMPTY && target_piece.piece_color_ != moving_piece.piece_color_) {
            move.is_capture_ = true;
        }

        // check for castling
        if (moving_piece.piece_type_ == chess::piece::KING) {
            bool is_king_side_castle_black = board.castling_rights_ & 1U;
            bool is_queen_side_castle_black = board.castling_rights_ & 2U;
            bool is_king_side_castle_white = board.castling_rights_ & 4U;
            bool is_queen_side_castle_white = board.castling_rights_ & 8U;


            // we can skip checking piece type on the square since castling rights tell us if the king/rook has moved
            if (target_square.square_ == chess::square::G8 && is_king_side_castle_black ) {
                move.is_castling_ = true;
            }
            if (target_square.square_ == chess::square::C8 && is_queen_side_castle_black) {
                move.is_castling_ = true;
            }
            if (target_square.square_ == chess::square::G1 && is_king_side_castle_white) {
                move.is_castling_ = true;
            }
            if (target_square.square_ == chess::square::C1 && is_queen_side_castle_white) {
                move.is_castling_ = true;
            }
        }

        return move;
    }
}
