`timescale 1ns / 1ns


module instruction_fetch(
    input clk,
    input reset,
    input [31:0]reset_address,

    output [31:0] mem_fetch_addr,
    output mem_fetch_addr_en,
    input [31:0] mem_inst_in,
    input mem_inst_valid,
    input mem_inst_access_fault,

    input override_pc,
    input [31:0]override_pc_addr,

    output [31:0]inst,
    output [31:0]inst_pc,
    output valid,
    output [5:0]exception_num,
    output exception_valid,

    input stall
    );

    reg [31:0] fetch_addr;

    wire [31:0] fetch_addr_w;

    // Should perform a memory request
    wire should_fetch = ~reset;

    // Should present an output
    wire should_present = ~reset &&
                          ~override_pc &&
                          (mem_inst_valid && ~stall);

    // Calculate exceptions
    wire addr_misalign_exception = fetch_addr_w[1:0] != 'b00;
    wire addr_access_fault_exception = mem_inst_access_fault;
    wire have_exception = addr_misalign_exception |
                          addr_access_fault_exception;

    assign exception_num = addr_misalign_exception ? 'd0 :
                           addr_access_fault_exception ? 'd1 :
                           'd0;

    // Fetch memory address
    assign mem_fetch_addr = fetch_addr;
    assign mem_fetch_addr_en = should_fetch && ~addr_misalign_exception;

    // Calculate fetch address for the next cycle
    assign fetch_addr_w = override_pc ? override_pc_addr :
                          should_present ? fetch_addr + 'd4 :
                          fetch_addr;

    // Intstruction Fetch Output
    assign valid = should_present;
    assign exception_valid = valid && have_exception;

    assign inst = ~should_present ? 'h000c0de0 :
                  have_exception ? 'h001c0de0 :
                  mem_inst_in;
                
    assign inst_pc = fetch_addr;

    always_ff @(posedge clk) begin
        if (reset == 'd1) begin
            fetch_addr <= reset_address;
        end else begin
            fetch_addr <= fetch_addr_w;
        end
    end


endmodule


