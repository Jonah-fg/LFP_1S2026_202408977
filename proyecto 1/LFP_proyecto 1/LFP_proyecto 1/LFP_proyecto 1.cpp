#include <iostream>
#include "AnalizadorLexico.h"
#include "Token.h"
#include <vector>

using namespace std;
int main() {

    string texto=R"(
    HOSPITAL {
    @
        PACIENTES [
            "Maria Lopez"
        ]
    }
    )";

    AnalizadorLexico lexer(texto);
    vector<Token> tokens;
    Token token=lexer.nextToken();

    while (token.type != TokenType::END_OF_FILE){

        tokens.push_back(token);
        token =lexer.nextToken();
    }
    
    cout << "TOKENS ENCONTRADOS:\n\n";
    for (int i = 0; i < tokens.size(); i++) {
        cout << "Tipo: "<< tokenTypeToString(tokens[i].type)<< "      Lexema: "<< tokens[i].lexema<< "      Linea: "<< tokens[i].linea<< endl;
    }

    cout<<"\nERRORES LEXICOS:\n\n";
    for (int i=0; i<lexer.errores.size(); i++) {

        cout << "Caracter: "<<lexer.errores[i].caracter<< "     Linea: "<< lexer.errores[i].linea<< "     Columna: "<< lexer.errores[i].columna<< endl;
    }
    cout << "Fin del analisis"<< endl;
    return 0;
}