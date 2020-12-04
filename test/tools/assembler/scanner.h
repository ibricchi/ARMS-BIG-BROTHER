#pragma once
#ifndef scanner_h
#define scanner_h

#include <unordered_map>
#include <vector>
#include <string>

struct Token{
    // type of token
    std::string name;

    // token data
    uint32_t data;

    // missing data (in case of label)
    uint32_t memLine;
    std::string label;

    // debug data
    uint32_t line;

    // initializer
    Token(std::string, uint32_t, uint32_t, uint32_t, std::string);
};

class Scanner{
private:
    // information to keep track of scanning location
    uint32_t line;
    uint32_t memLine;

    // table of all opcode to binary translateions
    static const std::unordered_map<std::string, uint8_t> op_map;
    
    // keep track of tokens
    std::vector<Token> labels;
    std::vector<Token> tokens;

    // helper tools
    void errorMsg(std::string);
    void warnMsg(std::string);
    void expectWhiteSpace(std::string::iterator&, std::string::iterator);
    void skipWhiteSpace(std::string::iterator&, std::string::iterator);

    // instruction line helper tools
    uint8_t read_reg(std::string::iterator&, std::string::iterator, bool=false, bool=false, bool=false);
    uint16_t read_imm(std::string::iterator&, std::string::iterator, bool=false);
    uint8_t read_as(std::string::iterator&, std::string::iterator);
    std::string read_label(std::string::iterator&, std::string::iterator);
    
    // reads specific line type
    uint32_t const_line(std::string::iterator&, std::string::iterator, bool=false);
    uint32_t instr_line(std::string, std::string::iterator&, std::string::iterator, std::string&);

public:
    Scanner();
    
    // keeps track of if errors where encountered
    bool error;
    
    // public API
    void reset();
    void scanLine(std::string);
    std::vector<Token>* getLabelsAddr();
    std::vector<Token>* getTokensAddr();
};

#endif