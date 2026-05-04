#pragma once
#ifndef TOKEN_H
#define TOKEN_H

#include <string>
using namespace std;

enum TipoToken {
    // Palabras reservadas
    TABLERO,
    COLUMNA,
    TAREA,
    PRIORIDAD,
    RESPONSABLE,
    FECHA_LIMITE,

    ALTA,
    MEDIA,
    BAJA,

    CADENA,
    FECHA,
    ENTERO,

    LLAVE_IZQ,
    LLAVE_DER,
    CORCHETE_IZQ,
    CORCHETE_DER,
    DOS_PUNTOS,
    COMA,
    PUNTO_COMA,

    FIN_ARCHIVO,
    DESCONOCIDO
};

string tipoTokenToString(TipoToken tipo);

class Token {
public:
    TipoToken tipo;
    string lexema;
    int linea;
    int columna;

    Token();

    Token(TipoToken t, string lex, int lin, int col);
    string aCadena();
};
#endif



