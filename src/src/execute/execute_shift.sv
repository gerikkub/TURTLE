`timescale 1ns / 1ns

module execute_alu(
    input clk,
    input reset,
    input flush,

    input var [6:0]decode_opcode,
    input var [2:0]decode_funct3,
    input var [6:0]decode_funct7,
    input var [31:0]decode_imm,

    input var [31:0]read_rs1_val,
    input var [31:0]read_rs2_val,
    input read_valid,

    output processing,
    output valid,
    output [31:0]rd_val_out
    );

    // OP-IMM
    wire op_imm = decode_opcode == 'b0010011;

    localparam OP_IMM_OPCODE = 'b0010011;
    localparam OP_OPCODE = 'b0110011;

    enum int unsigned {
        SLL,
        SRL,
        SRA,
        UNKNOWN
    } op;

    always_comb begin
        if (decode_opcode == OP_IMM_OPCODE) begin
            casez({decode_funct7,decode_funct3})
                10'b0000000001: op = SLL;
                10'b0000000101: op = SRL;
                10'b0100000101: op = SRA;
                default: op = UNKNOWN;
            endcase
        end else if (decode_opcode == OP_OPCODE) begin
            casez({decode_funct7,decode_funct3})
                'b0000000001: op = SLL;
                'b0000000101: op = SRL;
                'b0100000101: op = SRA;
                default: op = UNKNOWN;
            endcase
        end else
            op = UNKNOWN;
    end

    assign rd_val_out = shift_buffer_mux;
    assign valid = shift_done;

    wire [4:0]shamt = op_imm ? decode_imm[4:0] :
                               read_rs2_val[4:0];

    reg [4:0]shamt_buffer;
    reg [31:0]shift_buffer;
    reg shift_buffer_valid;

    // Subtract 8 from shift amount
    wire [1:0]shamt_buffer_neg8 = shamt_buffer[4:3] == 'b11 ? 'b10 :
                                  shamt_buffer[4:3] == 'b10 ? 'b01 :
                                  shamt_buffer[4:3] == 'b01 ? 'b00 : 'b00;
                             
    // Can shift by up top 8 bits per cycle
    wire [4:0]shamt_buffer_mux = read_valid ? shamt :
                                 shamt_buffer[4:3] != 0 ?
                                 {shamt_buffer_neg8, shamt_buffer[2:0]} :
                                 'd0;

    wire shift_buffer_valid_mux = read_valid ? (op == SLL || op == SRL || op == SRA) :
                                  shift_done ? 'd0 :
                                  flush ? 'd0 :
                                  shift_buffer_valid;

    // Shift complete. Output in shift_buffer_mux
    wire shift_done = shamt_buffer_mux == 'd0;

    // Barrel Shift outputs for left, right and right arithmetic shifts
    wire [31:0]barrel_shift_left_out[0:8];
    assign barrel_shift_left_out[0] = shift_buffer;
    assign barrel_shift_left_out[1] = {shift_buffer[30:0], 1'b0};
    assign barrel_shift_left_out[2] = {shift_buffer[29:0], 2'b0};
    assign barrel_shift_left_out[3] = {shift_buffer[28:0], 3'b0};
    assign barrel_shift_left_out[4] = {shift_buffer[27:0], 4'b0};
    assign barrel_shift_left_out[5] = {shift_buffer[26:0], 5'b0};
    assign barrel_shift_left_out[6] = {shift_buffer[25:0], 6'b0};
    assign barrel_shift_left_out[7] = {shift_buffer[24:0], 7'b0};
    assign barrel_shift_left_out[8] = {shift_buffer[23:0], 8'b0};

    wire [31:0]barrel_shift_right_out[0:8];
    assign barrel_shift_right_out[0] = shift_buffer;
    assign barrel_shift_right_out[1] = {1'b0, shift_buffer[31:1]};
    assign barrel_shift_right_out[2] = {2'b0, shift_buffer[31:2]};
    assign barrel_shift_right_out[3] = {3'b0, shift_buffer[31:3]};
    assign barrel_shift_right_out[4] = {4'b0, shift_buffer[31:4]};
    assign barrel_shift_right_out[5] = {5'b0, shift_buffer[31:5]};
    assign barrel_shift_right_out[6] = {6'b0, shift_buffer[31:6]};
    assign barrel_shift_right_out[7] = {7'b0, shift_buffer[31:7]};
    assign barrel_shift_right_out[8] = {8'b0, shift_buffer[31:8]};

    wire [31:0]barrel_shifta_right_out[0:8];
    assign barrel_shifta_right_out[0] = shift_buffer;
    assign barrel_shifta_right_out[1] = {{1{shift_buffer[31]}}, shift_buffer[31:1]};
    assign barrel_shifta_right_out[2] = {{2{shift_buffer[31]}}, shift_buffer[31:2]};
    assign barrel_shifta_right_out[3] = {{3{shift_buffer[31]}}, shift_buffer[31:3]};
    assign barrel_shifta_right_out[4] = {{4{shift_buffer[31]}}, shift_buffer[31:4]};
    assign barrel_shifta_right_out[5] = {{5{shift_buffer[31]}}, shift_buffer[31:5]};
    assign barrel_shifta_right_out[6] = {{6{shift_buffer[31]}}, shift_buffer[31:6]};
    assign barrel_shifta_right_out[7] = {{7{shift_buffer[31]}}, shift_buffer[31:7]};
    assign barrel_shifta_right_out[8] = {{8{shift_buffer[31]}}, shift_buffer[31:8]};

    logic [3:0]shamt_cycle = shamt[4:3] != 0 ? 'd8 : {1'b0, shamt[2:0]};
    wire [31:0]shift_buffer_mux = read_valid ? read_rs1_val :
                                  op == SLL ? barrel_shift_left_out[shamt_cycle] :
                                  op == SRL ? barrel_shift_right_out[shamt_cycle] :
                                  op == SRA ? barrel_shifta_right_out[shamt_cycle] :
                                  'h004c0de;

    always_ff @(posedge clk) begin
        if (reset == 'b1) begin
            shamt_buffer <= 'd0;
            shift_buffer <= 'd0;
            shift_buffer_valid <= 'd0;
        end else begin
            shamt_buffer <= shamt_buffer_mux;
            shift_buffer <= shift_buffer_mux;
            shift_buffer_valid <= shift_buffer_valid_mux;
        end
    end


endmodule

