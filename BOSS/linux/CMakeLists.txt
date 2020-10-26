cmake_minimum_required(VERSION 3.16)

# Using new features, don't argue.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# I am anal about safety in c++ because reasons
add_compile_options(-Wall -Wextra -Werror)

project("pingpong")

set(BOOST "../../../boost_1_74_0")

include_directories("../../src" "../../include" "${BOOST}/boost")

link_directories(${BOOST}/stage/lib)


add_definitions(-DLINUX )

add_executable(pingponglinux main.cpp )

target_link_libraries(pingponglinux boost_program_options)