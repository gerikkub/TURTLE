`timescale 1ns / 1ns

module execute(
    input clk,
    input reset,
    input flush,

    input var [6:0]deocde_opcode,
    input var [4:0]decode_rd,
    input var [4:0]decode_rs2,
    input var [2:0]decode_funct3,
    input var [6:0]decode_funct7,
    input var [31:0]decode_imm,
    input var [31:0]decode_pc,

    input var [31:0]read_rs1_val,
    input var [31:0]read_rs2_val,

    input read_valid,
    input [5:0]read_exception_num,
    input read_exception_valid,

    output valid,
    input stall, 
    output [4:0]rd_out,
    output [31:0]rd_val_out,
    output [31:0]exception_num_out,
    output exception_valid_out
    );


endmodule

