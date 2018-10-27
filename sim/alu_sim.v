`timescale 1ns / 1ns

`include "src/alu.v"

`define CHECK(condA, condB) if (!(condA === condB)) begin $display("[%d] Failed Eq: %x === %x", `__LINE__, condA, condB); end else tests_passed++; tests_total++;

module alu_sim();

    reg [31:0] A;
    reg [31:0] B;
    reg [2:0] ctrl;

    wire [31:0] out;

    reg signed [31:0] Asigned;
    reg [31:0] temp;

    alu dut(
        .A(A),
        .B(B),
        .ctrl(ctrl),
        .out(out)
    );

    initial begin

        int tests_passed;
        int tests_total;

        tests_passed = 0;
        tests_total = 0;

        $dumpfile("test.vcd");
        $dumpvars;

        // Add
        A = 'd0;
        B = 'd0;
        ctrl = 'd0;

        #1
        `CHECK(out, 'd0);

        A = 'd3;
        B = 'd8;
        ctrl = 'd0;

        #1
        `CHECK(out, 'd11);

        A = 'hFFFFFFFE;
        B = 'hFFFFFFFF;
        ctrl = 'd0;

        #1
        `CHECK(out, 'hFFFFFFFD);

        // Shift Left
        A = 'hAC98BC63;
        B = 'd0;
        ctrl = 'd1;

        #1
        `CHECK(out, 'hAC98BC63);

        A = 'hAC98BC63;
        B = 'd1;
        ctrl = 'd1;

        #1
        `CHECK(out, 'h593178c6);

        A = 'hAC98BC63;
        B = 'd2;
        ctrl = 'd1;

        #1
        `CHECK(out, 'hb262f18c);

        A = 'hAC98BC63;
        B = 'd3;
        ctrl = 'd1;

        #1
        `CHECK(out, 'h64c5e318);

        A = 'hAC98BC63;
        B = 'd4;
        ctrl = 'd1;

        #1
        `CHECK(out, 'hc98bc630);

        for (int i = 0; i < 32; i++) begin
            
            A = 'hAC98BC63;
            B = i;
            ctrl = 'd1;

            #1
            `CHECK(out, A << i);
        end

        // Set less than

        A = 'd5;
        B = 'd8;
        ctrl = 'd2;

        #1
        `CHECK(out, 'd1);

        A = 'd9;
        B = 'd7;
        ctrl = 'd2;

        #1
        `CHECK(out, 'd0);

        A = -'d9;
        B = 'd1;
        ctrl = 'd2;

        #1
        `CHECK(out, 'd1);

        A = -'d5;
        B = -'d6;
        ctrl = 'd2;

        #1
        `CHECK(out, 'd0);

        A = -'d8;
        B = -'d2;
        ctrl = 'd2;

        #1
        `CHECK(out, 'd1);

        A = 'd2;
        B = -'d10;
        ctrl = 'd2;

        #1
        `CHECK(out, 'd0);
        
        // Set less than unsigned

        A = 'd5;
        B = 'd8;
        ctrl = 'd3;

        #1
        `CHECK(out, 'd1);

        A = 'd9;
        B = 'd7;
        ctrl = 'd3;

        #1
        `CHECK(out, 'd0);

        A = -'d9;
        B = 'd1;
        ctrl = 'd3;

        #1
        `CHECK(out, 'd0);

        A = -'d5;
        B = -'d6;
        ctrl = 'd3;

        #1
        `CHECK(out, 'd0);

        A = -'d8;
        B = -'d2;
        ctrl = 'd3;

        #1
        `CHECK(out, 'd1);

        A = 'd2;
        B = -'d10;
        ctrl = 'd3;

        #1
        `CHECK(out, 'd1);

        // Xor
        A = 'hFFA5C600;
        B = 'hF0F0F0F0;
        ctrl = 'd4;

        #1
        `CHECK(out, 'h0F5536F0);

        A = 'hA5BC9B0E;
        B = 'hA5BC9B0E;
        ctrl = 'd4;

        #1
        `CHECK(out, 'd0);

        A = 'hA5A5A5A5;
        B = 'h5A5A5A5A;
        ctrl = 'd4;

        #1
        `CHECK(out, 'hFFFFFFFF);

        // Shift Right Logical

        for (int i = 0; i < 32; i++) begin

            A = 'hAC98BC63;
            B = i;
            ctrl = 'd5;

            #1
            `CHECK(out, A >> i);
        end

        //Shift Right Arithmetic

        for (int i = 0; i < 32; i++) begin

            A = 'hAC98BC63;
            B = i | 'h400;
            ctrl = 'd5;

            #1
            Asigned = A;
            temp = Asigned >>> i;
            `CHECK(out, temp);
        end

        for (int i = 0; i < 32; i++) begin

            A = 'h2C98BC63;
            B = i | 'h400;
            ctrl = 'd5;

            #1
            Asigned = A;
            temp = Asigned >>> i;
            `CHECK(out, temp);
        end

        // Or

        A = 'hFC84723B;
        B = 'hB97D3CE2;
        ctrl = 'd6;

        #1
        `CHECK(out, 'hFDFD7EFB);

        A = 'h00000000;
        B = 'hABCD4321;
        ctrl = 'd6;

        #1
        `CHECK(out, 'hABCD4321);

        A = 'hFFFFFFFF;
        B = 'h1234ABCD;
        ctrl = 'd6;

        #1
        `CHECK(out, 'hFFFFFFFF);

        // And

        A = 'hFC4592BE;
        B = 'hBEA65CA9;
        ctrl = 'd7;

        #1
        `CHECK(out, 'hBC0410A8);

        A = 'hFFFFFFFF;
        B = 'h1234CDEF;
        ctrl = 'd7;

        #1
        `CHECK(out, 'h1234CDEF);

        A = 'h00000000;
        B = 'h5642BDEA;
        ctrl = 'd7;

        #1
        `CHECK(out, 'h00000000);


        $display("%0d/%0d checks passed", tests_passed, tests_total);

        $finish;
    end
endmodule
