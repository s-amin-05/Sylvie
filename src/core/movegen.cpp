#include <iostream>
#include <movegen.h>

#include "utils.h"

// might add some use later
MoveGenerator::MoveGenerator() = default;

void MoveGenerator::generate_legal_moves(Board &board) {
    if (!pseudo_legal_moves_.empty()) {
        //clear all moves before adding legal moves
        pseudo_legal_moves_.clear();
        legal_moves_.clear();
        king_moves_.clear();
        sliding_moves_.clear();
        knight_moves_.clear();
        pawn_moves_.clear();
    }
    // generate all pseudo legal moves
    generate_all_pseudo_legal_moves(board);
    // filter out illegal moves

    int opp_color = board.turn_ == chess::color::WHITE ? chess::color::BLACK : chess::color::WHITE;
    for (auto move: pseudo_legal_moves_) {
        board.make_move(move, false);
        if (!is_in_check(board, !opp_color)) legal_moves_.emplace_back(move);
        board.unmake_move();
    }
    
}

bool MoveGenerator::is_in_check(const Board &board, int in_check_color) {
    int king_square = in_check_color == chess::color::WHITE ? board.white_king_square_ : board.black_king_square_;
    int attacking_color = in_check_color == chess::color::WHITE ? chess::color::BLACK : chess::color::WHITE;
    return BoardUtils::is_square_attacked(board, king_square, attacking_color);
}



void MoveGenerator::generate_all_pseudo_legal_moves(Board &board) {
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
        generate_pawn_moves_bb(board, sq);
        bb &= bb - 1;                 // Clear the lowest set bit to move to the next piece
    }

    // Generate Knight Moves
    bb = knights;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        generate_knight_moves_bb(board, sq);
        bb &= bb - 1;
    }

    // Generate Bishop Moves (Sliding)
    bb = bishops;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        generate_sliding_piece_moves(board, sq);
        bb &= bb - 1;
    }

    // Generate Rook Moves (Sliding)
    bb = rooks;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        generate_sliding_piece_moves(board, sq);
        bb &= bb - 1;
    }

    // Generate Queen Moves (Sliding)
    bb = queens;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        generate_sliding_piece_moves(board, sq);
        bb &= bb - 1;
    }

    // Generate King Moves
    bb = kings;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        generate_king_moves_bb(board, sq);
        bb &= bb - 1;
    }
}


void MoveGenerator::generate_pseudo_moves(Board &board, int square) {

    int moving_piece = board.board_[square];
    int moving_piece_type = Piece::type_(moving_piece);
    switch (moving_piece_type) {
        case chess::piece_type::PAWN:
            generate_pawn_moves(board, square);
            break;
        case chess::piece_type::KNIGHT:
            generate_knight_moves(board, square);
            break;
        case chess::piece_type::BISHOP:
        case chess::piece_type::ROOK:
        case chess::piece_type::QUEEN:
            generate_sliding_piece_moves(board, square);
            break;
        case chess::piece_type::KING:
            generate_king_moves(board, square);
            break;
        default:
            break;
    }

}



