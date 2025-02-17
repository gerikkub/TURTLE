`timescale 1ns / 1ns

module reg_file(
    input clk,
    input reset,

    input [4:0] readA,
    input [4:0] readB,

    input writeEnC,
    input [4:0] writeC,
    input [31:0] writeDataC,

    output [31:0]A,
    output [31:0]B
    );


    reg [31:0] xreg [1:31];

    initial begin
        for (int i=1; i < 32; i++) begin
            xreg[i] = 0;
        end
        xreg[2] = 'h80000800;
    end


    always @(posedge clk) begin

        if (reset == 'b0) begin
            xreg[1] <= 'd0;
            xreg[2] <= 'h80000800;
            xreg[3] <= 'd0;
            xreg[4] <= 'd0;
            xreg[5] <= 'd0;
            xreg[6] <= 'd0;
            xreg[7] <= 'd0;
            xreg[8] <= 'd0;
            xreg[9] <= 'd0;
            xreg[10] <= 'd0;
            xreg[11] <= 'd0;
            xreg[12] <= 'd0;
            xreg[13] <= 'd0;
            xreg[14] <= 'd0;
            xreg[15] <= 'd0;
            xreg[16] <= 'd0;
            xreg[17] <= 'd0;
            xreg[18] <= 'd0;
            xreg[19] <= 'd0;
            xreg[20] <= 'd0;
            xreg[21] <= 'd0;
            xreg[22] <= 'd0;
            xreg[23] <= 'd0;
            xreg[24] <= 'd0;
            xreg[25] <= 'd0;
            xreg[26] <= 'd0;
            xreg[27] <= 'd0;
            xreg[28] <= 'd0;
            xreg[29] <= 'd0;
            xreg[30] <= 'd0;
            xreg[31] <= 'd0;
        end else if (writeEnC && writeC != 'd0) begin
            xreg[writeC] <= writeDataC;
        end
    end

    assign A =  (readA == 'd0) ? 'd0 :
                xreg[readA];
    assign B =  (readB == 'd0) ? 'd0 :
                xreg[readB];

            
    wire [31:0] debug_x1;
    wire [31:0] debug_x2;
    wire [31:0] debug_x3;
    wire [31:0] debug_x4;
    wire [31:0] debug_x5;
    wire [31:0] debug_x6;
    wire [31:0] debug_x7;
    wire [31:0] debug_x8;
    wire [31:0] debug_x9;
    wire [31:0] debug_x10;
    wire [31:0] debug_x11;
    wire [31:0] debug_x12;
    wire [31:0] debug_x13;
    wire [31:0] debug_x14;
    wire [31:0] debug_x15;
    wire [31:0] debug_x16;
    wire [31:0] debug_x17;
    wire [31:0] debug_x18;
    wire [31:0] debug_x19;
    wire [31:0] debug_x20;
    wire [31:0] debug_x21;
    wire [31:0] debug_x22;
    wire [31:0] debug_x23;
    wire [31:0] debug_x24;
    wire [31:0] debug_x25;
    wire [31:0] debug_x26;
    wire [31:0] debug_x27;
    wire [31:0] debug_x28;
    wire [31:0] debug_x29;
    wire [31:0] debug_x30;
    wire [31:0] debug_x31;

    assign debug_x1 = xreg[1];
    assign debug_x2 = xreg[2];
    assign debug_x3 = xreg[3];
    assign debug_x4 = xreg[4];
    assign debug_x5 = xreg[5];
    assign debug_x6 = xreg[6];
    assign debug_x7 = xreg[7];
    assign debug_x8 = xreg[8];
    assign debug_x9 = xreg[9];
    assign debug_x10 = xreg[10];
    assign debug_x11 = xreg[11];
    assign debug_x12 = xreg[12];
    assign debug_x13 = xreg[13];
    assign debug_x14 = xreg[14];
    assign debug_x15 = xreg[15];
    assign debug_x16 = xreg[16];
    assign debug_x17 = xreg[17];
    assign debug_x18 = xreg[18];
    assign debug_x19 = xreg[19];
    assign debug_x20 = xreg[20];
    assign debug_x21 = xreg[21];
    assign debug_x22 = xreg[22];
    assign debug_x23 = xreg[23];
    assign debug_x24 = xreg[24];
    assign debug_x25 = xreg[25];
    assign debug_x26 = xreg[26];
    assign debug_x27 = xreg[27];
    assign debug_x28 = xreg[28];
    assign debug_x29 = xreg[29];
    assign debug_x30 = xreg[30];
    assign debug_x31 = xreg[31];

endmodule
