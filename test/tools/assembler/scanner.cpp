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
    {"ADD",     0b1000000},
    {"ADDI",    0},
    {"ADDIU",   0},
    {"ADDU",    0},
    {"AND",     0},
    {"ANDI",    0},
    {"BEQ",     0},
    {"BEQL",    0},
    {"BGEZ",    0}
};

void Scanner::reset(){
    tokens = vector<Token>();
}

inline static bool is_alpha(char c){
    return ('a' <= c && c <='z') || ('A' <= c && c <= 'Z');
}
inline static bool is_bin(char c){
    return c == '1' || c == '0';
}
inline static bool is_numeric(char c){
    return '1' <= c && c <= '9';
}
inline static bool is_hex(char c){
    return is_numeric || ('a' <= c && c <='f') || ('A' <= c && c <= 'F')
}
inline static bool is_alpha_numeric(char c){
    return is_alpha(c) || is_numeric(c) || (c == '_') ;
}

uint32_t Scanner::const_line(string::iterator it, string::iterator end){
    string str_val = "";
    str_val += *it;

    it++;
    if(it == end) return stoi(str_val);
    
    int valBase = 0; // 0 is for dec, 1 for hex, 2 for bin

    if(str_val == "0"){
        if(is_numeric(*it)){
            str_val += *it;
        }
        else if(*it == 'x' || *it == 'X') valBase = 1;
        else if(*it == 'b' || *it == 'B') valBase = 2;
        else if(*it == ' '){
            bool allSpaces = true;
            while(it != end){
                allSpaces &= (*it == ' ');
            }
            if(!allSpaces){
                error = true;
                cerr << "[Line " << line << "] Constants can only be decimal hex or binary numbers.";
                return 0;
            }
            return stoi(str_val);
        }
        else{
            error = true;
            cerr << "[Line " << line << "] Unexpected character '" << *it << "' in constant.";
            return 0;
        }
    }

    for(it++; it != end; it++){
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
                bool allSpaces = true;
                while(it != end){
                    allSpaces &= (*it == ' ');
                }
                if(!allSpaces){
                    error = true;
                    cerr << "[Line " << line << "] Constants can only be decimal hex or binary numbers.";
                    return 0;
                }
            }else{
                error = true;
                cerr << "[Line " << line << "] Unexpected character '" << *it << "' in constant.";
                return 0;
            }
        }
        str_val += *it;
    }

    switch (valBase)
    {
    case 0:
        return stoi(str_val);
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
uint32_t Scanner::instr_line(string::iterator it, string::iterator end){
    
}

void Scanner::scanLine(string line){
    string str = "";

    auto it = line.begin();
    // if first thing is a constant
    if(!is_numeric(*it)){
        tokens.push_back({"CONST", const_line(it, line.end())});
    }
    for(; it != line.end(); it++){
        // if first thing is an instr
        if(str.size() > 0 && (*it) == ' '){
            tokens.push_back({str, instr_line(it, line.end())});
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
    //!TODO should never reach this point
}

vector<Token>* Scanner::getTokensAddr(){
    return &tokens;
}