// assume the function is called for the correct piece
void MoveGenerator::generate_sliding_piece_moves(const Board &board, int square) {
    // counterclockwise from North (first 4 - Rooks, next 4 - Bishops)
    constexpr int direction_offsets[8] = {
        movegen::direction_offset::NORTH,
        movegen::direction_offset::WEST,
        movegen::direction_offset::SOUTH,
        movegen::direction_offset::EAST,
        movegen::direction_offset::NORTH_WEST,
        movegen::direction_offset::SOUTH_WEST,
        movegen::direction_offset::SOUTH_EAST,
        movegen::direction_offset::NORTH_EAST
    };

    const int moving_piece = board.board_[square];
    const int moving_piece_color = Piece::color_(moving_piece);
    // if the piece is not of the current player, we can't move
    if (moving_piece_color != board.turn_) return;
    // minimum distance to captures or edge
    // std::vector<int> min_distance_squares(8);
    int min_distance_squares[8];

    // setting min_distance to board edge
    min_distance_squares[0] = 7 - Square::rank_(square);
    min_distance_squares[1] = Square::file_(square);
    min_distance_squares[2] = Square::rank_(square);
    min_distance_squares[3] = 7 - Square::file_(square);
    min_distance_squares[4] = std::min(7-Square::rank_(square), Square::file_(square));
    min_distance_squares[5] = std::min(Square::rank_(square), Square::file_(square));
    min_distance_squares[6] = std::min(Square::rank_(square), 7-Square::file_(square));
    min_distance_squares[7] = std::min(7-Square::rank_(square), 7-Square::file_(square));

    int dir_index_start = 0;
    int dir_index_end = 8;
    int moving_piece_type = Piece::type_(moving_piece);
    if (moving_piece_type == chess::piece_type::ROOK) dir_index_end = 4;
    else if (moving_piece_type == chess::piece_type::BISHOP) dir_index_start = 4;


    // loop over all directions
    for (dir_index_start; dir_index_start < dir_index_end; dir_index_start++) {

        const int min_distance = min_distance_squares[dir_index_start];
        // loop over all squares till edge and check if we find a piece
        for (int i = 1; i <= min_distance; i++) {

            Move move;
            int curr_square_num = square + i*direction_offsets[dir_index_start];

            // piece found
            if (board.board_[curr_square_num] != chess::piece_type::EMPTY) {
                // if the piece is of the same color, we have a capture other wise
                // we must go back one step and NOT push the null move
                if (moving_piece_color == Piece::color_(board.board_[curr_square_num])) {
                    min_distance_squares[dir_index_start] = i-1;
                    break;
                }
                min_distance_squares[dir_index_start] = i;
                move = Move(square, curr_square_num, chess::piece_type::EMPTY, false, true, false );
                sliding_moves_.emplace_back(move);
                pseudo_legal_moves_.emplace_back(move);
                nonking_moves.emplace_back(move);
                break;
            }
            // no piece found -> normal move can be made
            move = Move(square, curr_square_num, chess::piece_type::EMPTY, false, false, false );
            sliding_moves_.emplace_back(move);
            pseudo_legal_moves_.emplace_back(move);
            nonking_moves.emplace_back(move);
        }

    }

}

void MoveGenerator::generate_knight_moves_bb(const Board &board, int square) {
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
        knight_moves_.emplace_back(move);
        pseudo_legal_moves_.emplace_back(move);
        nonking_moves.emplace_back(move);

        attack_mask &= attack_mask - 1;

    }
}

void MoveGenerator::generate_pawn_moves_bb(const Board &board, int square) {
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
                pawn_moves_.emplace_back(move);
                pseudo_legal_moves_.emplace_back(move);
                nonking_moves.emplace_back(move);
            }
        } else {
            auto move = Move(square, target_square, piece_type::EMPTY, false, is_capture, is_ep);
            pawn_moves_.emplace_back(move);
            pseudo_legal_moves_.emplace_back(move);
            nonking_moves.emplace_back(move);
        }

        // Clear the lowest set bit
        valid_moves &= valid_moves - 1;
    }
}

void MoveGenerator::generate_king_moves_bb(const Board &board, int square) {
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
        king_moves_.emplace_back(move);
        pseudo_legal_moves_.emplace_back(move);

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
                king_moves_.emplace_back(move);
                pseudo_legal_moves_.emplace_back(move);
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
                king_moves_.emplace_back(move);
                pseudo_legal_moves_.emplace_back(move);
            }
        }
    }
}

