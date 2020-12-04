//mux for pc selection
module mux4(
    input logic[31:0] PCout,
    input logic[31:0] alu_out,
    input logic       and_logic,

    output logic[31:0] pc_next
);

initial begin
    pc_next <= 0;
end

always_comb begin
    if(and_logic == 0) begin
        pc_next = PCout;
    end
    else begin
        pc_next = alu_out;
    end
end

endmodule