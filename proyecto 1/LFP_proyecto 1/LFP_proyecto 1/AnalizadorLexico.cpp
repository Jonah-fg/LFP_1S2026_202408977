#include "AnalizadorLexico.h"
#include <cctype>

AnalizadorLexico::AnalizadorLexico(std::string input) {

    this->input=input;
    this->posicion=0;
    this->linea=1;
    this->columna= 1;
}

Token AnalizadorLexico::nextToken() {

    while (posicion<input.length() && isspace(input[posicion])) {
        if (input[posicion]=='\n') {
            linea++;
            columna=1;
        }
        else{
            columna++;
        }
        posicion++;
    }

    if (posicion>=input.length()) {
        return Token(TokenType::END_OF_FILE, "", linea, columna);
    }

    char c=input[posicion];
    posicion++;

    if (isalpha(c)){
        string palabra="";
        palabra+=c;

        while (posicion<input.length() && isalpha(input[posicion])) {
            palabra+=input[posicion];
            posicion++;
        }

        if (palabra == "HOSPITAL")
            return Token(TokenType::HOSPITAL, palabra, linea, columna);

        if (palabra == "PACIENTES")
            return Token(TokenType::PACIENTES, palabra, linea, columna);

        if (palabra == "MEDICOS")
            return Token(TokenType::MEDICOS, palabra, linea, columna);

        if (palabra == "CITAS")
            return Token(TokenType::CITAS, palabra, linea, columna);

        if (palabra == "DIAGNOSTICOS")
            return Token(TokenType::DIAGNOSTICOS, palabra, linea, columna);
        return Token(TokenType::ERROR, palabra, linea, columna);

    }

    switch (c){

    case '{':
        return Token(TokenType::LLAVE_ABRE, "{", linea, columna);

    case '}':
        return Token(TokenType::LLAVE_CIERRA, "}", linea, columna);

    case '[':
        return Token(TokenType::CORCHETE_ABRE, "[", linea, columna);

    case ']':
        return Token(TokenType::CORCHETE_CIERRA, "]", linea, columna);

    case ':':
        return Token(TokenType::DOS_PUNTOS, ":", linea, columna);

    case ',':
        return Token(TokenType::COMA, ",", linea, columna);

    case ';':
        return Token(TokenType::PUNTO_COMA, ";", linea, columna);

    default:
        return Token(TokenType::ERROR, string(1, c), linea, columna);

    }

}