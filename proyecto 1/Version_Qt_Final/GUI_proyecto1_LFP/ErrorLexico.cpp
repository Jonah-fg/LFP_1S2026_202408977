#include "ErrorLexico.h"

    ErrorLexico::ErrorLexico(string caracter, int linea, int columna) {
        this->caracter = caracter;
        this->linea = linea;
        this->columna = columna;
    }

