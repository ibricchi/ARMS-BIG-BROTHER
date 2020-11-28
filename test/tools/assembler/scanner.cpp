#include "scanner.h"

#include <iostream>
#include <sstream>

using namespace std;

// Token Stuff
Token::Token(string _name, uint32_t _data, uint32_t _line, uint32_t _memLine, string _label){
    name = _name;
    data = _data;

    memLine = _memLine;
    label = _label;

    line = _line;
}
uint32_t Token::get_range(int l, int r){
    return (data<<l)>>(31-r);
}

// Scanner Stuff
Scanner::Scanner(){
    reset();
}
const unordered_map<string, uint8_t> Scanner::op_map = {
    //{OP_NAME, Special, OPCODE, RS, RT, RD, IMM}
    {"ADDIU",   0b001001},
    {"ADDU",    0b100001},
    {"AND",     0b100100},
    {"ANDI",    0b001100},
    {"BEQ",     0b1000100}, // Branch branch have extra 1 at the begining to avoid conflicts
    {"BGEZ",    0b11100001}, // Other BranchZ other branchz have 3 extra 1's at the begining to avoid conflict
    {"BGEZAL",  0b11110001}, // Other BranchZ 
    {"BGTZ",    0b000111}, // BranchZ branch z have extra 1 at the begining to avoid conflicts
    {"BLEZ",    0b000110}, // BranchZ
    {"BLTZ",    0b000001}, // BranchZ
    {"BLTZAL",  0b11110000}, // Other BranchZ 
    {"BNE",     0b1000101}, // Branch
    {"DIV",     0b011010},
    {"DIVU",    0b011011},
    {"J",       0b1000010}, // Jump jump have extra 1 at the begining to avoid conflicts
    {"JALR",    0b001001}, // JumpR
    {"JAL",     0b1000011}, // Jump
    {"JR",      0b001000},
    {"LB",      0b1100000}, // LoadStore load stores have extra 1 at the begining to avoid conflicts
    {"LBU",     0b1100100}, // LoadStore
    {"LH",      0b1100001}, // LoadStore
    {"LHU",     0b1100101}, // LoadStore
    {"LUI",     0b011111}, // LoadI
    {"LW",      0b1100011}, // LoadStore
    {"LWL",     0b1100010}, // LoadStore
    {"LWR",     0b1100110}, // LoadStore
    {"MTHI",    0b010001}, // MoveTo
    {"MTLO",    0b010011}, // MoveTo
    {"MULT",    0b011000},
    {"MULTU",   0b011001},
    {"OR",      0b100101},
    {"ORI",     0b001101},
    {"SB",      0b1101000}, // LoadStore
    {"SH",      0b1101001}, // LoadStore
    {"SLL",     0b000000}, // Shift
    {"SLLV",    0b000100}, // ShiftV
    {"SLT",     0b101010},
    {"SLTI",    0b001010},
    {"SLTIU",   0b001011},
    {"SLTU",    0b101011},
    {"SRA",     0b000011}, // Shift
    {"SRAV",    0b000111}, // ShiftV
    {"SRL",     0b000010}, // Shit
    {"SRLV",    0b000110}, // ShiftV
    {"SUBU",    0b100011},
    {"SW",      0b1101011}, // LoadStore
    {"XOR",     0b100110},
    {"XORI",    0b001110},
};

void Scanner::reset(){
    line = 1;
    memLine = 0;
    labels = vector<Token>();
    tokens = vector<Token>();
    error = false;
}

void Scanner::errorMsg(string msg){
    error = true;
    cerr << "Error: [Line " << line << "] " << msg << endl;
}
void Scanner::warnMsg(string msg){
    cerr << "Warning: [Line " << line << "] " << msg << endl;
}
void Scanner::expectWhiteSpace(string::iterator& it, string::iterator end){
    bool allSpaces = true;
    string msg = "";
    while(it != end){
        allSpaces &= (*it == ' ');
        msg += *it;
        it++;
    }
    if(!allSpaces){
        error = true;
        errorMsg("Expected whitespace, found '" + msg + "' instead.");
    }
}
void Scanner::skipWhiteSpace(string::iterator& it, string::iterator end){
    while(*it == ' ' && it != end) it++;
    if(it == end) errorMsg("Expected more parameters, reached end of line instead.");
}

