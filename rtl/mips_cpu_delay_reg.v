module delay_reg(
    input logic        clk,
    input logic[31:0]  add_out,
    input logic        and_result,
    input logic[31:0]  jump_address,
    input logic        jump,
    input exec1,
    input exec2,

    output logic[31:0] delay_address,
    output logic       delay_ctrl
);

always @(posedge clk) begin
    delay_ctrl = 0;
    if(and_result == 1) begin
        delay_address <= add_out;
        delay_ctrl = 0;
        @(posedge exec1);
        delay_ctrl = 1;
        @(negedge exec2);
        delay_ctrl = 0;
    end
    if(jump == 1) begin
        delay_address <= jump_address;
        delay_ctrl = 0;
        @(posedge exec1);
        delay_ctrl = 1;
        @(negedge exec2);
        delay_ctrl = 0;
    end

end

endmodule