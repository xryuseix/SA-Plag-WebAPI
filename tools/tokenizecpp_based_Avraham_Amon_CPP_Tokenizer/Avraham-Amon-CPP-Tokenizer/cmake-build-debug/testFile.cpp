using namespace std;

Tokenizer::Tokenizer(string filePath){
    this->fileReader.open(filePath);
    if(!fileReader){
        cout << "Couldn't open file " + filePath << endl;
        exit(1);
    }
    this->initKeywordMap();
    this->initPuncSet();
    this->initHexSet();
}

Token Tokenizer::nextToken() {
    memSetArrays();
    char c;
    int i = 0;
    Token tk;

    skipWhiteSpaces();

    //if at end of input stream
    c = getChar();
    if((int) c == 0){
        return Token("EOF", Token::END);
    }

    token[i++] = c;

    //check for integer or float
    tk = checkConstantNumbers(token, c, i);
    if(tk.getType() != Token::NULL_TYPE){
        return tk;
    }

    //check for string constant
    tk = checkConstantString(token, c, i);
    if(tk.getType() != Token::NULL_TYPE){
        return tk;
    }

    //check for char constant
    tk = checkConstantChar(token, c, i);
    if(tk.getType() != Token::NULL_TYPE){
        return tk;
    }

    //check for  block comment
    tk = checkBlockComment(token);
    if(tk.getType() != Token::NULL_TYPE){
        return tk;
    }

    //check for line comment
    tk = checkLineComment(token);
    if(tk.getType() != Token::NULL_TYPE){
        return tk;
    }

    //check for keyword
    tk = checkKeyword(token);
    if(tk.getType() != Token::NULL_TYPE){
        return tk;
    }

    //check for punctuators
    tk = checkPunctuators(token);
    if(tk.getType() != Token::NULL_TYPE){
        return tk;
    }

    //check for identifier
    tk = checkIdentifier(token);
    if(tk.getType() != Token::NULL_TYPE) {
        return tk;
    }

    return Token("", Token::NULL_TYPE);
}

