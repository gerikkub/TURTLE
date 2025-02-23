`timescale 1ns / 1ns

module execute_store(
    input clk,
    input reset,
    input flush,

    input var [6:0]decode_opcode,
    input var [2:0]decode_funct3,
    input var [31:0]decode_imm,

    input var [31:0]read_rs1_val,
    input var [31:0]read_rs2_val,
    input read_valid,

    output [32:0]in_a,
    output [32:0]in_b,
    output [4:0]alu_op,
    output alu_valid,
    input [31:0]alu_result,

    input storefifo_full,

    output processing,
    output valid,

    output [31:0]store_addr,
    output [31:0]store_val,
    output [1:0]store_size,
    output store_valid,

    output [5:0]exception_num_out,
    output exception_valid_out
    );

    localparam STORE_OPCODE = 'b0100011;

    localparam STORE_BYTE = 3'b000;
    localparam STORE_HALF = 3'b001;
    localparam STORE_WORD = 3'b010;

    localparam ALU_ADD = 0;

    localparam EXCEPTION_STORE_MISALIGN = 'd6;

    enum int {
        DECODE,
        WAIT_FIFO
    } state, next_state;

    wire known_opcode = (decode_opcode == STORE_OPCODE) &&
                        ((decode_funct3 == STORE_BYTE) ||
                         (decode_funct3 == STORE_HALF) ||
                         (decode_funct3 == STORE_WORD));

    assign alu_op = ALU_ADD;
    assign alu_valid = processing;
    assign in_a = {1'b0, read_rs1_val};
    assign in_b = {1'b0, decode_imm};

    assign store_val = read_rs2_val;
    assign store_addr = alu_result;
    assign store_valid = !exception_valid_out;

    assign store_size = decode_funct3[1:0];

    assign exception_valid_out = ((decode_funct3 == STORE_HALF) &&
                                  (store_addr[0] != 'b0)) ||
                                 ((decode_funct3 == STORE_WORD) &&
                                  (store_addr[1:0] != 'b00));
    assign exception_num_out = EXCEPTION_STORE_MISALIGN;

    always_comb begin
        if (state == DECODE) begin
            processing = read_valid && known_opcode && !flush;
            valid = processing && !storefifo_full;
        end else begin
            processing = !flush;
            valid = processing && !storefifo_full;
        end
    end

    always_comb begin
        if (state == DECODE) begin
            if (processing && storefifo_full)
                next_state = WAIT_FIFO;
            else
                next_state = DECODE;
        end else begin
            if ((!storefifo_full) || flush)
                next_state = DECODE;
            else
                next_state = WAIT_FIFO;
        end
    end

    always_ff @(posedge clk) begin
        if (reset == 'd1)
            state <= DECODE;
        else
            state <= next_state;
    end

endmodule

