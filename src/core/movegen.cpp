#include <iostream>
#include <movegen.h>

#include "utils.h"

// might add some use later
MoveGenerator::MoveGenerator() = default;

void MoveGenerator::generate_legal_moves(Board &board, MoveList &list) {
    using namespace chess;
    list.clear();

    int us = board.turn_;
    int them = (us == color::WHITE) ? color::BLACK : color::WHITE;

    u64 friendly_bb = (us == color::WHITE) ? board.occupancy_white_ : board.occupancy_black_;
    u64 enemy_bb = (them == color::WHITE) ? board.occupancy_white_ : board.occupancy_black_;
    u64 occupancy = friendly_bb | enemy_bb;

    int king_sq = __builtin_ctzll(board.piece_bitboard_[us == color::WHITE ? piecelists::WHITE_KING : piecelists::BLACK_KING]);

    // 1. CALCULATE CHECKERS
    u64 checkers = 0;
    u64 enemy_pawns = board.piece_bitboard_[them == color::WHITE ? piecelists::WHITE_PAWN : piecelists::BLACK_PAWN];
    u64 enemy_knights = board.piece_bitboard_[them == color::WHITE ? piecelists::WHITE_KNIGHT : piecelists::BLACK_KNIGHT];
    u64 enemy_bishops = board.piece_bitboard_[them == color::WHITE ? piecelists::WHITE_BISHOP : piecelists::BLACK_BISHOP];
    u64 enemy_rooks = board.piece_bitboard_[them == color::WHITE ? piecelists::WHITE_ROOK : piecelists::BLACK_ROOK];
    u64 enemy_queens = board.piece_bitboard_[them == color::WHITE ? piecelists::WHITE_QUEEN : piecelists::BLACK_QUEEN];

    checkers |= board.pawn_attacks[us][king_sq] & enemy_pawns;
    checkers |= board.knight_attacks[king_sq] & enemy_knights;
    checkers |= BitboardUtils::get_bishop_attacks(board, king_sq) & (enemy_bishops | enemy_queens);
    checkers |= BitboardUtils::get_rook_attacks(board, king_sq) & (enemy_rooks | enemy_queens);

    int check_count = __builtin_popcountll(checkers);

    // 2. CALCULATE PIN MASKS
    u64 pin_mask[64];
    for (int i = 0; i < 64; ++i) pin_mask[i] = ~0ULL; // Default: unpinned pieces can move anywhere

    // X-Ray trick: Raycast from the king using ONLY enemy pieces as blockers.
    u64 pinner_candidates = (BitboardUtils::get_rook_attacks(board, king_sq, enemy_bb) & (enemy_rooks | enemy_queens)) |
                            (BitboardUtils::get_bishop_attacks(board, king_sq, enemy_bb) & (enemy_bishops | enemy_queens));

    while (pinner_candidates) {
        int sniper_sq = __builtin_ctzll(pinner_candidates);
        u64 between = board.ray_between[king_sq][sniper_sq];

        u64 blockers = between & friendly_bb;
        if (__builtin_popcountll(blockers) == 1) { // Exactly one friendly piece is in the way
            int pinned_sq = __builtin_ctzll(blockers);
            // A pinned piece can only move along the ray between the king and the sniper (or capture the sniper)
            pin_mask[pinned_sq] = between | (1ULL << sniper_sq);
        }
        pinner_candidates &= pinner_candidates - 1;
    }

    // 3. GENERATE KING MOVES
    generate_king_moves(board, king_sq, list);

    if (check_count > 1) return;

    // 4. GENERATE NON-KING MOVES
    u64 check_mask = ~0ULL; // Default: Not in check, can move anywhere

    if (check_count == 1) {
        int checker_sq = __builtin_ctzll(checkers);
        // Can only move to capture the checker, or step in the ray between the checker and the king
        check_mask = board.ray_between[king_sq][checker_sq] | (1ULL << checker_sq);
    }

    generate_knight_moves(board, list, check_mask, pin_mask);
    generate_pawn_moves(board, list, check_mask, pin_mask);
    generate_sliding_piece_moves(board, list, check_mask, pin_mask);
}

