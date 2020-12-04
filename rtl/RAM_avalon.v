module RAM_avalon(
    input logic clk,
    input logic[31:0] address,
    input logic[3:0] byteenable,
    input logic read,
    input logic write,
    output logic waitrequest,
    output logic[31:0] readdata,
    input logic[31:0] writedata
);

// setup memory data
parameter RAM_INIT_FILE = "";
// cannot simulate full memory on verilog ideally would be  4294967296 or 4Gb
logic [32:0] memory [65535:0];
initial begin
    integer i;
    for(i = 0; i < 65536; i++) begin
        memory[i] = 0;
    end
    // load contents from file
    if(RAM_INIT_FILE != "") begin
        $display("RAM: INIT: Loading Ram contents from %s", RAM_INIT_FILE);
        $readmemh(RAM_INIT_FILE, memory, 3217031168);
    end
end

/*      Setting up assisting variables      */
    integer waitcycle;
    integer i;

    always @(posedge clk) begin
        if(read == 1)begin 
            waitcycle = $urandom_range(0,3);
            waitrequest = 1;
            for(i = 0; i< waitcycle; i++)begin 
                @(posedge clk);
            end 
            readdata = memory[address];
            waitrequest = 0;
        end 
        if(write == 1)begin 
            waitcycle = $urandom_range(0,3);
            waitrequest = 1;
            for(i = 0; i< waitcycle; i++)begin 
                @(posedge clk);
            end 
            memory[address] <= writedata;
            waitrequest = 0;
        end 

    end



// // used to determine whether or not the request is completed
// logic finished_request;

// // internal managing logic
// logic[31:0] data;

// // logic debug for wait cycles
// logic[5:0] cycleCount;
// logic[5:0] waitCount;

// initialise registers
// initial begin
//     waitrequest = 0;
//     readdata = 0;

//     cycleCount = 0;
//     waitCount = 4;
// end

// always @(posedge clk) begin
//     if(waitrequest) begin // if we're currently waiting for something
//         if(finished_request) begin // check if request is completed
//             cycleCount[5:0] <= 0;
//             waitrequest <= 0;
//             if(read) begin // if read then return requested data
//                 readdata <= data;
//             end
//             else begin // if write then change memory
//                 memory[address] <= writedata;
//             end
//         end
//         else begin // otherwise increase cycle count (this is to simulate waiting)
//             cycleCount <= cycleCount + 1;
//         end
//     end
//     else begin
//         if(read | write) begin // start wait request if read or write is high and not already in wait request
//             waitrequest <= 1;
//         end
//     end
// end

// // this will wait for cycleCount to be equal to or aboce waitcount and then set data to the correct value
// always_comb begin
//     if(cycleCount >= waitCount) begin
//         finished_request = 1;
//         data[31:0] = memory[address];
//     end
// end

endmodule