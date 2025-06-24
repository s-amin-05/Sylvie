#include <utils/utils.h>
#include <bits/stdc++.h>
#include <board.h>
#include <logger.h>

Board::Board(): Board(chess::starting_pos_fen) {}

Board::Board(const std::string &position_fen):
    turn_(chess::color::WHITE),
    castling_rights_(0xF),
    enpassant_target_(chess::square::EMPTY),
    ply_count_(0),
    fullmove_number_(1),
    repetition_count_(0),
    halfmove_count_(0),
    piece_lists_(12),
    // piece_index_board_{-1},
    piece_counts_(12, 0),
    captured_piece_(chess::piece::EMPTY),
    logger_("board.log")
{
    board_fen_ = position_fen;

    // Initialize board array to empty & index board to -1
    for (int i = 0; i < 64; i++) {
        board_[i] = Piece();
        piece_index_board_[i] = -1;
    }

    // Initialize piece-lists to EMPTY Square()
    for (int i = 0; i < 12; i++) {
        switch (i) {
            case chess::piecelists::WHITE_PAWN:
            case chess::piecelists::BLACK_PAWN:
                piece_lists_[i].resize(chess::piecelists::max_count::PAWNS, Square(chess::square::EMPTY));
                break;
            case chess::piecelists::WHITE_KNIGHT:
            case chess::piecelists::BLACK_KNIGHT:
                piece_lists_[i].resize(chess::piecelists::max_count::KNIGHTS, Square(chess::square::EMPTY));
                break;
            case chess::piecelists::WHITE_BISHOP:
            case chess::piecelists::BLACK_BISHOP:
                piece_lists_[i].resize(chess::piecelists::max_count::BISHOPS, Square(chess::square::EMPTY));
                break;
            case chess::piecelists::WHITE_ROOK:
            case chess::piecelists::BLACK_ROOK:
                piece_lists_[i].resize(chess::piecelists::max_count::ROOKS, Square(chess::square::EMPTY));
                break;
            case chess::piecelists::WHITE_QUEEN:
            case chess::piecelists::BLACK_QUEEN:
                piece_lists_[i].resize(chess::piecelists::max_count::QUEENS, Square(chess::square::EMPTY));
                break;
            case chess::piecelists::WHITE_KING:
            case chess::piecelists::BLACK_KING:
                piece_lists_[i].resize(chess::piecelists::max_count::KINGS, Square(chess::square::EMPTY));
                break;
            default:
                break;
        }
    }

    setup_using_fen();

    logger_.log_to_file("[BOARD INITIALIZED]");
    logger_.log_board_to_file(*this, Move("0000"), false);
}

