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
    if(!waitrequest) case(state)
        0: begin // HALT
            state <= 1;
            active <= 1;
        end
        1: begin // FETCH
            state <= 2;
        end
        2: begin // DECODE
            state <= 3;
        end
        3: begin // EXEC1
            state <= 4;
        end
        4: begin // EXEC2
            state <= 5;
        end
    endcase
end

always_ff @(posedge clk) begin
    if(address == 0) begin
        active <= 0;
        state <= 0; // halt
    end
end

//instruction register not yet implemented
//here I just created a logic 32-bit component as instruction
logic[31:0] instr;
logic[31:0] pc_in, pc_out;

pc pc_0(
    .clk(clk),
    .reset(reset),
    .pc_in(pc_in),
    .pc_out(pc_out)
);

//control unit (not updated yet)
logic[1:0] ALUOp;
logic ALUSrc, jump, branch, memread, memwrite, regdst, memtoreg, regwrite, inwrite, pctoadd;

control_unit control_0(
    .opcode(instr[31:26]),
    .state(state),
    .ALUOp(ALUOp),
    .ALUSrc(ALUSrc),
    .jump(jump),
    .branch(branch),
    .memread(memread),
    .memwrite(memwrite),
    .regdst(regdst),
    .memtoreg(memtoreg),
    .regwrite(regwrite),
    .inwrite(inwrite),
    .pctoadd(pctoadd)
);

// instr register
always_ff @(posedge clk) begin
    if(inwrite) begin
        instr <= readdata;
    end
end

//register file
logic[31:0] read_data1, read_data2;
logic[4:0] write_reg;
logic[31:0] write_data;
assign write_reg = (regdst == 0) ? instr[20:16] : instr[15:11];

register_file reg_file_0(
    .clk(clk),
    .reset(reset),

    .read_index1(instr[25:21]),
    .read_index2(instr[20:16]),
    .write_enable(regwrite),
    .write_reg(write_reg),
    .write_data(write_data),

    .read_data1(read_data1),
    .read_data2(read_data2)
);

logic[31:0] extend_out;
assign extend_out = {16'h0000, instr[15:0]};

logic[31:0] alu_b;
assign alu_b = (ALUSrc == 0) ? read_data2 : extend_out;

//shift left 2
logic[31:0] shift_out;
assign shift_out = extend_out << 2;

//ALU Control
logic[3:0] ALUCtrl;
alu_control alu_ctrl_0(
    .ALUOp(ALUOp),
    .FuncCode(instr[5:0]),
    .ALUCtrl(ALUCtrl)
);

//ALU
logic zero;
logic[31:0] ALU_out;

alu alu_0(
    .a(read_data1),
    .b(alu_b),
    .alu_control(ALUCtrl),
    .result(ALU_out),
    .zero(zero)
);

//Add ALU
logic[31:0] add_out;

add_alu add_alu_0(
    .pc_out(pc_out),
    .shift_out(shift_out),
    .out(add_out)
);

logic and_result;
assign and_result = branch && zero;

//MUX4 location
assign pc_in = (and_result == 0) ? pc_out : add_out;

//logic[31:0] readdata
//from data memory
assign address = pctoadd?pc_out:ALU_out;

//MUX3
assign write_data = (memtoreg == 0) ? ALU_out : readdata;

endmodule