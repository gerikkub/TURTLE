`timescale 1ns / 1ns

`include "utils/decoder.sv"

module commit(
    input clk,
    input reset,

    // Execute Inputs
    input execute_valid,

    input [4:0]execute_rd,
    input [31:0]execute_rd_val,

    input [31:0]execute_inst_pc,
    input [31:0]execute_jump_pc,
    input execute_jump_valid,

    input [5:0]execute_exception_num,
    input [31:0]execute_exception_val,
    input execute_exception_valid,

    input [31:0]execute_store_addr,
    input [31:0]execute_store_val,
    input [1:0]execute_store_size,
    input execute_store_valid,

    // CSR Write
    input [11:0]execute_csr_write_addr,
    input [31:0]execute_csr_write_val,
    input execute_csr_write_valid,

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
    output [31:0]pipeline_pc,
    output [31:0]active_rd,

    // Axi-Lite CSR Write
    // Note: This could probably be a simpler bus
    output [11:0]axil_csr_awaddr,
    output axil_csr_awvalid,
    input axil_csr_awready,
    output [31:0]axil_csr_wdata,
    output axil_csr_wvalid,
    input axil_csr_wready,
    input [2:0]axil_csr_bresp,
    input axil_csr_bvalid,
    output axil_csr_bready


    // TODO: Exceptions
    // TODO: Interrupts
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
    reg [11:0]csr_write_addr_in;
    reg [31:0]csr_write_val_in;
    reg csr_write_valid_in;

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
            csr_write_addr_in <= 'd0;
            csr_write_val_in <= 'd0;
            csr_write_valid_in <= 'd0;

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
                csr_write_addr_in <= execute_csr_write_addr;
                csr_write_val_in <= execute_csr_write_val;
                csr_write_valid_in <= execute_csr_write_valid;
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
                csr_write_addr_in <= csr_write_addr_in;
                csr_write_val_in <= csr_write_val_in;
                csr_write_valid_in <= csr_write_valid_in;
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
                csr_write_addr_in <= 'd0;
                csr_write_val_in <= 'd0;
                csr_write_valid_in <= 'd0;
            end
        end
    end

    localparam NODATA = 0;
    localparam COMMIT = 1;
    localparam EXCEPTION = 2;
    localparam WAIT_FIFO = 3;
    localparam WAIT_CSRW = 4;
    var int commit_kind;

    assign commit_kind = (!in_valid) ? NODATA :
                         (exception_valid_in) ? EXCEPTION :
                         (store_valid_in && datafifo_full) ? WAIT_FIFO :
                         (csr_write_valid_in) ? WAIT_CSRW : COMMIT;
    assign execute_stall = (commit_kind == WAIT_FIFO) || ((commit_kind == WAIT_CSRW) && (!csr_write_complete));

    // Register Writeback
    assign rd_out = rd_in;
    assign rd_val_out = rd_val_in;
    assign rd_valid_out = ((commit_kind == COMMIT) ||
                           ((commit_kind == WAIT_CSRW) && csr_write_complete && (!csr_write_exception_valid))) &&
                          (rd_out != 'd0);

    assign datafifo_addr_out = store_addr_in;
    assign datafifo_val_out = store_val_in;
    assign datafifo_size_out = store_size_in;
    assign datafifo_valid_out = (commit_kind == COMMIT) && store_valid_in;

    assign exception_num_out = (commit_kind == EXCEPTION) ? exception_num_in :
                               (commit_kind == WAIT_CSRW) ? csr_write_exception_num : 'd0;
    assign exception_val_out = (commit_kind == EXCEPTION) ? exception_val_in :
                               (commit_kind == WAIT_CSRW) ? csr_write_exception_val : 'd0;
    assign exception_pc_out = inst_pc_in;
    assign exception_valid_out = (commit_kind == EXCEPTION) ? 'd1 :
                                 (commit_kind == WAIT_CSRW) ? csr_write_exception_valid : 'd0;

    assign commit_valid = (commit_kind == EXCEPTION) || (commit_kind == COMMIT) ||
                          ((commit_kind == WAIT_CSRW) && csr_write_complete);
    assign pipeline_flush = (commit_kind == EXCEPTION) ||
                            ((commit_kind == COMMIT) && jump_valid_in) ||
                            ((commit_kind == WAIT_CSRW) && csr_write_exception_valid);
    assign pipeline_pc = jump_pc_in;

    wire [31:0]rd_decode;
    decoder #(5) rd_decoder(
        .bus_in(rd_in),
        .data_out(rd_decode));

    // Assign an active RD while waiting for CSR_COMMIT
    // Signal should not be value while rd_valid_out is set
    assign active_rd = (commit_kind != WAIT_CSRW) ? 'd0 :
                       (cw_state != CSR_COMMIT) ? rd_decode : 'd0;

    // Axi-Lite CSW Write SM

    localparam EXCEPTION_INVALID_INST = 'd2;

    wire csr_write_complete;
    wire [5:0]csr_write_exception_num = EXCEPTION_INVALID_INST;
    wire [31:0]csr_write_exception_val = 'd0;
    wire csr_write_exception_valid;

    reg csr_write_exception_buffer;
    assign csr_write_exception_valid = csr_write_exception_buffer;

    enum int unsigned {
        IDLE,
        WADDR,
        WDATA,
        BRESP,
        CSR_COMMIT
    } cw_state, cw_next_state;

    assign csr_write_complete = cw_state == CSR_COMMIT;

    assign axil_csr_awaddr = csr_write_addr_in;
    assign axil_csr_awvalid = cw_state == WADDR;

    assign axil_csr_wdata = csr_write_val_in;
    assign axil_csr_wvalid = cw_state == WDATA;

    assign axil_csr_bready = cw_state == BRESP;

    // Buffer BRESP exception valid for a cycle
    always_ff @(posedge clk) begin
        if (reset == 'd1)
            csr_write_exception_buffer <= 'd0;
        else
            if (cw_state == BRESP)
                csr_write_exception_buffer <= axil_csr_bvalid && (axil_csr_bresp != 'd0);
            else
                csr_write_exception_buffer <= 'd0;
    end

    always_comb begin
        if (cw_state == IDLE)
            if (commit_kind == WAIT_CSRW)
                cw_next_state = WADDR;
            else
                cw_next_state = IDLE;
        else if(cw_state == WADDR)
            if (axil_csr_awready == 'd1)
                cw_next_state = WDATA;
            else
                cw_next_state = WADDR;
        else if (cw_state == WDATA)
            if (axil_csr_wready == 'd1)
                cw_next_state = BRESP;
            else
                cw_next_state = WDATA;
        else if(cw_state == BRESP)
            if (axil_csr_bvalid == 'd1)
                cw_next_state = CSR_COMMIT;
            else
                cw_next_state = BRESP;
        else //(cw_state == CSR_COMMIT
            cw_next_state = IDLE;
    end

    always_ff @(posedge clk) begin
        if (reset == 'd1) begin
            cw_state <= IDLE;
        end else begin
            cw_state <= cw_next_state;
        end
    end


endmodule
