#include <cctype>
#include <piece.h>

char Piece::get_piece_notation() const{
    if (piece_type_ == chess::piece::EMPTY) return '.';

    char symbols[] = {'.', 'K', 'Q', 'R', 'B', 'N', 'P'};
    char symbol = symbols[piece_type_];

    return piece_color_ ? symbol : tolower(symbol);

}

Piece::Piece() : piece_type_(0U), piece_color_(0U) {}

Piece::Piece(u8 piece_type, bool piece_color) {
    piece_type_ = piece_type;
    piece_color_ = piece_color;
}

Piece::Piece(char piece_notation) {

    piece_color_ = isupper(piece_notation);
    piece_notation = toupper(piece_notation);
    switch (piece_notation) {
        case 'P':
            piece_type_ = chess::piece::PAWN;
            break;
        case 'N':
            piece_type_ = chess::piece::KNIGHT;
            break;
        case 'B':
            piece_type_ = chess::piece::BISHOP;
            break;
        case 'R':
            piece_type_ = chess::piece::ROOK;
            break;
        case 'Q':
            piece_type_ = chess::piece::QUEEN;
            break;
        case 'K':
            piece_type_ = chess::piece::KING;
            break;
        default:
            piece_type_ = chess::piece::EMPTY;
            break;
    }

}