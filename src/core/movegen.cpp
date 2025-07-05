#include <movegen.h>

#include "utils.h"

// might add some use later
MoveGenerator::MoveGenerator() = default;

// void MoveGenerator::generate_pseudo_moves(Board &board) {
//     for (int i=0; i<12; i++) {
//         int piece_type = (i%6)+1;
//         for (auto &square : board.piece_lists_[i]) {
//             switch (piece_type) {
//                 case chess::piece::PAWN:
//                     generate_pawn_moves(board, square);
//                     break;
//                 case chess::piece::KNIGHT:
//                     generate_knight_moves(board, square);
//                     break;
//                 case chess::piece::BISHOP:
//                 case chess::piece::ROOK:
//                 case chess::piece::QUEEN:
//                     generate_sliding_piece_moves(board, square);
//                     break;
//                 case chess::piece::KING:
//                     generate_king_moves(board, square);
//                     break;
//             }
//         }
//     }
// }


// assume the function is called for the correct piece
void MoveGenerator::generate_sliding_piece_moves(Board &board, Square &square) {
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

    Piece moving_piece = board.board_[square.square_];

    // if the piece is not of the current player, we can't move
    if (moving_piece.piece_color_ != board.turn_) return;
    // minimum distance to captures or edge
    std::vector<int> min_distance_squares(8);

    // setting min_distance to board edge
    min_distance_squares[0] = 7 - square.rank_;
    min_distance_squares[1] = square.file_;
    min_distance_squares[2] = square.rank_;
    min_distance_squares[3] = 7 - square.file_;
    min_distance_squares[4] = std::min(7-square.rank_, square.file_);
    min_distance_squares[5] = std::min(square.rank_, square.file_);
    min_distance_squares[6] = std::min(square.rank_, 7-square.file_);
    min_distance_squares[7] = std::min(7-square.rank_, 7-square.file_);

    int dir_index_start = 0;
    int dir_index_end = 8;
    if (moving_piece.piece_type_ == chess::piece::ROOK) dir_index_end = 4;
    else if (moving_piece.piece_type_ == chess::piece::BISHOP) dir_index_start = 4;


    // loop over all directions
    for (dir_index_start; dir_index_start < dir_index_end; dir_index_start++) {

        int min_distance = min_distance_squares[dir_index_start];
        // loop over all squares till edge and check if we find a piece
        for (int i = 1; i <= min_distance; i++) {

            Move move;
            int curr_square_num = square.square_ + i*direction_offsets[dir_index_start];

            // piece found
            if (board.board_[curr_square_num].piece_type_ != chess::piece::EMPTY) {
                // if the piece is of the same color, we have a capture other wise
                // we must go back one step and NOT push the null move
                if (moving_piece.piece_color_ == board.board_[curr_square_num].piece_color_) {
                    min_distance_squares[dir_index_start] = i-1;
                    break;
                }
                min_distance_squares[dir_index_start] = i;
                move = Move(square, curr_square_num, Piece(), false, true, false );
                sliding_moves_.push_back(move);
                pseudo_legal_moves_.push_back(move);
                break;
            }
            // no piece found -> normal move can be made
            move = Move(square, curr_square_num, Piece(), false, false, false );
            sliding_moves_.push_back(move);
            pseudo_legal_moves_.push_back(move);
        }

    }

}

void MoveGenerator::generate_knight_moves(Board &board, Square &square) {

    Piece moving_piece = board.board_[square.square_];
    if (moving_piece.piece_type_ != chess::piece::KNIGHT || moving_piece.piece_color_ != board.turn_)
        return;
    // from noNoWe to noNoEa counterclockwise
    int direction_offsets[8] = {movegen::knight_offset::NO_NO_WE, movegen::knight_offset::NO_WE_WE,
                                movegen::knight_offset::SO_WE_WE, movegen::knight_offset::SO_SO_WE,
                                movegen::knight_offset::SO_SO_EA, movegen::knight_offset::SO_EA_EA,
                                movegen::knight_offset::NO_EA_EA, movegen::knight_offset::NO_NO_EA};

    // west side
    if (square.file_ == chess::file::B || square.file_ == chess::file::A) {
        direction_offsets[1] = direction_offsets[2] = 0;
    }
    if (square.file_ == chess::file::A) {
        direction_offsets[0] = direction_offsets[3] = 0;
    }

    // east side
    if (square.file_ == chess::file::F || square.file_ == chess::file::G) {
        direction_offsets[5] = direction_offsets[6] = 0;
    }
    if (square.file_ == chess::file::G) {
        direction_offsets[4] = direction_offsets[7] = 0;
    }

    // north side
    if (square.rank_ == 6 || square.rank_ == 7) {
        direction_offsets[1] = direction_offsets[6] = 0;
    }
    if (square.rank_ == 7) {
        direction_offsets[0] = direction_offsets[7] = 0;
    }

    // south side
    if (square.rank_ == 0 || square.rank_ == 1) {
        direction_offsets[3] = direction_offsets[4] = 0;
    }
    if (square.rank_ == 0) {
        direction_offsets[2] = direction_offsets[5] = 0;
    }

    // loop over all directions
    for (int i = 0; i < 8; i++) {
        if (direction_offsets[i] == 0)
            continue;

        Square target_square = Square(square.square_ + direction_offsets[i]);
        Piece target_piece = board.board_[target_square.square_];
        bool is_capture = false;

        if (target_piece.piece_type_ != chess::piece::EMPTY && moving_piece.piece_color_ == target_piece.piece_color_)
            continue;

        if (target_piece.piece_type_ != chess::piece::EMPTY && moving_piece.piece_color_ != target_piece.piece_color_) {
            is_capture = true;
        }

        Move move = Move(square, target_square, Piece(), false, is_capture, false);
        knight_moves_.push_back(move);
        pseudo_legal_moves_.push_back(move);
    }
}

