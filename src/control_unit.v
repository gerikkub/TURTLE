`timescale 1ns / 1ns

`include "src/microcode_mod.v"

module control_unit(
    input clk,
    input reset,

    input [4:0] inst,

    output [CS_N:0] control_signals
    );

    parameter CS_N = 1;

    wire [3:0] opcode;

    reg [3:0] metadata_vec [31:0];

    reg pipeline_state;

    microcode_mod ucode_mod(
        .opcode(opcode),
        .control_signals(control_signals)
    );

    initial $readmemh("src/metadata_vector.txt", metadata_vec);

    initial begin
        pipeline_state <= 'd0;
    end

    assign opcode = (pipeline_state == 'd0) ? 'd0 : metadata_vec[inst];

    always @(posedge clk) begin

        if (reset == 'd0) begin
            pipeline_state <= 'd0;
        end else begin
            pipeline_state <= ~pipeline_state;
        end
    end

endmodule


