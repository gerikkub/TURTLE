`timescale 1ns / 1ns

`include "src/control_unit.v"
`include "src/alu.v"
`include "src/reg_file.v"
`include "src/pc.v"
`include "src/cs_mapper_mod.v"
`include "src/csr_reg.v"


module core(
    input clk,
    input reset,

    input [31:0] mem_din,
    output [31:0] mem_dout,
    output [31:0] mem_addr,
    output mem_read_en,
    output mem_write_en,
    output [1:0] mem_width,

    input mem_csr_read_en,
    input mem_csr_write_en,

    input ext_interrupt
    );

    wire cs_inst_write_en;

    reg [31:0] instruction;

    wire [4:0] inst_rd;
    wire [4:0] inst_rs1;
    wire [4:0] inst_rs2;

    wire [6:0] inst_opcode;
    wire [2:0] inst_funct3;
    wire [6:0] inst_funct7;

    wire [31:0] inst_imm_i;
    wire [31:0] inst_imm_s;
    wire [31:0] inst_imm_b;
    wire [31:0] inst_imm_u;
    wire [31:0] inst_imm_j;

    wire [11:0] inst_csr_addr;
    wire [31:0] inst_csr_zimm;

    assign inst_rd = instruction[11:7];
    assign inst_rs1 = instruction[19:15];
    assign inst_rs2 = instruction[24:20];

    assign inst_opcode = instruction[6:0];
    assign inst_funct3 = instruction[14:12];
    assign inst_funct7 = instruction[31:25];

    assign inst_imm_i = {{21{instruction[31]}}, instruction[30:25], instruction[24:21], instruction[20]};
    assign inst_imm_s = {{21{instruction[31]}}, instruction[30:25], instruction[11:8], instruction[7]};
    assign inst_imm_b = {{20{instruction[31]}}, instruction[7], instruction[30:25], instruction[11:8], 1'b0};
    assign inst_imm_u = {instruction[31:12], 12'b0};
    assign inst_imm_j = {{12{instruction[31]}}, instruction[19:12], instruction[20], instruction[30:25], instruction[24:21], 1'b0};

    assign inst_csr_addr = instruction[31:20];
    assign inst_csr_zimm = {27'b0, instruction[19:15]};


    always @(posedge clk) begin
        if (reset == 'd0) begin
            instruction <= 'd0;
        end else begin
            if (cs_inst_write_en == 'd1) begin
                instruction <= mem_din;
            end else begin
                instruction <= instruction;
            end
        end
    end

    // CSR


    reg [31:0] dummy_mepc;

    wire cs_csr_source;

    wire [11:0] csr_addr;
    wire [31:0] csr_din;
    wire [31:0] csr_dout;
    wire csr_write_en;
    wire [1:0] csr_write_type;
    wire csr_read_en;

    wire csr_trap_mie;
    wire csr_trap_mpie;
    wire [31:0] csr_trap_pc_in;
    wire csr_trap_int;
    wire [30:0] csr_trap_cause;
    wire [31:0] csr_trap_val;
    wire csr_trap_wr_en;

    wire csr_set_mip_mtip;
    wire csr_set_mip_meip;

    wire csr_mstatus_mie_out;
    wire csr_mstatus_mpie_out;

    wire csr_mip_msip_out;
    wire csr_mip_mtip_out;
    wire csr_mip_meip_out;
    wire csr_mie_msie_out;
    wire csr_mie_mtie_out;
    wire csr_mie_meie_out;
    wire [31:0] csr_mtvec_out;

    wire csr_inst_write_en;
    wire [1:0] csr_inst_write_type;
    wire csr_inst_read_en;

    wire [11:0] csr_inst_addr;
    wire [31:0] csr_inst_din;

    csr_reg csr_reg(
        .clk(clk),
        .reset(reset),
        .din(csr_din),
        .addr(csr_addr),
        .write_en(csr_write_en),
        .write_type(csr_write_type),
        .read_en(csr_read_en),
        .dout(csr_dout),
        .trap_mie(csr_trap_mie),
        .trap_mpie(csr_trap_mpie),
        .trap_pc_in(csr_trap_pc_in),
        .trap_int(csr_trap_int),
        .trap_cause(csr_trap_cause),
        .trap_val(csr_trap_val),
        .trap_wr_en(csr_trap_wr_en),
        .set_mip_mtip(csr_set_mip_mtip),
        .set_mip_meip(csr_set_mip_meip),
        .mstatus_mie_out(csr_mstatus_mie_out),
        .mstatus_mpie_out(csr_mstatus_mpie_out),
        .mip_msip_out(csr_mip_msip_out),
        .mip_mtip_out(csr_mip_mtip_out),
        .mip_meip_out(csr_mip_meip_out),
        .mie_msie_out(csr_mie_msie_out),
        .mie_mtie_out(csr_mie_mtie_out),
        .mie_meie_out(csr_mie_meie_out),
        .mtvec_out(csr_mtvec_out)
    );

    assign csr_addr = (cs_csr_source == 'd0) ? mem_din[11:0] : csr_inst_addr;
    assign csr_din = (cs_csr_source == 'd0) ? mem_addr : csr_inst_din;
    assign csr_write_en = (cs_csr_source == 'd0) ? mem_csr_write_en : csr_inst_write_en;
    assign csr_read_en = (cs_csr_source == 'd0) ? mem_csr_read_en : csr_inst_read_en;
    assign csr_write_type = (cs_csr_source == 'd0) ? 'd0 : csr_inst_write_type;


    assign csr_inst_addr = inst_csr_addr;

    assign csr_inst_din = (inst_funct3[2] == 'd0) ? reg_file_rs1 : inst_csr_zimm;

    // Not CSRRS or CSRRC with rd==x0
    assign csr_inst_write_en = ~(inst_funct3[1] == 'b1 && inst_rd == 'd0);

    // Not CSRRW with rs1==x0
    assign csr_inst_read_en = ~(inst_funct3[1:0] == 'b01 && inst_rs1 == 'd0);

    assign csr_write_type = inst_funct3[1:0];



    // Memory Bus

    wire [1:0]cs_mem_addr_sel;
    wire [1:0]cs_mem_dout_sel;
    wire cs_mem_read_en;
    wire cs_mem_write_en;
    wire cs_mem_width_sel;

    wire [31:0] mem_data8_signed;
    wire [31:0] mem_data8_unsigned;
    wire [31:0] mem_data16_signed;
    wire [31:0] mem_data16_unsigned;

    assign mem_addr = (cs_mem_addr_sel == 'd0) ? pc_out :
                  (cs_mem_addr_sel == 'd1) ? alu_out :
                  (cs_mem_addr_sel == 'd2) ? csr_mtvec_out : // Trap Base
                  'hFFFFFFFF;

    assign mem_dout = reg_file_rs2;
    assign mem_read_en = cs_mem_read_en;
    assign mem_write_en = cs_mem_write_en;

    assign mem_width = (cs_mem_width_sel == 'd0) ? 2'd2 :
                       inst_funct3[1:0];

    assign mem_data8_signed = {{24{mem_din[7]}}, mem_din[7:0]};
    assign mem_data8_unsigned = {24'b0, mem_din[7:0]};

    assign mem_data16_signed = {{16{mem_din[15]}}, mem_din[15:0]};
    assign mem_data16_unsigned = {16'b0, mem_din[15:0]};

    // ALU

    wire cs_alu_twos_b;
    wire [1:0]cs_alu_b_sel;
    wire [1:0]cs_alu_ctrl_sel;

    wire alu_branch_inv;
    wire br_take_branch;
    wire br_eq;
    wire br_lt;
    wire br_ltu;

    wire [31:0] alu_a;
    wire [31:0] alu_b;
    wire [2:0] alu_ctrl;
    wire [31:0] alu_out;

    wire [31:0] alu_b_temp;
    wire [2:0] alu_branch_sel;

    alu alu(
        .A(alu_a),
        .B(alu_b),
        .ctrl(alu_ctrl),
        .out(alu_out)
    );

    assign alu_a = reg_file_rs1; // Reg out 1

    assign alu_b_temp = (cs_alu_b_sel == 'd0) ? reg_file_rs2 :
                        (cs_alu_b_sel == 'd1) ? inst_imm_i :
                        (cs_alu_b_sel == 'd2) ? inst_imm_s :
                        'hFFFFFFFF;

    assign alu_b =  (cs_alu_twos_b == 'd0) ? alu_b_temp :
                    ((inst_funct7[5] == 'd0 && cs_alu_ctrl_sel == 'd0) || (alu_branch_inv == 'd0 && cs_alu_ctrl_sel == 'd1)) ? alu_b_temp :
                    (~alu_b_temp) + 'b1;

    assign alu_ctrl = (cs_alu_ctrl_sel == 'd0) ? inst_funct3 :
                      (cs_alu_ctrl_sel == 'd1) ? alu_branch_sel :
                      (cs_alu_ctrl_sel == 'd2) ? 'b000 :
                      'b111;


    assign alu_branch_inv = (cs_alu_ctrl_sel == 'd1 && (inst_funct3 == 'd0 || inst_funct3 == 'd1));

    assign alu_branch_sel = (inst_funct3 == 'd0) ? 'b000 :
                            (inst_funct3 == 'd1) ? 'b000 :
                            (inst_funct3 == 'd4) ? 'b010 :
                            (inst_funct3 == 'd5) ? 'b010 :
                            (inst_funct3 == 'd6) ? 'b011 :
                            (inst_funct3 == 'd7) ? 'b011 :
                            'b111;

    assign br_eq = (alu_out == 32'd0);
    assign br_lt = (alu_out[0] == 'd1); // Output of slt alu op
    assign br_ltu = (alu_out[0] == 'd1); // Output of sltu alu op

    assign br_take_branch = (inst_funct3 == 'd0) ? br_eq :
                            (inst_funct3 == 'd1) ? ~br_eq :
                            (inst_funct3 == 'd4) ? br_lt :
                            (inst_funct3 == 'd5) ? ~br_lt :
                            (inst_funct3 == 'd6) ? br_ltu :
                            (inst_funct3 == 'd7) ? ~br_ltu :
                            1'b0;

    // Register File

    wire [2:0] cs_reg_write_rd_sel;
    wire cs_reg_write_rd_en;

    wire [4:0] reg_file_read_rs1;
    wire [4:0] reg_file_read_rs2;
    wire reg_file_write_en_rd;
    wire [4:0] reg_file_write_rd;
    wire [31:0] reg_file_write_data_rd;
    wire [31:0] reg_file_rs1;
    wire [31:0] reg_file_rs2;

    wire [31:0] reg_mem_din;

    reg_file reg_file(
        .clk(clk),
        .reset(reset),
        .readA(reg_file_read_rs1),
        .readB(reg_file_read_rs2),
        .writeEnC(reg_file_write_en_rd),
        .writeC(reg_file_write_rd),
        .writeDataC(reg_file_write_data_rd),
        .A(reg_file_rs1),
        .B(reg_file_rs2)
    );

    assign reg_file_read_rs1 = inst_rs1;
    assign reg_file_read_rs2 = inst_rs2;
    assign reg_file_write_en_rd = cs_reg_write_rd_en;
    assign reg_file_write_rd = inst_rd;

    assign reg_file_write_data_rd = (cs_reg_write_rd_sel == 'd0) ? alu_out :
                                    (cs_reg_write_rd_sel == 'd1) ? reg_mem_din :
                                    (cs_reg_write_rd_sel == 'd2) ? csr_dout : //CSR Reg
                                    (cs_reg_write_rd_sel == 'd3) ? pc_4_out : // PC + 4
                                    (cs_reg_write_rd_sel == 'd4) ? inst_imm_u :
                                    (cs_reg_write_rd_sel == 'd5) ? inst_imm_u + pc_out :
                                    'hFFFFFFFF;

    assign reg_mem_din = (inst_funct3 == 'd0) ? mem_data8_signed :
                         (inst_funct3 == 'd1) ? mem_data16_signed :
                         (inst_funct3 == 'd2) ? mem_din :
                         (inst_funct3 == 'd4) ? mem_data8_unsigned :
                         (inst_funct3 == 'd5) ? mem_data16_unsigned :
                         'hFFFFFFFF;


    // PC

    wire [2:0] cs_pc_mux_sel;

    wire [31:0] pc_out;
    wire [31:0] pc_4_out;

    pc pc(
        .clk(clk),
        .reset(reset),
        .pc_mux_sel(cs_pc_mux_sel),
        .alu_out(alu_out),
        .inst_imm_j(inst_imm_j),
        .inst_imm_b(inst_imm_b),
        .csr_mtvec(csr_mtvec_out),
        .mem_dout(mem_dout),
        .pc_out(pc_out),
        .pc_4_out(pc_4_out),
        .take_branch(br_take_branch)
    );


    // Control Unit

    wire [18:0] control_signals;

    control_unit #(
        .CS_N(18))
        control_unit(
            .clk(clk),
            .reset(reset),
            .inst(inst_opcode[6:2]),
            .control_signals(control_signals)
        );

    cs_mapper_mod cs_mapper(
        .cs_alu_ctrl_sel(cs_alu_ctrl_sel),
        .cs_csr_source(cs_csr_source),
        .cs_alu_b_sel(cs_alu_b_sel),
        .cs_reg_write_rd_en(cs_reg_write_rd_en),
        .cs_reg_write_rd_sel(cs_reg_write_rd_sel),
        .cs_mem_read_en(cs_mem_read_en),
        .cs_mem_addr_sel(cs_mem_addr_sel),
        .cs_mem_width_sel(cs_mem_width_sel),
        .cs_alu_twos_b(cs_alu_twos_b),
        .cs_inst_write_en(cs_inst_write_en),
        .cs_mem_write_en(cs_mem_write_en),
        .cs_pc_mux_sel(cs_pc_mux_sel),
        .control_signals(control_signals)
    );

endmodule
