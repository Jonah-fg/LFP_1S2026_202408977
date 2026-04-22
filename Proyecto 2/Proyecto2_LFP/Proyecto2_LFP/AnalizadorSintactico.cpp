#include "AnalizadorSintactico.h"
#include <iostream>
using namespace std;

AnalizadorSintactico::AnalizadorSintactico(vector<Token> tokens, GestorErrores* gestor) {
    this->tokens =tokens;
    indiceActual=0;
    gestorErrores =gestor;
    raiz=NULL;
}

Token AnalizadorSintactico::tokenActual() {
    if (indiceActual <tokens.size()) {
        return tokens[indiceActual];
    }
    return Token(FIN_ARCHIVO, "EOF", -1, -1);
}

void AnalizadorSintactico::avanzar() 
{
    if (indiceActual<tokens.size()) {
        indiceActual++;
    }
}

bool AnalizadorSintactico::coincidir(TipoToken esperado) {
    if (tokenActual().tipo== esperado) {
        avanzar();
        return true;
    }
    return false;
}

void AnalizadorSintactico::sincronizar() {
    while (tokenActual().tipo != FIN_ARCHIVO && tokenActual().tipo != PUNTO_COMA && tokenActual().tipo != LLAVE_DER && tokenActual().tipo != COMA) {
        avanzar();
    }
}

// <programa> ::=TABLERO CADENA LLAVE_IZQ <lista_columnas> LLAVE_DER PUNTO_COMA
NodoArbol* AnalizadorSintactico::analizarPrograma() {
    NodoArbol* nodo=new NodoArbol("<programa>");

    if (!coincidir(TABLERO)) {
        gestorErrores->agregarErrorSintactico(tokenActual().lexema, tokenActual().linea, tokenActual().columna, "Se esperaba 'TABLERO'");
        sincronizar();
    }
    else{
        nodo->agregarHijo(new NodoArbol("TABLERO", TABLERO));
    }

    if (tokenActual().tipo !=CADENA) {
        gestorErrores->agregarErrorSintactico(tokenActual().lexema, tokenActual().linea, tokenActual().columna, "Se esperaba una CADENA");
        sincronizar();
    }
    else {
        nodo->agregarHijo(new NodoArbol(tokenActual().lexema, CADENA));
        avanzar();
    }

    if (!coincidir(LLAVE_IZQ)) {
        gestorErrores->agregarErrorSintactico(tokenActual().lexema, tokenActual().linea, tokenActual().columna, "Se esperaba '{'");
        sincronizar();
    }
    else {
        nodo->agregarHijo(new NodoArbol("{", LLAVE_IZQ));
    }

    NodoArbol* columnas= analizarListaColumnas();
    if (columnas != NULL) {
        nodo->agregarHijo(columnas);
    }

    if (!coincidir(LLAVE_DER)) {
        gestorErrores->agregarErrorSintactico(tokenActual().lexema, tokenActual().linea, tokenActual().columna, "Se esperaba '}'");
    }
    else {
        nodo->agregarHijo(new NodoArbol("}", LLAVE_DER));
    }

    if (!coincidir(PUNTO_COMA)) {
        gestorErrores->agregarErrorSintactico(tokenActual().lexema, tokenActual().linea, tokenActual().columna, "Se esperaba ';'");
    }
    else {
        nodo->agregarHijo(new NodoArbol(";", PUNTO_COMA));
    }
    return nodo;
}

// <lista_columnas> ::=<columna> <mas_columnas>
NodoArbol* AnalizadorSintactico::analizarListaColumnas() {
    NodoArbol* nodo =new NodoArbol("<lista_columnas>");

    while (tokenActual().tipo == COLUMNA) {
        NodoArbol* col= analizarColumna();
        if (col !=NULL) {
            nodo->agregarHijo(col);
        }
    }
    return nodo;
}