inline static bool is_alpha(char c){
    return ('a' <= c && c <='z') || ('A' <= c && c <= 'Z');
}
inline static bool is_bin(char c){
    return c == '1' || c == '0';
}
inline static bool is_numeric(char c){
    return '0' <= c && c <= '9';
}
inline static bool is_hex(char c){
    return is_numeric || ('a' <= c && c <='f') || ('A' <= c && c <= 'F');
}
inline static bool is_alpha_numeric(char c){
    return is_alpha(c) || is_numeric(c) || (c == '_') ;
}

uint32_t Scanner::const_line(string::iterator& it, string::iterator end, bool allow_paren){
    string str_val = "";
    str_val += *it;

    it++;
    if(it == end) return stol(str_val);
    
    int valBase = 0; // 0 is for dec, 1 for hex, 2 for bin

    if(str_val == "0"){
        if(is_numeric(*it) || (allow_paren && *it=='(')){
            str_val += *it;
        }
        else if(*it == 'x' || *it == 'X') valBase = 1;
        else if(*it == 'b' || *it == 'B') valBase = 2;
        else if(*it == ' '){
            expectWhiteSpace(it, end);
            if(error) return 0;
            else return stol(str_val);
        }
        else{
            string it_str = string() + *it;
            errorMsg("Unexpected character '" + it_str + "' in constant.");
            return 0;
        }
    }

    for(it+=(valBase!=0); it != end; it++){
        bool is_valid = false;

        switch (valBase)
        {
        case 0:
            is_valid = is_numeric(*it);
            break;
        case 1:
            is_valid = is_hex(*it);
            break;
        case 2:
            is_valid = is_bin(*it);
            break;
        }

        if(!is_valid){
            if(*it == ' '){
                expectWhiteSpace(it, end);
                if(error) return 0;
                break;
            }else if(allow_paren && *it == '('){
                break;
            }else{
                string it_str = string() + *it;
                errorMsg("Unexpected character '" + it_str + "' in constant.");
                return 0;
            }
        }
        str_val += *it;
    }

    switch (valBase)
    {
    case 0:
        return stol(str_val);
        break;
    case 1:{
        uint32_t out;
        stringstream ss;
        ss << std::hex << str_val;
        ss >> out;
        return out;
    }
    case 2:{
        uint32_t out = 0;
        for(int m = 1, i = str_val.size() - 1; i >= 0; i--, m*=2){
            out += m * (str_val[i] == '1');
        }
        return out;
    }
    }

}
uint8_t Scanner::read_reg(string::iterator& it, string::iterator end, bool comma_terminated, bool paren_terminated){
    if(*it != '$'){
        errorMsg("Expected a register.");
        return 0;
    }
    it++;
    if(it == end){
        errorMsg("Expected register number, reached end of line instead.");
        return 0;
    }
    if(!is_numeric(*it)){
        string it_str = string() + *it;
        errorMsg("Expected integer found '" + it_str + "' instead.");
        return 0;
    }
    string reg_str = "";
    if(is_numeric(*it)) reg_str += *it;
    it++;
    if(comma_terminated ? *it == ',' : (paren_terminated ? *it == ')' : (it == end || *it == ' '))){
        if(comma_terminated || paren_terminated) it++;
        return stol(reg_str);
    }
    if(!is_numeric(*it)){
        string it_str = string() + *it;
        errorMsg("Expected integer found '" + it_str + "' instead.");
        return 0;
    }
    reg_str += *it;
    it++;
    if(comma_terminated ? *it != ',' : (paren_terminated ? *it != ')' : (it != end && *it != ' '))){
        string it_str = string() + *it;
        if(comma_terminated)
            errorMsg("Expected comma, found '" + it_str + "' instead.");
        if(paren_terminated)
            errorMsg("Expected ')' found '" + it_str + "' instead.");
        else
            errorMsg("Expected space or end of line, found '" + it_str + "' instead.");
        return 0;
    }
    if(comma_terminated || paren_terminated) it++;
    uint8_t reg_num = stol(reg_str);
    if(reg_num > 31){
        errorMsg("Register numbers range between 0-31, found '" + to_string(reg_num) + "'.");
        return 0;
    }
    return reg_num;
}
uint16_t Scanner::read_imm(string::iterator& it, string::iterator end, bool paren_terminated){
    if(!is_numeric(*it)){
        errorMsg("Immediate parameter expects a constant.");
        return 0;
    }
    uint32_t imm_32 = const_line(it, end, paren_terminated);
    if(paren_terminated){
        if(*it != '('){
            string it_str = string() + *it;
            errorMsg("Imediate parameter expected '(' character, found '" + it_str + "' instead.");
            return 0;
        }
        else it++;
    }
    uint16_t imm_16 = imm_32;
    if(imm_16 != imm_32){
        warnMsg("Imediate constant is greater than 16bits");
    }
    return imm_32;
}
uint8_t Scanner::read_as(string::iterator& it, string::iterator end){
    if(!is_numeric(*it)){
        errorMsg("'as' field expects a constant.");
        return 0;
    }
    uint32_t as_32 = const_line(it, end);
    uint8_t as_5 = as_32 & 0b011111;
    if(as_5 != as_32){
        warnMsg("'as' field constant is greater than 5bits");
    }
    return as_5;
}
string Scanner::read_label(string::iterator& it, string::iterator end){
    if(!is_alpha(*it)){
        errorMsg("Label parameter invalid, name must begin with a letter");
        return "";
    }
    string label = string() + *it;
    it++;
    for(;*it != ' ' && it != end; it++){
        if(is_alpha_numeric(*it))
            label += *it;
        else{
            string it_str = string() + *it;
            errorMsg("Label parameter invalid, expected digit, letter, or underscore, found '" + it_str + "' instead.");
            return "";
        }
    }
    return label;
}

