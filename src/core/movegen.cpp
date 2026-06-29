#include <iostream>
#include <movegen.h>

#include "utils.h"

// might add some use later
MoveGenerator::MoveGenerator() = default;

void MoveGenerator::generate_legal_moves(Board &board, MoveList &list) {

    list.clear();
    generate_all_pseudo_legal_moves(board, list);

    // 2. Filter in-place:
    // We iterate backwards so we can safely remove elements by overwriting them
    for (int i = list.count - 1; i >= 0; --i) {
        Move move = list[i];
        board.make_move(move, false);

        // If the move leaves us in check, it is illegal. Remove it.
        if (is_in_check(board, board.turn_ == chess::color::WHITE ? chess::color::BLACK : chess::color::WHITE)) {
            // Swap this move with the last move in the list and decrement count
            list[i] = list[list.count - 1];
            list.count--;
        }
        board.unmake_move();
    }
}

bool MoveGenerator::is_in_check(const Board &board, int in_check_color) {
    int king_square = in_check_color == chess::color::WHITE ? board.white_king_square_ : board.black_king_square_;
    int attacking_color = in_check_color == chess::color::WHITE ? chess::color::BLACK : chess::color::WHITE;
    return BoardUtils::is_square_attacked(board, king_square, attacking_color);
}



void MoveGenerator::generate_all_pseudo_legal_moves(Board &board, MoveList &list) {
    // 1. Grab the correct bitboards based on whose turn it is
    u64 pawns, knights, bishops, rooks, queens, kings;

    if (board.turn_ == chess::color::WHITE) {
        pawns   = board.piece_bitboard_[chess::piecelists::WHITE_PAWN];
        knights = board.piece_bitboard_[chess::piecelists::WHITE_KNIGHT];
        bishops = board.piece_bitboard_[chess::piecelists::WHITE_BISHOP];
        rooks   = board.piece_bitboard_[chess::piecelists::WHITE_ROOK];
        queens  = board.piece_bitboard_[chess::piecelists::WHITE_QUEEN];
        kings   = board.piece_bitboard_[chess::piecelists::WHITE_KING];
    } else {
        pawns   = board.piece_bitboard_[chess::piecelists::BLACK_PAWN];
        knights = board.piece_bitboard_[chess::piecelists::BLACK_KNIGHT];
        bishops = board.piece_bitboard_[chess::piecelists::BLACK_BISHOP];
        rooks   = board.piece_bitboard_[chess::piecelists::BLACK_ROOK];
        queens  = board.piece_bitboard_[chess::piecelists::BLACK_QUEEN];
        kings   = board.piece_bitboard_[chess::piecelists::BLACK_KING];
    }

    // 2. Iterate through each bitboard using compiler intrinsics

    // Generate Pawn Moves
    u64 bb = pawns;
    while (bb) {
        int sq = __builtin_ctzll(bb); // Hardware instruction: Find index of lowest set bit (0-63)
        generate_pawn_moves(board, sq, list);
        bb &= bb - 1;                 // Clear the lowest set bit to move to the next piece
    }

    // Generate Knight Moves
    bb = knights;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        generate_knight_moves(board, sq, list);
        bb &= bb - 1;
    }

    // Generate Bishop Moves (Sliding)
    bb = bishops;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        generate_sliding_piece_moves(board, sq, list);
        bb &= bb - 1;
    }

    // Generate Rook Moves (Sliding)
    bb = rooks;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        generate_sliding_piece_moves(board, sq, list);
        bb &= bb - 1;
    }

    // Generate Queen Moves (Sliding)
    bb = queens;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        generate_sliding_piece_moves(board, sq, list);
        bb &= bb - 1;
    }

    // Generate King Moves
    bb = kings;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        generate_king_moves(board, sq, list);
        bb &= bb - 1;
    }
}




