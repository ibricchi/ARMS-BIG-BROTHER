module mips_cpu_bus(
    /* Standard signals */
    input logic clk,
    input logic reset,
    output logic active,
    output logic[31:0] register_v0,

    /* Avalon memory mapped bus controller (master) */
    output logic[31:0] address,
    output logic write,
    output logic read,
    input logic waitrequest,
    output logic[31:0] writedata,
    output logic[3:0] byteenable,
    input logic[31:0] readdata
);

// setup state machine
logic[3:0] state;
initial begin
    state = 0;
    active = 0;
end

always_ff @(posedge clk) begin // on every clock cycle if waitrequest is low change state
    // debug code
    $display("Instruction: ", instr, " PC: ", pc_out - 3217031168, " PC_IN: ", add_out - 3217031168, " Zero: ", zero, " Branch: ", branch);
    if(!waitrequest) case(state)
        0: begin // HALT
            state <= 1;
            active <= 1;
            $display("Entering FETCH STATE: ");
        end
        1: begin // FETCH
            state <= 2;
            $display("Entering DECO STATE: ");
        end
        2: begin // DECODE
            state <= 3;
            $display("Entering EXEC1 STATE: ");
        end
        3: begin // EXEC1
            state <= 4;
            $display("Entering EXEC2 STATE: ");
        end
        4: begin // EXEC2
            state <= 1;
            $display("Entering FETCH STATE: ");
        end
    endcase
end

always_ff @(posedge clk) begin // check if pc is at 0 and terminate
    if(state!=0 & pc_out == 0) begin
        active <= 0;
        state <= 0; // halt
    end
end

//instruction register not yet implemented
//here I just created a logic 32-bit component as instruction
logic[31:0] instr, instr_reg;
logic[31:0] pc_in, pc_out;
logic pcwrite;

pc pc_0(
    .clk(clk),
    .reset(reset),
    .pc_in(pc_in),
    .pcwrite(pcwrite),
    .pc_out(pc_out)
);

//control unit (not updated yet)
logic[3:0] ALUOp;
logic[1:0] div_mult_op;
logic ALUSrc, jump, branch, regdst, memtoreg, regwrite, inwrite, pctoadd, regtojump, div_mult_en, div_mult_signed, hitoreg, lotoreg, link, loadimmed;

control_unit control_0(
    .opcode(instr[31:26]),
    .state(state),
    .fun(instr[5:0]),
    .branchFunc(instr[20:16]),
    .waitrequest(waitrequest),

    .ALUOp(ALUOp),
    .ALUSrc(ALUSrc),
    .jump(jump),
    .branch(branch),
    .memread(read),
    .memwrite(write),
    .regdst(regdst),
    .memtoreg(memtoreg),
    .regwrite(regwrite),
    .inwrite(inwrite),
    .pctoadd(pctoadd),
    .pcwrite(pcwrite),
    .regtojump(regtojump),
    .div_mult_en(div_mult_en),
    .div_mult_signed(div_mult_signed),
    .div_mult_op(div_mult_op),
    .hitoreg(hitoreg),
    .lotoreg(lotoreg),
    .link(link),
    .loadimmed(loadimmed)

);

// instr register
always_ff @(posedge clk) begin
    if(inwrite) begin
        instr_reg <= readdata;
    end
end
assign instr = (state==2)?readdata:instr_reg;

//register file
logic[31:0] read_data1, read_data2;
logic[4:0] write_reg;
logic[31:0] write_data;
assign write_reg = (regdst == 0) ? ((link == 1)? 5'b11111:instr[20:16]) : instr[15:11];

register_file reg_file_0(
    .clk(clk),
    .reset(reset),

    .read_index1(instr[25:21]),
    .read_index2(instr[20:16]),
    .write_enable(regwrite),
    .write_reg(write_reg),
    .write_data(write_data),

    .read_data1(read_data1),
    .read_data2(read_data2),
    .register_v0(register_v0)
);

// multiply and divide alu and register
logic[31:0] hi, lo;
div_mult_reg div_mult_reg_0(
    .clk(clk),
    .reset(reset),

    .write_en(div_mult_en),
    .sin(div_mult_signed),
    .op(div_mult_op),
    .in_1(read_data1),
    .in_2(read_data2),

    .hi(hi),
    .lo(lo)
);

logic[31:0] alu_b;
assign alu_b = (ALUSrc == 0) ? read_data2 : extend_out;

//ALU Control
logic[4:0] ALUCtrl;
alu_control alu_ctrl_0(
    .ALUOp(ALUOp),
    .FuncCode(instr[5:0]),
    .ALUCtrl(ALUCtrl),
    .BranchzFunc(instr[20:16])
);

//ALU
logic zero;
logic[31:0] ALU_out;

alu alu_0(
    .a(read_data1),
    .b(alu_b),
    .as(instr[10:6]),
    .alu_control(ALUCtrl),
    .result(ALU_out),
    .zero(zero)
);

// pc_calculated on branch
logic[31:0] extend_out;
logic[31:0] add_out;
assign extend_out = {{16{instr[15]}}, instr[15:0]};
assign add_out = pc_out + (extend_out << 2);

logic and_result;
assign and_result = branch && zero;

//MUX4 location
assign pc_in = jump ?
    (regtojump ? read_data1 : (instr[25:0] << 2)) :
    ((and_result ? add_out : pc_out) + 4);

//from data memory
assign address = pctoadd?pc_out:ALU_out;

//writedata always second output of register
assign writedata = read_data2;

//MUX3
logic[31:0] loadresult;
assign loadresult = {instr[15:0],16'h0000};

always_comb begin
    if(memtoreg) begin
        write_data = readdata;
    end
    else if(link) begin
        write_data = pc_out+4;
    end
    else if(loadimmed) begin
        write_data = loadresult;
    end
    else if(hitoreg) begin
        write_data = hi;
    end
    else if(lotoreg) begin
        write_data = lo;
    end
    else begin
        write_data = ALU_out;
    end
end

endmodule