void MoveGenerator::generate_pawn_moves(Board &board, Square &square) {
    Piece moving_piece = board.board_[square.square_];
    if (moving_piece.piece_type_ != chess::piece::PAWN || moving_piece.piece_color_ != board.turn_)
        return;
    int pawn_direction = (moving_piece.piece_color_ == chess::color::WHITE) ? 1 : -1;
    // we are taking offsets for white pawns and flipping them for black
    int pawn_forward_offsets[] = {
        movegen::direction_offset::NORTH * pawn_direction,
        2 * movegen::direction_offset::NORTH * pawn_direction,
    };
    int pawn_capture_offsets[] = {
        movegen::direction_offset::NORTH_WEST * pawn_direction,
        movegen::direction_offset::NORTH_EAST * pawn_direction,
    };
    int promotion_piece_types[] = {chess::piece::QUEEN, chess::piece::ROOK, chess::piece::BISHOP, chess::piece::KNIGHT};

    int pawn_start_rank = (moving_piece.piece_color_ == chess::color::WHITE) ? 1 : 6;
    int pawn_promotion_rank = (moving_piece.piece_color_ == chess::color::WHITE) ? 6 : 1;
    int pawn_enpassant_rank = (moving_piece.piece_color_ == chess::color::WHITE) ? 4 : 3;

    Piece promotion_piece = Piece();
    bool is_enpassant = false, is_capture = false, is_castling = false;

    // MOVE VALIDATION
    // check for pawn files to avoid 360 captures
    if (square.file_ == chess::file::A) {
        pawn_capture_offsets[(moving_piece.piece_color_? 0: 1)] = 0;
    }else if (square.file_ == chess::file::H) {
        pawn_capture_offsets[(moving_piece.piece_color_? 1: 0)] = 0;
    }

    // check if double pawn push is allowed or not
    // if pawn is at its starting square or not
    if (square.rank_ != pawn_start_rank) {
        pawn_forward_offsets[1] = 0;
    }

    // check for blocking pieces before making moves
    if (board.board_[square.square_ + pawn_forward_offsets[0]].piece_type_ != chess::piece::EMPTY) {
        pawn_forward_offsets[0] = 0;
        pawn_forward_offsets[1] = 0;
    }else if (board.board_[square.square_ + pawn_forward_offsets[1]].piece_type_ != chess::piece::EMPTY) {
        pawn_forward_offsets[1] = 0;
    }

    for (int i=0; i < 2; i++) {
        Square capture_square = Square(square.square_ + pawn_capture_offsets[i]);
        bool check_empty = (board.board_[capture_square.square_].piece_type_ == chess::piece::EMPTY);
        bool check_same_color = (moving_piece.piece_color_ == board.board_[capture_square.square_].piece_color_);
        bool check_enpassant = (board.enpassant_target_.square_ == capture_square.square_ && square.rank_ == pawn_enpassant_rank);
        if ((check_empty || check_same_color) && !(check_empty && check_enpassant)) {
            pawn_capture_offsets[i] = 0;
        }
    }


    // pawn pushes
    // single pawn push is valid but not double pawn push
    if (pawn_forward_offsets[0] && !pawn_forward_offsets[1]) {
        Square target_square = Square(square.square_ + pawn_forward_offsets[0]);
        if (square.rank_ == pawn_promotion_rank) {
            for (int i = 0; i < 4; i++) {
                promotion_piece = Piece(promotion_piece_types[i], moving_piece.piece_color_);
                Move move = Move(square, target_square, promotion_piece, is_castling, is_capture, is_enpassant);
                pawn_moves_.push_back(move);
                pseudo_legal_moves_.push_back(move);
            }
        }else {
            promotion_piece = Piece();
            Move move = Move(square, target_square, promotion_piece, is_castling, is_capture, is_enpassant);
            pawn_moves_.push_back(move);
            pseudo_legal_moves_.push_back(move);
        }
    }
    // double pawn pushes
    else if (pawn_forward_offsets[0] && pawn_forward_offsets[1]) {
        promotion_piece = Piece();
        for (int i = 0; i < 2; i++) {
            Square target_square = Square(square.square_ + pawn_forward_offsets[i]);
            Move move = Move(square, target_square, promotion_piece, is_castling, is_capture, is_enpassant);
            pawn_moves_.push_back(move);
            pseudo_legal_moves_.push_back(move);
        }
    }

    // capturing moves
    for (int i=0; i < 2; i++) {
        if (!pawn_capture_offsets[i]) continue;
        Square target_square = Square(square.square_ + pawn_capture_offsets[i]);
        is_capture = true;
        is_enpassant = (board.enpassant_target_.square_ == target_square.square_);
        if (square.rank_ == pawn_promotion_rank) {
            for (int j=0; j < 4; j++) {
                promotion_piece = Piece(promotion_piece_types[j], moving_piece.piece_color_);
                Move move = Move(square, target_square, promotion_piece, is_castling, is_capture, is_enpassant);
                pawn_moves_.push_back(move);
                pseudo_legal_moves_.push_back(move);
            }
        }else {
            promotion_piece = Piece();
            Move move = Move(square, target_square, promotion_piece, is_castling, is_capture, is_enpassant);
            pawn_moves_.push_back(move);
            pseudo_legal_moves_.push_back(move);
        }

    }


}

