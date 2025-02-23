`timescale 1ns / 1ns

module alu(
    input var [32:0]in_a,
    input var [32:0]in_b,
    input var [4:0]alu_op,

    output [31:0]rd_val_out,
    output lt,
    output ltu,
    output eq
    );

    localparam ALU_ADD = 0;
    localparam ALU_SUB = 1;
    localparam ALU_SLT = 2;
    localparam ALU_SLTU = 3;
    localparam ALU_XOR = 4;
    localparam ALU_OR = 5;
    localparam ALU_AND = 6;
    localparam ALU_LUI = 7;
    localparam ALU_UNKNOWN = 8;

    wire [32:0] in_b_twos = (~in_b) + 'd1;
    wire [32:0] in_b_mux = (alu_op == ALU_SUB ||
                            alu_op == ALU_SLT ||
                            alu_op == ALU_SLTU) ?
                                in_b_twos : in_b;

    wire [32:0] adder_out = {in_a + in_b_mux};

    // Logical Operations
    wire [31:0] alu_and = in_a[31:0] & in_b[31:0];
    wire [31:0] alu_or = in_a[31:0] | in_b[31:0];
    wire [31:0] alu_xor = in_a[31:0] ^ in_b[31:0];

    wire alu_sltu = adder_out[32];
    wire alu_slt = in_a[31] == in_b[31] ? adder_out[32] :
                   (in_a[31] == 'b1 && in_b[31] == 'b0) ? 'b1 : 'b0;

    assign ltu = alu_sltu;
    assign lt = alu_slt;
    assign eq = adder_out == 'd0;

    always_comb begin
        case(alu_op)
            ALU_ADD: rd_val_out = adder_out[31:0];
            ALU_SUB: rd_val_out = adder_out[31:0];
            ALU_SLT: rd_val_out = {31'b0, alu_slt};
            ALU_SLTU: rd_val_out = {31'b0, alu_sltu};
            ALU_XOR: rd_val_out = alu_xor;
            ALU_OR: rd_val_out = alu_or;
            ALU_AND: rd_val_out = alu_and;
            ALU_LUI: rd_val_out = in_b[31:0];
            ALU_UNKNOWN: rd_val_out = 'h003c0de;
        endcase
    end

endmodule
