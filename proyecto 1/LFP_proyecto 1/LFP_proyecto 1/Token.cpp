#include "Token.h"

Token::Token(TokenType type, std::string lexema, int linea, int columna) {
    this->type=type;
    this->lexema=lexema;
    this->linea=linea;
    this->columna=columna;
}