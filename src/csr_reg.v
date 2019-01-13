`timescale 1ns / 1ns

module csr_reg(
    input         clk,
    input         reset,

    // Data In/Out
    input [31:0]  din,
    input [11:0]  addr,
    input         write_en,
    input [1:0]   write_type,
    input         read_en,

    output [31:0] dout,

    // Control In
    input         trap_mie,
    input         trap_mpie,
    input [31:0]  trap_pc_in,
    input         trap_int,
    input [30:0]  trap_cause,
    input [31:0]  trap_val,
    input         trap_wr_en,

    input         set_mip_mtip,
    input         set_mip_meip,

    //Control Out
    output        mstatus_mie_out,
    output        mstatus_mpie_out,

    output        mip_msip_out,
    output        mip_mtip_out,
    output        mip_meip_out,

    output        mie_msie_out,
    output        mie_mtie_out,
    output        mie_meie_out,

    output [31:0] mtvec_out
    );

    wire [31:0] dout_temp;

    wire [31:0] wire_mvendorid;
    wire [31:0] wire_marchid;
    wire [31:0] wire_mimpid;
    wire [31:0] wire_mhartid;

    wire [31:0] wire_mstatus;
    wire [31:0] wire_misa;
    wire [31:0] wire_medelg;
    wire [31:0] wire_midelg;
    wire [31:0] wire_mie;
    wire [31:0] wire_mtvec;
    wire [31:0] wire_mcouteren;

    wire [31:0] wire_mscratch;
    wire [31:0] wire_mepc;
    wire [31:0] wire_mcause;
    wire [31:0] wire_mtval;
    wire [31:0] wire_mip;

    wire [63:0] wire_mcycle;
    wire [63:0] wire_mtime;
    wire [63:0] wire_minstret;

    // Machine-Mode Status
    reg mstatus_mie;
    reg mstatus_mpie;

    // Machine ISA
    localparam [1:0] misa_mxl = 'h1;
    localparam [25:0] misa_ext = 'h100;

    // Machine Trap-Vector Base-Address
    localparam [31:2] mtvec_base = 'h1;
    localparam [1:0] mtvec_mode = 'd0;

    // Machine Interrupt-Pending
    reg mip_msip;
    reg mip_mtip;
    reg mip_meip;

    // Machine Interrupt-Enable
    reg mie_msie;
    reg mie_mtie;
    reg mie_meie;

    // Machine-Mode Scratch
    reg [31:0] mscratch;

    // Machine Exception Program Counter
    reg [31:0] mepc;

    // Machine Cause
    reg mcause_int;
    reg [30:0] mcause_code;

    // Machine Trap Value
    reg [31:0] mtval;


    initial begin
        mstatus_mie = 'd0;
        mstatus_mpie = 'd0;

        mip_msip = 'd0;
        mip_mtip = 'd0;
        mip_meip = 'd0;

        mie_msie = 'd0;
        mie_mtie = 'd0;
        mie_meie = 'd0;

        mscratch = 'd0;

        mepc = 'd0;

        mcause_int = 'd0;
        mcause_code = 'd0;

        mtval = 'd0;
    end

    always @(posedge clk) begin
        if (reset == 'd0) begin
            mstatus_mie <= 'd0;
            mstatus_mpie <= 'd0;

            mip_msip <= 'd0;
            mip_mtip <= 'd0;
            mip_meip <= 'd0;

            mie_msie <= 'd0;
            mie_mtie <= 'd0;
            mie_meie <= 'd0;

            mscratch <= 'd0;

            mepc <= 'd0;

            mcause_int <= 'd0;
            mcause_code <= 'd0;

            mtval <= 'd0;
        end else begin
            if (trap_wr_en == 'd1) begin
                mstatus_mie <= trap_mie;
                mstatus_mpie <= trap_mpie;

                mepc <= trap_pc_in;
                mcause_int <= trap_int;
                mcause_code <= trap_cause;
                mtval <= trap_val;
            end else if (write_en == 'd1) begin
                case (addr)
                  'h300:
                    begin
                        if (write_type == 'd1) begin
                            mstatus_mie <= din[3];
                            mstatus_mpie <= din[7];
                        end else if (write_type == 'd2) begin
                            mstatus_mie <= mstatus_mie | din[3];
                            mstatus_mpie <= mstatus_mie | din[7];
                        end else if (write_type == 'd3) begin
                            mstatus_mie <= mstatus_mie & (~din[3]);
                            mstatus_mpie <= mstatus_mie & (~din[7]);
                        end else begin
                            mstatus_mie <= 'd0;
                            mstatus_mpie <= 'd0;
                        end
                    end
                  'h304:
                    begin
                        if (write_type == 'd1) begin
                            mie_msie <= din[3];
                            mie_mtie <= din[7];
                            mie_meie <= din[11];
                        end else if (write_type == 'd2) begin
                            mie_msie <= mie_msie | din[3];
                            mie_mtie <= mie_mtie | din[7];
                            mie_meie <= mie_meie | din[11];
                        end else if (write_type == 'd3) begin
                            mie_msie <= mie_msie & (~din[3]);
                            mie_mtie <= mie_mtie & (~din[7]);
                            mie_meie <= mie_meie & (~din[11]);
                        end else begin
                            mie_msie <= 0;
                            mie_mtie <= 0;
                            mie_meie <= 0;
                        end
                    end
                  //'h306: mcouteren <= din;
                  'h340: 
                    begin
                        if (write_type == 'd1) begin
                            mscratch <= din;
                        end else if (write_type == 'd2) begin
                            mscratch <= mscratch | din;
                        end else if (write_type == 'd3) begin
                            mscratch <= mscratch & (~din);
                        end else begin
                            mscratch <= 'hDEADC0DE;
                        end
                    end
                  'h341:
                    begin
                        if (write_type == 'd1) begin
                            mepc <= din & 'hFFFFFFFC;
                        end else if (write_type == 'd2) begin
                            mepc <= (mepc | din) & 'hFFFFFFFC;
                        end else if (write_type == 'd3) begin
                            mepc <= (mepc & (~din)) & 'hFFFFFFFC;
                        end else begin
                            mepc <= 'hDEADC0DE;
                        end
                    end
                  'h342:
                    begin
                        if (write_type == 'd1) begin
                            mcause_int <= din[31];
                            mcause_code <= din[30:0];
                        end else if (write_type == 'd2) begin
                            mcause_int <= mcause_int | din[31];
                            mcause_code <= mcause_code | din[30:0];
                        end else if (write_type == 'd3) begin
                            mcause_int <= mcause_int & (~din[31]);
                            mcause_code <= mcause_code & (~din[30:0]);
                        end else begin
                            mcause_int <= 'd1;
                            mcause_code <= 'h5EADC0DE;
                        end
                    end
                  'h343:
                    begin
                        if (write_type == 'd1) begin
                            mtval <= din;
                        end else if (write_type == 'd2) begin
                            mtval <= mtval | din;
                        end else if (write_type == 'd3) begin
                            mtval <= mtval & (~din);
                        end else begin
                            mtval <= 'hDEADC0DE;
                        end
                    end
                  'h344: 
                    begin
                        if (write_type == 'd1) begin
                            mip_msip <= din[3];
                            mip_mtip <= din[7];
                            mip_meip <= din[11];
                        end else if (write_type == 'd2) begin
                            mip_msip <= mip_msip | din[3];
                            mip_mtip <= mip_mtip | din[7];
                            mip_meip <= mip_meip | din[11];
                        end else if (write_type == 'd3) begin
                            mip_msip <= (mip_msip & (~din[3]));
                            mip_mtip <= (mip_mtip & (~din[7]));
                            mip_meip <= (mip_meip & (~din[11]));
                        end else begin
                            mip_msip <= 'd0;
                            mip_mtip <= 'd0;
                            mip_meip <= 'd0;
                        end
                    end
                  //'hB00: mcycle[31:0] <= din;
                  //'hB01: mtime[31:0] <= din;
                  //'hB02: minstreg[31:0] <= din;
                  //'hB80: mcycle[63:32] <= din;
                  //'hB81: mtime[63:32] <= din;
                  //'hB82: minstreg[63:32] <= din;
                endcase
            end
            if ((write_en == 'd0) ||
                (write_en == 'd1 && addr != 'h344)) begin
                mip_mtip <= mip_mtip | set_mip_mtip;
                mip_meip <= mip_meip | set_mip_meip;
            end
        end
    end

    // Control Out
    assign mstatus_mie_out = mstatus_mie;
    assign mstatus_mpie_out = mstatus_mpie;

    assign mip_msip_out = mip_msip;
    assign mip_mtip_out = mip_mtip;
    assign mip_meip_out = mip_meip;

    assign mie_msie_out = mie_msie;
    assign mie_mtie_out = mie_mtie;
    assign mie_meie_out = mie_meie;

    assign mtvec_out = {wire_mtvec[31:2], 2'b0};


    // Data Out
    assign wire_mvendorid = 'hC0DE1122;
    assign wire_marchid = 'hC0DE3344;
    assign wire_mimpid = 'hC0DE5566;
    assign wire_mhartid = 'hC0DE7788;

    assign wire_mstatus = {24'b0, mstatus_mpie, 3'b0, mstatus_mie, 3'b0};
    assign wire_misa = {misa_mxl, 4'b0, misa_ext};

    assign wire_mtvec = {mtvec_base, mtvec_mode};

    assign wire_medelg = 'd0;
    assign wire_midelg = 'd0;

    assign wire_mip = {20'b0, mip_meip, 3'b0, mip_mtip, 3'b0, mip_msip, 3'b0};
    assign wire_mie = {20'b0, mie_meie, 3'b0, mie_mtie, 3'b0, mie_msie, 3'b0};

    // TODO: Implement performance counters
    assign wire_mtime = 'd0;
    assign wire_mcycle = 'd0;
    assign wire_minstret = 'd0;

    assign wire_mcouteren = 'd0;

    assign wire_mscratch = mscratch;

    assign wire_mepc = mepc;

    assign wire_mcause = {mcause_int, mcause_code};

    assign wire_mtval = mtval;

    assign dout_temp = (addr == 'hF11) ? wire_mvendorid :
                       (addr == 'hF12) ? wire_marchid :
                       (addr == 'hF13) ? wire_mimpid :
                       (addr == 'hF14) ? wire_mhartid :
                       (addr == 'h300) ? wire_mstatus :
                       (addr == 'h301) ? wire_misa :
                       (addr == 'h302) ? wire_medelg :
                       (addr == 'h303) ? wire_midelg :
                       (addr == 'h304) ? wire_mie :
                       (addr == 'h305) ? wire_mtvec :
                       (addr == 'h306) ? wire_mcouteren :
                       (addr == 'h340) ? wire_mscratch :
                       (addr == 'h341) ? wire_mepc :
                       (addr == 'h342) ? wire_mcause :
                       (addr == 'h343) ? wire_mtval :
                       (addr == 'h344) ? wire_mip :
                       (addr == 'hB00) ? wire_mcycle[31:0] :
                       (addr == 'hB01) ? wire_mtime[31:0] :
                       (addr == 'hB02) ? wire_minstret[31:0] :
                       (addr == 'hB80) ? wire_mcycle[63:32] :
                       (addr == 'hB81) ? wire_mtime[63:32] :
                       (addr == 'hB82) ? wire_minstret[63:32] :
                       'hFFFFFFFF;

    assign dout = (read_en == 'd0) ? 'd0 : dout_temp;

endmodule


