#include "GestorErrores.h"
using namespace std;

EntradaError::EntradaError() {
    numero = 0;
    lexemaOToken = "";
    tipo = "";
    descripcion = "";
    linea = 0;
    columna = 0;
}

EntradaError::EntradaError(int num, string lex, string tip, string desc, int lin, int col) {
    numero = num;
    lexemaOToken = lex;
    tipo = tip;
    descripcion = desc;
    linea = lin;
    columna = col;
}

void GestorErrores::agregarErrorLexico(string lexema, int linea, int columna, string desc) {
    int num = errores.size() + 1;
    EntradaError Eerror(num, lexema, "Lexico", desc, linea, columna);
    errores.push_back(Eerror);
}

void GestorErrores::agregarErrorSintactico(string lexema, int linea, int columna, string desc) {
    int num = errores.size() + 1;
    EntradaError Eerror(num, lexema, "Sintactico", desc, linea, columna);
    errores.push_back(Eerror);
}

vector<EntradaError> GestorErrores::obtenerTodos() {
    return errores;
}

bool GestorErrores::hayErrores() {
    return !errores.empty();
}

