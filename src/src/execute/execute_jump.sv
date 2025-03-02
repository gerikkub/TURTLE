`timescale 1ns / 1ns

module execute_jump(
    input var [6:0]decode_opcode,
    input var [2:0]decode_funct3,
    input var [31:0]decode_imm,
    input var [31:0]decode_pc,

    input var [31:0]read_rs1_val,
    input read_valid,

    output [32:0]in_a,
    output [32:0]in_b,
    output [4:0]alu_op,
    output alu_valid,
    input [31:0]alu_result,

    output processing,
    output valid,
    output [31:0]rd_val_out,
    output [31:0]pc_out,
    output jump_pc_out,
    output [5:0]exception_num_out,
    output exception_valid_out
    );

    localparam JALR_OPCODE = 'b1100111;
    localparam JAL_OPCODE = 'b1101111;
    //localparam BRANCH_OPCODE = 'b1100011;

    localparam ALU_ADD = 0;
    localparam ALU_UNKNOWN = 8;

    localparam EXCEPTION_ADDR_MISALIGN = 'd0;

    assign exception_num_out = EXCEPTION_ADDR_MISALIGN;
    assign exception_valid_out = pc_out[1:0] != 'd0;

    assign jump_pc_out = read_valid && !exception_valid_out;

    always_comb begin
        in_a = {1'b0, 32'h005c0de0};
        in_b = {1'b0, 32'h006c0de0};
        alu_op = ALU_UNKNOWN;
        alu_valid = 0;

        processing = 0;
        valid = 0;
        rd_val_out = 'h007c0de0;
        pc_out = 'h008c0de0;
        if (read_valid) begin
            if (decode_opcode == JAL_OPCODE) begin
                in_a = {1'b0, decode_pc};
                in_b = {1'b0, decode_imm};
                alu_op = ALU_ADD;
                alu_valid = 1;

                processing = 1;
                valid = 1;
                rd_val_out = decode_pc + 'd4;
                pc_out = alu_result;
            end else if (decode_opcode == JALR_OPCODE &&
                         decode_funct3 == 'd0) begin
                in_a = {1'b0, read_rs1_val};
                in_b = {1'b0, decode_imm};
                alu_op = ALU_ADD;
                alu_valid = 1;

                processing = 1;
                valid = 1;
                rd_val_out = decode_pc + 'd4;
                pc_out = {alu_result[31:1], 1'b0};
            end
        end
    end

endmodule

