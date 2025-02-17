cmake_minimum_required(VERSION 3.31)
project(TURTLE)

cmake_policy(SET CMP0163 NEW)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#set_property(GLOBAL PROPERTY GLOBAL_DEPENDS_DEBUG_MODE ON)

set(CMAKE_CXX_VERSION 14)

add_subdirectory(src)

add_subdirectory(tests)
