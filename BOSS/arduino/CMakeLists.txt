cmake_minimum_required(VERSION 3.16)

# Using new features, don't argue.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(CMAKE_C_COMPILER avr-gcc)
SET(CMAKE_CXX_COMPILER avr-g++)

# I am anal about safety in c++ because reasons
add_compile_options(-Wall -Wextra -Werror)

project("pingpong")

include_directories("../../src")

#add_definitions(-DLINUX)

add_executable(pingpongarduino main.cpp)