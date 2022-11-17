cmake_minimum_required(VERSION 3.16.0)

set(CMAKE_CXX_STANDARD 20)

project(pulse-deinterleave-with-DPML VERSION 0.0.1 LANGUAGES C CXX)

include_directories(${PROJECT_SOURCE_DIR})
add_executable(mldp_a2 mldp_a2.cpp)
target_link_libraries(mldp_a2 pthread)

add_executable(mldp_a2_for_miss mldp_a2_for_miss.cpp)
target_link_libraries(mldp_a2_for_miss pthread)

add_executable(mldp_a2_for_spurious mldp_a2_for_spurious.cpp)
target_link_libraries(mldp_a2_for_spurious pthread)

add_executable(mldp_a2_for_miss_sliding mldp_a2_for_miss_sliding.cpp)
target_link_libraries(mldp_a2_for_miss_sliding pthread)

add_executable(mldp_A4 mldp_A4.cpp)
target_link_libraries(mldp_A4 pthread)

add_executable(mldp_A4_spurious mldp_A4_spurious.cpp)
target_link_libraries(mldp_A4_spurious pthread)

add_executable(mldp_A4_spurious_A3 mldp_A4_spurious_A3.cpp)
target_link_libraries(mldp_A4_spurious_A3 pthread)

add_executable(mldp_A4_miss mldp_A4_miss.cpp)
target_link_libraries(mldp_A4_miss pthread)