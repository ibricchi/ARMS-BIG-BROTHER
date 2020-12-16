module register_file(
    input logic clk,

    input logic reset,

    input logic[4:0]  read_index1,
    input logic[4:0]  read_index2,
    input logic       write_enable,
    input logic[4:0]  write_reg,
    input logic[31:0] write_data,
    
    output logic[31:0] read_data1,
    output logic[31:0] read_data2,
    output logic[31:0] register_v0
);

    logic[31:0] register[31:0];

    always_ff @(posedge clk) begin
        if(reset) begin
            for(int i = 0; i < 32; i = i + 1) begin
                register[i] <= 0;
            end
        end
        else begin
            if(write_enable) begin
                register[write_reg] <= write_data;
                $display("Setting reg %d to %h", write_reg, write_data);
            end
        end
    end

    assign read_data1 = (read_index1 == 0)? 32'b0 : register[read_index1];
    assign read_data2 = (read_index2 == 0)? 32'b0 : register[read_index2];
    assign register_v0 = register[2];

endmodule