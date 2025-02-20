`timescale 1ns / 1ns


module instruction_fetch(
    input clk,
    input reset,

    output [31:0] mem_fetch_addr,
    output mem_fetch_addr_en,
    input [31:0] mem_inst_in,
    input mem_inst_valid,

    input override_pc,
    input [31:0]override_pc_addr,

    output [31:0]inst,
    output [31:0]inst_pc,
    output valid,
    input stall

    // TODO
    //output [31:0] exception
    );

    enum int unsigned {
        // Fetch an instruction over the bus and present it if available
        FETCH_AND_PRESENT,
        // Present an instruction if it was not accepted on a previous cycle
        PRESENT
    } state, next_state;

    reg [31:0] fetch_addr;
    reg [31:0] fetch_inst;

    wire [31:0] fetch_addr_w;
    wire [31:0] inst_mux;

    // Fetch memory address
    assign mem_fetch_addr = fetch_addr;
    assign mem_fetch_addr_en = state == FETCH_AND_PRESENT &&
                               (~override_pc) &&
                               (~reset);

    // Fetch Address Wire
    assign fetch_addr_w = valid ? fetch_addr + 'd4 :
                          override_pc ? override_pc_addr :
                          fetch_addr;

    // Intstruction Fetch Output
    assign valid = ((state == FETCH_AND_PRESENT && mem_inst_valid) ||
                    (state == PRESENT)) &&
                   (~override_pc) &&
                   (~reset) &&
                   (~stall);

    assign inst_mux = state == FETCH_AND_PRESENT ? mem_inst_in :
                                                   fetch_inst;
    assign inst = valid ? inst_mux : 'hc0defec4;
    assign inst_pc = fetch_addr;

    // Advance fetch state machine
    always_comb begin
        if (state == FETCH_AND_PRESENT) begin
            if (mem_inst_valid && stall)
                next_state = PRESENT;
            else
                next_state = FETCH_AND_PRESENT;
        end else begin
            if ((~stall) || override_pc)
                next_state = FETCH_AND_PRESENT;
            else
                next_state = PRESENT;
        end
    end

    always_ff @(posedge clk) begin
        if (reset == 'd1) begin
            fetch_addr <= 'd0;
            fetch_inst <= 'd0;
            state <= FETCH_AND_PRESENT;
        end else begin
            state <= next_state;
            fetch_addr <= fetch_addr_w;
            if (mem_inst_valid)
                fetch_inst <= mem_inst_in;
            else
                fetch_inst <= fetch_inst;
        end
    end


endmodule


