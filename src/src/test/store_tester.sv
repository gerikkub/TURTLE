`timescale 1ns / 1ns

`include "execute/execute_store.sv"
`include "execute/alu.sv"

module store_tester(
    input var [6:0]decode_opcode,
    input var [2:0]decode_funct3,
    input var [31:0]decode_imm,

    input var [31:0]read_rs1_val,
    input var [31:0]read_rs2_val,

    input read_valid,

    output [31:0]store_addr,
    output [31:0]store_val,
    output [1:0]store_size,
    output store_valid,

    output processing,
    output valid,
    output [5:0]exception_num_out,
    output exception_valid_out
    );

    wire [32:0] in_a_store;
    wire [32:0] in_b_store;
    wire [4:0] alu_op_store;
    wire alu_valid_store;

    wire [32:0] in_a = alu_valid_store ? in_a_store : 'd0;
    wire [32:0] in_b = alu_valid_store ? in_b_store : 'd0;
    wire [4:0] alu_op = alu_valid_store ? alu_op_store : 'd8;

    wire [31:0] alu_out;

    execute_store es0(
        .decode_opcode(decode_opcode),
        .decode_funct3(decode_funct3),
        .decode_imm(decode_imm),
        .read_rs1_val(read_rs1_val),
        .read_rs2_val(read_rs2_val),
        .read_valid(read_valid),
        .in_a(in_a_store),
        .in_b(in_b_store),
        .alu_op(alu_op_store),
        .alu_valid(alu_valid_store),
        .alu_result(alu_out),
        .store_addr(store_addr),
        .store_val(store_val),
        .store_size(store_size),
        .store_valid(store_valid),
        .processing(processing),
        .valid(valid),
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

