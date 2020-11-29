module sign_extend(
    input logic[15:0] in,
    output logic[31:0] out
);
    assign out = {16'h0000,in};

endmodule