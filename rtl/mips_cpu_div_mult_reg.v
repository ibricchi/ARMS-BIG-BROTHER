//An ADD ALU situated at the top of the diagram
module div_mult_reg(
    input logic clk,
    input logic reset,

    input logic write_en,
    input logic sin,
    input logic[1:0] op,
    input logic[31:0] in_1,
    input logic[31:0] in_2,

    output logic[31:0] hi,
    output logic[31:0] lo
);

logic[63:0] in_1_u, in_2_u, in_1_se, in_2_se;

logic in_1_s, in_2_s;
logic[63:0] in_1_mag, in_2_mag;

logic[63:0] prod, prod_u, prod_s;
logic[63:0] div, div_u, div_s;
logic[63:0] rem, rem_u, rem_s;

assign in_1_s = in_1[31];
assign in_2_s = in_2[31];
always_comb begin
    in_1_u = $unsigned(in_1);
    in_2_u = $unsigned(in_2);
    in_1_se = $signed(in_1);
    in_2_se = $signed(in_2);

    div_u = in_1_u / in_2_u;
    rem_u = in_1_u % in_2_u;

    in_1_mag = in_1_s?$unsigned(-$signed(in_1)):$unsigned(in_1);
    in_2_mag = in_2_s?$unsigned(-$signed(in_2)):$unsigned(in_2);
    div_s = in_1_mag / in_2_mag;
    rem_s = in_1_mag % in_2_mag;

    prod_u = in_1_u * in_2_u;
    prod_s = in_1_se * in_2_se;
    prod = sin?prod_s:prod_u;
    
    if(sin) begin
        if(in_1_s&&in_2_s) begin
            div = div_s;
            rem = - rem_s;
            // $display("-/- %d %d", $signed(div), $signed(rem));
        end
        else if(in_1_s) begin
            div = -div_s;
            rem = (rem_s==0)?(in_2_mag - rem_s):0;
            // $display("-/+ %d %d", $signed(div), $signed(rem));
        end
        else if(in_2_s) begin
            div = -div_s;
            rem = (rem_s==0)?0:(rem_s - in_2_mag);
            // $display("+/- %d %d", $signed(div), $signed(rem));
        end
        else begin
            div = div_s;
            rem = rem_s;
            // $display("+/+ %d %d", $signed(div), $signed(rem));
        end
        rem = (rem == in_2_mag)?0:rem;
    end
    else begin
        div = div_u;
        rem = (rem_u==in_2_u)?0:rem_u;
    end
end


always_ff @(posedge clk) begin
    if(reset) begin
        hi <= 0;
        lo <= 0;
    end
    else begin
        if(write_en) begin
            case (op)
                2'b00: begin // MTHI
                    hi <= in_1;
                end
                2'b01: begin // MTLO
                    lo <= in_1;
                end
                2'b10: begin // MULTU
                    hi <= prod[63:32];
                    lo <= prod[31:0];
                    $display("Multiplication %h:%h", prod[63:32], prod[31:0]);
                end
                2'b11: begin // DIVU
                    lo <= div[31:0];
                    hi <= rem[31:0];
                end
            endcase
        end
    end
end

endmodule