uint32_t Scanner::instr_line(string instr, string::iterator& it, string::iterator end, string& label){
    if(op_map.find(instr) == op_map.end()){
        errorMsg("Unkown instruction '" + instr + "'.");
        return 0;
    }
    uint8_t op = op_map.at(instr);
    uint32_t out = 0;
    switch (op)
    {
    // ArithLog
    case 0b100001: // ADD
    case 0b100100: // AND
    case 0b100101: // OR
    case 0b101010: // SLT
    case 0b101011: // SLTU
    case 0b100011: // SUBU
    case 0b100110: // XOR
    {
        uint8_t rd = read_reg(it, end, true);
        if(error) return 0;
        if(it == end){
            errorMsg("Not enough parameters passed to instruction.");
            return 0;
        }
        skipWhiteSpace(it, end);
        if(error) return 0;
        uint8_t rs = read_reg(it, end, true);
        if(error) return 0;
        if(it == end){
            errorMsg("Not enough parameters passed to instruction.");
            return 0;
        }
        skipWhiteSpace(it, end);
        if(error) return 0;
        uint8_t rt = read_reg(it, end, false);
        if(error) return 0;
        if(it != end){
            expectWhiteSpace(it, end);
            if(error) return 0;
        }
        out |= rs;
        out = out << 5 | rt;
        out = out << 5 | rd;
        out = out << 11 | op;
        break;
    }
    // DivMult
    case 0b011010: // DIV
    case 0b011011: // DIVU
    case 0b011000: // MULT
    case 0b011001: // MULTU
    {
        uint8_t rs = read_reg(it, end, true);
        if(error) return 0;
        if(it == end){
            errorMsg("Not enough parameters passed to instruction.");
            return 0;
        }
        skipWhiteSpace(it, end);
        if(error) return 0;
        uint8_t rt = read_reg(it, end, false);
        if(error) return 0;
        if(it != end){
            expectWhiteSpace(it, end);
            if(error) return 0;
        }
        out |= rs;
        out = out << 5 | rt;
        out = out << 16 | op;
        break;
    }
    // Shift
    case 0b000000: // SLL
    case 0b000011: // SRA
    case 0b000010: // SRL
    {
        uint8_t rd = read_reg(it, end, true);
        if(error) return 0;
        if(it == end){
            errorMsg("Not enough parameters passed to instruction.");
            return 0;
        }
        skipWhiteSpace(it, end);
        if(error) return 0;
        uint8_t rt = read_reg(it, end, true);
        if(error) return 0;
        if(it == end){
            errorMsg("Not enough parameters passed to instruction.");
            return 0;
        }
        skipWhiteSpace(it, end);
        if(error) return 0;
        uint8_t as = read_as(it, end);
        if(error) return 0;
        if(it != end){
            expectWhiteSpace(it, end);
            if(error) return 0;
        }
        out |= rt;
        out = out << 5 | rd;
        out = out << 5 | as;
        out = out << 6 | op;
        break;
    }
    // ShiftV
    case 0b000100: // SLLV
    case 0b000111: // SRAV
    case 0b000110: // SRLV
    {
        uint8_t rd = read_reg(it, end, true);
        if(error) return 0;
        if(it == end){
            errorMsg("Not enough parameters passed to instruction.");
            return 0;
        }
        skipWhiteSpace(it, end);
        if(error) return 0;
        uint8_t rt = read_reg(it, end, true);
        if(error) return 0;
        if(it == end){
            errorMsg("Not enough parameters passed to instruction.");
            return 0;
        }
        skipWhiteSpace(it, end);
        if(error) return 0;
        uint8_t rs = read_reg(it, end, false);
        if(error) return 0;
        if(it != end){
            expectWhiteSpace(it, end);
            if(error) return 0;
        }
        out |= rs;
        out = out << 5 | rt;
        out = out << 5 | rd;
        out = out << 11 | op;
        break;
    }
    // JumpR
    case 0b001000: // JR
    {
        uint8_t rs = read_reg(it, end, false);
        if(error) return 0;
        if(it != end){
            expectWhiteSpace(it, end);
            if(error) return 0;
        }
        out |= rs;
        out = out << 21 | op;
        break;
    }
    // MoveTo
    case 0b010001: // MTHI
    case 0b010011: // MTLO
    {
        uint8_t rs = read_reg(it, end, false);
        if(error) return 0;
        if(it != end){
            expectWhiteSpace(it, end);
            if(error) return 0;
        }
        out |= rs;
        out = out << 21 | op;
        break;
    }
    // ArithLogI
    case 0b001001: // ADDIU
    case 0b001100: // ANDI
    case 0b001101: // ORI
    case 0b001010: // SLTI
    case 0b001011: // SLTIU
    case 0b001110: // XORI
    {
        uint8_t rt = read_reg(it, end, true);
        if(error) return 0;
        if(it == end){
            errorMsg("Not enough parameters passed to instruction.");
            return 0;
        }
        skipWhiteSpace(it, end);
        if(error) return 0;
        uint8_t rs = read_reg(it, end, true);
        if(error) return 0;
        if(it == end){
            errorMsg("Not enough parameters passed to instruction.");
            return 0;
        }
        skipWhiteSpace(it, end);
        if(error) return 0;
        uint16_t imm = read_imm(it, end);
        if(error) return 0;
        if(it != end){
            expectWhiteSpace(it, end);
            if(error) return 0;
        }
        out |= op;
        out = out << 5 | rs;
        out = out << 5 | rt;
        out = out << 16 | imm;
        break;
    }
    // LoadI
    case 0b011111:{ // LUI
        uint8_t rt = read_reg(it, end, true);
        if(error) return 0;
        if(it == end){
            errorMsg("Not enough parameters passed to instruction.");
            return 0;
        }
        skipWhiteSpace(it, end);
        uint16_t imm = read_imm(it, end);
        if(error) return 0;
        if(it != end){
            expectWhiteSpace(it, end);
            if(error) return 0;
        }
        out |= op;
        out = out << 10 | rt;
        out = out << 16 | imm;
        break;
    }
    // Branch
    case 0b1000100: // BEQ
    case 0b1000101: // BNE
    {
        uint8_t rs = read_reg(it, end, true);
        if(error) return 0;
        if(it == end){
            errorMsg("Not enough parameters passed to instruction.");
            return 0;
        }
        skipWhiteSpace(it, end);
        if(error) return 0;
        uint8_t rt = read_reg(it, end, true);
        if(error) return 0;
        if(it == end){
            errorMsg("Not enough parameters passed to instruction.");
            return 0;
        }
        skipWhiteSpace(it, end);
        if(error) return 0;
        label = read_label(it, end);
        if(error) return 0;
        if(it != end){
            expectWhiteSpace(it, end);
            if(error) return 0;
        }
        out |= op;
        out = out << 5 | rs;
        out = out << 5 | rt;
        out <<= 16;
        break;
    }
    // BranchZ
    case 0b1000111: // BGTZ
    case 0b1000110: // BLEZ
    case 0b1000001: // BLTZ
    {
        uint8_t rs = read_reg(it, end, true);
        if(error) return 0;
        if(it == end){
            errorMsg("Not enough parameters passed to instruction.");
            return 0;
        }
        skipWhiteSpace(it, end);
        if(error) return 0;
        label = read_label(it, end);
        if(error) return 0;
        if(it != end){
            expectWhiteSpace(it, end);
            if(error) return 0;
        }
        out |= op;
        out = out << 5 | rs;
        out <<= 21;
        break;
    }
    // Other BranchZ
    case 0b11100001: // BGEZ
    case 0b11110001: // BGEZAL
    case 0b11110000: // BLTZAL
    {
        uint8_t rs = read_reg(it, end, true);
        if(error) return 0;
        if(it == end){
            errorMsg("Not enough parameters passed to instruction.");
            return 0;
        }
        skipWhiteSpace(it, end);
        if(error) return 0;
        label = read_label(it, end);
        if(error) return 0;
        if(it != end){
            expectWhiteSpace(it, end);
            if(error) return 0;
        }
        out |= 0b000001;
        out = out << 5 | rs;
        out = out << 5 | (op | 0b11111);
        out <<= 16;
        break;
    }
    // LoadStore
    case 0b1100000: // LB
    case 0b1100100: // LBU
    case 0b1100001: // LH
    case 0b1100101: // LHU
    case 0b1100010: //LWL
    case 0b1100110: //LWR
    case 0b1101000: // SB
    case 0b1101001: // SH
    case 0b1101011: // SW
    {
        uint8_t rt = read_reg(it, end, true);
        if(error) return 0;
        if(it == end){
            errorMsg("Not enough parameters passed to instruction.");
            return 0;
        }
        skipWhiteSpace(it, end);
        uint16_t imm = read_imm(it, end, true);
        if(it == end){
            errorMsg("Not enough parameters passed to instruction.");
            return 0;
        }
        if(error) return 0;
        uint8_t rs = read_reg(it, end, false, true);
        if(error) return 0;
        if(it != end){
            expectWhiteSpace(it, end);
            if(error) return 0;
        }
        out |= op;
        out = out << 5 | rs;
        out = out << 5 | rt;
        out = out << 16 | imm;
        break;
    }
    // Jump
    case 0b1000010: // J
    case 0b1000011: // JAL
    {
        label = read_label(it, end);
        if(error) return 0;
        if(it != end){
            expectWhiteSpace(it, end);
            if(error) return 0;
        }
        out |= op;
        out <<= 26;
        break;
    }
    default:
        errorMsg("Instruction '" + instr + "' has not yet been implemented, but should exist.");
        return 0;
    }
    return out;
}