bool MoveGenerator::is_in_check(const Board &board, int in_check_color) {
    int king_square = in_check_color == chess::color::WHITE ? board.white_king_square_ : board.black_king_square_;
    int attacking_color = in_check_color == chess::color::WHITE ? chess::color::BLACK : chess::color::WHITE;
    return BoardUtils::is_square_attacked(board, king_square, attacking_color);
}



// void MoveGenerator::generate_all_pseudo_legal_moves(Board &board, MoveList &list) {
//     // 1. Grab the correct bitboards based on whose turn it is
//     u64 pawns, knights, bishops, rooks, queens, kings;
//
//     if (board.turn_ == chess::color::WHITE) {
//         pawns   = board.piece_bitboard_[chess::piecelists::WHITE_PAWN];
//         knights = board.piece_bitboard_[chess::piecelists::WHITE_KNIGHT];
//         bishops = board.piece_bitboard_[chess::piecelists::WHITE_BISHOP];
//         rooks   = board.piece_bitboard_[chess::piecelists::WHITE_ROOK];
//         queens  = board.piece_bitboard_[chess::piecelists::WHITE_QUEEN];
//         kings   = board.piece_bitboard_[chess::piecelists::WHITE_KING];
//     } else {
//         pawns   = board.piece_bitboard_[chess::piecelists::BLACK_PAWN];
//         knights = board.piece_bitboard_[chess::piecelists::BLACK_KNIGHT];
//         bishops = board.piece_bitboard_[chess::piecelists::BLACK_BISHOP];
//         rooks   = board.piece_bitboard_[chess::piecelists::BLACK_ROOK];
//         queens  = board.piece_bitboard_[chess::piecelists::BLACK_QUEEN];
//         kings   = board.piece_bitboard_[chess::piecelists::BLACK_KING];
//     }
//
//     // 2. Iterate through each bitboard using compiler intrinsics
//
//     // Generate Pawn Moves
//     u64 bb = pawns;
//     while (bb) {
//         int sq = __builtin_ctzll(bb); // Hardware instruction: Find index of lowest set bit (0-63)
//         generate_pawn_moves(board, sq, list);
//         bb &= bb - 1;                 // Clear the lowest set bit to move to the next piece
//     }
//
//     // Generate Knight Moves
//     bb = knights;
//     while (bb) {
//         int sq = __builtin_ctzll(bb);
//         generate_knight_moves(board, sq, list);
//         bb &= bb - 1;
//     }
//
//     // Generate Bishop Moves (Sliding)
//     bb = bishops;
//     while (bb) {
//         int sq = __builtin_ctzll(bb);
//         generate_sliding_piece_moves(board, sq, list);
//         bb &= bb - 1;
//     }
//
//     // Generate Rook Moves (Sliding)
//     bb = rooks;
//     while (bb) {
//         int sq = __builtin_ctzll(bb);
//         generate_sliding_piece_moves(board, sq, list);
//         bb &= bb - 1;
//     }
//
//     // Generate Queen Moves (Sliding)
//     bb = queens;
//     while (bb) {
//         int sq = __builtin_ctzll(bb);
//         generate_sliding_piece_moves(board, sq, list);
//         bb &= bb - 1;
//     }
//
//     // Generate King Moves
//     bb = kings;
//     while (bb) {
//         int sq = __builtin_ctzll(bb);
//         generate_king_moves(board, sq, list);
//         bb &= bb - 1;
//     }
// }