void MoveGenerator::generate_knight_moves(const Board &board, int square) {

    const int moving_piece = board.board_[square];
    const int moving_piece_type = Piece::type_(moving_piece);
    const int moving_piece_color = Piece::color_(moving_piece);
    if (moving_piece_type != chess::piece_type::KNIGHT || moving_piece_color != board.turn_)
        return;
    // from noNoWe to noNoEa counterclockwise
    int direction_offsets[8] = {movegen::knight_offset::NO_NO_WE, movegen::knight_offset::NO_WE_WE,
                                movegen::knight_offset::SO_WE_WE, movegen::knight_offset::SO_SO_WE,
                                movegen::knight_offset::SO_SO_EA, movegen::knight_offset::SO_EA_EA,
                                movegen::knight_offset::NO_EA_EA, movegen::knight_offset::NO_NO_EA};

    // west side
    if (Square::file_(square) == chess::file::B || Square::file_(square) == chess::file::A) {
        direction_offsets[1] = direction_offsets[2] = 0;
    }
    if (Square::file_(square) == chess::file::A) {
        direction_offsets[0] = direction_offsets[3] = 0;
    }

    // east side
    if (Square::file_(square) == chess::file::G || Square::file_(square) == chess::file::H) {
        direction_offsets[5] = direction_offsets[6] = 0;
    }
    if (Square::file_(square) == chess::file::H) {
        direction_offsets[4] = direction_offsets[7] = 0;
    }


    // loop over all directions
    for (int i = 0; i < 8; i++) {
        if (direction_offsets[i] == 0)
            continue;

        const auto target_square = square + direction_offsets[i];
        if (target_square < 0 || target_square > 63) continue;
        const int target_piece = board.board_[target_square];
        bool is_capture = false;

        if (target_piece != chess::piece_type::EMPTY && moving_piece_color == Piece::color_(target_piece))
            continue;

        if (target_piece != chess::piece_type::EMPTY && moving_piece_color != Piece::color_(target_piece)) {
            is_capture = true;
        }

        auto move = Move(square, target_square, chess::piece_type::EMPTY, false, is_capture, false);
        knight_moves_.emplace_back(move);
        pseudo_legal_moves_.emplace_back(move);
        nonking_moves.emplace_back(move);
    }
}

