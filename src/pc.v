`timescale 1ns / 1ns

module pc(
    input clk,
    input reset,

    input [2:0] pc_mux_sel,
    input [31:0] alu_out,
    input [31:0] inst_imm_j,
    input [31:0] inst_imm_b,
    input [31:0] csr_mtvec,
    input [31:0] mem_dout,
    input take_branch,

    output [31:0] pc_out,
    output [31:0] pc_4_out
    );

    reg [31:0] pc_reg;

    wire [31:0] pc_reg_update;
    wire [31:0] pc_4;
    wire [31:0] branch_addr;

    initial begin
        pc_reg <= 'd4;
    end

    always @(posedge clk) begin
        if (reset == 'd0) begin
            pc_reg <= 'd4;
        end else begin
            pc_reg <= pc_reg_update;
        end
    end

    assign pc_4 = pc_reg + 'd4;

    assign pc_reg_update = (pc_mux_sel == 'd0) ? pc_reg :
                           (pc_mux_sel == 'd1) ? pc_4 :
                           (pc_mux_sel == 'd2) ? pc_reg + inst_imm_j :
                           (pc_mux_sel == 'd3) ? {alu_out[31:1], 1'b0} :
                           (pc_mux_sel == 'd4) ? branch_addr :
                           (pc_mux_sel == 'd5) ? csr_mtvec :
                           (pc_mux_sel == 'd6) ? mem_dout :
                           'hFFFFFFFF;

    assign branch_addr = (take_branch == 'd0) ? pc_4 :
                         pc_reg + inst_imm_b;

    assign pc_out = pc_reg;
    assign pc_4_out = pc_4;

endmodule
