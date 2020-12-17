#include "scanner.h"

#include <iostream>
#include <sstream>

using namespace std;

// Token Stuff
Token::Token(string _name, uint32_t _data, uint32_t _line, uint32_t _memLine, string _label){
    name = _name;
    data = _data;

    label = _label;
    memLine = _memLine;

    line = _line;
}

// Scanner Stuff
Scanner::Scanner(){
    reset();
}
const unordered_map<string, uint8_t> Scanner::op_map = {
    //{OP_NAME, Special, OPCODE, RS, RT, RD, IMM}
    {"ADDIU",   0b1001001}, // ADDIU has extra 1 at the begining to avoid clonflict
    {"ADDU",    0b100001},
    {"AND",     0b100100},
    {"ANDI",    0b001100},
    {"BEQ",     0b1000100}, // Branch branch have extra 1 at the begining to avoid conflicts
    {"BGEZ",    0b11100001}, // Other BranchZ other branchz have 3 extra 1's at the begining to avoid conflict
    {"BGEZAL",  0b11110001}, // Other BranchZ 
    {"BGTZ",    0b1000111}, // BranchZ branch z have extra 1 at the begining to avoid conflicts
    {"BLEZ",    0b1000110}, // BranchZ
    {"BLTZ",    0b1000001}, // BranchZ
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
    {"LUI",     0b001111}, // LoadI
    {"LW",      0b1100011}, // LoadStore
    {"LWL",     0b1100010}, // LoadStore
    {"LWR",     0b1100110}, // LoadStore
    {"MTHI",    0b010001}, // MoveTo
    {"MTLO",    0b010011}, // MoveTo
    {"MFHI",    0b010000}, // MoveTo
    {"MFLO",    0b010010}, // MoveTo
    {"MULT",    0b011000},
    {"MULTU",   0b011001},
    {"NOP",     0b111111}, // I JUST CHOSE A RANDOM NUMBER THAT WASN'T IN USE
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
    memLine = 0xBFC00000;
    labels = vector<Token>();
    tokens = vector<Token>();
    error = false;
}

// function used to easily handle errors
void Scanner::errorMsg(string msg){
    error = true;
    cerr << "Error: [Line " << line << "] " << msg << endl;
}
// function used to easily handle warnings
void Scanner::warnMsg(string msg){
    cerr << "Warning: [Line " << line << "] " << msg << endl;
}
// loop through remainder of line and if anything non whitespace is encountered throw error
void Scanner::expectWhiteSpace(string::iterator& it, string::iterator end){
    bool allSpaces = true;
    string msg = "";
    while(it != end){
        if(*it == '#'){
            it = end;
            return;
        }
        allSpaces &= (*it == ' ');
        msg += *it;
        it++;
    }
    if(!allSpaces){
        error = true;
        errorMsg("Expected whitespace, found '" + msg + "' instead.");
    }
}
// skip any white space, if end of line is reached throw error
void Scanner::skipWhiteSpace(string::iterator& it, string::iterator end){
    while(*it == ' ' && it != end) it++;
    if(it == end) errorMsg("Expected more parameters, reached end of line instead.");
}

// static helpers taht provide quick checks
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

