#include <iostream>
#include <fstream>
#include <sstream>
#include "AnalizadorLexico.h"
#include "AnalizadorSintactico.h"
#include "GestorErrores.h"
#include "NodoArbol.h"
using namespace std;

void imprimirTokens(vector<Token> tokens) {
    cout<< "\n--- Tokens ----\n";
    for (int i =0; i<tokens.size(); i++) {
        Token tok =tokens[i];
        cout<< i+1 << "\t"<<tok.lexema<< "\tTipo:" << tok.tipo << "\tL:" <<tok.linea << " C:" << tok.columna << "\n";
    }
}

void imprimirErrores(vector<EntradaError> errores) {
    if (errores.empty()){
        cout << "\n--- Sin errrores ----\n";
        return;
    }
    cout << "\n--- Tabla de erores----\n";
    for (int i=0; i <errores.size(); i++) {
        EntradaError error=errores[i];
        cout <<error.numero << "\t" << error.lexemaOToken << "\t" << error.tipo<< "\t" << error.descripcion << "\tL:" << error.linea << " C:" << error.columna << "\n";
    }
}

void imprimirArbol(NodoArbol* nodo, int nivel) {
    if (nodo ==NULL)
        return;

    for (int i =0; i< nivel; i++) {
        cout << "  ";
    }
    cout << nodo->etiqueta<< "\n";

    for (int i= 0; i< nodo->hijos.size(); i++) {
        imprimirArbol(nodo->hijos[i], nivel+1);
    }
}

int main() {
    ifstream archivo("prueba.task");
    if (!archivo.is_open()) {
        cout<< "No se pudo abrir el archivo prueba.task\n";
        return 1;
    }

    stringstream buffer;
    buffer <<archivo.rdbuf();
    string entrada =buffer.str();
    archivo.close();

    // Analisis lexico
    GestorErrores gestor;
    AnalizadorLexico lexico(entrada, &gestor);
    vector<Token> tokens =lexico.analizarTodo();

    imprimirTokens(tokens);
    imprimirErrores(gestor.obtenerTodos());

    // Analisis sintactico
    AnalizadorSintactico sintactico(tokens, &gestor);
    NodoArbol* raiz=sintactico.analizar();

    cout << "\n--- Arbol de derivaciion ---\n";
    imprimirArbol(raiz, 0);
    return 0;
}
