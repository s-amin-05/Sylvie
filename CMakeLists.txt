# Set compiler options
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(EngineName "sylvie")
# option(USE_POPCNT "Use POPCNT instructions" ON)



cmake_minimum_required(VERSION 3.15)
project(${EngineName} VERSION 1.0)
add_executable(${EngineName} src/main.cpp)

# add executable for custom gui (maybe)


target_include_directories(${EngineName} PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/${EngineName}/
    ${CMAKE_SOURCE_DIR}/include/${EngineName}/core/
    ${CMAKE_SOURCE_DIR}/include/${EngineName}/core/board/
)

target_compile_options(${EngineName} PUBLIC 
    -O3
    # $<$<BOOL:${USE_POPCNT}>:-msse4.2>
)

# Linking core and debug libraries
add_subdirectory(src/core)
target_link_libraries(${EngineName} PUBLIC core)


 # Tests
add_executable(tests
    src/tests/perft.cpp
)

target_include_directories(tests PUBLIC
    ${CMAKE_SOURCE_DIR}/include/${EngineName}/tests/
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