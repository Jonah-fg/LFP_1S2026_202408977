#ifndef ANALIZADORSINTACTICO_H
#define ANALIZADORSINTACTICO_H

#include <vector>
#include "Token.h"
#include "NodoArbol.h"
#include "GestorErrores.h"
using namespace std;

class AnalizadorSintactico {
private:
    vector<Token> tokens;
    int indiceActual;
    GestorErrores* gestorErrores;
    NodoArbol* raiz;

    Token tokenActual();
    void avanzar();
    bool coincidir(TipoToken esperado);
    void sincronizar();

    NodoArbol* analizarPrograma();
    NodoArbol* analizarListaColumnas();
    NodoArbol* analizarColumna();
    NodoArbol* analizarListaTareas();
    NodoArbol* analizarTarea();
    NodoArbol* analizarListaAtributos();
    NodoArbol* analizarAtributo();
    NodoArbol* analizarPrioridad();

public:
    AnalizadorSintactico(vector<Token> tokens, GestorErrores* gestor);
    NodoArbol* analizar();
    NodoArbol* obtenerRaiz();
};

#endif