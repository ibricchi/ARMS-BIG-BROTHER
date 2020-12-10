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
    for (i=0; i<4096; i++) begin
        memory[i]=0;
    end
    /* Load contents from file if specified */
    if (RAM_INIT_FILE != "") begin
        $display("RAM : INIT : Loading RAM contents from %s", RAM_INIT_FILE);
        $readmemh(RAM_INIT_FILE, memory);
    end
    memory[0] = 32'h24210000;//BGEZAL TEST BENCH (initialize all reg to -111)
    memory[1] = 32'h042B00FF;//shouldN'T jump here//BGEZAL R1
    memory[2] = 32'h24210001;
    memory[3] = 32'h24210001;
    memory[4] = 32'h24210111;
    memory[5] = 32'h042B0002; //should jump // BGEZAL R1 
    memory[6] = 32'h242100FF; //should not add a big num
    memory[7] = 32'h24210001; 
    memory[8] = 32'h24210001;
    memory[9] = 32'h24210001;
    memory[10] = 32'h24210001;
    memory[11] = 32'h24210001;
    // memory[12] = 32'h0C000010;
    //the sub program start 
    // memory[64] = 32'h00000080;
    // memory[128] = 32'h24210001;

end

integer waitcycle;
// initialise registers
initial begin
    waitrequest = 0;
    readdata = 0;

    // waitcycle = $urandom_range(0,5);
    waitcycle = 0;
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
                readdata <= memory[address];
            end
            else if(write) begin // set write data if requested
                memory[address] <= writedata;
            end
            // waitcycle <= $urandom_range(0,5); // reset reandom wait time (this can be set to a constant, random can be useful for testing)
            waitrequest <= 0; // reset wait request
        end
    end
end

endmodule