Token Tokenizer::checkConstantNumbers(char *token, char c, int i) {
    int state;
    if(isdigit(c) || c == '.'){
        if(c == '.'){
            state = 4;
            c = getChar();
            char d = getChar();
            bool variadic = c == '.' && d == '.';
            if(variadic){
                return Token("...", Token::VARIADIC);
            }
            else if(!isdigit(c)){
                ungetChar(d);
                ungetChar(c);
                return Token(".", Token::PERIOD);
            }
            ungetChar(d);
            ungetChar(c);
        }
        else {
            char d = getChar();
            if(c == '0' && isdigit(d)){
                state = 9;
            }
            else {
                state = 2;
            }
            ungetChar(d);
        }

        c = getChar();
        while(state > 0){
            switch (state) {
                case 2:
                    if (isdigit(c))
                        state = 2;
                    else if (c == 'e' || c == 'E')
                        state = 5;
                    else if (c == '.')
                        state = 3;
                    else if (isIntSuffix(c)) {
                        state = 12;
                    }
                    else
                        state = -2;
                    break;
                case 3:
                    if (isdigit(c))
                        state = 8;
                    else
                        state = -3;
                    break;
                case 4:
                    if (isdigit(c))
                        state = 3;
                    else
                        state = -4;
                    break;
                case 5:
                    if (isdigit(c))
                        state = 6;
                    else if (c == '-' || c == '+')
                        state = 7;
                    else
                        state = -5;
                    break;
                case 6:
                    if (isdigit(c))
                        state = 6;
                    else if (isFloatSuffix(c)){
                        state = 19;
                    }
                    else
                        state = -6;
                    break;
                case 7:
                    if(isdigit(c))
                        state = 6;
                    else
                        state = -7;
                    break;
                case 8:
                    if(isdigit(c))
                        state = 8;
                    else if(c == 'e' || c == 'E')
                        state = 5;
                    else if(isFloatSuffix(c)) {
                        state = 19;
                    }
                    else
                        state = -8;
                    break;
                case 9:
                    if(c == 'x' || c == 'X'){
                        state = 11;
                    }
                    else{
                        state = 10;
                    }
                    break;

                    int num;
                case 10:
                    num = c - '0';
                    if(isdigit(c) && num < 7)
                        state = 10;
                    else if(isIntSuffix(c)){
                        state = 12;
                    }
                    else
                        state = -10;
                    break;
                case 11:
                    if(isdigit(c) || hexSet.find(c) != hexSet.end())
                        state = 11;
                    else if(isIntSuffix(c)){
                        state = 12;
                    }
                    else
                        state = -11;
                    break;
                case 12:
                    if(c == 'U')
                        state = 13;
                    else if(c == 'u')
                        state = 14;
                    else if(c == 'L')
                        state = 15;
                    else if(c == 'l')
                        state = 16;
                    else if(punctuators.find(c) != punctuators.end())
                        state = -2;
                    else
                        state = -12;
                    break;
                case 13:
                    if(c == 'L')
                        state = 15;
                    else if(punctuators.find(c) != punctuators.end())
                        state = -2;
                    else
                        state = -12;
                    break;
                case 14:
                    if(c == 'l')
                        state = 16;
                    else if(punctuators.find(c) != punctuators.end())
                        state = -2;
                    else
                        state = -12;
                    break;
                case 15:
                    if(c == 'L')
                        state = 17;
                    else if(punctuators.find(c) != punctuators.end())
                        state = -2;
                    else
                        state = -12;
                    break;
                case 16:
                    if(c == 'l')
                        state = 18;
                    else if(punctuators.find(c) != punctuators.end())
                        state = -2;
                    else
                        state = -12;
                    break;
                case 17:
                    state = -2;
                    break;
                case 18:
                    if(punctuators.find(c) != punctuators.end())
                        state = -2;
                    else
                        state = -12;
                    break;
                case 19:
                    if(punctuators.find(c) != punctuators.end()){
                        state = -6;
                    }
                    else
                        state = -12;
                    break;
                case 20:
                    state = -6;
                    break;
                default:
                    state = -1;
            }
            if (state > 0) {
                token[i++] = c;
                token[i] = '\0';
                c = getChar();
            }
        }
        ungetChar(c);
        switch(-state){
            case 2:
                return Token(token, Token::INTEGER_CONSTANT);
                //CLion IDE says that "1." format in case -3 turns number to zero. Online gcc compilers just make
                //it an integer, usually a very small negative. I give it the lexeme 0 with INTEGER_CONSTANT as token.
            case 3:
                return Token("0", Token::INTEGER_CONSTANT);
            case 4:
                return Token(token, Token::PERIOD);
            case 6:
                return Token(token, Token::FLOAT_CONSTANT);
            case 8:
                return Token(token, Token::FLOAT_CONSTANT);
            case 10:
                return Token(token, Token::BAD_OCTAL);
            case 11:
                return Token(token, Token::BAD_HEX);
            case 12:
                return Token(token, Token::BAD_SUFFIX_TOKEN);
            default:
                return Token(token, Token::BAD_FLOAT);
        }
    }
    return Token("", Token::NULL_TYPE);
}


Token Tokenizer::checkConstantString(char *token, char c, int i) {
    if(c == '"'){
        bool unbalancedQuote = false;
        while((c = getChar()) != '"' ||
              (token[i - 1] == '\\' && token[i - 2] != '\\')){
            if(c == '"' || c == 0){
                unbalancedQuote = true;
            }
            else{
                unbalancedQuote = false;
            }
            if((int) c == 0 )
                break;
            token[i++] = c;
        }
        token[i] = c;

        if(c == '"'){
            unbalancedQuote = false;
        }

        if(unbalancedQuote)
            return Token(token, Token::BAD_STRING);

        return Token(token, Token::STRING_CONSTANT);
    }
    return Token("", Token::NULL_TYPE);
}

Token Tokenizer::checkConstantChar(char *token, char c, int i) {
    if(c == '\''){
        bool unbalancedSingleQuote = false;
        while((c = getChar()) != '\'' ||
              (token[i - 1] == '\\' && token[i - 2] != '\\')){
            if(c =='\'' || c == 0){
                unbalancedSingleQuote = true;
            }
            else{
                unbalancedSingleQuote = false;
            }
            if((int) c == 0 )
                break;
            token[i++] = c;
        }
        token[i] = c;

        if(c == '\''){
            unbalancedSingleQuote = false;
        }

        if(unbalancedSingleQuote)
            return Token(token, Token::BAD_CHAR);

        return Token(token, Token::CHAR_CONSTANT);
    }
    return Token("", Token::NULL_TYPE);
}