void Scanner::scanLine(string in){
    string str = "";

    auto it = in.begin();
    for(; it != in.end(); it++){
        // if first thing is an instr
        if(str.size() > 0 && (*it) == ' '){
            skipWhiteSpace(it, in.end());
            string label = "";
            tokens.push_back({str, instr_line(str, it, in.end(), label), line, memLine, label});
            memLine++;
            line++;
            return;
        }
        // if first thing is a label
        else if(str.size() > 0 && (*it) == ':'){
            labels.push_back({str, memLine, line, memLine, ""});
            str = "";
            continue;
        }
        // if first thing is a constant
        else if(str.size() == 0 && is_numeric(*it)){
            tokens.push_back({"CONST", const_line(it, in.end()), line, memLine, ""});
            line++;
            memLine++;
            return;
        }
        // if we don't know what the first thing is yet
        if(is_alpha_numeric(*it)){
            str += *it;
        }
        else if(*it != ' '){
            string it_str = string() + *it;
            errorMsg("Unexpected character '" + it_str + "'.");
            line++;
            return;
        }
    }
    if(str.size() != 0){
        errorMsg("Unexpected token '" + str + "'.");
        line++;
        return;
    }
    line++;
}

vector<Token>* Scanner::getLabelsAddr(){
    return &labels;
}

vector<Token>* Scanner::getTokensAddr(){
    return &tokens;
}