//
// Created by hamon11 on 12/27/17.
//

#include "Token.h"

Token::Token() {

}

Token::Token(const char lexeme[], Type type) {
    this->lexeme = std::string(lexeme);
    this->type = type;
}

string Token::getLexeme() const {
    return lexeme;
}

Token::Type Token::getType() const {
    return type;
}

string Token::toString() {
    return "Token: " + std::string(strEnums[type]) + " Lexeme: " + lexeme;
}