// assume the function is called for the correct piece
void MoveGenerator::generate_sliding_piece_moves(const Board &board, MoveList &list, u64 check_mask, const u64 pin_mask[64]) {
    using namespace chess;
    int us = board.turn_;
    u64 friendly_bb = (us == color::WHITE) ? board.occupancy_white_ : board.occupancy_black_;
    u64 enemy_bb = (us == color::WHITE) ? board.occupancy_black_ : board.occupancy_white_;

    u64 bishops = board.piece_bitboard_[us == color::WHITE ? piecelists::WHITE_BISHOP : piecelists::BLACK_BISHOP];
    u64 rooks = board.piece_bitboard_[us == color::WHITE ? piecelists::WHITE_ROOK : piecelists::BLACK_ROOK];
    u64 queens = board.piece_bitboard_[us == color::WHITE ? piecelists::WHITE_QUEEN : piecelists::BLACK_QUEEN];

    u64 diagonal_sliders = bishops | queens;
    while (diagonal_sliders) {
        int sq = __builtin_ctzll(diagonal_sliders);
        // STRICT LEGALITY: Filter by check_mask and pin_mask
        u64 attacks = BitboardUtils::get_bishop_attacks(board, sq) & ~friendly_bb & check_mask & pin_mask[sq];

        while (attacks) {
            int target_square = __builtin_ctzll(attacks);
            bool is_capture = (enemy_bb & (1ULL << target_square)) != 0ULL;
            list.emplace_back(Move(sq, target_square, piece_type::EMPTY, false, is_capture, false));
            attacks &= attacks - 1;
        }
        diagonal_sliders &= diagonal_sliders - 1;
    }

    u64 straight_sliders = rooks | queens;
    while (straight_sliders) {
        int sq = __builtin_ctzll(straight_sliders);
        // STRICT LEGALITY: Filter by check_mask and pin_mask
        u64 attacks = BitboardUtils::get_rook_attacks(board, sq) & ~friendly_bb & check_mask & pin_mask[sq];

        while (attacks) {
            int target_square = __builtin_ctzll(attacks);
            bool is_capture = (enemy_bb & (1ULL << target_square)) != 0ULL;
            list.emplace_back(Move(sq, target_square, piece_type::EMPTY, false, is_capture, false));
            attacks &= attacks - 1;
        }
        straight_sliders &= straight_sliders - 1;
    }
}

void MoveGenerator::generate_knight_moves(const Board &board, MoveList &list, u64 check_mask, const u64 pin_mask[64]) {
    using namespace chess;
    int us = board.turn_;
    u64 friendly_bb = (us == color::WHITE) ? board.occupancy_white_ : board.occupancy_black_;
    u64 enemy_bb = (us == color::WHITE) ? board.occupancy_black_ : board.occupancy_white_;

    u64 knights = board.piece_bitboard_[us == color::WHITE ? piecelists::WHITE_KNIGHT : piecelists::BLACK_KNIGHT];

    while (knights) {
        int sq = __builtin_ctzll(knights);

        // STRICT LEGALITY: AND the attacks with the check_mask and pin_mask
        // (A pinned knight's pin_mask is a straight ray, so `knight_attacks & straight_ray` instantly evaluates to 0!)
        u64 attacks = board.knight_attacks[sq] & ~friendly_bb & check_mask & pin_mask[sq];

        while (attacks) {
            int target_square = __builtin_ctzll(attacks);
            bool is_capture = (enemy_bb & (1ULL << target_square)) != 0ULL;

            list.emplace_back(Move(sq, target_square, piece_type::EMPTY, false, is_capture, false));
            attacks &= attacks - 1;
        }
        knights &= knights - 1;
    }
}

