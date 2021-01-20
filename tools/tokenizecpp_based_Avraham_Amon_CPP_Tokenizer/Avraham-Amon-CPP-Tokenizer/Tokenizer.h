//
// Created by hamon11 on 12/27/17.
//

#ifndef C_TOKENIZER_TOKENIZER_H
#define C_TOKENIZER_TOKENIZER_H

#include <stack>
#include <map>
#include <set>
#include <fstream>
#include <iostream>
#include <cstring>
#include "Token.h"

using namespace std;

class Tokenizer {

public:
    explicit Tokenizer(string filePath);
    Token nextToken();
private:

    std::stack<char> stk;
    char token[10000];
    char potentialKw[10000];
    std::map<string, Token::Type> keywordTypes;
    std::set<char> punctuators;
    std::set<char> hexSet;
    ifstream fileReader;

    Token checkConstantNumbers(char token[], char c, int i);
    Token checkConstantString(char token[], char c, int i);
    Token checkConstantChar(char token[], char c, int i);
    Token checkBlockComment(char token[]);
    Token checkLineComment(char token[]);
    Token checkIdentifier(char token[]);
    Token checkKeyword(char token[]);
    Token checkPunctuators(char token[]);
    char getChar();
    void ungetChar(char c);
    void skipWhiteSpaces();
    void initKeywordMap();
    void initPuncSet();
    void initHexSet();
    bool isIntSuffix(char c);
    bool isFloatSuffix(char c);
    void memSetArrays();
};


#endif //C_TOKENIZER_TOKENIZER_H