Token Tokenizer::checkBlockComment(char token[]){
    char c = getChar();
    if(token[0] == '/' && c == '*') {
        token[1] = '*';
        char first = getChar();
        char second = getChar();
        while (first != '*' && second != '/') {
            if((int) first == 0){
                return Token(token, Token::BAD_COMMENT);
            }
            first = second;
            second = getChar();
        }
        return Token(token, Token::BLOCK_COMMENT);
    }
    ungetChar(c);
    return Token("", Token::NULL_TYPE);
}

Token Tokenizer::checkLineComment(char token[]){
    char c = getChar();
    if(token[0] == '/' && c =='/'){
        token[1] = '/';
        while((c = getChar()) != '\n' && c != 0){
            //Nothing to do, parse until we hit the next line.
        }
        return Token(token, Token::LINE_COMMENT);
    }
    ungetChar(c);
    return Token("", Token::NULL_TYPE);
}

Token Tokenizer::checkIdentifier(char token[]){
    char c = token[0];
    int i = 1;
    if(isalpha(c) || c == '_' || c == '$'){
        c = getChar();
        while(isalpha(c) || isdigit(c) || c == '_' || c == '$'){
            token[i++] = c;
            c = getChar();
        }
        ungetChar(c);

        return Token(token, Token::IDENTIFIER);
    }
    return Token("", Token::NULL_TYPE);
}

Token Tokenizer::checkKeyword(char token[]){
    int i = 1;
    if(isspace(token[0])){
        token[0] = 0;
        i = 0;
    }

    potentialKw[0] = token[0];

    char c = getChar();

    while(!isspace(c) && c != 0 && c != '(' && c != '{' && c != ':' && c != '['){
        potentialKw[i++] = c;
        c = getChar();
    }
    if(c == ' ' || c == '(' || c == '{' || c == ':' || c == '['){
        ungetChar(c);
    }

    if(keywordTypes.find(std::string(potentialKw)) != keywordTypes.end()){
        return Token(potentialKw, keywordTypes.find(std::string(potentialKw))->second);
    }
    else{
        //> and not >= because we already have the first character in the token[].
        //when the next function is called, the first character should be the one
        //looked at in the token[], and then the stack.
        int j;
        for(j = i - 1; j > 0; j--){
            ungetChar(potentialKw[j]);
        }
    }
    return Token("", Token::NULL_TYPE);
}

