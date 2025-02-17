
add_subdirectory(microcode)

find_package(PkgConfig REQUIRED)
pkg_check_modules(VERILATOR REQUIRED verilator)

file(GLOB CORE_SRC src/*.v)

set(VCORE_DIR ${CMAKE_CURRENT_BINARY_DIR}/Vcore)
set(VERILATOR_FLAGS --trace -CFLAGS "-g -std=c++14 -pthread" -LDFLAGS -lpthread)
message("VCORE: ${VCORE_DIR}")
add_custom_command(
    OUTPUT ${VCORE_DIR}/Vcore.cpp
    OUTPUT ${VCORE_DIR}/Vcore.h
    COMMAND verilator --cc src/core ${VERILATOR_FLAGS} -Mdir ${VCORE_DIR}
    DEPENDS microcode
    DEPENDS ${CORE_SRC}
    VERBATIM
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_command(
    OUTPUT ${VCORE_DIR}/libVcore.a
           ${VCORE_DIR}/libverilated.a
    COMMAND make -f Vcore.mk
    DEPENDS ${VCORE_DIR}/Vcore.cpp
    WORKING_DIRECTORY ${VCORE_DIR}
)

add_custom_target(vcore_verilator_lib_gen DEPENDS
    ${VCORE_DIR}/libVcore.a
    ${VCORE_DIR}/libverilated.a
)

add_library(vcore_verilator_lib INTERFACE)

target_link_libraries(vcore_verilator_lib INTERFACE
    ${VCORE_DIR}/libVcore.a
    ${VCORE_DIR}/libverilated.a
    ${VERILATOR_LIBRARIES}
)
target_include_directories(vcore_verilator_lib INTERFACE
    ${VCORE_DIR}
    ${VERILATOR_INCLUDE_DIRS}
)
target_compile_options(vcore_verilator_lib INTERFACE
    ${VERILATOR_CFLAGS}
)
add_dependencies(vcore_verilator_lib DEPENDS vcore_verilator_lib_gen)

add_subdirectory(vcore_sim)