void MoveGenerator::generate_pawn_moves(const Board &board, MoveList &list, u64 check_mask, const u64 pin_mask[64]) {
    using namespace chess;
    int us = board.turn_;
    int them = (us == color::WHITE) ? color::BLACK : color::WHITE;
    u64 friendly_bb = (us == color::WHITE) ? board.occupancy_white_ : board.occupancy_black_;
    u64 enemy_bb = (us == color::WHITE) ? board.occupancy_black_ : board.occupancy_white_;
    u64 occupancy = friendly_bb | enemy_bb;

    u64 pawns = board.piece_bitboard_[us == color::WHITE ? piecelists::WHITE_PAWN : piecelists::BLACK_PAWN];
    int promotion_rank = (us == color::WHITE) ? 7 : 0;

    while (pawns) {
        int sq = __builtin_ctzll(pawns);
        u64 pawn_bb = 1ULL << sq;

        u64 pushes = 0ULL;
        u64 captures = 0ULL;
        u64 ep_capture = 0ULL;

        if (us == color::WHITE) {
            pushes = (pawn_bb << 8) & ~occupancy;
            if (pushes && Square::rank_(sq) == 1) pushes |= (pawn_bb << 16) & ~occupancy;
        } else {
            pushes = (pawn_bb >> 8) & ~occupancy;
            if (pushes && Square::rank_(sq) == 6) pushes |= (pawn_bb >> 16) & ~occupancy;
        }

        captures = board.pawn_attacks[us][sq] & enemy_bb;

        if (board.enpassant_target_ != square::EMPTY) {
            u64 ep_bb = 1ULL << board.enpassant_target_;
            if (board.pawn_attacks[us][sq] & ep_bb) {
                ep_capture = ep_bb;
            }
        }

        // STRICT LEGALITY: Filter standard moves
        u64 valid_moves = (pushes | captures) & check_mask & pin_mask[sq];

        while (valid_moves) {
            int target = __builtin_ctzll(valid_moves);
            bool is_capture = (captures & (1ULL << target)) != 0ULL;

            if (Square::rank_(target) == promotion_rank) {
                int promotion_pieces[] = {piece_type::QUEEN, piece_type::ROOK, piece_type::BISHOP, piece_type::KNIGHT};
                for (int p_type : promotion_pieces) {
                    list.emplace_back(Move(sq, target, Piece::piece_(p_type, us), false, is_capture, false));
                }
            } else {
                list.emplace_back(Move(sq, target, piece_type::EMPTY, false, is_capture, false));
            }
            valid_moves &= valid_moves - 1;
        }

        // EN PASSANT FIX: Simulate the board state after EP to verify safety
        if (ep_capture) {
            int target = board.enpassant_target_;
            int ep_pawn_sq = (us == color::WHITE) ? target - 8 : target + 8;

            // Simulating removing our pawn, removing their pawn, and placing our pawn on the EP square
            u64 occ_after_ep = occupancy ^ (1ULL << sq) ^ (1ULL << target) ^ (1ULL << ep_pawn_sq);

            int king_sq = __builtin_ctzll(board.piece_bitboard_[us == color::WHITE ? piecelists::WHITE_KING : piecelists::BLACK_KING]);
            u64 enemy_rooks = board.piece_bitboard_[them == color::WHITE ? piecelists::WHITE_ROOK : piecelists::BLACK_ROOK];
            u64 enemy_bishops = board.piece_bitboard_[them == color::WHITE ? piecelists::WHITE_BISHOP : piecelists::BLACK_BISHOP];
            u64 enemy_queens = board.piece_bitboard_[them == color::WHITE ? piecelists::WHITE_QUEEN : piecelists::BLACK_QUEEN];

            bool ep_safe = true;
            if (BitboardUtils::get_rook_attacks(board, king_sq, occ_after_ep) & (enemy_rooks | enemy_queens)) ep_safe = false;
            if (BitboardUtils::get_bishop_attacks(board, king_sq, occ_after_ep) & (enemy_bishops | enemy_queens)) ep_safe = false;

            if (ep_safe) {
                list.emplace_back(Move(sq, target, piece_type::EMPTY, false, true, true));
            }
        }
        pawns &= pawns - 1;
    }
}

