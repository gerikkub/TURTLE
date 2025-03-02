`timescale 1ns / 1ns

module commit(
    input clk,
    input reset,

    // Execute Inputs
    input execute_valid,

    output [4:0]execute_rd,
    output [31:0]execute_rd_val,

    output [31:0]execute_inst_pc,
    output [31:0]execute_jump_pc,
    output execute_jump_valid,

    output [5:0]execute_exception_num,
    output [31:0]execute_exception_val,
    output execute_exception_valid,

    output [31:0]execute_store_addr,
    output [31:0]execute_store_val,
    output [1:0]execute_store_size,
    output execute_store_valid,

    // Data FIFO
    input datafifo_full,
    output [31:0]datafifo_addr_out,
    output [31:0]datafifo_val_out,
    output [1:0]datafifo_size_out,
    output datafifo_valid_out,

    // Exception handling
    output [5:0]exception_num_out,
    output [31:0]exception_val_out,
    output [31:0]exception_pc_out,
    output exception_valid_out,

    // Register Writeback
    output [4:0]rd_out,
    output [31:0]rd_val_out,
    output rd_valid_out,

    // Pipeline Control
    output commit_valid,
    output execute_stall,
    output pipeline_flush,
    output [31:0]pipeline_pc

    // TODO: Exceptions
    // TODO: Interrupts
    // TODO: CSR Writes
    );

    reg [4:0]rd_in;
    reg [31:0]rd_val_in;
    reg [31:0]inst_pc_in;
    reg [31:0]jump_pc_in;
    reg jump_valid_in;
    reg [5:0]exception_num_in;
    reg [31:0]exception_val_in;
    reg exception_valid_in;
    reg [31:0]store_addr_in;
    reg [31:0]store_val_in;
    reg [1:0]store_size_in;
    reg store_valid_in;

    reg in_valid;

    wire in_valid_mux = execute_valid ? 'd1 :
                        commit_valid ? 'd0 :
                        in_valid;

    always_ff @(posedge clk) begin
        if (reset == 'd1) begin
            rd_in <= 'd0;
            rd_val_in <= 'd0;
            inst_pc_in <= 'd0;
            jump_pc_in <= 'd0;
            jump_valid_in <= 'd0;
            exception_num_in <= 'd0;
            exception_val_in <= 'd0;
            exception_valid_in <= 'd0;
            store_addr_in <= 'd0;
            store_val_in <= 'd0;
            store_size_in <= 'd0;
            store_valid_in <= 'd0;
            in_valid <= 'd0;
        end else begin
            in_valid <= in_valid_mux;
            if (execute_valid == 'd1) begin
                rd_in <= execute_rd;
                rd_val_in <= execute_rd_val;
                inst_pc_in <= execute_inst_pc;
                jump_pc_in <= execute_jump_pc;
                jump_valid_in <= execute_jump_valid;
                exception_num_in <= execute_exception_num;
                exception_val_in <= execute_exception_val;
                exception_valid_in <= execute_exception_valid;
                store_addr_in <= execute_store_addr;
                store_val_in <= execute_store_val;
                store_size_in <= execute_store_size;
                store_valid_in <= execute_store_valid;
            end else if (in_valid_mux == 1) begin
                rd_in <= rd_in;
                rd_val_in <= rd_val_in;
                inst_pc_in <= inst_pc_in;
                jump_pc_in <= jump_pc_in;
                jump_valid_in <= jump_valid_in;
                exception_num_in <= exception_num_in;
                exception_val_in <= exception_val_in;
                exception_valid_in <= exception_valid_in;
                store_addr_in <= store_addr_in;
                store_val_in <= store_val_in;
                store_size_in <= store_size_in;
                store_valid_in <= store_valid_in;
            end else begin
                rd_in <= 'd0;
                rd_val_in <= 'd0;
                inst_pc_in <= 'd0;
                jump_pc_in <= 'd0;
                jump_valid_in <= 'd0;
                exception_num_in <= 'd0;
                exception_val_in <= 'd0;
                exception_valid_in <= 'd0;
                store_addr_in <= 'd0;
                store_val_in <= 'd0;
                store_size_in <= 'd0;
                store_valid_in <= 'd0;
            end
        end
    end

    localparam NODATA = 0;
    localparam COMMIT = 1;
    localparam EXCEPTION = 2;
    localparam WAIT_FIFO = 3;
    var int commit;

    assign commit = (!in_valid) ? NODATA :
                    (exception_valid_in) ? EXCEPTION :
                    (store_valid_in && datafifo_full) ? WAIT_FIFO : COMMIT;
    assign execute_stall = commit == WAIT_FIFO;

    // Register Writeback
    assign rd_out = rd_in;
    assign rd_val_out = rd_val_in;
    assign rd_valid_out = (commit == COMMIT) && (rd_out != 'd0);

    assign datafifo_addr_out = store_addr_in;
    assign datafifo_val_out = store_val_in;
    assign datafifo_size_out = store_size_in;
    assign datafifo_valid_out = (commit == COMMIT) && store_valid_in;

    assign exception_num_out = exception_num_in;
    assign exception_val_out = exception_val_in;
    assign exception_pc_out = inst_pc_in;
    assign exception_valid_out = (commit == EXCEPTION);

    assign commit_valid = (commit == EXCEPTION) || (commit == COMMIT);
    assign pipeline_flush = (commit == EXCEPTION) ||
                            ((commit == COMMIT) && jump_valid_in);
    assign pipeline_pc = jump_pc_in;




endmodule
