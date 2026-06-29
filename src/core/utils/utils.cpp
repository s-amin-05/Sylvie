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


namespace PieceListUtils {
    void add_piece_to_piece_list(int piece, const int square, int piece_lists[][16],int piece_index_board[], std::vector<int> &piece_counts) {
        const int piece_list_type = get_piece_list_type(piece);
        const int count = piece_counts[piece_list_type];
        // if (count >= piece_lists[piece_list_type].size()) {
        //     std::cerr << "ERROR: Too many pieces of type " << piece_list_type << "!\n";
        //     std::exit(EXIT_FAILURE);
        // }
        piece_lists[piece_list_type][count] = square;
        piece_index_board[square] = count;
        piece_counts[piece_list_type]++;
    }

    void remove_piece_from_piece_list(int piece, int square, int piece_lists[][16], int piece_index_board[], std::vector<int> &piece_counts) {
        const int piece_list_type = get_piece_list_type(piece);
        // if (piece_counts[piece_list_type] <= 0) {
        //     std::cerr << "ERROR: Removing piece from empty list for type " << piece_list_type << "\n";
        //     std::exit(EXIT_FAILURE);
        // }
        int piece_count = --piece_counts[piece_list_type];

        int index = piece_index_board[square];

        const int last_square = piece_lists[piece_list_type][piece_count];
        if (index != piece_count) {
            piece_lists[piece_list_type][index] = last_square;
            piece_index_board[last_square] = index;
        }
        piece_index_board[square] = -1;
    }

    void update_piece_list(int piece, const int starting_square, const int target_square, int piece_lists[][16], int piece_index_board[]) {
        const int piece_list_type = get_piece_list_type(piece);
        int index = piece_index_board[starting_square];
        piece_lists[piece_list_type][index] = target_square;
        piece_index_board[target_square] = index;
        piece_index_board[starting_square] = -1;
    }

    int get_piece_list_type(int piece) {
        return Piece::type_(piece) - 1 + (Piece::color_(piece) == chess::color::WHITE ? 0 : 6);
    }

}

// namespace PieceCountUtils {
//     void increment_piece_count(Board &board, int piece, int increment) {
//         switch (piece) {
//             case chess::piece::WHITE_KING:
//                 board.piece_count_[chess::piecelists::WHITE_KING] += increment;
//                 break;
//             case chess::piece::BLACK_KING:
//                 board.piece_count_[chess::piecelists::BLACK_KING] += increment;
//                 break;
//             case chess::piece::WHITE_QUEEN:
//                 board.piece_count_[chess::piecelists::WHITE_QUEEN] += increment;
//                 break;
//             case chess::piece::BLACK_QUEEN:
//                 board.piece_count_[chess::piecelists::BLACK_QUEEN] += increment;
//                 break;
//             case chess::piece::WHITE_ROOK:
//                 board.piece_count_[chess::piecelists::WHITE_ROOK] += increment;
//                 break;
//             case chess::piece::BLACK_ROOK:
//                 board.piece_count_[chess::piecelists::BLACK_ROOK] += increment;
//                 break;
//             case chess::piece::WHITE_BISHOP:
//                 board.piece_count_[chess::piecelists::WHITE_BISHOP] += increment;
//                 break;
//             case chess::piece::BLACK_BISHOP:
//                 board.piece_count_[chess::piecelists::BLACK_BISHOP] += increment;
//                 break;
//             case chess::piece::WHITE_KNIGHT:
//                 board.piece_count_[chess::piecelists::WHITE_KNIGHT] += increment;
//                 break;
//             case chess::piece::BLACK_KNIGHT:
//                 board.piece_count_[chess::piecelists::BLACK_KNIGHT] += increment;
//                 break;
//             case chess::piece::WHITE_PAWN:
//                 board.piece_count_[chess::piecelists::WHITE_PAWN] += increment;
//                 break;
//             case chess::piece::BLACK_PAWN:
//                 board.piece_count_[chess::piecelists::BLACK_PAWN] += increment;
//                 break;
//             default:
//                 break;
//
//         }
//     }
// }

namespace BoardUtils {

