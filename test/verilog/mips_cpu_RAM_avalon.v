module RAM_avalon(
    input logic clk,
    input logic[31:0] addressin,
    input logic[3:0] byteenable,
    input logic read,
    input logic write,
    output logic waitrequest,
    output logic[31:0] readdata,
    input logic[31:0] writedata
);

logic[31:0] address;

// setup memory data
parameter RAM_INIT_FILE = "";
// cannot simulate full memory on verilog ideally would be  4294967296 or 4Gb
reg [31:0] memory [4095:0];
initial begin
    integer i;
    /* Initialise to zero by default */
    //memory[0] = 32'h00211021;
    for (i=0; i<4096; i++) begin
        memory[i]=0;
    end
    /* Load contents from file if specified */
    if (RAM_INIT_FILE != "") begin
        $display("RAM : INIT : Loading RAM contents from %s", RAM_INIT_FILE);
        $readmemh(RAM_INIT_FILE, memory);
    end
end

integer waitcycle;
// initialise registers
initial begin
    waitrequest = 0;
    readdata = 0;

    waitcycle = $urandom_range(0,5);
end

// simulate location in memory (wrap around if no valid location)
assign address = ((addressin - 3217031168)>>2)%4096;
// addressin - num = 0
// >>2 would shift right by 2 
// % to check the remainder

// start wait request if read or write is high and not already in wait request
always_ff @(posedge read) begin
    waitrequest <= 1;
end
always_ff @(posedge write) begin
    waitrequest <= 1;
end

// simulate a random wait time after request
always_ff @(posedge clk) begin
    if(waitrequest) begin // if in waitrequest
        if(waitcycle != 0) begin // check if waitcycle has finihsed
            waitcycle <= waitcycle - 1;
        end
        else if(waitcycle == 0) begin
            if(read) begin // set readdata if requested
                // $display("Address: %h data: %h", address, memory[address]);
                readdata <= memory[address];
            end
            else if(write) begin // set write data if requested
                // $display("Bytenable: %b address: %h data: %h", byteenable, address, writedata);
                memory[address] <= {
                    byteenable[3] ? writedata[31:24] : memory[address][31:24],
                    byteenable[2] ? writedata[23:16] : memory[address][23:16],
                    byteenable[1] ? writedata[15:8] : memory[address][15:8],
                    byteenable[0] ? writedata[7:0] : memory[address][7:0]
                };
            end
            waitcycle <= 1;//$urandom_range(0,5); // reset reandom wait time (this can be set to a constant, random can be useful for testing)
            waitrequest <= 0; // reset wait request
        end
    end
end

endmodule
