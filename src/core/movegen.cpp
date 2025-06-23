#include <movegen.h>

// might add some use later
MoveGenerator::MoveGenerator() = default;

// assume the function is called for the correct piece
void MoveGenerator::generate_sliding_piece_moves(Board &board, Square &square) {
    Piece moving_piece = board.board_[square.square_];

    // if the piece is not of the current player, we can't move
    if (moving_piece.piece_color_ != board.turn_) return;
    // minimum distance to captures or edge
    std::vector<int> min_distance_squares(8);

    // setting min_distance to board edge
    min_distance_squares[movegen::sliding_directions::NORTH] = 7 - square.rank_;
    min_distance_squares[movegen::sliding_directions::WEST] = square.file_;
    min_distance_squares[movegen::sliding_directions::SOUTH] = square.rank_;
    min_distance_squares[movegen::sliding_directions::EAST] = 7 - square.file_;
    min_distance_squares[movegen::sliding_directions::NORTH_WEST] = std::min(7-square.rank_, (int)square.file_);
    min_distance_squares[movegen::sliding_directions::SOUTH_WEST] = std::min((int)square.rank_, (int)square.file_);
    min_distance_squares[movegen::sliding_directions::SOUTH_EAST] = std::min((int)square.rank_, 7-square.file_);
    min_distance_squares[movegen::sliding_directions::NORTH_EAST] = std::min(7-square.rank_, 7-square.file_);

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
            int curr_square_num = square.square_ + i*_direction_offsets_[dir_index_start];

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

    // loop over all directions

}





