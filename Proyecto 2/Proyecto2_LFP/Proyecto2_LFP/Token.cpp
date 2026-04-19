#include "Token.h"
#include <sstream>
using namespace std;

Token::Token() {
    tipo = DESCONOCIDO;
    lexema = "";
    linea = 0;
    columna = 0;
}

Token::Token(TipoToken t, string lex, int lin, int col) {
    tipo = t;
    lexema = lex;
    linea = lin;
    columna = col;
}

string Token::aCadena() {
    stringstream ss;
    ss << "Token(" << tipo << ", '" << lexema << "', linea " << linea << ", col " << columna << ")";
    return ss.str();
}

