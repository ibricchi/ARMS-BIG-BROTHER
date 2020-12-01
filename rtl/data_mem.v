module data_mem(
    input logic        clk,
    input logic[6:0]   address,
    input logic[31:0]  writedata,
    input logic        memwrite,
    input logic        memread,
    
    output logic[31:0] readdata
);

logic [31:0] Mem[0:127]; //32 bits mem with 128 entries

initial begin
    Mem[0] = 5;
    Mem[1] = 6;
    Mem[2] = 7;
end

always_ff @(posedge clk) begin
    if(memwrite == 1) begin
        Mem[address[6:2]] <= writedata;
    end
end

always_ff @(negedge clk) begin
    if(memread == 1) begin
        readdata <= Mem[address[6:2]];
    end
end

endmodule