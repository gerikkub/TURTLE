`timescale 1ns / 1ns

`include "src/csr_reg.v"

`define CHECK(condA, condB) if (!(condA === condB)) begin $display("[%d] Failed Eq: %x === %x", `__LINE__, condA, condB); end else tests_passed++; tests_total++;

module csr_reg_sim();

    reg clk;
    reg reset;

    // Data In/Out
    reg [31:0] din;
    reg [11:0] addr;
    reg write_en;
    reg read_en;
    reg [1:0] write_type;

    wire [31:0] dout;

    // Control In
    reg trap_mie;
    reg trap_mpie;
    reg [31:0] trap_pc_in;
    reg trap_int;
    reg [30:0] trap_cause;
    reg [31:0] trap_val;
    reg trap_wr_en;

    reg set_mip_mtip;
    reg set_mip_meip;

    //Control Out
    wire mstatus_mie_out;

    wire mstatus_mpie_out;

    wire mip_msip_out;
    wire mip_mtip_out;
    wire mip_meip_out;

    wire mie_msie_out;
    wire mie_mtie_out;
    wire mie_meie_out;

    csr_reg dut(
        .clk(clk),
        .reset(reset),
        .din(din),
        .addr(addr),
        .write_en(write_en),
        .read_en(read_en),
        .write_type(write_type),
        .dout(dout),
        .trap_mie(trap_mie),
        .trap_mpie(trap_mpie),
        .trap_pc_in(trap_pc_in),
        .trap_int(trap_int),
        .trap_cause(trap_cause),
        .trap_val(trap_val),
        .trap_wr_en(trap_wr_en),
        .set_mip_mtip(set_mip_mtip),
        .set_mip_meip(set_mip_meip),
        .mstatus_mie_out(mstatus_mie_out),
        .mstatus_mpie_out(mstatus_mpie_out),
        .mip_msip_out(mip_msip_out),
        .mip_mtip_out(mip_mtip_out),
        .mip_meip_out(mip_meip_out),
        .mie_msie_out(mie_msie_out),
        .mie_mtie_out(mie_mtie_out),
        .mie_meie_out(mie_meie_out)
    );

    int csr_regs[1:22];

    reg [31:0] csr_regs_vals_expected[1:'h1000];



    initial begin

        // Data In/Out
        din = 'd0;
        addr = 'd0;
        write_en = 'd0;
        read_en = 'd0;
        write_type = 'd0;

        // Control In
        trap_mie = 'd0;
        trap_mpie = 'd0;
        trap_pc_in = 'd0;
        trap_int = 'd0;
        trap_cause = 'd0;
        trap_val = 'd0;
        trap_wr_en = 'd0;

        set_mip_mtip = 'd0;
        set_mip_meip = 'd0;

        csr_regs[1] = 'hF11;
        csr_regs[2] = 'hF12;
        csr_regs[3] = 'hF13;
        csr_regs[4] = 'hF14;
        csr_regs[5] = 'h300;
        csr_regs[6] = 'h301;
        csr_regs[7] = 'h302;
        csr_regs[8] = 'h303;
        csr_regs[9] = 'h304;
        csr_regs[10] = 'h305;
        csr_regs[11] = 'h306;
        csr_regs[12] = 'h340;
        csr_regs[13] = 'h341;
        csr_regs[14] = 'h342;
        csr_regs[15] = 'h343;
        csr_regs[16] = 'h344;
        csr_regs[17] = 'hB00;
        csr_regs[18] = 'hB01;
        csr_regs[19] = 'hB02;
        csr_regs[20] = 'hB80;
        csr_regs[21] = 'hB81;
        csr_regs[22] = 'hB82;

        for (int i = 0; i < 'h1000; i++) begin
            csr_regs_vals_expected[i] = 0;
        end

        csr_regs_vals_expected['h301] = 'h40000010;
        csr_regs_vals_expected['h305] = 'h00000004;

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
        #1

        // Check defaults
        read_en = 'd1;
        for (int i = 1; i <= 22; i++) begin
            #1
            addr = csr_regs[i];
            #1
            `CHECK(dout, csr_regs_vals_expected[csr_regs[i]]);
        end

        `CHECK(mstatus_mie_out, 'd0);

        `CHECK(mie_msie_out, 'd0);
        `CHECK(mie_mtie_out, 'd0);
        `CHECK(mie_meie_out, 'd0);

        `CHECK(mip_msip_out, 'd0);
        `CHECK(mip_mtip_out, 'd0);
        `CHECK(mip_meip_out, 'd0);

        read_en = 'd0;

        // Setup interrupts
        write_en = 'd1;

        // Mstatus
        addr = 'h300;
        din = 'h8;

        csr_regs_vals_expected['h300] = 'h8;

        @(posedge clk);
        #1

        `CHECK(mstatus_mie_out, 'd1);
        `CHECK(mstatus_mpie_out, 'd0);

        // Mie
        addr = 'h304;
        din = 'h008;

        @(posedge clk);
        #1

        `CHECK(mie_msie_out, 'd1);
        `CHECK(mie_mtie_out, 'd0);
        `CHECK(mie_meie_out, 'd0);

        din = 'h088;

        @(posedge clk);
        #1

        `CHECK(mie_msie_out, 'd1);
        `CHECK(mie_mtie_out, 'd1);
        `CHECK(mie_meie_out, 'd0);

        din = 'hFFFFFFFF;

        @(posedge clk);
        #1

        `CHECK(mie_msie_out, 'd1);
        `CHECK(mie_mtie_out, 'd1);
        `CHECK(mie_meie_out, 'd1);

        csr_regs_vals_expected['h304] = 'h888;

        // Mscratch

        addr = 'h340;
        din = 'hAABBCCDD;

        @(posedge clk);
        #1

        csr_regs_vals_expected['h340] = 'hAABBCCDD;

        // Mip

        addr = 'h344;
        din = 'h8;

        @(posedge clk);
        #1

        `CHECK(mip_msip_out, 'd1);
        `CHECK(mip_mtip_out, 'd0);
        `CHECK(mip_meip_out, 'd0);

        write_en = 'd0;

        set_mip_mtip = 'd1;

        @(posedge clk);
        #1

        `CHECK(mip_msip_out, 'd1);
        `CHECK(mip_mtip_out, 'd1);
        `CHECK(mip_meip_out, 'd0);

        set_mip_meip = 'd1;

        @(posedge clk);
        #1

        `CHECK(mip_msip_out, 'd1);
        `CHECK(mip_mtip_out, 'd1);
        `CHECK(mip_meip_out, 'd1);

        set_mip_mtip = 'd0;
        set_mip_meip = 'd0;

        @(posedge clk);
        #1

        `CHECK(mip_msip_out, 'd1);
        `CHECK(mip_mtip_out, 'd1);
        `CHECK(mip_meip_out, 'd1);

        csr_regs_vals_expected['h344] = 'h888;

        // Verify registers
        read_en = 'd1;
        for (int i = 1; i <= 22; i++) begin
            #1
            addr = csr_regs[i];
            #1
            `CHECK(dout, csr_regs_vals_expected[csr_regs[i]]);
        end

        // Trigger Exception

        trap_mie = 'd0;
        trap_mpie = mstatus_mie_out;

        trap_pc_in = 'h00008AB4;
        trap_int = 'd0;
        trap_cause = 'd2;
        trap_val = 'hFFEEDD11;
        trap_wr_en = 'd1;

        @(posedge clk);
        #1

        trap_wr_en = 'd0;

        csr_regs_vals_expected['h300] = 'h80;
        csr_regs_vals_expected['h341] = 'h00008Ab4;
        csr_regs_vals_expected['h342] = 'h2;
        csr_regs_vals_expected['h343] = 'hFFEEDD11;

        `CHECK(mstatus_mie_out, 'd0);
        `CHECK(mstatus_mpie_out, 'd1);

        `CHECK(mie_msie_out, 'd1);
        `CHECK(mie_mtie_out, 'd1);
        `CHECK(mie_meie_out, 'd1);

        `CHECK(mip_msip_out, 'd1);
        `CHECK(mip_mtip_out, 'd1);
        `CHECK(mip_meip_out, 'd1);

        // Verify registers
        read_en = 'd1;
        for (int i = 1; i <= 22; i++) begin
            #1
            addr = csr_regs[i];
            #1
            `CHECK(dout, csr_regs_vals_expected[csr_regs[i]]);
        end

        @(posedge clk);
        #1

        trap_mie = 'd0;
        trap_mpie = mstatus_mie_out;

        trap_pc_in = 'h00008A9C;
        trap_int = 'd1;
        trap_cause = 'd8;
        trap_val = 'h0;
        trap_wr_en = 'd1;

        @(posedge clk);
        #1

        trap_wr_en = 'd0;

        csr_regs_vals_expected['h300] = 'h00;
        csr_regs_vals_expected['h341] = 'h00008A9C;
        csr_regs_vals_expected['h342] = 'h80000008;
        csr_regs_vals_expected['h343] = 'h0;

        `CHECK(mstatus_mie_out, 'd0);
        `CHECK(mstatus_mpie_out, 'd0);

        `CHECK(mie_msie_out, 'd1);
        `CHECK(mie_mtie_out, 'd1);
        `CHECK(mie_meie_out, 'd1);

        `CHECK(mip_msip_out, 'd1);
        `CHECK(mip_mtip_out, 'd1);
        `CHECK(mip_meip_out, 'd1);


        // Verify registers
        read_en = 'd1;
        for (int i = 1; i <= 22; i++) begin
            #1
            addr = csr_regs[i];
            #1
            `CHECK(dout, csr_regs_vals_expected[csr_regs[i]]);
        end

        @(posedge clk);
        #1

        write_type = 'd2;
        read_en = 'd0;

        addr = 'h304; //mie

        din = 'h8; // Clear msie

        write_en = 'd1;

        csr_regs_vals_expected['h304] = 'h880;

        @(posedge clk);
        #1

        write_en = 'd0;
        read_en = 'd1;
        for (int i = 1; i <= 22; i++) begin
            #1
            addr = csr_regs[i];
            #1
            `CHECK(dout, csr_regs_vals_expected[csr_regs[i]]);
        end

        @(posedge clk);
        #1

        write_type = 'd1;
        read_en = 'd0;

        addr = 'h304;

        din = 'h8;

        write_en = 'd1;

        csr_regs_vals_expected['h304] = 'h888;

        @(posedge clk);
        #1

        write_en = 'd0;
        read_en = 'd1;

        for (int i = 1; i <= 22; i++) begin
            #1
            addr = csr_regs[i];
            #1
            `CHECK(dout, csr_regs_vals_expected[csr_regs[i]]);
        end

        $display("%0d/%0d checks passed", tests_passed, tests_total);

        $finish;
    end

endmodule