void MoveGenerator::generate_king_moves(const Board &board, int square, MoveList &list) {
    using namespace chess;
    int us = board.turn_;
    int them = (us == color::WHITE) ? color::BLACK : color::WHITE;
    u64 friendly_bb = (us == color::WHITE) ? board.occupancy_white_ : board.occupancy_black_;
    u64 enemy_bb = (us == color::WHITE) ? board.occupancy_black_ : board.occupancy_white_;
    u64 occupancy = friendly_bb | enemy_bb;

    // THE FIX: Remove the king from the occupancy to properly evaluate X-Ray attacks!
    u64 occ_no_king = occupancy ^ (1ULL << square);

    u64 enemy_pawns = board.piece_bitboard_[them == color::WHITE ? piecelists::WHITE_PAWN : piecelists::BLACK_PAWN];
    u64 enemy_knights = board.piece_bitboard_[them == color::WHITE ? piecelists::WHITE_KNIGHT : piecelists::BLACK_KNIGHT];
    u64 enemy_bishops = board.piece_bitboard_[them == color::WHITE ? piecelists::WHITE_BISHOP : piecelists::BLACK_BISHOP];
    u64 enemy_rooks = board.piece_bitboard_[them == color::WHITE ? piecelists::WHITE_ROOK : piecelists::BLACK_ROOK];
    u64 enemy_queens = board.piece_bitboard_[them == color::WHITE ? piecelists::WHITE_QUEEN : piecelists::BLACK_QUEEN];
    u64 enemy_king = board.piece_bitboard_[them == color::WHITE ? piecelists::WHITE_KING : piecelists::BLACK_KING];

    // Ultra-fast local lambda to check if a square is attacked
    auto is_square_safe = [&](int sq) {
        if (board.pawn_attacks[us][sq] & enemy_pawns) return false;
        if (board.knight_attacks[sq] & enemy_knights) return false;
        if (board.king_attacks[sq] & enemy_king) return false;
        if (BitboardUtils::get_bishop_attacks(board, sq, occ_no_king) & (enemy_bishops | enemy_queens)) return false;
        if (BitboardUtils::get_rook_attacks(board, sq, occ_no_king) & (enemy_rooks | enemy_queens)) return false;
        return true;
    };

    // 1. Normal King Moves
    u64 attack_mask = board.king_attacks[square] & ~friendly_bb;
    while (attack_mask) {
        int target = __builtin_ctzll(attack_mask);
        if (is_square_safe(target)) {
            bool is_capture = (enemy_bb & (1ULL << target)) != 0ULL;
            list.emplace_back(Move(square, target, piece_type::EMPTY, false, is_capture, false));
        }
        attack_mask &= attack_mask - 1;
    }

    // 2. Castling Logic
    int kingside_bitmask = (us == color::WHITE) ? bitmask::castling::WHITE_KING : bitmask::castling::BLACK_KING;
    int queenside_bitmask = (us == color::WHITE) ? bitmask::castling::WHITE_QUEEN : bitmask::castling::BLACK_QUEEN;

    bool is_kingside = (board.castling_rights_ & kingside_bitmask);
    bool is_queenside = (board.castling_rights_ & queenside_bitmask);
    int rank = (us == color::WHITE) ? 0 : 7;

    bool king_in_check = !is_square_safe(square);

    if (is_kingside && !king_in_check) {
        int f = Square::square_(file::F, rank);
        int g = Square::square_(file::G, rank);
        u64 mask = (1ULL << f) | (1ULL << g);
        if ((occupancy & mask) == 0ULL) {
            if (is_square_safe(f) && is_square_safe(g)) {
                list.emplace_back(Move(square, g, piece_type::EMPTY, true, false, false));
            }
        }
    }

    if (is_queenside && !king_in_check) {
        int b = Square::square_(file::B, rank);
        int c = Square::square_(file::C, rank);
        int d = Square::square_(file::D, rank);
        u64 mask = (1ULL << b) | (1ULL << c) | (1ULL << d);
        if ((occupancy & mask) == 0ULL) {
            if (is_square_safe(c) && is_square_safe(d)) {
                list.emplace_back(Move(square, c, piece_type::EMPTY, true, false, false));
            }
        }
    }
}

