#include "scanner.h"

#include <iostream>
#include <sstream>

using namespace std;

// Token Stuff
Token::Token(string _name, uint32_t _data){
    name = _name;
    data = _data;
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
    {"ADDI",    0},
    {"ADDIU",   0},
    {"ADDU",    0b100001},
    {"AND",     0},
    {"ANDI",    0},
    {"BEQ",     0},
    {"BEQL",    0},
    {"BGEZ",    0}
};

void Scanner::reset(){
    line = 0;
    memLine = 0;
    labels = vector<Token>();
    tokens = vector<Token>();
    error = false;
}

void Scanner::errorMsg(string msg){
    error = true;
    cerr << "[Line " << line << "] " << msg << endl;
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

uint32_t Scanner::const_line(string::iterator& it, string::iterator end){
    string str_val = "";
    str_val += *it;

    it++;
    if(it == end) return stol(str_val);
    
    int valBase = 0; // 0 is for dec, 1 for hex, 2 for bin

    if(str_val == "0"){
        if(is_numeric(*it)){
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
uint8_t Scanner::read_reg(string::iterator& it, string::iterator end, bool comma_terminated){
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
    if(comma_terminated ? *it == ',' : (it == end || *it == ' ')){
        if(comma_terminated) it++;
        return stol(reg_str);
    }
    if(!is_numeric(*it)){
        string it_str = string() + *it;
        errorMsg("Expected integer found '" + it_str + "' instead.");
        return 0;
    }
    reg_str += *it;
    it++;
    if(comma_terminated ? *it != ',' : (it != end && *it != ' ')){
        string it_str = string() + *it;
        if(comma_terminated)
            errorMsg("Expected comma, found '" + it_str + "' instead.");
        else
            errorMsg("Expected space or end of line, found '" + it_str + "' instead.");
        return 0;
    }
    if(comma_terminated) it++;
    uint8_t reg_num = stol(reg_str);
    if(reg_num > 31){
        errorMsg("Register numbers range between 0-31, found '" + to_string(reg_num) + "'.");
        return 0;
    }
    return reg_num;
}
uint32_t Scanner::instr_line(string instr, string::iterator& it, string::iterator end){
    if(op_map.find(instr) == op_map.end()){
        errorMsg("Unkown instruction '" + instr + "'.");
        return 0;
    }
    uint8_t op = op_map.at(instr);
    uint32_t out = 0;
    switch (op)
    {
    // 3 register expressions
    case 0b100001:{ // ADD
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
        uint8_t rd = read_reg(it, end, false);
        if(error) return 0;
        if(it != end){
            expectWhiteSpace(it, end);
            if(error) return 0;
        }
        out |= rs;
        out = out << 5 | rt;
        out = out << 5 | rd;
        out = out << 10 | op;
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
    // if first thing is a constant
    if(is_numeric(*it)){
        tokens.push_back({"CONST", const_line(it, in.end())});
        line++;
        memLine++;
        return;
    }
    for(; it != in.end(); it++){
        // if first thing is an instr
        if(str.size() > 0 && (*it) == ' '){
            skipWhiteSpace(it, in.end());
            tokens.push_back({str, instr_line(str, it, in.end())});
            memLine++;
            line++;
            return;
        }
        // if first thing is a label
        if((*it) == ':'){
            labels.push_back(Token(str, memLine));
            str = "";
        }
        // if we don't know what the first thing is yet
        if(is_alpha_numeric(*it)){
            str += *it;
        }
    }
    if(str.size() != 0){
        errorMsg("Unexpected token '" + str + "'.");
        line++;
        return;
    }
    line++;
}

vector<Token>* Scanner::getTokensAddr(){
    return &tokens;
}