    bool is_square_attacked(const Board &board, const int square, const int attacking_color) {

        int defending_color;

        u64 enemy_pawns;
        u64 enemy_knights;
        u64 enemy_king;
        u64 enemy_bishops;
        u64 enemy_rooks;
        u64 enemy_queens;


        if (attacking_color == chess::color::WHITE) {
            defending_color = chess::color::BLACK;

            enemy_pawns = board.piece_bitboard_[chess::piecelists::WHITE_PAWN];
            enemy_knights = board.piece_bitboard_[chess::piecelists::WHITE_KNIGHT];
            enemy_king = board.piece_bitboard_[chess::piecelists::WHITE_KING];
            enemy_bishops = board.piece_bitboard_[chess::piecelists::WHITE_BISHOP];
            enemy_rooks = board.piece_bitboard_[chess::piecelists::WHITE_ROOK];
            enemy_queens = board.piece_bitboard_[chess::piecelists::WHITE_QUEEN];

        } else {
            defending_color = chess::color::WHITE;

            enemy_pawns = board.piece_bitboard_[chess::piecelists::BLACK_PAWN];
            enemy_knights = board.piece_bitboard_[chess::piecelists::BLACK_KNIGHT];
            enemy_king = board.piece_bitboard_[chess::piecelists::BLACK_KING];
            enemy_bishops = board.piece_bitboard_[chess::piecelists::BLACK_BISHOP];
            enemy_rooks = board.piece_bitboard_[chess::piecelists::BLACK_ROOK];
            enemy_queens = board.piece_bitboard_[chess::piecelists::BLACK_QUEEN];

        }



        if (board.pawn_attacks[defending_color][square] & enemy_pawns) return true;
        if (board.knight_attacks[square] & enemy_knights) return true;
        if (board.king_attacks[square] & enemy_king) return true;

        // bishops, rooks, queens
        u64 bishop_attacks = BitboardUtils::get_bishop_attacks(board, square);
        u64 rook_attacks = BitboardUtils::get_rook_attacks(board, square);

        if (bishop_attacks & (enemy_bishops | enemy_queens)) return true;
        if (rook_attacks & (enemy_rooks | enemy_queens)) return true;

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

namespace BitboardUtils {

    void add_piece_to_bb(Board &board, int piece, int square) {
        int bb_index = PieceListUtils::get_piece_list_type(piece);
        u64 bit = 1ULL << square;
        board.piece_bitboard_[bb_index] |= bit;

        if (Piece::color_(piece) == chess::color::WHITE)
            board.occupancy_white_ |= bit;
        else
            board.occupancy_black_ |= bit;
    }

    void remove_piece_from_bb(Board &board, int piece, int square) {
        int bb_index = PieceListUtils::get_piece_list_type(piece);
        u64 bit = 1ULL << square;
        board.piece_bitboard_[bb_index] &= ~bit;

        if (Piece::color_(piece) == chess::color::WHITE)
            board.occupancy_white_ &= ~bit;
        else
            board.occupancy_black_ &= ~bit;
    }

    void update_piece_in_bb(Board &board, int piece, int starting_square, int target_square) {
        int bb_index = PieceListUtils::get_piece_list_type(piece);
        u64 starting_bit = 1ULL << starting_square;
        u64 target_bit = 1ULL << target_square;
        board.piece_bitboard_[bb_index] ^= starting_bit | target_bit;

        if (Piece::color_(piece) == chess::color::WHITE)
            board.occupancy_white_ ^= starting_bit | target_bit;
        else
            board.occupancy_black_ ^= starting_bit | target_bit;


    }

    void compute_attack_tables(Board &board) {

        for (int sq=0; sq<64; sq++) {
            board.pawn_attacks[chess::color::WHITE][sq] = compute_pawn_attacks(sq, chess::color::WHITE);
            board.pawn_attacks[chess::color::BLACK][sq] = compute_pawn_attacks(sq, chess::color::BLACK);
            board.knight_attacks[sq] = compute_knight_attacks(sq);
            board.king_attacks[sq] = compute_king_attacks(sq);
        }
    }

    u64 compute_pawn_attacks(int square, int color) {
        using namespace movegen::direction_offset;
        u64 attacks = 0;

        u64 bitboard = 1ULL << square;

        if (color == chess::color::WHITE) {
            // Mask the origin square first, THEN shift
            attacks |= (bitboard & ~bitmask::file::A) << NORTH_WEST;
            attacks |= (bitboard & ~bitmask::file::H) << NORTH_EAST;
        } else {
            // south
            attacks |= (bitboard & ~bitmask::file::A) >> -SOUTH_WEST;
            attacks |= (bitboard & ~bitmask::file::H) >> -SOUTH_EAST;
        }

        return attacks;
    }

    u64 compute_knight_attacks(int square) {
        using namespace movegen::knight_offset;
        u64 attacks = 0;

        u64 bitboard = 1ULL << square;

        attacks |= (bitboard & ~bitmask::file::A) << NO_NO_WE;
        attacks |= (bitboard & ~bitmask::file::H) << NO_NO_EA;
        attacks |= (bitboard & ~(bitmask::file::A | bitmask::file::B)) << NO_WE_WE;
        attacks |= (bitboard & ~(bitmask::file::G | bitmask::file::H)) << NO_EA_EA;

        // South
        attacks |= (bitboard & ~bitmask::file::H) >> -SO_SO_EA;
        attacks |= (bitboard & ~bitmask::file::A) >> -SO_SO_WE;
        attacks |= (bitboard & ~(bitmask::file::G | bitmask::file::H)) >> -SO_EA_EA;
        attacks |= (bitboard & ~(bitmask::file::A | bitmask::file::B)) >> -SO_WE_WE;

        return attacks;
    }

    u64 compute_king_attacks(int square) {
        using namespace movegen::direction_offset;
        u64 attacks = 0;

        u64 bitboard = 1ULL << square;

        attacks |= bitboard << NORTH;
        attacks |= (bitboard & ~bitmask::file::A) << NORTH_WEST;
        attacks |= (bitboard & ~bitmask::file::H) << EAST;
        attacks |= (bitboard & ~bitmask::file::H) << NORTH_EAST;

        // South
        attacks |= bitboard >> -SOUTH;
        attacks |= (bitboard & ~bitmask::file::H) >> -SOUTH_EAST;
        attacks |= (bitboard & ~bitmask::file::A) >> -WEST;
        attacks |= (bitboard & ~bitmask::file::A) >> -SOUTH_WEST;

        return attacks;
    }

    void compute_bishop_magic_bitboards(Board &board) {
        for (int sq = 0; sq < 64; sq++) {
            board.bishop_masks[sq] = mask_bishop_attacks(sq);
            u64 mask = board.bishop_masks[sq];

            int bit_count = bitboards::BISHOP_RELEVANT_BITS[sq];
            int shift = 64 - bit_count;

            // The Carry-Rippler trick to iterate over all subsets of the mask
            u64 blockers = 0;
            do {
                // 1. Hash the blocker configuration to get the table index
                int index = (blockers * bitboards::BISHOP_MAGICS[sq]) >> shift;

                // 2. Store the correct attack bitboard at that index
                board.bishop_attack_table[sq][index] = raycast_bishop_attacks(sq, blockers);

                // 3. Move to the next permutation
                blockers = (blockers - mask) & mask;
            } while (blockers);
        }
    }


    u64 mask_bishop_attacks(int square) {
        u64 attacks = 0;
        int tr = Square::rank_(square);
        int tf = Square::file_(square);

        for (int r = tr + 1, f = tf + 1; r <= 6 && f <= 6; r++, f++) attacks |= (1ULL << (r * 8 + f));
        for (int r = tr + 1, f = tf - 1; r <= 6 && f >= 1; r++, f--) attacks |= (1ULL << (r * 8 + f));
        for (int r = tr - 1, f = tf + 1; r >= 1 && f <= 6; r--, f++) attacks |= (1ULL << (r * 8 + f));
        for (int r = tr - 1, f = tf - 1; r >= 1 && f >= 1; r--, f--) attacks |= (1ULL << (r * 8 + f));

        return attacks;
    }

    u64 raycast_bishop_attacks(int square, u64 blockers) {
        u64 attacks = 0;
        int tr = Square::rank_(square);
        int tf = Square::file_(square);

        for (int r = tr + 1, f = tf + 1; r <= 7 && f <= 7; r++, f++) {
            attacks |= (1ULL << (r * 8 + f));
            if (blockers & (1ULL << (r * 8 + f))) break; // Stop if we hit a blocker
        }
        for (int r = tr + 1, f = tf - 1; r <= 7 && f >= 0; r++, f--) {
            attacks |= (1ULL << (r * 8 + f));
            if (blockers & (1ULL << (r * 8 + f))) break;
        }
        for (int r = tr - 1, f = tf + 1; r >= 0 && f <= 7; r--, f++) {
            attacks |= (1ULL << (r * 8 + f));
            if (blockers & (1ULL << (r * 8 + f))) break;
        }
        for (int r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--) {
            attacks |= (1ULL << (r * 8 + f));
            if (blockers & (1ULL << (r * 8 + f))) break;
        }

        return attacks;
    }

    inline u64 get_bishop_attacks(const Board &board, int square) {
        u64 occupancy = board.occupancy_black_ | board.occupancy_white_;
        // 1. Strip away irrelevant pieces (like edges or pieces not on the diagonal)
        u64 blockers = occupancy & board.bishop_masks[square];

        // 2. Multiply by the magic number and shift to get the index
        int index = (blockers * bitboards::BISHOP_MAGICS[square]) >> (64 - bitboards::BISHOP_RELEVANT_BITS[square]);

        // 3. Return the precalculated bitboard
        return board.bishop_attack_table[square][index];
    }

    void compute_rook_magic_bitboards(Board &board) {
        for (int sq = 0; sq < 64; sq++) {
            board.rook_masks[sq] = mask_rook_attacks(sq);
            u64 mask = board.rook_masks[sq];

            int bit_count = bitboards::ROOK_RELEVANT_BITS[sq];
            int shift = 64 - bit_count;

            // The Carry-Rippler trick to iterate over all subsets of the mask
            u64 blockers = 0;
            do {
                // 1. Hash the blocker configuration to get the table index
                int index = (blockers * bitboards::ROOK_MAGICS[sq]) >> shift;

                // 2. Store the correct attack bitboard at that index
                board.rook_attack_table[sq][index] = raycast_rook_attacks(sq, blockers);

                // 3. Move to the next permutation of blockers
                blockers = (blockers - mask) & mask;
            } while (blockers);
        }
    }

    u64 mask_rook_attacks(int square) {
        u64 attacks = 0;
        int tr = Square::rank_(square);
        int tf = Square::file_(square);

        // Raycast North, South, East, West (Stop BEFORE the outer edges: ranks 0/7, files 0/7)
        for (int r = tr + 1; r <= 6; r++) attacks |= (1ULL << (r * 8 + tf)); // North
        for (int r = tr - 1; r >= 1; r--) attacks |= (1ULL << (r * 8 + tf)); // South
        for (int f = tf + 1; f <= 6; f++) attacks |= (1ULL << (tr * 8 + f)); // East
        for (int f = tf - 1; f >= 1; f--) attacks |= (1ULL << (tr * 8 + f)); // West

        return attacks;
    }

    u64 raycast_rook_attacks(int square, u64 blockers) {
        u64 attacks = 0;
        int tr = Square::rank_(square);
        int tf = Square::file_(square);

        // Raycast North, South, East, West (Go ALL the way to the edge this time)
        for (int r = tr + 1; r <= 7; r++) {
            attacks |= (1ULL << (r * 8 + tf));
            if (blockers & (1ULL << (r * 8 + tf))) break; // Stop after hitting blocker
        }
        for (int r = tr - 1; r >= 0; r--) {
            attacks |= (1ULL << (r * 8 + tf));
            if (blockers & (1ULL << (r * 8 + tf))) break;
        }
        for (int f = tf + 1; f <= 7; f++) {
            attacks |= (1ULL << (tr * 8 + f));
            if (blockers & (1ULL << (tr * 8 + f))) break;
        }
        for (int f = tf - 1; f >= 0; f--) {
            attacks |= (1ULL << (tr * 8 + f));
            if (blockers & (1ULL << (tr * 8 + f))) break;
        }

        return attacks;
    }


    inline u64 get_rook_attacks(const Board &board, int square) {
        u64 occupancy = board.occupancy_black_ | board.occupancy_white_;
        // 1. Strip away irrelevant pieces
        u64 blockers = occupancy & board.rook_masks[square];

        // 2. Multiply by the magic number and shift to get the index
        int index = (blockers * bitboards::ROOK_MAGICS[square]) >> (64 - bitboards::ROOK_RELEVANT_BITS[square]);

        // 3. Return the precalculated bitboard
        return board.rook_attack_table[square][index];
    }


    void print_single_bitboard(u64 bb, const std::string &name) {
        std::cout << "\n=== " << name << " ===\n\n";

        for (int rank = 7; rank >= 0; rank--) {
            std::cout << rank + 1 << "  ";

            for (int file = 0; file < 8; file++) {
                int square = rank * 8 + file;

                if (bb & (1ULL << square))
                    std::cout << "1 ";
                else
                    std::cout << ". ";
            }

            std::cout << '\n';
        }

        std::cout << "\n   a b c d e f g h\n";
        std::cout << "\nHex : 0x"
                  << std::hex << std::setw(16) << std::setfill('0') << bb
                  << std::dec << "\n";
    }


} // namespace BitboardUtils
