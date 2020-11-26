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

    // initializer
    Token(std::string, uint32_t);

    // helper functions
    uint32_t get_range(int, int);
};

class Scanner{
private:
    uint32_t line;
    uint32_t memLine;
    static const std::unordered_map<std::string, uint8_t> op_map;
    static const std::unordered_map<std::string, int> reg_map;
    std::vector<Token> labels;
    std::vector<Token> tokens;

    void errorMsg(std::string);
    void expectWhiteSpace(std::string::iterator&, std::string::iterator);
    void skipWhiteSpace(std::string::iterator&, std::string::iterator);

    uint32_t const_line(std::string::iterator&, std::string::iterator);
    uint8_t read_reg(std::string::iterator&, std::string::iterator, bool);
    uint32_t instr_line(std::string, std::string::iterator&, std::string::iterator);

public:
    Scanner();
    
    bool error;
    
    void reset();
    void scanLine(std::string);
    std::vector<Token>* getLabelsAddr();
    std::vector<Token>* getTokensAddr();
};

#endif