set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(core
        board/board.cpp
    movegen.cpp 
    bitboard.cpp 
    eval.cpp 
    search.cpp 
    utils.cpp
        board/piece.cpp
        board/square.cpp
        board/move.cpp
)

target_include_directories(core PUBLIC
    ${CMAKE_SOURCE_DIR}/include/${EngineName}/core/
    ${CMAKE_SOURCE_DIR}/include/${EngineName}/core/board/
)

target_compile_options(core PUBLIC 
    -O3
    # $<$<BOOL:${USE_POPCNT}>:-msse4.2>
)