void Board::setup_using_fen() {

    std::vector<std::string> fen_parts = Utils::split(board_fen_, ' ');

    // fenParts[0] = piece_placement
    std::vector<std::string> piece_placement = Utils::split(fen_parts[0], '/');

    for (int i = 0; i < 8; i++) {
        int j=0;
        for (char c : piece_placement[i]) {
            if (Utils::is_digit(c)) {
                j += c - '0';
            }
            else {
                // place the piece on board
                Piece piece = Piece(c);
                Square square = Square((7-i)*8 + j);
                board_[square.square_] = piece;

                // place pieces on respective piece_lists
                int piece_list_type = (int)piece.piece_type_ - 1 + 6*(piece.piece_color_ == chess::color::WHITE ? 0 : 1);
                int piece_count = piece_counts_[piece_list_type];
                int index = piece_count;
                piece_lists_[piece_list_type][index] = Square(square.square_);
                piece_index_board_[square.square_] = index;
                piece_counts_[piece_list_type]++;
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
    if (fen_parts[2].find('k') != std::string::npos) {
        castling_rights_ |= 1U;
    }
    if (fen_parts[2].find('q') != std::string::npos) {
        castling_rights_ |= 2U;
    }
    if (fen_parts[2].find('K') != std::string::npos) {
        castling_rights_ |= 4U;
    }
    if (fen_parts[2].find('Q') != std::string::npos) {
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
    if (turn_ == chess::color::WHITE) {
        fullmove_number_++;
    }


    // SET Move flags for special cases based on board state
    MoveUtils::set_move_flags(move, *this);

    // note: Board::enpassant_target_ is kept empty ONLY after setting move flags
    enpassant_target_ = Square();
    Square starting_square = Square(move.starting_square_.square_);
    Square target_square = Square(move.target_square_.square_);
    Square captured_square = Square();
    Piece moving_piece = Piece(board_[starting_square.square_]);
    Square castling_rook_start_square = Square();
    Square castling_rook_end_square = Square();

    if (move.is_capture_) {
        captured_piece_ = Piece(board_[target_square.square_]);
        captured_square = Square(target_square.square_);
    }


    // special cases for promotion, enpassants & castling
    // note that is_capture_, etc will be set in utils by user
    // and in movgen by engine

    // pawn moves

    if (move.is_en_passant_) {
        // remove the captured pawn
        if (moving_piece.piece_color_ == chess::color::WHITE) {
            captured_square.square_ = target_square.square_ - 8;
            captured_piece_ = Piece(board_[captured_square.square_]);

        }else {
            captured_square.square_ = target_square.square_ + 8;
            captured_piece_ = Piece(board_[captured_square.square_]);
        }

    }else if (MoveUtils::is_double_pawn_push(move, *this)){

        if (moving_piece.piece_color_ == chess::color::WHITE) {
            enpassant_target_ = Square(target_square.square_ - 8);
        }else {
            enpassant_target_ = Square(target_square.square_ + 8);
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
                    castling_rook_start_square = Square(chess::square::H1);
                    castling_rook_end_square = Square(chess::square::F1);
                    break;
                case chess::square::C1:
                    castling_rook_start_square = Square(chess::square::A1);
                    castling_rook_end_square = Square(chess::square::D1);
                    break;
                case chess::square::G8:
                    castling_rook_start_square = Square(chess::square::H8);
                    castling_rook_end_square = Square(chess::square::F8);
                    break;
                case chess::square::C8:
                    castling_rook_start_square = Square(chess::square::A8);
                    castling_rook_end_square = Square(chess::square::D8);
                    break;
            }
            board_[castling_rook_start_square.square_] = Piece();
            board_[castling_rook_end_square.square_] = Piece(chess::piece::ROOK, moving_piece.piece_color_);
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
    if (captured_piece_.piece_type_ == chess::piece::ROOK && target_square.square_ == chess::square::A1) {
        castling_rights_ &= ~8U;
    }else if (captured_piece_.piece_type_ == chess::piece::ROOK && target_square.square_ == chess::square::H1) {
        castling_rights_ &= ~4U;
    }else if (captured_piece_.piece_type_ == chess::piece::ROOK && target_square.square_ == chess::square::A8) {
        castling_rights_ &= ~2U;
    }else if (captured_piece_.piece_type_ == chess::piece::ROOK && target_square.square_ == chess::square::H8) {
        castling_rights_ &= ~1U;
    }

    // update piece-list values
    // capturing and not promotion (also works for enpassants)
    if (move.is_capture_ && move.promotion_piece_.piece_type_ == chess::piece::EMPTY) {
        PieceListUtils::remove_piece_from_piece_list(captured_piece_, captured_square, piece_lists_, piece_index_board_, piece_counts_);
        PieceListUtils::update_piece_list(moving_piece, starting_square, target_square, piece_lists_, piece_index_board_);
    }
    // capturing and promotion
    else if (move.is_capture_ && move.promotion_piece_.piece_type_ != chess::piece::EMPTY) {
        PieceListUtils::remove_piece_from_piece_list(moving_piece, starting_square, piece_lists_, piece_index_board_, piece_counts_);
        PieceListUtils::remove_piece_from_piece_list(captured_piece_, captured_square, piece_lists_, piece_index_board_, piece_counts_);
        PieceListUtils::add_piece_to_piece_list(move.promotion_piece_, target_square, piece_lists_, piece_index_board_, piece_counts_);
    }
    // not capturing and promotion
    else if (!move.is_capture_ && move.promotion_piece_.piece_type_ != chess::piece::EMPTY) {
        PieceListUtils::remove_piece_from_piece_list(moving_piece, starting_square, piece_lists_, piece_index_board_, piece_counts_);
        PieceListUtils::add_piece_to_piece_list(move.promotion_piece_, target_square, piece_lists_, piece_index_board_, piece_counts_);
    }
    // castling (not capturing and not promotion is implied)
    else if (move.is_castling_) {
        PieceListUtils::update_piece_list(moving_piece, starting_square, target_square, piece_lists_, piece_index_board_);
        // move rook
        Piece rook = Piece(chess::piece::ROOK, moving_piece.piece_color_);
        PieceListUtils::update_piece_list(rook, castling_rook_start_square, castling_rook_end_square, piece_lists_, piece_index_board_);
    }else {
        PieceListUtils::update_piece_list(moving_piece, starting_square, target_square, piece_lists_, piece_index_board_);
    }


    // set irreversible state
    IrreversibleState state;
    state.captured_piece = Piece(captured_piece_.piece_type_, captured_piece_.piece_color_);
    state.castling_rights = castling_rights_;
    state.enpassant_target = Square(enpassant_target_.square_);
    state.halfmove_count = halfmove_count_;
    state.repetition_count = repetition_count_;

    // make the move on the board finally
    irreversible_state_stack_.push(state);
    move_stack_.push(move);
    board_[starting_square.square_] = Piece();
    if (captured_square.square_ != chess::square::EMPTY) board_[captured_square.square_] = Piece();
    // handle promotions here
    board_[target_square.square_] = (move.promotion_piece_.piece_type_ == chess::piece::EMPTY) ? moving_piece : Piece(move.promotion_piece_.piece_type_, moving_piece.piece_color_);

    // log board state
    logger_.log_to_file("[MOVE " + move.get_move_notation() + "]");
    logger_.log_board_to_file(*this, move, false);

}

void Board::unmake_move() {
    // incremental updates
    turn_ = !turn_;
    ply_count_--;
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
    captured_piece_ = state.captured_piece;

    // restore irreversible state
    castling_rights_ = state.castling_rights;
    enpassant_target_ = Square(state.enpassant_target.square_);
    halfmove_count_ = state.halfmove_count;
    repetition_count_ = state.repetition_count;


    // handle enpassants & captures
    if (move.is_capture_ && move.is_en_passant_) {
        if (moving_piece.piece_color_ == chess::color::WHITE) {
            board_[target_square.square_ - 8] = captured_piece_;
        }else {
            board_[target_square.square_ + 8] = captured_piece_;
        }
    }else if (move.is_capture_) {
        board_[target_square.square_] = captured_piece_;
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

    // log state after unmake move
    logger_.log_to_file("[UNMAKE MOVE " + move.get_move_notation() + "]");
    logger_.log_board_to_file(*this, move, false);
}


void Board::reset_board() {
    setup_using_fen();
}

void Board::print_board() {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            std::cout<<board_[(7-i)*8+j].get_piece_notation()<<" ";
        }
        std::cout<<std::endl;
    }
}



