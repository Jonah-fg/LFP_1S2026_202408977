#ifndef GENERADORREPORTES_H
#define GENERADORREPORTES_H

#include <string>
#include <vector>
#include "Token.h"
#include "NodoArbol.h"
using namespace std;

class GeneradorReportes {
public:
    static void generarReporteKanban(vector<Token>& tokens, string nombreArchivo);
    static void generarReporteCarga(vector<Token>& tokens, string nombreArchivo);
    static void generarReporteTareasFechas(vector<Token>& tokens, string nombreArchivo);
    static void generarArbolDot(NodoArbol* raiz, string nombreArchivo);

    static void escribirNodoRecursivo(NodoArbol* nodo, int idPadre, int& contador, ofstream& archivo);
    static string escaparDot(string texto);
};

#endif