void MoveGenerator::generate_captures(Board &board, MoveList &list) {
    using namespace chess;
    list.clear();

    int us = board.turn_;
    int them = (us == color::WHITE) ? color::BLACK : color::WHITE;

    u64 friendly_bb = (us == color::WHITE) ? board.occupancy_white_ : board.occupancy_black_;
    u64 enemy_bb = (them == color::WHITE) ? board.occupancy_white_ : board.occupancy_black_;

    int king_sq = __builtin_ctzll(board.piece_bitboard_[us == color::WHITE ? piecelists::WHITE_KING : piecelists::BLACK_KING]);

    // 1. CALCULATE CHECKERS
    u64 checkers = 0;
    u64 enemy_pawns = board.piece_bitboard_[them == color::WHITE ? piecelists::WHITE_PAWN : piecelists::BLACK_PAWN];
    u64 enemy_knights = board.piece_bitboard_[them == color::WHITE ? piecelists::WHITE_KNIGHT : piecelists::BLACK_KNIGHT];
    u64 enemy_bishops = board.piece_bitboard_[them == color::WHITE ? piecelists::WHITE_BISHOP : piecelists::BLACK_BISHOP];
    u64 enemy_rooks = board.piece_bitboard_[them == color::WHITE ? piecelists::WHITE_ROOK : piecelists::BLACK_ROOK];
    u64 enemy_queens = board.piece_bitboard_[them == color::WHITE ? piecelists::WHITE_QUEEN : piecelists::BLACK_QUEEN];

    checkers |= board.pawn_attacks[us][king_sq] & enemy_pawns;
    checkers |= board.knight_attacks[king_sq] & enemy_knights;
    checkers |= BitboardUtils::get_bishop_attacks(board, king_sq) & (enemy_bishops | enemy_queens);
    checkers |= BitboardUtils::get_rook_attacks(board, king_sq) & (enemy_rooks | enemy_queens);

    int check_count = __builtin_popcountll(checkers);

    // 2. CALCULATE PIN MASKS
    u64 pin_mask[64];
    for (int i = 0; i < 64; ++i) pin_mask[i] = ~0ULL;

    u64 pinner_candidates = (BitboardUtils::get_rook_attacks(board, king_sq, enemy_bb) & (enemy_rooks | enemy_queens)) |
                            (BitboardUtils::get_bishop_attacks(board, king_sq, enemy_bb) & (enemy_bishops | enemy_queens));

    while (pinner_candidates) {
        int sniper_sq = __builtin_ctzll(pinner_candidates);
        u64 between = board.ray_between[king_sq][sniper_sq];

        u64 blockers = between & friendly_bb;
        if (__builtin_popcountll(blockers) == 1) {
            int pinned_sq = __builtin_ctzll(blockers);
            pin_mask[pinned_sq] = between | (1ULL << sniper_sq);
        }
        pinner_candidates &= pinner_candidates - 1;
    }

    // 3. GENERATE KING CAPTURES
    generate_king_captures(board, king_sq, list);

    if (check_count > 1) return; // Double check: Only king moves are legal

    // 4. GENERATE NON-KING CAPTURES
    u64 check_mask = ~0ULL;

    if (check_count == 1) {
        int checker_sq = __builtin_ctzll(checkers);
        // Can only move to capture the checker, or step in the ray (but steps aren't captures!)
        // In a capture generator, we only care about capturing the checker.
        check_mask = board.ray_between[king_sq][checker_sq] | (1ULL << checker_sq);
    }

    generate_knight_captures(board, list, check_mask, pin_mask);
    generate_pawn_captures(board, list, check_mask, pin_mask);
    generate_sliding_piece_captures(board, list, check_mask, pin_mask);
}

