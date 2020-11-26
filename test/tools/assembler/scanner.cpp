#include "scanner.h"

Token::Token(bool _is_operator, uint32_t _data){
    is_operator = _is_operator;
    data = _data;
}

int Token::get_reg(int i){
    return (data >> (16 - 4 * (3-i))) & 0xF;
}

uint16_t Token::get_imm(){
    return data;
}

