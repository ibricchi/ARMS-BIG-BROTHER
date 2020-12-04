`timescale 1ns/100ps

module mips_cpu_bus_tb_slave(
);
    /*  The central line compenent  */

    /*  NOT THE COMPONENT CONTROLLED BY THE SLAVE  */

    logic clk;
    logic reset;

    /*  Input of the slave  */
    logic[31:0] address;
    logic write;
    logic read;
    logic active;
    logic[31:0] writedata;
    logic[3:0] byteenable;

    /*  Output of the slave  */
    logic waitrequest;
    logic[31:0] readdata;

    /*  Omniscient perspective  */
    logic[31:0] register_v0;

    

    /*  Declaring assistant variables */

    integer waitcycle;
    logic[31:0] Mem [0:127];
    integer instr_pointer;
    integer i;

    
    /* This is the user controlling the CPU */
    initial begin
        $timeformat(-9, 1, " ns", 20);
        $dumpfile("mips_cpu_bus_tb_slave.vcd");
        $dumpvars(0, mips_cpu_bus_tb_slave);
        
        clk = 0;
        reset = 0;
        #5;
        reset = 1;
        #5 clk = !clk;
        #5 clk = !clk;
        reset = 0;

        repeat (100) begin
            #5 clk = !clk;
        end 
        $fatal(1, "Testbench timed out rather than exiting gracefully.");
    
    end

    /* This is the slave module communicating with the master*/
    /* The slave's address is 0x00000001  */
    /* The slave is planning to transfer 8 packets to master */
    initial begin 
        instr_pointer = 0;

        Mem[0] = 32'h00221820;
        Mem[4] = 32'hAC010000;
        Mem[8] = 32'h8C240000;
        Mem[12] = 32'h10210001;
        Mem[16] = 32'h00411822;
        repeat(100) begin 
            @(posedge clk);
            if(address == 32'h00000001)begin 
                if(read == 1)begin 
                    waitcycle = $urandom_range(0,3);
                    waitrequest = 1;
                    for(i = 0; i< waitcycle; i++)begin 
                        @(posedge clk);
                    end 
                    readdata = Mem[instr_pointer];
                    waitrequest = 0;
                    instr_pointer += 4;

                end 
                if(write == 1)begin 

                end 

            end 


        end 
        
           

    end 

// initial begin
//     
// end

/* Instructions to be tested are
add: R3,R1,R2
sw:  R1, 0(R0)
lw:  R4, 0(R1)
beq: R1,R1,8
add: R3,R0,R0
sub: R3,R2,R1
*/


    mips_cpu_bus cpu_master(
        .clk(clk),
        .reset(reset),
        .active(active),
        .register_v0(register_v0),
        .waitrequest(waitrequest),
        .readdata(readdata),
        .read(read),
        .write(write),
        .address(address),
        .writedata(writedata),
        .byteenable(byteenable)
    );


endmodule 