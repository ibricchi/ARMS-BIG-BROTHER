module data_mem(
    input logic        clk,
    input logic[6:0]   address,
    input logic[31:0]  writedata,
    
    output logic[31:0] readdata
);