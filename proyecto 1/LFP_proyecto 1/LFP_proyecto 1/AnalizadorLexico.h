#ifndef ANALIZADORLEXICO_H
#define ANALIZADORLEXICO_H
#include <string>
#include <vector>
#include "Token.h"
#include "ErrorLexico.h"

enum class Estado {
    INICIO,
    LEYENDO_PALABRA,
    LEYENDO_NUMERO,
    LEYENDO_CODIGO_ID,
    LEYENDO_FECHA,
    LEYENDO_HORA,
    LEYENDO_CADENA,
    LEYENDO_HORA_SEGUNDA_PARTE,
    LEYENDO_FECHA_DESPUES_GUION,
    EN_ERROR
};

class AnalizadorLexico{

private:
    std::string input;
    int posicion;
    int linea;
    int columna;
	int inicioLexema;

    char siguienteCaracter();
    void retroceder();
    Token crearToken(TokenType tipo);
    void reportarError(std::string lexema, std::string descripcion, int lineaError, int columnaError);

public:
    vector<ErrorLexico> errores;
    AnalizadorLexico(std::string input);

    const std::vector<ErrorLexico>& getErrores() const { return errores; }

    Token nextToken();
};
#endif