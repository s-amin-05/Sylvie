#include <cassert>
#include <iostream>
#include <sstream>
#include <utils.h>

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
    // // we will only change the state of Move and not the Board
    // Move move_from_string(std::string move_string, Board &board) {
    //     // This line sets
    //     // starting & target square and promotion piece if any
    //     Move move = Move(move_string);
    //
    //     Square starting_square = Square(move.starting_square_.square_);
    //     Square target_square = Square(move.target_square_.square_);
    //     Piece moving_piece = board.board_[starting_square.square_];
    //     Piece target_piece = board.board_[target_square.square_];
    //
    //     // check for enpassant
    //     if (moving_piece.piece_type_ == chess::piece::PAWN) {
    //         if (target_square.square_ == board.enpassant_target_.square_) {
    //             move.is_en_passant_ = true;
    //             move.is_capture_ = true;
    //         }
    //     }
    //
    //     // check for captures
    //     if (target_piece.piece_type_ != chess::piece::EMPTY && target_piece.piece_color_ != moving_piece.piece_color_) {
    //         move.is_capture_ = true;
    //     }
    //
    //     // check for castling
    //     if (moving_piece.piece_type_ == chess::piece::KING) {
    //         bool is_king_side_castle_black = board.castling_rights_ & 1U;
    //         bool is_queen_side_castle_black = board.castling_rights_ & 2U;
    //         bool is_king_side_castle_white = board.castling_rights_ & 4U;
    //         bool is_queen_side_castle_white = board.castling_rights_ & 8U;
    //
    //
    //         // we can skip checking piece type on the square since castling rights tell us if the king/rook has moved
    //         if (target_square.square_ == chess::square::G8 && is_king_side_castle_black ) {
    //             move.is_castling_ = true;
    //         }
    //         if (target_square.square_ == chess::square::C8 && is_queen_side_castle_black) {
    //             move.is_castling_ = true;
    //         }
    //         if (target_square.square_ == chess::square::G1 && is_king_side_castle_white) {
    //             move.is_castling_ = true;
    //         }
    //         if (target_square.square_ == chess::square::C1 && is_queen_side_castle_white) {
    //             move.is_castling_ = true;
    //         }
    //     }
    //
    //     return move;
    // }

    Move set_move_flags(Move &move, Board &board) {
        Square starting_square = Square(move.starting_square_.square_);
        Square target_square = Square(move.target_square_.square_);
        u8 moving_piece_type = board.board_[starting_square.square_].piece_type_;
        bool moving_piece_color = board.board_[starting_square.square_].piece_color_;
        u8 target_piece_type = board.board_[target_square.square_].piece_type_;
        bool target_piece_color = board.board_[target_square.square_].piece_color_;

        // doing this because the assignment operator was not copying the whole object properly
        Piece moving_piece = Piece(moving_piece_type, moving_piece_color);
        Piece target_piece = Piece(target_piece_type, target_piece_color);



        // check for captures
        if (target_piece.piece_type_ != chess::piece::EMPTY && target_piece.piece_color_ != moving_piece.piece_color_) {
            move.is_capture_ = true;
        }

        // check for enpassant
        if (moving_piece.piece_type_ == chess::piece::PAWN) {
            if (target_square.square_ == board.enpassant_target_.square_) {
                move.is_en_passant_ = true;
                move.is_capture_ = true;
            }
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
            else if (target_square.square_ == chess::square::C8 && is_queen_side_castle_black) {
                move.is_castling_ = true;
            }
            else if (target_square.square_ == chess::square::G1 && is_king_side_castle_white) {
                move.is_castling_ = true;
            }
            else if (target_square.square_ == chess::square::C1 && is_queen_side_castle_white) {
                move.is_castling_ = true;
            }
        }

        return move;
    }

    bool is_double_pawn_push(Move &move, Board &board) {
        u8 moving_piece_type = board.board_[move.starting_square_.square_].piece_type_;
        bool moving_piece_color = board.board_[move.starting_square_.square_].piece_color_;
        Piece moving_piece = Piece(moving_piece_type, moving_piece_color);

        if (moving_piece.piece_type_ != chess::piece::PAWN) {
            return false;
        }
        if ((move.starting_square_.rank_ == 1 || move.starting_square_.rank_ == 6) && abs(move.target_square_.rank_ - move.starting_square_.rank_) == 2) {
            return true;
        }
        return false;
    }

}

namespace PieceListUtils {
    void add_piece_to_piece_list(Piece piece, Square square, std::vector<std::vector<Square>> &piece_lists,int *piece_index_board, std::vector<int> &piece_counts) {
        int piece_list_type = get_piece_list_type(piece);
        int count = piece_counts[piece_list_type];
        if (count >= piece_lists[piece_list_type].size()) {
            std::cerr << "ERROR: Too many pieces of type " << piece_list_type << "!\n";
            std::exit(EXIT_FAILURE);
        }
        piece_lists[piece_list_type][count] = Square(square.square_);
        piece_index_board[square.square_] = count;
        piece_counts[piece_list_type]++;
    }

    void remove_piece_from_piece_list(Piece piece, Square square, std::vector<std::vector<Square>> &piece_lists, int *piece_index_board, std::vector<int> &piece_counts) {
        int piece_list_type = get_piece_list_type(piece);
        if (piece_counts[piece_list_type] <= 0) {
            std::cerr << "ERROR: Removing piece from empty list for type " << piece_list_type << "\n";
            std::exit(EXIT_FAILURE);
        }
        int piece_count = --piece_counts[piece_list_type];

        int index = piece_index_board[square.square_];
        // some thing is fucking causing the index to be -1
        // some move is fukced up and removing a piece which doesn't exist
        // if (index < 0 || index > piece_count) {
        //     std::cerr << "Invalid index: " << index
        //               << " at square: " << square.square_
        //               << " in list: " << piece_list_type << "\n";
        //     std::exit(EXIT_FAILURE);
        // }

        Square last_square = piece_lists[piece_list_type][piece_count];
        if (index != piece_count) {
            piece_lists[piece_list_type][index] = last_square;
            piece_index_board[last_square.square_] = index;
        }
        piece_index_board[square.square_] = -1;
    }

