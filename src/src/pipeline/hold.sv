`timescale 1ns/1ns

module hold #(
    parameter WIDTH = 1
    ) (
    input clk,
    input reset,

    input var [WIDTH-1:0] data_in,
    output [WIDTH-1:0] data_out,

    input input_valid,
    input stall,
    output valid_out
    );

    reg [WIDTH-1:0] data_reg;
    reg data_valid;

    wire passthrough = !stall &&
                       !data_valid &&
                       input_valid;
    assign data_out = passthrough ? data_in :
                                    data_reg;

    assign valid_out = !stall && !reset &&
                       (data_valid || input_valid);

    wire [WIDTH-1:0]data_mux = data_valid ? data_reg :
                                            data_in;
    var data_valid_mux;

    always_comb begin
        if (data_valid) begin
            if (stall) begin
                data_valid_mux = 'd1;
            end else begin
                data_valid_mux = 'd0;
            end
        end else begin
            if (input_valid && stall) begin
                data_valid_mux = 'd1;
            end else begin
                data_valid_mux = 'd0;
            end
        end

    end

    always_ff @(posedge clk) begin
        if (reset == 'd1) begin
            data_reg <= 'd0;
            data_valid <= 'd0;
        end else begin
            data_reg <= data_mux;
            data_valid <= data_valid_mux;
        end

    end

endmodule

    
