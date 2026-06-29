cmake_minimum_required(VERSION 3.15)

set(EngineName "sylvie")
project(${EngineName} VERSION 1.0)

# Set compiler options
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ==========================================
# --- PERFORMANCE OPTIMIZATIONS ---
# ==========================================

# 1. Enable Link Time Optimization (LTO) / Interprocedural Optimization (IPO) globally
cmake_policy(SET CMP0069 NEW)
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported)
if(ipo_supported)
    message(STATUS "IPO / LTO enabled for maximum performance.")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

# 2. Aggressive Compiler Flags for Release Mode
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        # -O3: Maximum optimization
        # -march=native: Unlocks hardware POPCNT and BMI2 instructions (CRITICAL for Bitboards)
        # -DNDEBUG: Disables assertions
        add_compile_options(-O3 -march=native -DNDEBUG)
    elseif(MSVC)
        # /O2: Max speed, /Oi: Intrinsic functions, /Ot: Favor fast code
        add_compile_options(/O2 /Oi /Ot /arch:AVX2 /DNDEBUG)
    endif()
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -O0)
endif()
# ==========================================


add_executable(${EngineName}
        src/main.cpp
        src/bot.cpp
        src/uci.cpp
        include/sylvie/bot.h
        include/sylvie/uci.h
)

target_include_directories(${EngineName} PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/${EngineName}/
        ${CMAKE_SOURCE_DIR}/include/${EngineName}/core/
        ${CMAKE_SOURCE_DIR}/include/${EngineName}/core/board/
)

# Linking core and debug libraries
add_subdirectory(src/core)
target_link_libraries(${EngineName} PUBLIC core)


# Tests
add_executable(tests
        src/tests/perft.cpp
        src/bot.cpp
        src/uci.cpp
)

target_include_directories(tests PUBLIC
        ${CMAKE_SOURCE_DIR}/include/${EngineName}/
)

target_link_libraries(tests PRIVATE core)
target_compile_definitions(tests PRIVATE TESTING)

#if (CMAKE_BUILD_TYPE STREQUAL "Debug")
#    message(STATUS "Building with AddressSanitizer")
#
#    target_compile_options(${EngineName} PUBLIC -fsanitize=address -fno-omit-frame-pointer)
#    target_link_options(${EngineName} PUBLIC -fsanitize=address)
#
#    target_compile_options(tests PUBLIC -fsanitize=address -fno-omit-frame-pointer)
#    target_link_options(tests PUBLIC -fsanitize=address)
#endif()