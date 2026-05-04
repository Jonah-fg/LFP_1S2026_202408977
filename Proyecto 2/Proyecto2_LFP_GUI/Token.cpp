#include "Token.h"
#include <sstream>
#include <map>
using namespace std;

Token::Token() {
    tipo = DESCONOCIDO;
    lexema="";
    linea =0;
    columna =0;
}

Token::Token(TipoToken t, string lex, int lin, int col) {
    tipo =t;
    lexema= lex;
    linea=lin;
    columna =col;
}

string Token::aCadena() {
    stringstream ss;
    ss << "Token(" << tipo << ", '" <<lexema << "', linea " << linea << ", col " << columna << ")";
    return ss.str();
}

string tipoTokenToString(TipoToken tipo) {
    map<TipoToken, string> conversion= {
        {TABLERO,      "TABLERO"},
        {COLUMNA,      "COLUMNA"},
        {TAREA,        "tarea"},
        {PRIORIDAD,    "prioridad"},
        {RESPONSABLE,  "responsable"},
        {FECHA_LIMITE, "fecha_limite"},
        {ALTA,         "ALTA"},
        {MEDIA,        "MEDIA"},
        {BAJA,         "BAJA"},
        {CADENA,       "CADENA"},
        {FECHA,        "FECHA"},
        {ENTERO,       "ENTERO"},
        {LLAVE_IZQ,    "LLAVE_IZQ"},
        {LLAVE_DER,    "LLAVE_DER"},
        {CORCHETE_IZQ, "CORCHETE_IZQ"},
        {CORCHETE_DER, "CORCHETE_DER"},
        {DOS_PUNTOS,   "DOS_PUNTOS"},
        {COMA,         "COMA"},
        {PUNTO_COMA,   "PUNTO_COMA"},
        {FIN_ARCHIVO,  "EOF"},
        {DESCONOCIDO,  "DESCONOCIDO"}
    };

    auto it =conversion.find(tipo);
    if (it !=conversion.end()) {
        return it->second;
    }
    return "DESCONOCIDO";
}