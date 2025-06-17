#include <../../../include/sylvie/core/utils/utils.h>
#include <bits/stdc++.h>
#include <board.h>

Board::Board(): Board(chess::starting_pos_fen) {}

Board::Board(const string &position_fen):
    turn_(chess::color::WHITE),
    castling_rights_(0xF),
    enpassant_target_(chess::square::EMPTY),
    ply_count_(0),
    fullmove_number_(1),
    repetition_count_(0),
    halfmove_count_(0)
{
    board_fen_ = position_fen;

    // Initialize board array to empty
    for (int i = 0; i < 64; i++) {
        board_[i] = Piece();
    }

    setup_using_fen();

}

void Board::setup_using_fen() {

    vector<string> fen_parts = Utils::split(board_fen_, ' ');

    // fenParts[0] = piece_placement
    vector<string> piece_placement = Utils::split(fen_parts[0], '/');

    for (int i = 0; i < 8; i++) {
        int j=0;
        for (char c : piece_placement[i]) {
            if (Utils::is_digit(c)) {
                j += c - '0';
            }
            else {
                Piece piece = Piece(c);
                Square square = Square((7-i)*8 + j);
                board_[square.square_] = piece;
                j++;
            }
        }
    }

    // fenParts[1] = active_color
    if (fen_parts[1] == "w") {
        turn_ = chess::color::WHITE;
    }else if (fen_parts[1] == "b") {
        turn_ = chess::color::BLACK;
    }

    // fenParts[2] = castling_rights
    castling_rights_ = static_cast<u8>(0);
    if (fen_parts[2].find('k') != string::npos) {
        castling_rights_ |= 1U;
    }
    if (fen_parts[2].find('q') != string::npos) {
        castling_rights_ |= 2U;
    }
    if (fen_parts[2].find('K') != string::npos) {
        castling_rights_ |= 4U;
    }
    if (fen_parts[2].find('Q') != string::npos) {
        castling_rights_ |= 8U;
    }
    // fenParts[3] = enpassant_targets
    if (fen_parts[3] == "-") {
        enpassant_target_ = chess::square::EMPTY;
    }else {
        enpassant_target_ = Square(fen_parts[3]);
    }

    // fenParts[4] = halfmove_clock
    halfmove_count_ = stoi(fen_parts[4]);

    // fenParts[5] = fullmove_number
    fullmove_number_ = stoi(fen_parts[5]);
    ply_count_ = (fullmove_number_ - 1) * 2 + (turn_ == chess::color::BLACK ? 1 : 0);
}

