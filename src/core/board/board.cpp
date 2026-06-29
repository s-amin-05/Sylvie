#include <utils/utils.h>
#include <iostream>
#include <piece.h>
#include <square.h>
#include <board.h>
#include <logger.h>

Board::Board(): Board(chess::starting_pos_fen) {}

// void validate_piece_index_board(Board& board) {
//     for (int sq = 0; sq < 64; ++sq) {
//         const Piece& piece = board.board_[sq];
//         if (piece.piece_type_ != chess::piece::EMPTY) {
//             int index = board.piece_index_board_[sq];
//             if (index == -1) {
//                 std::cerr << "[CORRUPTION] Tracked piece at "
//                           << Square(sq).get_square_notation()
//                           << " has invalid index!" << std::endl;
//                 board.print_board();
//                 std::abort();
//             }
//         }
//     }
// }

Board::Board(const std::string &position_fen):
    turn_(chess::color::WHITE),
    castling_rights_(0xF),
    enpassant_target_(chess::square::EMPTY),
    ply_count_(0),
    fullmove_number_(1),
    repetition_count_(0),
    halfmove_count_(0),
    // piece_index_board_{-1},
    piece_bitboard_{0ULL},
    // piece_count_(12, 0),
    captured_piece_(chess::piece_type::EMPTY),
    logger_("board.log")
{
    board_fen_ = position_fen;

    // Initialize board array to empty & index board to -1
    for (int sq = 0; sq < 64; sq++) {
        board_[sq] = chess::piece_type::EMPTY;
    }
    occupancy_white_ = 0ULL;
    occupancy_black_ = 0ULL;

    BitboardUtils::compute_attack_tables(*this);
    setup_using_fen();

    logger_.log_to_file("[BOARD INITIALIZED]");
    logger_.log_board_to_file(*this, Move("0000"), false);
}

