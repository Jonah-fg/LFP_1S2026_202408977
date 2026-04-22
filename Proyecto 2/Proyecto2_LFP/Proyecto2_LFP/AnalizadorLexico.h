#ifndef ANALIZADORLEXICO_H
#define ANALIZADORLEXICO_H

#include <string>
#include <vector>
#include "Token.h"
#include "GestorErrores.h"
using namespace std;

class AnalizadorLexico {
private:
    string fuente;
    int pos;
    int linea;
    int columna;
    vector<Token> tokens;
    GestorErrores* gestorErrores;

    char caracterActual();
    void avanzar();
    bool esFin();
    void saltarEspacios();

    Token siguienteToken();

public:
    AnalizadorLexico(string entrada, GestorErrores* gestor);
    vector<Token> analizarTodo();
    vector<Token> obtenerTokens();
};
#endif