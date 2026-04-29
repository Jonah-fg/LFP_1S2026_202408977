#ifndef NODOARBOL_H
#define NODOARBOL_H

#include <string>
#include <vector>
#include "Token.h"
using namespace std;

class NodoArbol {
public:
    string etiqueta;
    TipoToken tipoToken; 
    vector<NodoArbol*> hijos;

    NodoArbol(string etiq);
    NodoArbol(string etiq, TipoToken tipo);
    void agregarHijo(NodoArbol* hijo);
};

#endif

