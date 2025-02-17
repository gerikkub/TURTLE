cmake_minimum_required(VERSION 3.31)
project(TURTLE)

cmake_policy(SET CMP0163 NEW)

set(CMAKE_CXX_VERSION 14)

add_subdirectory(tests)

add_subdirectory(src)
