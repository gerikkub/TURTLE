`timescale 1ns / 1ns

module execute_shift(
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
            casez({decode_imm[11:5],decode_funct3})
                'b0000000001: op = SLL;
                'b0000000101: op = SRL;
                'b0100000101: op = SRA;
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


    reg [1:0]shamt_buffer;
    reg [31:0]shift_buffer;
    reg shift_buffer_valid;


    wire [31:0]barrel_mux = read_valid ? read_rs1_val :
                            shift_buffer;

    // Barrel Shift outputs for left, right and right arithmetic shifts
    wire [31:0]barrel_shift_left_out[0:8];
    assign barrel_shift_left_out[0] = barrel_mux;
    assign barrel_shift_left_out[1] = {barrel_mux[30:0], 1'b0};
    assign barrel_shift_left_out[2] = {barrel_mux[29:0], 2'b0};
    assign barrel_shift_left_out[3] = {barrel_mux[28:0], 3'b0};
    assign barrel_shift_left_out[4] = {barrel_mux[27:0], 4'b0};
    assign barrel_shift_left_out[5] = {barrel_mux[26:0], 5'b0};
    assign barrel_shift_left_out[6] = {barrel_mux[25:0], 6'b0};
    assign barrel_shift_left_out[7] = {barrel_mux[24:0], 7'b0};
    assign barrel_shift_left_out[8] = {barrel_mux[23:0], 8'b0};

    wire [31:0]barrel_shift_right_out[0:8];
    assign barrel_shift_right_out[0] = barrel_mux;
    assign barrel_shift_right_out[1] = {1'b0, barrel_mux[31:1]};
    assign barrel_shift_right_out[2] = {2'b0, barrel_mux[31:2]};
    assign barrel_shift_right_out[3] = {3'b0, barrel_mux[31:3]};
    assign barrel_shift_right_out[4] = {4'b0, barrel_mux[31:4]};
    assign barrel_shift_right_out[5] = {5'b0, barrel_mux[31:5]};
    assign barrel_shift_right_out[6] = {6'b0, barrel_mux[31:6]};
    assign barrel_shift_right_out[7] = {7'b0, barrel_mux[31:7]};
    assign barrel_shift_right_out[8] = {8'b0, barrel_mux[31:8]};

    wire [31:0]barrel_shifta_right_out[0:8];
    assign barrel_shifta_right_out[0] = barrel_mux;
    assign barrel_shifta_right_out[1] = {{1{barrel_mux[31]}}, barrel_mux[31:1]};
    assign barrel_shifta_right_out[2] = {{2{barrel_mux[31]}}, barrel_mux[31:2]};
    assign barrel_shifta_right_out[3] = {{3{barrel_mux[31]}}, barrel_mux[31:3]};
    assign barrel_shifta_right_out[4] = {{4{barrel_mux[31]}}, barrel_mux[31:4]};
    assign barrel_shifta_right_out[5] = {{5{barrel_mux[31]}}, barrel_mux[31:5]};
    assign barrel_shifta_right_out[6] = {{6{barrel_mux[31]}}, barrel_mux[31:6]};
    assign barrel_shifta_right_out[7] = {{7{barrel_mux[31]}}, barrel_mux[31:7]};
    assign barrel_shifta_right_out[8] = {{8{barrel_mux[31]}}, barrel_mux[31:8]};

    // Shift by lower bits on the read cycle and 8 bits on other cycles
    logic [3:0]shamt_cycle;
    assign shamt_cycle = read_valid ? {1'b0, shamt_in[2:0]} : 'd8;
    wire [31:0]shift_buffer_mux = op == SLL ? barrel_shift_left_out[shamt_cycle] :
                                  op == SRL ? barrel_shift_right_out[shamt_cycle] :
                                  op == SRA ? barrel_shifta_right_out[shamt_cycle] :
                                  'h004c0de;

    assign rd_val_out = (read_valid && shift_buffer_valid) ? 'h004c0de0 :
                        shift_buffer_mux;

    wire [4:0]shamt_in = op_imm ? decode_imm[4:0] :
                                  read_rs2_val[4:0];
    wire [1:0]shamt_next = read_valid ? shamt_in[4:3] :
                           shamt_buffer == 'b11 ? 'b10 :
                           shamt_buffer == 'b10 ? 'b01 :
                           shamt_buffer == 'b01 ? 'b00 : 'b00;
                             
    // Can shift by up top 8 bits per cycle
    wire [1:0]shamt_buffer_mux = shamt_next;

    wire shift_buffer_valid_mux = flush ? 'd0 :
                                  read_valid ? ((op != UNKNOWN) && !shift_done) :
                                  shift_done ? 'd0 :
                                  shift_buffer_valid;

    // Shift complete. Output in shift_buffer_mux
    wire shift_done = (read_valid || shift_buffer_valid) &&
                      (shamt_next == 'd0);

    assign processing = ~flush &&
                        (shift_buffer_valid ||
                         (read_valid && (op != UNKNOWN)));
    assign valid = processing && shift_done;

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

