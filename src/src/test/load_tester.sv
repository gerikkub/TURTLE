`timescale 1ns / 1ns

`include "execute/execute_load.sv"
`include "execute/alu.sv"

module store_tester(
    input clk,
    input reset,
    input flush,

    input var [6:0]decode_opcode,
    input var [2:0]decode_funct3,
    input var [31:0]decode_imm,

    input var [31:0]read_rs1_val,
    input read_valid,

    output [31:0]mem_data_addr,
    output mem_data_addr_valid,
    output [1:0]mem_data_size,
    input [31:0]mem_data_in,
    input mem_data_valid,
    input mem_data_access_fault,

    output processing,
    output valid,
    output [31:0]rd_val,

    output [5:0]exception_num_out,
    output exception_valid_out
    );

    wire [32:0] in_a_load;
    wire [32:0] in_b_load;
    wire [4:0] alu_op_load;
    wire alu_valid_load;

    wire [32:0] in_a = alu_valid_load ? in_a_load : 'd0;
    wire [32:0] in_b = alu_valid_load ? in_b_load : 'd0;
    wire [4:0] alu_op = alu_valid_load ? alu_op_load : 'd8;

    wire [31:0] alu_out;

    execute_load el0(
        .clk(clk),
        .reset(reset),
        .flush(flush),
        .decode_opcode(decode_opcode),
        .decode_funct3(decode_funct3),
        .decode_imm(decode_imm),
        .read_rs1_val(read_rs1_val),
        .read_valid(read_valid),
        .in_a(in_a_load),
        .in_b(in_b_load),
        .alu_op(alu_op_load),
        .alu_valid(alu_valid_load),
        .alu_result(alu_out),
        .mem_data_addr(mem_data_addr),
        .mem_data_addr_valid(mem_data_addr_valid),
        .mem_data_size(mem_data_size),
        .mem_data_in(mem_data_in),
        .mem_data_valid(mem_data_valid),
        .mem_data_access_fault(mem_data_access_fault),
        .processing(processing),
        .valid(valid),
        .rd_val(rd_val),
        .exception_num_out(exception_num_out),
        .exception_valid_out(exception_valid_out));

    alu a0(
        .in_a(in_a),
        .in_b(in_b),
        .alu_op(alu_op),
        .rd_val_out(alu_out),
        .lt(),
        .ltu(),
        .eq());

endmodule


