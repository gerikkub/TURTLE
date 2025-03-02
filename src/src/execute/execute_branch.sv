`timescale 1ns / 1ns

module execute_branch(
    input clk,
    input reset,
    input flush,

    input var [6:0]decode_opcode,
    input var [2:0]decode_funct3,
    input var [31:0]decode_imm,
    input var [31:0]decode_pc,

    input var [31:0]read_rs1_val,
    input var [31:0]read_rs2_val,
    input read_valid,

    output [32:0]in_a,
    output [32:0]in_b,
    output [4:0]alu_op,
    output alu_valid,
    input [31:0]alu_result,
    input alu_lt,
    input alu_ltu,
    input alu_eq,

    output processing,
    output valid,
    output [31:0]pc_out,
    output jump_pc_out,
    output [5:0]exception_num_out,
    output exception_valid_out
    );

    localparam BRANCH_OPCODE = 'b1100011;

    localparam ALU_ADD = 0;
    localparam ALU_SUB = 1;
    localparam ALU_SLT = 2;
    localparam ALU_SLTU = 3;
    localparam ALU_UNKNOWN = 8;

    localparam EXCEPTION_ADDR_MISALIGN = 'd0;

    localparam COND_BEQ = 'd0;
    localparam COND_BNE = 'd1;
    localparam COND_BLT = 'd4;
    localparam COND_BGE = 'd5;
    localparam COND_BLTU = 'd6;
    localparam COND_BGEU = 'd7;

    enum int {
        CHECK_CONDITION,
        TAKE_BRANCH
    } state, next_state;

    assign exception_num_out = EXCEPTION_ADDR_MISALIGN;
    assign exception_valid_out = (state == TAKE_BRANCH) &&
                                 (pc_out[1:0] != 'd0);

    wire known_opcode = (decode_opcode == BRANCH_OPCODE) &&
                        ((decode_funct3 == COND_BEQ) ||
                         (decode_funct3 == COND_BNE) ||
                         (decode_funct3 == COND_BLT) ||
                         (decode_funct3 == COND_BGE) ||
                         (decode_funct3 == COND_BLTU) ||
                         (decode_funct3 == COND_BGEU));

    logic should_take;

    always_comb begin
        if (state == TAKE_BRANCH) begin
             processing = 'd1;
             alu_valid = 'd1;
         end else if (read_valid && known_opcode) begin
             processing = 'd1;
             alu_valid = 'd1;
        end else begin
             processing = 'd0;
             alu_valid = 'd0;
        end
    end

    // ALU Operation this cycle
    always_comb begin
        if (state == CHECK_CONDITION) begin
            case (decode_funct3)
                COND_BEQ: alu_op = ALU_SUB;
                COND_BNE: alu_op = ALU_SUB;
                COND_BLT: alu_op = ALU_SLT;
                COND_BGE: alu_op = ALU_SLT;
                COND_BLTU: alu_op = ALU_SLTU;
                COND_BGEU: alu_op = ALU_SLTU;
                default: alu_op = ALU_UNKNOWN;
            endcase
        end else
            alu_op = ALU_ADD;
    end

    // Check if a branch should be taken
    always_comb begin
        case (decode_funct3)
            COND_BEQ: should_take = alu_eq;
            COND_BNE: should_take = !alu_eq;
            COND_BLT: should_take = alu_lt;
            COND_BGE: should_take = (!alu_lt) || alu_eq;
            COND_BLTU: should_take = alu_ltu;
            COND_BGEU: should_take = (!alu_ltu) || alu_eq;
            default: should_take = 'd0;
        endcase
    end

    // Calculate ALU inputs
    always_comb begin

        if (!processing || flush) begin
            in_a = 'h009c0de;
            in_b = 'h00ac0de;
            jump_pc_out = 'd0;
            pc_out = 'h00bc0de;
            valid = 'd0;
        end else if (state == CHECK_CONDITION) begin
            in_a = {1'b0, read_rs1_val};
            in_b = {1'b0, read_rs2_val};
            if (should_take) begin
                jump_pc_out = 'd0;
                // PC will be calculated on next cycle
                pc_out = 'h00cc0de;
                valid = 'd0;
            end else begin
                jump_pc_out = 'd0;
                pc_out = 'h00dc0de;
                // End the cycle here
                valid = 'd1;
            end
        end else begin
            in_a = {1'b0, decode_pc};
            in_b = {1'b0, decode_imm};

            // Only jump if there isn't an exception
            jump_pc_out = ~exception_valid_out;
            pc_out = alu_result;
            valid = 'd1;
        end

    end

    // Should progress to TAKE_BRANCH state
    always_comb begin
        if (state == CHECK_CONDITION) begin
            if (!flush && read_valid &&
                known_opcode && should_take)
                next_state = TAKE_BRANCH;
            else
                next_state = CHECK_CONDITION;
        end else begin
            next_state = CHECK_CONDITION;
        end
    end

    always_ff @(posedge clk) begin
        if (reset == 'd1) begin
            state <= CHECK_CONDITION;
        end else begin
            state <= next_state;
        end
    end

endmodule