// assume the function is called for the correct piece
void MoveGenerator::generate_sliding_piece_moves(const Board &board, int square, MoveList &list) {

    using namespace chess;

    // 1. Identify piece type at this square to know which magic table to use
    int piece = board.board_[square];
    int type = Piece::type_(piece);
    int color = Piece::color_(piece);

    // 2. Determine occupancy and friendly mask for blocking
    u64 friendly_bb = (color == color::WHITE) ? board.occupancy_white_ : board.occupancy_black_;
    u64 enemy_bb = (color == color::WHITE) ? board.occupancy_black_ : board.occupancy_white_;

    u64 attacks = 0ULL;

    // 3. Dispatch to the correct magic lookup table
    if (type == piece_type::ROOK || type == piece_type::QUEEN) {
        attacks |= BitboardUtils::get_rook_attacks(board, square);
    }

    if (type == piece_type::BISHOP || type == piece_type::QUEEN) {
        attacks |= BitboardUtils::get_bishop_attacks(board, square);
    }

    // 4. Remove moves that land on friendly pieces (we only care about pseudo-legal moves here)
    attacks &= ~friendly_bb;

    // 5. Convert bitboard to Move objects
    while (attacks) {
        int target_square = __builtin_ctzll(attacks);

        // Fast capture check
        bool is_capture = (enemy_bb & (1ULL << target_square)) != 0ULL;

        auto move = Move(square, target_square, piece_type::EMPTY, false, is_capture, false);
        list.emplace_back(move);

        // Clear the bit
        attacks &= attacks - 1;
    }

}

void MoveGenerator::generate_knight_moves(const Board &board, int square, MoveList &list) {
    using namespace chess;
    u64 enemy_bb, friendly_bb;

    if (board.turn_ == color::WHITE) {
        enemy_bb = board.occupancy_black_;
        friendly_bb = board.occupancy_white_;
    } else {
        enemy_bb = board.occupancy_white_;
        friendly_bb = board.occupancy_black_;
    }

    u64 attack_mask = board.knight_attacks[square] & ~friendly_bb;

    while (attack_mask) {
        int target_square = __builtin_ctzll(attack_mask);

        // check for captures
        bool is_capture = (enemy_bb & (1ULL << target_square)) != 0ULL;

        auto move = Move(square, target_square, piece::EMPTY, false, is_capture, false );
        list.emplace_back(move);

        attack_mask &= attack_mask - 1;

    }
}

void MoveGenerator::generate_pawn_moves(const Board &board, int square, MoveList &list) {
    using namespace chess;

    u64 enemy_bb = (board.turn_ == color::WHITE) ? board.occupancy_black_ : board.occupancy_white_;
    u64 occupancy = board.occupancy_white_ | board.occupancy_black_;

    u64 pawn = 1ULL << square;
    u64 pushes = 0ULL;

    // 1. Captures (Look up precomputed diagonal attacks, AND with enemy pieces)
    u64 captures = board.pawn_attacks[board.turn_][square] & enemy_bb;
    u64 ep_capture = 0ULL;

    int promotion_rank = (board.turn_ == color::WHITE) ? 7 : 0; // 0-indexed ranks

    // 2. En Passant
    if (board.enpassant_target_ != square::EMPTY) {
        u64 ep_bb = 1ULL << board.enpassant_target_;
        if (board.pawn_attacks[board.turn_][square] & ep_bb) {
            ep_capture = ep_bb;
        }
    }

    // 3. Pushes (Bitshift the pawn, AND with empty squares)
    if (board.turn_ == color::WHITE) {
        pushes = (pawn << 8) & ~occupancy; // Single push
        if (pushes && Square::rank_(square) == 1) { // If on start rank and single push is valid
            pushes |= (pawn << 16) & ~occupancy; // Double push
        }
    } else {
        pushes = (pawn >> 8) & ~occupancy; // Single push
        if (pushes && Square::rank_(square) == 6) {
            pushes |= (pawn >> 16) & ~occupancy; // Double push
        }
    }

    // Combine all valid pawn destinations into one bitboard
    u64 valid_moves = pushes | captures | ep_capture;

    // 4. Iterate through all targets and extract the moves
    while (valid_moves) {
        int target_square = __builtin_ctzll(valid_moves);

        bool is_ep = (ep_capture & (1ULL << target_square)) != 0ULL;
        bool is_capture = (captures & (1ULL << target_square)) != 0ULL || is_ep;

        // Handle Promotions
        if (Square::rank_(target_square) == promotion_rank) {
            int promotion_pieces[] = {piece_type::QUEEN, piece_type::ROOK, piece_type::BISHOP, piece_type::KNIGHT};
            for (int p_type : promotion_pieces) {
                int promoted_piece = Piece::piece_(p_type, board.turn_);
                auto move = Move(square, target_square, promoted_piece, false, is_capture, is_ep);
                list.emplace_back(move);
            }
        } else {
            auto move = Move(square, target_square, piece_type::EMPTY, false, is_capture, is_ep);
            list.emplace_back(move);
        }

        // Clear the lowest set bit
        valid_moves &= valid_moves - 1;
    }
}

