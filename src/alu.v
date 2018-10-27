`timescale 1ns / 1ns

module alu(
    input [31:0] A,
    input [31:0] B,
    input [2:0] ctrl,

    output [31:0] out
    );

    reg [31:0] shift_left;
    reg [31:0] set_less_than;
    reg [31:0] set_less_than_u;

    wire [31:0] w_add;
    wire [31:0] w_xor;
    wire [31:0] w_shift_right;
    wire [31:0] w_or;
    wire [31:0] w_and;

    wire [4:0] shift_bits;
    wire s_sign;

    assign w_add = A + B;

    assign w_xor = A ^ B;
    assign w_or = A | B;
    assign w_and = A & B;

    assign shift_bits = B[4:0];
    assign s_sign = B[10] & A[31];

    assign w_shift_right = (shift_bits == 'd0) ? A :
                           (shift_bits == 'd1) ? {{1{s_sign}}, A[31:1]} :
                           (shift_bits == 'd2) ? {{2{s_sign}}, A[31:2]} :
                           (shift_bits == 'd3) ? {{3{s_sign}}, A[31:3]} :
                           (shift_bits == 'd4) ? {{4{s_sign}}, A[31:4]} :
                           (shift_bits == 'd5) ? {{5{s_sign}}, A[31:5]} :
                           (shift_bits == 'd6) ? {{6{s_sign}}, A[31:6]} :
                           (shift_bits == 'd7) ? {{7{s_sign}}, A[31:7]} :
                           (shift_bits == 'd8) ? {{8{s_sign}}, A[31:8]} :
                           (shift_bits == 'd9) ? {{9{s_sign}}, A[31:9]} :
                           (shift_bits == 'd10) ? {{10{s_sign}}, A[31:10]} :
                           (shift_bits == 'd11) ? {{11{s_sign}}, A[31:11]} :
                           (shift_bits == 'd12) ? {{12{s_sign}}, A[31:12]} :
                           (shift_bits == 'd13) ? {{13{s_sign}}, A[31:13]} :
                           (shift_bits == 'd14) ? {{14{s_sign}}, A[31:14]} :
                           (shift_bits == 'd15) ? {{15{s_sign}}, A[31:15]} :
                           (shift_bits == 'd16) ? {{16{s_sign}}, A[31:16]} :
                           (shift_bits == 'd17) ? {{17{s_sign}}, A[31:17]} :
                           (shift_bits == 'd18) ? {{18{s_sign}}, A[31:18]} :
                           (shift_bits == 'd19) ? {{19{s_sign}}, A[31:19]} :
                           (shift_bits == 'd20) ? {{20{s_sign}}, A[31:20]} :
                           (shift_bits == 'd21) ? {{21{s_sign}}, A[31:21]} :
                           (shift_bits == 'd22) ? {{22{s_sign}}, A[31:22]} :
                           (shift_bits == 'd23) ? {{23{s_sign}}, A[31:23]} :
                           (shift_bits == 'd24) ? {{24{s_sign}}, A[31:24]} :
                           (shift_bits == 'd25) ? {{25{s_sign}}, A[31:25]} :
                           (shift_bits == 'd26) ? {{26{s_sign}}, A[31:26]} :
                           (shift_bits == 'd27) ? {{27{s_sign}}, A[31:27]} :
                           (shift_bits == 'd28) ? {{28{s_sign}}, A[31:28]} :
                           (shift_bits == 'd29) ? {{29{s_sign}}, A[31:29]} :
                           (shift_bits == 'd30) ? {{30{s_sign}}, A[31:30]} :
                           {{31{s_sign}}, A[31]};

    assign out = (ctrl == 'd0) ? w_add :
                 (ctrl == 'd1) ? shift_left :
                 (ctrl == 'd2) ? set_less_than :
                 (ctrl == 'd3) ? set_less_than_u :
                 (ctrl == 'd4) ? w_xor :
                 (ctrl == 'd5) ? w_shift_right :
                 (ctrl == 'd6) ? w_or :
                 w_and;


    always @(*) begin
        // Set Less Than
        if (A < B)
            set_less_than_u <= 'b1;
        else
            set_less_than_u <= 'b0;

        // Set Less Than Unsigned
        if (A & 'h80000000) begin
            if (B & 'h80000000) begin
                // Both < 0
                if (A < B) begin
                    set_less_than <= 'b1;
                end else begin
                    set_less_than <= 'b0;
                end
            end else begin
                // A < 0 && B >=0
                set_less_than <= 'b1;
            end
        end else begin
            if (B & 'h80000000) begin
                // A >=0 && B < 0
                set_less_than <= 'b0;
            end else begin
                // Both >= 0
                if (A < B) begin
                    set_less_than <= 'b1;
                end else begin
                    set_less_than <= 'b0;
                end

            end
        end

        // Shift Left
        case(B[4:0])
            'd0: shift_left <= A << 'd0;
            'd1: shift_left <= A << 'd1;
            'd2: shift_left <= A << 'd2;
            'd3: shift_left <= A << 'd3;
            'd4: shift_left <= A << 'd4;
            'd5: shift_left <= A << 'd5;
            'd6: shift_left <= A << 'd6;
            'd7: shift_left <= A << 'd7;
            'd8: shift_left <= A << 'd8;
            'd9: shift_left <= A << 'd9;
            'd10: shift_left <= A << 'd10;
            'd11: shift_left <= A << 'd11;
            'd12: shift_left <= A << 'd12;
            'd13: shift_left <= A << 'd13;
            'd14: shift_left <= A << 'd14;
            'd15: shift_left <= A << 'd15;
            'd16: shift_left <= A << 'd16;
            'd17: shift_left <= A << 'd17;
            'd18: shift_left <= A << 'd18;
            'd19: shift_left <= A << 'd19;
            'd20: shift_left <= A << 'd20;
            'd21: shift_left <= A << 'd21;
            'd22: shift_left <= A << 'd22;
            'd23: shift_left <= A << 'd23;
            'd24: shift_left <= A << 'd24;
            'd25: shift_left <= A << 'd25;
            'd26: shift_left <= A << 'd26;
            'd27: shift_left <= A << 'd27;
            'd28: shift_left <= A << 'd28;
            'd29: shift_left <= A << 'd29;
            'd30: shift_left <= A << 'd30;
            'd31: shift_left <= A << 'd31;
        endcase
    end

endmodule

