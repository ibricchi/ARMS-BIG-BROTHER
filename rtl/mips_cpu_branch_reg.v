module branch_reg(
    input logic        clk,
    input logic[31:0]  add_out,
    input logic        and_result,
    input exec1,
    input exec2,

    output logic[31:0] branch_address,
    output logic       branch_ctrl
);

always @(posedge clk) begin
    branch_ctrl = 0;
    if(and_result == 1) begin
        branch_address <= add_out;
        branch_ctrl = 0;
        @(posedge exec1);
        branch_ctrl = 1;
        @(negedge exec2);
        branch_ctrl = 0;
    end

end

endmodule