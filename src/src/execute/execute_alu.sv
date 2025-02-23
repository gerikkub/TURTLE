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

    output processing,
    output valid,
    output [31:0]rd_val_out
    );

    localparam OP_IMM_OPCODE = 'b0010011;
    localparam OP_OPCODE = 'b0110011;
    localparam LUI_OPCODE = 'b0110111;
    localparam AUIPC_OPCODE = 'b0010111;

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
    wire [32:0] in_a = op == AUIPC ? {1'b0, decode_pc} :
                                     {1'b0, read_rs1_val};

    wire [32:0] in_b_pre = (decode_opcode == OP_IMM_OPCODE ||
                            op == AUIPC) ? {1'b0, decode_imm} :
                                                  {1'b0, read_rs2_val};
    wire [32:0] in_b = (op == SUB || op == SLT || op == SLTU) ?
                            ((~in_b_pre) + 'd1) :
                            in_b_pre;

    wire [32:0] adder_out = {in_a + in_b};

    // Logical Operations
    wire [31:0] alu_and = in_a[31:0] & in_b[31:0];
    wire [31:0] alu_or = in_a[31:0] | in_b[31:0];
    wire [31:0] alu_xor = in_a[31:0] ^ in_b[31:0];

    wire alu_sltu = adder_out[32];
    wire alu_slt = in_a[31] == in_b_pre[31] ? adder_out[32] :
                   (in_a[31] == 'b1 && in_b_pre[31] == 'b0) ? 'b1 : 'b0;

    always_comb begin
    end

    always_comb begin
        case(op)
            ADD: rd_val_out = adder_out[31:0];
            SUB: rd_val_out = adder_out[31:0];
            SLT: rd_val_out = {31'b0, alu_slt};
            SLTU: rd_val_out = {31'b0, alu_sltu};
            XOR: rd_val_out = alu_xor;
            OR: rd_val_out = alu_or;
            AND: rd_val_out = alu_and;
            LUI: rd_val_out = decode_imm;
            AUIPC: rd_val_out = adder_out[31:0];
            UNKNOWN: rd_val_out = 'h003c0de;
        endcase
    end

    assign valid = (op == UNKNOWN) ? 'd0 :
                                     should_handle;

endmodule
