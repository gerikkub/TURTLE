`timescale 1ns / 1ns

`include "pipeline/hold.sv"
`include "execute/alu.sv"
`include "execute/execute_alu.sv"
`include "execute/execute_branch.sv"
//`include "execute/execute_jump.sv"
//`include "execute/execute_shift.sv"
//`include "execute/execute_store.sv"

module execute(
    input clk,
    input reset,
    input flush,

    input var [6:0]decode_opcode,
    input var [4:0]decode_rd,
    input var [4:0]decode_rs2,
    input var [2:0]decode_funct3,
    input var [6:0]decode_funct7,
    input var [31:0]decode_imm,
    input var [31:0]decode_pc,

    input var [31:0]read_rs1_val,
    input var [31:0]read_rs2_val,

    input read_valid,
    output read_stall,
    input [5:0]read_exception_num,
    input read_exception_valid,

    output valid,
    output processing,
    input stall, 
    output [4:0]rd_out,
    output [31:0]rd_val_out,
    output [31:0]inst_pc_out,
    output [31:0]jump_pc_out,
    output jump_out,
    output [5:0]exception_num_out,
    output exception_valid_out

    );

    // Input register
    reg [6:0]opcode_in;
    reg [4:0]rd_in;
    reg [4:0]rs2_in;
    reg [2:0]funct3_in;
    reg [6:0]funct7_in;
    reg [31:0]imm_in;
    reg [31:0]pc_in;
    reg [31:0]rs1_val_in;
    reg [31:0]rs2_val_in;
    reg [5:0]exception_num_in;
    reg exception_valid_in;
    reg in_valid;
    reg first_cycle;

    wire in_valid_mux = !flush &&
                        read_valid ||
                        (in_valid &&
                         !valid);

    wire ex_valid = ex_alu_valid ||
                    ex_branch_valid;
    assign processing = ex_alu_processing ||
                        ex_branch_processing;

    var [1:0] ex_list = {ex_alu_processing,
                         ex_branch_processing};
    always_comb begin
        int count = 0;
        for (int i = 0; i < 2; i++) begin
            count += {31'b0, ex_list[i]};
        end
        assert(count <= 1);
    end

    assign read_stall = in_valid && !valid;


    always @(posedge clk) begin
        if (reset == 'd1) begin
            opcode_in <= 'd0;
            rd_in <= 'd0;
            rs2_in <= 'd0;
            funct3_in <= 'd0;
            funct7_in <= 'd0;
            imm_in <= 'd0;
            pc_in <= 'd0;
            rs1_val_in <= 'd0;
            rs2_val_in <= 'd0;
            exception_num_in <= 'd0;
            exception_valid_in <= 'd0;
            in_valid <= 'd0;
            first_cycle <= 'd0;

        end else begin
            if (read_valid == 'd1) begin
                opcode_in <= decode_opcode;
                rd_in <= decode_rd;
                rs2_in <= decode_rs2;
                funct3_in <= decode_funct3;
                funct7_in <= decode_funct7;
                imm_in <= decode_imm;
                pc_in <= decode_pc;

                rs1_val_in <= read_rs1_val;
                rs2_val_in <= read_rs2_val;

                exception_num_in <= read_exception_num;
                exception_valid_in <= read_exception_valid;

                first_cycle <= 'd1;
            end else begin
                opcode_in <= opcode_in;
                rd_in <= rd_in;
                rs2_in <= rs2_in;
                funct3_in <= funct3_in;
                funct7_in <= funct7_in;
                imm_in <= imm_in;
                pc_in <= pc_in;

                rs1_val_in <= rs1_val_in;
                rs2_val_in <= rs2_val_in;

                exception_num_in <= exception_num_in;
                exception_valid_in <= exception_valid_in;

                first_cycle <= 'd0;
            end

            in_valid <= in_valid_mux;
        end
    end

    /// Result logic
    wire [31:0]pc_result = ex_branch_processing ? ex_branch_pc_result :
                           'h014c0de;

    wire jump_pc_result = ex_branch_processing ? ex_branch_jump_result :
                          'd0;

    wire [31:0]rd_val_result = ex_alu_processing ? alu_result :
                               'h013c0de;

    wire [5:0]exception_num_result = ex_alu_processing ? 'd0 :
                                     ex_branch_processing ? ex_branch_exception_num_result :
                                     'd0;
    wire exception_valid_result = ex_alu_processing ? 'd0 :
                                  ex_branch_processing ? ex_branch_exception_valid_result :
                                  'd0;

    wire [108:0]hold_in = {exception_valid_result,
                           exception_num_result,
                           jump_pc_result,
                           pc_result,
                           pc_in,
                           rd_val_result,
                           rd_in};

    wire [108:0]hold_out;

    hold #(109) h0(
        .clk(clk),
        .reset(reset),
        .data_in(hold_in),
        .data_out(hold_out),
        .input_valid(ex_valid),
        .stall(stall),
        .valid_out(valid));

    assign rd_out = hold_out[4:0];
    assign rd_val_out = hold_out[36:5];
    assign inst_pc_out = hold_out[68:37];
    assign jump_pc_out = hold_out[100:69];
    assign jump_out = hold_out[101];
    assign exception_num_out = hold_out[107:102];
    assign exception_valid_out = hold_out[108];

    // ALU
    localparam ALU_UNKNOWN = 8;

    wire [32:0] alu_in_a = ex_alu_processing ? ex_alu_alu_in_a :
                           ex_branch_alu_valid ? ex_branch_alu_in_a :
                           'h010c0de0;
    wire [32:0] alu_in_b = ex_alu_processing ? ex_alu_alu_in_b :
                           ex_branch_alu_valid ? ex_branch_alu_in_b :
                           'h011c0de0;
    wire [4:0] alu_op = ex_alu_processing ? ex_alu_alu_op :
                        ex_branch_alu_valid ? ex_branch_alu_op :
                        ALU_UNKNOWN;

    wire [31:0] alu_result;
    wire alu_lt;
    wire alu_ltu;
    wire alu_eq;

    alu alu0(
        .in_a(alu_in_a),
        .in_b(alu_in_b),
        .alu_op(alu_op),
        .rd_val_out(alu_result),
        .lt(alu_lt),
        .ltu(alu_ltu),
        .eq(alu_eq));

    /// ALU Execution Unit
    wire [32:0] ex_alu_alu_in_a;
    wire [32:0] ex_alu_alu_in_b;
    wire [4:0] ex_alu_alu_op;

    wire ex_alu_processing;
    wire ex_alu_valid;

    execute_alu ex_alu0(
        .decode_opcode(opcode_in),
        .decode_funct3(funct3_in),
        .decode_funct7(funct7_in),
        .decode_imm(imm_in),
        .decode_pc(pc_in),
        .read_rs1_val(rs1_val_in),
        .read_rs2_val(rs2_val_in),
        .read_valid(first_cycle),
        .in_a(ex_alu_alu_in_a),
        .in_b(ex_alu_alu_in_b),
        .alu_op(ex_alu_alu_op),
        .processing(ex_alu_processing),
        .valid(ex_alu_valid));

    // Branch Execution Unit
    wire [32:0]ex_branch_alu_in_a;
    wire [32:0]ex_branch_alu_in_b;
    wire [4:0]ex_branch_alu_op;
    wire ex_branch_alu_valid;

    wire [31:0]ex_branch_pc_result;
    wire ex_branch_jump_result;
    wire [5:0]ex_branch_exception_num_result;
    wire ex_branch_exception_valid_result;
    wire ex_branch_processing;
    wire ex_branch_valid;

    execute_branch ex_branch0(
        .clk(clk),
        .reset(reset),
        .flush(flush),
        .decode_opcode(opcode_in),
        .decode_funct3(funct3_in),
        .decode_imm(imm_in),
        .decode_pc(pc_in),
        .read_rs1_val(rs1_val_in),
        .read_rs2_val(rs2_val_in),
        .read_valid(first_cycle),
        .in_a(ex_branch_alu_in_a),
        .in_b(ex_branch_alu_in_b),
        .alu_op(ex_branch_alu_op),
        .alu_valid(ex_branch_alu_valid),
        .alu_result(alu_result),
        .alu_lt(alu_lt),
        .alu_ltu(alu_ltu),
        .alu_eq(alu_eq),
        .processing(ex_branch_processing),
        .valid(ex_branch_valid),
        .pc_out(ex_branch_pc_result),
        .jump_pc_out(ex_branch_jump_result),
        .exception_num_out(ex_branch_exception_num_result),
        .exception_valid_out(ex_branch_exception_valid_result));


endmodule

