
function(add_module_test)

    set(oneValueArgs TOP)
    set(multiValueArgs SRC)
    cmake_parse_arguments(PARSE_ARGV 0 arg
        "" "${oneValueArgs}" "${multiValueArgs}")

    find_package(GTest REQUIRED)
    set_source_files_properties(
        ${arg_SRC}
        PROPERTIES OBJECT_DEPENDS ${CMAKE_BINARY_DIR}/src/Vinstruction_fetch/Vinstruction_fetch.h
    )

    add_executable(${arg_TOP}_test
        mod_test
        ${arg_SRC}
    )

    target_include_directories(${arg_TOP}_test PRIVATE
        ${VERILATOR_INCLUDE_DIRS}
        ${CMAKE_BINARY_DIR}/src/Vinstruction_fetch/
    )

    target_compile_options(${arg_TOP}_test PRIVATE "-g")

    target_link_libraries(${arg_TOP}_test PRIVATE
        v${arg_TOP}_verilator_lib
        GTest::gtest
    )
    add_dependencies(${arg_TOP}_test
        v${arg_TOP}_verilator_lib
    )

    add_test(NAME ${arg_TOP}_test COMMAND ${arg_TOP}_test)

endfunction()

find_package(PkgConfig REQUIRED)
pkg_check_modules(VERILATOR REQUIRED verilator)

add_library(mod_test
    ${CMAKE_CURRENT_SOURCE_DIR}/mod_test.cpp
)
target_include_directories(mod_test PUBLIC SYSTEM
    ${VERILATOR_INCLUDE_DIRS}
)

add_module_test(TOP instruction_fetch
    SRC ${CMAKE_CURRENT_SOURCE_DIR}/instruction_fetch_test.cpp
)

add_module_test(TOP decode
    SRC ${CMAKE_CURRENT_SOURCE_DIR}/decode.cpp
)

add_module_test(TOP decode_inst
    SRC ${CMAKE_CURRENT_SOURCE_DIR}/decode_inst.cpp
)

add_module_test(TOP read_registers
    SRC ${CMAKE_CURRENT_SOURCE_DIR}/read_registers.cpp
)

add_module_test(TOP reg_file
    SRC ${CMAKE_CURRENT_SOURCE_DIR}/reg_file.cpp
)

add_module_test(TOP alu_tester
    SRC ${CMAKE_CURRENT_SOURCE_DIR}/alu_tester.cpp
)

add_module_test(TOP execute_shift
    SRC ${CMAKE_CURRENT_SOURCE_DIR}/execute_shift.cpp
)

add_module_test(TOP jump_tester
    SRC ${CMAKE_CURRENT_SOURCE_DIR}/jump_tester.cpp
)

add_module_test(TOP branch_tester
    SRC ${CMAKE_CURRENT_SOURCE_DIR}/branch_tester.cpp
)

add_module_test(TOP store_tester
    SRC ${CMAKE_CURRENT_SOURCE_DIR}/store_tester.cpp
)

add_module_test(TOP load_tester
    SRC ${CMAKE_CURRENT_SOURCE_DIR}/load_tester.cpp
)

add_module_test(TOP execute_csr
    SRC ${CMAKE_CURRENT_SOURCE_DIR}/execute_csr.cpp
)

add_module_test(TOP hold
    SRC ${CMAKE_CURRENT_SOURCE_DIR}/hold.cpp
)

add_module_test(TOP execute
    SRC ${CMAKE_CURRENT_SOURCE_DIR}/execute.cpp
)

add_module_test(TOP commit
    SRC ${CMAKE_CURRENT_SOURCE_DIR}/commit.cpp
)

add_module_test(TOP pipeline
    SRC ${CMAKE_CURRENT_SOURCE_DIR}/pipeline.cpp
)



