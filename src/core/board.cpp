#include <bits/stdc++.h>
#include <board.h>
// #include <movegen.h>
using namespace std;
using u64 = uint64_t;
using u8 = uint8_t;


Board::Board(string position_fen)
{
    board_fen_ = position_fen;
    enpassant_target_ = Square(chess::square::EMPTY);
}

Piece::Piece() : piece_(0), piece_type_(0), piece_color_(false) {}

Piece::Piece(u8 piece) {
    piece_ = piece;
    piece_type_ = piece & 0b00000111;
    piece_color_ = piece & (1 << 3);
};

Square::Square(u8 square) {
    square_ = square;
    file_ = square % 8;
    rank_ = square / 8;
}

Square::Square() {
    square_ = 0;
    file_ = 0;
    rank_ = 0;
}