Token Tokenizer::checkPunctuators(char token[]){
    char c = token[0];
    char first;
    char second;
    char third;
    switch(c){
        case '[':
            return Token(token, Token::OPEN_BRACKET);
        case ']':
            return Token(token, Token::CLOSE_BRACKET);
        case '(':
            return Token(token, Token::OPEN_PARENT);
        case ')':
            return Token(token, Token::CLOSE_PARENT);
        case '{':
            return Token(token, Token::OPEN_BRACE);
        case '}':
            return Token(token, Token::CLOSE_BRACE);
        case '.': //case covered in numbers method. Gets periods and variadics. checks for .. and returns bad token.
            first = getChar();
            if(isalpha(first) || first == '_'){ //identifier next
                ungetChar(first);
                return Token(token, Token::MEMBERSHIP);
            }
            else if(first == '.'){
                token[1] = first;
                return Token(token, Token::BAD_PUNCTUATOR);
            }
            else if(first == '*'){
                token[1] = first;
                return Token(token, Token::MEMBER_SELECT_POINTER);
            }
            else{
                ungetChar(first);
                return Token("", Token::NULL_TYPE);
            }
        case '-':
            first = getChar();
            if(first == '>') {
                token[1] = first;
                return Token(token, Token::ARROW);
            }
            else if(first == '-'){
                token[1] = first;
                return Token(token, Token::DECREMENT);
            }
            else if(first == '='){
                token[1] = first;
                return Token(token, Token::MINUS_EQUALS);
            }
            else{
                ungetChar(first);
                return Token(token, Token::SUB_OP);
            }
        case '+':
            first = getChar();
            if(first == '+'){
                token[1] = first;
                return Token(token, Token::INCREMENT);
            }
            else if(first == '='){
                token[1] = first;
                return Token(token, Token::PLUS_EQUALS);
            }
            else{
                ungetChar(first);
                return Token(token, Token::ADD_OP);
            }
        case '*':
            first = getChar();
            if(first == '='){
                token[1] = first;
                return Token(token, Token::MULT_EQUALS);
            }
            else{
                ungetChar(first);
                return Token(token, Token::MULT_OP);
            }
        case '/':
            first = getChar();
            if(first == '='){
                token[1] = first;
                return Token(token, Token::DIV_EQUALS);
            }
            else{
                ungetChar(first);
                return Token(token, Token::DIV_OP);
            }
        case '%':
            first = getChar();
            second = getChar();
            third = getChar();
            if(first == ':' && second == '%' && third == ':'){
                token[1] = first;
                token[2] = second;
                token[3] = third;
                return Token(token, Token::PERCENT_COLON_PERCENT_COLON);
            }
            else if(first == '='){
                token[1] = first;
                ungetChar(third);
                ungetChar(second);
                return Token(token, Token::MOD_EQUALS);
            }
            else if(first == ':'){
                token[1] = first;
                ungetChar(third);
                ungetChar(second);
                return Token(token, Token::PERCENT_COLON);
            }
            else if(first == '>'){
                token[1] = first;
                return Token(token, Token::RIGHT_ANGLE_PERCENT);
            }
            else{
                ungetChar(third);
                ungetChar(second);
                ungetChar(first);
                return Token(token, Token::MOD_OP);
            }
        case '&':
            first = getChar();
            if(first == '&'){
                token[1] = first;
                return Token(token, Token::LOG_AND);
            }
            else if(first == '='){
                token[1] = first;
                return Token(token, Token::ASSIGN_AND);
            }
            else{
                ungetChar(first);
                return Token(token, Token::BIT_AND);
            }
        case '~':
            return Token(token, Token::BIT_NOT);
        case '!':
            first = getChar();
            if(first == '='){
                token[1] = first;
                return Token(token, Token::NOT_EQUAL);
            }
            else{
                ungetChar(first);
                return Token(token, Token::LOG_NOT);
            }
        case '^':
            first = getChar();
            if(first == '='){
                token[1] = first;
                return Token(token, Token::ASSIGN_XOR);
            }
            else{
                ungetChar(first);
                return Token(token, Token::BIT_XOR);
            }
        case '|':
            first = getChar();
            if(first == '|'){
                token[1] = first;
                return Token(token, Token::LOG_OR);
            }
            else if(first == '='){
                token[1] = first;
                return Token(token, Token::ASSIGN_OR);
            }
            else{
                ungetChar(first);
                return Token(token, Token::BIT_OR);
            }
        case '<':
            first = getChar();
            second = getChar();
            if(first == '<' && second == '='){
                token[1] = first;
                token[2] = second;
                return Token(token, Token::ASSIGN_LEFT);
            }
            else if(first == '<'){
                token[1] = first;
                ungetChar(second);
                return Token(token, Token::BIT_LEFT);
            }
            else if(first == '='){
                token[1] = first;
                ungetChar(second);
                return Token(token, Token::LESS_THAN_EQUAL);
            }
            else if(first == ':'){
                token[1] = first;
                ungetChar(second);
                return Token(token, Token::LEFT_ANGLE_COLON);
            }
            else if(first == '%'){
                token[1] = first;
                ungetChar(second);
                return Token(token, Token::LEFT_ANGLE_PERCENT);
            }
            else{
                ungetChar(second);
                ungetChar(first);
                return Token(token, Token::LESS_THAN);
            }
        case '>':
            first = getChar();
            second = getChar();
            if(first == '>' && second == '='){
                token[1] = first;
                token[2] = second;
                return Token(token, Token::ASSIGN_RIGHT);
            }
            else if(first == '>'){
                token[1] = first;
                ungetChar(second);
                return Token(token, Token::BIT_RIGHT);
            }
            else if(first == '='){
                token[1] = first;
                ungetChar(second);
                return Token(token, Token::GREATER_THAN_EQUAL);
            }
            else{
                ungetChar(second);
                ungetChar(first);
                return Token(token, Token::GREATER_THAN);
            }
        case '=':
            first = getChar();
            if(first == '='){
                token[1] = first;
                return Token(token, Token::EQUAL);
            }
            else{
                ungetChar(first);
                return Token(token, Token::ASSIGN);
            }
        case '?':
            first = getChar();
            if(first == ':'){
                token[1] = first;
                return Token(token, Token::CONDITIONAL);
            }
            return Token(token, Token::TERNARY);
        case ':':
            first = getChar();
            if(first == '>'){
                token[1] = first;
                return Token(token, Token::RIGHT_ANGLE_COLON);
            }
            else if(first == ':'){
                token[1] = first;
                return Token(token, Token::SCOPE_RESOLUTION);
            }
            else{
                ungetChar(first);
                return Token(token, Token::COLON);
            }
        case ';':
            return Token(token, Token::SEMI_COLON);
        case ',':
            return Token(token, Token::COMMA);
        case '#':
            return Token(token, Token::POUND);
        default:
            return Token("", Token::NULL_TYPE);
    }
}