void MoveGenerator::generate_pawn_moves(const Board &board, int square) {
    const int moving_piece = board.board_[square];
    const int moving_piece_type = Piece::type_(moving_piece);
    const int moving_piece_color = Piece::color_(moving_piece);

    if (moving_piece_type != chess::piece_type::PAWN || moving_piece_color != board.turn_)
        return;
    int pawn_direction = (moving_piece_color == chess::color::WHITE) ? 1 : -1;
    // we are taking offsets for white pawns and flipping them for black
    int pawn_forward_offsets[] = {
        movegen::direction_offset::NORTH * pawn_direction,
        2 * movegen::direction_offset::NORTH * pawn_direction,
    };
    int pawn_capture_offsets[] = {
        movegen::direction_offset::NORTH_WEST * pawn_direction,
        movegen::direction_offset::NORTH_EAST * pawn_direction,
    };
    int promotion_piece_types[] = {chess::piece_type::QUEEN, chess::piece_type::ROOK, chess::piece_type::BISHOP, chess::piece_type::KNIGHT};

    int pawn_start_rank = (moving_piece_color == chess::color::WHITE) ? 1 : 6;
    int pawn_promotion_rank = (moving_piece_color == chess::color::WHITE) ? 6 : 1;
    int pawn_enpassant_rank = (moving_piece_color == chess::color::WHITE) ? 4 : 3;

    int promotion_piece = chess::piece_type::EMPTY;
    bool is_enpassant = false, is_capture = false, is_castling = false;

    // MOVE VALIDATION
    // check for pawn files to avoid 360 captures
    if (Square::file_(square) == chess::file::A) {
        pawn_capture_offsets[(moving_piece_color? 0: 1)] = 0;
    }else if (Square::file_(square) == chess::file::H) {
        pawn_capture_offsets[(moving_piece_color? 1: 0)] = 0;
    }

    // check if double pawn push is allowed or not
    // if pawn is at its starting square or not
    if (Square::rank_(square) != pawn_start_rank) {
        pawn_forward_offsets[1] = 0;
    }

    // check for blocking pieces before making moves
    if (board.board_[square + pawn_forward_offsets[0]] != chess::piece_type::EMPTY) {
        pawn_forward_offsets[0] = 0;
        pawn_forward_offsets[1] = 0;
    }else if (board.board_[square + pawn_forward_offsets[1]] != chess::piece_type::EMPTY) {
        pawn_forward_offsets[1] = 0;
    }

    for (int i=0; i < 2; i++) {
        int capture_square = square + pawn_capture_offsets[i];
        bool check_empty = (board.board_[capture_square]) == chess::piece_type::EMPTY;
        bool check_same_color = (moving_piece_color == Piece::color_(board.board_[capture_square]));
        bool check_enpassant = (board.enpassant_target_ == capture_square && Square::rank_(square) == pawn_enpassant_rank);
        if ((check_empty || check_same_color) && !(check_empty && check_enpassant)) {
            pawn_capture_offsets[i] = 0;
        }
    }


    // pawn pushes
    // single pawn push is valid but not double pawn push
    if (pawn_forward_offsets[0] && !pawn_forward_offsets[1]) {
        int target_square = square + pawn_forward_offsets[0];
        if (Square::rank_(square) == pawn_promotion_rank) {
            for (int i = 0; i < 4; i++) {
                promotion_piece = Piece::piece_(promotion_piece_types[i], moving_piece_color);
                auto move = Move(square, target_square, promotion_piece, is_castling, is_capture, is_enpassant);
                pawn_moves_.emplace_back(move);
                pseudo_legal_moves_.emplace_back(move);
                nonking_moves.emplace_back(move);
            }
        }else {
            promotion_piece = chess::piece_type::EMPTY;
            auto move = Move(square, target_square, promotion_piece, is_castling, is_capture, is_enpassant);
            pawn_moves_.emplace_back(move);
            pseudo_legal_moves_.emplace_back(move);
            nonking_moves.emplace_back(move);
        }
    }
    // double pawn pushes
    else if (pawn_forward_offsets[0] && pawn_forward_offsets[1]) {
        promotion_piece = chess::piece_type::EMPTY;
        for (int i = 0; i < 2; i++) {
            int target_square = square + pawn_forward_offsets[i];
            auto move = Move(square, target_square, promotion_piece, is_castling, is_capture, is_enpassant);
            pawn_moves_.emplace_back(move);
            pseudo_legal_moves_.emplace_back(move);
            nonking_moves.emplace_back(move);
        }
    }

    // capturing moves
    for (int i=0; i < 2; i++) {
        if (!pawn_capture_offsets[i]) continue;
        int target_square = square + pawn_capture_offsets[i];
        is_capture = true;
        is_enpassant = (board.enpassant_target_ == target_square);
        if (Square::rank_(square) == pawn_promotion_rank) {
            for (int j=0; j < 4; j++) {
                promotion_piece = Piece::piece_(promotion_piece_types[j], moving_piece_color);
                auto move = Move(square, target_square, promotion_piece, is_castling, is_capture, is_enpassant);
                pawn_moves_.emplace_back(move);
                pseudo_legal_moves_.emplace_back(move);
                nonking_moves.emplace_back(move);
            }
        }else {
            promotion_piece = chess::piece_type::EMPTY;
            auto move = Move(square, target_square, promotion_piece, is_castling, is_capture, is_enpassant);
            pawn_moves_.emplace_back(move);
            pseudo_legal_moves_.emplace_back(move);
            nonking_moves.emplace_back(move);
        }

    }


}

