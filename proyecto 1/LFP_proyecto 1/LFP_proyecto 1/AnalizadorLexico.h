#ifndef ANALIZADORLEXICO_H
#define ANALIZADORLEXICO_H
#include <string>
#include "Token.h"
using namespace std;

class AnalizadorLexico{

private:
    string input;
    int posicion;
    int linea;
    int columna;

public:
    AnalizadorLexico(string input);
    Token nextToken();
};
#endif