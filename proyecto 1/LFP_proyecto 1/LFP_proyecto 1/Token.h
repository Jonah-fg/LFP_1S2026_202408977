#ifndef TOKEN_H
#define TOKEN_H
#include <string>
using namespace std;

enum class TokenType {
    HOSPITAL,
    PACIENTES,
    MEDICOS,
    CITAS,
    DIAGNOSTICOS,

    PACIENTE,
    MEDICO,
    CITA,
    DIAGNOSTICO,

    CADENA,
    ENTERO,

    LLAVE_ABRE,
    LLAVE_CIERRA,
    CORCHETE_ABRE,
    CORCHETE_CIERRA,
    DOS_PUNTOS,
    COMA,
    PUNTO_COMA,

    END_OF_FILE,
    ERROR
};

class Token {

public:
    TokenType type;
    string lexema;
    int linea;
    int columna;

    Token(TokenType type, string lexema, int linea, int columna);

};

#endif