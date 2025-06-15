#include <board.h>
#include <bits/stdc++.h>
#include <utils.h>
using namespace std;

Board::Board(): Board(chess::starting_pos_fen) {}

Board::Board(string position_fen):
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


    Piece moving_piece = board_[move.starting_square_.square_];
    turn_ = !turn_;
    ply_count_++;
    halfmove_count_++;
    if (turn_ == chess::color::WHITE) {
        fullmove_number_++;
    }


    // set move flags for special cases
    MoveUtils::set_move_flags(move, *this);

    // special cases for captures, enpassants & castling
    // note that is_capture_, etc will be set in utils by user
    // and in movgen by engine

    // pawn moves
    if (moving_piece.piece_type_ == chess::piece::PAWN) {
        if (move.is_en_passant_) {
            // remove the captured pawn
            if (moving_piece.piece_color_ == chess::color::WHITE) {
                board_[move.target_square_.square_ - 8] = Piece();
            }else {
                board_[move.target_square_.square_ + 8] = Piece();
            }

            // reset enpassant target after enpassant
            enpassant_target_ = Square(chess::square::EMPTY);

        }else if (MoveUtils::is_double_pawn_push(move, *this)){

            if (moving_piece.piece_color_ == chess::color::WHITE) {
                enpassant_target_ = Square(move.target_square_.square_ - 8);
            }else {
                enpassant_target_ = Square(move.target_square_.square_ + 8);
            }
        }else {
            // reset enpassant target for single pawn push
            enpassant_target_ = Square(chess::square::EMPTY);
        }
    }else {
        // reset enpassant target for non pawn moves
        enpassant_target_ = Square(chess::square::EMPTY);
    }


    if (move.is_castling_) {
        if (move.target_square_.square_ == chess::square::G1) {
            board_[chess::square::F1] = Piece('R');
            board_[chess::square::H1] = Piece();
            // castling_rights_ &= ;
        }
        switch (move.target_square_.square_) {
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

    // make the move on the board finally
    move_stack_.push(move);
    board_[move.starting_square_.square_] = Piece();
    board_[move.target_square_.square_] = moving_piece;
}

void Board::unmake_move() {

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



