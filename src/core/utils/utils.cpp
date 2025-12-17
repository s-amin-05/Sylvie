#include <cassert>
#include <iostream>
#include <sstream>
#include <utils.h>

#include "eval.h"

namespace Utils {
    std::vector<std::string> split(const std::string &s, const char delim) {
        std::stringstream ss(s);
        std::string token;
        std::vector<std::string> string_parts;
        while (std::getline(ss, token, delim)) {
            string_parts.push_back(token);
        }

        return string_parts;
    }

    std::string trim(const std::string &s) {
        size_t start = 0;
        while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start])))
            start++;

        size_t end = s.size();
        while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1])))
            end--;

        return s.substr(start, end - start);
    }

    std::string get_fen_from_args(std::vector<std::string> &args) {
        std::string fen;

        auto it = std::ranges::find(args, "fen");
        if (it == args.end())
            return "";

        ++it;

        for (; it != args.end(); ++it) {
            if (*it == "moves")
                break;

            if (!fen.empty())
                fen += " ";

            fen += *it;
        }
        return fen;
    }

    std::vector<std::string> get_moves_from_args(std::vector<std::string> &args) {
        std::vector<std::string> moves;

        auto it = std::ranges::find(args, "moves");
        if (it == args.end())
            return moves;

        ++it;

        for (; it != args.end(); ++it) {
            moves.push_back(*it);
        }
        return moves;
    }

    std::string get_arg_after_keyword(std::vector<std::string> &args, std::string keyword) {
        auto it = std::ranges::find(args, keyword);
        if (it == args.end())
            return "";

        ++it;
        return *it;
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

    Move set_move_flags(Move &move, const Board &board) {
        const int starting_square = move.starting_square_;
        const int target_square = move.target_square_;
        const int moving_piece = board.board_[starting_square];
        const int target_piece = board.board_[target_square];
        const int moving_piece_type = Piece::type_(moving_piece);
        const int moving_piece_color = Piece::color_(moving_piece);
        const int target_piece_type = Piece::type_(target_piece);
        const int target_piece_color = Piece::color_(target_piece);

        // // doing this because the assignment operator was not copying the whole object properly
        // const int moving_piece = Piece(moving_piece_type, moving_piece_color);
        // const int target_piece = Piece(target_piece_type, target_piece_color);



        // check for captures
        if (target_piece_type != chess::piece_type::EMPTY && target_piece_color != moving_piece_color) {
            move.is_capture_ = true;
        }

        // check for enpassant
        if (moving_piece_type == chess::piece_type::PAWN) {
            if (target_square == board.enpassant_target_) {
                move.is_en_passant_ = true;
                move.is_capture_ = true;
            }
        }

        // check for castling
        if (moving_piece_type == chess::piece_type::KING) {
            const bool is_king_side_castle_black = board.castling_rights_ & 1U;
            const bool is_queen_side_castle_black = board.castling_rights_ & 2U;
            const bool is_king_side_castle_white = board.castling_rights_ & 4U;
            const bool is_queen_side_castle_white = board.castling_rights_ & 8U;


            // we can skip checking piece type on the square since castling rights tell us if the king/rook has moved
            if (target_square == chess::square::G8 && is_king_side_castle_black ) {
                move.is_castling_ = true;
            }
            else if (target_square == chess::square::C8 && is_queen_side_castle_black) {
                move.is_castling_ = true;
            }
            else if (target_square == chess::square::G1 && is_king_side_castle_white) {
                move.is_castling_ = true;
            }
            else if (target_square == chess::square::C1 && is_queen_side_castle_white) {
                move.is_castling_ = true;
            }
        }

        return move;
    }

    bool is_double_pawn_push(const Move &move, const Board &board) {
        auto moving_piece = board.board_[move.starting_square_];
        const int moving_piece_type = Piece::type_(moving_piece);
        const int moving_piece_color = Piece::color_(moving_piece);

        if (moving_piece_type != chess::piece_type::PAWN) {
            return false;
        }
        if ((Square::rank_(move.starting_square_) == 1 || Square::rank_(move.starting_square_) == 6) && abs(Square::rank_(move.target_square_) - Square::rank_(move.starting_square_)) == 2) {
            return true;
        }
        return false;
    }

}

