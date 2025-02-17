
add_library(vcore_lib
    simulation.cpp
)

target_include_directories(vcore_lib PRIVATE
    ${VCORE_DIR}
    ${VERILATOR_INCLUDE_DIRS}
)

target_link_libraries(vcore_lib PUBLIC
    vcore_verilator_lib
)

find_package(GTest REQUIRED)

add_executable(vcore_test
    core_sim.cpp
)

target_link_libraries(vcore_test
    vcore_lib
    GTest::gtest
)
add_dependencies(vcore_test
    turtle_test_bins
)

add_test(NAME vcore_test COMMAND vcore_test)

