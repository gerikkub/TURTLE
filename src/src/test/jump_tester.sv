`timescale 1ns / 1ns

`include "execute/execute_jump.sv"
`include "execute/alu.sv"

module jump_tester(
    input var [6:0]decode_opcode,
    input var [2:0]decode_funct3,
    input var [31:0]decode_imm,
    input var [31:0]decode_pc,

    input var [31:0]read_rs1_val,

    input read_valid,

    output [31:0]rd_val_out,
    output [31:0]pc_out,
    output jump_pc,

    output processing,
    output valid,
    output [5:0]exception_num_out,
    output exception_valid_out
    );

    wire [32:0] in_a_jump;
    wire [32:0] in_b_jump;
    wire [4:0] alu_op_jump;
    wire alu_valid_jump;

    wire [32:0] in_a = alu_valid_jump ? in_a_jump : 'd0;
    wire [32:0] in_b = alu_valid_jump ? in_b_jump : 'd0;
    wire [4:0] alu_op = alu_valid_jump ? alu_op_jump : 'd8;

    wire [31:0] alu_out;

    execute_jump ej0(
        .decode_opcode(decode_opcode),
        .decode_funct3(decode_funct3),
        .decode_imm(decode_imm),
        .decode_pc(decode_pc),
        .read_rs1_val(read_rs1_val),
        .read_valid(read_valid),
        .in_a(in_a_jump),
        .in_b(in_b_jump),
        .alu_op(alu_op_jump),
        .alu_valid(alu_valid_jump),
        .alu_result(alu_out),
        .processing(processing),
        .valid(valid),
        .rd_val_out(rd_val_out),
        .pc_out(pc_out),
        .jump_pc_out(jump_pc),
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

