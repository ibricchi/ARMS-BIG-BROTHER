module pc(
    input logic        clk,
    input logic        reset,
    input logic[31:0]  pc_in,
    input logic        pcwrite,

    output logic[31:0] pc_out
);

always_ff @(posedge clk) begin
    if(reset == 1) begin
        pc_out <= 3217031168;
    end
    else if(pcwrite) begin
        pc_out <= pc_in;
    end
end

endmodule