cmake_minimum_required(VERSION 3.16.0)

set(CMAKE_CXX_STANDARD 20)

project(pulse-deinterleave-with-DPML VERSION 0.0.1 LANGUAGES C CXX)

include_directories(${PROJECT_SOURCE_DIR}/include)

file(COPY data DESTINATION .)
file(COPY config DESTINATION .)


add_subdirectory(./src)
add_subdirectory(./test)

