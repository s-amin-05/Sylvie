set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(core 
    board.cpp
    movegen.cpp 
    bitboard.cpp 
    eval.cpp 
    search.cpp 
    utils.cpp 
)

target_include_directories(core PUBLIC
    ${CMAKE_SOURCE_DIR}/include/${EngineName}/core/
)

target_compile_options(core PUBLIC 
    -O3
    # $<$<BOOL:${USE_POPCNT}>:-msse4.2>
)