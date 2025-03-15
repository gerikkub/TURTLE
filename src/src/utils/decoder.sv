`timescale 1ns / 1ns

`ifndef DECODER_SV
`define DECODER_SV

module decoder #(
    parameter WIDTH = 4
    ) (
    input var [WIDTH-1:0] bus_in,
    output [(2 ** WIDTH)-1:0] data_out
    );

    assign data_out = 1 << bus_in;

endmodule

`endif
