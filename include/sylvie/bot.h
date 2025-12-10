#pragma once
#include <board.h>
#include <movegen.h>

class Engine {

    std::string name_ = "Sylvie";
    std::string author_ = "Amin Solkar";

    bool debug_flag_;
    Board board_;
    std::string fen_string_;
    MoveGenerator move_generator_;
    Move best_move_;
    int evaluation_;
    int depth_;
    int time_remaining_black_, time_remaining_white_;
    int time_increment_black_, time_increment_white_;


public:
    // initialize MoveGenerator, debug flag as false
    Engine();

    void print_engine_info();

    void set_debug_flag(bool flag);
    bool get_debug_flag();

    void clear_engine_state();

    void set_fen_string(const std::string &fen_string);
    std::string get_fen_string();

    void setup_board();

    void make_move(Move &move);

    // set best
    void set_best_eval();
    std::string get_best_move();
    float get_evaluation();


    void set_times(int time_remaining_white, int time_remaining_black, int time_increment_white, int time_increment_black);


};