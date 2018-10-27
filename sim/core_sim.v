`timescale 1ns / 1ns

`include "src/core.v"

`define CHECK(condA, condB) if (!(condA === condB)) begin $display("[%d] Failed Eq: %x === %x", `__LINE__, condA, condB); end else tests_passed++; tests_total++;

module core_sim();

    reg clk;
    reg reset;

    reg [31:0] mem_din;
    reg ext_interrupt;

    wire [31:0] mem_dout;
    wire [31:0] mem_addr;
    wire mem_read_en;
    wire mem_write_en;
    wire [1:0] mem_width;

    core dut(
        .clk(clk),
        .reset(reset),
        .mem_din(mem_din),
        .mem_dout(mem_dout),
        .mem_addr(mem_addr),
        .mem_read_en(mem_read_en),
        .mem_write_en(mem_write_en),
        .mem_width(mem_width),
        .ext_interrupt(ext_interrupt)
    );

    reg [31:0] rom [1023:0];
    reg [7:0] ram [2051:0];

    //initial $readmemh("asm/tests/op-imm/op-imm.bin.mem", rom);
    //initial $readmemh("asm/tests/op/op.bin.mem", rom);
    //initial $readmemh("asm/tests/jalr/jalr.bin.mem", rom);
    //initial $readmemh("asm/tests/jal/jal.bin.mem", rom);
    //initial $readmemh("asm/tests/branch/branch.bin.mem", rom);
    //initial $readmemh("asm/tests/load/load.bin.mem", rom);
    //initial $readmemh("asm/tests/store/store.bin.mem", rom);
    //initial $readmemh("asm/tests/lui_auipc/lui_auipc.bin.mem", rom);

    initial $readmemh("c/tests/main.bin.mem", rom);


    wire [31:0] rom_0;
    wire [31:0] rom_4;

    reg [7:0] ram_0;
    reg [7:0] ram_8;
    reg [7:0] ram_16;
    reg [7:0] ram_24;

    wire [7:0] ram_addr_0;
    wire [7:0] ram_addr_1;
    wire [7:0] ram_addr_2;
    wire [7:0] ram_addr_3;
    wire [7:0] ram_addr_4;
    wire [7:0] ram_addr_5;
    wire [7:0] ram_addr_6;
    wire [7:0] ram_addr_7;
    wire [7:0] ram_addr_8;

    assign rom_0 = rom[0];
    assign rom_4 = rom[1];

    assign ram_addr_0 = ram[0];
    assign ram_addr_1 = ram[1];
    assign ram_addr_2 = ram[2];
    assign ram_addr_3 = ram[3];
    assign ram_addr_4 = ram[4];
    assign ram_addr_5 = ram[5];
    assign ram_addr_6 = ram[6];
    assign ram_addr_7 = ram[7];
    assign ram_addr_8 = ram[8];

    initial begin
        for (int i = 0; i < 2052; i++) begin
            ram[i] = 'd0;
        end
    end

    initial begin

        mem_din = 'd0;
        ext_interrupt = 'd0;

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

        repeat(20000) @(posedge clk);

        $display("%0d/%0d checks passed", tests_passed, tests_total);

        $finish;
    end

    always @(mem_read_en,mem_addr) begin
        if (mem_read_en == 'd1) begin
            if (mem_addr[31] == 'd0) begin
                if (mem_addr[30:0] < 'd4096) begin
                    mem_din = rom[mem_addr >> 2];
                end else begin
                    mem_din = 'd0;
                end
            end else begin
                if (mem_addr[30:0] < ('d2052 - 'd4)) begin
                    ram_0 = ram[mem_addr[7:0]];
                    ram_8 = ram[mem_addr[7:0] + 'd1];
                    ram_16 = ram[mem_addr[7:0] + 'd2];
                    ram_24 = ram[mem_addr[7:0] + 'd3];
                end else begin
                    ram_0 = 'd0;
                    ram_8 = 'd0;
                    ram_16 = 'd0;
                    ram_24 = 'd0;
                end
                mem_din = {ram_0,ram_8,ram_16,ram_24};
            end
        end else begin
            mem_din = 'd0;
        end
    end

    always @(posedge clk) begin
        if (mem_write_en == 'd1) begin
            if (mem_addr[31] == 'd1) begin
                case (mem_width)
                    'b00: begin
                        ram[mem_addr[7:0]] = mem_dout[7:0];
                    end
                    'b01: begin
                        ram[mem_addr[7:0]] = mem_dout[7:0];
                        ram[mem_addr[7:0] + 'd1] = mem_dout[15:8];
                    end
                    'b10: begin
                        ram[mem_addr[7:0]] = mem_dout[7:0];
                        ram[mem_addr[7:0] + 'd1] = mem_dout[15:8];
                        ram[mem_addr[7:0] + 'd2] = mem_dout[23:16];
                        ram[mem_addr[7:0] + 'd3] = mem_dout[31:24];
                    end
                    'b11: begin
                        $display("ERR. mem_width on write is invalid");
                        $finish;
                    end
                endcase
            end
        end
    end

endmodule
