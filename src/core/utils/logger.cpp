#include <logger.h>
#include <iostream>
#include <board.h>
#include <constants.h>

#ifndef NDEBUG
    bool debug_flag = true;
#else
    bool debug_flag = false;
#endif

Logger::Logger(const std::string &filename) {
    if (debug_flag) {
        file_.open(filename, std::ios::out | std::ios::trunc);
        if (!file_.is_open()) {
            std::cerr << "Failed to open log file: " << filename << std::endl;
        }
    }

}

void Logger::log_to_file(const std::string &msg) {
    if (!debug_flag) return;
    if (file_.is_open()) {
        file_ << msg << std::endl;
    } else {
        std::cerr << "Log file is not open." << std::endl;
    }
}

void Logger::close_file() {
    if (!debug_flag) return;
    if (file_.is_open()) {
        file_.close();
    }
}

Logger::~Logger() {
    close_file();
}

void Logger::log_board_to_file(Board &board, const Move &move, const bool detailed) {
    if (!debug_flag) return;
    if (file_.is_open()) {

        // print board with pieces
        log_pieces_to_file(board.board_);

        // print piece lists
        // log_piece_list_to_file(board.piece_lists_, board.piece_counts_);

        // print piece index board
        // log_piece_index_board_to_file(board.piece_index_board_);

        // board fen
        file_ << "FEN: \"" << board.board_fen_ << "\"\n\n";

        // board state (incremental)
        log_board_state_to_file(board);

        // irreversible board state
        log_irreversible_state_to_file(board);

        // log move details if any
        if (move.get_move_notation() != chess::move::NO_MOVE) {
            log_to_file("Move Details:-");
            log_move_to_file(move);
            log_to_file("\n");
        }

        if (detailed) {
            // move stack (each move in detail)
            log_move_stack_to_file(board.move_stack_);

            // board state (irreversible stack)
            // log_irreversible_state_stack_to_file(board.irreversible_state_stack_);
        }

        log_to_file(chess::debug::line_seperator);
        
    } else {
        std::cerr << "Log file is not open." << std::endl;
    }
}

void Logger::log_pieces_to_file(int board[]) {
    if (!debug_flag) return;
    file_ << "Pieces on Board:- \n\n";
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            file_ << Piece::piece_notation(board[(7-i)*8+j]) << " ";
        }
        file_ << "\n";
    }
    file_ << "\n";
}

// void Logger::log_piece_list_to_file(std::vector<std::vector<Square>> &piece_lists, std::vector<int> &piece_counts) {
//     if (!debug_flag) return;
//     file_ << "Piece Lists:- \n\n";
//     for (int i= 0; i < 12; i++) {
//         int piece_type = (i%6)+1;
//         bool piece_color = (i/6 == 0? chess::color::WHITE: chess::color::BLACK);
//         Piece piece = Piece(piece_type, piece_color);
//         file_ << piece.get_piece_notation() << " : " << piece_counts[i]<< " { ";
//         for (int j=0; j < piece_counts[i]; j++) {
//             file_ << piece_lists[i][j].get_square_notation() << " ";
//         }
//         file_ << "} \n";
//     }
//     file_ << "\n";
// }
//
// void Logger::log_piece_index_board_to_file(int *piece_index_board) {
//     if (!debug_flag) return;
//     file_ << "Piece Index Board:- \n\n";
//     for (int i=0; i < 8; i++) {
//         for (int j=0; j < 8; j++) {
//             if (piece_index_board[(7-i)*8+j] == -1) {
//                 file_ << ". ";
//             }else {
//                 file_ << piece_index_board[(7-i)*8+j] << " ";
//             }
//         }
//         file_ << "\n";
//     }
//     file_ << "\n";
// }



void Logger::log_board_state_to_file(const Board &board) {
    if (!debug_flag) return;
    file_ << "Incremental Board State:- \n";
    file_ << "Turn: " << (board.turn_ == chess::color::WHITE? "White" : "Black") << "\n";
    file_ << "Ply Count: " << board.ply_count_ << "\n";
    file_ << "Fullmove Number: " << board.fullmove_number_ << "\n\n";
}

void Logger::log_irreversible_state_to_file(const Board &board) {
    if (!debug_flag) return;
    file_ << "Irreversible Board State:- \n";
    // file_ << "Captured Piece: " << board.captured_piece_.get_piece_notation() << "\n";
    file_ << "Castling Rights: " << (board.castling_rights_ & 0xF) << "\n";
    file_ << "En Passant Target: " << Square::square_notation(board.enpassant_target_) << "\n";
    file_ << "Halfmove Count: " << board.halfmove_count_ << "\n";
    file_ << "Repetition Count: " << board.repetition_count_ << "\n\n";
}

void Logger::log_move_to_file(const Move &move) {
    if (!debug_flag) return;
    file_ << "Starting Square: " << Square::square_notation(move.starting_square_) << "\n";
    file_ << "Target Square: " << Square::square_notation(move.target_square_) << "\n";
    file_ << "Promotion Piece: " << Piece::piece_notation(move.promotion_piece_) << "\n";
    file_ << "Is Capture: " << (move.is_capture_? "True": "False") << "\n";
    file_ << "Is En Passant: " << (move.is_en_passant_? "True": "False") << "\n";
    file_ << "Is Castling: " << (move.is_castling_? "True": "False") << "\n";
}

void Logger::log_move_stack_to_file(std::stack<Move> &move_stack) {
    if (!debug_flag) return;
    file_ << "Move Stack: ";
    std::stack<Move> temp_stack;

    // if empty
    if (move_stack.empty()) {
        file_ << "[EMPTY]\n\n";
        return;
    }

    // reverse stack to display moves in order
    while (!move_stack.empty()) {
        Move move = move_stack.top();
        temp_stack.push(move);
        move_stack.pop();
    }

    int i=0;
    while (!temp_stack.empty()) {
        Move move = temp_stack.top();
        file_  << move.get_move_notation() <<" ";

        move_stack.push(move);
        temp_stack.pop();
    }
    file_ << "\n\n";

}


void Logger::log_irreversible_state_stack_to_file(std::stack<IrreversibleState> &irreversible_state_stack) {
    if (!debug_flag) return;
    file_ << "Irreversible State Stack:- \n";
    std::stack<IrreversibleState> temp_stack;

    // if empty
    if (irreversible_state_stack.empty()) {
        file_ << "[EMPTY]\n\n";
        return;
    }

    // reverse stack to display moves in order
    while (!irreversible_state_stack.empty()) {
        IrreversibleState state = irreversible_state_stack.top();
        temp_stack.push(state);
        irreversible_state_stack.pop();
    }

    int i=0;
    while (!temp_stack.empty()) {
        IrreversibleState state = temp_stack.top();
        file_ << "Move " << i++ << ": \n";
        file_ << "Captured Piece: " << Piece::piece_notation(state.captured_piece) << "\n";
        file_ << "Castling Rights: " << (state.castling_rights & 0xF) << "\n";
        file_ << "En Passant Target: " << Square::square_notation(state.enpassant_target) << "\n";
        file_ << "Halfmove Count: " << state.halfmove_count << "\n";
        file_ << "Repetition Count: " << state.repetition_count << "\n";
        irreversible_state_stack.push(state);
        temp_stack.pop();
    }
    file_ << "\n";
}