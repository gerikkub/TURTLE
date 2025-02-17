

add_custom_target(microcode
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/generate_mc_tables.py ${CMAKE_CURRENT_SOURCE_DIR}/control_signals.json ${CMAKE_CURRENT_SOURCE_DIR}/microcode_descriptor.json
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/metadata_vector.txt
           ${CMAKE_CURRENT_BINARY_DIR}/obcode_vector.txt
           ${CMAKE_CURRENT_BINARY_DIR}/subop_vector.txt
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/generate_mc_tables.py
            ${CMAKE_CURRENT_SOURCE_DIR}/control_signals.json
            ${CMAKE_CURRENT_SOURCE_DIR}/microcode_descriptor.json
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# add_custom_target(microcode)
# add_dependencies(microcode
# ${CMAKE_CURRENT_BINARY_DIR}/metadata_vector.txt
# ${CMAKE_CURRENT_BINARY_DIR}/opcode_vector.txt
# ${CMAKE_CURRENT_BINARY_DIR}/subop_vector.txt
# )