// expects remainder of line to be a constant, allows for decimanl, binary, and hex
// if anything else is found throws error
// assumes first character is valid
// also used to obtain imediates, and shifts in load instructions
uint32_t Scanner::const_line(string::iterator& it, string::iterator end, bool allow_paren){
    // keep track of string value
    string str_val = string() + *it;

    it++;
    if(it == end) return stol(str_val); // if 1 digit then return digit, must be decimal as both bin and hex require at least 3 chars

    int mult = 1;
    int valBase = 0; // 0 is for dec, 1 for hex, 2 for bin

    if(str_val == "0"){ // check what the base of the number is dec, bin, or hex
        if(is_numeric(*it) || (allow_paren && *it=='(')){
            str_val += *it;
        }
        else if(*it == 'x' || *it == 'X') valBase = 1;
        else if(*it == 'b' || *it == 'B') valBase = 2;
        else if(*it == ' '){ // whitespace indicates end of line, and nothing should apear after
            expectWhiteSpace(it, end);
            if(error) return 0;
            else return stol(str_val);
        }
        else{ // any other character is
            string it_str = string() + *it;
            errorMsg("Unexpected character '" + it_str + "' in constant.");
            return 0;
        }
    }
    else if(str_val == "-") mult = -1;

    // if valid base is 0 then don't advance at begining otherwise skip of b or x character
    // iterate until end of line
    for(it+=(valBase!=0); it != end; it++){
        bool is_valid = false;

        switch (valBase) // allow different characters depending on the base
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

        if(!is_valid){ // if encounters invalid charater check what kind
            if(*it == ' '){ // spaces are allowed as long as nothing comes after
                expectWhiteSpace(it, end);
                if(error) return 0;
                break;
            }else if(allow_paren && *it == '('){ // brackets are allowed if flag is enabled, this is used for certain load instructions
                break;
            }else{ // anything else is not allowed throw error
                string it_str = string() + *it;
                errorMsg("Unexpected character '" + it_str + "' in constant.");
                return 0;
            }
        }
        str_val += *it; // if no errors where encountered then add character to string value
    }

    switch (valBase) // process string value based on base
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
// tool to get register number
// specific required delimiters are checked for and consumer , and ) if required 
uint8_t Scanner::read_reg(string::iterator& it, string::iterator end, bool comma_terminated, bool paren_terminated, bool comma_or_white_terminated){
    if(*it != '$'){ // registers must begin with a $ symbol
        errorMsg("Expected a register.");
        return 0;
    }
    it++;
    if(it == end){ // can't have reached end
        errorMsg("Expected register number, reached end of line instead.");
        return 0;
    }
    uint8_t reg_num = 0;
    switch (*it) // check register name type and process differently
    {
    case 'a': // named registers all follow the same pattern
        it++;
        if(it == end || *it == ' '){ // check that we haven't reached the end
            errorMsg("Register 'a' does not exist.");
            return 0;
        }
        if(*it != 't' && !('0' <= *it && *it <= '3')){ // check valid second characters
            string it_str = string() + *it;
            errorMsg("'a' register must be followed by t or 0-3, found '" + it_str + "' instead.");
            return 0;
        }
        reg_num = (*it == 't')?1:(4 + (*it-'0')); // calculate the number the label corresponds to
        it++;
        break;
    case 'f':
        it++;
        if(it == end || *it == ' '){
            errorMsg("Register 'f' does not exist.");
            return 0;
        }
        if(*it != 'p'){
            string it_str = string() + *it;
            errorMsg("'f' register must be followed by p found '" + it_str + "' instead.");
            return 0;
        }
        reg_num = 30;
        it++;
        break;
    case 'g':
        it++;
        if(it == end || *it == ' '){
            errorMsg("Register 'g' does not exist.");
            return 0;
        }
        if(*it != 'p'){
            string it_str = string() + *it;
            errorMsg("'g' register must be followed by p found '" + it_str + "' instead.");
            return 0;
        }
        reg_num = 28;
        it++;
        break;
    case 'k':
        it++;
        if(it == end || *it == ' '){
            errorMsg("Register 'k' does not exist.");
            return 0;
        }
        if(*it != '0' && *it != '1'){
            string it_str = string() + *it;
            errorMsg("'k' registers must be followed by 0 or 1, found '" + it_str + "' instead.");
            return 0;
        }
        reg_num = 26 + (*it=='1');
        it++;
        break;
    case 'r':
        it++;
        if(it == end || *it == ' '){
            errorMsg("Register 'r' does not exist.");
            return 0;
        }
        if(*it != 'a'){
            string it_str = string() + *it;
            errorMsg("'r' register must be followed by a found '" + it_str + "' instead.");
            return 0;
        }
        reg_num = 31;
        it++;
        break;
    case 's':
        it++;
        if(it == end || *it == ' '){
            errorMsg("Register 's' does not exist.");
            return 0;
        }
        if(!('0'<=*it&&*it<='7') && *it != 'p'){
            string it_str = string() + *it;
            errorMsg("'s' register must be followed by p or 0-7, found '" + it_str + "' instead.");
            return 0;
        }
        reg_num = *it=='p'?29:(16 + (*it-'0'));
        it++;
        break;
    case 't':
        it++;
        if(it == end || *it == ' '){
            errorMsg("Register 't' does not exist.");
            return 0;
        }
        if(!is_numeric(*it)){
            string it_str = string() + *it;
            errorMsg("'t' register must be followed by 0-9, found '" + it_str + "' instead.");
            return 0;
        }
        reg_num = ('0'<=*it&&*it<='7')?(8 + (*it-'0')):(24+(*it=='9'));
        it++;
        break;
    case 'v':
        it++;
        if(it == end || *it == ' '){
            errorMsg("Register 'v' does not exist.");
            return 0;
        }
        if(*it != '0' && *it != '1'){
            string it_str = string() + *it;
            errorMsg("'v' registers must be followed by 0 or 1, found '" + it_str + "' instead.");
            return 0;
        }
        reg_num = 2 + (*it=='1');
        it++;
        break;
    case 'z': // zero register is the only to begin with z, so just check each character until zero pattern is matched
        it++;
        if(it == end || *it == ' '){
            errorMsg("Register 'z' does not exist.");
            return 0;
        }
        if(*it != 'e'){
            string it_str = string() + *it;
            errorMsg("Expected to find register 'zero', found 'z" + it_str + "' instead.");
            return 0;
        }
        it++;
        if(it == end || *it == ' '){
            errorMsg("Register 'ze' does not exist.");
            return 0;
        }
        if(*it != 'r'){
            string it_str = string() + *it;
            errorMsg("Expected to find register 'zero', found 'ze" + it_str + "' instead.");
            return 0;
        }
        it++;
        if(it == end || *it == ' '){
            errorMsg("Register 'zer' does not exist.");
            return 0;
        }
        if(*it != 'o'){
            string it_str = string() + *it;
            errorMsg("Expected to find register 'zero', found 'zer" + it_str + "' instead.");
            return 0;
        }
        reg_num = 0;
        it++;
        break;
    default: // numeric registers are also allowed
        if(is_numeric(*it)){
            string reg_str = string() + *it;
            it++;
            if(((comma_terminated|comma_or_white_terminated) && *it == ',') || (paren_terminated && *it == ')') || (!(comma_terminated || paren_terminated) && (it == end || *it == ' '))){ // check if expected terminator has been found, this indicates a single digit register, single digit will never be > 32 so just return value
                if(comma_terminated || paren_terminated) it++; // consume non space delimiter
                return stol(reg_str);
            }
            if(!is_numeric(*it)){ // if second character is non numeric and non delimiter then return error
                string it_str = string() + *it;
                errorMsg("Expected integer found '" + it_str + "' instead.");
                return 0;
            }
            reg_str += *it;
            
            reg_num = stol(reg_str); // set register number to found value
            if(reg_num > 31){ // check that found value is not greater than 31 as mips only has 31 registers
                errorMsg("Register numbers range between 0-31, found '" + to_string(reg_num) + "'.");
                return 0;
            }
            
            it++;
        }
        else{ // any other characters must be errros
            string it_str = string() + *it;
            errorMsg("Invalid regiter name begining with '" + it_str + "'.");
            return 0;
        }
        break;
    }

    if(((comma_terminated | (comma_or_white_terminated && (it != end && *it != ' '))) && *it != ',') || (paren_terminated && *it != ')') || (!(comma_terminated || paren_terminated || comma_or_white_terminated) && (it != end && *it != ' '))){ // check register has proper delimiter
        
        string it_str = string() + *it;
        if(comma_terminated)
            errorMsg("Expected comma, found '" + it_str + "' instead.");
        if(paren_terminated)
            errorMsg("Expected ')' found '" + it_str + "' instead.");
        else
            errorMsg("Expected space or end of line, found '" + it_str + "' instead.");
        return 0;
    }
    if(comma_terminated || paren_terminated) it++; // consumer delimiter if necessary

    return reg_num;
}
// tool to get imediate field
// ensures nothing comes after field
// warns if field is more than 16 bits but allows value in a truncated mannor
uint16_t Scanner::read_imm(string::iterator& it, string::iterator end, bool paren_terminated){
    if(!is_numeric(*it) && *it!='-'){ // check if first character is number
        errorMsg("Immediate parameter expects a constant.");
        return 0;
    }
    uint32_t imm_32 = const_line(it, end, paren_terminated); // use const line to obtain constant
    if(paren_terminated){ // if parentesis delimited ensure presence and consume
        if(*it != '('){
            string it_str = string() + *it;
            errorMsg("Imediate parameter expected '(' character, found '" + it_str + "' instead.");
            return 0;
        }
        else it++;
    }
    uint16_t imm_16 = imm_32;
    if((int16_t)imm_16 != (int32_t)imm_32){ // check that constant fits in 16 bits and warn otherwise
        warnMsg("Imediate constant is greater than 16bits");
    }
    return imm_32;
}
// identical to read_imm but ensures 5 bit constant, and doesn't check for delimiters
uint8_t Scanner::read_as(string::iterator& it, string::iterator end){
    if(!is_numeric(*it) && *it != '-'){
        errorMsg("'as' field expects a constant.");
        return 0;
    }
    uint32_t as_32 = const_line(it, end);
    uint8_t as_8 = as_32;
    uint8_t as_5 = as_8 & 0b011111;
    
    if((int8_t)as_8 != (int32_t)as_32 && (as_8<<3) != (as_5 << 3)){
        warnMsg("'as' field constant is greater than 5bits");
    }
    return as_5;
}
// tool to get label
// returns error only if label is not valid format
// labels must begin with a letter and only have letters, _'s and digits
string Scanner::read_label(string::iterator& it, string::iterator end){
    if(!is_alpha(*it)){ // check if first character is valid
        errorMsg("Label parameter invalid, name must begin with a letter");
        return "";
    }
    string label = string() + *it;
    it++;
    for(;*it != ' ' && it != end; it++){ // loop until white space
        if(is_alpha_numeric(*it)) // check if valid character otherwise throw error
            label += *it;
        else{
            string it_str = string() + *it;
            errorMsg("Label parameter invalid, expected digit, letter, or underscore, found '" + it_str + "' instead.");
            return "";
        }
    }
    return label;
}

// process instruction
uint32_t Scanner::instr_line(string instr, string::iterator& it, string::iterator end, string& label){
    if(op_map.find(instr) == op_map.end()){ // check if instruction is known
        errorMsg("Unkown instruction '" + instr + "'.");
        return 0;
    }
    uint8_t op = op_map.at(instr); // get instruction opcode
    uint32_t out = 0;
    switch (op) // process opcode based on pattern
    {
    // all patterns follow the same code logic
    // they use the above tools to get correct values and form the final instruction binary
    // they ensure that the format of the instruction is valid
    // anytime any of the above finds an error return 0 as binary
    // this is fine as the error flag is set to true and user of the scanner should check it before assuming that the output is valid

    // NOP
    case 0b111111:
    {
        if(it != end){
            expectWhiteSpace(it, end);
            if(error) return 0;
        }
        out = 0;
        break;
    }
    // ArithLog
    case 0b100001: // ADD
    case 0b100100: // AND
    case 0b100101: // OR
    case 0b101010: // SLT
    case 0b101011: // SLTU
    case 0b100011: // SUBU
    case 0b100110: // XOR
    {
        skipWhiteSpace(it, end); // skip any whitespace before arguments
        uint8_t rd = read_reg(it, end, true); // read register
        if(error) return 0;
        if(it == end){ // ensure end hasn't been reached
            errorMsg("Not enough parameters passed to instruction.");
            return 0;
        }
        skipWhiteSpace(it, end); // skip any whitespace
        if(error) return 0;
        uint8_t rs = read_reg(it, end, true); // read register
        if(error) return 0;
        if(it == end){ // ensure end hasn't been reached
            errorMsg("Not enough parameters passed to instruction.");
            return 0;
        }
        skipWhiteSpace(it, end); // etc...
        if(error) return 0;
        uint8_t rt = read_reg(it, end, false);
        if(error) return 0;
        if(it != end){
            expectWhiteSpace(it, end);
            if(error) return 0;
        }
        out |= rs; // use data gathered to form instruction binary representation
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
        skipWhiteSpace(it, end); // skip any whitespace before arguments
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
        skipWhiteSpace(it, end); // skip any whitespace before arguments
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
        skipWhiteSpace(it, end); // skip any whitespace before arguments
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
    case 0b001001: // JALR
    {
        skipWhiteSpace(it, end); // skip any whitespace before arguments
        uint8_t rdors = read_reg(it, end, false, false, true);
        if(error) return 0;
        if(it == end || *it == ' '){
            if(it != end){
                expectWhiteSpace(it, end);
                if(error) return 0;
            }
            out |= rdors; // as rs
            out = out << 10 | 31;
            out = out << 11 | op;
        }
        else if(*it == ','){
            it++;
            skipWhiteSpace(it, end);
            if(error) return 0;
            uint8_t rs = read_reg(it, end, false);
            if(error) return 0;
            if(it != end){
                expectWhiteSpace(it, end);
                if(error) return 0;
            }
            out |= rs;
            out = out << 10 | rdors; // as rd
            out = out << 11 | op;
        }
        else{
            string it_str = string() + *it;
            errorMsg("Unexpected character '" + it_str + "'.");
            if(error) return 0;
        }
        break;
    }
    // JumpMoveTo
    case 0b001000: // JR
    case 0b010001: // MTHI
    case 0b010011: // MTLO
    {
        skipWhiteSpace(it, end); // skip any whitespace before arguments
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
    case 0b010000: // MFHI
    case 0b010010: // MFLO
    {
        skipWhiteSpace(it, end); // skip any whitespace before arguments
        uint8_t rd = read_reg(it, end, false);
        if(error) return 0;
        if(it != end){
            expectWhiteSpace(it, end);
            if(error) return 0;
        }
        out |= rd;
        out = out << 11 | op;
        break;
    }
    // ArithLogI
    case 0b1001001: // ADDIU
    case 0b001100: // ANDI
    case 0b001101: // ORI
    case 0b001010: // SLTI
    case 0b001011: // SLTIU
    case 0b001110: // XORI
    {
        skipWhiteSpace(it, end); // skip any whitespace before arguments
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
    case 0b001111:{ // LUI
        skipWhiteSpace(it, end); // skip any whitespace before arguments
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
        skipWhiteSpace(it, end); // skip any whitespace before arguments
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
        skipWhiteSpace(it, end); // skip any whitespace before arguments
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
        skipWhiteSpace(it, end); // skip any whitespace before arguments
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
        out = out << 5 | (op & 0b11111);
        out <<= 16;
        break;
    }
    // LoadStore
    case 0b1100000: // LB
    case 0b1100100: // LBU
    case 0b1100001: // LH
    case 0b1100101: // LHU
    case 0b1100011: // LW
    case 0b1100010: //LWL
    case 0b1100110: //LWR
    case 0b1101000: // SB
    case 0b1101001: // SH
    case 0b1101011: // SW
    {
        skipWhiteSpace(it, end); // skip any whitespace before arguments
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
        skipWhiteSpace(it, end); // skip any whitespace before arguments
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

// takes the input of the line and processes every possible part
// allows for labels first, and the processes remaining characters as either an instruction or constant
void Scanner::scanLine(string in){
    string str = "";

    auto it = in.begin(); // the whole program uses iterators to control flow
    for(; it != in.end(); it++){
        // if first thing is an instr
        if(str.size() > 0 && (*it) == ' '){
            string label = ""; // this is variable is passed to instr_line, and can by reference to be changed if label instruction is found
            tokens.push_back({str, instr_line(str, it, in.end(), label), line, memLine, label});
            memLine += 4;
            line++;
            return;
        }
        // if first thing is a label
        else if(str.size() > 0 && (*it) == ':'){
            // cout << "Added label " << str << endl;
            labels.push_back({str, memLine, line, memLine, ""});
            str = ""; // reset str to "" to basically initialise the loop so it can find another label isntr or const line
            continue;
        }
        // if first thing is a constant
        else if(str.size() == 0 && (is_numeric(*it) || *it == '-')){ // constants must begin with constant
            tokens.push_back({"CONST", const_line(it, in.end()), line, memLine, ""});
            line++;
            memLine += 4;
            return;
        }
        // if we don't know what the first thing is yet
        if(is_alpha_numeric(*it)){
            str += *it;
        }
        else if(*it == '#'){ // if we find a comment
            it = in.end();
            return;
        }
        else if(*it != ' '){ // we have already checked all valid inputs so anything else is an error
            string it_str = string() + *it;
            errorMsg("Unexpected character '" + it_str + "'.");
            line++;
            return;
        }
    }
    if(str.size() != 0){ // at the end a string should've been processed
        if(str == "NOP"){
            tokens.push_back({str, 0, line, memLine, ""});
            memLine += 4;
            line++;
            return;
        }
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