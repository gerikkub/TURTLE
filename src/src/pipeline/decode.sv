`timescale 1ns / 1ns

`include "pipeline/decode_inst.sv"

module decode(
    input clk,
    input reset,

    input [31:0]inst,
    input [31:0]inst_pc,
    input inst_valid,
    output fetch_stall,
    input [5:0]exception_num_in,
    input exception_valid_in,

    input flush,

    output var [6:0] opcode,
    output var [4:0] rd,
    output var [4:0] rs1,
    output var [4:0] rs2,
    output var [2:0] funct3,
    output var [6:0] funct7,
    output var [31:0] imm,
    output var [31:0] inst_pc_out,

    output valid,
    input stall,
    output [5:0]exception_num_out,
    output exception_valid_out
    );

    wire decode_valid;
    decode_inst d0 (
        .inst(inst_buffer),
        .opcode(opcode),
        .rd(rd),
        .rs1(rs1),
        .rs2(rs2),
        .funct3(funct3),
        .funct7(funct7),
        .imm(imm),
        .valid(decode_valid));

    reg [31:0] inst_buffer;
    reg [31:0] inst_pc_buffer;
    reg [5:0] fetch_exception_buffer;
    reg fetch_exception_valid;
    reg inst_buffer_valid;

    wire [31:0] inst_buffer_mux;
    wire [31:0] inst_pc_buffer_mux;
    wire [5:0] fetch_exception_buffer_mux;
    wire fetch_exception_valid_mux;
    wire inst_buffer_valid_mux;

    wire should_ingest_inst = inst_valid &&
                              ((~inst_buffer_valid) ||
                               (inst_buffer_valid && ~stall));
    wire should_advance_inst = inst_buffer_valid && ~stall && ~flush;

    // Fetch new instructions into the pipeline
    assign inst_buffer_mux = should_ingest_inst ? inst :
                                                  inst_buffer;
    assign inst_pc_buffer_mux = should_ingest_inst ? inst_pc :
                                                     inst_pc_buffer;
    // Either we are ingesting a new instruction so the buffer is valid
    // OR we have a valid instruciton and are not advancing it
    assign inst_buffer_valid_mux = ~flush &&
                                   (should_ingest_inst ||
                                    (inst_buffer_valid && ~should_advance_inst));

    assign fetch_exception_buffer_mux = should_ingest_inst ? exception_num_in :
                                                             fetch_exception_buffer;
    assign fetch_exception_valid_mux = should_ingest_inst ? exception_valid_in :
                                                            fetch_exception_valid;


    // fetch_stall should be set when inst_buffer is
    // valid and the output is not being sent
    // and when not flushing
    assign fetch_stall = ~flush && inst_buffer_valid && ~should_advance_inst;

    assign valid = should_advance_inst;
    assign inst_pc_out = inst_pc_buffer;

    // Exception management
    wire illegal_instruction_exception = ~decode_valid;

    // Fetch exceptions will produce illegal instruction bits
    // so prioritize fetch exceptions over illegal instruction
    assign exception_num_out = fetch_exception_valid ? fetch_exception_buffer :
                               illegal_instruction_exception ? 'd2 : 'd0;
    assign exception_valid_out = fetch_exception_valid | illegal_instruction_exception;

    always_ff @(posedge clk) begin
        if (reset == 'd1) begin
            inst_buffer <= 'd0;
            inst_pc_buffer <= 'd0;
            inst_buffer_valid <= 'd0;
            fetch_exception_buffer <= 'd0;
            fetch_exception_valid <= 'd0;
        end else begin
            inst_buffer <= inst_buffer_mux;
            inst_pc_buffer <= inst_pc_buffer_mux;
            inst_buffer_valid <= inst_buffer_valid_mux;
            fetch_exception_buffer <= fetch_exception_buffer_mux;
            fetch_exception_valid <= fetch_exception_valid_mux;
        end
    end


endmodule

