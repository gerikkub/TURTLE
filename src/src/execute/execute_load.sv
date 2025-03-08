`timescale 1ns / 1ns

module execute_load(
    input clk,
    input reset,
    input flush,

    input var [6:0]decode_opcode,
    input var [2:0]decode_funct3,
    input var [31:0]decode_imm,

    input var [31:0]read_rs1_val,
    input read_valid,

    output [32:0]in_a,
    output [32:0]in_b,
    output [4:0]alu_op,
    output alu_valid,
    input [31:0]alu_result,

    output [31:0]mem_data_addr,
    output mem_data_addr_valid,
    output [1:0]mem_data_size,
    input [31:0] mem_data_in,
    input mem_data_valid,
    input mem_data_access_fault,

    output processing,
    output valid,
    output [31:0]rd_val,

    output [5:0]exception_num_out,
    output exception_valid_out
    );

    localparam LOAD_OPCODE = 'b0000011;
    localparam LOAD_BYTE = 3'b000;
    localparam LOAD_HALF = 3'b001;
    localparam LOAD_WORD = 3'b010;
    localparam LOAD_BYTEU = 3'b100;
    localparam LOAD_HALFU = 3'b101;

    localparam ALU_ADD = 0;

    localparam EXCEPTION_LOAD_MISALIGN = 'd4;
    localparam EXCEPTION_LOAD_ACCESS = 'd5;

    wire known_opcode = (decode_opcode == LOAD_OPCODE) &&
                        ((decode_funct3 == LOAD_BYTE) ||
                         (decode_funct3 == LOAD_BYTEU) ||
                         (decode_funct3 == LOAD_HALF) ||
                         (decode_funct3 == LOAD_HALFU) ||
                         (decode_funct3 == LOAD_WORD));

    reg [31:0]load_addr_buffer;
    reg load_addr_valid_buffer;

    wire [31:0]load_addr_mux;
    wire load_addr_valid_mux;

    assign load_addr_mux = flush == 'd1 ? 'd0 :
                           read_valid ? alu_result :
                           load_addr_buffer;

    assign load_addr_valid_mux = flush == 'd1 ? 'd0 :
                                 (read_valid && known_opcode) ? 'd1 :
                                 (load_addr_valid_buffer &&
                                  !mem_data_valid);

    assign valid = (!flush) && load_addr_valid_buffer && mem_data_valid;
    assign processing = (!flush && known_opcode && (read_valid || load_addr_valid_buffer));

    always_ff @(posedge clk) begin
        if (reset == 'd1) begin
            load_addr_buffer <= 'd0;
            load_addr_valid_buffer <= 'd0;
        end else begin
            load_addr_buffer <= load_addr_mux;
            load_addr_valid_buffer <= load_addr_valid_mux;
        end
    end

    // Perform ALU ops on the same cycle that data is available
    assign in_a = {1'b0, read_rs1_val};
    assign in_b = {1'b0, decode_imm};
    assign alu_op = ALU_ADD;
    assign alu_valid = read_valid && known_opcode;

    assign mem_data_addr = mem_data_addr_valid ? load_addr_buffer :
                           'h022c0de0;
    assign mem_data_size = decode_funct3[1:0];
    // Perform memory transaction as long load_addr_buffer is valid
    // AND not flush
    assign mem_data_addr_valid = (!flush) && load_addr_valid_buffer;

    wire [31:0] data_word = mem_data_in;
    wire [31:0] data_half = {{16{mem_data_in[15]}}, mem_data_in[15:0]};
    wire [31:0] data_halfu = {16'b0, mem_data_in[15:0]};
    wire [31:0] data_byte = {{24{mem_data_in[7]}}, mem_data_in[7:0]};
    wire [31:0] data_byteu = {24'b0, mem_data_in[7:0]};

    assign rd_val = (!mem_data_valid) ? 'h020c0de0 :
                    (decode_funct3 == LOAD_BYTE) ? data_byte :
                    (decode_funct3 == LOAD_BYTEU) ? data_byteu :
                    (decode_funct3 == LOAD_HALF) ? data_half :
                    (decode_funct3 == LOAD_HALFU) ? data_halfu :
                    (decode_funct3 == LOAD_WORD) ? data_word :
                    // Note: This should be caught as an illegal opcode
                    'h021c0de0;

    assign exception_num_out = EXCEPTION_LOAD_ACCESS;
    assign exception_valid_out = valid &&
                                 mem_data_valid && mem_data_access_fault;

endmodule