// namespace PieceListUtils {
//     void add_piece_to_piece_list(Piece piece, const Square square, std::vector<std::vector<Square>> &piece_lists,int *piece_index_board, std::vector<int> &piece_counts) {
//         const int piece_list_type = get_piece_list_type(piece);
//         const int count = piece_counts[piece_list_type];
//         if (count >= piece_lists[piece_list_type].size()) {
//             std::cerr << "ERROR: Too many pieces of type " << piece_list_type << "!\n";
//             std::exit(EXIT_FAILURE);
//         }
//         piece_lists[piece_list_type][count] = Square(square.square_);
//         piece_index_board[square.square_] = count;
//         piece_counts[piece_list_type]++;
//     }
//
//     void remove_piece_from_piece_list(Piece piece, Square square, std::vector<std::vector<Square>> &piece_lists, int *piece_index_board, std::vector<int> &piece_counts) {
//         const int piece_list_type = get_piece_list_type(piece);
//         if (piece_counts[piece_list_type] <= 0) {
//             std::cerr << "ERROR: Removing piece from empty list for type " << piece_list_type << "\n";
//             std::exit(EXIT_FAILURE);
//         }
//         int piece_count = --piece_counts[piece_list_type];
//
//         int index = piece_index_board[square.square_];
//         // some thing is fucking causing the index to be -1
//         // some move is fukced up and removing a piece which doesn't exist
//         // if (index < 0 || index > piece_count) {
//         //     std::cerr << "Invalid index: " << index
//         //               << " at square: " << square.square_
//         //               << " in list: " << piece_list_type << "\n";
//         //     std::exit(EXIT_FAILURE);
//         // }
//
//         const Square last_square = piece_lists[piece_list_type][piece_count];
//         if (index != piece_count) {
//             piece_lists[piece_list_type][index] = last_square;
//             piece_index_board[last_square.square_] = index;
//         }
//         piece_index_board[square.square_] = -1;
//     }
//
//     void update_piece_list(Piece piece, const Square starting_square, const Square target_square, std::vector<std::vector<Square>> &piece_lists, int *piece_index_board) {
//         const int piece_list_type = get_piece_list_type(piece);
//         int index = piece_index_board[starting_square.square_];
//         piece_lists[piece_list_type][index] = Square(target_square.square_);
//         piece_index_board[target_square.square_] = index;
//         piece_index_board[starting_square.square_] = -1;
//     }
//
//     int get_piece_list_type(const Piece &piece) {
//         return static_cast<int>(piece.piece_type_) - 1 + (piece.piece_color_ == chess::color::WHITE ? 0 : 6);
//     }
//
// }

namespace PieceCountUtils {
    void increment_piece_count(Board &board, int piece, int increment) {
        switch (piece) {
            case chess::piece::WHITE_KING:
                board.piece_count_[chess::piecelists::WHITE_KING] += increment;
                break;
            case chess::piece::BLACK_KING:
                board.piece_count_[chess::piecelists::BLACK_KING] += increment;
                break;
            case chess::piece::WHITE_QUEEN:
                board.piece_count_[chess::piecelists::WHITE_QUEEN] += increment;
                break;
            case chess::piece::BLACK_QUEEN:
                board.piece_count_[chess::piecelists::BLACK_QUEEN] += increment;
                break;
            case chess::piece::WHITE_ROOK:
                board.piece_count_[chess::piecelists::WHITE_ROOK] += increment;
                break;
            case chess::piece::BLACK_ROOK:
                board.piece_count_[chess::piecelists::BLACK_ROOK] += increment;
                break;
            case chess::piece::WHITE_BISHOP:
                board.piece_count_[chess::piecelists::WHITE_BISHOP] += increment;
                break;
            case chess::piece::BLACK_BISHOP:
                board.piece_count_[chess::piecelists::BLACK_BISHOP] += increment;
                break;
            case chess::piece::WHITE_KNIGHT:
                board.piece_count_[chess::piecelists::WHITE_KNIGHT] += increment;
                break;
            case chess::piece::BLACK_KNIGHT:
                board.piece_count_[chess::piecelists::BLACK_KNIGHT] += increment;
                break;
            case chess::piece::WHITE_PAWN:
                board.piece_count_[chess::piecelists::WHITE_PAWN] += increment;
                break;
            case chess::piece::BLACK_PAWN:
                board.piece_count_[chess::piecelists::BLACK_PAWN] += increment;
                break;
            default:
                break;

        }
    }
}

namespace BoardUtils {

