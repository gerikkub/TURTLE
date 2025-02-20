
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

add_module_test(TOP instruction_fetch
    SRC ${CMAKE_CURRENT_SOURCE_DIR}/instruction_fetch_test.cpp
)

add_module_test(TOP decode
    SRC ${CMAKE_CURRENT_SOURCE_DIR}/decode.cpp
)

add_module_test(TOP decode_inst
    SRC ${CMAKE_CURRENT_SOURCE_DIR}/decode_inst.cpp
)