void Board::make_move(Move move) {
    // assuming the generated/uci-given move is correct

    // incremental updates
    turn_ = !turn_;
    ply_count_++;
    halfmove_count_++;
    if (turn_ == chess::color::WHITE) {
        fullmove_number_++;
    }


    // SET Move flags for special cases
    MoveUtils::set_move_flags(move, *this);

    // note: Board::enpassant_target_ is kept empty ONLY after setting move flags
    enpassant_target_ = Square(chess::square::EMPTY);
    Square starting_square = Square(move.starting_square_.square_);
    Square target_square = Square(move.target_square_.square_);
    Piece moving_piece = Piece(board_[starting_square.square_]);
    Piece captured_piece;
    if (!move.is_capture_) captured_piece = Piece();
    else captured_piece = Piece(board_[target_square.square_]);

    // set irreversible state
    IrreversibleState state;
    state.captured_piece = Piece(captured_piece.piece_type_, moving_piece.piece_color_);
    state.castling_rights = castling_rights_;
    state.enpassant_target = Square(enpassant_target_.square_);
    state.halfmove_count = halfmove_count_;
    state.repetition_count = repetition_count_;

    // special cases for promotion, enpassants & castling
    // note that is_capture_, etc will be set in utils by user
    // and in movgen by engine

    // pawn moves
    if (moving_piece.piece_type_ == chess::piece::PAWN) {
        if (move.is_en_passant_) {
            // remove the captured pawn
            if (moving_piece.piece_color_ == chess::color::WHITE) {
                captured_piece = Piece(board_[target_square.square_ - 8]);
                board_[target_square.square_ - 8] = Piece();
            }else {
                captured_piece = Piece(board_[target_square.square_ + 8]);
                board_[target_square.square_ + 8] = Piece();
            }

        }else if (MoveUtils::is_double_pawn_push(move, *this)){

            if (moving_piece.piece_color_ == chess::color::WHITE) {
                enpassant_target_ = Square(target_square.square_ - 8);
            }else {
                enpassant_target_ = Square(target_square.square_ + 8);
            }
        }
        // promotion white
        else if (moving_piece.piece_color_ == chess::color::WHITE && move.promotion_piece_.piece_type_ != chess::piece::EMPTY) {
            moving_piece.piece_type_ = move.promotion_piece_.piece_type_;
        }

    }

    // king moves
    if (moving_piece.piece_type_ == chess::piece::KING) {
        // castling rights
        // if the king moves, castling rights will be lost for him
        castling_rights_ &= moving_piece.piece_color_ == chess::color::WHITE ? ~12U : ~3U;

        if (move.is_castling_) {
            switch (target_square.square_) {
                case chess::square::G1:
                    board_[chess::square::F1] = Piece('R');
                    board_[chess::square::H1] = Piece();
                    break;
                case chess::square::C1:
                    board_[chess::square::D1] = Piece('R');
                    board_[chess::square::A1] = Piece();
                    break;
                case chess::square::G8:
                    board_[chess::square::F8] = Piece('R');
                    board_[chess::square::H8] = Piece();
                    break;
                case chess::square::C8:
                    board_[chess::square::D8] = Piece('R');
                    board_[chess::square::A8] = Piece();
                    break;
            }
        }
    }else if (moving_piece.piece_type_ == chess::piece::ROOK) {

        if (starting_square.square_ == chess::square::A1 && moving_piece.piece_color_ == chess::color::WHITE) {
            castling_rights_ &= ~8U;
        }else if (starting_square.square_ == chess::square::H1 && moving_piece.piece_color_ == chess::color::WHITE) {
            castling_rights_ &= ~4U;
        }else if (starting_square.square_ == chess::square::A8 && moving_piece.piece_color_ == chess::color::BLACK) {
            castling_rights_ &= ~2U;
        }else if (starting_square.square_ == chess::square::H8 && moving_piece.piece_color_ == chess::color::BLACK) {
            castling_rights_ &= ~1U;
        }
    }

    // update castling rights if rooks get captured
    if (captured_piece.piece_type_ == chess::piece::ROOK && target_square.square_ == chess::square::A1) {
        castling_rights_ &= ~8U;
    }else if (captured_piece.piece_type_ == chess::piece::ROOK && target_square.square_ == chess::square::H1) {
        castling_rights_ &= ~4U;
    }else if (captured_piece.piece_type_ == chess::piece::ROOK && target_square.square_ == chess::square::A8) {
        castling_rights_ &= ~2U;
    }else if (captured_piece.piece_type_ == chess::piece::ROOK && target_square.square_ == chess::square::H8) {
        castling_rights_ &= ~1U;
    }


    // make the move on the board finally
    irreversible_state_stack_.push(state);
    move_stack_.push(move);
    board_[starting_square.square_] = Piece();
    board_[target_square.square_] = moving_piece;
}

void Board::unmake_move() {
    // incremental updates
    turn_ = !turn_;
    ply_count_--;
    halfmove_count_--;
    if (turn_ == chess::color::BLACK) {
        fullmove_number_--;
    }

    // irreversible state
    IrreversibleState state = irreversible_state_stack_.top();
    irreversible_state_stack_.pop();
    Move move = move_stack_.top();
    move_stack_.pop();

    Square starting_square = Square(move.starting_square_.square_);
    Square target_square = Square(move.target_square_.square_);
    Piece moving_piece = Piece(board_[target_square.square_]);
    Piece captured_piece = state.captured_piece;

    // restore irreversible state
    castling_rights_ = state.castling_rights;
    enpassant_target_ = Square(state.enpassant_target.square_);
    halfmove_count_ = state.halfmove_count;
    repetition_count_ = state.repetition_count;


    // handle enpassants & captures
    if (move.is_capture_ && move.is_en_passant_) {
        if (moving_piece.piece_color_ == chess::color::WHITE) {
            board_[target_square.square_ - 8] = captured_piece;
        }else {
            board_[target_square.square_ + 8] = captured_piece;
        }
    }else if (move.is_capture_) {
        board_[target_square.square_] = captured_piece;
    }else {
        // for regular moves just empty the target square;
        board_[target_square.square_] = Piece();
    }

    // handle castling
    if (move.is_castling_) {
        if (move.target_square_.square_ == chess::square::C1) {
            board_[chess::square::D1] = Piece();
            board_[chess::square::A1] = Piece('R');
        }else if (move.target_square_.square_ == chess::square::G1) {
            board_[chess::square::F1] = Piece();
            board_[chess::square::H1] = Piece('R');
        }else if (move.target_square_.square_ == chess::square::C8) {
            board_[chess::square::D8] = Piece();
            board_[chess::square::A8] = Piece('r');
        }else if (move.target_square_.square_ == chess::square::G8) {
            board_[chess::square::F8] = Piece();
            board_[chess::square::H8] = Piece('r');
        }
    }


    // put moving piece back to its original place
    board_[starting_square.square_] = moving_piece;
}


void Board::reset_board() {
    setup_using_fen();
}

void Board::print_board() {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            cout<<board_[(7-i)*8+j].get_piece_notation()<<" ";
        }
        cout<<endl;
    }
}



