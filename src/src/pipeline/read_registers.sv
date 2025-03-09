`timescale 1ns / 1ns

module read_registers(
    input clk,
    input reset,
    input flush,

    input var [6:0] decode_opcode,
    input var [4:0] decode_rd,
    input var [4:0] decode_rs1,
    input var [4:0] decode_rs2,
    input var [2:0] decode_funct3,
    input var [6:0] decode_funct7,
    input var [31:0] decode_imm,
    input var [31:0] decode_pc,

    input decode_valid,
    input [5:0]decode_exception_num,
    input decode_exception_valid,
    output decode_stall,

    output [4:0]read_rs1,
    input [31:0]rs1_val,
    output [4:0]read_rs2,
    input [31:0]rs2_val,

    input [4:0]exec_rd,

    output var [6:0] opcode_out,
    output var [4:0] rd_out,
    output var [4:0] rs1_out,
    output var [4:0] rs2_out,
    output var [31:0] rs1_val_out,
    output var [31:0] rs2_val_out,
    output var [2:0] funct3_out,
    output var [6:0] funct7_out,
    output var [31:0] imm_out,
    output var [31:0] pc_out,

    output valid,
    input stall,
    output [5:0]exception_num_out,
    output exception_valid_out
    );

    reg [6:0]opcode_buffer;
    reg [4:0]rd_buffer;
    reg [4:0]rs1_buffer;
    reg [4:0]rs2_buffer;
    reg [2:0]funct3_buffer;
    reg [6:0]funct7_buffer;
    reg [31:0]imm_buffer;
    reg [31:0]pc_buffer;
    reg [5:0]exception_num_buffer;
    reg exception_valid_buffer;
    reg decode_buffer_valid;

    // Advance if:
    // decode_buffer_valid AND
    //   there is an incoming exception OR
    //   the executed destination does not match a source destination
    wire should_advance = ~flush &&
                          ~stall &&
                          decode_buffer_valid &&
                          (exception_valid_buffer ||
                           (((rs1_buffer != exec_rd) || rs1_buffer == 'd0) &&
                            ((rs2_buffer != exec_rd) || rs2_buffer == 'd0)));
                           
    wire should_stall = ~flush &&
                        decode_buffer_valid &&
                        ~should_advance;
    wire should_ingest = decode_valid &&
                         (decode_buffer_valid ? should_advance :
                                                'd1);

    wire [6:0]opcode_buffer_mux = should_ingest ? decode_opcode : opcode_buffer;
    wire [4:0]rd_buffer_mux = should_ingest ? decode_rd : rd_buffer;
    wire [4:0]rs1_buffer_mux = should_ingest ? decode_rs1 : rs1_buffer;
    wire [4:0]rs2_buffer_mux = should_ingest ? decode_rs2 : rs2_buffer;
    wire [2:0]funct3_buffer_mux = should_ingest ? decode_funct3 : funct3_buffer;
    wire [6:0]funct7_buffer_mux = should_ingest ? decode_funct7 : funct7_buffer;
    wire [31:0]imm_buffer_mux = should_ingest ? decode_imm : imm_buffer;
    wire [31:0]pc_buffer_mux = should_ingest ? decode_pc : pc_buffer;
    wire [5:0]exception_num_buffer_mux = should_ingest ? decode_exception_num : exception_num_buffer;
    wire exception_valid_buffer_mux = should_ingest ? decode_exception_valid : exception_valid_buffer;

    wire decode_buffer_valid_mux = ~flush &&
                                   (should_ingest ? 'd1 :
                                    should_advance ? 'd0 : decode_buffer_valid);

    assign decode_stall = should_stall;
    assign valid = should_advance;

    // Pass through decode states
    assign opcode_out = opcode_buffer;
    assign rd_out = rd_buffer;
    assign rs1_out = rs1_buffer;
    assign rs2_out = rs2_buffer;
    assign funct3_out = funct3_buffer;
    assign funct7_out = funct7_buffer;
    assign imm_out = imm_buffer;
    assign pc_out = pc_buffer;
    assign exception_num_out = exception_num_buffer;
    assign exception_valid_out = exception_valid_buffer;

    // Pass read register values
    assign rs1_val_out = rs1_val;
    assign rs2_val_out = rs2_val;

    // Read registers
    assign read_rs1 = rs1_buffer;
    assign read_rs2 = rs2_buffer;

    always_ff @(posedge clk) begin
        if (reset == 'd1) begin
            opcode_buffer <= 'd0;
            rd_buffer <= 'd0;
            rs1_buffer <= 'd0;
            rs2_buffer <= 'd0;
            funct3_buffer <= 'd0;
            funct7_buffer <= 'd0;
            imm_buffer <= 'd0;
            pc_buffer <= 'd0;
            exception_num_buffer <= 'd0;
            exception_valid_buffer <= 'd0;
            decode_buffer_valid <= 'd0;
        end else begin
            opcode_buffer <= opcode_buffer_mux;
            rd_buffer <= rd_buffer_mux;
            rs1_buffer <= rs1_buffer_mux;
            rs2_buffer <= rs2_buffer_mux;
            funct3_buffer <= funct3_buffer_mux;
            funct7_buffer <= funct7_buffer_mux;
            imm_buffer <= imm_buffer_mux;
            pc_buffer <= pc_buffer_mux;
            exception_num_buffer <= exception_num_buffer_mux;
            exception_valid_buffer <= exception_valid_buffer_mux;
            decode_buffer_valid <= decode_buffer_valid_mux;
        end
    end

endmodule
