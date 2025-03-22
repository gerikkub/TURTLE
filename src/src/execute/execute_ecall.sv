`timescale 1ns / 1ns

module execute_ecall(
    input var [6:0]decode_opcode,
    input var [2:0]decode_funct3,
    input var [6:0]decode_funct7,
    input var [11:0]decode_imm,
    input read_valid,

    output processing,
    output valid,
    output [5:0]exception_num_out,
    output exception_valid_out,
    output exception_return_valid_out
    );

    localparam SYSTEM_OPCODE = 'b1110011;
    localparam PRIV_FUNCT3 = 'b000;
    localparam ECALL_IMM = 'b00000000000;
    localparam EBREAK_IMM = 'b00000000001;
    localparam WFI_IMM = 'b000100000101;
    localparam MRET_IMM = 'b001100000010;

    localparam EXCEPTION_ECALL_M = 'd11;
    localparam EXCEPTION_EBREAK = 'd3;

    assign processing = read_valid &&
                        (decode_opcode == SYSTEM_OPCODE) &&
                        (decode_funct3 == PRIV_FUNCT3) &&
                        ((decode_imm == ECALL_IMM) ||
                         (decode_imm == EBREAK_IMM) ||
                         (decode_imm == WFI_IMM) ||
                         (decode_imm == MRET_IMM));
    assign valid = processing;

    assign exception_valid_out = (decode_imm == ECALL_IMM) ||
                                 (decode_imm == EBREAK_IMM);
    assign exception_num_out = (decode_imm == ECALL_IMM) ?
                                    EXCEPTION_ECALL_M :
                               (decode_imm == EBREAK_IMM) ?
                                    EXCEPTION_EBREAK :
                                    'd0;

    assign exception_return_valid_out = processing &&
                                        (decode_imm == MRET_IMM);

endmodule



