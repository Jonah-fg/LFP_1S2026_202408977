#include "NodoArbol.h"
using namespace std;

NodoArbol::NodoArbol(string etiq) {
    etiqueta= etiq;
    tipoToken=DESCONOCIDO;
}

NodoArbol::NodoArbol(string etiq, TipoToken tipo) {
    etiqueta = etiq;
    tipoToken=tipo;
}

void NodoArbol::agregarHijo(NodoArbol* hijo) {
    hijos.push_back(hijo);
} 