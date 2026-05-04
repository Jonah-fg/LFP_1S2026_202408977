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
    while (tokenActual().tipo !=FIN_ARCHIVO && tokenActual().tipo != PUNTO_COMA && tokenActual().tipo != LLAVE_DER && tokenActual().tipo != COMA) {
        avanzar();
    }
}

// <programa> ::=TABLERO CADENA LLAVE_IZQ <lista_columnas> LLAVE_DER PUNTO_COMA
NodoArbol* AnalizadorSintactico::analizarPrograma() {
    NodoArbol* nodo=new NodoArbol("<programa>");

    if (!coincidir(TABLERO)) {
        gestorErrores->agregarErrorSintactico(tokenActual().lexema, tokenActual().linea, tokenActual().columna, "Se esperaba 'TABLERO'");
        return nodo;
    }
    nodo->agregarHijo(new NodoArbol("TABLERO", TABLERO));

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
    else{
        nodo->agregarHijo(new NodoArbol("{", LLAVE_IZQ));
    }

    NodoArbol* columnas= analizarListaColumnas();
    if (columnas != NULL) {
        nodo->agregarHijo(columnas);
    }

    if (!coincidir(LLAVE_DER)) {
        gestorErrores->agregarErrorSintactico(tokenActual().lexema, tokenActual().linea, tokenActual().columna, "Se esperaba '}'");
        sincronizar();
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

    if (!coincidir(COLUMNA)){
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

    if (!coincidir(CORCHETE_IZQ)) {
        gestorErrores->agregarErrorSintactico(tokenActual().lexema, tokenActual().linea, tokenActual().columna, "Se esperaba '['");
        sincronizar();
        return nodo;
    }
    nodo->agregarHijo(new NodoArbol("[", CORCHETE_IZQ));

    NodoArbol* atributos=analizarListaAtributos();
    if (atributos !=NULL) {
        nodo->agregarHijo(atributos);
    }

    if (!coincidir(CORCHETE_DER)) {
        gestorErrores->agregarErrorSintactico(tokenActual().lexema, tokenActual().linea, tokenActual().columna, "Se esperaba ']'");
    }
    else{
        nodo->agregarHijo(new NodoArbol("]", CORCHETE_DER));
    }
    return nodo;

}


// <lista_atributos> ::= <atributo> <mas_atributos>
NodoArbol* AnalizadorSintactico::analizarListaAtributos() {
    NodoArbol* nodo = new NodoArbol("<lista_atributos>");

    if (tokenActual().tipo != PRIORIDAD &&
        tokenActual().tipo != RESPONSABLE &&
        tokenActual().tipo != FECHA_LIMITE) {
        return nodo;
    }

    do {
        if (tokenActual().tipo == COMA) {
            nodo->agregarHijo(new NodoArbol(",", COMA));
            avanzar();
        }

        if (tokenActual().tipo == PRIORIDAD ||
            tokenActual().tipo == RESPONSABLE ||
            tokenActual().tipo == FECHA_LIMITE) {
            NodoArbol* attr = analizarAtributo();
            if (attr != NULL) {
                nodo->agregarHijo(attr);
            }
        } else if (tokenActual().tipo == CORCHETE_DER || tokenActual().tipo == FIN_ARCHIVO) {
            break;
        } else {
            // Token desconocido
            gestorErrores->agregarErrorSintactico(tokenActual().lexema,
                                                  tokenActual().linea,
                                                  tokenActual().columna,
                                                  "Se esperaba 'prioridad', 'responsable' o 'fecha_limite'");
            while (tokenActual().tipo != COMA &&
                   tokenActual().tipo != CORCHETE_DER &&
                   tokenActual().tipo != FIN_ARCHIVO) {
                avanzar();
            }
        }
    } while (tokenActual().tipo == COMA ||
             tokenActual().tipo == PRIORIDAD ||
             tokenActual().tipo == RESPONSABLE ||
             tokenActual().tipo == FECHA_LIMITE);

    return nodo;
}


// <atributo> ::= PRIORIDAD DOS_PUNTOS <prioridad> | RESPONSABLE DOS_PUNTOS CADENA | FECHA_LIMITE DOS_PUNTOS FECHA
NodoArbol* AnalizadorSintactico::analizarAtributo() {
    NodoArbol* nodo=new NodoArbol("<atributo>");
    TipoToken tipoAtributo = tokenActual().tipo;

    if (tipoAtributo ==PRIORIDAD) {
        nodo->agregarHijo(new NodoArbol("prioridad", PRIORIDAD));
        avanzar();

        if (!coincidir(DOS_PUNTOS)) {
            gestorErrores->agregarErrorSintactico(tokenActual().lexema, tokenActual().linea, tokenActual().columna, "Se esperaba ':'");
            if (tokenActual().tipo==ALTA || tokenActual().tipo == MEDIA || tokenActual().tipo == BAJA) {
                avanzar();
            }
        }
        else {
            nodo->agregarHijo(new NodoArbol(":", DOS_PUNTOS));

            NodoArbol* prio=analizarPrioridad();
            if (prio != NULL) {
                nodo->agregarHijo(prio);
            }
        }
    }
    else if (tipoAtributo==RESPONSABLE) {
        nodo->agregarHijo(new NodoArbol("responsable", RESPONSABLE));
        avanzar();

        if (!coincidir(DOS_PUNTOS)) {
            gestorErrores->agregarErrorSintactico(tokenActual().lexema, tokenActual().linea, tokenActual().columna, "Se esperaba ':'");
            if (tokenActual().tipo ==CADENA) {
                avanzar();
            }
        }
        else {
            nodo->agregarHijo(new NodoArbol(":", DOS_PUNTOS));
            if (tokenActual().tipo !=CADENA) {
                gestorErrores->agregarErrorSintactico(tokenActual().lexema, tokenActual().linea, tokenActual().columna, "Se esperaba CADENA");
            }
            else{
                nodo->agregarHijo(new NodoArbol(tokenActual().lexema, CADENA));
                avanzar();
            }
        }
    }
    else if (tipoAtributo==FECHA_LIMITE) {
        nodo->agregarHijo(new NodoArbol("fecha_limite", FECHA_LIMITE));
        avanzar();

        if (!coincidir(DOS_PUNTOS)) {
            gestorErrores->agregarErrorSintactico(tokenActual().lexema, tokenActual().linea, tokenActual().columna, "Se esperaba ':'");
            if (tokenActual().tipo==FECHA) {
                avanzar();
            }
        }
        else {
            nodo->agregarHijo(new NodoArbol(":", DOS_PUNTOS));
            // Solo verificamos el valor si sí había ":"
            if (tokenActual().tipo!=FECHA) {
                gestorErrores->agregarErrorSintactico(tokenActual().lexema, tokenActual().linea, tokenActual().columna, "Se esperaba FECHA (AAAA-MM-DD)");
            }
            else{
                nodo->agregarHijo(new NodoArbol(tokenActual().lexema, FECHA));
                avanzar();
            }
        }
    }
    else {
        gestorErrores->agregarErrorSintactico(tokenActual().lexema, tokenActual().linea, tokenActual().columna, "Se esperaba 'prioridad', 'responsble' o 'fecha_limite'");
        avanzar();
    }
    return nodo;
}


// <prioridad> ::=ALTA | MEDIA | BAJA
NodoArbol* AnalizadorSintactico::analizarPrioridad() {
    NodoArbol* nodo = new NodoArbol("<prioridad>");
    TipoToken tipo =tokenActual().tipo;

    if (tipo ==ALTA || tipo==MEDIA || tipo== BAJA) {
        nodo->agregarHijo(new NodoArbol(tokenActual().lexema, tipo));
        avanzar();
    }
    else {
        gestorErrores->agregarErrorSintactico(tokenActual().lexema, tokenActual().linea, tokenActual().columna, "Se esperaba ALTA, MEDIA o BAJA");
    }
    return nodo;
}

NodoArbol* AnalizadorSintactico::analizar() {
    raiz= analizarPrograma();
    return raiz;
}

NodoArbol* AnalizadorSintactico::obtenerRaiz() {
    return raiz;
}



