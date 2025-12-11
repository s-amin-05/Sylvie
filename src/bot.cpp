#include <bot.h>
#include <iostream>

#include <search.h>

Engine::Engine() {
     debug_flag_ = false;
     time_increment_black_ = time_increment_white_ = time_remaining_black_ = time_remaining_white_ = 0;
     move_generator_ = MoveGenerator();
     // TODO: add iterative deepening
     depth_ = 4;
 }

void Engine::print_engine_info() {
     std::cout << "id name " << name_ << std::endl;
     std::cout << "id author " << author_ << std::endl;
 }

void Engine::set_debug_flag(bool flag) {
     debug_flag_ = flag;
 }

bool Engine::get_debug_flag() {
     return debug_flag_;
 }

void Engine::clear_engine_state() {
     board_.reset_board();
     move_generator_ = MoveGenerator();
     Engine();
 }



void Engine::setup_board(const std::string &fen_string) {
     board_.reset_board();
     board_.board_fen_ = fen_string;
     board_.setup_using_fen();
}

void Engine::make_move(Move &move) {
    board_.make_move(move);
}


std::string Engine::search_best_move() {
    searcher_.search_best_move(depth_, board_);
    return searcher_.get_best_move().get_move_notation();
}

float Engine::get_evaluation() {
    return searcher_.get_best_evaluation() / 100.0f;
}

void Engine::set_times(int time_remaining_white, int time_remaining_black, int time_increment_white,
                       int time_increment_black) {
    time_remaining_white_ = time_remaining_white;
    time_remaining_black_ = time_remaining_black;
    time_increment_white_ = time_increment_white;
    time_increment_black_ = time_increment_black;
}






