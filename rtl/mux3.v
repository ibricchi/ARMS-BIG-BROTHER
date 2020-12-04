//mux right after the data memory
module mux3(
    input logic[31:0]  readdata,
    input logic[31:0]  ALUout,
    input logic        memtoreg,

    output logic[31:0] writedata
);

always_comb begin
    if(memtoreg == 0) begin
        writedata = ALUout;
    end
    else begin
        writedata = readdata;
    end
end

endmodule