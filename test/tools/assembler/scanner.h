#pragma once
#ifndef scanner_h
#define scanner_h

#include <unordered_map>
#include <vector>

struct Token{
    // type of token
    bool is_operator;

    // token data
    uint32_t data;

    // initializer
    Token(bool _is_operator, uint32_t);

    // helper functions
    int get_reg(int);
    uint16_t get_imm();
};

class Scanner{
private:
    static std::unordered_map<std::string, uint16_t> op_map;
    static std::unordered_map<std::string, uint> reg_map;
    std::vector<Token> tokens;
public:
    Scanner();
    void reset();
    void scanLine();
    std::vector<Token>* getTokensAddr();
};

#endif