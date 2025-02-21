`timescale 1ns / 1ns

module reg_file(
    input clk,
    input reset,

    input [4:0] read_rs1,
    input [4:0] read_rs2,

    input write_en_rd,
    input var [4:0] write_rd,
    input [31:0] write_val,

    output [31:0]rs1_val,
    output [31:0]rs2_val
    );


    reg [31:0] xreg [1:31];
    var logic [31:0] xreg_mux [1:31];

    initial begin
        for (int i=1; i < 32; i++) begin
            xreg[i] = 0;
        end
        xreg[2] = 'h80000800;
    end

    always_comb begin

        for (int i=1; i < 32; i++) begin
            if (write_en_rd && (write_rd == i[4:0]))
                xreg_mux[i] = write_val;
            else
                xreg_mux[i] = xreg[i];
        end
    end

    always_ff @(posedge clk) begin

        if (reset == 'b1) begin
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
        end else begin
            for (int i=1; i < 32; i++) begin
                xreg[i] <= xreg_mux[i];
            end
        end
    end

    assign rs1_val = (read_rs1 == 'd0) ? 'd0 :
                     (write_en_rd && (read_rs1 == write_rd) ? write_val :
                                                              xreg[read_rs1]);
    assign rs2_val = (read_rs2 == 'd0) ? 'd0 :
                     (write_en_rd && (read_rs2 == write_rd) ? write_val :
                                                              xreg[read_rs2]);

endmodule
