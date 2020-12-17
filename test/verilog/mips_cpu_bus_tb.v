module mips_cpu_bus_tb;
    timeunit 1ns / 10ps;

    parameter RAM_INIT_FILE = "";
    parameter TIMEOUT_CYCLES = 10000;

    logic clk;
    logic reset;

    logic active;
    logic[31:0] register_v0;

    logic[31:0] address;
    logic[3:0] byteenable;
    logic read;
    logic write;
    logic waitrequest;
    logic[31:0] readdata;
    logic[31:0] writedata;

    RAM_avalon #(RAM_INIT_FILE) ramInst(clk, address, byteenable, read, write, waitrequest, readdata, writedata);
    mips_cpu_bus cpuInst(clk, reset, active, register_v0, address, write, read, waitrequest, writedata, byteenable, readdata);

    // generate clock
    initial begin
        // $timeformat(-9, 1, " ns", 20);
        // $dumpfile("mips_cpu_bus_tb.vcd");
        // $dumpvars(0, mips_cpu_bus_tb);
        clk = 0;

        repeat(TIMEOUT_CYCLES) begin
            #10;
            clk = !clk;
            #10
            clk = !clk;
        end

        $fatal(2, "Simulation did not finish within %d cycles.", TIMEOUT_CYCLES);
    end

    initial begin
        reset <= 1;
        
        @(posedge clk);
        reset <= 0;

        @(posedge clk);
        assert(active==1)
        else $display("TB: CPU did not set active=1 after reset.");

        while(active) begin
            @(posedge clk);
            if(waitrequest) begin
                assert(address[1:0] == 2'b00) else begin
                   $display("RAM: Error: Unlaigned address %b", address);
                   $fatal(1); 
                end
            end
        end

        $display("TB: INFO: active = 0");
        $display("TB: INFO: register_v0 = %d", register_v0);

        $finish;
    end
endmodule
