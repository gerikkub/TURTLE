`timescale 1ns / 1ns

`include "execute/execute_alu.sv"
`include "execute/alu.sv"

module alu_tester(
    input var [6:0]decode_opcode,
    input var [2:0]decode_funct3,
    input var [6:0]decode_funct7,
    input var [31:0]decode_imm,
    input var [31:0]decode_pc,

    input var [31:0]read_rs1_val,
    input var [31:0]read_rs2_val,

    input read_valid,

    output [31:0]rd_val_out,
    output lt,
    output ltu,
    output eq,

    output processing,
    output valid);

    wire [32:0] in_a;
    wire [32:0] in_b;
    wire [4:0] alu_op;

    execute_alu ea0(
        .decode_opcode(decode_opcode),
        .decode_funct3(decode_funct3),
        .decode_funct7(decode_funct7),
        .decode_imm(decode_imm),
        .decode_pc(decode_pc),
        .read_rs1_val(read_rs1_val),
        .read_rs2_val(read_rs2_val),
        .read_valid(read_valid),
        .in_a(in_a),
        .in_b(in_b),
        .alu_op(alu_op),
        .processing(processing),
        .valid(valid));

    alu a0(
        .in_a(in_a),
        .in_b(in_b),
        .alu_op(alu_op),
        .rd_val_out(rd_val_out),
        .lt(lt),
        .ltu(ltu),
        .eq(eq));

endmodule
