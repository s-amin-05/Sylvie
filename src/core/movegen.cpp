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

    for (int sq=0; sq<64; sq++) {
        generate_pseudo_moves(board, sq);
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
void MoveGenerator::generate_sliding_piece_moves(const Board &board, const int &square) {
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
    std::vector<int> min_distance_squares(8);

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

void MoveGenerator::generate_knight_moves(const Board &board, int &square) {

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

    // // north side
    // if (Square::rank_(square) == 6 || Square::rank_(square) == 7) {
    //     direction_offsets[1] = direction_offsets[6] = 0;
    // }
    // if (Square::rank_(square) == 7) {
    //     direction_offsets[0] = direction_offsets[7] = 0;
    // }
    //
    // // south side
    // if (Square::rank_(square) == 0 || Square::rank_(square) == 1) {
    //     direction_offsets[3] = direction_offsets[4] = 0;
    // }
    // if (Square::rank_(square) == 0) {
    //     direction_offsets[2] = direction_offsets[5] = 0;
    // }

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

void MoveGenerator::generate_pawn_moves(const Board &board, int &square) {
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

void MoveGenerator::generate_king_moves(const Board &board, int &square) {
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