void MoveGenerator::generate_king_moves(Board &board, Square &square) {
    Piece moving_piece = board.board_[square.square_];
    if (moving_piece.piece_type_ != chess::piece::KING || moving_piece.piece_color_ != board.turn_) return;


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

    if (square.file_ == chess::file::A) {
        direction_offsets[1] = direction_offsets[4] = direction_offsets[5] = 0;
    }else if (square.file_ == chess::file::H) {
        direction_offsets[3] = direction_offsets[6] = direction_offsets[7] = 0;
    }

    if (square.rank_ == 0) {
        direction_offsets[2] = direction_offsets[5] = direction_offsets[6] = 0;
    }else if (square.rank_ == 7) {
        direction_offsets[0] = direction_offsets[4] = direction_offsets[7] = 0;
    }

    // check for normal moves
    for (int i=0; i<8; i++) {
        if (!direction_offsets[i]) continue;
        Square target_square = Square(square.square_ + direction_offsets[i]);
        Piece target_piece = board.board_[target_square.square_];
        if (target_piece.piece_type_ != chess::piece::EMPTY && target_piece.piece_color_ == moving_piece.piece_color_) continue;
        Move move;
        if (target_piece.piece_type_ == chess::piece::EMPTY) {
            move = Move(square, target_square, Piece(), false, false, false);
        }
        else if (target_piece.piece_type_ != chess::piece::EMPTY && target_piece.piece_color_ != moving_piece.piece_color_) {
            move = Move(square, target_square, Piece(), false, true, false);
        }
    }

    // check for castling
    bool is_kingside = (board.castling_rights_ & (bitmask::castling::WHITE_KING | bitmask::castling::BLACK_KING));
    bool is_queenside = (board.castling_rights_ & (bitmask::castling::WHITE_QUEEN | bitmask::castling::BLACK_QUEEN));
    int rank = (moving_piece.piece_color_ == chess::color::WHITE) ? 0 : 7;

    if (is_kingside) {
        // check for occupancy
        Square g = Square(chess::file::G, rank);
        Square f = Square(chess::file::F, rank);
        bool g_occupied = (board.board_[g.square_].piece_type_ != chess::piece::EMPTY);
        bool f_occupied = (board.board_[f.square_].piece_type_ != chess::piece::EMPTY);
        bool g_attacked = (BoardUtils::is_square_attacked(board, g, moving_piece.piece_color_));
        bool f_attacked = (BoardUtils::is_square_attacked(board, f, moving_piece.piece_color_));
        if (!g_occupied && !f_occupied && !g_attacked && !f_attacked) {
            Move move = Move(square, Square("f1"), Piece(), true, false, false);
            king_moves_.push_back(move);
            pseudo_legal_moves_.push_back(move);
        }
    }else if (is_queenside) {
        // check for occupancy
        Square b = Square(chess::file::B, rank);
        Square c = Square(chess::file::C, rank);
        Square d = Square(chess::file::D, rank);
        bool b_occupied = (board.board_[b.square_].piece_type_ != chess::piece::EMPTY);
        bool c_occupied = (board.board_[c.square_].piece_type_ != chess::piece::EMPTY);
        bool d_occupied = (board.board_[d.square_].piece_type_ != chess::piece::EMPTY);
        bool b_attacked = (BoardUtils::is_square_attacked(board, b, moving_piece.piece_color_));
        bool c_attacked = (BoardUtils::is_square_attacked(board, c, moving_piece.piece_color_));
        bool d_attacked = (BoardUtils::is_square_attacked(board, d, moving_piece.piece_color_));

        if (!b_occupied && !c_occupied && !d_occupied && !b_attacked && !c_attacked && !d_attacked) {
            Move move = Move(square, Square("d1"), Piece(), true, false, false);
            king_moves_.push_back(move);
        }
    }


}



