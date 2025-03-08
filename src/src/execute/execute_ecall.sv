`timescale 1ns / 1ns

module execute_ecall(
    input var [6:0]decode_opcode,
    input var [2:0]decode_funct3,
    input var [6:0]decode_funct7,
    input read_valid,

    output processing,
    output valid,
    output [5:0]exception_num_out,
    output exception_valid_out
    );

    localparam SYSTEM_OPCODE = 'b1110011;
    localparam PRIV_FUNCT3 = 'b000;
    localparam ECALL_FUNCT7 = 'b0000000;
    localparam EBREAK_FUNCT7 = 'b0000001;

    localparam EXCEPTION_ECALL_M = 'd11;
    localparam EXCEPTION_EBREAK = 'd3;

    assign processing = read_valid &&
                        (decode_opcode == SYSTEM_OPCODE) &&
                        (decode_funct3 == PRIV_FUNCT3) &&
                        ((decode_funct7 == ECALL_FUNCT7) ||
                         (decode_funct7 == EBREAK_FUNCT7));
    assign valid = processing;

    assign exception_valid_out = processing;
    assign exception_num_out = (decode_funct7 == ECALL_FUNCT7) ?
                                    EXCEPTION_ECALL_M :
                               (decode_funct7 == EBREAK_FUNCT7) ?
                                    EXCEPTION_EBREAK :
                                    'd0;

endmodule



