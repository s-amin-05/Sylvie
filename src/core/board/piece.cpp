#include <piece.h>

int Piece::type_(const int piece) {
    return piece & bitmask::piece::PIECE_TYPE;
}

int Piece::color_(const int piece) {
    return (piece & bitmask::piece::PIECE_COLOR) >> 3;
}

int Piece::piece_(const int type, const int color) {
    return (color << 3) | type;
}

char Piece::piece_notation(int piece) {
    int color = color_(piece);
    int type = type_(piece);

    char symbols[] = {'.', 'K', 'Q', 'R', 'B', 'N', 'P'};
    char symbol = symbols[type];

    return (color == chess::color::WHITE)  ? symbol : std::tolower(symbol);
}

int Piece::get_piece_from_notation(char piece_notation) {
    int piece_color = isupper(piece_notation) ? chess::color::WHITE : chess::color::BLACK;
    int piece_type;
    piece_notation = toupper(piece_notation);
    switch (piece_notation) {
        case 'P':
            piece_type = chess::piece_type::PAWN;
            break;
        case 'N':
            piece_type = chess::piece_type::KNIGHT;
            break;
        case 'B':
            piece_type = chess::piece_type::BISHOP;
            break;
        case 'R':
            piece_type = chess::piece_type::ROOK;
            break;
        case 'Q':
            piece_type = chess::piece_type::QUEEN;
            break;
        case 'K':
            piece_type = chess::piece_type::KING;
            break;
        default:
            piece_type = chess::piece_type::EMPTY;
            break;
    }
    return (piece_color << 3) | piece_type;
}
