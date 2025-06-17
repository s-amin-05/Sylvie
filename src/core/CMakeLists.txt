set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(core
    movegen.cpp
    bitboard.cpp
    eval.cpp
    search.cpp
        board/board.cpp
        board/piece.cpp
        board/square.cpp
        board/move.cpp
        utils/utils.cpp
        utils/logger.cpp

)

target_include_directories(core PUBLIC
    ${CMAKE_SOURCE_DIR}/include/${EngineName}/core/
    ${CMAKE_SOURCE_DIR}/include/${EngineName}/core/board/
    ${CMAKE_SOURCE_DIR}/include/${EngineName}/core/utils/
)

target_compile_options(core PUBLIC 
    -O3
    # $<$<BOOL:${USE_POPCNT}>:-msse4.2>
)