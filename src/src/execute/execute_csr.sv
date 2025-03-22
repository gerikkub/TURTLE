`timescale 1ns / 1ns

module execute_csr(
    input clk,
    input reset,
    input flush,

    input var [6:0]decode_opcode,
    input var [2:0]decode_funct3,
    input var [4:0]decode_rd,
    input var [4:0]decode_rs1,
    input var [11:0]decode_imm,

    input var [31:0]read_rs1_val,
    input read_valid,

    // CSR Read-only bus
    output [11:0]csrbus_araddr,
    output csrbus_arvalid,

    input [31:0]csrbus_rdata,
    input [1:0]csrbus_rresp,
    input csrbus_rvalid,

    output [11:0]csr_write_addr,
    output [31:0]csr_write_val,
    output csr_write_valid,

    output processing,
    output valid,
    output [31:0]rd_val_out,

    output [5:0]exception_num_out,
    output exception_valid_out
    );

    localparam SYSTEM_OPCODE = 'b1110011;

    localparam CSRRW_FUNCT3 = 'b001;
    localparam CSRRS_FUNCT3 = 'b010;
    localparam CSRRC_FUNCT3 = 'b011;
    localparam CSRRWI_FUNCT3 = 'b101;
    localparam CSRRSI_FUNCT3 = 'b110;
    localparam CSRRCI_FUNCT3 = 'b111;

    localparam EXCEPTION_ILLEGAL_INST = 'd2;


    enum int unsigned {
        WAIT_OP,
        WAIT_READ
    } state, next_state;

    wire known_opcode = (decode_opcode == SYSTEM_OPCODE) &&
                        ((decode_funct3 == CSRRW_FUNCT3) ||
                         (decode_funct3 == CSRRS_FUNCT3) ||
                         (decode_funct3 == CSRRC_FUNCT3) ||
                         (decode_funct3 == CSRRWI_FUNCT3) ||
                         (decode_funct3 == CSRRSI_FUNCT3) ||
                         (decode_funct3 == CSRRCI_FUNCT3));

    // Read-Modify-Write op
    wire do_read = (decode_rd != 0) ||
                   ((decode_funct3 == CSRRS_FUNCT3) ||
                    (decode_funct3 == CSRRC_FUNCT3) ||
                    (decode_funct3 == CSRRSI_FUNCT3) ||
                    (decode_funct3 == CSRRCI_FUNCT3));

    assign processing = (!flush) &&
                        ((read_valid && known_opcode) ||
                         (state != WAIT_OP));
    assign valid = (!flush) &&
                   ((read_valid && known_opcode && (!do_read)) ||
                    (state == WAIT_READ) && (csrbus_rvalid));

    always_ff @(posedge clk) begin
        if (reset == 'd1) begin
            state <= WAIT_OP;
        end else begin
            state <= next_state;
        end
    end

    always_comb begin
        if (flush == 'd1)
            next_state = WAIT_OP;
        else if (state == WAIT_OP)
            if (read_valid && known_opcode && do_read)
                next_state = WAIT_READ;
            else 
                next_state = state;
        else // (state == WAIT_READ)
            if (csrbus_rvalid)
                next_state = WAIT_OP;
            else 
                next_state = state;
    end

    // Read Value CsrBus
    assign csrbus_arvalid = (!flush) && (state == WAIT_READ);
    assign csrbus_araddr = decode_imm[11:0];

    assign rd_val_out = (state == WAIT_READ) ? csrbus_rdata :
                        'h040c0de0;
    assign exception_valid_out = (!flush) && (state == WAIT_READ) && csrbus_rvalid && (csrbus_rresp != 0);
    assign exception_num_out = exception_valid_out ? EXCEPTION_ILLEGAL_INST : 'd0;

    // CSR Write State
    assign csr_write_addr = csr_write_valid ? decode_imm[11:0] : 'd0;

    always_comb begin

        if ((!valid) || (decode_rs1 == 'd0)) begin
            csr_write_val = 'h041c0de0;
            csr_write_valid = 'd0;
        end else begin
            csr_write_valid = 'd1;
            case (decode_funct3)
            CSRRW_FUNCT3: csr_write_val = read_rs1_val;
            CSRRWI_FUNCT3: csr_write_val = {27'b0, decode_rs1};
            CSRRS_FUNCT3: csr_write_val = csrbus_rdata | read_rs1_val;
            CSRRSI_FUNCT3: csr_write_val = csrbus_rdata | {27'b0, decode_rs1};
            CSRRC_FUNCT3: csr_write_val = csrbus_rdata & (~read_rs1_val);
            CSRRCI_FUNCT3: csr_write_val = csrbus_rdata & (~{27'b0, decode_rs1});
            default: csr_write_val = 'h042c0de;
            endcase
        end
    end

endmodule
