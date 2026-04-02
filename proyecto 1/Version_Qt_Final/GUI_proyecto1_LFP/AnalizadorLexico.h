#ifndef ANALIZADORLEXICO_H
#define ANALIZADORLEXICO_H

#include <string>
#include <vector>
#include <map>
#include "Token.h"
#include "ErrorLexico.h"

using namespace std;

enum class Estado {
    INICIO,
    LEYENDO_ID,
    LEYENDO_NUMERO,
    LEYENDO_CODIGO_ID,
    LEYENDO_FECHA,
    LEYENDO_HORA,
    LEYENDO_CADENA,
    EN_ERROR
};

class AnalizadorLexico {
private:
    string input;
    int posicion;
    int linea;
    int columna;
    int inicioLexema;
    int lineaInicioLexema;
    int columnaInicioLexema;

    static const map<string, TokenType> palabrasReservadas;
    static const map<string, TokenType> especialidadesValidas;
    static const map<string, TokenType> dosisValidas;

    char siguienteCaracter();
    void retroceder();
    void reiniciarParaSiguienteToken();
    void reportarError(string lexema, string descripcion, int lineaError, int columnaError);
    bool esLetra(char c);
    bool esDigito(char c);
    bool esLetraMayuscula(char c);
    bool esCaracterValido(char c);
    bool esEspacioBlanco(char c);

public:
    vector<ErrorLexico> errores;
    AnalizadorLexico(string input);
    Token nextToken();
    const vector<ErrorLexico>& getErrores() const { return errores; }
};

#endif