void Board::setup_using_fen() {

    const std::vector<std::string> fen_parts = Utils::split(board_fen_, ' ');

    // fenParts[0] = piece_placement
    const std::vector<std::string> piece_placement = Utils::split(fen_parts[0], '/');

    for (int i = 0; i < 8; i++) {
        int j=0;
        for (const char c : piece_placement[i]) {
            if (Utils::is_digit(c)) {
                j += c - '0';
            }
            else {
                // place the piece on board
                const int piece = Piece::get_piece_from_notation(c);
                const int piece_type = Piece::type_(piece);
                const int piece_color = Piece::color_(piece);
                const int square = (7-i)*8 + j;
                if (piece_type == chess::piece_type::KING && piece_color == chess::color::WHITE)
                    white_king_square_ = square;
                else if (piece_type == chess::piece_type::KING && piece_color == chess::color::BLACK)
                    black_king_square_ = square;

                BitboardUtils::add_piece_to_bb(*this, piece, square);
                // PieceListUtils::add_piece_to_piece_list(piece, square, piece_lists_, piece_index_board_, piece_count_);

                // PieceCountUtils::increment_piece_count(*this, piece, 1);
                board_[square] = piece;
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
        castling_rights_ |= bitmask::castling::BLACK_KING;
    }
    if (fen_parts[2].find('q') != std::string::npos) {
        castling_rights_ |= bitmask::castling::BLACK_QUEEN;
    }
    if (fen_parts[2].find('K') != std::string::npos) {
        castling_rights_ |= bitmask::castling::WHITE_KING;
    }
    if (fen_parts[2].find('Q') != std::string::npos) {
        castling_rights_ |= bitmask::castling::WHITE_QUEEN;
    }
    // fenParts[3] = enpassant_targets
    if (fen_parts[3] == "-") {
        enpassant_target_ = chess::square::EMPTY;
    }else {
        enpassant_target_ = Square::get_square_from_notation(fen_parts[3]);
    }

    // fenParts[4] = halfmove_clock
    halfmove_count_ = stoi(fen_parts[4]);

    // fenParts[5] = fullmove_number
    fullmove_number_ = stoi(fen_parts[5]);
    ply_count_ = (fullmove_number_ - 1) * 2 + (turn_ == chess::color::BLACK ? 1 : 0);
}


void Board::make_move(Move &move, const bool uci_flag) {
    // assuming the generated/uci-given move is correct

    // incremental updates
    turn_ = (turn_ == chess::color::WHITE) ? chess::color::BLACK : chess::color::WHITE;
    ply_count_++;
    if (turn_ == chess::color::WHITE) {
        fullmove_number_++;
    }

    // set irreversible state here because we need the state of board BEFORE making the move
    IrreversibleState state;
    state.castling_rights = castling_rights_;
    state.enpassant_target = enpassant_target_;
    state.halfmove_count = halfmove_count_;
    state.repetition_count = repetition_count_;

    // SET Move flags for special cases based on board state
    if (uci_flag) MoveUtils::set_move_flags(move, *this);

    // note: Board::enpassant_target_ is kept empty ONLY after setting move flags
    enpassant_target_ = chess::square::EMPTY;
    const int starting_square = move.starting_square_;
    const int target_square = move.target_square_;
    int captured_square = chess::square::EMPTY;
    int moving_piece = board_[starting_square];
    int castling_rook_start_square = chess::square::EMPTY;
    int castling_rook_end_square = chess::square::EMPTY;

    if (move.is_capture_) {
        captured_piece_ = board_[target_square];
        captured_square = target_square;
    }

    int moving_piece_type = Piece::type_(moving_piece);
    int moving_piece_color = Piece::color_(moving_piece);

    // pawn moves
    if (move.is_en_passant_) {
        // remove the captured pawn
        if (moving_piece_color == chess::color::WHITE) {
            captured_square = target_square - 8;
            captured_piece_ = board_[captured_square];
        } else {
            captured_square = target_square + 8;
            captured_piece_ = board_[captured_square];
        }
    } else if (MoveUtils::is_double_pawn_push(move, *this)) {
        if (moving_piece_color == chess::color::WHITE) {
            enpassant_target_ = target_square - 8;
        } else {
            enpassant_target_ = target_square + 8;
        }
    }

    // king moves
    if (moving_piece_type == chess::piece_type::KING) {
        // castling rights
        castling_rights_ &= moving_piece_color == chess::color::WHITE ? ~(bitmask::castling::WHITE_KING | bitmask::castling::WHITE_QUEEN) : ~(bitmask::castling::BLACK_KING | bitmask::castling::BLACK_QUEEN);

        if (move.is_castling_) {
            switch (target_square) {
                case chess::square::G1:
                    castling_rook_start_square = chess::square::H1;
                    castling_rook_end_square = chess::square::F1;
                    break;
                case chess::square::C1:
                    castling_rook_start_square = chess::square::A1;
                    castling_rook_end_square = chess::square::D1;
                    break;
                case chess::square::G8:
                    castling_rook_start_square = chess::square::H8;
                    castling_rook_end_square = chess::square::F8;
                    break;
                case chess::square::C8:
                    castling_rook_start_square = chess::square::A8;
                    castling_rook_end_square = chess::square::D8;
                    break;
                default:
                    break;
            }
            if (castling_rook_start_square != chess::square::EMPTY && castling_rook_end_square != chess::square::EMPTY) {
                int piece = Piece::piece_(chess::piece_type::ROOK, moving_piece_color);
                BitboardUtils::remove_piece_from_bb(*this, piece, castling_rook_start_square);
                BitboardUtils::add_piece_to_bb(*this, piece, castling_rook_end_square);

                // PieceListUtils::update_piece_list(board_[castling_rook_start_square], castling_rook_start_square, castling_rook_end_square, piece_lists_, piece_index_board_);
                board_[castling_rook_start_square] = chess::piece_type::EMPTY;
                board_[castling_rook_end_square] = Piece::piece_(chess::piece_type::ROOK, moving_piece_color);
            }
        }
        if (moving_piece_color == chess::color::WHITE) white_king_square_ = target_square;
        else black_king_square_ = target_square;

    } else if (moving_piece_type == chess::piece_type::ROOK) {
        if (starting_square == chess::square::A1 && moving_piece_color == chess::color::WHITE) {
            castling_rights_ &= ~bitmask::castling::WHITE_QUEEN;
        } else if (starting_square == chess::square::H1 && moving_piece_color == chess::color::WHITE) {
            castling_rights_ &= ~bitmask::castling::WHITE_KING;
        } else if (starting_square == chess::square::A8 && moving_piece_color == chess::color::BLACK) {
            castling_rights_ &= ~bitmask::castling::BLACK_QUEEN;
        } else if (starting_square == chess::square::H8 && moving_piece_color == chess::color::BLACK) {
            castling_rights_ &= ~bitmask::castling::BLACK_KING;
        }
    }

    int captured_piece_type = Piece::type_(captured_piece_);
    // update castling rights if rooks get captured
    if (captured_piece_type == chess::piece_type::ROOK && target_square == chess::square::A1) {
        castling_rights_ &= ~bitmask::castling::WHITE_QUEEN;
    } else if (captured_piece_type == chess::piece_type::ROOK && target_square == chess::square::H1) {
        castling_rights_ &= ~bitmask::castling::WHITE_KING;
    } else if (captured_piece_type == chess::piece_type::ROOK && target_square == chess::square::A8) {
        castling_rights_ &= ~bitmask::castling::BLACK_QUEEN;
    } else if (captured_piece_type == chess::piece_type::ROOK && target_square == chess::square::H8) {
        castling_rights_ &= ~bitmask::castling::BLACK_KING;
    }

    // make the move on the board finally

    // captured piece is set down because we need it for unmaking move
    state.captured_piece = captured_piece_;

    // FAST ARRAY PUSH (Replaces std::stack)
    irreversible_history_[history_ply_] = state;
    move_history_[history_ply_] = move;
    history_ply_++;

    BitboardUtils::remove_piece_from_bb(*this, moving_piece, starting_square);
    // PieceListUtils::remove_piece_from_piece_list(moving_piece, starting_square, piece_lists_, piece_index_board_, piece_count_);
    board_[starting_square] = chess::piece_type::EMPTY;

    if (captured_square != chess::square::EMPTY) {
        BitboardUtils::remove_piece_from_bb(*this, captured_piece_, captured_square);
        // PieceListUtils::remove_piece_from_piece_list(captured_piece_, captured_square, piece_lists_, piece_index_board_, piece_count_);
        board_[captured_square] = chess::piece_type::EMPTY;
    }

    // handle promotions here
    if (move.promotion_piece_ != chess::piece_type::EMPTY) {
        BitboardUtils::add_piece_to_bb(*this, move.promotion_piece_, target_square);
        // PieceListUtils::add_piece_to_piece_list(move.promotion_piece_, target_square, piece_lists_, piece_index_board_, piece_count_);
        board_[target_square] = move.promotion_piece_;
    } else {
        BitboardUtils::add_piece_to_bb(*this, moving_piece, target_square);
        // PieceListUtils::add_piece_to_piece_list(moving_piece, target_square, piece_lists_, piece_index_board_, piece_count_);
        board_[target_square] = moving_piece;
    }
}

void Board::unmake_move() {
    // Prevent unmaking if there are no moves in history
    if (history_ply_ <= 0) {
        return;
    }

    // FAST ARRAY POP (Replaces std::stack)
    history_ply_--;
    Move move = move_history_[history_ply_];
    IrreversibleState state = irreversible_history_[history_ply_];

    // 1. Revert incremental updates (turns and counters)
    if (turn_ == chess::color::WHITE) {
        fullmove_number_--;
    }

    turn_ = (turn_ == chess::color::WHITE) ? chess::color::BLACK : chess::color::WHITE;
    ply_count_--;

    // 2. Restore irreversible state variables
    castling_rights_ = state.castling_rights;
    enpassant_target_ = state.enpassant_target;
    halfmove_count_ = state.halfmove_count;
    repetition_count_ = state.repetition_count;

    // Safely restore the internal captured_piece_ tracker to the PRIOR turn's state
    if (history_ply_ > 0) {
        captured_piece_ = irreversible_history_[history_ply_ - 1].captured_piece;
    } else {
        captured_piece_ = chess::piece_type::EMPTY;
    }

    const int starting_square = move.starting_square_;
    const int target_square = move.target_square_;
    const int moving_piece_color = turn_;

    // 3. Handle moving piece and promotions
    int moving_piece;
    if (move.promotion_piece_ != chess::piece_type::EMPTY) {
        moving_piece = Piece::piece_(chess::piece_type::PAWN, moving_piece_color);
        BitboardUtils::remove_piece_from_bb(*this, move.promotion_piece_, target_square);
        // PieceListUtils::remove_piece_from_piece_list(move.promotion_piece_, target_square, piece_lists_, piece_index_board_, piece_count_);
    } else {
        moving_piece = board_[target_square];
        BitboardUtils::remove_piece_from_bb(*this, moving_piece, target_square);
        // PieceListUtils::remove_piece_from_piece_list(moving_piece, target_square, piece_lists_, piece_index_board_, piece_count_);
    }

    // Temporarily clear the target square
    board_[target_square] = chess::piece_type::EMPTY;

    // 4. Move the piece back to its starting square
    BitboardUtils::add_piece_to_bb(*this, moving_piece, starting_square);
    // PieceListUtils::add_piece_to_piece_list(moving_piece, starting_square, piece_lists_, piece_index_board_, piece_count_);
    board_[starting_square] = moving_piece;

    // 5. King moves & Castling Reversals
    if (Piece::type_(moving_piece) == chess::piece_type::KING) {
        // Restore king position trackers
        if (moving_piece_color == chess::color::WHITE) {
            white_king_square_ = starting_square;
        } else {
            black_king_square_ = starting_square;
        }

        // If castling, move the rook back to its origin
        if (move.is_castling_) {
            int castling_rook_start_square = chess::square::EMPTY;
            int castling_rook_end_square = chess::square::EMPTY;

            switch (target_square) {
                case chess::square::G1:
                    castling_rook_start_square = chess::square::H1;
                    castling_rook_end_square = chess::square::F1;
                    break;
                case chess::square::C1:
                    castling_rook_start_square = chess::square::A1;
                    castling_rook_end_square = chess::square::D1;
                    break;
                case chess::square::G8:
                    castling_rook_start_square = chess::square::H8;
                    castling_rook_end_square = chess::square::F8;
                    break;
                case chess::square::C8:
                    castling_rook_start_square = chess::square::A8;
                    castling_rook_end_square = chess::square::D8;
                    break;
            }

            if (castling_rook_start_square != chess::square::EMPTY && castling_rook_end_square != chess::square::EMPTY) {
                int piece = Piece::piece_(chess::piece_type::ROOK, moving_piece_color);
                BitboardUtils::remove_piece_from_bb(*this, piece, castling_rook_end_square);
                BitboardUtils::add_piece_to_bb(*this, piece, castling_rook_start_square);

                // PieceListUtils::update_piece_list(board_[castling_rook_end_square], castling_rook_end_square, castling_rook_start_square, piece_lists_, piece_index_board_);
                board_[castling_rook_end_square] = chess::piece_type::EMPTY;
                board_[castling_rook_start_square] = Piece::piece_(chess::piece_type::ROOK, moving_piece_color);
            }
        }
    }

    // 6. Restore captured piece to the board (if applicable)
    if (move.is_capture_) {
        int captured_square = target_square;

        // If en passant, the captured pawn is not on the target square
        if (move.is_en_passant_) {
            if (moving_piece_color == chess::color::WHITE) {
                captured_square = target_square - 8;
            } else {
                captured_square = target_square + 8;
            }
        }

        // Add the captured piece stored in the local state variable back to the board
        BitboardUtils::add_piece_to_bb(*this, state.captured_piece, captured_square);
        // PieceListUtils::add_piece_to_piece_list(state.captured_piece, captured_square, piece_lists_, piece_index_board_, piece_count_);
        board_[captured_square] = state.captured_piece;
    }
}

void Board::reset_board() {
    // board_fen_ = chess::starting_pos_fen;
    for (int sq=0; sq < 64; sq++) {
        board_[sq] = chess::piece_type::EMPTY;
    }
    // piece_count_.clear();
    // piece_count_.resize(12, 0);
    // setup_using_fen();
}

void Board::print_board() const {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            std::cout<<Piece::piece_notation(board_[(7-i)*8+j])<<" ";
        }
        std::cout<<std::endl;
    }
}


void Board::print_bitboards() const {
    static const char *names[12] = {
        "White King",
        "White Queens",
        "White Rooks",
        "White Bishops",
        "White Knights",
        "White Pawns",
        "Black King",
        "Black Queens",
        "Black Rooks",
        "Black Bishops",
        "Black Knights",
        "Black Pawns"
    };

    for (int i = 0; i < 12; i++)
        BitboardUtils::print_single_bitboard(piece_bitboard_[i], names[i]);

    BitboardUtils::print_single_bitboard(occupancy_white_, "White Pieces");
    BitboardUtils::print_single_bitboard(occupancy_black_, "Black Pieces");
}