`timescale 1ns / 1ns

module decode_inst(
    input [31:0]inst,

    output var [6:0] opcode,
    output var [4:0] rd,
    output var [4:0] rs1,
    output var [4:0] rs2,
    output var [2:0] funct3,
    output var [6:0] funct7,
    output var [31:0] imm,

    output var valid
    );

    // Instruction decoding wires
    enum int unsigned {
        INST_R,
        INST_I,
        INST_S,
        INST_U,
        INST_B,
        INST_J
    } rv32_type;

    enum int unsigned {
        RV32G_LOAD,
        RV32G_LOAD_FP,
        RV32G_MISC_MEM,
        RV32G_OP_IMM,
        RV32G_AUIPC,
        RV32G_OP_IMM32,
        RV32G_STORE,
        RV32G_STORE_FP,
        RV32G_AMO,
        RV32G_OP,
        RV32G_LUI,
        RV32G_OP_32,
        RV32G_MADD,
        RV32G_MSUB,
        RV32G_NMSUB,
        RV32G_NMADD,
        RV32G_OP_FP,
        RV32G_OP_V,
        RV32G_BRANCH,
        RV32G_JALR,
        RV32G_JAL,
        RV32G_SYSTEM,
        RV32G_OP_VE,
        UNKNOWN
    } rv32_opcode_kind;

    assign opcode = inst[6:0];

    wire [1:0]inst_width;
    wire [4:0]inst_opcode_kind;

    assign inst_width = inst[1:0];
    assign inst_opcode_kind = inst[6:2];

    // Decode 32 bit opcodes
    always_comb begin
        if (inst_width != 'b11)
            rv32_opcode_kind = UNKNOWN;
        else begin
            case (inst_opcode_kind)
                'b00000: rv32_opcode_kind = RV32G_LOAD;
                'b00001: rv32_opcode_kind = RV32G_LOAD_FP;
                'b00010: rv32_opcode_kind = UNKNOWN;
                'b00011: rv32_opcode_kind = RV32G_MISC_MEM;
                'b00100: rv32_opcode_kind = RV32G_OP_IMM;
                'b00101: rv32_opcode_kind = RV32G_AUIPC;
                'b00110: rv32_opcode_kind = RV32G_OP_IMM32;
                'b00111: rv32_opcode_kind = UNKNOWN;
                'b01000: rv32_opcode_kind = RV32G_STORE;
                'b01001: rv32_opcode_kind = RV32G_STORE_FP;
                'b01010: rv32_opcode_kind = UNKNOWN;
                'b01011: rv32_opcode_kind = RV32G_AMO;
                'b01100: rv32_opcode_kind = RV32G_OP;
                'b01101: rv32_opcode_kind = RV32G_LUI;
                'b01110: rv32_opcode_kind = RV32G_OP_32;
                'b01111: rv32_opcode_kind = UNKNOWN;
                'b10000: rv32_opcode_kind = RV32G_MADD;
                'b10001: rv32_opcode_kind = RV32G_MSUB;
                'b10010: rv32_opcode_kind = RV32G_NMSUB;
                'b10011: rv32_opcode_kind = RV32G_NMADD;
                'b10100: rv32_opcode_kind = RV32G_OP_FP;
                'b10101: rv32_opcode_kind = RV32G_OP_V;
                'b10110: rv32_opcode_kind = UNKNOWN;
                'b10111: rv32_opcode_kind = UNKNOWN;
                'b11000: rv32_opcode_kind = RV32G_BRANCH;
                'b11001: rv32_opcode_kind = RV32G_JALR;
                'b11010: rv32_opcode_kind = UNKNOWN;
                'b11011: rv32_opcode_kind = RV32G_JAL;
                'b11100: rv32_opcode_kind = RV32G_SYSTEM;
                'b11101: rv32_opcode_kind = RV32G_OP_VE;
                'b11110: rv32_opcode_kind = UNKNOWN;
                'b11111: rv32_opcode_kind = UNKNOWN;
            endcase
        end
    end

    // Opcode to instruction type mapping
    always_comb begin

        valid = 1;
        case (rv32_opcode_kind)
            RV32G_LOAD: rv32_type = INST_I;
            //RV32G_LOAD_FP,
            //RV32G_MISC_MEM,
            RV32G_OP_IMM: rv32_type = INST_I;
            RV32G_AUIPC: rv32_type = INST_U;
            //RV32G_OP_IMM32,
            RV32G_STORE: rv32_type = INST_S;
            //RV32G_STORE_FP,
            //RV32G_AMO,
            RV32G_OP: rv32_type = INST_R;
            RV32G_LUI: rv32_type = INST_U;
            //RV32G_OP_32,
            //RV32G_MADD,
            //RV32G_MSUB,
            //RV32G_NMSUB,
            //RV32G_NMADD,
            //RV32G_OP_FP,
            //RV32G_OP_V,
            RV32G_BRANCH: rv32_type = INST_B;
            RV32G_JALR: rv32_type = INST_I;
            RV32G_JAL: rv32_type = INST_J;
            RV32G_SYSTEM: rv32_type = INST_I;
            //RV32G_OP_VE,
            //UNKNOWN
            // Note: This should trigger an invalid instruction elsewhere
            default: begin
                rv32_type = INST_R;
                valid = 0;
            end
        endcase
    end

    // Decode instruction bits
    always_comb begin
        if (rv32_type == INST_R) begin
            rd = inst[11:7];
            funct3 = inst[14:12];
            rs1 = inst[19:15];
            rs2 = inst[24:20];
            funct7 = inst[31:25];
            imm = 'd0;
        end else if (rv32_type == INST_I) begin
            rd = inst[11:7];
            funct3 = inst[14:12];
            rs1 = inst[19:15];
            rs2 = 'd0;
            funct7 = 'd0;
            imm = {{21{inst[31]}},inst[30:20]};
        end else if (rv32_type == INST_S) begin
            rd = 'd0;
            funct3 = inst[14:12];
            rs1 = inst[19:15];
            rs2 = inst[24:20];
            funct7 = 'd0;
            imm = {{21{inst[31]}},inst[30:25],inst[11:7]};
        end else if (rv32_type == INST_B) begin
            rd = 'd0;
            funct3 = inst[14:12];
            rs1 = inst[19:15];
            rs2 = inst[24:20];
            funct7 = 'd0;
            imm = {{20{inst[31]}},inst[7],
                   inst[30:25],inst[11:8],1'b0};
        end else if (rv32_type == INST_U) begin
            rd = inst[11:7];
            funct3 = 'd0;
            rs1 = 'd0;
            rs2 = 'd0;
            funct7 = 'd0;
            imm = {inst[31],inst[30:12],12'b0};
        end else begin // INST_J
            rd = inst[11:7];
            funct3 = 'd0;
            rs1 = 'd0;
            rs2 = 'd0;
            funct7 = 'd0;
            imm = {{12{inst[31]}},inst[19:12],
                   inst[20],inst[30:21],1'b0};
        end
    end
endmodule