// <columna>::=COLUMNA CADENA LLAVE_IZQ <lista_tareas> LLAVE_DER PUNTO_COMA
NodoArbol* AnalizadorSintactico::analizarColumna() {
    NodoArbol* nodo =new NodoArbol("<columna>");

    if (!coincidir(COLUMNA)) {
        gestorErrores->agregarErrorSintactico(tokenActual().lexema, tokenActual().linea, tokenActual().columna, "Se esperaba 'COLUMNA'");
        sincronizar();
        return nodo;
    }
    nodo->agregarHijo(new NodoArbol("COLUMNA", COLUMNA));

    if (tokenActual().tipo!= CADENA) {
        gestorErrores->agregarErrorSintactico(tokenActual().lexema, tokenActual().linea, tokenActual().columna, "Se esperaba CAENA");
        sincronizar();
    }
    else {
        nodo->agregarHijo(new NodoArbol(tokenActual().lexema, CADENA));
        avanzar();
    }

    if (!coincidir(LLAVE_IZQ)) {
        gestorErrores->agregarErrorSintactico(tokenActual().lexema, tokenActual().linea, tokenActual().columna, "Se esperaba '{'");
        sincronizar();
    }
    else {
        nodo->agregarHijo(new NodoArbol("{", LLAVE_IZQ));
    }

    NodoArbol* tareas=analizarListaTareas();
    if (tareas !=NULL) {
        nodo->agregarHijo(tareas);
    }

    if (!coincidir(LLAVE_DER)) {
        gestorErrores->agregarErrorSintactico(tokenActual().lexema, tokenActual().linea, tokenActual().columna, "Se esperaba '}'");
    }
    else {
        nodo->agregarHijo(new NodoArbol("}", LLAVE_DER));
    }

    if (!coincidir(PUNTO_COMA)) {
        gestorErrores->agregarErrorSintactico(tokenActual().lexema, tokenActual().linea, tokenActual().columna, "Se esperaba ';'");
    }
    else{
        nodo->agregarHijo(new NodoArbol(";", PUNTO_COMA));
    }
    return nodo;
}


// <lista_tareas> ::=<tarea> <mas_tareas>
NodoArbol* AnalizadorSintactico::analizarListaTareas() {
    NodoArbol* nodo=new NodoArbol("<lista_tareas>");

    if (tokenActual().tipo!= TAREA) {
        return nodo;
    }

    do {
        if (tokenActual().tipo== COMA) {
            nodo->agregarHijo(new NodoArbol(",", COMA));
            avanzar();
        }
        NodoArbol* tarea =analizarTarea();
        if (tarea != NULL) {
            nodo->agregarHijo(tarea);
        }
    } 
    while (tokenActual().tipo ==COMA || tokenActual().tipo==TAREA);
    return nodo;
}


// <tarea> ::=TAREA DOS_PUNTOS CADENA LLAVE_IZQ <lista_atributos>LLAVE_DER
NodoArbol* AnalizadorSintactico::analizarTarea(){
    NodoArbol* nodo =new NodoArbol("<tarea>");

    if (!coincidir(TAREA)) {
        gestorErrores->agregarErrorSintactico(tokenActual().lexema, tokenActual().linea, tokenActual().columna, "Se esperaba 'tarea'");
        sincronizar();
        return nodo;
    }
    nodo->agregarHijo(new NodoArbol("tarea", TAREA));

    if (!coincidir(DOS_PUNTOS)) {
        gestorErrores->agregarErrorSintactico(tokenActual().lexema, tokenActual().linea, tokenActual().columna, "Se esperaba ':'");
    }
    else {
        nodo->agregarHijo(new NodoArbol(":", DOS_PUNTOS));
    }

    if (tokenActual().tipo != CADENA) {
        gestorErrores->agregarErrorSintactico(tokenActual().lexema, tokenActual().linea, tokenActual().columna, "Se esperaba CADENA");
    }
    else {
        nodo->agregarHijo(new NodoArbol(tokenActual().lexema, CADENA));
        avanzar();
    }

    if (!coincidir(LLAVE_IZQ)) {
        gestorErrores->agregarErrorSintactico(tokenActual().lexema, tokenActual().linea, tokenActual().columna, "Se esperaba '{'");
        sincronizar();
        return nodo;
    }
    nodo->agregarHijo(new NodoArbol("{", LLAVE_IZQ));

    NodoArbol* atributos=analizarListaAtributos();
    if (atributos !=NULL) {
        nodo->agregarHijo(atributos);
    }

    if (!coincidir(LLAVE_DER)) {
        gestorErrores->agregarErrorSintactico(tokenActual().lexema, tokenActual().linea,
            tokenActual().columna, "Se esperba '}'");
    }
    else {
        nodo->agregarHijo(new NodoArbol("}", LLAVE_DER));
    }
    return nodo;
}


