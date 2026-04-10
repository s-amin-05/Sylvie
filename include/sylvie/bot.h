#pragma once
#include <board.h>
#include <movegen.h>
#include <search.h>

class Engine {

    std::string name_ = "Sylvie";
    std::string author_ = "Amin Solkar";

    bool debug_flag_;
    Board board_;
    std::string fen_string_;
    // put it in search
    MoveGenerator move_generator_;
    Searcher searcher_;
    int depth_;



    // TODO: put them in search
    int time_remaining_black_, time_remaining_white_;
    int time_increment_black_, time_increment_white_;


    void search_loop();

public:
    // initialize MoveGenerator, debug flag as false
    Engine();

    void print_engine_info() const;

    void set_debug_flag(bool flag);
    bool get_debug_flag() const;

    void clear_engine_state();

    void setup_board(const std::string &fen_string);

    void make_move(Move &move);

    void start_search();
    void stop_search();
    float get_evaluation();

    void set_times(int time_remaining_white, int time_remaining_black, int time_increment_white, int time_increment_black);



};