    void update_piece_list(Piece piece, Square starting_square, Square target_square, std::vector<std::vector<Square>> &piece_lists, int *piece_index_board) {
        int piece_list_type = get_piece_list_type(piece);
        int index = piece_index_board[starting_square.square_];
        piece_lists[piece_list_type][index] = Square(target_square.square_);
        piece_index_board[target_square.square_] = index;
        piece_index_board[starting_square.square_] = -1;
    }

    int get_piece_list_type(Piece &piece) {
        return (int)piece.piece_type_ - 1 + (piece.piece_color_ == chess::color::WHITE ? 0 : 6);
    }

}

namespace BoardUtils {

    bool is_square_attacked(Board &board, Square square, bool attacking_color) {
        // check for pawns
        int pawn_attack_offset[4] = {9, 7, -9, -7};
        if (square.file_ == chess::file::A) {
            pawn_attack_offset[0] = pawn_attack_offset[2] = 0;
        }else if (square.file_ == chess::file::H) {
            pawn_attack_offset[1] = pawn_attack_offset[3] = 0;
        }

        for (int i=(attacking_color==chess::color::BLACK? 0: 2); i<(attacking_color==chess::color::BLACK? 2: 4); i++) {
            if (!pawn_attack_offset[i]) continue;
            Piece attacking_piece = board.board_[square.square_ + pawn_attack_offset[i]];
            if (attacking_piece.piece_type_ == chess::piece::PAWN && attacking_piece.piece_color_ == attacking_color) {
                return true;
            }
        }


        // check for knight
        int knight_offsets[8] = {6, -10, 15, -17, 17, -15, 10, -6};
        int i = 0, n=8;
        if (square.file_ == chess::file::B) {
            i += 2;
        }else if (square.file_ == chess::file::A) {
            i += 4;
        }
        if (square.file_ == chess::file::G) {
            n-= 2;
        }else if (square.file_ == chess::file::H) {
            n-= 4;
        }
        for (int j=i; j<n; j++) {
            if (!knight_offsets[j]) continue;
            if (square.square_ + knight_offsets[j] >= 64 || square.square_ + knight_offsets[j] < 0) continue;
            Piece attacking_piece = board.board_[square.square_ + knight_offsets[j]];
            if (attacking_piece.piece_type_ == chess::piece::KNIGHT && attacking_piece.piece_color_ == attacking_color) {
                return true;
            }
        }

        // check for king
        const int king_offsets[8] = {
            movegen::direction_offset::NORTH_WEST,
            movegen::direction_offset::WEST,
            movegen::direction_offset::SOUTH_WEST,
            movegen::direction_offset::NORTH,
            movegen::direction_offset::SOUTH,
            movegen::direction_offset::NORTH_EAST,
            movegen::direction_offset::EAST,
            movegen::direction_offset::SOUTH_EAST
        };

        i = 0, n=8;
        if (square.file_ == chess::file::A) i += 3;
        if (square.file_ == chess::file::H) n -= 3;
        for (int j=i; j<n; j++) {
            if (!king_offsets[j]) continue;
            if (square.square_ + king_offsets[j] >= 64 || square.square_ + king_offsets[j] < 0) continue;
            Piece attacking_piece = board.board_[square.square_ + king_offsets[j]];
            if (attacking_piece.piece_type_ == chess::piece::KING && attacking_piece.piece_color_ == attacking_color) {
                return true;
            }
        }

        // check for sliding pieces
        const int sliding_offsets[8] = {
            movegen::direction_offset::NORTH,
            movegen::direction_offset::WEST,
            movegen::direction_offset::SOUTH,
            movegen::direction_offset::EAST,
            movegen::direction_offset::NORTH_WEST,
            movegen::direction_offset::SOUTH_WEST,
            movegen::direction_offset::SOUTH_EAST,
            movegen::direction_offset::NORTH_EAST
        };

        int min_distance[8] = {
            7 - square.rank_,
            square.file_,
            square.rank_,
            7 - square.file_,
            std::min(7 - square.rank_, square.file_),
            std::min(square.rank_, square.file_),
            std::min(square.rank_, 7 - square.file_),
            std::min(7 - square.rank_, 7 - square.file_)
        };

        for (int j=0; j<8; j++) {
            u8 secondary_piece_type = (j < 4? chess::piece::ROOK: chess::piece::BISHOP);

            for (int k=1; k<=min_distance[j]; k++) {
                Piece attacking_piece = board.board_[square.square_ + k*sliding_offsets[j]];

                // if blocked by friendly piece no need to check the remaining
                // if blocked by attacking side piece

                bool is_secondary = attacking_piece.piece_type_ == secondary_piece_type;
                bool is_queen = attacking_piece.piece_type_ == chess::piece::QUEEN;
                // enemy piece blocks
                if (attacking_piece.piece_type_ != chess::piece::EMPTY && !(is_secondary || is_queen)) break;
                // friendly piece blocks
                if (attacking_piece.piece_type_ != chess::piece::EMPTY && attacking_piece.piece_color_ != attacking_color) break;
                if ((is_secondary || is_queen) && attacking_piece.piece_color_ == attacking_color) {
                    return true;
                }
            }
        }

        // if not attacked by anything
        return false;

    }

}
