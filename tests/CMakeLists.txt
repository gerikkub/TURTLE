
include(ExternalProject)

ExternalProject_Add(turtle_test_bins_proj
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/c
    INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}
    CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${CMAKE_CURRENT_SOURCE_DIR}/c/riscv32-toolchain.cmake
               -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_target(turtle_test_bins DEPENDS turtle_test_bins_proj)