    bool is_square_attacked(const Board &board, const int square, const int attacking_color) {
        int file = Square::file_(square);
        int rank = Square::rank_(square);
        // check for pawns
        int pawn_attack_offset[4] = {
            movegen::direction_offset::NORTH_EAST,
            movegen::direction_offset::NORTH_WEST,
            movegen::direction_offset::SOUTH_EAST,
            movegen::direction_offset::SOUTH_WEST
        };
        if (file == chess::file::A) {
            pawn_attack_offset[1] = pawn_attack_offset[3] = 0;
        }else if (file == chess::file::H) {
            pawn_attack_offset[0] = pawn_attack_offset[2] = 0;
        }

        for (int i=(attacking_color==chess::color::BLACK? 0: 2); i<(attacking_color==chess::color::BLACK? 2: 4); i++) {
            if (pawn_attack_offset[i] == 0) continue;
            int attacking_piece = board.board_[square + pawn_attack_offset[i]];
            if (Piece::type_(attacking_piece) == chess::piece_type::PAWN && Piece::color_(attacking_piece) == attacking_color) {
                return true;
            }
        }


        // check for knight
        constexpr int knight_offsets[8] = {6, -10, 15, -17, 17, -15, 10, -6};
        int i = 0, n=8;
        if (file == chess::file::B) {
            i += 2;
        }else if (file == chess::file::A) {
            i += 4;
        }
        if (file == chess::file::G) {
            n-= 2;
        }else if (file == chess::file::H) {
            n-= 4;
        }
        for (int j=i; j<n; j++) {
            if (!knight_offsets[j]) continue;
            if (square + knight_offsets[j] >= 64 || square + knight_offsets[j] < 0) continue;
            int attacking_piece = board.board_[square + knight_offsets[j]];
            if (Piece::type_(attacking_piece) == chess::piece_type::KNIGHT && Piece::color_(attacking_piece) == attacking_color) {
                return true;
            }
        }

        // check for king
        constexpr int king_offsets[8] = {
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
        if (file == chess::file::A) i += 3;
        if (file == chess::file::H) n -= 3;
        for (int j=i; j<n; j++) {
            if (!king_offsets[j]) continue;
            if (square + king_offsets[j] >= 64 || square + king_offsets[j] < 0) continue;
            int attacking_piece = board.board_[square + king_offsets[j]];
            if (Piece::type_(attacking_piece) == chess::piece_type::KING && Piece::color_(attacking_piece) == attacking_color) {
                return true;
            }
        }

        // check for sliding pieces
        constexpr int sliding_offsets[8] = {
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
            7 - rank,
            file,
            rank,
            7 - file,
            std::min(7 - rank, file),
            std::min(rank, file),
            std::min(rank, 7 - file),
            std::min(7 - rank, 7 - file)
        };

        for (int j=0; j<8; j++) {
            const int secondary_piece_type = (j < 4? chess::piece_type::ROOK: chess::piece_type::BISHOP);

            for (int k=1; k<=min_distance[j]; k++) {
                const int attacking_piece = board.board_[square + k*sliding_offsets[j]];
                const int attacking_piece_type = Piece::type_(attacking_piece);
                // if blocked by friendly piece no need to check the remaining
                // if blocked by attacking side piece

                const bool is_secondary = attacking_piece_type == secondary_piece_type;
                const bool is_queen = attacking_piece_type == chess::piece_type::QUEEN;
                // enemy piece blocks
                if (attacking_piece_type != chess::piece_type::EMPTY && !(is_secondary || is_queen)) break;
                // friendly piece blocks
                if (attacking_piece_type != chess::piece_type::EMPTY && Piece::color_(attacking_piece) != attacking_color) break;
                if ((is_secondary || is_queen) && Piece::color_(attacking_piece) == attacking_color) {
                    return true;
                }
            }
        }

        // if not attacked by anything
        return false;

    }

}


namespace SearchUtils {
    int score_mvv_lva(const Move& move, const Board& board) {

        if (!move.is_capture_) {
            return 0;
        }

        int victim_value = 0;
        int endgame_phase = Evaluation::get_endgame_phase(board);
        if (move.is_en_passant_) {
            victim_value = chess::evaluation::PAWN;
        } else {
            int victim_square = board.board_[move.target_square_];
            int victim_type = Piece::type_(victim_square);
            int victim_color = Piece::color_(victim_square);
            victim_value = Evaluation::get_piece_material_value(victim_type) + Evaluation::get_position_bonus(victim_type, move.target_square_, victim_color, endgame_phase);
        }
        int aggressor_square = board.board_[move.starting_square_];
        int aggressor_type = Piece::type_(aggressor_square);
        int aggressor_color = Piece::color_(aggressor_square);
        int aggressor_value = Evaluation::get_piece_material_value(aggressor_type) + Evaluation::get_position_bonus(aggressor_type, move.starting_square_, aggressor_color, endgame_phase);

        return 100000 + (victim_value * 1000) - aggressor_value;
    }

    void order_moves(std::vector<Move> &move_list, const Board &board) {
        std::sort(move_list.begin(), move_list.end(), [&board](const Move &a, const Move &b) {
            return score_mvv_lva(a, board) > score_mvv_lva(b, board);
        });
    }
}
