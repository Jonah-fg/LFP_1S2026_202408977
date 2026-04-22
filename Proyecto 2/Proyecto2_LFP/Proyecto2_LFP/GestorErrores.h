#ifndef GESTORERRORES_H
#define GESTORERRORES_H

#include <vector>
#include <string>
using namespace std;

struct EntradaError {
    int numero;
    string lexemaOToken;
    string tipo;
    string descripcion;
    int linea;
    int columna;

    EntradaError();
    EntradaError(int num, string lex, string tip, string desc, int lin, int col);
};

class GestorErrores {
private:
    vector<EntradaError> errores;

public:
    void agregarErrorLexico(string lexema, int linea, int columna, string desc);
    void agregarErrorSintactico(string lexema, int linea, int columna, string desc);
    vector<EntradaError> obtenerTodos();
    bool hayErrores();
    void limpiar();
};

#endif