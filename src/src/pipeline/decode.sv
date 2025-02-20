`timescale 1ns / 1ns

`include "pipeline/decode_inst.sv"

module decode(
    input clk,
    input reset,

    input [31:0]inst,
    input [31:0]inst_pc,
    input inst_valid,
    output fetch_stall,

    output var [6:0] opcode,
    output var [4:0] rd,
    output var [4:0] rs1,
    output var [4:0] rs2,
    output var [2:0] funct3,
    output var [6:0] funct7,
    output var [31:0] imm,
    output var [31:0] inst_pc_out,

    output valid,
    input stall

    // TODO
    //output [31:0] exception
    );

    var decode_valid;
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

    enum int unsigned {
        FETCH,
        PRESENT
    } state, next_state;

    reg [31:0] inst_buffer;
    reg [31:0] inst_pc_buffer;
    reg inst_buffer_valid;

    wire [31:0] inst_buffer_mux;
    wire [31:0] inst_pc_buffer_mux;
    wire inst_buffer_valid_mux;

    // Fetch new instructions into the pipeline
    assign inst_buffer_mux = state == FETCH ? inst :
                                              inst_buffer;
    assign inst_pc_buffer_mux = state == FETCH ? inst_pc :
                                                  inst_pc_buffer;
    assign inst_buffer_valid_mux =
        state == FETCH ? inst_valid :
                         'd1;

    assign fetch_stall = state == FETCH ? stall :
                                          'd1;
    assign valid = inst_buffer_valid_mux;
    assign inst_pc_out = inst_pc_buffer;

    always_comb begin
        if (state == FETCH) begin
            if (inst_valid && stall)
                next_state = PRESENT;
            else
                next_state = FETCH;
        end else begin
            if (~stall)
                next_state = FETCH;
            else
                next_state = PRESENT;
        end
    end

    always_ff @(posedge clk) begin
        if (reset == 'd1) begin
            inst_buffer <= 'd0;
            inst_pc_buffer <= 'd0;
            inst_buffer_valid <= 'd0;
            state <= FETCH;
        end else begin
            state <= next_state;
            inst_buffer <= inst_buffer_mux;
            inst_pc_buffer <= inst_pc_buffer_mux;
            inst_buffer_valid <= inst_buffer_valid_mux;
        end
    end


endmodule

