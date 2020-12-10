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
// used to determine whether or not the request is completed
logic finished_request;

// internal managing logic
logic[31:0] data;

// logic debug for wait cycles
logic[5:0] cycleCount;
logic[5:0] waitCount;

// initialise registers
initial begin
    waitrequest = 0;
    readdata = 0;

    cycleCount = 0;
    waitCount = 4;
end

always_ff @(posedge clk) begin
    if(waitrequest) begin // if we're currently waiting for something
        if(finished_request) begin // check if request is completed
            // $display("finished request ", read, " ", write);
            if(read) begin // if read then return requested data
                // $display("setting readddata", data);
                readdata <= data;
            end
            else begin // if write then change memory
                memory[address] <= writedata;
            end
            cycleCount[5:0] <= 0;
            waitrequest <= 0;
        end
        else begin // otherwise increase cycle count (this is to simulate waiting)
            cycleCount <= cycleCount + 1;
        end
    end
end

// start wait request if read or write is high and not already in wait request
always_ff @(posedge read) begin
    // $display("read based interrupt ", read);
    waitrequest <= 1;
end
always_ff @(posedge write) begin
    waitrequest <= 1;
end

// this will wait for cycleCount to be equal to or aboce waitcount and then set data to the correct value
always_comb begin
    address = ((addressin - 3217031168)>>2)%4096;
    if(cycleCount >= waitCount) begin
        finished_request = 1;
        data[31:0] = memory[address];
    end
    else begin
        finished_request = 0;
    end
end

endmodule