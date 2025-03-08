`timescale 1ns / 1ns

module execute_fence(
    input var [6:0]decode_opcode,
    input var [2:0]decode_funct3,
    input var [6:0]decode_funct7,
    input read_valid,

    output processing,
    output valid
    );

    localparam MISC_MEM_OPCODE = 'b0001111;
    localparam FENCE_FUNCT = 'b000;

    // FENCE is a no-op
    assign processing = read_valid &&
                        (decode_opcode == MISC_MEM_OPCODE) &&
                        (decode_funct3 == FENCE_FUNCT);
    assign valid = processing;

endmodule


