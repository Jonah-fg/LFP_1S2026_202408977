#include <iostream>
#include "AnalizadorLexico.h"
#include "Token.h"
#include <vector>

using namespace std;
int main() {

    string texto = "HOSPITAL { PACIENTES [] }";

    AnalizadorLexico lexer(texto);

    vector<Token> tokens;

    Token t = lexer.nextToken();

    while (t.type != TokenType::END_OF_FILE) {

        tokens.push_back(t);
        t = lexer.nextToken();
    }

    cout << "TOKENS:\n";

    for (int i = 0; i < tokens.size(); i++) {

        cout << tokenTypeToString(tokens[i].type)
            << " -> "
            << tokens[i].lexema
            << endl;
    }

    cout << "\nERRORES:\n";

    for (int i = 0; i < lexer.errores.size(); i++) {

        cout << "Caracter: "
            << lexer.errores[i].caracter
            << " Linea: "
            << lexer.errores[i].linea
            << " Columna: "
            << lexer.errores[i].columna
            << endl;
    }
    return 0;
}