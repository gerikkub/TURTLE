`timescale 1ns / 1ns

`include "pipeline/instruction_fetch.sv"
`include "pipeline/decode.sv"
`include "pipeline/read_registers.sv"
`include "pipeline/execute.sv"
`include "pipeline/commit.sv"
`include "reg_file.sv"


module pipeline(
    input clk,
    input reset,

    // Instruction Bus
    output [31:0] mem_fetch_addr,
    output mem_fetch_addr_en,
    input [31:0] mem_inst_in,
    input mem_inst_valid,
    input mem_inst_access_fault,

    // Data Bus
    input datafifo_full,
    output [31:0]datafifo_addr_out,
    output [31:0]datafifo_val_out,
    output [1:0]datafifo_size_out,
    output datafifo_valid_out
    );


    wire pipeline_flush;
    wire [31:0]commit_jump_pc;


    wire fetch_jump_pc = pipeline_flush;
    wire [31:0]fetch_jump_pc_addr = commit_jump_pc;

    wire fetch_valid;
    wire [31:0]fetch_inst;
    wire [31:0]fetch_inst_pc;
    wire [5:0]fetch_exception_num;
    wire fetch_exception_valid;
    instruction_fetch fetch0(
        .clk(clk),
        .reset(reset),
        .mem_fetch_addr(mem_fetch_addr),
        .mem_fetch_addr_en(mem_fetch_addr_en),
        .mem_inst_in(mem_inst_in),
        .mem_inst_valid(mem_inst_valid),
        .mem_inst_access_fault(mem_inst_access_fault),
        .override_pc(fetch_jump_pc),
        .override_pc_addr(fetch_jump_pc_addr),
        .inst(fetch_inst),
        .inst_pc(fetch_inst_pc),
        .valid(fetch_valid),
        .exception_num(fetch_exception_num),
        .exception_valid(fetch_exception_valid),
        .stall(fetch_stall));

    wire fetch_stall;


    wire [6:0]decode_opcode;
    wire [4:0]decode_rd;
    wire [4:0]decode_rs1;
    wire [4:0]decode_rs2;
    wire [2:0]decode_funct3;
    wire [6:0]decode_funct7;
    wire [31:0]decode_imm;
    wire [31:0]decode_inst_pc_out;

    wire decode_valid;
    wire [5:0]decode_exception_num;
    wire decode_exception_valid;

    decode decode0(
        .clk(clk),
        .reset(reset),
        .inst(fetch_inst),
        .inst_pc(fetch_inst_pc),
        .inst_valid(fetch_valid),
        .fetch_stall(fetch_stall),
        .exception_num_in(fetch_exception_num),
        .exception_valid_in(fetch_exception_valid),
        .flush(pipeline_flush),
        .opcode(decode_opcode),
        .rd(decode_rd),
        .rs1(decode_rs1),
        .rs2(decode_rs2),
        .funct3(decode_funct3),
        .funct7(decode_funct7),
        .imm(decode_imm),
        .inst_pc_out(decode_inst_pc_out),
        .valid(decode_valid),
        .stall(decode_stall),
        .exception_num_out(decode_exception_num),
        .exception_valid_out(decode_exception_valid));

    wire decode_stall;

    wire [4:0]rf_read_rs1;
    wire [4:0]rf_read_rs2;
    wire [31:0]rf_rs1_val;
    wire [31:0]rf_rs2_val;

    wire rf_write_valid;
    wire [4:0]rf_write_rd;
    wire [31:0]rf_write_val;

    reg_file register_file0(
        .clk(clk),
        .reset(reset),
        .read_rs1(rf_read_rs1),
        .read_rs2(rf_read_rs2),
        .write_en_rd(rf_write_valid),
        .write_rd(rf_write_rd),
        .write_val(rf_write_val),
        .rs1_val(rf_rs1_val),
        .rs2_val(rf_rs2_val));

    wire [6:0]rr_opcode;
    wire [4:0]rr_rd;
    wire [4:0]rr_rs2;
    wire [31:0]rr_rs1_val;
    wire [31:0]rr_rs2_val;
    wire [2:0]rr_funct3;
    wire [6:0]rr_funct7;
    wire [31:0]rr_imm;
    wire [31:0]rr_pc;

    wire rr_valid;
    wire [5:0]rr_exception_num;
    wire rr_exception_valid;

    read_registers read_registers0(
        .clk(clk),
        .reset(reset),
        .flush(pipeline_flush),
        .decode_opcode(decode_opcode),
        .decode_rd(decode_rd),
        .decode_rs1(decode_rs1),
        .decode_rs2(decode_rs2),
        .decode_funct3(decode_funct3),
        .decode_funct7(decode_funct7),
        .decode_imm(decode_imm),
        .decode_pc(decode_inst_pc_out),
        .decode_valid(decode_valid),
        .decode_exception_num(decode_exception_num),
        .decode_exception_valid(decode_exception_valid),
        .decode_stall(decode_stall),
        .read_rs1(rf_read_rs1),
        .rs1_val(rf_rs1_val),
        .read_rs2(rf_read_rs2),
        .rs2_val(rf_rs2_val),
        .exec_rd(execute_rd),
        .opcode_out(rr_opcode),
        .rd_out(rr_rd),
        .rs2_out(rr_rs2),
        .rs1_val_out(rr_rs1_val),
        .rs2_val_out(rr_rs2_val),
        .funct3_out(rr_funct3),
        .funct7_out(rr_funct7),
        .imm_out(rr_imm),
        .pc_out(rr_pc),
        .valid(rr_valid),
        .stall(read_registers_stall),
        .exception_num_out(rr_exception_num),
        .exception_valid_out(rr_exception_valid));

    wire read_registers_stall;

    wire execute_valid;
    wire execute_processing;
    wire [4:0]execute_processing_rd;

    wire [4:0]execute_rd;
    wire [31:0]execute_rd_val;
    wire [31:0]execute_inst_pc;
    wire [31:0]execute_jump_pc;
    wire execute_jump_valid;
    wire [5:0]execute_exception_num;
    wire [31:0]execute_exception_val;
    wire execute_exception_valid;
    wire [31:0]execute_store_addr;
    wire [31:0]execute_store_val;
    wire [1:0]execute_store_size;
    wire execute_store_valid;

    execute execute0(
        .clk(clk),
        .reset(reset),
        .flush(pipeline_flush),
        .decode_opcode(rr_opcode),
        .decode_rd(rr_rd),
        .decode_rs2(rr_rs2),
        .decode_funct3(rr_funct3),
        .decode_funct7(rr_funct7),
        .decode_imm(rr_imm),
        .decode_pc(rr_pc),
        .read_rs1_val(rr_rs1_val),
        .read_rs2_val(rr_rs2_val),
        .read_valid(rr_valid),
        .read_stall(read_registers_stall),
        .read_exception_num(rr_exception_num),
        .read_exception_valid(rr_exception_valid),
        .valid(execute_valid),
        .processing(execute_processing),
        .processing_rd(execute_processing_rd),
        .stall(execute_stall),
        .rd_out(execute_rd),
        .rd_val_out(execute_rd_val),
        .inst_pc_out(execute_inst_pc),
        .jump_pc_out(execute_jump_pc),
        .jump_out(execute_jump_valid),
        .exception_num_out(execute_exception_num),
        .exception_val_out(execute_exception_val),
        .exception_valid_out(execute_exception_valid),
        .store_addr_out(execute_store_addr),
        .store_val_out(execute_store_val),
        .store_size_out(execute_store_size),
        .store_valid_out(execute_store_valid));

    wire execute_stall;

    wire commit_valid;
    wire [5:0]commit_exception_num;
    wire [31:0]commit_exception_val;
    wire [31:0]commit_exception_pc;
    wire commit_exception_valid;

    commit commit0(
        .clk(clk),
        .reset(reset),
        .execute_valid(execute_valid),
        .execute_rd(execute_rd),
        .execute_rd_val(execute_rd_val),
        .execute_inst_pc(execute_inst_pc),
        .execute_jump_pc(execute_jump_pc),
        .execute_jump_valid(execute_jump_valid),
        .execute_exception_num(execute_exception_num),
        .execute_exception_val(execute_exception_val),
        .execute_exception_valid(execute_exception_valid),
        .execute_store_addr(execute_store_addr),
        .execute_store_val(execute_store_val),
        .execute_store_size(execute_store_size),
        .execute_store_valid(execute_store_valid),
        .datafifo_full(datafifo_full),
        .datafifo_addr_out(datafifo_addr_out),
        .datafifo_val_out(datafifo_val_out),
        .datafifo_size_out(datafifo_size_out),
        .datafifo_valid_out(datafifo_valid_out),
        .exception_num_out(commit_exception_num),
        .exception_val_out(commit_exception_val),
        .exception_pc_out(commit_exception_pc),
        .exception_valid_out(commit_exception_valid),
        .rd_out(rf_write_rd),
        .rd_val_out(rf_write_val),
        .rd_valid_out(rf_write_valid),
        .commit_valid(commit_valid),
        .execute_stall(execute_stall),
        .pipeline_flush(pipeline_flush),
        .pipeline_pc(commit_jump_pc));


endmodule

