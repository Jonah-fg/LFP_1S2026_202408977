#include <iostream>
#include "AnalizadorLexico.h"

using namespace std;

int main()
{
    string texto = "HOSPITAL PACIENTE";
    AnalizadorLexico lexer(texto);
    Token token = lexer.nextToken();
    while (token.type!=TokenType::END_OF_FILE){

        cout << "Token encontrado: " << token.lexema << endl;
        token=lexer.nextToken();
    }
    cout << "Fin del analisis" << endl;
}