void MoveGenerator::generate_king_moves(const Board &board, int square) {
    int moving_piece = board.board_[square];
    const int moving_piece_type = Piece::type_(moving_piece);
    const int moving_piece_color = Piece::color_(moving_piece);

    if (moving_piece_type != chess::piece_type::KING || moving_piece_color != board.turn_) return;


    int direction_offsets[8] = {
        movegen::direction_offset::NORTH,
        movegen::direction_offset::WEST,
        movegen::direction_offset::SOUTH,
        movegen::direction_offset::EAST,
        movegen::direction_offset::NORTH_WEST,
        movegen::direction_offset::SOUTH_WEST,
        movegen::direction_offset::SOUTH_EAST,
        movegen::direction_offset::NORTH_EAST
    };

    if (Square::file_(square) == chess::file::A) {
        direction_offsets[1] = direction_offsets[4] = direction_offsets[5] = 0;
    }else if (Square::file_(square) == chess::file::H) {
        direction_offsets[3] = direction_offsets[6] = direction_offsets[7] = 0;
    }

    if (Square::rank_(square) == 0) {
        direction_offsets[2] = direction_offsets[5] = direction_offsets[6] = 0;
    }else if (Square::rank_(square) == 7) {
        direction_offsets[0] = direction_offsets[4] = direction_offsets[7] = 0;
    }

    // check for normal moves
    for (int i=0; i<8; i++) {
        if (!direction_offsets[i]) continue;
        // may remove to optimize
        if (square + direction_offsets[i] < 0 || square + direction_offsets[i] > 63) continue;
        auto target_square = square + direction_offsets[i];
        int target_piece = board.board_[target_square];
        if (target_piece != chess::piece_type::EMPTY && Piece::color_(target_piece) == moving_piece_color) continue;
        Move move;
        if (target_piece == chess::piece_type::EMPTY) {
            move = Move(square, target_square, chess::piece_type::EMPTY, false, false, false);
        }
        else if (Piece::color_(target_piece) != moving_piece_color) {
            move = Move(square, target_square, chess::piece_type::EMPTY, false, true, false);
        }
        king_moves_.emplace_back(move);
        pseudo_legal_moves_.emplace_back(move);
    }

    // check for castling
    int kingside_bitmask = (board.turn_ == chess::color::WHITE) ? bitmask::castling::WHITE_KING : bitmask::castling::BLACK_KING;
    int queenside_bitmask = (board.turn_ == chess::color::WHITE) ? bitmask::castling::WHITE_QUEEN : bitmask::castling::BLACK_QUEEN;

    bool is_kingside = (board.castling_rights_ & kingside_bitmask);
    bool is_queenside = (board.castling_rights_ & queenside_bitmask);
    int rank = (moving_piece_color == chess::color::WHITE) ? 0 : 7;

    int opp_color = moving_piece_color == chess::color::WHITE ? chess::color::BLACK: chess::color::WHITE;
    bool king_in_check = BoardUtils::is_square_attacked(board, square, opp_color);

    if (is_kingside && !king_in_check) {
        // check for occupancy
        int g = Square::square_(chess::file::G, rank);
        int f = Square::square_(chess::file::F, rank);
        bool g_occupied = (board.board_[g] != chess::piece_type::EMPTY);
        bool f_occupied = (board.board_[f] != chess::piece_type::EMPTY);

        bool g_attacked = (BoardUtils::is_square_attacked(board, g, opp_color));
        bool f_attacked = (BoardUtils::is_square_attacked(board, f, opp_color));
        if (!g_occupied && !f_occupied && !g_attacked && !f_attacked) {
            auto move = Move(square, g, chess::piece_type::EMPTY, true, false, false);
            king_moves_.emplace_back(move);
            pseudo_legal_moves_.emplace_back(move);
        }
    }
    if (is_queenside && !king_in_check) {
        // check for occupancy
        int b = Square::square_(chess::file::B, rank);
        int c = Square::square_(chess::file::C, rank);
        int d = Square::square_(chess::file::D, rank);
        bool b_occupied = (board.board_[b] != chess::piece_type::EMPTY);
        bool c_occupied = (board.board_[c] != chess::piece_type::EMPTY);
        bool d_occupied = (board.board_[d] != chess::piece_type::EMPTY);
        // bool b_attacked = (BoardUtils::is_square_attacked(board, b, !Piece::color_(moving_piece)));
        bool c_attacked = (BoardUtils::is_square_attacked(board, c, opp_color));
        bool d_attacked = (BoardUtils::is_square_attacked(board, d, opp_color));

        if (!b_occupied && !c_occupied && !d_occupied && !c_attacked && !d_attacked) {
            auto move = Move(square, c, chess::piece_type::EMPTY, true, false, false);
            king_moves_.emplace_back(move);
            pseudo_legal_moves_.emplace_back(move);
        }
    }


}



