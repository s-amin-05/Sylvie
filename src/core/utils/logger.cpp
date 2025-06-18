#include <logger.h>
#include <iostream>
#include <board.h>
#include <constants.h>

Logger::Logger(std::string filename) {
    file_.open(filename, std::ios::out | std::ios::trunc);
    if (!file_.is_open()) {
        std::cerr << "Failed to open log file: " << filename << std::endl;
    }
}

void Logger::log_to_file(std::string msg) {
    if (file_.is_open()) {
        file_ << msg << std::endl;
    } else {
        std::cerr << "Log file is not open." << std::endl;
    }
}

void Logger::close_file() {
    if (file_.is_open()) {
        file_.close();
    }
}

Logger::~Logger() {
    close_file();
}

void Logger::log_board_to_file(Board &board, Move move, bool detailed) {
    if (file_.is_open()) {

        // print board with pieces
        log_pieces_to_file(board.board_);

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
            log_irreversible_state_stack_to_file(board.irreversible_state_stack_);
        }

        log_to_file(chess::debug::line_seperator);
        
    } else {
        std::cerr << "Log file is not open." << std::endl;
    }
}

void Logger::log_pieces_to_file(Piece board[]) {
    file_ << "Pieces on Board:- \n\n";
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            file_ << board[(7-i)*8+j].get_piece_notation() << " ";
        }
        file_ << "\n";
    }
    file_ << "\n";
}

void Logger::log_board_state_to_file(Board &board) {
    file_ << "Incremental Board State:- \n";
    file_ << "Turn: " << (board.turn_ == chess::color::WHITE? "White" : "Black") << "\n";
    file_ << "Ply Count: " << board.ply_count_ << "\n";
    file_ << "Fullmove Number: " << board.fullmove_number_ << "\n\n";
}

void Logger::log_irreversible_state_to_file(Board &board) {

    file_ << "Irreversible Board State:- \n";
    file_ << "Captured Piece: " << board.captured_piece_.get_piece_notation() << "\n";
    file_ << "Castling Rights: " << (board.castling_rights_ & 0xF) << "\n";
    file_ << "En Passant Target: " << board.enpassant_target_.get_square_notation() << "\n";
    file_ << "Halfmove Count: " << board.halfmove_count_ << "\n";
    file_ << "Repetition Count: " << board.repetition_count_ << "\n\n";
}

void Logger::log_move_to_file(Move move) {
    file_ << "Starting Square: " << move.starting_square_.get_square_notation() << "\n";
    file_ << "Target Square: " << move.target_square_.get_square_notation() << "\n";
    file_ << "Promotion Piece: " << move.promotion_piece_.get_piece_notation() << "\n";
    file_ << "Is Capture: " << (move.is_capture_? "True": "False") << "\n";
    file_ << "Is En Passant: " << (move.is_en_passant_? "True": "False") << "\n";
    file_ << "Is Castling: " << (move.is_castling_? "True": "False") << "\n";
}

void Logger::log_move_stack_to_file(std::stack<Move> &move_stack) {
    file_ << "Move Stack:- \n";
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
        file_ << "Move " << i++ << ":\n";
        log_move_to_file(move);
        move_stack.push(move);
        temp_stack.pop();
    }
    file_ << "\n";

}

void Logger::log_irreversible_state_stack_to_file(std::stack<IrreversibleState> &irreversible_state_stack) {
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
        file_ << "Captured Piece: " << state.captured_piece.get_piece_notation() << "\n";
        file_ << "Castling Rights: " << (state.castling_rights & 0xF) << "\n";
        file_ << "En Passant Target: " << state.enpassant_target.get_square_notation() << "\n";
        file_ << "Halfmove Count: " << state.halfmove_count << "\n";
        file_ << "Repetition Count: " << state.repetition_count << "\n";
        irreversible_state_stack.push(state);
        temp_stack.pop();
    }
    file_ << "\n";
}