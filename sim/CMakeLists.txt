
find_package(PkgConfig REQUIRED)
pkg_check_modules(VERILATOR REQUIRED verilator)

find_package(Boost REQUIRED COMPONENTS
    program_options
)

add_executable(turtle_sim
    "${CMAKE_CURRENT_LIST_DIR}/src/main.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/dut.cpp"
)

target_compile_options(turtle_sim PRIVATE "-g")

target_include_directories(turtle_sim PRIVATE
    ${VERILATOR_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIR}
)

target_link_libraries(turtle_sim
    vpipeline_verilator_lib
    ${Boost_LIBRARIES}
    elfio::elfio
)

