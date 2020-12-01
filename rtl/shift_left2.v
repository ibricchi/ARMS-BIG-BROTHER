module shift_left2(
    input logic[31:0]  shift_in,
    output logic[31:0] shift_out
);

always @(shift_in) begin
    shift_out = shift_in << 2;
end

endmodule