void MoveGenerator::generate_sliding_piece_captures(const Board &board, MoveList &list, u64 check_mask, const u64 pin_mask[64]) {
    using namespace chess;
    int us = board.turn_;
    u64 enemy_bb = (us == color::WHITE) ? board.occupancy_black_ : board.occupancy_white_;

    u64 bishops = board.piece_bitboard_[us == color::WHITE ? piecelists::WHITE_BISHOP : piecelists::BLACK_BISHOP];
    u64 rooks = board.piece_bitboard_[us == color::WHITE ? piecelists::WHITE_ROOK : piecelists::BLACK_ROOK];
    u64 queens = board.piece_bitboard_[us == color::WHITE ? piecelists::WHITE_QUEEN : piecelists::BLACK_QUEEN];

    u64 diagonal_sliders = bishops | queens;
    while (diagonal_sliders) {
        int sq = __builtin_ctzll(diagonal_sliders);
        // STRICT LEGALITY + ENEMY ONLY MASK
        u64 attacks = BitboardUtils::get_bishop_attacks(board, sq) & enemy_bb & check_mask & pin_mask[sq];

        while (attacks) {
            int target_square = __builtin_ctzll(attacks);
            // We know 100% this is a capture, no need to check
            list.emplace_back(Move(sq, target_square, piece_type::EMPTY, false, true, false));
            attacks &= attacks - 1;
        }
        diagonal_sliders &= diagonal_sliders - 1;
    }

    u64 straight_sliders = rooks | queens;
    while (straight_sliders) {
        int sq = __builtin_ctzll(straight_sliders);
        u64 attacks = BitboardUtils::get_rook_attacks(board, sq) & enemy_bb & check_mask & pin_mask[sq];

        while (attacks) {
            int target_square = __builtin_ctzll(attacks);
            list.emplace_back(Move(sq, target_square, piece_type::EMPTY, false, true, false));
            attacks &= attacks - 1;
        }
        straight_sliders &= straight_sliders - 1;
    }
}

void MoveGenerator::generate_knight_captures(const Board &board, MoveList &list, u64 check_mask, const u64 pin_mask[64]) {
    using namespace chess;
    int us = board.turn_;
    u64 enemy_bb = (us == color::WHITE) ? board.occupancy_black_ : board.occupancy_white_;
    u64 knights = board.piece_bitboard_[us == color::WHITE ? piecelists::WHITE_KNIGHT : piecelists::BLACK_KNIGHT];

    while (knights) {
        int sq = __builtin_ctzll(knights);
        u64 attacks = board.knight_attacks[sq] & enemy_bb & check_mask & pin_mask[sq];

        while (attacks) {
            int target_square = __builtin_ctzll(attacks);
            list.emplace_back(Move(sq, target_square, piece_type::EMPTY, false, true, false));
            attacks &= attacks - 1;
        }
        knights &= knights - 1;
    }
}

void MoveGenerator::generate_king_captures(const Board &board, int square, MoveList &list) {
    using namespace chess;
    int us = board.turn_;
    int them = (us == color::WHITE) ? color::BLACK : color::WHITE;
    u64 enemy_bb = (us == color::WHITE) ? board.occupancy_black_ : board.occupancy_white_;
    u64 occupancy = board.occupancy_white_ | board.occupancy_black_;

    u64 occ_no_king = occupancy ^ (1ULL << square);

    u64 enemy_pawns = board.piece_bitboard_[them == color::WHITE ? piecelists::WHITE_PAWN : piecelists::BLACK_PAWN];
    u64 enemy_knights = board.piece_bitboard_[them == color::WHITE ? piecelists::WHITE_KNIGHT : piecelists::BLACK_KNIGHT];
    u64 enemy_bishops = board.piece_bitboard_[them == color::WHITE ? piecelists::WHITE_BISHOP : piecelists::BLACK_BISHOP];
    u64 enemy_rooks = board.piece_bitboard_[them == color::WHITE ? piecelists::WHITE_ROOK : piecelists::BLACK_ROOK];
    u64 enemy_queens = board.piece_bitboard_[them == color::WHITE ? piecelists::WHITE_QUEEN : piecelists::BLACK_QUEEN];
    u64 enemy_king = board.piece_bitboard_[them == color::WHITE ? piecelists::WHITE_KING : piecelists::BLACK_KING];

    auto is_square_safe = [&](int sq) {
        if (board.pawn_attacks[us][sq] & enemy_pawns) return false;
        if (board.knight_attacks[sq] & enemy_knights) return false;
        if (board.king_attacks[sq] & enemy_king) return false;
        if (BitboardUtils::get_bishop_attacks(board, sq, occ_no_king) & (enemy_bishops | enemy_queens)) return false;
        if (BitboardUtils::get_rook_attacks(board, sq, occ_no_king) & (enemy_rooks | enemy_queens)) return false;
        return true;
    };

    // Castling is completely removed since it's never a capture
    u64 attack_mask = board.king_attacks[square] & enemy_bb;
    while (attack_mask) {
        int target = __builtin_ctzll(attack_mask);
        if (is_square_safe(target)) {
            list.emplace_back(Move(square, target, piece_type::EMPTY, false, true, false));
        }
        attack_mask &= attack_mask - 1;
    }
}