void MoveGenerator::generate_king_moves(const Board &board, int square, MoveList &list) {
    using namespace chess;
    u64 enemy_bb, friendly_bb;

    int moving_piece_color = board.turn_;
    int opp_color = (moving_piece_color == color::WHITE) ? color::BLACK : color::WHITE;

    if (moving_piece_color == color::WHITE) {
        enemy_bb = board.occupancy_black_;
        friendly_bb = board.occupancy_white_;
    } else {
        enemy_bb = board.occupancy_white_;
        friendly_bb = board.occupancy_black_;
    }

    u64 occupancy = friendly_bb | enemy_bb;

    // 1. Normal King Moves using precomputed bitboards
    u64 attack_mask = board.king_attacks[square] & ~friendly_bb;

    while (attack_mask) {
        int target_square = __builtin_ctzll(attack_mask);

        // Check if the target square contains an enemy piece
        bool is_capture = (enemy_bb & (1ULL << target_square)) != 0ULL;

        auto move = Move(square, target_square, piece_type::EMPTY, false, is_capture, false);
        list.emplace_back(move);

        // Clear the lowest set bit
        attack_mask &= attack_mask - 1;
    }

    // 2. Castling Logic using bitwise occupancy checks
    int kingside_bitmask = (moving_piece_color == color::WHITE) ? bitmask::castling::WHITE_KING : bitmask::castling::BLACK_KING;
    int queenside_bitmask = (moving_piece_color == color::WHITE) ? bitmask::castling::WHITE_QUEEN : bitmask::castling::BLACK_QUEEN;

    bool is_kingside = (board.castling_rights_ & kingside_bitmask);
    bool is_queenside = (board.castling_rights_ & queenside_bitmask);
    int rank = (moving_piece_color == color::WHITE) ? 0 : 7;

    bool king_in_check = BoardUtils::is_square_attacked(board, square, opp_color);

    if (is_kingside && !king_in_check) {
        int f = Square::square_(file::F, rank);
        int g = Square::square_(file::G, rank);

        // Check for occupancy instantly using a bitmask
        u64 mask = (1ULL << f) | (1ULL << g);
        if ((occupancy & mask) == 0ULL) {
            if (!BoardUtils::is_square_attacked(board, f, opp_color) &&
                !BoardUtils::is_square_attacked(board, g, opp_color)) {
                auto move = Move(square, g, piece_type::EMPTY, true, false, false);
                list.emplace_back(move);
            }
        }
    }

    if (is_queenside && !king_in_check) {
        int b = Square::square_(file::B, rank);
        int c = Square::square_(file::C, rank);
        int d = Square::square_(file::D, rank);

        // Check for occupancy instantly using a bitmask
        u64 mask = (1ULL << b) | (1ULL << c) | (1ULL << d);
        if ((occupancy & mask) == 0ULL) {
            if (!BoardUtils::is_square_attacked(board, c, opp_color) &&
                !BoardUtils::is_square_attacked(board, d, opp_color)) {
                auto move = Move(square, c, piece_type::EMPTY, true, false, false);
                list.emplace_back(move);
            }
        }
    }
}
