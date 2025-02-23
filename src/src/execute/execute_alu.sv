`timescale 1ns / 1ns

module execute_alu(
    input var [6:0]decode_opcode,
    input var [2:0]decode_funct3,
    input var [6:0]decode_funct7,
    input var [31:0]decode_imm,
    input var [31:0]decode_pc,

    input var [31:0]read_rs1_val,
    input var [31:0]read_rs2_val,
    input read_valid,

    output [32:0] in_a,
    output [32:0] in_b,
    output [4:0] alu_op,

    output processing,
    output valid
    );

    localparam OP_IMM_OPCODE = 'b0010011;
    localparam OP_OPCODE = 'b0110011;
    localparam LUI_OPCODE = 'b0110111;
    localparam AUIPC_OPCODE = 'b0010111;

    localparam ALU_ADD = 0;
    localparam ALU_SUB = 1;
    localparam ALU_SLT = 2;
    localparam ALU_SLTU = 3;
    localparam ALU_XOR = 4;
    localparam ALU_OR = 5;
    localparam ALU_AND = 6;
    localparam ALU_LUI = 7;
    localparam ALU_UNKNOWN = 8;

    enum int unsigned {
        ADD,
        SUB,
        SLT,
        SLTU,
        XOR,
        OR,
        AND,
        LUI,
        AUIPC,
        UNKNOWN
    } op;

    wire should_handle = (op != UNKNOWN) && read_valid;
    assign processing = should_handle;

    always_comb begin
        if (decode_opcode == OP_IMM_OPCODE) begin
            casez({decode_funct7,decode_funct3})
                10'b???????000: op = ADD;
                10'b???????010: op = SLT;
                10'b???????011: op = SLTU;
                10'b???????100: op = XOR;
                10'b???????110: op = OR;
                10'b???????111: op = AND;
                default: op = UNKNOWN;
            endcase
        end else if (decode_opcode == OP_OPCODE) begin
            casez({decode_funct7,decode_funct3})
                'b0000000000: op = ADD;
                'b0100000000: op = SUB;
                'b0000000010: op = SLT;
                'b0000000011: op = SLTU;
                'b0000000100: op = XOR;
                'b0000000110: op = OR;
                'b0000000111: op = AND;
                default: op = UNKNOWN;
            endcase
        end else if (decode_opcode == AUIPC_OPCODE)
            op = AUIPC;
        else if (decode_opcode == LUI_OPCODE)
            op = LUI;
        else
            op = UNKNOWN;
    end

    // Calculate adder inputs. Add an extra bit for SLTU op
    assign in_a = op == AUIPC ? {1'b0, decode_pc} :
                                     {1'b0, read_rs1_val};

    assign in_b = (decode_opcode == OP_IMM_OPCODE ||
                   decode_opcode == LUI_OPCODE ||
                   decode_opcode == AUIPC_OPCODE) ?
                    {1'b0, decode_imm} :
                    {1'b0, read_rs2_val};

    assign valid = (op == UNKNOWN) ? 'd0 :
                                     should_handle;

    always_comb begin
        case (op)
            ADD: alu_op = ALU_ADD;
            SUB: alu_op = ALU_SUB;
            SLT: alu_op = ALU_SLT;
            SLTU: alu_op = ALU_SLTU;
            XOR: alu_op = ALU_XOR;
            OR: alu_op = ALU_OR;
            AND: alu_op = ALU_AND;
            LUI: alu_op = ALU_LUI;
            AUIPC: alu_op = ALU_ADD;
            UNKNOWN: alu_op = ALU_UNKNOWN;
        endcase
    end

endmodule
