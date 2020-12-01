module pc(
    input logic        clk,
    input logic        reset,
    input logic[31:0]  pc_in,

    output logic[31:0] pc_out
);

always_ff @(posedge clk) begin
    if(reset == 1) begin
        pc_out <= 0;
    end
    else begin
        pc_out <= pc_in + 4;
    end
end

endmodule