void MoveGenerator::generate_pawn_captures(const Board &board, MoveList &list, u64 check_mask, const u64 pin_mask[64]) {
    using namespace chess;
    int us = board.turn_;
    int them = (us == color::WHITE) ? color::BLACK : color::WHITE;
    u64 enemy_bb = (us == color::WHITE) ? board.occupancy_black_ : board.occupancy_white_;
    u64 occupancy = board.occupancy_white_ | board.occupancy_black_;

    u64 pawns = board.piece_bitboard_[us == color::WHITE ? piecelists::WHITE_PAWN : piecelists::BLACK_PAWN];
    int promotion_rank = (us == color::WHITE) ? 7 : 0;

    while (pawns) {
        int sq = __builtin_ctzll(pawns);
        u64 pawn_bb = 1ULL << sq;

        u64 pushes = 0ULL;
        u64 captures = 0ULL;
        u64 ep_capture = 0ULL;

        // ONLY generate pushes if they result in a promotion
        if (us == color::WHITE) {
            if (Square::rank_(sq) == 6) pushes = (pawn_bb << 8) & ~occupancy;
        } else {
            if (Square::rank_(sq) == 1) pushes = (pawn_bb >> 8) & ~occupancy;
        }

        captures = board.pawn_attacks[us][sq] & enemy_bb;

        if (board.enpassant_target_ != square::EMPTY) {
            u64 ep_bb = 1ULL << board.enpassant_target_;
            if (board.pawn_attacks[us][sq] & ep_bb) ep_capture = ep_bb;
        }

        u64 valid_moves = (pushes | captures) & check_mask & pin_mask[sq];

        while (valid_moves) {
            int target = __builtin_ctzll(valid_moves);
            bool is_capture = (captures & (1ULL << target)) != 0ULL;

            if (Square::rank_(target) == promotion_rank) {
                // In QS, we generally only test Queen promotions
                list.emplace_back(Move(sq, target, piece_type::QUEEN, false, is_capture, false));
            } else {
                list.emplace_back(Move(sq, target, piece_type::EMPTY, false, true, false));
            }
            valid_moves &= valid_moves - 1;
        }

        if (ep_capture) {
            int target = board.enpassant_target_;
            int ep_pawn_sq = (us == color::WHITE) ? target - 8 : target + 8;
            u64 occ_after_ep = occupancy ^ (1ULL << sq) ^ (1ULL << target) ^ (1ULL << ep_pawn_sq);

            int king_sq = __builtin_ctzll(board.piece_bitboard_[us == color::WHITE ? piecelists::WHITE_KING : piecelists::BLACK_KING]);
            u64 enemy_rooks = board.piece_bitboard_[them == color::WHITE ? piecelists::WHITE_ROOK : piecelists::BLACK_ROOK];
            u64 enemy_bishops = board.piece_bitboard_[them == color::WHITE ? piecelists::WHITE_BISHOP : piecelists::BLACK_BISHOP];
            u64 enemy_queens = board.piece_bitboard_[them == color::WHITE ? piecelists::WHITE_QUEEN : piecelists::BLACK_QUEEN];

            bool ep_safe = true;
            if (BitboardUtils::get_rook_attacks(board, king_sq, occ_after_ep) & (enemy_rooks | enemy_queens)) ep_safe = false;
            if (BitboardUtils::get_bishop_attacks(board, king_sq, occ_after_ep) & (enemy_bishops | enemy_queens)) ep_safe = false;

            if (ep_safe) {
                list.emplace_back(Move(sq, target, piece_type::EMPTY, false, true, true));
            }
        }
        pawns &= pawns - 1;
    }
}