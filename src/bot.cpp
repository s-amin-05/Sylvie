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

void Engine::print_engine_info() const {
     std::cout << "id name " << name_ << std::endl;
     std::cout << "id author " << author_ << std::endl;
}

void Engine::set_debug_flag(const bool flag) {
     debug_flag_ = flag;
 }

bool Engine::get_debug_flag() const {
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

void Engine::make_move(const Move &move) {
    board_.make_move(move);
}

void Engine::start_search() {
    searcher_.stop_search_ = false;

    // Prevent multiple threads
    if (searcher_.search_thread_.joinable())
        searcher_.search_thread_.join();

    // Launch thread
    searcher_.search_thread_ = std::thread([&]() {
        search_loop();
    });
}

void Engine::stop_search() {
    searcher_.stop_search_ = true;

    if (searcher_.search_thread_.joinable())
        searcher_.search_thread_.join();
}

void Engine::search_loop() {
    searcher_.search_best_move(depth_, board_);

    if (searcher_.stop_search_)
        std::cout << "info string search stopped early\n";
    std::cout << "bestmove " << searcher_.get_best_move().get_move_notation() << std::endl;
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






