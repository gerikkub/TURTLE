`timescale 1ns / 1ns

`include "src/reg_file.v"

`define CHECK(condA, condB) if (!(condA === condB)) begin $display("[%d] Failed Eq: %x === %x", `__LINE__, condA, condB); end else tests_passed++; tests_total++;

module reg_file_sim();

    reg clk = 'd0;
    reg reset = 'd1;

    reg [4:0] readA = 'd0;
    reg [4:0] readB = 'd0;

    reg writeEnC = 'd0;
    reg [4:0] writeC = 'd0;
    reg [31:0] writeDataC = 'd0;

    wire [31:0] A;
    wire [31:0] B;

    reg_file reg_file(
        .clk(clk),
        .reset(reset),
        .readA(readA),
        .readB(readB),
        .writeEnC(writeEnC),
        .writeC(writeC),
        .writeDataC(writeDataC),
        .A(A),
        .B(B)
    );

    initial begin
        clk = 1'b0;
        reset = 1'b0;
        repeat(4) #10 clk = ~clk;
        reset = 1'b1;
        forever #10 clk = ~clk;
    end

    initial begin

        int tests_passed;
        int tests_total;

        tests_passed = 0;
        tests_total = 0;

        $dumpfile("test.vcd");
        $dumpvars;

        @(posedge reset);

        readA = 'd0;
        readB = 'd0;

        `CHECK(A, 'd0);
        `CHECK(B, 'd0);

        for (int i = 1; i < 32; i++) begin
            readA = i;
            `CHECK(A, 'd0);
        end

        readA = 'd0;
        readB = 'd0;

        writeC = 'd1;
        writeDataC = 'hAC;

        @(posedge clk);

        readA = 'd1;
        `CHECK(A, 'd0);

        writeEnC = 'd1;

        @(posedge clk);

        readA = 'd1;
        `CHECK(A, 'hAC);


        for (int i = 2; i < 32; i++) begin
            writeEnC = 'd0;
            writeC = i;
            readA = i;
            readB = i;
            writeDataC = i + 7;
            @(posedge clk);
            #1
            `CHECK(A, 'd0);
            `CHECK(B, 'd0);
            
            writeEnC = 'd1;
            @(posedge clk);
            #1
            `CHECK(A, i + 7);
            `CHECK(B, i + 7);
        end

        writeEnC = 'd0;

        // Check always 0 register
        readA = 'd0;

        @(posedge clk);
        `CHECK(A, 'd0);

        writeDataC = 'h98;
        writeEnC = 'd1;
        writeC = 'd0;

        @(posedge clk);
        readA = 'd0;
        #1
        `CHECK(A, 'd0);

        readA = 'd0;
        readB = 'd0;

        writeDataC = 'h56;
        writeEnC = 'd1;
        writeC = 'd1;

        @(posedge clk);
        //@(posedge clk);
        #1

        writeDataC = 'hC2;
        writeEnC = 'd1;
        writeC = 'd2;

        //@(posedge clk);
        @(posedge clk);
        #1

        writeEnC = 'd0;

        readA = 'd1;
        readB = 'd2;
        
        #1

        `CHECK(A, 'h56);
        `CHECK(B, 'hC2);

        @(posedge clk);

        $display("%0d/%0d checks passed", tests_passed, tests_total);

        $finish;
    end

endmodule