char Tokenizer::getChar() {
    if(stk.empty()){
        char code;
//        fp >> code;
        if(fileReader >> std::noskipws >> code){
            return code;
        }
    }
    else{
        char c = stk.top();
        stk.pop();
        return c;
    }
    return 0;
}

void Tokenizer::skipWhiteSpaces(){
    bool whiteSpace = true;
    char ch = getChar();
    int c = (int) ch;
    while(whiteSpace && c > 0){
        if(isspace(c) || c == '\r'){
            c = getChar();
        } else {
            whiteSpace = false;
            ungetChar((char) c);
        }
    }
}

void Tokenizer::ungetChar(char c){
    stk.push(c);
}

bool Tokenizer::isIntSuffix(char c){
    return c == 'l' || c == 'L' || c == 'u' || c == 'U';
}

bool Tokenizer::isFloatSuffix(char c){
    return c == 'f' || c == 'F' || c == 'l' || c == 'L';
}

void Tokenizer::initKeywordMap(){
    keywordTypes["auto"] = Token::AUTO_KEYWORD;
    keywordTypes["break"] = Token::BREAK_KEYWORD;
    keywordTypes["case"] = Token::CASE_KEYWORD;
    keywordTypes["char"] = Token::CHAR_KEYWORD;
    keywordTypes["const"] = Token::CONST_KEYWORD;
    keywordTypes["continue"] = Token::CONTINUE_KEYWORD;
    keywordTypes["default"] = Token::DEFAULT_KEYWORD;
    keywordTypes["do"] = Token::DO_KEYWORD;
    keywordTypes["double"] = Token::DOUBLE_KEYWORD;
    keywordTypes["else"] = Token::ELSE_KEYWORD;
    keywordTypes["enum"] = Token::ENUM_KEYWORD;
    keywordTypes["extern"] = Token::EXTERN_KEYWORD;
    keywordTypes["float"] = Token::FLOAT_KEYWORD;
    keywordTypes["for"] = Token::FOR_KEYWORD;
    keywordTypes["goto"] = Token::GOTO_KEYWORD;
    keywordTypes["if"] = Token::IF_KEYWORD;
    keywordTypes["inline"] = Token::INLINE_KEYWORD;
    keywordTypes["int"] = Token::INT_KEYWORD;
    keywordTypes["long"] = Token::LONG_KEYWORD;
    keywordTypes["register"] = Token::REGISTER_KEYWORD;
    keywordTypes["restrict"] = Token::RESTRICT_KEYWORD;
    keywordTypes["return"] = Token::RETURN_KEYWORD;
    keywordTypes["short"] = Token::SHORT_KEYWORD;
    keywordTypes["signed"] = Token::SIGNED_KEYWORD;
    keywordTypes["sizeof"] = Token::SIZEOF_KEYWORD;
    keywordTypes["static"] = Token::STATIC_KEYWORD;
    keywordTypes["struct"] = Token::STRUCT_KEYWORD;
    keywordTypes["switch"] = Token::SWITCH_KEYWORD;
    keywordTypes["typedef"] = Token::TYPEDEF_KEYWORD;
    keywordTypes["union"] = Token::UNION_KEYWORD;
    keywordTypes["unsigned"] = Token::UNSIGNED_KEYWORD;
    keywordTypes["void"] = Token::VOID_KEYWORD;
    keywordTypes["volatile"] = Token::VOLATILE_KEYWORD;
    keywordTypes["while"] = Token::WHILE_KEYWORD;
    keywordTypes["and"] = Token::AND_KEYWORD;
    keywordTypes["and_eq"] = Token::AND_EQ_KEYWORD;
    keywordTypes["asm"] = Token::ASM_KEYWORD;
    keywordTypes["bitand"] = Token::BITAND_KEYWORD;
    keywordTypes["bitor"] = Token::BITOR_KEYWORD;
    keywordTypes["bool"] = Token::BOOL_KEYWORD;
    keywordTypes["class"] = Token::CLASS_KEYWORD;
    keywordTypes["compl"] = Token::COMPL_KEYWORD;
    keywordTypes["const_cast"] = Token::CONST_CAST_KEYWORD;
    keywordTypes["#define"] = Token::HASH_DEFINE_KEYWORD;
    keywordTypes["delete"] = Token::DELETE_KEYWORD;
    keywordTypes["dynamic_cast"] = Token::DYNAMIC_CAST_KEYWORD;
    keywordTypes["explicit"] = Token::EXPLICIT_KEYWORD;
    keywordTypes["export"] = Token::EXPORT_KEYWORD;
    keywordTypes["false"] = Token::FALSE_KEYWORD;
    keywordTypes["friend"] = Token::FRIEND_KEYWORD;
    keywordTypes["mutable"] = Token::MUTABLE_KEYWORD;
    keywordTypes["namespace"] = Token::NAMESPACE_KEYWORD;
    keywordTypes["new"] = Token::NEW_KEYWORD;
    keywordTypes["not"] = Token::NOT_KEYWORD;
    keywordTypes["not_eq"] = Token::NOT_EQ_KEYWORD;
    keywordTypes["operator"] = Token::OPERATOR_KEYWORD;
    keywordTypes["or"] = Token::OR_KEYWORD;
    keywordTypes["or_eq"] = Token::OR_EQ_KEYWORD;
    keywordTypes["private"] = Token::PRIVATE_KEYWORD;
    keywordTypes["protected"] = Token::PROTECTED_KEYWORD;
    keywordTypes["public"] = Token::PUBLIC_KEYWORD;
    keywordTypes["reinterpret_cast"] = Token::REINTERPRET_CAST_KEYWORD;
    keywordTypes["static_cast"] = Token::STATIC_CAST_KEYWORD;
    keywordTypes["template"] = Token::TEMPLATE_KEYWORD;
    keywordTypes["this"] = Token::THIS_KEYWORD;
    keywordTypes["throw"] = Token::THROW_KEYWORD;
    keywordTypes["true"] = Token::TRUE_KEYWORD;
    keywordTypes["try"] = Token::TRY_KEYWORD;
    keywordTypes["typeid"] = Token::TYPEID_KEYWORD;
    keywordTypes["typename"] = Token::TYPENAME_KEYWORD;
    keywordTypes["using"] = Token::USING_KEYWORD;
    keywordTypes["virtual"] = Token::VIRTUAL_KEYWORD;
    keywordTypes["wchar_t"] = Token::WCHAR_T_KEYWORD;
    keywordTypes["xor"] = Token::XOR_KEYWORD;
    keywordTypes["xor_eq"] = Token::XOR_EQ_KEYWORD;
}

