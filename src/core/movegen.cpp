#include <movegen.h>

// might add some use later
MoveGenerator::MoveGenerator() = default;

void MoveGenerator::generate_pseudo_moves(Board &board) {
    for (int i=0; i<12; i++) {
        int piece_type = (i%6)+1;
        for (auto &square : board.piece_lists_[i]) {
            switch (piece_type) {
                case chess::piece::PAWN:
                    generate_pawn_moves(board, square);
                    break;
                case chess::piece::KNIGHT:
                    generate_knight_moves(board, square);
                    break;
                case chess::piece::BISHOP:
                case chess::piece::ROOK:
                case chess::piece::QUEEN:
                    generate_sliding_piece_moves(board, square);
                    break;
                case chess::piece::KING:
                    generate_king_moves(board, square);
                    break;
            }
        }
    }
}


// assume the function is called for the correct piece
void MoveGenerator::generate_sliding_piece_moves(Board &board, Square &square) {
    // counterclockwise from North (first 4 - Rooks, next 4 - Bishops)
    constexpr int direction_offsets[8] = {
        movegen::sliding_offset::NORTH,
        movegen::sliding_offset::WEST,
        movegen::sliding_offset::SOUTH,
        movegen::sliding_offset::EAST,
        movegen::sliding_offset::NORTH_WEST,
        movegen::sliding_offset::SOUTH_WEST,
        movegen::sliding_offset::SOUTH_EAST,
        movegen::sliding_offset::NORTH_EAST
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
    if (moving_piece.piece_type_ != chess::piece::KNIGHT || moving_piece.piece_color_ != board.turn_) return;
    // from noNoWe to noNoEa counterclockwise
    int direction_offsets[8] = {
        movegen::knight_offset::NO_NO_WE,
        movegen::knight_offset::NO_WE_WE,
        movegen::knight_offset::SO_WE_WE,
        movegen::knight_offset::SO_SO_WE,
        movegen::knight_offset::SO_SO_EA,
        movegen::knight_offset::SO_EA_EA,
        movegen::knight_offset::NO_EA_EA,
        movegen::knight_offset::NO_NO_EA
    };

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
        if (direction_offsets[i] == 0) continue;

        Square target_square = Square(square.square_ + direction_offsets[i]);
        Piece target_piece = board.board_[target_square.square_];
        bool is_capture = false;

        if (target_piece.piece_type_ != chess::piece::EMPTY && moving_piece.piece_color_ == target_piece.piece_color_) continue;

        if (target_piece.piece_type_ != chess::piece::EMPTY && moving_piece.piece_color_ != target_piece.piece_color_) {
            is_capture = true;
        }

        Move move = Move(square, target_square, Piece(), false, is_capture, false );
        knight_moves_.push_back(move);
        pseudo_legal_moves_.push_back(move);
    }
    
}





