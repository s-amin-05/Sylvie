#include <bot.h>
#include <iostream>

#include <search.h>

Engine::Engine() {
     debug_flag_ = false;
     evaluation_ = 0;
     time_increment_black_ = time_increment_white_ = time_remaining_black_ = time_remaining_white_ = 0;
     move_generator_ = MoveGenerator();
     best_move_ = Move();
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
     best_move_ = Move();
     move_generator_ = MoveGenerator();
     Engine();
 }



void Engine::setup_board(const std::string &fen_string) {
     board_.board_fen_ = fen_string;
     board_.setup_using_fen();
}

void Engine::make_move(Move &move) {
    board_.make_move(move);
}


void Engine::set_best_eval() {
     Search::minmax_search(depth_, board_, best_move_);
 }

std::string Engine::get_best_move() {
    return best_move_.get_move_notation();
}

float Engine::get_evaluation() {
    return evaluation_ / 100.0f;
}

void Engine::set_times(int time_remaining_white, int time_remaining_black, int time_increment_white,
                       int time_increment_black) {
    time_remaining_white_ = time_remaining_white;
    time_remaining_black_ = time_remaining_black;
    time_increment_white_ = time_increment_white;
    time_increment_black_ = time_increment_black;
}