void Tokenizer::initPuncSet(){
    punctuators.insert('[');
    punctuators.insert(']');
    punctuators.insert('(');
    punctuators.insert(')');
    punctuators.insert('{');
    punctuators.insert('}');
    punctuators.insert('-');
    punctuators.insert('+');
    punctuators.insert('&');
    punctuators.insert('*');
    punctuators.insert('~');
    punctuators.insert('!');
    punctuators.insert('|');
    punctuators.insert('/');
    punctuators.insert('%');
    punctuators.insert('<');
    punctuators.insert('>');
    punctuators.insert('=');
    punctuators.insert('^');
    punctuators.insert('|');
    punctuators.insert('?');
    punctuators.insert(':');
    punctuators.insert(';');
    punctuators.insert(',');
}

void Tokenizer::initHexSet(){
    hexSet.insert('A');
    hexSet.insert('B');
    hexSet.insert('C');
    hexSet.insert('D');
    hexSet.insert('E');
    hexSet.insert('F');
    hexSet.insert('a');
    hexSet.insert('b');
    hexSet.insert('c');
    hexSet.insert('d');
    hexSet.insert('e');
    hexSet.insert('f');
}

void Tokenizer::memSetArrays() {
    memset(token, 0, sizeof(token));
    memset(potentialKw, 0, sizeof(token));
}


